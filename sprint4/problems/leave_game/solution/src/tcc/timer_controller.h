#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <pqxx/pqxx>
#include <queue>
#include <optional>
#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>

#include "../uuid/uuid.h"

using namespace std::literals;

namespace api
{
  enum class MoveStautus
  {
    MOVING,
    STANDING
  };

  struct Activity{
    MoveStautus movestatus;
    std::chrono::system_clock::time_point time_point;
  };

  struct IdToken
  {
    size_t id;
    std::string token;
    std::string map_name;
  };

   struct IdTWithDurSc
  {
    IdToken params;
    int64_t duration;
    int score;
    std::string name;
  };
}

using pqxx::operator"" _zv;
constexpr const char GAME_DB_URL[]{"GAME_DB_URL"};

namespace sql
{
  constexpr pqxx::zview records = "records";
  class SQL
  {

  public:
    using Strand = boost::asio::strand<boost::asio::io_context::executor_type>;

    static void SetConnectAdressAndBuildTable()
    {
      // УСТАНАВЛИВАЕМ АДРЕС ДЛЯ СОЗДАНИЯ ПОДКЛЮЧЕНИЙ
      adress_ = std::getenv(GAME_DB_URL);

      pqxx::connection conn{adress_};
      pqxx::transaction trz{conn};

      // СТРОИМ ТАБЛИЦУ
      trz.exec (R"(
      CREATE TABLE IF NOT EXISTS retired_players
      (
          id UUID PRIMARY KEY,
          name VARCHAR(100) NOT NULL,
          score INT NOT NULL,
          play_time_ms INT NOT NULL
          
      );

      CREATE INDEX IF NOT EXISTS retired_players_score_playtime_name_idx ON retired_players (
      score DESC, play_time_ms, name
      );
      )"_zv);
      
      
      
      
      trz.commit();
    }

    static void RecordTOSQLKicked(std::vector<api::IdTWithDurSc> to_kick)
    {
      pqxx::connection conn{adress_};
      pqxx::transaction trz{conn};

      for (auto &member : to_kick)
      {
        trz.exec_params("INSERT INTO retired_players VALUES ( $1 ,$2 , $3, $4 );"_zv,
                        uuid::NewStringUUDD(), std::move(member.name), member.score, member.duration);
      }
      trz.commit();
    }

    static std::string GetRecords(int offset, int limit)
    {

      boost::json::array arr;
      pqxx::connection conn{adress_};
      pqxx::transaction trz{conn};

      for (auto [name, score, time] : trz.exec_params(

                                             R"( 
          SELECT name, score, play_time_ms 
          FROM retired_players 
          ORDER BY score DESC, play_time_ms ASC, name 
          LIMIT  $1 OFFSET $2; 
         )"_zv,
                                             limit, offset)
                                          .iter<std::string, int, int>())
      {

        boost::json::object obj;
        obj["name"] = std::move(name);
        obj["score"] = score;
        obj["playTime"] = static_cast<double>(time) / 1000.;
        arr.push_back(std::move(obj));
      }

      return boost::json::serialize(arr);
    }

    static void SetContextAndStrand(boost::asio::io_context &cont)
    {
      ioc_ = &cont;
      strand_ = std::make_unique<Strand>(boost::asio::make_strand(*ioc_));
    }

  private:
    static char *adress_;
    static boost::asio::io_context *ioc_;
    static std::unique_ptr<Strand> strand_;
  };

}

namespace api
{

  // TimerControllerClass
  class TCC
  {
  public:
    TCC();
    // УСТАНОВКА ВРЕМЕНИ ПРОСТОЯ ДО КИКА
    static void SetRetireTime(std::chrono::milliseconds time);

    // ПРОВЕРКА ЧТО БЫ НЕ ЗАБЫТЬ УСТАНОВТЬ ВРЕМЯ
    static void Check();

    static void JoinToTimeObserve(const std::string &map, size_t id_pl, const std::string &token_pl,
                                  const std::chrono::system_clock::time_point& servertime);

    static std::optional<api::IdTWithDurSc> StructToDeleteOrNull(const std::string &map, size_t id_pl,
    const std::chrono::system_clock::time_point& servertime, std::chrono::milliseconds delta);

    static void SignalMove(const std::string &map, size_t id_pl){
         activity_.at(map).at(id_pl).movestatus =  MoveStautus::MOVING;
    }

    static void SignalStop(const std::string &map, size_t id_pl, const std::chrono::system_clock::time_point& servertime){
         activity_.at(map).at(id_pl) = { MoveStautus::STANDING , servertime};
    }
  
  private:
    static std::chrono::system_clock::time_point GetNow();
    // ВРЕМЯ УДАЛЕНИЯ ИГРОКА
    static std::chrono::milliseconds retire_time_;
    // МОМЕНТ ВРЕМЕНИ ВХОДА ИГРОКА
    static std::unordered_map<std::string, std::unordered_map<size_t, std::chrono::system_clock::time_point>> time_entered_;
    //АКТИВНОСТЬ ИГРОКА
    static std::unordered_map<std::string, std::unordered_map<size_t, Activity>> activity_;
  
    static std::unordered_map<std::string, std::unordered_map<size_t, IdToken>> info_;
  };

}
