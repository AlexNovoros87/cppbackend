#include "request_handler.h"

// #define SERIALIZE 1
#ifdef SERIALIZE
#include "json_loader.h"
#endif

namespace request_handler
{
 VariantResponse RequestHandler::MakeStaticRespone(unsigned version, bool keep_alive, std::string target)
  {
   
    std::string_view tmp = target;
    if (!tmp.empty())
    {
      if (tmp[0] == '\\' || tmp[0] == '/')
        tmp = tmp.substr(1);
    }
    const fs::path req_path = fs::weakly_canonical(fs::path(tmp));                // target запроса
    const fs::path index_addition = fs::weakly_canonical(fs::path("index.html")); // добавка к пути паки статик
    const fs::path absolute = way_to_static / req_path;                           // абсолютный желаемый путь запроса
    const fs::path index_path = way_to_static / index_addition;                   // путь до index.html

    if (!IsSubPath(absolute, way_to_static))
    {
      auto resp = Make400(version, keep_alive);
      resp.prepare_payload();
      return resp;
    }

    FileResponse response;
    http::file_body::value_type file;
    std::string req_line = target;
    std::string extention = GetExtention(req_line);
    
    if (tmp.empty())
    {
      if (sys::error_code ec; file.open(index_path.c_str(), beast::file_mode::read, ec), ec)
      {
        std::cout << "Failed to open file "sv << index_path.c_str() << std::endl;
        auto resp = Make404(version, keep_alive);
        resp.prepare_payload();
        return resp;
      }
      response.body() = std::move(file);
      response.set(http::field::content_type, type_content.at(".html"));
      response.prepare_payload();
      return response;
    }

    if (sys::error_code ec; file.open(absolute.c_str(), beast::file_mode::read, ec), ec)
    {
      auto resp = Make404(version, keep_alive);
      std::cout << "Failed to open file "sv << absolute.c_str() << std::endl;
      resp.prepare_payload();
      return resp;
    }
   
    if(type_content.count(extention) == 0){
       response.set(http::field::content_type, type_content.at(""));
    }
    else
    {
     response.set(http::field::content_type, type_content.at(extention));
    }
    
    response.body() = std::move(file);
    response.prepare_payload();
    
    return response;
  };
 
 
 VariantResponse RequestHandler::ReportServerError(unsigned version, bool keep_alive) const{
  
  
  return{};
 };

} // namespace http_handler

