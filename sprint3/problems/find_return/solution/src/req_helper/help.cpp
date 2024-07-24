#include "help.h"
#include <sstream>
#include <iomanip>
#include <fstream>

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
УВАЖАЕМЫЙ КОД-РЕВЬЮЕР!!!!
ГДЕ ВСТРЕЧАЕТСЯ  {Literals::ID, *(map.GetId())} И ПОДОБНОЕ,
ЭТО НЕ УКАЗАТЕЛЬ!! ЭТО ОБЪЕКТ ИЗ tagget.h ДАННЫЙ ИЗ ТЕОРИИ
*/

// PARSED TARGET
namespace request_handler
{

  std::vector<std::string> ParseToken(std::string target)
  {
    if (target.empty())
      return {};

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
    if (container.size() < 3 || container.size() > 5)
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
      if (container[3] == json_foo::tick)
        return DirectionAPI::TICK;
    }
    if (container.size() == 5)
    {
      if ((container[3] == json_foo::player) && (container[4] == json_foo::action))
        return DirectionAPI::PLAYER_ACTION;
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
    if (container.size() < 3 || container.size() > 5)
      return false;
    if (container[0] != json_foo::api)
      return false;
    if (container[1] != json_foo::v1)
      return false;
    if (container[2] != json_foo::maps &&
        container[2] != json_foo::game)
      return false;

    return true;
  };

}

// RAZNOYE
namespace request_handler
{

  TimePoint GetNow()
  {
    return std::chrono::system_clock::now();
  };

  bool IsCorrectWay(std::string_view way)
  {
    if (way != model::UDLR::U && way != model::UDLR::D && way != model::UDLR::L && way != model::UDLR::R && way != model::UDLR::STOP)
      return false;
    return true;
  };

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
  StringResponse TemplateTxtContent(unsigned version, bool alive)
  {
    StringResponse response;
    response.keep_alive(alive);
    response.version(version);
    response.set(http::field::content_type, request_handler::type_content.at(Extensions::txt));
    return response;
  }

  StringResponse Make400(unsigned version, bool alive)
  {
    StringResponse response = TemplateTxtContent(version, alive);
    response.result(http::status::bad_request);
    response.body() = "ERROR 400: BAD REQUEST";
    return response;
  };

  StringResponse Make404(unsigned version, bool alive)
  {
    StringResponse response = TemplateTxtContent(version, alive);
    response.result(http::status::not_found);
    response.body() = "ERROR 404: NOT FOUND";
    return response;
  };

}

// JSON PREPARE
namespace request_handler
{
  json::object LootTobagObject(const model::Loot &loot)
  {
    json::object tmp;
    tmp[std::string(req_static_str::id)] = loot.id;
    tmp[std::string(req_static_str::type)] = loot.type;
    return tmp;
  }
  json::array DogBagObject(const std::vector<model::Loot> &bag)
  {
    json::array arr;
    for (auto loot : bag)
    {
      arr.push_back(LootTobagObject(loot));
    }
    return arr;
  }
  json::value DogObject(std::shared_ptr<model::Dog> dog)
  {
    json::object tmp;
    json::array pos{json::value(dog->GetDogCoord().x).get_double(), dog->GetDogCoord().y};
    json::array spd{dog->GetDodSpd().speed_x, dog->GetDodSpd().speed_y};
    tmp[std::string(req_static_str::pos)] = std::move(pos);
    tmp[std::string(req_static_str::speed)] = std::move(spd);
    tmp[std::string(req_static_str::dir)] = std::string(model::nswe_string.at(dog->GetOrientation()));
    tmp[std::string(req_static_str::bag)] = DogBagObject(dog->GetLootBag());

    json::value obj = std::move(tmp);
    return obj;
  };

  json::object LootObject(const std::pair<model::LootCoordinates, model::Loot> &loot)
  {

    json::array coords;
    coords.push_back(loot.first.x);
    coords.push_back(loot.first.y);

    json::object lootlist;
    lootlist["type"] = loot.second.type;
    lootlist["pos"] = std::move(coords);
    return lootlist;
  }

  std::string MakeStateBody(std::shared_ptr<model::GameSession> session_)
  {
    auto &dogs = session_->GetDogs();
    json::object doglist;

    for (auto &&dog : dogs)
    {
      doglist[std::to_string(dog.first)] = (DogObject(dog.second));
    };

    json::object lootlist;
    auto &loot = session_->GetLootList();
    size_t num = 0;
    for (auto &&thing : loot)
    {
      lootlist[std::to_string(num++)] = LootObject(thing);
    }

    json::object tmp;
    tmp[std::string(req_static_str::players)] = std::move(doglist);
    tmp[std::string(req_static_str::lostObjects)] = std::move(lootlist);

    json::value obj = std::move(tmp);
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
    return json::serialize(msg);
  };

}

// JSON
namespace request_handler
{

