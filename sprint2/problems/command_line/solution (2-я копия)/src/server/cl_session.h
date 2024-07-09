#pragma once
#include "cl_session_base.h"

/*
Этот класс будет отвечать за сеанс асинхронного обмена
данными с клиентом. Экземпляры этого класса будут
размещаться в куче, чтобы не разрушиться раньше времени.

Для управления временем жизни экземпляров Session будет
использован приём с захватом указателя shared_ptr
на текущий объект.
*/

namespace http_server
{
    template <typename RequestHandler>
    class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>>
    {
    public:
        template <typename Handler>
        Session(tcp::socket &&socket, Handler &&request_handler)
            : SessionBase(std::move(socket)), request_handler_(std::forward<Handler>(request_handler))
        {
        }

    private:
        RequestHandler request_handler_;

        void HandleRequest(HttpRequest &&request) override
        {
               
#ifdef CONSOLE_LOGGING
            logger::LogRequest(logger::RequestParams(request, ip_));
#endif

            logger::TimePoint begin = logger::GetNow();
            request_handler_(std::move(request), [&, self = this->shared_from_this()](auto &&response)
                             {
                                                           
                                 logger::ResponseParams params(response);

                                 if (std::holds_alternative<http::response<http::string_body>>(response))
                                     self->Write(std::move(std::get<0>(response)));

                                 else if (std::holds_alternative<http::response<http::file_body>>(response))
                                     self->Write(std::move(std::get<1>(response)));

#ifdef CONSOLE_LOGGING
                                 logger::LogResponse(params, std::move(begin));
#endif
                             });

        }

        std::shared_ptr<SessionBase> GetSharedThis() override
        {
            return this->shared_from_this();
        }
    };

} // namespace http_server