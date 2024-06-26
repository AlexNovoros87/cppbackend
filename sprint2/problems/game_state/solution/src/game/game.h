#pragma once

#include <unordered_map>
#include "../json/json_loader.h"
#include "../model/game_dog_session.h"
#include "../tokenizer/tokenizer.h"

#define SECTORS

namespace api
{

  class Player
  {
  public:
    Player(std::string dog, std::shared_ptr<model::GameSession> session);
    const std::string &GetToken() const;
    uint32_t GetId() const;
    std::shared_ptr<model::Dog> PlayersDog() const;
    std::shared_ptr<model::GameSession> PlayersSession() const;
    uint32_t GetDogId() const;

  private:
    std::shared_ptr<model::Dog> my_dog_;
    std::shared_ptr<model::GameSession> session_;
    std::string token_;
    static uint32_t ids_;
    uint32_t id_;
  };

  class Play
  {
  public:
    Play(const char *way_to_load);
    std::shared_ptr<Player> JoinGame(std::string name, std::string game_map);
    const model::Game &Game() const;
    std::shared_ptr<Player> FindByToken(const std::string &token) const;
    void PrintSessions();
    bool HavingSession(const std::string &session);
    bool HavingMap(const std::string &map);
    std::string GetJSONSession(std::shared_ptr<Player> player);
    void PrintSessions() const;
    const std::unordered_map<std::string, std::shared_ptr<model::GameSession>>& GameSessions() const;
  
  
  private:
    model::Game game_;
    std::unordered_map<std::string, std::shared_ptr<Player>> token_players_;
    std::unordered_map<std::string, std::shared_ptr<model::GameSession>> sessions_;
  };

}