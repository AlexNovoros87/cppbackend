#include "game.h"
#include "../req_helper/model_help.h"
#include "../tokenizer/tokenizer.h"
#include <sstream>

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
УВАЖАЕМЫЙ КОД-РЕВЬЮЕР!!!!
ГДЕ ВСТРЕЧАЕТСЯ  {Literals::ID, *(map.GetId())} И ПОДОБНОЕ,
ЭТО НЕ УКАЗАТЕЛЬ!! ЭТО ОБЪЕКТ ИЗ tagget.h ДАННЫЙ В ТЕОРИИ
*/

using namespace model;

model::DogCoordinates GenerateRandomDogPosition(const SortedSessionRoads &graph)
{
  auto randomroad = Gen::GENERATOR.RoadPositionRandom(graph);

#ifdef LOGGING
  LogRandomRoad(randomroad);
#endif

#ifdef FORCE
  return {0, 0};
#endif
  return Gen::GENERATOR.GetRandomRoadPoint(randomroad);
}

// GAME CONSTRUCTION
namespace api
{

  void Play::InitSessions()
  {
    for (auto &map : game_.GetMaps())
    {

      sessions_[*map.GetId()] = std::make_shared<model::GameSession>(map);
    }
  };

  Play::Play(model::Game game, double def_spd, bool rand, bool a_tick) : game_(std::move(game)), def_speed_(def_spd), random_(rand), auto_tick_(a_tick)
  {
    InitSessions();
    BuildSortedRoadsToRandomizer();
    BuildGraph();
  };

  void Play::BuildSortedRoadsToRandomizer()
  {

    for (auto &&session : sessions_) // ДЛЯ ВСЕХ СЕССИЙ
    {
      auto &sess_name = session.second->GetMap().GetId(); // имя данной сессии
      auto &roads = session.second->GetMap().GetRoads();

      for (auto &&road : roads)
      {
        if (road->IsHorizontal())
        {
          assert(road->GetStart().y == road->GetEnd().y);
          sorted_by_type_roads_[*sess_name][RoadOrient::HORIZONTAL].push_back(road);
        }
        else
        {
          sorted_by_type_roads_[*sess_name][RoadOrient::VERTRICAL].push_back(road);
        }
      }

      if (sorted_by_type_roads_.count(*sess_name) > 0)
      {

        if (sorted_by_type_roads_.at(*sess_name).count(RoadOrient::HORIZONTAL) > 0)
        {
          auto &roads_c = sorted_by_type_roads_.at(*sess_name).at(RoadOrient::HORIZONTAL);

          std::sort(roads_c.begin(), roads_c.end(), [](std::shared_ptr<model::Road> left, std::shared_ptr<model::Road> right)
                    { return std::min(left->GetStart().x, left->GetEnd().x) < std::min(right->GetStart().x, right->GetEnd().x); });
        }

        if (sorted_by_type_roads_.at(*sess_name).count(RoadOrient::VERTRICAL) > 0)
        {

          auto &roads_c = sorted_by_type_roads_.at(*sess_name).at(RoadOrient::VERTRICAL);

          std::sort(roads_c.begin(), roads_c.end(), [](std::shared_ptr<model::Road> left, std::shared_ptr<model::Road> right)
                    { return std::min(left->GetStart().y, left->GetEnd().y) < std::min(right->GetStart().y, right->GetEnd().y); });
        }
      }
    }
  };

  void Play::BuildGraph()
  {

    for (const auto &session : sessions_) // ДЛЯ ВСЕХ СЕССИЙ
    {
      auto &sess_name = session.second->GetMap().GetId(); // имя данной сессии
      auto &roads = session.second->GetMap().GetRoads();

      for (auto road : roads)
      {
        if (road->IsHorizontal())
        {
          assert(road->GetStart().y == road->GetEnd().y);
          int X_min = std::min(road->GetStart().x, road->GetEnd().x);
          int X_max = std::max(road->GetStart().x, road->GetEnd().x);
          int Y = road->GetStart().y;

          for (int X = X_min; X <= X_max; ++X)
          {
            graph_[*sess_name][X][Y].push_back(road);
          }
        }
        else
        {
          assert(road->GetStart().x == road->GetEnd().x);
          int Y_min = std::min(road->GetStart().y, road->GetEnd().y);
          int Y_max = std::max(road->GetStart().y, road->GetEnd().y);
          int X = road->GetStart().x;

          for (int Y = Y_min; Y <= Y_max; ++Y)
          {
            graph_[*sess_name][X][Y].push_back(road);
          }
        }
      }
    }
  }

