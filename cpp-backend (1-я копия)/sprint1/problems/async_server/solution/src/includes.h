
#pragma once
#include "sdk.h"

#define BOOST_BEAST_USE_STD_STRING_VIEW


//ASIO
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include<boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
//BEAST
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http_server {
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
void ReportError(beast::error_code ec, std::string_view what);

}


//MAIN
#include "sdk.h"
//

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>



namespace {
namespace net = boost::asio;
using namespace std::literals;
namespace sys = boost::system;
namespace http = boost::beast::http;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};


}
