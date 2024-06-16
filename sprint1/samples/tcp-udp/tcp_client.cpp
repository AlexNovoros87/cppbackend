#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;

using namespace std::literals;

int main(int argc, char** argv) {
    static const int port = 3333;

    if (argc != 2) {
        std::cout << "Usage: "sv << argv[0] << " <server IP>"sv << std::endl;
        return 1;
    }

    // Создадим endpoint - объект с информацией об адресе и порте.
    // Для разбора IP-адреса пользуемся функцией net::ip::make_address.
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(argv[1], ec), port);

    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        return 1;
    }

    //------------------------------------------------------------------------
    
    /*
    Теперь подключаемся к серверу. Акцептор на этот раз не нужен, 
    так как клиент не принимает подключений. Он отправляет запрос 
    — и для этого используется сокет, инициализированный контекстом:
    */

   net::io_context io_context;
   tcp::socket socket{io_context};
   socket.connect(endpoint, ec);

    if (ec) {
        std::cout << "Can't connect to server"sv << std::endl;
        return 1;
    }

   //---------------------------------------------------------------------------

    
// Повторим теперь процедуру обмена данными, но в обратном порядке: если 
// сервер сначала ждёт данные, то клиент должен первым их отправить. 
// Иначе возникнет ситуация, когда обе стороны неограниченно долго ждут, 
// пока их собеседник заговорит


// Отправляем данные и проверяем, что нет ошибки.

// write_some занимает промежуточное положение между двумя крайностями. 
// Она ждёт, пока хотя бы один байт передан. Это даёт гарантию, что в 
// момент вызова функции соединение ещё активно.

socket.write_some(net::buffer("Hello, I'm client!\n"sv), ec);
if (ec) {
    std::cout << "Error sending data"sv << std::endl;
    return 1;
}

net::streambuf stream_buf;
net::read_until(socket, stream_buf, '\n', ec);
std::string server_data{std::istreambuf_iterator<char>(&stream_buf),
                        std::istreambuf_iterator<char>()};

if (ec) {
    std::cout << "Error reading data"sv << std::endl;
    return 1;
}

std::cout << "Server responded: "sv << server_data << std::endl; 

//----------------------------------------------------------------



}