#pragma once

#include <boost/signals2.hpp>
#include "../tcc/timer_controller.h"
#include "../loot_gen/loot_generator.h"
#include "collizer.h"

namespace sig = boost::signals2;
using milliseconds = std::chrono::milliseconds;
using namespace std::literals;

using GameGraph = std::unordered_map<std::string, std::unordered_map<double, std::unordered_map<double, std::vector<std::shared_ptr<model::Road>>>>>;
using SessionGraph = std::unordered_map<double, std::unordered_map<double, std::vector<std::shared_ptr<model::Road>>>>;
using SortedSessionRoads = std::unordered_map<model::RoadOrient, std::vector<std::shared_ptr<model::Road>>>;

namespace serialization
{
  class ArchivePlayer;
  class DearchivePlay;
}
namespace api
{
  // ДЕКЛАРАЦИИ!!!!!!!!!!!!!!!
  using namespace model;
  class PlayObserver;

  ///////////////////////////

  class Player
  {
  public:
    Player(std::string dog, std::shared_ptr<model::GameSession> session,
           const SortedSessionRoads &s_roads, bool rand_dog);
    const std::string &GetToken() const;
    uint32_t GetId() const;
    std::shared_ptr<model::Dog> PlayersDog() const;
    std::shared_ptr<model::GameSession> PlayersSession() const;
    uint32_t GetDogId() const;
    static uint32_t GetPlayersCount();
    bool NeedRandom() const;

  private:
    friend class serialization::ArchivePlayer;
    std::shared_ptr<model::GameSession> session_;
    const SortedSessionRoads &sorted_roads_;

    bool need_randomize_dog_;
    std::shared_ptr<model::Dog> my_dog_;
    std::string token_;
    uint32_t id_;
    static uint32_t ids_;

    Player(std::shared_ptr<model::GameSession> session,
           const SortedSessionRoads &s_roads,
           bool rand_dog,
           std::shared_ptr<model::Dog> sobaka,
           std::string tok,
           uint32_t id

    );
    static void SetStaticPlayersCount(uint32_t plc);
  };

  class Play
  {

  public:
    using TickSignal = sig::signal<void(milliseconds)>;
    using MovingSignal = sig::signal<void(const std::string&, size_t)>;
    using StopSignal = sig::signal<void(const std::string&, size_t,const std::chrono::system_clock::time_point&)>;

    
    Play(model::Game game, double def_spd, bool rand, bool a_tick);
    const model::Game &Game() const;
    // ПРИСОЕДИНИТЬСЯ К ИГРЕ
    std::shared_ptr<Player> JoinGame(std::string name, std::string game_map);
    // ДАТЬ ХОД ИГРОВФМ ЧАСАМНА ЕД.ВРЕМЕНИ
    void ManualTick(double delta_t);
    void ManualTick(std::chrono::milliseconds delta_t);
    // ЗАДАТЬ НАПРАВЛЕНИЕ ИГРОКУ
    void MovePlayer(std::shared_ptr<Player> player, const std::string &move);
    // УСТАНОВКА ЛУТ-МЕНЕДЖЕРА ДЛЯ ГЕНЕРАЦИИ ЛУТА НА КАРТЕ
    void SetupLootManager(double period, double prob);

    // Добавляем обработчик сигнала tick и возвращаем объект connection для управления,
    // при помощи которого можно отписаться от сигнала
    sig::connection SubsribeFunctionToGameSignal(const TickSignal::slot_type &handler);
    
    sig::connection SubscribeMoving(const MovingSignal::slot_type& handler){
          return move_signal_.connect(handler);
    };

    sig::connection SubscribeStop(const StopSignal::slot_type& handler){
          return stop_signal_.connect(handler);
    };



  private:
    friend class PlayObserver;
    friend class serialization::DearchivePlay;
    // ИНИЦИАЛИЗИРУЕТ СЕССИИ
    void InitSessions();
    // СТРОИТ ГРАФ
    void BuildGraph();
    // СТРОИТ ОТСОРТИРОВАННЫЕ ДОРОГИ
    void BuildSortedRoadsToRandomizer();
    // ОБНОВЛЯЕТ ПОЗИЦИИ СОБАК И ЗАОДО СТРОИТ ВЕКТОР СОБИРАТЕЛЕЙ ДЛЯ КОЛЛИЗИЙ
    std::vector<collision_detector::GathererDog> UpdateDogPositionsAndGetGathers(GameSession &sess, double delta_t);
    // ОБНОВЛЯЕТ ЛУТ НА КАРТАХ ИГРЫ
    void UpdateLootPositions(GameSession &sess, double delta_t);

