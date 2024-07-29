#include "logger.h"

namespace logger{
struct LogConst{
    LogConst() = delete;

    struct All{
    All() = delete;
    static constexpr boost::json::string_view message = "message";
    static constexpr boost::json::string_view data = "data";
    static constexpr boost::json::string_view timestamp = "timestamp";
    static constexpr boost::json::string_view code = "code";
    };
   
   struct Req{
   Req() = delete;
   static constexpr boost::json::string_view request_received = "request received";
   static constexpr boost::json::string_view ip = "ip";
   static constexpr boost::json::string_view URI = "URI";
   static constexpr boost::json::string_view  method = "method";
   };
   
   struct Resp{
   Resp() = delete;
   static constexpr boost::json::string_view response_sent  = "response sent";
   static constexpr boost::json::string_view response_time = "response_time";
   static constexpr boost::json::string_view content_type = "content_type";
   static constexpr boost::json::string_view null = "null";
   };
   
   struct Run{
   Run()= delete;
   static constexpr boost::json::string_view port = "port";
   static constexpr boost::json::string_view address = "address";
   static constexpr boost::json::string_view server_started = "server started";
   };
 
   struct Stop{
   Stop() = delete;
   static constexpr boost::json::string_view server_exited = "server exited";
   static constexpr boost::json::string_view exception = "exception";
   
   };
 
   struct Err{
   Err() = delete;
   static constexpr boost::json::string_view error = "error";
   static constexpr boost::json::string_view text = "text";
   static constexpr boost::json::string_view where = "where";
   static constexpr boost::json::string_view read = "read";
   static constexpr boost::json::string_view write = "write";
   static constexpr boost::json::string_view accept = "accept";
   };

};

std::string GetTimeStamp(){
    boost::posix_time::ptime current_time = boost::posix_time::microsec_clock::local_time();
    return to_iso_extended_string(current_time);
}

TimePoint GetNow(){
    return std::chrono::system_clock::now();
};

int ChronoDurationMS(const TimePoint& start){
    auto dur = GetNow() - start;
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
    return milliseconds.count();
}

void SetNowTimeStamp(json::value & val){
 val.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
}

json::value GetGeneralTemplate(){
 json::value obj{
        {LogConst::All::timestamp, ""},
        {LogConst::All::data, {}},
        {LogConst::All::message, ""} 
        };
 return obj;
}

json::value ReadyJSONObject(json::value tmp, boost::json::string_view str){
json::value val = GetGeneralTemplate();
val.as_object().at(LogConst::All::data) = std::move(tmp);
val.as_object().at(LogConst::All::message) = str;
return val;  
};

json::value RequestTemplate()
{
    json::value tmp {{LogConst::Req::ip, ""}, 
                    {LogConst::Req::URI, ""}, 
                    {LogConst::Req::method, ""}};

    return ReadyJSONObject(std::move(tmp), LogConst::Req::request_received);
};

json::value ResponseTemplate() { 

 json::value tmp    {{LogConst::Resp::response_time, 0}, 
                    {LogConst::All::code, 200}, 
                    {LogConst::Resp::content_type, 0 }};

 return ReadyJSONObject(std::move(tmp), LogConst::Resp::response_sent);

};

json::value RunServerTemplate() {

json::value tmp {   {LogConst::Run::port, INT_MAX}, 
                    {LogConst::Run::address, ""}};

return ReadyJSONObject(std::move(tmp), LogConst::Run::server_started);
};

json::value StopServerTemplate(){

json::value tmp {   {LogConst::All::code, 0} 
                    };

    return ReadyJSONObject(std::move(tmp), LogConst::Stop::server_exited);

};

json::value ErrorTemplate(){
json::value tmp {   {LogConst::All::code, 0},
                    {LogConst::Err::text, ""}, 
                    {LogConst::Err::where,""}};
return ReadyJSONObject(std::move(tmp), LogConst::Err::error);
};


//!!!!!!!!!!!!!!!!!!!!!
json::value LogRun(int port, const std::string& ip){
   auto run_obj = RunServerTemplate();
   run_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Run::address) = std::string(ip);
   run_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Run::port) = port;
   run_obj.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
    BOOST_LOG_TRIVIAL(info) <<json::serialize(run_obj);
   return run_obj;
};
using HttpHeader = http::field;
json::value LogErr(beast::error_code ec, std::string_view what){
    auto err_obj = ErrorTemplate();
    err_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::All::code) = ec.value();
    err_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Err::text) = ec.message();
    err_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Err::where) = std::string(what);
    err_obj.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
    BOOST_LOG_TRIVIAL(info) <<json::serialize(err_obj);
    return err_obj;
};

json::value LogRequest(RequestParams req){

 auto req_obj = RequestTemplate();
 req_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Req::ip) = std::move(req.ip);
 req_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Req::URI) = std::move(req.target);
 req_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Req::method) = std::move(req.method);
 req_obj.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
 BOOST_LOG_TRIVIAL(info) <<json::serialize(req_obj);
 return req_obj;
};


json::value LogResponse(const ResponseParams& resp,TimePoint start){

  auto resp_obj = ResponseTemplate();
  resp_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Resp::content_type) = resp.content_type;
  resp_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::All::code) = resp.code;
  resp_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::Resp::response_time) = ChronoDurationMS(start);
  resp_obj.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
  BOOST_LOG_TRIVIAL(info) <<json::serialize(resp_obj);

  return resp_obj; 
};

json::value LogStop(const std::exception&ex, int code){
  auto stop_obj = StopServerTemplate();
  stop_obj.as_object().at(LogConst::All::data).as_object()[LogConst::Stop::exception] = ex.what();
  stop_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::All::code) = code;
  stop_obj.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
  BOOST_LOG_TRIVIAL(info) <<json::serialize(stop_obj);
  return stop_obj;
};

json::value LogStop(int code){
 auto stop_obj = StopServerTemplate();
 stop_obj.as_object().at(LogConst::All::data).as_object().at(LogConst::All::code) = code;
 stop_obj.as_object().at(LogConst::All::timestamp) = GetTimeStamp();
 BOOST_LOG_TRIVIAL(info) <<json::serialize(stop_obj);
 return stop_obj;
};

}