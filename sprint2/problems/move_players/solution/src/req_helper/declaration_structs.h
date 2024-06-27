#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include<unordered_map>
#include<map>
#include<string>
#include<typeinfo>
#include<iostream>

using context = boost::asio::io_context;
using strand = boost::asio::io_context::strand;

namespace model
{

#define DECLARE_PAIR_STRUCT(Name, Type, n1, Type2, n2) \
    struct Name                                        \
    {                                                  \
        Type n1;                                       \
        Type2 n2;                                      \
    }

    using Dimension = int;
    using Coord = Dimension;

    // Деклрирование структур 1.Имя 2.Тип переменной 1 3.Имя аргумента1  4.Тип аргумента 2 5.Имя аргуменнта 2

    // ИНФРАСТУКТУРА
    DECLARE_PAIR_STRUCT(Point, int, x, int, y);                     // ТОЧКА
    DECLARE_PAIR_STRUCT(Size, Dimension, width, Dimension, height); // РАЗМЕР
    DECLARE_PAIR_STRUCT(Offset, Dimension, dx, Dimension, dy);      // ОТСТУП
    DECLARE_PAIR_STRUCT(Rectangle, Point, position, Size, size);    // ПРЯМОУГОЛЬНИК

    // СОБАКА
    DECLARE_PAIR_STRUCT(DogCoordinates, double, x, double, y);       // КООРДИНЫТЫ ПСА
    DECLARE_PAIR_STRUCT(DogSpeed, double, speed_x, double, speed_y); // СКОРОСТЬ ПСА

    enum NSWE_Direction
    {
        NORTH,
        SOUTH,
        WEST,
        EAST,
        STOP
    };

    
  struct UDLR{
     UDLR() = delete;
     static constexpr std::string_view U = "U";
     static constexpr std::string_view D = "D";
     static constexpr std::string_view L = "L";
     static constexpr std::string_view R = "R";
     static constexpr std::string_view STOP = "";
  };
  
 const std::map<NSWE_Direction, std::string_view> nswe_string{
        {NSWE_Direction::NORTH, UDLR::U},
        {NSWE_Direction::SOUTH, UDLR::D},
        {NSWE_Direction::WEST,  UDLR::L},
        {NSWE_Direction::EAST,  UDLR::R},
        {NSWE_Direction::STOP,  UDLR::STOP}};

 const std::map<std::string_view, NSWE_Direction>string_nswe{
        { UDLR::U ,NSWE_Direction::NORTH},
        { UDLR::D, NSWE_Direction::SOUTH},
        { UDLR::L, NSWE_Direction::WEST},
        { UDLR::R, NSWE_Direction::EAST},
        { UDLR::STOP, NSWE_Direction::STOP}
        };


}

namespace request_handler
{

    enum TypeRequest
    {
        API,
        STATIC
    };

    enum DirectionAPI
    {
        MAPS,
        JOIN,
        PLAYERS,
        STATE,
        PLAYER_ACTION,
        ERROR
    };

};

std::ostream& operator<<(std::ostream& os,const model::Point& pt);
std::ostream& operator<<(std::ostream& os,const model::DogCoordinates& pt);
std::ostream& operator<<(std::ostream& os,const model::DogSpeed& ds);
std::ostream& operator<<(std::ostream& os,const model::NSWE_Direction dir);
