#pragma once
#include "../game/game.h"

using SessionGraph = std::unordered_map<double, std::unordered_map<double, std::vector<std::shared_ptr<model::Road>>>>;

namespace model
{
    DogSpeed GetDogSpeedByDrection(std::string_view direction, double speed);
    std::pair<DogCoordinates, Limits> GetNewCoordinatesAndLimits(const GameSession &game, std::shared_ptr<Dog> dog, const SessionGraph &graph, double delta_t);
    DogCoordinates MoveDogDeltaT(std::shared_ptr<Dog> dog, double delta_t, Limits lim);
    std::chrono::milliseconds ConvertDoubleToMS(double period);
   
#ifdef LOGGING
    void LogWas(std::string foo, std::shared_ptr<model::Dog> dog);
    void LogNow(std::string foo, std::shared_ptr<model::Dog> dog);
    void LogLimits(const Limits &lim);
    void LogEndls();
    void LogLine();
    void LogTdelta(double delta);
#endif
}