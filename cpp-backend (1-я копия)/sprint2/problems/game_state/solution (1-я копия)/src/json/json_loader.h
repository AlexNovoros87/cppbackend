#pragma once
#include <boost/json.hpp>
#include <filesystem>
#include "../model/game_dog_session.h"
namespace json = boost::json;

namespace json_loader {
model::Game LoadGame(const std::filesystem::path& json_path);
}  // namespace json_loader
