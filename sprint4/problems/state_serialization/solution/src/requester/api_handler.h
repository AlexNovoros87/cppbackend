#pragma once
#pragma once
#include "../req_helper/help.h"
#include <fstream>
#include <functional>
#include <map>
// #define WHAT 1

namespace request_handler
{

  template <typename Requester>
  class APIHandler
  {
  private:
    Requester req_;
    api::Play &game_;
    std::vector<std::string> parsed_target_;
    api::PlayObserver obs_;

    std::variant<VariantResponse, std::shared_ptr<api::Player>> UseAutorizationPattern();
    VariantResponse GetHeadApi();
    VariantResponse Join();
    VariantResponse Players();
    VariantResponse State();
    VariantResponse PlayerMove();
    VariantResponse Error();
    VariantResponse Tick();

    std::unordered_map<DirectionAPI, std::function<VariantResponse()>> functor_{
        {DirectionAPI::PLAYERS, std::bind(&APIHandler<Requester>::Players, this)},
        {DirectionAPI::MAPS, std::bind(&APIHandler<Requester>::GetHeadApi, this)},
        {DirectionAPI::JOIN, std::bind(&APIHandler<Requester>::Join, this)},
        {DirectionAPI::STATE, std::bind(&APIHandler<Requester>::State, this)},
        {DirectionAPI::PLAYER_ACTION, std::bind(&APIHandler<Requester>::PlayerMove, this)},
        {DirectionAPI::ERROR, std::bind(&APIHandler<Requester>::Error, this)},
        {DirectionAPI::TICK, std::bind(&APIHandler<Requester>::Tick, this)}

    };

  public:
    APIHandler(Requester &&req, api::Play &gm, const std::vector<std::string> &pt)
        : req_(req), game_(gm), parsed_target_(pt), obs_(gm){};

    VariantResponse MakeResponce()
    {

      if (!CheckBaseValid(parsed_target_))
      {
        return Make400JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::badRequest),
                          std::string(reason_to_human::API_Base_Check_Failed));
      };