  void Play::SetupLootManager(double period, double prob)
  {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double, std::milli>(period));
    looter_ = std::make_shared<loot_gen::LootGenerator>(ms, prob);
  }

}

// GAME TICK
namespace api
{

  void DoEvent(collision_detector::GatheringEventMOD &event, GameSession &sess)
  {
    //ЕСЛИ КОЛИЗИЯ С ОФИСОМ
    if (event.event_kind == collision_detector::ItemType::OFFICE)
    {
      //СБРОСИТЬ ЛУТ
      event.initiator->ThrowLoot();
    }
    //ЕСЛИ КОЛИЗИЯ С ЛУТОМ
    else
    {
      //ПОЛУЧАЕМ СПИСОК ЛУТА НА СЕССИИ
      auto &lost_loot = sess.GetLootListNotConst();
      //ПОЛУЧАЕМ МАКС РАЗМЕР СУМКИ НА СЕССИИ
      size_t bag_capacity_on_level = sess.GetMap().GetBagCapacity();
      //ЕСЛИ ХВАТАЕТ МЕСТА В СУМКЕ
      if (event.initiator->GetLootBag().size() < bag_capacity_on_level)
      {
        //ПОДНЯТЬ ПРЕДМЕТ
        event.initiator->PickUpLoot(std::move(lost_loot.at(event.where)));
        //СТЕРЕТЬ ПРЕДМЕТ ИЗ БАЗЫ ПОТЕРЯННЫХ ВЕЩЕЙ 
        lost_loot.erase(event.where);
      }
      
    }
  }

  void UpdateEventChronology(GameSession &sess, std::vector<collision_detector::GathererDog> gathers)
  {
    // СОЗДАТЬ НАСЛЕДНИКА
    collision_detector::Collizer situation_queuer_;
    //ПЕРЕДАТЬ СОБИРАТЕЛЕЙ
    situation_queuer_.AddGatherers(std::move(gathers));

    //ДОБАВИТЬ ВЕСЬ ЛУТ С КАРТЫ
    for (auto &&loot : sess.GetLootList())
    {
      situation_queuer_.AddLootObject(loot.first);
    }

    //ДОБАВИТЬ ВСЕ ОФИСЫ
    for (auto office : sess.GetMap().GetOffices())
    {
      situation_queuer_.AddOffice(office);
    }
    
    //ПОСТРОИТЬ ХРОНОЛОГИЮ СОБЫТИЙ
    auto chronology = collision_detector::FindGatherEventsOrder(situation_queuer_);

    //ВЫПОЛНИТЬ СОБЫТИЯ
    for(auto& event : chronology){
      DoEvent(event, sess);
    }
  }

  void Play::ManualTick(std::chrono::milliseconds delta_t)
  {
    double delta = delta_t.count();
    ManualTick(delta);
  };

  collision_detector::GathererDog MakeGatherDog(const DogCoordinates &old_coord, const DogCoordinates &coordinates,
                                                std::shared_ptr<model::Dog> dog)
  {
    collision_detector::GathererDog gather;
    gather.start_pos = {old_coord.x, old_coord.y};
    gather.end_pos = {coordinates.x, coordinates.y};
    gather.width = collision_detector::PlayConditions::player_weigth;
    gather.initiator = dog;
    return gather;
  }

