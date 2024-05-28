#include "audio.h"
#include <boost/asio.hpp>
#include <string>
#include <string_view>
#include <iostream>

using namespace std::literals;
namespace net = boost::asio;
using net::ip::udp;

//GLOBAL CONFS
const int MAX_BUFFER_SIZE = 65000;
const int MAX_TRANSFER = 65000;
const int MIN_PORT = 0;
const int MAX_PORT = 65536;

bool ValidPort(int port){
    if(port<0 || port >65536) return false;
    return (port >= MIN_PORT && port <= MAX_PORT);  
} 


void StartServer(uint16_t port){
   std::cout<<"SERVER RUNNING"<<std::endl;
    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

        // Запускаем сервер в цикле, чтобы можно было работать со многими клиентами
        for (;;) {
            // Создаём буфер достаточного размера, чтобы вместить датаграмму.
            std::array<char, MAX_BUFFER_SIZE> recv_buf;
            udp::endpoint remote_endpoint;

            // Получаем не только данные, но и endpoint клиента
            auto size = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
            std::cout<<"SIZE RESIEVED"<<size<<std::endl;

            Player  player(ma_format_u8, 1);
            player.PlayBuffer(recv_buf.data(), size, 1.5s);
            std::cout << "Playing done" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }



}

Recorder::RecordingResult Record(){
   //RECORDING
        Recorder recorder(ma_format_u8, 1);
        std::string str;
        std::cout << "Press Enter to record message..." << std::endl;
        std::getline(std::cin, str);
        auto rec_result = recorder.Record(65000, 1.5s);
        std::cout << "Recording done" << std::endl;
    return rec_result;
}


void StartClient(uint16_t port){
   std::cout<<"CLIENT RUNNING"<<std::endl;
  
    try {
       Recorder::RecordingResult res = Record();
       while (res.data.empty()) {
         res = Record();
       }
        size_t size_to_send = res.frames * sizeof(res.data[0]);
       
        std::cout<<size_to_send<<std::endl;
        net::io_context io_context;
        // Перед отправкой данных нужно открыть сокет. 
        // При открытии указываем протокол (IPv4 или IPv6) вместо endpoint.
        udp::socket socket(io_context, udp::v4());

        boost::system::error_code ec;
        auto endpoint = udp::endpoint(net::ip::make_address("127.0.0.1", ec), port);
        socket.send_to(net::buffer(res.data.data(), size_to_send), endpoint);

        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char** argv) {
   const std::string SRV = "server";
   const std::string CLNT = "client";

    if(argc < 3) {
       throw std::invalid_argument("ivalid arguments");
    }

    std::string mode;
    int port;

    try {
      mode = std::string(argv[1]);
      std::string s_port = std::string(argv[2]);
      
       if(mode.size() != 6)  throw std::invalid_argument("MODE Must be server or client");
       size_t converted_chars = 0;
       port = stoi(s_port, &converted_chars);
       if(converted_chars != s_port.size()) throw std::invalid_argument("ivalid PORT argument");
       
       for(auto &symbol : mode){
        symbol = tolower(symbol);
       }
       
       if(mode != SRV && mode != CLNT) throw std::invalid_argument("MODE Must be server or client");
       if(!ValidPort(port)) throw std::invalid_argument("PORT is invalid");
      
    }
    catch(std::exception &ex){
      std::cout<<ex.what()<<std::endl;
      std::abort();
    }
    catch(...){
       std::cout<<"unknown error in begin. reason::check arguments"<<std::endl;
       std::abort();
    }

    
    if(mode == SRV){
       StartServer(port);
    }
    else{
       StartClient(port);
    }
    
   
}


 /*
    
    Recorder recorder(ma_format_u8, 1);
    Player   player(ma_format_u8, 1);

    
        std::string str;
        std::cout << "Press Enter to record message..." << std::endl;
        std::getline(std::cin, str);

        auto rec_result = recorder.Record(65000, 1.5s);
        std::cout << "Recording done" << std::endl;

        std::cout<<rec_result.data.size()<<" "<<rec_result.frames<<std::endl;
        

        player.PlayBuffer(rec_result.data.data(), rec_result.frames, 1.5s);
        std::cout << "Playing done" << std::endl;
    

    return 0;

    */