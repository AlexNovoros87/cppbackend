#pragma once
#include <sstream>
#include <string>
#include <filesystem>
#include "server/http_server.h"
#include "req_static_objects.h"
#include "model.h"

// #define SERIALIZE 1
#ifdef SERIALIZE
#include "json_loader.h"
#endif
namespace fs = std::filesystem;

http_handler::TypeRequest GetTypeRequset(const std::vector<std::string> &vec);
std::vector<std::string> ParseTarget(std::string target);
bool CheckValid(const std::vector<std::string> &container);
std::string MakeAllMaps(const model::Game &game);
std::string MakeOneMap(const model::Map *map);
bool IsSubPath(fs::path path, fs::path base);

template <typename Body, typename Allocator>
StringResponse Make400(http::request<Body, http::basic_fields<Allocator>> &&req)
{
  StringResponse response;
  response.keep_alive(req.keep_alive());
  response.version(req.version());
  response.result(http::status::bad_request);
  response.set(http::field::content_type, http_handler::type_content.at(".txt"));
  response.body() = "ERROR 400: BAD REQUEST";
  return response;
};

template <typename Body, typename Allocator>
StringResponse Make404(http::request<Body, http::basic_fields<Allocator>> &&req)
{
  StringResponse response;
  response.keep_alive(req.keep_alive());
  response.version(req.version());
  response.result(http::status::not_found);
  response.set(http::field::content_type, http_handler::type_content.at(".txt"));
  response.body() = "ERROR 404: NOT FOUND";
  return response;
};

namespace http_handler
{
  namespace beast = boost::beast;
  namespace http = beast::http;

  class RequestHandler
  {
  model::Game &game_;
  fs::path way_to_static;
  
   public:
    explicit RequestHandler(model::Game &game,const fs::path& path_to_static)
        : game_{game} , way_to_static(path_to_static)
    {
    }

    RequestHandler(const RequestHandler &) = delete;
    RequestHandler &operator=(const RequestHandler &) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send)
    {
      send(MakeRespone(std::move(req)));
    }

  private:
    template <typename Body, typename Allocator>
    VariantResponse MakeRespone(http::request<Body, http::basic_fields<Allocator>> &&req);

    template <typename Body, typename Allocator>
    VariantResponse MakeAPIRespone(http::request<Body, http::basic_fields<Allocator>> &&req,
                                   const std::vector<std::string> &parsed_target);

    template <typename Body, typename Allocator>
    VariantResponse MakeStaticRespone(http::request<Body, http::basic_fields<Allocator>> &&req);

    
  };
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  template <typename Body, typename Allocator>
  VariantResponse RequestHandler::MakeRespone(http::request<Body, http::basic_fields<Allocator>> &&req)
  {
    std::ostringstream ostr;
    ostr << req.target();
    auto parsed_target = ParseTarget(std::move(ostr.str()));
    TypeRequest type_request = GetTypeRequset(parsed_target);

    // std::cout<<"I am REQ"<<std::endl;

    switch (type_request)
    {
    case TypeRequest::API:
      return MakeAPIRespone(std::move(req), parsed_target);
      break;

    case TypeRequest::STATIC:
      return MakeStaticRespone(std::move(req));
      break;
    }

    return {};
  }
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  template <typename Body, typename Allocator>
  VariantResponse RequestHandler::MakeStaticRespone(http::request<Body, http::basic_fields<Allocator>> &&request)
  {
    http::request<Body, http::basic_fields<Allocator>> req(request);

    std::string_view tmp = req.target();
    if (!tmp.empty())
    {
      if (tmp[0] == '\\' || tmp[0] == '/')
        tmp = tmp.substr(1);
    }
    const fs::path req_path = fs::weakly_canonical(fs::path(tmp));                   // target запроса
    const fs::path index_addition = fs::weakly_canonical(fs::path("index.html"));    // добавка к пути паки статик
    const fs::path absolute = way_to_static / req_path;           // абсолютный желаемый путь запроса
    const fs::path index_path = way_to_static / index_addition;   // путь до index.html
    
    std::cout<<way_to_static<<std::endl;
    std::cout<<absolute<<std::endl;
    
    
    if (!IsSubPath(absolute, way_to_static))
    {
      return Make400(std::move(req));
    }

    FileResponse response;
    http::file_body::value_type file;
    if (tmp.empty())
    {
      if (sys::error_code ec; file.open(index_path.c_str(), beast::file_mode::read, ec), ec)
      {
        std::cout << "Failed to open file "sv << index_path.c_str() << std::endl;
        return Make404(std::move(req));
      }
      response.body() = std::move(file);
      response.prepare_payload();
      return response;
    }

    if (sys::error_code ec; file.open(absolute.c_str(), beast::file_mode::read, ec), ec)
    {
      std::cout << "Failed to open file "sv << absolute.c_str() << std::endl;
      return Make404(std::move(req));
    }
    response.body() = std::move(file);
    response.prepare_payload();
    return response;
 };

  template <typename Body, typename Allocator>
  VariantResponse RequestHandler::MakeAPIRespone(http::request<Body, http::basic_fields<Allocator>> &&req, const std::vector<std::string> &parsed_target)
  {

    StringResponse response;
    response.keep_alive(req.keep_alive());
    response.version(req.version());
    response.set(http::field::content_type, ContentType::APP_JSON);

    std::string body;
    if (!CheckValid(parsed_target))
    {
      response.result(http::status::bad_request);
      body = IncorrectResponse::WRONG_REQ;
    }
    else if (parsed_target.size() == 3)
    {
      response.result(http::status::ok);
      body = MakeAllMaps(game_);
    }
    else
    {
      auto map = game_.FindMap(model::Map::Id(parsed_target[3]));
      if (map == nullptr)
      {
        response.result(http::status::not_found);
        body = IncorrectResponse::WRONG_MAP;
      }
      else
      {
        response.result(http::status::ok);
        body.append("{\n").append(MakeOneMap(map)).append("\n}");
      }
    }

#ifdef SERIALIZE
    auto obj = json::parse(body);
    response.body() = json::serialize(obj);
#else
    response.body() = body;
#endif

    return response;
  };

} // namespace http_handler