  std::vector<collision_detector::GathererDog> Play::UpdateDogPositionsAndGetGathers(GameSession &sess, double delta_t)
  {

    auto &dogs = sess.GetDogs();
    const auto &session_graph = Graph().at(*sess.GetMap().GetId());

    std::vector<collision_detector::GathererDog> gathers;
    // ДЛЯ КАЖДОЙ СОБАКИ
    for (auto &dog : dogs)
    {
      // СТАРЫЕ КООРДИНАТЫ СОБАКИ
      DogCoordinates old_coord = dog.second->GetDogCoord();

      // GetNewCoordinates(const GameSession& game, std::shared_ptr<Dog> dog , const GameGraph& graph , uint64_t delta_t);
      // ПОЛУЧАЕМ НОВЫЕ КООРДИНАТЫ И ЛИМИТЫ ХОДОВ
      auto [coordinates, limits] = model::GetNewCoordinatesAndLimits(sess, dog.second, session_graph, delta_t);

      // ДЕЛАЕМ СОБИРАТЕЛЯ
      collision_detector::GathererDog gather = MakeGatherDog(old_coord, coordinates, dog.second);

      // ДОБАВЛЯЕМ СОБИРАТЕЛЯ В ВЕКТОР
      gathers.push_back(std::move(gather));

      // ОБНОВЛЯЕМ СОСТОЯНИЕ ТЕКУЩЕЙ СОБАКИ
      dog.second->IsNeededTStopDog(coordinates, limits);
      dog.second->SetDogCoordinates(coordinates);
    }
    return gathers;
  };

  void Play::UpdateLootPositions(GameSession &sess, double delta_t)
  {
    auto &dogs = sess.GetDogs();
    std::chrono::milliseconds ms = ConvertDoubleToMS(delta_t);
    auto &name = sess.GetMap().GetName();
    auto &sort_roads = sorted_by_type_roads_.at(*sess.GetMap().GetId());
    size_t dogs_on_map = dogs.size();
    size_t loot_on_map = sess.GetLootList().size();
    size_t loot_needed = looter_->Generate(ms, loot_on_map, dogs_on_map);

    // Gen::GENERATOR.GenerateLoot генерирует вектор пар "координаты на карте - порядковый номер лута в контейнере"
    for (auto [coord, order_num] : Gen::GENERATOR.GenerateLoot(sort_roads, loot_needed, name))
    {
      sess.AddLoot(coord, order_num);
    }
  };

  void Play::ManualTick(double delta_t)
  {
    assert(TestLooterisHere() == true);
    auto &sessions = GameSessions();
    for (auto &sess_p : sessions)
    {
      auto &sess = *sess_p.second;
      UpdateLootPositions(sess, delta_t);
      // ОБНОВЛЯЕМ ПОЗИЦИИ СОБАК И ПОЛУЧАЕМ ВЕКТОР СОБИРАТЕЛЕЙ
      auto gathers = UpdateDogPositionsAndGetGathers(sess, delta_t);

      UpdateEventChronology(sess, std::move(gathers));
    
    }
  }

}

// GAME MOVE && JOIN
namespace api
{
  void Play::MovePlayer(std::shared_ptr<Player> player, const std::string &move)
  {
    double sess_spd = player->PlayersSession()->GetSessionSpeed();
    auto dog = player->PlayersDog();
    auto dogspeed = model::GetDogSpeedByDrection(move, sess_spd);

#ifdef LOGGING
    model::LogWas("CHANGE DIRECTION", dog);
#endif
    dog->SetDogSpeedAndDirection(std::move(dogspeed), model::string_nswe.at(move));
#ifdef LOGGING
    model::LogNow("CHANGE DIRECTION", dog);
#endif
  };

  std::shared_ptr<Player> Play::JoinGame(std::string name, std::string game_map)
  {

    if (sessions_.count(game_map) == 0)
      return nullptr;

    auto needed_session = sessions_.at(game_map);
    SortedSessionRoads &sorted_roads = sorted_by_type_roads_.at(game_map);

    auto player = std::make_shared<Player>(name, needed_session, sorted_roads, random_);
    std::string token = player->GetToken();
    token_players_[token] = std::move(player);
    needed_session->JoinGame(token_players_.at(token)->PlayersDog());
    return token_players_.at(token);
  }

