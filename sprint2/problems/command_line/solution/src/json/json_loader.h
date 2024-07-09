#pragma once
#include <boost/json.hpp>
#include <filesystem>
#include "../game/game.h"


struct Literals
{
    Literals() = delete;
    static constexpr const boost::json::string_view X = "x";
    static constexpr const boost::json::string_view Y = "y";
    static constexpr const boost::json::string_view W = "w";
    static constexpr const boost::json::string_view H = "h";
    static constexpr const boost::json::string_view X0 = "x0";
    static constexpr const boost::json::string_view Y0 = "y0";
    static constexpr const boost::json::string_view X1 = "x1";
    static constexpr const boost::json::string_view Y1 = "y1";
    static constexpr const boost::json::string_view OFFSETX = "offsetX";
    static constexpr const boost::json::string_view OFFSETY = "offsetY";
    static constexpr const boost::json::string_view ROADS = "roads";
    static constexpr const boost::json::string_view BUILDINGS = "buildings";
    static constexpr const boost::json::string_view OFFICES = "offices";
    static constexpr const boost::json::string_view MAPS = "maps";
    static constexpr const boost::json::string_view ID = "id";
    static constexpr const boost::json::string_view NAME = "name";
    static constexpr const boost::json::string_view defaultDogSpeed = "defaultDogSpeed";
    static constexpr const boost::json::string_view dogSpeed = "dogSpeed";
};

//#include"../model/game_dog_session.h"
namespace json = boost::json;
namespace json_loader {
     api::Play LoadPlay(const std::filesystem::path& json_path, bool random, bool auto_t);

}  // namespace json_loader
