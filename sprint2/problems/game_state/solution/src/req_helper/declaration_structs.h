#pragma once

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
        EAST
    };

    const std::unordered_map<NSWE_Direction, std::string> string_directions_{
        {NSWE_Direction::NORTH, "U"},
        {NSWE_Direction::SOUTH, "D"},
        {NSWE_Direction::WEST, "L"},
        {NSWE_Direction::EAST, "R"}};

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
        ERROR
    };

};