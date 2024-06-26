#include "game.h"
#include<mutex>

//GAME
namespace api
{
    Play::Play(const char *way_to_load) : game_(json_loader::LoadGame(way_to_load))
    {
        for (auto &map : game_.GetMaps())
        {
            sessions_[*map.GetId()] = std::make_shared<model::GameSession>(map);
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

    void Play::PrintSessions()
    {
        for (auto &&session : sessions_)
        {
            std::cout << session.first + ':' << std::endl;
            session.second->PrintDogs();
        }
    }

    std::shared_ptr<Player> Play::JoinGame(std::string name, std::string game_map)
    {
        
        if (sessions_.count(game_map) == 0)
            return nullptr;
             
            auto needed_session = sessions_.at(game_map);
            auto player= std::make_shared<Player>(name, needed_session);
            std::string token = player->GetToken();
       
            token_players_[token] = std::move(player);
            needed_session->JoinGame(token_players_.at(token)->PlayersDog());
    
            return token_players_.at(token);
        
    }

  
     bool Play::HavingSession(const std::string &session)
    {
      return (sessions_.count(session) > 0);
    };

    bool Play::HavingMap(const std::string &map)
    {
      return (game_.FindMap(model::Map::Id(map)) != nullptr);
    }

    std::string Play::GetJSONSession(std::shared_ptr<Player> player)
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

    const std::unordered_map<std::string, std::shared_ptr<model::GameSession>>& Play::GameSessions() const{
     return sessions_;
    }
  
}
  
//PLAYER
namespace api
{ 
    uint32_t Player::ids_;
    Player::Player(std::string dog, std::shared_ptr<model::GameSession> session)
    {
        my_dog_ = std::make_shared<model::Dog>(dog);
        session_ = session;
        token_ = Gen::GENERATOR.GenerateHEXToken();
        id_ = ids_++;
        my_dog_->SetRandomCoords();
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

    std::shared_ptr<model::GameSession>  Player::PlayersSession() const
    {
        return session_;
    }

    uint32_t Player::GetDogId() const
    {
        return my_dog_->GetId();
    }

};