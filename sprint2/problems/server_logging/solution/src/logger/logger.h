#pragma once

#include <boost/log/trivial.hpp>     // для BOOST_LOG_TRIVIAL
#include <boost/log/core.hpp>        // для logging::core
#include <boost/log/expressions.hpp> // для выражения, задающего фильтр 
#include <boost/log/utility/setup/file.hpp> //для boost::log::add_file_log
#include <boost/log/utility/setup/common_attributes.hpp> //для boost::log::add_common_attributes()
#include <boost/log/utility/setup/console.hpp> //для boost::log::logging::add_console_log()
#include <boost/date_time.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>//для манипулятора logging::add_value

#include <boost/beast/http.hpp>
#include <string_view>
#include <variant>
#include "../json_loader.h"

#define CONSOLE_LOGGING 1

using namespace std::literals;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace beast = boost::beast;
namespace http= beast::http;

using HttpRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;
using FileResponse = http::response<http::file_body>;
using VariantResponse = std::variant<StringResponse, FileResponse>;
using TimePoint = std::chrono::system_clock::time_point; 


struct ResponseParams{
  ResponseParams(const VariantResponse& resp){
      if(std::holds_alternative<StringResponse>(resp)){
         content_type = std::string(std::get<StringResponse>(resp)[http::field::content_type]);
         code =  std::get<StringResponse>(resp).result_int();
      }
      else if(std::holds_alternative<StringResponse>(resp)){
         content_type = std::string(std::get<FileResponse>(resp)[http::field::content_type]);
         code =  std::get<FileResponse>(resp).result_int();
      }
  };
  int code;
  std::string content_type; 
};

struct RequestParams{
  RequestParams(const HttpRequest& req, const std::string& IP){
     method = std::string(req.method_string());
     target = std::string(req.target());
     ip = IP;
  };
  std::string method;
  std::string target;
  std::string ip;
};

json::value LogRun(int port, const std::string& ip);
json::value LogErr(beast::error_code ec, std::string_view what);
json::value LogRequest(RequestParams req);
json::value LogResponse(const ResponseParams& resp, TimePoint start);
json::value LogStop(const std::exception&ex, int code);
json::value LogStop(int code);
TimePoint GetNow();



