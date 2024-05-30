#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <syncstream>
#include <chrono>
#include <functional>
#include <memory>
#include <boost/asio.hpp>
#include <thread>


namespace net = boost::asio;
namespace sys = boost::system;
using Timer = net::steady_timer;
using namespace std::literals;
using namespace std::chrono;