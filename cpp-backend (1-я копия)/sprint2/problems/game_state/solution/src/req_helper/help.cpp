#include "help.h"
#include <sstream>
#include <iomanip>
#include <fstream>

// PARSED TARGET
namespace request_handler
{
  std::vector<std::string> ParseToken(std::string target)
  {
    if (target.empty())
      return {};
    int i = 0;

    std::vector<std::string> parsed;
    std::string tmp;

    for (int j = 0; j < target.size(); ++j)
    {
      if (target[j] != ' ')
      {
        tmp += target[j];
      }
      else
      {

        if (!tmp.empty())
        {
          if (parsed.size() == 2)
            return {};
          parsed.push_back(tmp);
          tmp.clear();
        }
      }
    }

    if (!tmp.empty())
      parsed.push_back(tmp);
    if (parsed[0] != "Bearer" || parsed[1].size() != 32)
      return {};
    return parsed;
  };

  DirectionAPI Director(const std::vector<std::string> &container)
  {
    if (container.size() < 3 || container.size() > 4)
      return DirectionAPI::ERROR;
    if (container[2] == json_foo::maps)
      return DirectionAPI::MAPS;
    if (container[2] != json_foo::game)
      return DirectionAPI::ERROR;
    if (container.size() == 4)
    {
      if (container[3] == json_foo::players)
        return DirectionAPI::PLAYERS;
      if (container[3] == json_foo::join)
        return DirectionAPI::JOIN;
      if (container[3] == json_foo::state)
        return DirectionAPI::STATE;
    }
    return DirectionAPI::ERROR;
  };

  request_handler::TypeRequest GetTypeRequset(const std::vector<std::string> &vec)
  {
    if (vec.empty())
      return request_handler::TypeRequest::STATIC;
    if (vec[0] == json_foo::api)
      return request_handler::TypeRequest::API;
    return request_handler::TypeRequest::STATIC;
  };

  std::vector<std::string> ParseTarget(std::string target)
  {
    std::vector<std::string> targets;
    if (target.empty())
      return targets;
    target = target.substr(1);
    char lastchar;
    std::string oss;
    for (char ch : target)
    {
      if (ch == '/')
      {
        if (lastchar == '/')
          return {};
        if (!oss.empty())
          targets.push_back(std::move(oss));
        oss.clear();
        lastchar = ch;
      }
      else
      {
        oss.push_back(ch);
        lastchar = ch;
      }
    }
    if (!oss.empty())
      targets.push_back(std::move(oss));

    return targets;
  }

  bool CheckBaseValid(const std::vector<std::string> &container)
  {
    if (container.size() < 3 || container.size() > 4)
      return false;
    if (container[0] != json_foo::api)
      return false;
    if (container[1] != json_foo::v1)
      return false;
    if (container[2] != json_foo::maps && container[2] != json_foo::game)
      return false;

    return true;
  };

}

// RAZNOYE
namespace request_handler
{
  std::string GetExtention(std::string req_body)
  {
    std::string extention;
    if (req_body.empty())
      return extention;
    for (size_t i = req_body.size() - 1; i > 0; --i)
    {
      if (req_body[i] == '\\' || req_body[i] == '/')
        break;
      if (req_body[i] == '.')
      {
        extention += '.';
        break;
      };
      extention += req_body[i];
    }
    std::reverse(extention.begin(), extention.end());
    return extention;
  };

  bool IsSubPath(fs::path path, fs::path base)
  {
    // Проверяем, что все компоненты base содержатся внутри path
    for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p)
    {
      if (p == path.end() || *p != *b)
      {
        return false;
      }
    }
    return true;
  }

}
// STATIC ERRORS
namespace request_handler
{
  StringResponse Make400(unsigned version, bool alive)
  {
    StringResponse response;
    response.keep_alive(alive);
    response.version(version);
    response.result(http::status::bad_request);
    response.set(http::field::content_type, request_handler::type_content.at(Extensions::txt));
    response.body() = "ERROR 400: BAD REQUEST";
    return response;
  };

  StringResponse Make404(unsigned version, bool alive)
  {
    StringResponse response;
    response.keep_alive(alive);
    response.version(version);
    response.result(http::status::not_found);
    response.set(http::field::content_type, request_handler::type_content.at(".txt"));
    response.body() = "ERROR 404: NOT FOUND";
    return response;
  };

}

// JSON PREPARE
namespace request_handler
{

