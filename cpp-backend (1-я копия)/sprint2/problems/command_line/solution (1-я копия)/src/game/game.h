#pragma once

#include <unordered_map>
#include <filesystem>

#include "../model/game_dog_session.h"
using GameGraph = std::unordered_map<std::string, std::unordered_map<double , std::unordered_map<double , std::vector<std::shared_ptr<model::Road>>>>>;
using SessionGraph = std::unordered_map<double , std::unordered_map<double, std::vector<std::shared_ptr<model::Road>>>>;
using  SortedSessionRoads = std::unordered_map<model::RoadOrient,std::vector<std::shared_ptr<model::Road>>>;
namespace api
{
  using namespace model;
  class Player
  {
  public:
    Player(std::string dog, std::shared_ptr<model::GameSession> session, const SortedSessionRoads& roads , bool rand_dog) ;
    const std::string &GetToken() const;
    uint32_t GetId() const;
    std::shared_ptr<model::Dog> PlayersDog() const;
    std::shared_ptr<model::GameSession> PlayersSession() const;
    uint32_t GetDogId() const;

  private:
    const SortedSessionRoads& sorted_roads_;
    std::shared_ptr<model::Dog> my_dog_;
    std::shared_ptr<model::GameSession> session_;
    std::string token_;
    static uint32_t ids_;
    uint32_t id_;
    bool need_randomize_dog_;
  };

  class Play
  {

  public:
    
    Play(model::Game game, double def_spd, bool rand, bool a_tick);
    
    std::shared_ptr<Player> JoinGame(std::string name, std::string game_map);
    std::shared_ptr<Player> FindByToken(const std::string &token) const;
    bool HavingSession(const std::string &session) const;
    bool HavingMap(const std::string &map)const;
    
    std::string GetJSONSession(std::shared_ptr<Player> player) const;
    const std::unordered_map<std::string, std::shared_ptr<model::GameSession>> &GameSessions() const;
    double GetDefaultSpeed() const { return def_speed_; }
    const model::Game &Game() const;
    const GameGraph& Graph() const {return graph_;}
    std::shared_ptr<model::GameSession> GetSession(const std::string &name) const;
    
    void PrintSessions() const;
    void PrintSessionRoads() const;
    void ChangeMapSpeed(const std::string &map, double speed);
    void ManualTick(double delta_t);
    void MovePlayer(std::shared_ptr<Player> player , const std::string& move);
    bool AutoTick() const {return auto_tick_;}
    bool NeedRandom() const {return random_;}

  private:
    void InitSessions();
    void BuildGraph();
    void BuildSortedRoadsToRandomizer();
    
    double def_speed_ = std::numeric_limits<double>::min();
    const model::Game game_;
    bool random_;
    bool auto_tick_;
    
    //sorted_by_type_roads_ эту структуру можно было и не делать - но с ней удобно печатать 
    //дороги по сессиям и отдельно горизонтальные и отдельно вертикальные и с ней работает
    //рандомайзер  можно былобы передавать размер кол-во горизонтальных и ко-ло вертикальных
    //но передавая эту структуру можно отслеживать правильную ли дорогу выбрало и.т.д. 
    std::unordered_map<std::string,std::unordered_map<RoadOrient,std::vector<std::shared_ptr<Road>>>> sorted_by_type_roads_;
    std::unordered_map<std::shared_ptr<model::GameSession>, double> maps_speeds_;
    std::unordered_map<std::string, std::shared_ptr<Player>> token_players_;
    std::unordered_map<std::string, std::shared_ptr<model::GameSession>> sessions_;
    GameGraph graph_;
  };

}