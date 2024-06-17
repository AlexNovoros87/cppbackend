#include <boost/asio/signal_set.hpp>
// #include "json_loader.h"
#include "request_handler.h"
// #include "logger/logger.h"

/*
Уважаемый код-ревьюер!!!
server - как независимая единиа встроенная в этот проект, вынесена в отдельную папку
все классы в ней по отдельным файлам.. из прошлых уроков внесение изменений в 1 файл не
вызывает перекомпиляцию всего проекта и гораздо удобней ориентироваться.

в классах model внес поправки в конструктор и внес методы json-представления объекта
так как на стадии конструирования это делать логичней так как это снизит время конструирования
джсон-ответа при обращении к существующей карте
*/

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

    logging::add_console_log(
        std::clog,
        keywords::auto_flush = true);
}

int main(int argc, const char *argv[])
{
    StringResponse q;
    q.version(11);

    Add_Console_Log();
    // json::value custom_data = LogRun(8080, "0.0.0.0");

    // std::cout<<json::serialize(custom_data)<<std::endl;

    // BOOST_LOG_TRIVIAL(info) << json::serialize(custom_data)<<std::endl;

    if (argc != 3)
    {
        std::cerr << "Usage: game_server <game-config-json>, and path to static catalog!!"sv << std::endl;
        #ifdef CONSOLE_LOGGING
        LogStop(EXIT_FAILURE);
        #endif
        return EXIT_FAILURE;
    }
    try
    {
        // 1. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame(argv[1]);

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
        LogStop(ec.value());
        #endif
         
         
     } });

        // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры
        http_handler::RequestHandler handler(game, std::filesystem::path(argv[2]));

        // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
       // std::cout << "Hello! Server is starting at port " << port << std::endl;
        http_server::ServeHttp(ioc, {address, port}, [&handler](auto &&req, auto &&send)
                               { handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send)); });

        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        std::cout << "Server has started..."sv << std::endl;
        
        #ifdef CONSOLE_LOGGING
        LogRun(port, address.to_string());
        #endif
        
        //  6. Запускаем обработку асинхронных операций

        RunWorkers(std::max(1u, num_threads), [&ioc]
                   { ioc.run(); });
    }
    catch (const std::exception &ex)
    {
        
         #ifdef CONSOLE_LOGGING
         LogStop(ex, EXIT_FAILURE);
         #endif
       
        
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
