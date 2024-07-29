#pragma once
#include <random>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../model/tagged.h"
#include "../game/game.h"


///!!!!!!!!!!!!!DogCoordinates и LootCoordinates одна и та же структура

using SortedSessionRoads = std::unordered_map<model::RoadOrient, std::vector<std::shared_ptr<model::Road>>>;
namespace detail
{
    struct TokenTag
    {
    };
} // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;

class PlayerTokens
{
public:
    std::string GenerateHEXToken();
    std::shared_ptr<model::Road> RoadPositionRandom(const SortedSessionRoads &roads);
    model::DogCoordinates GetRandomRoadPoint(std::shared_ptr<model::Road> road);

    std::vector<std::pair<model::LootCoordinates, size_t>> 
    GenerateLoot(const SortedSessionRoads &roads, size_t objects_need, const std::string &map_name);
    

private:
   
   
    template <typename T>
    T RandomGenerate(T min, T max)
    {
        std::uniform_real_distribution<> dis(min, max);
        return dis(generator1_);
    }

    std::string NumToHex(uint64_t num);

    std::random_device random_device_;
    std::mt19937_64 generator1_{[this]
                                {
                                    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
                                    return dist(random_device_);
                                }()};
    std::mt19937_64 generator2_{[this]
                                {
                                    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
                                    return dist(random_device_);
                                }()};
    // Чтобы сгенерировать токен, получите из generator1_ и generator2_
    // два 64-разрядных числа и, переведя их в hex-строки, склейте в одну.
    // Вы можете поэкспериментировать с алгоритмом генерирования токенов,
    // чтобы сделать их подбор ещё более затруднительным
};

class Gen
{
public:
    static PlayerTokens GENERATOR;
};
