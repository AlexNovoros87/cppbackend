#pragma once
#include <boost/json.hpp>
#include <filesystem>
#include "../game/game.h"

struct Literals
{
    Literals() = delete;
    static constexpr boost::json::string_view X = "x";
    static constexpr boost::json::string_view Y = "y";
    static constexpr boost::json::string_view W = "w";
    static constexpr boost::json::string_view H = "h";
    static constexpr boost::json::string_view X0 = "x0";
    static constexpr boost::json::string_view Y0 = "y0";
    static constexpr boost::json::string_view X1 = "x1";
    static constexpr boost::json::string_view Y1 = "y1";
    static constexpr boost::json::string_view OFFSETX = "offsetX";
    static constexpr boost::json::string_view OFFSETY = "offsetY";
    static constexpr boost::json::string_view ROADS = "roads";
    static constexpr boost::json::string_view BUILDINGS = "buildings";
    static constexpr boost::json::string_view OFFICES = "offices";
    static constexpr boost::json::string_view MAPS = "maps";
    static constexpr boost::json::string_view ID = "id";
    static constexpr boost::json::string_view NAME = "name";
    static constexpr boost::json::string_view defaultDogSpeed = "defaultDogSpeed";
    static constexpr boost::json::string_view dogSpeed = "dogSpeed";
    static constexpr boost::json::string_view lootTypes = "lootTypes";
    static constexpr boost::json::string_view file = "file";
    static constexpr boost::json::string_view type = "type";
    static constexpr boost::json::string_view rotation = "rotation";
    static constexpr boost::json::string_view color = "color";
    static constexpr boost::json::string_view scale = "scale";
    static constexpr boost::json::string_view period = "period";
    static constexpr boost::json::string_view probability = "probability";
    static constexpr boost::json::string_view lootGeneratorConfig = "lootGeneratorConfig";
    static constexpr boost::json::string_view defaultBagCapacity = "defaultBagCapacity";
    static constexpr boost::json::string_view bagCapacity = "bagCapacity";
};


namespace json = boost::json;
namespace json_loader
{
    api::Play LoadPlay(const std::filesystem::path &json_path, bool random, bool auto_t);

} // namespace json_loader
