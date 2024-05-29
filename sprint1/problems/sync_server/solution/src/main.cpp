#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>; 


std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req; //<- сообщение
    // Считываем из socket запрос req, используя buffer для хранения данных.
    // В ec функция запишет код ошибки.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }
    if (ec) {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }
    return req;
} 


void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    // Выводим заголовки запроса
    for (const auto& header : req) {
        std::cout << "  "sv << header.name_string() << ": "sv << header.value() << std::endl;
    }
} 

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};


StringResponse MakeStringResponse(const StringRequest& req) {
    
    bool allowed_response = (req.method() == http::verb::get || req.method() == http::verb::head);
    StringResponse response((allowed_response) ?  http::status::ok : http::status::method_not_allowed, req.version());
    response.keep_alive(req.keep_alive()); 
    response.set(http::field::content_type, ContentType::TEXT_HTML);
    if(!allowed_response){ response.set(http::field::allow, "GET, HEAD");}
    
    if(allowed_response){
        response.body() = (req.method() == http::verb::get) ? "Hello, "s.append(req.target().substr(1)) : ""; 
    }
    else
    {
      response.body() = "Invalid method"s ;
    };
    response.content_length(response.body().size());
    return response;
}


StringResponse HandleRequest(StringRequest&& req) {
   auto request = req;
   return MakeStringResponse(request); 
    
} 


template <typename RequestHandler>
void HandleConnection(tcp::socket& socket, RequestHandler&& handle_request) {
    try {
        // Буфер для чтения данных в рамках текущей сессии.
        beast::flat_buffer buffer;

        // Продолжаем обработку запросов, пока клиент их отправляет
        while (auto request = ReadRequest(socket, buffer)) {
            DumpRequest(*request);
            StringResponse response = handle_request(*std::move(request));  
            http::write(socket, response);
            if (response.need_eof()) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    // Запрещаем дальнейшую отправку данных через сокет
    socket.shutdown(tcp::socket::shutdown_send, ec);
}


int main() {
    net::io_context ioc;

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    tcp::acceptor acceptor(ioc, {address, port});
    std::cout<<"Server has started..."<<std::endl;
    while (true) {
    tcp::socket socket(ioc);
        acceptor.accept(socket);

        // Запускаем обработку взаимодействия с клиентом в отдельном потоке
        //template< class Function, class... Args >
        //explicit thread( Function&& f, Args&&... args );
        
        std::thread t(
            // Лямбда-функция будет выполняться в отдельном потоке
            [](tcp::socket socket) {
                HandleConnection(socket, HandleRequest);
            },
            std::move(socket));  // Сокет нельзя скопировать, но можно переместить

        //Объект класса thread можно безопасно разрушить, когда с ним не связан 
        //поток выполнения. Для этого нужно вызвать метод thread::detach, который 
        //отвязывает объект thread от потока выполнения и поток будет продолжать 
        //выполняться сам по себе.
        
        t.detach();
    
    }
} 
