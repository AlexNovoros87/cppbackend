#include "game.h"
#include "../req_helper/model_help.h"
#include "../tokenizer/tokenizer.h"
#include <sstream>
#include <chrono>

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
УВАЖАЕМЫЙ КОД-РЕВЬЮЕР!!!!
ГДЕ ВСТРЕЧАЕТСЯ  {Literals::ID, *(map.GetId())} И ПОДОБНОЕ,
ЭТО НЕ УКАЗАТЕЛЬ!! ЭТО ОБЪЕКТ ИЗ tagget.h ДАННЫЙ В ТЕОРИИ
*/

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

  Play::Play(model::Game game, double def_spd, bool rand, bool a_tick) : game_(std::move(game)), def_speed_(def_spd), random_(rand), auto_tick_(a_tick),
                                                                         serv_time_{std::chrono::system_clock::now()}
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

// GAME TICK HELP
namespace api
{
  void DoEvent(collision_detector::GatheringEventMOD &event, GameSession &sess)
  {
    // ЕСЛИ КОЛИЗИЯ С ОФИСОМ
    if (event.event_kind == collision_detector::ItemType::OFFICE)
    {
      // ПОЛУЧАЕМ СУМКУ ИНИЦИАТОРА
      auto &lootbag = event.initiator->GetLootBag();
      for (auto &&loot : lootbag)
      {
        // ПОЛУЧАЕМ ЦЕНУ С ПРЕДМЕТА С ФРОНЕНД ОБЪЕКТА
        int scores = frontend::FrontEndLoot::GetPriceScore(sess.GetMap().GetName(), loot.type);
        // НАЧИСЛЯЕМ ПРЕМИЮ СОБАКЕ.
        event.initiator->TakeScores(scores);
      }

      // СБРОСИТЬ ЛУТ
      event.initiator->ThrowLoot();
    }
    // ЕСЛИ КОЛИЗИЯ С ЛУТОМ
    else
    {
      // ПОЛУЧАЕМ СПИСОК ЛУТА НА СЕССИИ
      auto &lost_loot = sess.GetLootListNotConst();
      // ПОЛУЧАЕМ МАКС РАЗМЕР СУМКИ НА СЕССИИ
      size_t bag_capacity_on_level = sess.GetMap().GetBagCapacity();
      // ЕСЛИ ХВАТАЕТ МЕСТА В СУМКЕ

      if (event.initiator->GetLootBag().size() < bag_capacity_on_level)
      {

        try
        {
          if(lost_loot.count(event.where)){
          // ПОДНЯТЬ ПРЕДМЕТ
          event.initiator->PickUpLoot(std::move(lost_loot.at(event.where)));
          // СТЕРЕТЬ ПРЕДМЕТ ИЗ БАЗЫ ПОТЕРЯННЫХ ВЕЩЕЙ
          lost_loot.erase(event.where);
          }
        }
        catch (const std::exception &ex)
        {
          throw std::logic_error("IN PICKUP LOOT");
        }
      }
    }
  }

  void UpdateEventChronology(GameSession &sess, std::vector<collision_detector::GathererDog> gathers)
  {
    // СОЗДАТЬ НАСЛЕДНИКА
    collision_detector::Collizer situation_queuer_;
    // ПЕРЕДАТЬ СОБИРАТЕЛЕЙ
    try
    {

      situation_queuer_.AddGatherers(std::move(gathers));

      // ДОБАВИТЬ ВЕСЬ ЛУТ С КАРТЫ
      for (auto &&loot : sess.GetLootList())
      {
        situation_queuer_.AddLootObject(loot.first);
      }

      // ДОБАВИТЬ ВСЕ ОФИСЫ
      for (auto office : sess.GetMap().GetOffices())
      {
        situation_queuer_.AddOffice(office);
      }
    }
    catch (std::exception &ex)
    {
      throw std::logic_error("UpdateEventChronology PART1");
    }

    std::vector<collision_detector::GatheringEventMOD> chronology;

    try
    {
      // ПОСТРОИТЬ ХРОНОЛОГИЮ СОБЫТИЙ
      chronology = collision_detector::FindGatherEventsOrder(situation_queuer_);
    }
    catch (std::exception &ex)
    {
      throw std::logic_error("ERROR IN FindGatherEventsOrder");
    }

    try
    {
      // ВЫПОЛНИТЬ СОБЫТИЯ
      for (auto &event : chronology)
      {
        DoEvent(event, sess);
      }
    }
    catch (std::exception &ex)
    {
      throw std::logic_error("ERROR IN DoEvent");
    }
  }

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

}

