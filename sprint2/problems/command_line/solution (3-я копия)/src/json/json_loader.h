#pragma once
#include <boost/json.hpp>
#include <filesystem>
#include "../game/game.h"

//#include"../model/game_dog_session.h"
namespace json = boost::json;
namespace json_loader {
     api::Play LoadPlay(const std::filesystem::path& json_path, bool random, bool auto_t);

}  // namespace json_loader
