#include "model_help.h"

#ifdef LOGGING
#include <fstream>
#include <cmath>
std::ofstream fsi("/home/user/_____logTICK.txt", std::ios::trunc);
#endif

namespace model
{
    // РАСЧЕТ НА ШИРИНУ
    constexpr double ROAD_DELTA = 0.4;

    const Limits LIMIT_TEMPLATE{
        std::numeric_limits<double>::max(), // xmin
        std::numeric_limits<double>::min(), // xmax
        std::numeric_limits<double>::max(), // ymin
        std::numeric_limits<double>::min()  // ymax
    };

    DogCoordinates MoveDogDeltaT(std::shared_ptr<Dog> dog, double delta_t, Limits lim)
    {
        DogCoordinates to_return;

        double coord_X = dog->GetDogCoord().x;
        double coord_Y = dog->GetDogCoord().y;

        auto dog_speed = dog->GetDodSpd();
        double dtime = delta_t / 1000;

        double potential_y = (dog_speed.speed_y * dtime) + coord_Y;
        double potential_x = (dog_speed.speed_x * dtime) + coord_X;

        if (std::isgreater(potential_y, lim.ymax))
        {
            to_return.y = lim.ymax;
        }
        else if (std::isless(potential_y, lim.ymin))
        {
            to_return.y = lim.ymin;
        }
        else
        {
            to_return.y = potential_y;
        }

        if (std::isgreater(potential_x, lim.xmax))
        {
            to_return.x = lim.xmax;
        }
        else if (std::isless(potential_x, lim.xmin))
        {
            to_return.x = lim.xmin;
        }
        else
        {
            to_return.x = potential_x;
        }

        return to_return;
    };

    bool CoordinateOnRoad(const DogCoordinates &coord, std::shared_ptr<Road> road)
    {
        double minX = static_cast<double>(std::min(road->GetStart().x, road->GetEnd().x)) - ROAD_DELTA;
        double maxX = static_cast<double>(std::max(road->GetStart().x, road->GetEnd().x)) + ROAD_DELTA;
        double minY = static_cast<double>(std::min(road->GetStart().y, road->GetEnd().y)) - ROAD_DELTA;
        double maxY = static_cast<double>(std::max(road->GetStart().y, road->GetEnd().y)) + ROAD_DELTA;

        if (std::isless(coord.x, minX))
            return false;
        if (std::isgreater(coord.x, maxX))
            return false;
        if (std::isless(coord.y, minY))
            return false;
        if (std::isgreater(coord.y, maxY))
            return false;
        return true;
    }

    std::vector<std::shared_ptr<Road>> OnWhichRoadsDod(const GameSession &game, DogCoordinates coordy, const SessionGraph &graph)
    {

        DogCoordinates coord = coordy;
        //   yendl(coord);

        int X = static_cast<int>(coord.x);
        int Y = static_cast<int>(coord.y);

        std::vector<std::shared_ptr<Road>> roads;

        for (int x = X - 1; x <= X + 1; ++x)
        {
            if (graph.count(x) == 0)
                continue;

            for (int y = Y - 1; y <= Y + 1; ++y)
            {
                if (graph.at(x).count(y) == 0)
                    continue;

                auto &road_range = graph.at(x).at(y);

                for (auto road : road_range)
                {
                    if (CoordinateOnRoad(coord, road))
                    {
                        roads.push_back(road);
                    }
                }
            }
        }

        return roads;
    }

    Limits GetTurnLimits(const std::vector<std::shared_ptr<Road>> &roads, const DogCoordinates &coord)
    {
        // yendl(coord);

        Limits lim = LIMIT_TEMPLATE;
        for (auto road : roads)
        {

            double xmin = std::min(road->GetStart().x, road->GetEnd().x);
            lim.xmin = (xmin < lim.xmin) ? xmin : lim.xmin;

            double xmax = std::max(road->GetStart().x, road->GetEnd().x);
            lim.xmax = (xmax > lim.xmax) ? xmax : lim.xmax;

            double ymin = std::min(road->GetStart().y, road->GetEnd().y);
            lim.ymin = (ymin < lim.ymin) ? ymin : lim.ymin;

            double ymax = std::max(road->GetStart().y, road->GetEnd().y);
            lim.ymax = (ymax > lim.ymax) ? ymax : lim.ymax;
        }

        lim.xmin -= ROAD_DELTA;
        lim.ymin -= ROAD_DELTA;

        lim.xmax += ROAD_DELTA;
        lim.ymax += ROAD_DELTA;

        return lim;
    };

    DogSpeed GetDogSpeedByDrection(std::string_view direction, double speed)
    {
        model::NSWE_Direction dir = model::string_nswe.at(direction);

        switch (dir)
        {
        case model::NSWE_Direction::WEST:
            return {(-1. * speed), 0};
            break;
        case model::NSWE_Direction::EAST:
            return {speed, 0};
            break;
        case model::NSWE_Direction::NORTH:
            return {0, -1. * speed};
            break;
        case model::NSWE_Direction::SOUTH:
            return {0, speed};
            break;
        }
        return {0., 0.};
    }

    std::pair<DogCoordinates, Limits> GetNewCoordinatesAndLimits(const GameSession &game, std::shared_ptr<Dog> dog, const SessionGraph &graph, double delta_t)
    {
        // НА КАКИХ ДОРОГАХ ПЕС
        auto dogroads = OnWhichRoadsDod(game, dog->GetDogCoord(), graph);
        // ЛИМИТЫ ПЕРЕДВИЖЕНИЯ
        auto limits = GetTurnLimits(dogroads, dog->GetDogCoord());
        // ВОЗВРАЩАЕМ НОВЫЕ КООРДИНАТЫ
        auto new_coords = MoveDogDeltaT(dog, delta_t, limits);

        return {std::move(new_coords), std::move(limits)};
    };

}

#ifdef LOGGING
namespace model
{

    void LogWas(std::string foo, std::shared_ptr<model::Dog> dog)
    {
        fsi << "FUNCTION " << foo << std::endl;
        fsi << "Name: " << dog->GetName() << "  Was Coords: " << dog->GetDogCoord()
            << " Was Direction: " << dog->GetOrientation() << " Was Speed: " << dog->GetDodSpd() << std::endl;
    };

    void LogNow(std::string foo, std::shared_ptr<model::Dog> dog)
    {
        fsi << "Name: " << dog->GetName() << "  Now Coords: " << dog->GetDogCoord()
            << " Now Direction: " << dog->GetOrientation() << " Now Speed: " << dog->GetDodSpd() << std::endl
            << std::endl;
        fsi << "====================================================================" << std::endl
            << std::endl;
    };

    void LogLimits(const Limits &lim)
    {
        fsi << "LimitsLog : " << lim << std::endl;
    };

    void LogEndls()
    {
        fsi << "\n\n";
    };

    void LogLine()
    {
        fsi << "\n---------------------------------------------------------\n";
    };

    void LogTdelta(double delta)
    {
        fsi << "TIMEDELTA MS:" << delta / 1000 << std::endl;
    };

}
#endif