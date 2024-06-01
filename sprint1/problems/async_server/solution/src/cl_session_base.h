
#pragma once
#include"includes.h"

namespace http_server {

/*
Класс SessionBase не предназначен для полиморфного удаления. 
Поэтому его деструктор объявлен защищённым и невиртуальным, 
чтобы уменьшить накладные расходы.

*/


class SessionBase {
protected:
    using HttpRequest = http::request<http::string_body>;
   explicit SessionBase(tcp::socket&& socket)
        : stream_(std::move(socket)) {
    }
    ~SessionBase() = default;

private:
    //ПОТОК tcp_stream содержит внутри себя сокет и добавляет поддержку таймаутов
    beast::tcp_stream stream_;
    //БУФЕР
    beast::flat_buffer buffer_;
    //ТЕКУЩИЙ ЗАПРОС
    HttpRequest request_;
    //ПОЛУЧЕНИЕ shared_from_this()
    virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;

public:
 // Запрещаем копирование и присваивание объектов SessionBase и его наследников
 SessionBase(const SessionBase&) = delete;
 SessionBase& operator=(const SessionBase&) = delete;

 void Run(){
     // Вызываем метод Read, используя executor объекта stream_.
    // Таким образом вся работа со stream_ будет выполняться, используя его executor
    net::dispatch(stream_.get_executor(),
                  beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));

   
   
   
   /*
   Среди перегрузок функции net::dispatch есть и та, которая принимает 
   только объект-обработчик. В этом случае функция выполнит вызов, 
   используя системный executor, который может выполнить вызов либо 
   синхронно, либо асинхронно. Параллельно с методом Read другие методы 
   класса SessionBase не вызываются. Поэтому вызов будет безопасным, 
   хоть и произойдёт вне strand, связанного с сокетом.
   */
   
   };



private:
 // Обработку запроса делегируем подклассу
 virtual void HandleRequest(HttpRequest&& request) = 0;
 
 /*
В OnRead в возможны три ситуации:
Если клиент закрыл соединение, то сервер должен завершить сеанс.
Если произошла ошибка чтения, выведите её в stdout.
Если запрос прочитан без ошибок, делегируйте его обработку классу-наследнику.
 */
  void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
        using namespace std::literals;
        if (ec == http::error::end_of_stream) {
            // Нормальная ситуация - клиент закрыл соединение
            return Close();
        }
        if (ec) {
            return ReportError(ec, "read"sv);
        }
        HandleRequest(std::move(request_));
    }

    void Close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }
 
 
 void Read() {
        using namespace std::literals;
        // Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
        request_ = {};
        stream_.expires_after(30s);
        // Считываем request_ из stream_, используя buffer_ для хранения считанных данных
        http::async_read(stream_, buffer_, request_,
                         // По окончании операции будет вызван метод OnRead
                         beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
    }


void OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
        if (ec) {
            return ReportError(ec, "write"sv);
        }

        if (close) {
            // Семантика ответа требует закрыть соединение
            return Close();
        }

        // Считываем следующий запрос
        Read();
    }


protected:
template <typename Body, typename Fields>
    void Write(http::response<Body, Fields>&& response) {
        // Запись выполняется асинхронно, поэтому response перемещаем в область кучи
        auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));

        auto self = GetSharedThis();
        http::async_write(stream_, *safe_response,
                          [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                              self->OnWrite(safe_response->need_eof(), ec, bytes_written);
                          });
                          }    


};


}