    void KickUnuseful(std::chrono::milliseconds delta_t)
    {
      std::vector<IdTWithDurSc> to_sql_members;

      // ПРИ ОБХОДЕ СЛОВАРЯ ИГРОКОВ УДАЛЯТЬ ИГРОКОВ НЕ БУДЕМ
      // ТОЛЬКО СОБАК ИЗ СЕССИЙ
      for (auto &pair : token_players_)
      {
        // ИГРОК, СЕССИЯ, СОБАКА
        std::shared_ptr<Player> player = pair.second;
        std::shared_ptr<GameSession> pl_sess = player->PlayersSession();
        std::shared_ptr<Dog> pl_dog = player->PlayersDog();

        // ИД СЕССИИ И СОБАКИ
        auto &sess_id = *pl_sess->GetMap().GetId();
        size_t dog_id = pl_dog->GetId();

          if(pl_dog->GetDodSpd().IsNoSpeed()){
           //ЕСЛИ ПРИШЛО ВРЕМЯ УДАЛЯТЬ ЕЕ ИЗ ИГРЫ
          if (auto remove = TCC::StructToDeleteOrNull(sess_id, dog_id,serv_time_,delta_t))
          {
            (*remove).score = pl_dog->Score();
            (*remove).name = pl_dog ->GrabName();
            pl_sess->LeaveGame(dog_id);
            to_sql_members.push_back(std::move(*remove));
          }
        }
        
        
        
        }
      //УДАЛЯЕМ ИГРОКОВ
      for (auto &&member : to_sql_members)
      {
        token_players_.erase(member.params.token);
      }
    
      //НАПРАВЛЯЕМ НА SQL- запись вышедших
      //почти в 99.99% это будет 1 игрок
      sql::SQL::RecordTOSQLKicked(std::move(to_sql_members)); 
    
    
    };

    const model::Game game_;
    double def_speed_ = std::numeric_limits<double>::min();
    bool random_;
    bool auto_tick_;
    std::chrono::time_point<std::chrono::system_clock> serv_time_;
    
    
    TickSignal tick_signal_;
    MovingSignal move_signal_;
    StopSignal stop_signal_;

    // sorted_by_type_roads_ эту структуру можно было и не делать - но с ней удобно печатать
    // дороги по сессиям и отдельно горизонтальные и отдельно вертикальные и с ней работает
    // рандомайзер  можно былобы передавать размер кол-во горизонтальных и ко-ло вертикальных
    // но передавая эту структуру можно отслеживать правильно ли дорогу выбрало и.т.д.
    std::unordered_map<std::string, std::unordered_map<RoadOrient, std::vector<std::shared_ptr<Road>>>> sorted_by_type_roads_;
    // ПЛЕЕРЫ ПО ТОКЕНАМ
    std::unordered_map<std::string, std::shared_ptr<Player>> token_players_;
    // ИГРОСЕССИИ
    std::unordered_map<std::string, std::shared_ptr<model::GameSession>> sessions_;
    // ГЕНЕРАТОР ЛУТА НА КАРТУ
    std::shared_ptr<loot_gen::LootGenerator> looter_ = nullptr;
    // ГРАФ ТОЧЕК
    GameGraph graph_;

    
  
    
  
  };

  /*
  КЛАСС ДЛЯ ИНФОРМАЦИОННОЙ ПОДДЕРЖКИ Play....
  Play ответственнен за изменение игрового состояние!!!
  */

  class PlayObserver
  {
  private:
    const Play &play_;

  public:
    PlayObserver(const Play &play);
    // ПЕЧАТАЕТ ДОРОГИ ИГРЫ
    void PrintSessionRoads() const;
    // ПЕЧАТАЕТ СЕССИИ ИГРЫ
    void PrintSessions() const;

    /////////////////////////////////////////////////////////////////////////////////
    // ВОЗВРАЩАЕТ ОТСОРТИРОВАННЫЕ ДОРОГИ
    const decltype(play_.sorted_by_type_roads_) &SortedRoads() const;
    // ВОЗВРАЩАЕТ БД ИГРОКОВ
    const decltype(play_.token_players_) &GetTokenPlayers() const;
    // ВОЗВРАЩАЕТ БД СЕССИЙ
    const decltype(play_.sessions_) &GetSessionList() const;

    /////////////////////////////////////////////////////////////////////////////////
    // ВОЗВРАЩАЕТ ИГРОКА ПО ЕГО ТОКЕНУ
    std::shared_ptr<Player> FindByToken(const std::string &token) const;
    // ВОЗВРАЩАЕТ СЕССИЮ ПО ИМЕНИ
    std::shared_ptr<model::GameSession> GetSession(const std::string &name) const;
    // ВОЗВРАЩАЕТ ВСЕ ИГРОВЫЕ СЕССИИ
    const std::unordered_map<std::string, std::shared_ptr<model::GameSession>> &GameSessions() const;
    // ВОЗВРАЩАЕТ СКОРОСТЬ ПО УМОЛЧАНИЮ
    double GetDefaultSpeed() const;
    // ВОЗВРАЩАЕТ ИГРОФОЙ ГРАФ ТОЧЕК ДОРОГ
    const GameGraph &Graph() const;
    // ВОЗВРАЩАЕТ В АВТОМАТИЧЕСКОМ  ЛИ РЕЖИМЕ ПРОИСХОДИТ
    // УПРАВЛЕНИЕ ИГРОВЫМ ВРЕМЕНМ
    bool AutoTick() const;
    // ВОЗВРАЩАЕТ НУЖНО ЛИ РАНДОМНО ГЕНЕРИРОВАТЬ СОБАК НА КАРТЕ
    bool NeedRandom() const;
    // ИМЕЕТСЯ ЛИ СЕССИЯ С ТАКИМ ИМЕНЕМ
    bool HavingSession(const std::string &session) const;
    // ИМЕЕТСЯ ЛИ КАРТА С ТАКИМ ИМЕНЕМ
    bool HavingMap(const std::string &map) const;
    // ВОЗВРАЩАЕТ ДЖИСОН ПРЕДСТАВЛЕНИЕ О СЕССИЯХ И ИГРОКАХ ГДЕ СЕЙЧАС ИГРОК
    std::string GetJSONSession(std::shared_ptr<Player> player) const;
    void PrintPlayers() const;
  };
}