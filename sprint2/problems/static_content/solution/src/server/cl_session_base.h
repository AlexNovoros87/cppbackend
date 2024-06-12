
#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <string_view>
#include <iostream>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
using tcp = net::ip::tcp;

namespace http_server
{
    void ReportError(beast::error_code ec, std::string_view what);
    /*
    Класс SessionBase не предназначен для полиморфного удаления.
    Поэтому его деструктор объявлен защищённым и невиртуальным,
    чтобы уменьшить накладные расходы.
    */

    class SessionBase
    {
    protected:
        using HttpRequest = http::request<http::string_body>;
        explicit SessionBase(tcp::socket &&socket);
        ~SessionBase() = default;

    private:
        // ПОТОК tcp_stream содержит внутри себя сокет и добавляет поддержку таймаутов
        beast::tcp_stream stream_;
        // БУФЕР
        beast::flat_buffer buffer_;
        // ТЕКУЩИЙ ЗАПРОС
        HttpRequest request_;
        // ПОЛУЧЕНИЕ shared_from_this()
        virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;

    public:
        // Запрещаем копирование и присваивание объектов SessionBase и его наследников
        SessionBase(const SessionBase &) = delete;
        SessionBase &operator=(const SessionBase &) = delete;
        void Run();

    private:
        // Обработку запроса делегируем подклассу
        virtual void HandleRequest(HttpRequest &&request) = 0;

        /*
       В OnRead в возможны три ситуации:
       Если клиент закрыл соединение, то сервер должен завершить сеанс.
       Если произошла ошибка чтения, выведите её в stdout.
       Если запрос прочитан без ошибок, делегируйте его обработку классу-наследнику.
        */
        void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read);
        void Close();
        void Read();
        void OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written);

    protected:
        template <typename Body, typename Fields>
        void Write(http::response<Body, Fields> &&response);
    };

    template <typename Body, typename Fields>
    void SessionBase::Write(http::response<Body, Fields> &&response)
    {
        // Запись выполняется асинхронно, поэтому response перемещаем в область кучи
        auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));

        auto self = GetSharedThis();
        http::async_write(stream_, *safe_response,
                          [safe_response, self](beast::error_code ec, std::size_t bytes_written)
                          {
                              self->OnWrite(safe_response->need_eof(), ec, bytes_written);
                          });
    }

}
