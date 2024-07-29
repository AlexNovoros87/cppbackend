#include "serialize_overloads.h"

namespace serialization
{
  std::optional<ArchiveGame> LoadOldGame(const std::filesystem::path &pth)
  {
    if (!std::filesystem::exists(pth.lexically_normal()))
      return std::nullopt;
    ArchiveGame playarch;
    try
    {
      std::ifstream ifs(pth.lexically_normal(), std::ios::binary);
      boost::archive::text_iarchive inarchive(ifs);
      inarchive >> playarch;
    }
    catch (const std::exception &ex)
    {
      std::cerr << "NOT READ" << std::endl;
      throw ex;
    };
    return playarch;
  }
}

// DearchivePlay
namespace serialization
{

  DearchivePlay::DearchivePlay(api::Play &play) : play_(play), observer_(play) {}

  api::Play &DearchivePlay::MyGame() { return play_; }
  const api::PlayObserver &DearchivePlay::GameObserver() const
  {
    return observer_;
  }

  std::unordered_map<std::string, std::shared_ptr<api::Player>> &DearchivePlay::GameTokenPlayersNoConst()
  {
    return play_.token_players_;
  }

  std::unordered_map<std::string, std::shared_ptr<model::GameSession>> &DearchivePlay::GetSessionListNotConst()
  {
    return play_.sessions_;
  }
}

// АРХИВ СОБАКИ И ИГРОКА
namespace serialization
{
  DogRepr::DogRepr(const model::Dog &dog)
      : name_(dog.GetName()),
        id_(dog.GetId()),
        coordinates_(dog.GetDogCoord()),
        speed_(dog.GetDodSpd()),
        direction_(dog.GetOrientation()),
        bag_(dog.GetLootBag()),
        score_(dog.Score()),
        ids_(dog.DogsCount()) {}

  [[nodiscard]] model::Dog DogRepr::Restore() const
  {
    model::Dog dog(std::move(name_));
    dog.RestoreId(id_);
    dog.SetDogCoordinates(coordinates_);
    dog.SetDogSpeedAndDirection(speed_, direction_);
    dog.RestoreLoot(bag_);
    dog.TakeScores(score_);
    dog.RestoreIdStatic(ids_);

    return dog;
  }

  ArchivePlayer::ArchivePlayer(const api::Player &player) : session_map_id_(*player.PlayersSession()->GetMap().GetId()),
                                                            need_randomize_(player.NeedRandom()),
                                                            token_(player.GetToken()),
                                                            id_(player.GetId()),
                                                            players_count_(player.GetPlayersCount()),
                                                            dogid_(player.PlayersDog()->GetId())

  {
  }

  api::Player ArchivePlayer::RestorePlayer(DearchivePlay &arch)
  {
    const auto &play = arch.GameObserver();
    auto game_sess = play.GetSession(session_map_id_);
    auto &roads = play.SortedRoads().at(session_map_id_);
    auto &dogs = game_sess->GetDogs();

    std::shared_ptr<model::Dog> dog;
    assert(game_sess->GetDogs().count(dogid_) > 0);
    dog = game_sess->GetDogs().at(dogid_);
    api::Player actor(game_sess, roads, need_randomize_, dog, token_, id_);
    actor.SetStaticPlayersCount(players_count_);
    return actor;
  }

}
// АРХИВ ИГРЫ И СЕССИИ
namespace serialization
{

  ArchiveSession::ArchiveSession(const model::GameSession &game_sess) : session_id_(*game_sess.GetMap().GetId()),
                                                                        players_(game_sess.GetPlayersCount()),
                                                                        add_loot_order_(game_sess.GetLootOrder())

  {
    for (auto &&dog : game_sess.GetDogs())
    {
      dogs_[dog.first] = DogRepr(*dog.second);
    }

    for (auto &&loot : game_sess.GetLootList())
    {
      loot_obj_[loot.first] = loot.second;
    }
  }

  void ArchiveSession::RestoreSession(DearchivePlay &arch)
  {
    const api::PlayObserver &play = arch.GameObserver();
    auto &game_sess = *play.GetSession(session_id_);
    assert(*game_sess.GetMap().GetId() == session_id_);
    std::map<uint32_t, std::shared_ptr<model::Dog>> restored;
    for (auto &&archived : dogs_)
    {
      restored[archived.first] = std::make_shared<model::Dog>(archived.second.Restore());
    }
    game_sess.RestoreSession(std::move(restored), players_, std::move(loot_obj_), add_loot_order_);
  }

  ArchiveGame::ArchiveGame(const api::PlayObserver &play)
  {
    auto &play_token_base = play.GetTokenPlayers();
    auto &game_session_base = play.GetSessionList();

    for (auto &&player : play_token_base)
    {
      players_[player.first] = ArchivePlayer(*player.second);
    }

    for (auto &&gameses : game_session_base)
    {
      sessions_[gameses.first] = ArchiveSession(*gameses.second);
    }
  }

  void ArchiveGame::RestoreGAME(DearchivePlay &arch)
  {

    for (auto &&sess : sessions_)
    {
      sess.second.RestoreSession(arch);
    }

    for (auto &&pl : players_)
    {
      arch.GameTokenPlayersNoConst()[pl.first] = std::make_shared<api::Player>(pl.second.RestorePlayer(arch));
    }
  }

}
namespace serialization
{

  ArchivePlay::ArchivePlay(const api::Play &play, std::filesystem::path pth) : observer_(play),
                                                                               path_to_save_tmp(std::filesystem::path(pth.parent_path() / "tmp.txt")),
                                                                               path_to_save_(pth) {}

  void ArchivePlay::NeedWrite(std::chrono::milliseconds ms)
  {
    if (!react_to_signal || ms == ZERO)
      return;
    period_accamulator_ += ms;

    if (period_accamulator_ >= write_period_)
    {
      Archive();
      period_accamulator_ = ZERO;
    }
  }

  void ArchivePlay::Archive()
  {
    try
    {
      std::ofstream writer_(path_to_save_tmp, std::ios::binary);
      boost::archive::text_oarchive out_archive_(writer_);
      ArchiveGame arch_game(observer_);
      out_archive_ << arch_game;
    }
    catch (const std::exception &ex)
    {
      std::cerr << "BAD ARCHIVE" << std::endl;
    }
    if (std::filesystem::exists(path_to_save_))
      std::filesystem::remove(path_to_save_);
    std::filesystem::rename(path_to_save_tmp, path_to_save_);
  };

  void ArchivePlay::SetSavePerod(std::chrono::milliseconds period)
  {
    write_period_ = period;
    react_to_signal = true;
  }
}
