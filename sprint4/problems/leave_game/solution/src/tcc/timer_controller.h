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

  struct Activity
  {
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

  class SQL
  {
  public:
    static void SetConnectAdressAndBuildTable();
    // ВСЕХ КИКНУТЫХ ИГОКОВ ЗАНОСИМ В БАЗУ СККУЭЛЬ
    static void RecordTOSQLKicked(std::vector<api::IdTWithDurSc> to_kick);
    static std::string GetRecords(int offset, int limit);

  private:
    static char *adress_;
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

    // ПОДКЛЮЧЕНИЕ ИГРОКА К НАБЛОЮДАТЕЛЮ ЗА ЕГО АКТИВНОСТЬЮ
    static void JoinToTimeObserve(const std::string &map, size_t id_pl, const std::string &token_pl,
                                  const std::chrono::system_clock::time_point &servertime);

    // ПРИНИМАЕТ РЕШЕНИЕ УДАЛЯТЬ ЛИ ДАННОГО ИГРОКА ИЛИ НЕТ
    static std::optional<api::IdTWithDurSc> StructToDeleteOrNull(const std::string &map, size_t id_pl,
                                                                 const std::chrono::system_clock::time_point &servertime, std::chrono::milliseconds delta);

    // СИГНАЛ НА ДВИДЕНИЕ НА (карте Х)(игрок У) начал движение....
    static void SignalMove(const std::string &map, size_t id_pl);
    // СИГНАЛ НА ДВИДЕНИЕ НА (карте Х)(игрок У) начал остановился....
    // И начался отсчет его простоя с servertime///
    static void SignalStop(const std::string &map, size_t id_pl, const std::chrono::system_clock::time_point &servertime);

  private:
    static std::chrono::system_clock::time_point GetNow();
    // ВРЕМЯ УДАЛЕНИЯ ИГРОКА
    static std::chrono::milliseconds retire_time_;
    // МОМЕНТ ВРЕМЕНИ ВХОДА ИГРОКА
    static std::unordered_map<std::string, std::unordered_map<size_t, std::chrono::system_clock::time_point>> time_entered_;
    // АКТИВНОСТЬ ИГРОКА
    static std::unordered_map<std::string, std::unordered_map<size_t, Activity>> activity_;
    // ПО КАРТЕ И АЙДИ ХРАНИТ ТОКЕН, АЙДИ И АЙДИ КАРТЫ ГДЕ ИГРОК
    static std::unordered_map<std::string, std::unordered_map<size_t, IdToken>> info_;
  };

}
