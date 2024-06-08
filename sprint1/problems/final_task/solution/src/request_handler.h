#pragma once
#include <sstream>
#include <string>
#include "server/http_server.h"
#include "model.h"

// #define SERIALIZE 1
#ifdef SERIALIZE
#include "json_loader.h"
#endif

std::vector<std::string> ParseTarget(std::string target);
bool CheckValid(const std::vector<std::string> &container);
std::string MakeAllMaps(const model::Game &game);
std::string MakeOneMap(const model::Map *map);

namespace http_handler
{
  namespace beast = boost::beast;
  namespace http = beast::http;

  struct IncorrectResponse
  {
    IncorrectResponse() = delete;
    constexpr static std::string_view WRONG_MAP = "{\n  \"code\": \"mapNotFound\",\n  \"message\": \"Map not found\"\n}";
    constexpr static std::string_view WRONG_REQ = "{\n  \"code\": \"badRequest\", \n  \"message\": \"Bad request\"\n}";
  };

  class RequestHandler
  {
  public:
    explicit RequestHandler(model::Game &game)
        : game_{game}
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
    StringResponse MakeRespone(http::request<Body, http::basic_fields<Allocator>> &&req)
    {
      StringResponse response;
      response.keep_alive(req.keep_alive());
      response.set(http::field::content_type, ContentType::APP_JSON);
      response.version(req.version());

      std::ostringstream ostr;
      ostr << req.target();
      auto parsed_target = ParseTarget(std::move(ostr.str()));
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
    }

    model::Game &game_;
  };

} // namespace http_handler
