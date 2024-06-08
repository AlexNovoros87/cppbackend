#pragma once
//GASCOOKER
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <cassert>
#include <deque>
#include <memory>

//INGRIDIENTS
//#include <functional>
//#include <optional>

//HOTDOG
#include <chrono>
#include <functional>
#include <optional>
#include <stdexcept>

//ORDER
//#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
//#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>
//#include <memory>
#include <thread>


namespace net = boost::asio;
namespace sys = boost::system;

//CLOCK
//#include <chrono>
using Clock = std::chrono::high_resolution_clock;
using Milliseconds = std::chrono::milliseconds;
using Timer = net::steady_timer;
using boost::system::error_code;
