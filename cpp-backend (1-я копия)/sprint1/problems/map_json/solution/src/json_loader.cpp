

// Этот заголовочный файл надо подключить в одном и только одном .cpp-файле программы
// #include <boost/json/src.hpp>
#include "json_loader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>


using namespace std::literals;



std::optional<model::Road> MakeRoad(boost::json::object obj)
{
    if (obj.size() != 3)
        return std::nullopt;

    if (obj.count("x0") == 0 || obj.count("y0") == 0 ||
        (obj.count("x1") == 0 && obj.count("y1") == 0))
        return std::nullopt;

    model::Point start{static_cast<int>(obj.at("x0").as_int64()), 
                       static_cast<int>(obj.at("y0").as_int64())};

    if (obj.count("x1") > 0)
    {
        auto value = obj.at("x1").as_int64();
        model::Road rd(model::Road::HORIZONTAL, start, value);
        return {std::move(rd)};
    }
    else if (obj.count("y1") > 0)
    {
        auto value = obj.at("y1").as_int64();
        model::Road rd(model::Road::VERTICAL, start, value);
        return {std::move(rd)};
    }
    return std::nullopt;
}

std::optional<model::Building> MakeBuilding(boost::json::object obj)
{
    if (obj.count("x") == 0 || obj.count("y") == 0 || obj.count("w") == 0 || obj.count("h") == 0)
        return std::nullopt;

    int posx = static_cast<int>(obj.at("x").as_int64());
    int posy = static_cast<int>(obj.at("y").as_int64());
    int weigth = static_cast<int>(obj.at("w").as_int64());
    int height = static_cast<int>(obj.at("h").as_int64());
    return model::Building({{posx, posy}, {weigth, height}});
}

std::optional<model::Office> MakeOffice(boost::json::object obj)
{
    if (obj.count("id") == 0 || obj.count("x") == 0 || obj.count("y") == 0 || obj.count("offsetX") == 0 || obj.count("offsetY") == 0)
        return std::nullopt;
    model::Office::Id office_id(std::string(std::move(obj.at("id").as_string())));
    // ctor -> id, point, offfset
    int posx = static_cast<int>(obj.at("x").as_int64());
    int posy = static_cast<int>(obj.at("y").as_int64());
    int offx = static_cast<int>(obj.at("offsetX").as_int64());
    int offy = static_cast<int>(obj.at("offsetY").as_int64());
    return model::Office(std::move(office_id), {posx, posy}, {offx, offy});
}

std::vector<model::Map> ParseMaps(boost::json::array maps)
{
    std::vector<model::Map> maps_collection;
    for (auto &&map : maps)
    {

        std::string name(map.as_object().at("name").as_string());
        std::string id(map.as_object().at("id").as_string());
        model::Map mp(model::Map::Id(id), name);

        auto &mp_obj = map.as_object();
        auto &roads = mp_obj.at("roads").as_array();
        auto &buildings = mp_obj.at("buildings").as_array();
        auto &offices = mp_obj.at("offices").as_array();

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

    model::Game LoadGame(const std::filesystem::path &json_path)
    {
        model::Game game;
        auto parse = ParseFile(json_path);
        auto maps = ParseMaps(parse.as_object().at("maps").as_array());
        LoadMapsToGame(maps, game);
        return game;
    }

} // namespace json_loader
