#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>
#include <optional>
#include <cassert>
#include <cmath>
#include <filesystem>

// #define LOGGING 1
// #define FORCE 1
   #define MAX_SPEED_RESPONSE 1

using context = boost::asio::io_context;
using strand = boost::asio::io_context::strand;
bool IsGreaterEqual(double a, double b);
bool IsLessEqual(double a, double b);
bool IsDoubleEqual(double a, double b);


struct LoadConfig{
    double map_speed = 1.;
    size_t bag_capacity = 3;
};

namespace model
{

    using Dimension = int;
    using Coord = Dimension;

    // Деклрирование структур 1.Имя 2.Тип переменной 1 3.Имя аргумента1  4.Тип аргумента 2 5.Имя аргуменнта 2

    // ИНФРАСТУКТУРА
    struct Point
    {
        int x, y;
        bool operator==(const Point &rhs) const
        {
            return ((x == rhs.x) && (y == rhs.y));
        }
    };

    struct Size
    {
        int width, height;
        bool operator==(const Size &rhs) const
        {
            return ((width == rhs.width) && (height == rhs.height));
        }
    };

    struct DogCoordinates
    {
        double x, y;
        bool operator==(const DogCoordinates &other) const
        {
            return IsDoubleEqual(x, other.x) && IsDoubleEqual(y, other.y);
        }
    };
    using LootCoordinates = DogCoordinates;

    struct Offset{
      Dimension dx, dy;
    };
    
    struct Rectangle{
       Point position;
       Size size; 
    };
    
    struct DogSpeed{
      double speed_x, speed_y;
    };
    
    enum RoadOrient
    {
        VERTRICAL,
        HORIZONTAL
    };

    enum NSWE_Direction
    {
        NORTH,
        SOUTH,
        WEST,
        EAST,
        STOP
    };

    struct UDLR
    {
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
        {NSWE_Direction::WEST, UDLR::L},
        {NSWE_Direction::EAST, UDLR::R},
        {NSWE_Direction::STOP, UDLR::STOP}};

    const std::map<std::string_view, NSWE_Direction> string_nswe{
        {UDLR::U, NSWE_Direction::NORTH},
        {UDLR::D, NSWE_Direction::SOUTH},
        {UDLR::L, NSWE_Direction::WEST},
        {UDLR::R, NSWE_Direction::EAST},
        {UDLR::STOP, NSWE_Direction::STOP}};

    struct Limits
    {
        double xmin;
        double xmax;
        double ymin;
        double ymax;
        using ColisionSquare = Limits;
    };
}


namespace request_handler
{
    using TimePoint = std::chrono::system_clock::time_point;

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
        TICK,
        ERROR
    };

};

std::ostream &operator<<(std::ostream &os, const model::Point &pt);
std::ostream &operator<<(std::ostream &os, const model::DogCoordinates &pt);
std::ostream &operator<<(std::ostream &os, const model::DogSpeed &ds);
std::ostream &operator<<(std::ostream &os, const model::NSWE_Direction dir);
std::ostream &operator<<(std::ostream &os, const model::Limits &lim);

#ifdef LOGGING
template <typename T>
void yendl(const T &val)
{
    std::cout << val << std::endl;
}

template <typename T, typename V>
void yendl(const T &val, const V &val2)
{
    std::cout << val << ": " << val2 << std::endl;
}

template <typename T>
void nendl(const T &val)
{
    std::cout << val;
}

template <typename T>
void showtype(const T &val)
{
    yendl(typeid(T).name());
}

template <typename T>
void arrow_endl(const T &val)
{
    std::cout << "->" << val << "<-" << std::endl;
}

template <typename T>
void arrow_nendl(const T &val)
{
    std::cout << "->" << val << "<-";
}
#endif