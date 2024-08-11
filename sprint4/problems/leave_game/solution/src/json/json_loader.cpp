// Этот заголовочный файл надо подключить в одном и только одном .cpp-файле программы
#include <boost/json/src.hpp>
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

LoadConfig GetGonfig(const json::value& map, const LoadConfig& deflt) {
     LoadConfig config = deflt;

        if (map.as_object().count(Literals::dogSpeed))
        {
            config.map_speed = map.as_object().at(Literals::dogSpeed).as_double();
        }
        else
        {
            config.map_speed = deflt.map_speed;
        }

        if (map.as_object().count(Literals::bagCapacity))
        {
            config.bag_capacity = map.as_object().at(Literals::bagCapacity).as_double();
        }
        else
        {
            config.map_speed = deflt.map_speed;
        }

   return config;
}

std::vector<model::Map> ParseMaps(boost::json::array maps, const LoadConfig& lconfig)
{
    std::vector<model::Map> maps_collection;

    for (auto &&map : maps)
    {
        //ИМЯЯ КАРТЫ
        std::string name(map.as_object().at(Literals::NAME).as_string());
        //АЙДИ КАРТЫ
        std::string id(map.as_object().at(Literals::ID).as_string());

        //ПОЛУЧАЕМ УСТАНОВКИ СКОРОСТИ СОБАКИ И ЕМКОСТИ РЮКЗАКА
        LoadConfig config = GetGonfig(map,lconfig);

        //СОЗДАЕМ КАРТУ
        model::Map mp(model::Map::Id(id), name, config.map_speed, config.bag_capacity);

        auto &mp_obj = map.as_object();
        auto &roads = mp_obj.at(Literals::ROADS).as_array();
        auto &buildings = mp_obj.at(Literals::BUILDINGS).as_array();
        auto &offices = mp_obj.at(Literals::OFFICES).as_array();
        auto &lootobjects = mp_obj.at(Literals::lootTypes).as_array();

        //ДОБАВЛЯЕМ ДОРОГИ
        for (auto &&element : roads)
        { // addroad
            auto road = MakeRoad(element.as_object());
            if (road.has_value())
            {
                mp.AddRoad(std::move(road.value()));
            };
        } // addroad

        
        //ДОБАВЛЯЕМ ЗДАНИЯ
        for (auto &&element : buildings)
        { // building
            auto building = MakeBuilding(element.as_object());
            if (building.has_value())
            {
                mp.AddBuilding(std::move(building.value()));
            }
        } // building

        //ДОБАВЛЯЕМ ОФИСЫ
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

void AddLoot(const std::string mapname, boost::json::array arr){
  frontend::FrontEndLoot::AddMapLoot(std::move(mapname), std::move(arr));
}

void AddAllLoost(const boost::json::array& maps){
    for(auto && map: maps){
       std::string name =  std::string(map.as_object().at(Literals::NAME).as_string());
       if(map.as_object().count(Literals::lootTypes)>0){
        AddLoot(name, map.as_object().at(Literals::lootTypes).as_array());
       }

    }

}


void SetupAll(double time){
    
    std::chrono::milliseconds ms(static_cast<int64_t>(time));
    //УСТАНАВЛИВАЕМ ВРЕМЯ 
    api::TCC::SetRetireTime(ms);
    //УСТАНАВЛИВАЕМ ПЕРЕМЕННУЮ СОЕДИНЕНИЯ И СТРОИМ ТАБЛИЦУ
    sql::SQL::SetConnectAdressAndBuildTable();
}

namespace json_loader
{

    api::Play LoadPlay(const std::filesystem::path &json_path, bool random, bool auto_t)
    {

        double kicktime = 60000.; 
        
        model::Game game;
        auto parse = ParseFile(json_path);
        LoadConfig config;

        if (parse.as_object().count(Literals::defaultDogSpeed) > 0)
        {
           config.map_speed = parse.as_object().at(Literals::defaultDogSpeed).as_double();
        }

        if (parse.as_object().count(Literals::defaultBagCapacity) > 0)
        {
           config.bag_capacity = parse.as_object().at(Literals::defaultBagCapacity).as_uint64();
        }

        auto maps = ParseMaps(parse.as_object().at(Literals::MAPS).as_array(), config);
        
        LoadMapsToGame(maps, game);
        AddAllLoost(parse.as_object().at(Literals::MAPS).as_array());

        api::Play play(std::move(game), config.map_speed, random, auto_t);
        
        double period = parse.as_object().at(Literals::lootGeneratorConfig).at(Literals::period).as_double();
        double probabil = parse.as_object().at(Literals::lootGeneratorConfig).at(Literals::probability).as_double();
        play.SetupLootManager(period , probabil);


        //ЕСЛИ ВРЕМЯ КИКА НЕ ДЕФОЛТ - МЕНЯЕМ
        if (parse.as_object().count(Literals::dogRetirementTime) > 0)
        {
           kicktime = parse.as_object().at(Literals::dogRetirementTime).as_double() * 1000.;
        }
        //СТРОИМ ТАБЛИЦЫ И НАСТРАИВАЕМ НАБЛЮДАТЕЛЯ ЗА ПРОСТОЕМ
        SetupAll(kicktime);


        return play;
    };

} // namespace json_loader
