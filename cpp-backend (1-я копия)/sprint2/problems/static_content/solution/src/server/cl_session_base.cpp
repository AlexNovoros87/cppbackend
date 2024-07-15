#include "cl_session_base.h"


namespace http_server
{
  SessionBase::SessionBase(tcp::socket &&socket)
            : stream_(std::move(socket))
        {
        }


  void SessionBase::Run()
        {
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

   void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read)
        {
            using namespace std::literals;
            if (ec == http::error::end_of_stream)
            {
                // Нормальная ситуация - клиент закрыл соединение
                return Close();
            }
            if (ec)
            {
                return ReportError(ec, "read"sv);
            }
            HandleRequest(std::move(request_));
        }

        void SessionBase::Close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        }

        void SessionBase::Read()
        {
            using namespace std::literals;
            // Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
            request_ = {};
            stream_.expires_after(30s);
            // Считываем request_ из stream_, используя buffer_ для хранения считанных данных
            http::async_read(stream_, buffer_, request_,
                             // По окончании операции будет вызван метод OnRead
                             beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
        }

        void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written)
        {
            if (ec)
            {
                return ReportError(ec, "write"sv);
            }

            if (close)
            {
                // Семантика ответа требует закрыть соединение
                return Close();
            }

            // Считываем следующий запрос
            Read();
        }













}