
#include "http_server.h"
// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive,
                                  std::string_view content_type = ContentType::TEXT_HTML) {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
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

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

int main() {
    const unsigned num_threads = std::thread::hardware_concurrency();

    net::io_context ioc(num_threads);

    // Подписываемся на сигналы и при их получении завершаем работу сервера
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
        if (!ec) {
            ioc.stop();
        }
    });

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;
    // http_server::ServeHttp(ioc, {address, port}, [](auto&& req, auto&& sender) {
    //      sender(HandleRequest(std::forward<decltype(req)>(req)));
    // });

    http_server::ServeHttp(ioc, {address, port}, HandleRequest);
    
    // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
    std::cout << "Server has started..."sv << std::endl;

    RunWorkers(num_threads, [&ioc] {
        ioc.run();
    });
}