namespace api
{

  void Play::ManualTick(std::chrono::milliseconds delta_t)
  {
    double delta = delta_t.count();
    ManualTick(delta);
  };

  std::vector<collision_detector::GathererDog> Play::UpdateDogPositionsAndGetGathers(GameSession &sess, double delta_t)
  {

    auto &dogs = sess.GetDogs();
    const auto &session_graph = graph_.at(*sess.GetMap().GetId());

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
      bool is_dog_stoped_now = dog.second->IsNeededTStopDog(coordinates, limits);

      if (is_dog_stoped_now)
      {
        stop_signal_(*sess.GetMap().GetId(), dog.second->GetId(), serv_time_);
      }

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

    // std::chrono::time_point b = std::chrono::system_clock::now(); 
    
    std::chrono::milliseconds ms = ConvertDoubleToMS(delta_t);
    // ОБНОВЛЯЕМ ТОЧКУ ОБНОВЛЕНИЯ ИГРОВЫХ ЧАСОВ
    serv_time_ += ms;

    // ПРОВЕРЯЕМ НА ПРОСТОЙ И ИСКЛЮЧАЕМ БЕЗДЕЙСТВУЮЩИХ
    try
    {
       KickUnuseful(ms);
    }
    catch (std::exception &ex)
    {
      throw std::logic_error("KIKUNUSEFUL");
    }

    //assert(looter_ != nullptr);
    auto &sessions = sessions_;
    for (auto &sess_p : sessions)
    {
      auto &sess = *sess_p.second;

      UpdateLootPositions(sess, delta_t);

      // ОБНОВЛЯЕМ ПОЗИЦИИ СОБАК И ПОЛУЧАЕМ ВЕКТОР СОБИРАТЕЛЕЙ

      std::vector<collision_detector::GathererDog> gathers;

      gathers = UpdateDogPositionsAndGetGathers(sess, delta_t);

      UpdateEventChronology(sess, std::move(gathers));
    }

    // СИГНАЛ НА ЗАПИСЬ
    tick_signal_(ms);
  
  //  std::chrono::time_point e = std::chrono::system_clock::now(); 
  //  auto i = std::chrono::duration_cast<std::chrono::milliseconds>(e-b).count();
  //   std::cout<< std::chrono::duration_cast<std::chrono::milliseconds>(e-b).count()<<std::endl;
  
  
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

    if (move == "")
    {
      stop_signal_(*player->PlayersSession()->GetMap().GetId(), dog->GetId(), serv_time_);
    }
    else
    {
      move_signal_(*player->PlayersSession()->GetMap().GetId(), dog->GetId());
    }

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

    // ПРИСОЕДИНЯЕМСЯ К СЧЕТЧИКУ ВРЕМЕНИ ПРОСТОЯ
    TCC::JoinToTimeObserve(*needed_session->GetMap().GetId(), token_players_.at(token)->PlayersDog()->GetId(), token_players_.at(token)->GetToken(), serv_time_);

    return token_players_.at(token);
  }

}

// GAME
namespace api
{
  const model::Game &Play::Game() const
  {
    return game_;
  }

  // Добавляем обработчик сигнала tick и возвращаем объект connection для управления,
  // при помощи которого можно отписаться от сигнала
  sig::connection Play::SubsribeFunctionToGameSignal(const Play::TickSignal::slot_type &handler)
  {
    return tick_signal_.connect(handler);
  }

}
