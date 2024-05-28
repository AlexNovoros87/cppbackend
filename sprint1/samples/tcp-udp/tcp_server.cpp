#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

int main() {
    static const int port = 3333;
 // контекст ввода-вывода.
    net::io_context io_context;
 // чтобы сервер мог принимать подключения клиентов, 
 // он должен создать акцептор, то есть объект 
 // типа tcp::acceptor. Акцептор слушает порт и может 
 // принимает входящие соединения
 
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cout << "Waiting for connection..."sv << std::endl;
// -------------------------------------------------------------------

//создание акцептора не вынуждает программу ждать подключения. 
//Для этого нужно явно попросить Asio принять соединение:

    
    /*
    Код ошибки ec используется как альтернатива выбрасыванию исключения.
    */
    boost::system::error_code ec;
    
    /*
     После принятия соединения сокет можно использовать для получения 
     и отправления данных. Он привязан к установленному соединению и 
     становится для нас интерфейсом этого соединения.    
    */
    tcp::socket socket{io_context};
    /*
    Вызов метода accept заставит программу ждать, пока кто-то не подключится 
    к серверу по указанному порту. 
    */
    acceptor.accept(socket, ec);

    if (ec) {
        std::cout << "Can't accept connection"sv << std::endl;
        return 1;
    }
   
   // -----------------------------------------------------------------------------
    // В Boost.Asio есть множество функций и методов для чтения. 
    // Мы выбрали read_until потому, что хотим прочитать строку 
    // вплоть до символа \n. Вместо одиночного символа эта функция 
    // может принимать также регулярное выражение, показывающее, 
    // где нужно остановиться. 
    // Результат сохраняется в объект streambuf, который 
    // преобразуется в std::string парой итераторов. 
    // Операция синхронная — она будет ждать пока все нужные данные 
    // не будут прочитаны.
    
    net::streambuf stream_buf;

    net::read_until(socket, stream_buf, '\n', ec);
    
    std::string client_data{std::istreambuf_iterator<char>(&stream_buf),
                            std::istreambuf_iterator<char>()};

  //------------------------------------------------------------------
  //Завершим сервер ответом клиенту.  
   
    if (ec) {
        std::cout << "Error reading data"sv << std::endl;
        return 1;
    }

    std::cout << "Client said: "sv << client_data << std::endl;

    // Чтобы отправить данные через сокет противоположной стороне, 
    // мы использовали метод write_some. В него передаётся буфер, 
    // который можно сконструировать из разных объектов. 
    // Самый простой и понятный способ — из std::string_view. 
    // Буфер конструирует функция boost::asio::buffer.
    
    // Помимо std::string_view она может принимать const void*, 
    // снабжённый количеством передаваемых байт. Этот способ допустимо 
    // использовать для передачи данных произвольного объекта.
    
    socket.write_some(net::buffer("Hello, I'm server!\n"sv), ec);

    if (ec) {
        std::cout << "Error sending data"sv << std::endl;
        return 1;
    }

}