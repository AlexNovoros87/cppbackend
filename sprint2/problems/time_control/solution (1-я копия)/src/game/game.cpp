#include "game.h"
#include "../json/json_loader.h"
using namespace model;

#ifdef LOGGING
#include <fstream>
std::ofstream fs("/home/user/_____logdog.txt", std::ios::trunc);
#endif

model::DogCoordinates GenerateRandomDogPosition(const SortedSessionRoads &graph)
{
  auto randomroad = Gen::GENERATOR.RoadPositionRandom(graph);

#ifdef LOGGING
  // fs << "Roads\n";
  // for (auto &&type : graph)
  // {
  //    for(auto&&road: type.second){
  //    fs << *road << std::endl;
  //    }
  // }
  fs << " RandomRoad: " << *randomroad << std::endl;
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

  Play::Play(model::Game game) : game_(std::move(game))
  {
    InitSessions();
    BuildSortedRoadsToRandomizer();
    BuildGraph();
  };

  Play::Play(model::Game game, double def_spd) : game_(std::move(game)), def_speed_(def_spd)
  {
    InitSessions();
    BuildSortedRoadsToRandomizer();
    BuildGraph();
    def_speed_ = def_spd;
  };

  void Play::BuildSortedRoadsToRandomizer()
  {

    for (auto &&session : sessions_) // ДЛЯ ВСЕХ СЕССИЙ
    {
      auto &sess_name = session.second->GetMap().GetId(); // имя данной сессии
      auto &roads = session.second->GetMap().GetRoads();

      for (auto road : roads)
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
          auto &roads = sorted_by_type_roads_.at(*sess_name).at(RoadOrient::HORIZONTAL);

          std::sort(roads.begin(), roads.end(), [](std::shared_ptr<model::Road> left, std::shared_ptr<model::Road> right)
                    { return std::min(left->GetStart().x, left->GetEnd().x) < std::min(right->GetStart().x, right->GetEnd().x); });
        }

        if (sorted_by_type_roads_.at(*sess_name).count(RoadOrient::VERTRICAL) > 0)
        {

          auto &roads = sorted_by_type_roads_.at(*sess_name).at(RoadOrient::VERTRICAL);

          std::sort(roads.begin(), roads.end(), [](std::shared_ptr<model::Road> left, std::shared_ptr<model::Road> right)
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

}

// GAME
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

  void Play::ChangeMapSpeed(const std::string &map, double speed)
  {
    if (game_.FindMap(model::Map::Id(map)) != nullptr)
    {
      maps_speeds_[sessions_.at(map)] = speed;
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

  std::shared_ptr<Player> Play::JoinGame(std::string name, std::string game_map)
  {

    if (sessions_.count(game_map) == 0)
      return nullptr;

    auto needed_session = sessions_.at(game_map);
    SortedSessionRoads &sorted_roads = sorted_by_type_roads_.at(game_map);

    auto player = std::make_shared<Player>(name, needed_session, sorted_roads);
    std::string token = player->GetToken();

    token_players_[token] = std::move(player);
    needed_session->JoinGame(token_players_.at(token)->PlayersDog());

    return token_players_.at(token);
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
    return (json::serialize(obj));
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
  Player::Player(std::string dog, std::shared_ptr<model::GameSession> session, const SortedSessionRoads &s_roads) : sorted_roads_(s_roads)
  {
    my_dog_ = std::make_shared<model::Dog>(dog);
    session_ = session;
    token_ = Gen::GENERATOR.GenerateHEXToken();
    auto rand_coord = GenerateRandomDogPosition(sorted_roads_);
    id_ = ids_++;
#ifdef LOGGING

    fs << "curl -X POST -H \"Content-Type: application/json\" -d '{\"timeDelta\": 10}' 127.0.0.1:8080/api/v1/game/tick" << std::endl;
    fs << "curl -H \"Authorization: Bearer " << token_ << "\" 127.0.0.1:8080/api/v1/game/players" << std::endl;
    fs << "curl -H \"Authorization: Bearer " << token_ << " \" 127.0.0.1:8080/api/v1/game/state" << std::endl;

    fs << "curl -X POST -H \"Content-Type: application/json\" -H \"Authorization: Bearer " << token_
       << "\" -d '{\"move\": \"R\"}' 127.0.0.1:8080/api/v1/game/player/action" << std::endl
       << std::endl
       << std::endl;
    fs << "Name: " << dog << "|  Token: " << token_ << "|  Random pos: " << rand_coord << std::endl;
    fs << "============================================================================\n\n\n";
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