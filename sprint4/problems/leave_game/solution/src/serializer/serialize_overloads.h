#pragma once

#include <iostream>
#include <chrono>
#include <fstream>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "../game/game.h"
using namespace std::literals;

/*
  ИМЕЕТ ДОСТУП К ПОЛЯМ КЛАССА Play И ИМЕННО ЕГО ПЕРЕДАЕМ
  в структуру ArciveGame для восстановления той гейм-сесии
  для которой он является оберткой.
  Имеет Так же PlayObserver для поиска нужных "вещей" при
  его работе.
  */

namespace model
{

    template <typename Archive>
    void serialize(Archive &ar, model::DogCoordinates &coord, const unsigned int file_version)
    {
        ar & coord.x;
        ar & coord.y;
    }

    template <typename Archive>
    void serialize(Archive &ar, model::DogSpeed &spd, const unsigned int file_version)
    {
        ar & spd.speed_x;
        ar & spd.speed_y;
    }

    template <typename Archive>
    void serialize(Archive &ar, model::NSWE_Direction &dir, const unsigned int file_version)
    {
        ar & dir;
    }

    template <typename Archive>
    void serialize(Archive &ar, model::Loot &loot, const unsigned int file_version)
    {
        ar & loot.id;
        ar & loot.type;
    }

}

namespace serialization{

//ДЕАРХИВ ИГРЫ
class DearchivePlay
{

public:
    DearchivePlay(api::Play &play);
    api::Play &MyGame();
    const api::PlayObserver &GameObserver() const;
    //НЕКОНСТ ССЫЛКА НА ИГРОК-ТОКЕН
    std::unordered_map<std::string, std::shared_ptr<api::Player>> &GameTokenPlayersNoConst();
     //НЕКОНСТ ССЫЛКА НА СЕССИИ
    std::unordered_map<std::string, std::shared_ptr<model::GameSession>> &GetSessionListNotConst();

private:
    api::Play &play_;
    api::PlayObserver observer_;
};


// DogRepr (DogRepresentation) - сериализованное представление класса Dog
class DogRepr
{
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog &dog);
   [[nodiscard]] model::Dog Restore() const;

    template <typename Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned version)
    {
        ar & name_;
        ar & id_;
        ar & coordinates_;
        ar & speed_;
        ar & direction_;
        ar & bag_;
        ar & score_;
        ar & ids_;
    }

private:
    std::string name_;
    uint32_t id_;
    model::DogCoordinates coordinates_;
    model::DogSpeed speed_;
    model::NSWE_Direction direction_;
    std::vector<model::Loot> bag_;
    int score_;
    uint32_t ids_;
};

class ArchiveSession
{
public:
    ArchiveSession() = default;
    ArchiveSession(const model::GameSession &game_sess);
    void RestoreSession(DearchivePlay &arch);

    template <typename Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned version)
    {
        ar & session_id_;
        ar & loot_obj_;
        ar & players_;
        ar & add_loot_order_;
        ar & dogs_;
    }

private:
    std::string session_id_;
    std::unordered_map<model::LootCoordinates, model::Loot, model::GameSession::Hasher> loot_obj_;
    int players_;
    size_t add_loot_order_;
    std::map<uint32_t, DogRepr> dogs_;
};



//АРХИВИРОВАНИЕ ИГРОКА
class ArchivePlayer
{
public:
    ArchivePlayer() = default;
    ArchivePlayer(const api::Player &player);
    api::Player RestorePlayer(DearchivePlay &arch);

    template <typename Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned version)
    {
        ar & session_map_id_;
        ar & need_randomize_;
        ar & token_;
        ar & id_;
        ar & players_count_;
        ar & dogid_;
    }

private:
    std::string session_map_id_;
    bool need_randomize_;
    std::string token_;
    uint32_t id_;
    uint32_t players_count_;
    int dogid_;
};

//АРХИВ ИГРЫ
class ArchiveGame
{
public:
    ArchiveGame() = default;
    ArchiveGame(const api::PlayObserver &play);
    void RestoreGAME(DearchivePlay &arch);

    template <typename Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned version)
    {
        ar & players_;
        ar & sessions_;
    }

private:
    std::unordered_map<std::string, ArchivePlayer> players_;
    std::unordered_map<std::string, ArchiveSession> sessions_;
};



class ArchivePlay : public std::enable_shared_from_this<ArchivePlay>
{
public:

    ArchivePlay(const api::Play &play, std::filesystem::path pth);
    //НАДОБНОСТЬ АРХИВИРОВАТЬ
    void NeedWrite(std::chrono::milliseconds ms);
    //АРХИВИРОВАТЬ
    void Archive();
    //УСТАНОВИТЬ ПЕРИОД СОХРАНЕНИЯ
    void SetSavePerod(std::chrono::milliseconds period);


private:
    api::PlayObserver observer_;
    std::filesystem::path path_to_save_;
    std::filesystem::path path_to_save_tmp;
 
    bool react_to_signal = false;
    constexpr static std::chrono::milliseconds ZERO = 0ms;
    std::chrono::milliseconds write_period_ = ZERO;
    std::chrono::milliseconds period_accamulator_ = ZERO;
    
};

//ЗАГРУЗИТЬ СТАРУЮ ИГРУ
std::optional<ArchiveGame> LoadOldGame(const std::filesystem::path& pth);

}
