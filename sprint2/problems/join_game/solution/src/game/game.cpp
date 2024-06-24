#include "game.h"
#include<mutex>

namespace api
{
    uint32_t Player::ids_;
   

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

    Player::Player(std::string dog, std::shared_ptr<model::GameSession> session)
    {
        my_dog_ = std::make_shared<model::Dog>(dog);
        session_ = session;
        token_ = tokenizer_.GenerateHEXToken();
        id_ = ids_++;
    }

    const std::string &Player::GetToken() const
    {
        return token_;
    }

    const PlayerTokens &Player::UseTokenizer()
    {
        return tokenizer_;
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