  json::value DogObject(std::shared_ptr<model::Dog> dog)
  {
    json::object tmp;
    json::array pos{json::value(dog->GetDogCoord().x).get_double() , dog->GetDogCoord().y};
    json::array spd{dog->GetDodSpd().speed_x , dog->GetDodSpd().speed_y};
    tmp[std::string(req_static_str::pos)] = std::move(pos);
    tmp[std::string(req_static_str::speed)] = std::move(spd);
    tmp[std::string(req_static_str::dir)] = model::string_directions_.at(dog->GetOrientation());
    json::value obj = std::move(tmp);
    return obj;
  };

  std::string MakeStateBody(std::shared_ptr<model::GameSession> session_)
  {
    auto &dogs = session_->GetDogs();
    json::object doglist;
    
    for (auto &&dog : dogs)
    {
      doglist[std::to_string(dog.first)] = (DogObject(dog.second));
    };

    json::object tmp;
    tmp[std::string(req_static_str::players)] = std::move(doglist);
    json::value obj=std::move(tmp);
    
   
    
    return json::serialize(obj);
  
  };

  std::string CodeMessageBody(std::string code, std::string message)
  {
    auto msg = obj_code_message;
    msg.as_object().at(std::string(req_static_str::message)) = std::move(message);
    msg.as_object().at(std::string(req_static_str::code)) = std::move(code);
    return json::serialize(msg);
  };

  std::string TokenIdBody(std::string token, uint64_t id)
  {

    auto msg = obj_token_id_player;

    msg.as_object().at(std::string(req_static_str::authToken)) = token;

    msg.as_object().at(std::string(req_static_str::playerId)) = id;
    try
    {
      json::serialize(msg);
    }
    catch (...)
    {
    }
    return json::serialize(msg);
  };

}

// JSON
namespace request_handler
{

  StringResponse Template(unsigned version, bool alive, bool control)
  {
    StringResponse resp;
    resp.version(version);
    resp.keep_alive(alive);
    resp.set(HttpHeader::content_type, ContentType::APP_JSON);
    if (control)
    {
      resp.set(HttpHeader::cache_control, std::string(req_static_str::no_cache));
    }
    return resp;
  };

  // JSB - JSon-Body
  StringResponse Make200JSB(unsigned version, bool alive, std::string token, uint32_t id)
  {
    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST200);
    resp.body() = TokenIdBody(token, id);
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make200State(unsigned version, bool alive, std::shared_ptr<model::GameSession> session)
  {

    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST200);
    resp.body() = MakeStateBody(session);
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make200JSB(unsigned version, bool alive, std::string body)
  {
    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST200);
    resp.body() = std::move(body);
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make400JSB(unsigned version, bool alive, std::string code, std::string message)
  {
    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST400);
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make401JSB(unsigned version, bool alive, std::string code, std::string message)
  {
    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST401);
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make404JSB(unsigned version, bool alive, std::string code, std::string message)
  {
    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST404);
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make405JSB(unsigned version, bool alive, std::string code, std::string message, std::string allow)
  {
    StringResponse resp = Template(version, alive, true);
    resp.result(statuses::ST405);
    resp.set(HttpHeader::allow, std::move(allow));
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

}

// MAPS MANIP
namespace request_handler
{

  std::string MakeOneMap(const model::Map *map)
  {
    std::string oss;
    oss.append("  \"id\": \"").append(*map->GetId()).append("\",\n  \"name\": \"").append(map->GetName()).append("\",\n  \"roads\": [\n");
    auto &roads = map->GetRoads();
    for (int i = 0; i < roads.size(); ++i)
    {
      oss.append("    ").append(roads[i].GetJsonType());
      if (i + 1 < roads.size())
        oss.append(",");
      oss.append("\n");
    }
    oss.append("  ],\n      \"buildings\": [\n");
    auto &buildings = map->GetBuildings();
    for (int i = 0; i < buildings.size(); ++i)
    {
      oss.append("    ").append(buildings[i].GetJsonType());
      if (i + 1 < buildings.size())
        oss.append(",");
      oss.append("\n");
    }
    oss.append("  ],\n      \"offices\": [\n");
    auto &offices = map->GetOffices();
    for (int i = 0; i < offices.size(); ++i)
    {
      oss.append("    ").append(offices[i].GetJsonType());
      if (i + 1 < offices.size())
        oss.append(",");
      oss.append("\n");
    }
    oss.append("  ]");
    return oss;
  };

  std::string MakeAllMaps(const model::Game &game)
  {
    auto &maps = game.GetMaps();
    std::string oss;
    oss.append("[");
    for (size_t i = 0; i < maps.size(); ++i)
    {
      oss.append("{\"id\": \"" + *maps[i].GetId() + "\", \"name\": \"" + maps[i].GetName() + "\"}");
      if (i + 1 < maps.size())
        oss.append(",");
    };
    oss.append("]");
    return oss;
  };
}