  void Play::ChangeMapSpeed(const std::string &map, double speed)
  {
    if (game_.FindMap(model::Map::Id(map)) != nullptr)
    {
      maps_speeds_[sessions_.at(map)] = speed;
    }
  }

}

// GAME CONST GET METHODS
namespace api
{

  std::shared_ptr<model::GameSession> Play::GetSession(const std::string &name) const
  {
    if (sessions_.count(name) > 0)
    {
      return sessions_.at(name);
    }
    return nullptr;
  }

  void Play::PrintSessionRoads() const /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  {
    for (auto session : sorted_by_type_roads_)
    {
      std::cout << "Session name: " << session.first << std::endl
                << "HORIZONTAL:" << std::endl;
      if (session.second.count(RoadOrient::HORIZONTAL))
      {
        for (auto &&road : session.second.at(RoadOrient::HORIZONTAL))
        {
          std::cout << *road << std::endl;
        }
      };
      std::cout << "VERTICAL:" << std::endl;
      if (session.second.count(RoadOrient::VERTRICAL))
      {
        for (auto &&road : session.second.at(RoadOrient::VERTRICAL))
        {
          std::cout << *road << std::endl;
        }
      };
      std::cout << "------------------------------------------------------------" << std::endl
                << std::endl
                << std::endl;
    }
  }

  const model::Game &Play::Game() const
  {
    return game_;
  }

  std::shared_ptr<Player> Play::FindByToken(const std::string &token) const
  {
    if (token_players_.count(token) > 0)
      return token_players_.at(token);
    return nullptr;
  }

  bool Play::HavingSession(const std::string &session) const
  {
    return (sessions_.count(session) > 0);
  };

  bool Play::HavingMap(const std::string &map) const
  {
    return (game_.FindMap(model::Map::Id(map)) != nullptr);
  }

  std::string Play::GetJSONSession(std::shared_ptr<Player> player) const
  {
    if (token_players_.count(player->GetToken()) == 0)
      return {};

    auto dogs = player->PlayersSession()->GetDogs();
    boost::json::value obj{{"w", ""}};
    for (const auto &dog : dogs)
    {
      obj.as_object()[std::to_string(dog.second->GetId())] = dog.second->GetName();
    };
    obj.as_object().erase("w");
    return (boost::json::serialize(obj));
  };

  void Play::PrintSessions() const
  {
    for (auto &&session : sessions_)
    {
      std::cout << session.first << std::endl;
      session.second->PrintDogs();
    }
  }

  const std::unordered_map<std::string, std::shared_ptr<model::GameSession>> &Play::GameSessions() const
  {
    return sessions_;
  }

}

// PLAYER
namespace api
{
  uint32_t Player::ids_;
  Player::Player(std::string dog, std::shared_ptr<model::GameSession> session, const SortedSessionRoads &s_roads, bool rand_dog) : sorted_roads_(s_roads)
  {
    my_dog_ = std::make_shared<model::Dog>(dog);
    session_ = session;
    token_ = Gen::GENERATOR.GenerateHEXToken();
    id_ = ids_++;
    DogCoordinates rand_coord;

    if (rand_dog)
    {
      rand_coord = GenerateRandomDogPosition(sorted_roads_);
    }
    else
    {
      const auto &roads = session_->GetMap().GetRoads();
      if (roads.empty())
      {
        throw std::logic_error("No roads in session");
      }
      auto st_crd = roads[0]->GetStart();
      rand_coord = {static_cast<double>(st_crd.x), static_cast<double>(st_crd.y)};
    }
    my_dog_->SetDogCoordinates(rand_coord);

#ifdef LOGGING
    LogToCURL(dog, token_, coord);
#endif
  }

  const std::string &Player::GetToken() const
  {
    return token_;
  }

  uint32_t Player::GetId() const
  {
    return id_;
  }

  std::shared_ptr<model::Dog> Player::PlayersDog() const
  {
    return my_dog_;
  }

  std::shared_ptr<model::GameSession> Player::PlayersSession() const
  {
    return session_;
  }

  uint32_t Player::GetDogId() const
  {
    return my_dog_->GetId();
  }

};