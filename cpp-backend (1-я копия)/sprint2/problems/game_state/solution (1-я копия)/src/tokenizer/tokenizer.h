#pragma once
#include <random>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../model/tagged.h"
#include "../model/infrastructure.h"

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
    std::string GenerateHEXToken() ;
    uint64_t RoadPositionRandom(uint64_t value);
    model::DogCoordinates GetRandomRoadPoint(const model::Road& road);

private:
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



class Gen{
  public:
  static PlayerTokens GENERATOR;
};
 