      DirectionAPI direcion = Director(parsed_target_);
      return functor_.at(direcion)();
    };
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
  VariantResponse APIHandler<Requester>::Tick()
  {
    using namespace model;

    if (obs_.AutoTick())
      return Make400JSB(req_.version(), req_.keep_alive(),
                        std::string(req_static_str::badRequest), "Time in auto mode");

    // ЕСЛИ МЕТОД ОТЛИЧАЕТСЯ ОТ ПОСТ
    if (req_.method() != RequestMethod::post)
      return Make405JSB(req_.version(), req_.keep_alive(),
                        std::string(req_static_str::invalidMethod), std::string(reason_to_human::Only_POST_method_is_expected),
                        std::string(req_static_str::Allowed_POST));
    if (req_[HttpHeader::content_type] != type_content.at(Extensions::json))
    {
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument), "Invalid Content Type");
    }

    try
    {
      // ПАРСИМ ТЕЛО ЗАПРОСА
      json::value object = json::parse(req_.body());

      if (!object.as_object().at(req_static_str::timeDelta).is_number())
        throw std::invalid_argument("Not correct delta");

      // ПОЛУЧАЕМ ДЕЛЬТУ ВРЕМЕНИ
      int64_t delta = object.as_object().at(req_static_str::timeDelta).as_int64();
      double d_time = static_cast<double>(delta);
      if (std::isnan(d_time) || std::isinf(d_time) || d_time == std::numeric_limits<double>::min() || d_time == std::numeric_limits<double>::max())
        throw std::invalid_argument("Not correct delta");

      game_.ManualTick(d_time);

      json::object obj;
      return Make200JSB(req_.version(), req_.keep_alive(), json::serialize(obj));
    }
    catch (const std::exception &ex)
    {
#ifdef WHAT
      std::cout << ex.what() << std::endl;
#endif

      // ОТВЕТ В СЛУЧАЕ ОШИБКИ
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string("ERROR IN TICK"));
    }
    return {};
  }

  template <typename Requester>
  VariantResponse APIHandler<Requester>::Error()
  {
    return Make400JSB(req_.version(), req_.keep_alive(),
                      std::string(req_static_str::badRequest),
                      std::string(reason_to_human::API_Base_Check_Failed));
  }

  template <typename Requester>
  VariantResponse APIHandler<Requester>::GetHeadApi()
  {

    // ПРОВЕРЯЕМ МЕТОД ЗАПРОСА
      if (req_.method() != RequestMethod::get && req_.method() != RequestMethod::head)
        return Make405JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidMethod), std::string(reason_to_human::Invalid_method),
                          std::string(req_static_str::Allowed_GET_HEAD));
    
    // ПОЛУЧАЕМ ШАБЛОН С ГОТОВЫМИ ПОЛЯМИ
    
    StringResponse response = Template(req_.version(), req_.keep_alive());
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
        body= MakeOneMap(map);
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

    // ЕСЛИ МЕТОД ОТЛИЧАЕТСЯ ОТ ПОСТ
    if (req_.method() != RequestMethod::post)
      return Make405JSB(req_.version(), req_.keep_alive(),
                        std::string(req_static_str::invalidMethod), std::string(reason_to_human::Only_POST_method_is_expected),
                        std::string(req_static_str::Allowed_POST));
    try
    {
      // ПАРСИМ ТЕЛО ЗАПРОСА
      json::value object = json::parse(req_.body());

      // ПОЛУЧАЕМ ИМЯ ИГРОКА
      std::string u_name = std::string(object.as_object().at(std::string(req_static_str::userName)).as_string());

      // ПОЛУЧАЕМ ЖЕЛАЕМУЮ КАРТУ
      std::string map_id = std::string(object.as_object().at(std::string(req_static_str::mapId)).as_string());

      // ЕСЛИ ИМЯ ПУСТОЕ
      if (u_name.empty())
        return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                          std::string(reason_to_human::Invalid_name));

      // ЕСЛИ НЕТ КАРТЫ КОТОРУЮ ЗАПРОСИЛИ
      if (!obs_.HavingMap(map_id))
        return Make404JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::mapNotFound),
                          std::string(reason_to_human::Map_not_found));

      // ПРИСОЕДИНЯЕМСЯ К ИГРЕ И ПОЛУЧАЕМ УКАЗАТЕЛЬ НА ИГРОКА (можно было и не получать)
      auto player = game_.JoinGame(u_name, map_id);

      // ОТВЕТ
      return Make200JSB(req_.version(), req_.keep_alive(), player->GetToken(), player->GetId());
    }
    catch (const std::exception &ex)
    {

#ifdef WHAT
      std::cout << ex.what() << std::endl;
#endif
      // ОТВЕТ В СЛУЧАЕ ОШИБКИ
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string(reason_to_human::Join_game_request_parse_error));
    }
    return {};
  };

  template <typename Requester>
  std::variant<VariantResponse, std::shared_ptr<api::Player>> APIHandler<Requester>::UseAutorizationPattern()
  {
    try
    {
      // ПРОВЕРЯЕМ МЕТОД ЗАПРОСА
      if (req_.method() != RequestMethod::get && req_.method() != RequestMethod::head)
        return Make405JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidMethod), std::string(reason_to_human::Invalid_method),
                          std::string(req_static_str::Allowed_GET_HEAD));

      // ПОЛУЧАЕТ СОЖЕРЖАНИЕ ЗАГОЛОВКА АВТОРИЗАЦИИ
      std::string authorization = std::string(req_[HttpHeader::authorization]);

      // ПАРСИМ ТЕЛО ЗАГОЛОВКА АВТОРИЗАЦИИ
      std::vector<std::string> parsed_poken = ParseToken(std::move(authorization));

      // ЕСЛИ ПАРСИНГ ПРОШЕЛ НЕУДАЧНО
      if (parsed_poken.empty())
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidToken), std::string(reason_to_human::Authorization_header_is_missing));

      // ЕСЛИ ПАРСИНГ ПРОШЕЛ УДАЧНО
      std::shared_ptr<api::Player> player = obs_.FindByToken(parsed_poken[1]);

      // ЕСЛИ НЕТ ИГРОКА С ДАННЫМ ТОКЕНОМ
      if (player == nullptr)
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::unknownToken), std::string(reason_to_human::Player_token_has_not_been_found));
      // ЕСЛИ ЕСТЬ ТАКОЙ ИГРОК - ВОЗВРАЩАЕМ НА НЕГО УКАЗАТЕЛЬ
      return player;
    }
    catch (const std::exception &ex)
    {
#ifdef WHAT
      std::cout << ex.what() << std::endl;
#endif
      // ЕСЛИ В СЛУЧАЕ ПАРСИНГА ПРОИЗОШЛА ОШИБКА
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string(reason_to_human::Players_parse_error));
    }
  };

  template <typename Requester>
  VariantResponse APIHandler<Requester>::Players()
  {
    // ЗАПУСК ПАТТЕРНА ПРОВЕРКИ АВТОРИЗАЦИИ
    auto action = UseAutorizationPattern();
    // ЕСЛИ ВЕРНУЛСЯ СФОРМИРОВАНЫЙ ОТВЕТ - ВОЗВРАЩАЕМ ЕГО
    if (std::holds_alternative<VariantResponse>(action))
    {
      return std::move(std::get<VariantResponse>(action));
    }
    // ЕСЛИ В ЭКШН ВЕРНУЛСЯ УКАЗАТЕЛЬ С КОТОРОГО НУЖНЫ ДОП.ДАННЫЕ
    auto player = std::move(std::get<std::shared_ptr<api::Player>>(action));
    // ПОЛУЧАЕМ ДЖИСОН ПРЕДСТАВЛЕНИЕ СЕССИИ
    std::string tmp = obs_.GetJSONSession(player);
    // ВОЗВРАЩАЕМ ОТВЕТ
    return Make200JSB(req_.version(), req_.keep_alive(), std::move(tmp));
  }

  template <typename Requester>
  VariantResponse APIHandler<Requester>::State()
  {
     
   // ЗАПУСК ПАТТЕРНА ПРОВЕРКИ АВТОРИЗАЦИИ
    auto action = UseAutorizationPattern();
    // ЕСЛИ ВЕРНУЛСЯ СФОРМИРОВАНЫЙ ОТВЕТ - ВОЗВРАЩАЕМ ЕГО
    if (std::holds_alternative<VariantResponse>(action))
    {
      return std::move(std::get<VariantResponse>(action));
    }
    // ЕСЛИ В ЭКШН ВЕРНУЛСЯ УКАЗАТЕЛЬ С КОТОРОГО НУЖНЫ ДОП.ДАННЫЕ
    auto player = std::move(std::get<std::shared_ptr<api::Player>>(action));
    // ПОЛУЧАЕМ СЕССИЮ ИГРОКА
    std::shared_ptr<model::GameSession> sess = player->PlayersSession();
    // ВОЗВРАЩАЕМ ОТВЕТ
    return Make200State(req_.version(), req_.keep_alive(), sess);
  }

  template <typename Requester>
  VariantResponse APIHandler<Requester>::PlayerMove()
  {
    // std::cout << "I AM PLAYERMIVE" << std::endl;
    // ЕСЛИ МЕТОД ОТЛИЧАЕТСЯ ОТ ПОСТ
    if (req_.method() != RequestMethod::post)
      return Make405JSB(req_.version(), req_.keep_alive(),
                        std::string(req_static_str::invalidMethod), std::string(reason_to_human::Only_POST_method_is_expected),
                        std::string(req_static_str::Allowed_POST));

    if (req_[HttpHeader::content_type] != type_content.at(Extensions::json))
    {
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument), "Invalid Content Type");
    }

    try
    {
      // ПАРСИМ ТЕЛО ЗАПРОСА
      json::value object = json::parse(req_.body());

      // ПОЛУЧАЕМ НАПРАВЛЕНИЕ ДВИЖЕНИЯ
      std::string move = std::string(object.as_object().at(std::string(req_static_str::move)).as_string());

      // ПРОВЕРЯЕМ НАПРАВЛЕНИЕ ДВИЖЕНИЯ
      if (!IsCorrectWay(move))
        return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                          std::string("DIRECTION MUST BE U-D-L-R"));

      // ПОЛУЧАЕТ СОЖЕРЖАНИЕ ЗАГОЛОВКА АВТОРИЗАЦИИ
      std::string authorization = std::string(req_[HttpHeader::authorization]);

      // ПАРСИМ ТЕЛО ЗАГОЛОВКА АВТОРИЗАЦИИ
      std::vector<std::string> parsed_poken = ParseToken(std::move(authorization));

      // ЕСЛИ ПАРСИНГ ПРОШЕЛ НЕУДАЧНО
      if (parsed_poken.empty())
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidToken), std::string("INVALID TOKEN"));

      // ЕСЛИ ПАРСИНГ ПРОШЕЛ УДАЧНО
      std::shared_ptr<api::Player> player = obs_.FindByToken(parsed_poken[1]);

      if (player == nullptr)
        return Make401JSB(req_.version(), req_.keep_alive(),
                          std::string(req_static_str::invalidToken), std::string("YOU HAVE NO PERMISSION"));

      game_.MovePlayer(player, move);

      json::object obj;
      return Make200JSB(req_.version(), req_.keep_alive(), json::serialize(obj));
    }
    catch (const std::exception &ex)
    {
#ifdef WHAT
      std::cout << ex.what() << std::endl;
#endif

      // ОТВЕТ В СЛУЧАЕ ОШИБКИ
      return Make400JSB(req_.version(), req_.keep_alive(), std::string(req_static_str::invalidArgument),
                        std::string("ERROR IN PLAYER ACTION"));
    }
    return {};
  };
}