#include <boost/asio/signal_set.hpp>
#include "json/json_loader.h"
#include "requester/request_handler.h"
#include "req_helper/model_help.h"
#include "parse_ticker/comparser.h"
#include "parse_ticker/ticker.h"

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned threads, const Fn &fn)
{
    threads = std::max(1u, threads);
    std::vector<std::jthread> workers;
    workers.reserve(threads - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--threads)
    {
        workers.emplace_back(fn);
    }
    fn();
}

void Add_Console_Log()
{
    /*
   Если мы хотим логировать и в консоль, и в файл, либо хотим изменить
   параметры логирования, то на помощь придёт функция
   logging::add_console_log из файла <boost/log/utility/setup/console.hpp>.
   Ей нужно указать поток вывода. Логично использовать поток std::clog,
   который специально предназначен для логирования:
   Но фактически std::clog посылает все данные в std::cerr
   */

    logger::logging::add_console_log(
        std::clog,
        logger::keywords::auto_flush = true);
}

int main(int argc, const char *argv[])
{

    
    
    
    
    
    
    try
    {
        auto params = ParseCommandLine(argc, argv);

        if (params)
        {
            Add_Console_Log();
            using namespace model;

            bool auto_ticker = params->period != std::numeric_limits<uint64_t>::max() ;
            api::Play game = json_loader::LoadPlay(params->config_file, params->random_respawn, auto_ticker);

            // 2. Инициализируем io_context
            const unsigned num_threads = std::thread::hardware_concurrency();
            net::io_context ioc(num_threads);

            // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
            // Подписываемся на сигналы и при их получении завершаем работу сервера
            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&ioc](const sys::error_code &ec, [[maybe_unused]] int signal_number)
                               {
     if (!ec) {
         ioc.stop();
#ifdef CONSOLE_LOGGING
        logger::LogStop(ec.value());
#endif
    } });

            const auto adress = net::ip::make_address("0.0.0.0");
            constexpr net::ip::port_type port = 8080;
            const tcp::endpoint endpoint{adress, port};
            // strand для выполнения запросов к API
            auto api_strand = net::make_strand(ioc);

            // TICKER
            if (auto_ticker)
            {
                
                std::chrono::milliseconds delta = std::chrono::milliseconds(params->period);
                auto ticker = std::make_shared<Ticker>(api_strand, 50ms,
                                                       [&game](std::chrono::milliseconds delta)
                                                       {  game.ManualTick(delta); });
                ticker->Start();
            }

            // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры, передаем отдельный странд для запросов к API
            auto handler = std::make_shared<request_handler::RequestHandler>(game, std::filesystem::path(params->www_root), api_strand, endpoint);

            // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
            http_server::ServeHttp(ioc, endpoint, [handler](auto &&req, auto &&send)
                                   { handler->operator()(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send)); });

            // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
            // std::cout << "Server has started..."sv << std::endl;

#ifdef CONSOLE_LOGGING
            logger::LogRun(port, adress.to_string());
#endif

            //  6. Запускаем обработку асинхронных операций
            RunWorkers(std::max(1u, num_threads), [&ioc]
                       { ioc.run(); });
        }
    }
    catch (const std::exception &ex)
    {

#ifdef CONSOLE_LOGGING
        logger::LogStop(ex, EXIT_FAILURE);
#endif

        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    };
};
