#pragma once
#pragma once
#include "../req_helper/help.h"

namespace request_handler
{

  template <typename Requester>
  class APIHandler
  {
  private:
    Requester req_;
    api::Play &game_;
    std::vector<std::string> parsed_target_;

  public:
    APIHandler(Requester &&req, api::Play &gm, std::vector<std::string> pt) 
              : req_(req), game_(gm), parsed_target_(pt){};

    VariantResponse MakeResponce()
    {

      if (!CheckBaseValid(parsed_target_))
      {
        return Make400JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::badRequest), 
                          std::string(reason_to_human::API_Base_Check_Failed));
      };

      DirectionAPI direcion = Director(parsed_target_);

      if (direcion == DirectionAPI::MAPS)
      {
        return GetHeadApi();
      }
      else if (direcion == DirectionAPI::PLAYERS)
      {
        return Players();
      }
      else if (direcion == DirectionAPI::JOIN)
      {
        return Join();
      }
      else if (direcion == DirectionAPI::STATE)
      {
        return State();
      }
      else
      {
        return Make400JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::badRequest), 
                          std::string(reason_to_human::API_Base_Check_Failed));
      };
      return {};
    };

  private:
    VariantResponse GetHeadApi();
    VariantResponse Join();
    VariantResponse Players();
    VariantResponse State();
  };

/*
Уважаемый ревьюер!! Здесь прекрасно вижу места копи-паста... Но
зато каждая функция хорошо последовательн о читабельна.. 
Пожертвовал несколькими десятками строками кода в+ к к размеру
бинарника - зато все последовательно и ясно видна логика функций
и без скачков...
Несколько вариантов есть убрать повторяющийся паттерн - но сильно
ухудшит читабельность и последовательность логики....
*/
  template <typename Requester>
  VariantResponse APIHandler<Requester>::GetHeadApi()
  {

    StringResponse response = Template(req_.version(), req_.keep_alive(), false);
    std::string body;

    if (parsed_target_.size() == 3)
    {
      response.result(http::status::ok);
      body = MakeAllMaps(game_.Game());
    }
    else
    {
      auto map = game_.Game().FindMap(model::Map::Id(parsed_target_[3]));
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
    response.prepare_payload();
    return response;
  };

  template <typename Requester>
  VariantResponse APIHandler<Requester>::Join()
  {

    if (req_.method() != RequestMethod::post)
      return Make405JSB(req_.version(), req_.keep_alive(),
                        std::string(req_static_str::invalidMethod), std::string(reason_to_human::Only_POST_method_is_expected),
                        std::string(req_static_str::Allowed_POST));
    try
    {
      json::value object = json::parse(req_.body());

      std::string u_name = std::string(object.as_object().at(std::string(req_static_str::userName)).as_string());
      std::string map_id = std::string(object.as_object().at(std::string(req_static_str::mapId)).as_string());

      if (u_name.empty())
        return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                          std::string(reason_to_human::Invalid_name));

      if (!game_.HavingMap(map_id))
        return Make404JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::mapNotFound),
                          std::string(reason_to_human::Map_not_found));

      auto player = game_.JoinGame(u_name, map_id);

      return Make200JSB(req_.version(), req_.keep_alive(), player->GetToken(), player->GetId());
    }
    catch (...)
    {
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string(reason_to_human::Join_game_request_parse_error));
    }
    return {};
  };

  template <typename Requester>
  VariantResponse APIHandler<Requester>::Players()
  {
    try
    {
      /////////////
      if (req_.method() != RequestMethod::get && req_.method() != RequestMethod::head)
        return Make405JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidMethod), std::string(reason_to_human::Invalid_method),
                          std::string(req_static_str::Allowed_GET_HEAD));

      std::string authorization = std::string(req_[HttpHeader::authorization]);
      std::vector<std::string> parsed_poken = ParseToken(std::move(authorization));
      if (parsed_poken.empty())
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidToken), std::string(reason_to_human::Authorization_header_is_missing));

      std::shared_ptr<api::Player> player = game_.FindByToken(parsed_poken[1]);

      if (player == nullptr)
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::unknownToken), std::string(reason_to_human::Player_token_has_not_been_found));

      ///////////
      
      std::string tmp = game_.GetJSONSession(player);
      return Make200JSB(req_.version(), req_.keep_alive(), std::move(tmp));
    }
    catch (...)
    {
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string(reason_to_human::Players_parse_error));
    }
  }

  template <typename Requester>
  VariantResponse APIHandler<Requester>::State(){
     
    try
    {

      if (req_.method() != RequestMethod::get && req_.method() != RequestMethod::head)
        return Make405JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidMethod), std::string(reason_to_human::Invalid_method),
                          std::string(req_static_str::Allowed_GET_HEAD));

      std::string authorization = std::string(req_[HttpHeader::authorization]);
      std::vector<std::string> parsed_poken = ParseToken(std::move(authorization));
      if (parsed_poken.empty())
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidToken), std::string(reason_to_human::Authorization_header_is_missing));

      
      std::shared_ptr<api::Player> player = game_.FindByToken(parsed_poken[1]);
      if (player == nullptr)
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::unknownToken), std::string(reason_to_human::Player_token_has_not_been_found));

      std::shared_ptr<model::GameSession> sess = player->PlayersSession();
      return Make200State(req_.version(), req_.keep_alive() , sess);
      
    }
    catch (...)
    {
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string(reason_to_human::Players_parse_error));
    }
 }

}