  StringResponse Template(unsigned version, bool alive)
  {
    StringResponse resp;
    resp.version(version);
    resp.keep_alive(alive);
    resp.set(HttpHeader::content_type, type_content.at(Extensions::json));
    resp.set(HttpHeader::cache_control, std::string(req_static_str::no_cache));
    return resp;
  };

  // JSB - JSon-Body
  StringResponse Make200JSB(unsigned version, bool alive, std::string token, uint32_t id)
  {
    StringResponse resp = Template(version, alive);
    resp.result(statuses::ST200);
    resp.body() = TokenIdBody(token, id);
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make200State(unsigned version, bool alive, std::shared_ptr<model::GameSession> session)
  {

    StringResponse resp = Template(version, alive);
    resp.result(statuses::ST200);
    resp.body() = MakeStateBody(session);
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make200JSB(unsigned version, bool alive, std::string body)
  {
    StringResponse resp = Template(version, alive);
    resp.result(statuses::ST200);
    resp.body() = std::move(body);
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make400JSB(unsigned version, bool alive, std::string code, std::string message)
  {
    StringResponse resp = Template(version, alive);
    resp.result(statuses::ST400);
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make401JSB(unsigned version, bool alive, std::string code, std::string message)
  {
    StringResponse resp = Template(version, alive);
    resp.result(statuses::ST401);
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make404JSB(unsigned version, bool alive, std::string code, std::string message)
  {
    StringResponse resp = Template(version, alive);
    resp.result(statuses::ST404);
    resp.body() = CodeMessageBody(std::move(code), std::move(message));
    resp.prepare_payload();
    return resp;
  };

  StringResponse Make405JSB(unsigned version, bool alive, std::string code, std::string message, std::string allow)
  {
    StringResponse resp = Template(version, alive);
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

  json::object VRoad(const model::Road &road)
  {
    json::object res{{Literals::X0, road.GetStart().x},
                     {Literals::Y0, road.GetStart().y},
                     {Literals::Y1, road.GetEnd().y}};

    return res;
  }

  json::object HRoad(const model::Road &road)
  {
    json::object res{{Literals::X0, road.GetStart().x},
                     {Literals::Y0, road.GetStart().y},
                     {Literals::X1, road.GetEnd().x}};

    return res;
  }

  json::object MakeJSRoad(const model::Road &road)
  {
    if (road.IsVertical())
      return VRoad(road);
    return HRoad(road);
  }

  json::object MakeJSBuilding(const model::Building &build)
  {
    auto &bounds = build.GetBounds();
    json::object res{
        {Literals::X, bounds.position.x},
        {Literals::Y, bounds.position.y},
        {Literals::W, bounds.size.width},
        {Literals::H, bounds.size.height}};

    return res;
  }

  json::object MakeJSOffice(const model::Office &office)
  {
    json::object res{
        {Literals::ID, *(office.GetId())},
        {Literals::X, office.GetPosition().x},
        {Literals::Y, office.GetPosition().y},
        {Literals::OFFSETX, office.GetOffset().dx},
        {Literals::OFFSETY, office.GetOffset().dy}};

    return res;
  }

  json::object MakeShortMap(const model::Map &map)
  {
    json::object mp_json{
        {Literals::ID, *(map.GetId())},
        {Literals::NAME, map.GetName()}};

    return mp_json;
  }

  std::string MakeOneMap(const model::Map *map)
  {

    assert(map != nullptr);
    json::object mp_json;
    mp_json[Literals::ID] = *map->GetId();
    mp_json[Literals::NAME] = map->GetName();

    // ДОРОГИ
    json::array road_arr;
    auto &roads = map->GetRoads();
    for (auto &&rd : roads)
    {
      road_arr.push_back(MakeJSRoad(*rd));
    }
    mp_json[Literals::ROADS] = std::move(road_arr);

    // ЗДАНИЯ
    json::array build_arr;
    auto &build = map->GetBuildings();
    for (auto &&bld : build)
    {
      build_arr.push_back(MakeJSBuilding(bld));
    }
    mp_json[Literals::BUILDINGS] = std::move(build_arr);

    // ОФИСЫ
    json::array office_arr;
    auto &offices = map->GetOffices();
    for (auto &&of : offices)
    {
      office_arr.push_back(MakeJSOffice(of));
    }
    mp_json[Literals::OFFICES] = std::move(office_arr);

    // ЛУТ
    auto loot = frontend::FrontEndLoot::GetLootToFrontend(map->GetName());
    if (!loot.empty())
    {
      mp_json[Literals::lootTypes] = std::move(loot);
    }
    return json::serialize(mp_json);
  };

  std::string MakeAllMaps(const model::Game &game)
  {
    auto &maps = game.GetMaps();
    json::array maps_arr;
    for (auto &&map : maps)
    {
      maps_arr.push_back(MakeShortMap(map));
    }
    return json::serialize(maps_arr);
  }
}