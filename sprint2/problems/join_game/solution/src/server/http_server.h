#pragma once
#include <string>
#include <vector>
#include <thread>
#include <variant>

#include "cl_listener.h"
struct ContentType
{
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APP_JSON = "application/json"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

namespace http_server
{
    template <typename RequestHandler>
    void ServeHttp(net::io_context &ioc, const tcp::endpoint &endpoint, RequestHandler &&handler)
    {
        using MyListener = Listener<std::decay_t<RequestHandler>>;
        std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run();
    }
}


