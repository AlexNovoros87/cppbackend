#pragma once
#include <boost/json.hpp>
#include <filesystem>
#include "model.h"
namespace json = boost::json;

namespace json_loader {
model::Game LoadGame(const std::filesystem::path& json_path);
}  // namespace json_loader
