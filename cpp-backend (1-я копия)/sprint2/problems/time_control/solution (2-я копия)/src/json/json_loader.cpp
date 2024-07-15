

// Этот заголовочный файл надо подключить в одном и только одном .cpp-файле программы
#include <boost/json/src.hpp>
#include "json_loader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>

using namespace std::literals;

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

std::optional<model::Road> MakeRoad(boost::json::object obj)
{
    if (obj.size() != 3)
        return std::nullopt;

    if (obj.count(Literals::X0) == 0 || obj.count(Literals::Y0) == 0 ||
        (obj.count(Literals::X1) == 0 && obj.count(Literals::Y1) == 0))
        return std::nullopt;

    model::Point start{static_cast<int>(obj.at(Literals::X0).as_int64()),
                       static_cast<int>(obj.at(Literals::Y0).as_int64())};

    if (obj.count(Literals::X1) > 0)
    {
        auto value = obj.at(Literals::X1).as_int64();
        model::Road rd(model::Road::HORIZONTAL, start, value);
        return {std::move(rd)};
    }
    else if (obj.count(Literals::Y1) > 0)
    {
        auto value = obj.at(Literals::Y1).as_int64();
        model::Road rd(model::Road::VERTICAL, start, value);
        return {std::move(rd)};
    }
    return std::nullopt;
}

std::optional<model::Building> MakeBuilding(boost::json::object obj)
{
    if (obj.count(Literals::X) == 0 || obj.count(Literals::Y) == 0 || obj.count(Literals::W) == 0 || obj.count(Literals::H) == 0)
        return std::nullopt;

    int posx = static_cast<int>(obj.at(Literals::X).as_int64());
    int posy = static_cast<int>(obj.at(Literals::Y).as_int64());
    int weigth = static_cast<int>(obj.at(Literals::W).as_int64());
    int height = static_cast<int>(obj.at(Literals::H).as_int64());
    return model::Building({{posx, posy}, {weigth, height}});
}

std::optional<model::Office> MakeOffice(boost::json::object obj)
{
    if (obj.count(Literals::ID) == 0 || obj.count(Literals::X) == 0 || obj.count(Literals::Y) == 0 || obj.count(Literals::OFFSETX) == 0 || obj.count(Literals::OFFSETY) == 0)
        return std::nullopt;
    model::Office::Id office_id(std::string(std::move(obj.at(Literals::ID).as_string())));
    int posx = static_cast<int>(obj.at(Literals::X).as_int64());
    int posy = static_cast<int>(obj.at(Literals::Y).as_int64());
    int offx = static_cast<int>(obj.at(Literals::OFFSETX).as_int64());
    int offy = static_cast<int>(obj.at(Literals::OFFSETY).as_int64());
    return model::Office(std::move(office_id), {posx, posy}, {offx, offy});
}

std::vector<model::Map> ParseMaps(boost::json::array maps, double def_speed)
{
    std::vector<model::Map> maps_collection;

    for (auto &&map : maps)
    {

        std::string name(map.as_object().at(Literals::NAME).as_string());
        std::string id(map.as_object().at(Literals::ID).as_string());

        double map_speed;
        if (map.as_object().count(Literals::dogSpeed))
        {
            map_speed = map.as_object().at(Literals::dogSpeed).as_double();
        }
        else
        {
            map_speed = def_speed;
        }

        model::Map mp(model::Map::Id(id), name, map_speed);

        auto &mp_obj = map.as_object();
        auto &roads = mp_obj.at(Literals::ROADS).as_array();
        auto &buildings = mp_obj.at(Literals::BUILDINGS).as_array();
        auto &offices = mp_obj.at(Literals::OFFICES).as_array();

        for (auto &&element : roads)
        { // addroad
            auto road = MakeRoad(element.as_object());
            if (road.has_value())
            {
                mp.AddRoad(std::move(road.value()));
            };
        } // addroad

        for (auto &&element : buildings)
        { // building
            auto building = MakeBuilding(element.as_object());
            if (building.has_value())
            {
                mp.AddBuilding(std::move(building.value()));
            }
        } // building

        for (auto &&element : offices)
        { // offices
            auto office = MakeOffice(element.as_object());
            if (office.has_value())
            {
                mp.AddOffice(office.value());
            }
        } // offices

        maps_collection.push_back(std::move(mp));
    }
    return maps_collection;
}

auto ParseFile(const std::filesystem::path &json_path)
{
    std::ifstream js_fstream;
    js_fstream.open(json_path);

    if (!js_fstream)
    {
        std::cerr << "FILE OPEN ERROR" << std::endl;
        std::abort();
    }

    std::ostringstream js_str;
    std::string line;
    while (std::getline(js_fstream, line))
    {
        js_str << line << std::endl;
    }
    auto parse = json::parse(js_str.str());
    return (std::move(parse));
}

void LoadMapsToGame(std::vector<model::Map> &maps, model::Game &game)
{
    for (auto &map : maps)
    {
        game.AddMap(std::move(map));
    }
    maps.clear();
}

namespace json_loader
{

    api::Play LoadPlay(const std::filesystem::path &json_path)
    {

        model::Game game;
        auto parse = ParseFile(json_path);
        double def_speed = 1.;

        if (parse.as_object().count(Literals::defaultDogSpeed) > 0)
        {
            def_speed = parse.as_object().at(Literals::defaultDogSpeed).as_double();
        }

        auto maps = ParseMaps(parse.as_object().at(Literals::MAPS).as_array(), def_speed);
        LoadMapsToGame(maps, game);

        api::Play play(std::move(game), def_speed);
        return play;
    };

} // namespace json_loader
