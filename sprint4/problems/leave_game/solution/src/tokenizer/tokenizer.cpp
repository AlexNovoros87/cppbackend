#include "tokenizer.h"

using namespace model;
std::pair<bool, bool> all_false{false, false};

std::pair<bool, bool> CheckCorrectroads(const SortedSessionRoads &roads)
{

  std::pair<bool, bool> answer = all_false;
  if (roads.count(model::RoadOrient::VERTRICAL) == 0 && roads.count(model::RoadOrient::HORIZONTAL) == 0)
    return all_false;

  if (roads.count(model::RoadOrient::VERTRICAL) > 0)
  {
    if (!roads.at(model::RoadOrient::VERTRICAL).empty())
      answer.first = true;
  }

  if (roads.count(model::RoadOrient::HORIZONTAL) > 0)
  {
    if (!roads.at(model::RoadOrient::HORIZONTAL).empty())
      answer.second = true;
  }
  return answer;
}

std::string PlayerTokens::GenerateHEXToken()
{
  uint64_t first = generator1_();
  uint64_t second = generator2_();
  return NumToHex(first) + NumToHex(second);
}

std::vector<std::pair<model::LootCoordinates, size_t>>
PlayerTokens::GenerateLoot(const SortedSessionRoads &roads, size_t objects_need, const std::string &map_name)
{

  // В объекте для фронтенда ищем контейнер относящийся к нашей гейм-сессии, нам будет нужен его размер
  auto &loot_container = frontend::FrontEndLoot::GetLootToFrontend(map_name);
  if (loot_container.empty()){
    throw(std::logic_error("Impossible to generate. Loot container at your nsme is empty"));}

  // Обьявляем Координаты - номер в контейнере
  std::vector<std::pair<model::LootCoordinates, size_t>> object_list;

  // objects_need - сколько рандомных пар сгенерировать
  for (size_t i = 0; i < objects_need; ++i)
  {
    // рандом дороги
    auto needed_road = RoadPositionRandom(roads);
    // рандом координат на дороге
    auto need_coordinates = GetRandomRoadPoint(needed_road);
    // рандом порядкового номера лута из списка
    size_t loot_num = RandomGenerate(size_t(0), loot_container.size());
    object_list.push_back({need_coordinates, loot_num});
  }
  return object_list;
}

std::shared_ptr<model::Road> PlayerTokens::RoadPositionRandom(const SortedSessionRoads &roads)
{

  auto check = CheckCorrectroads(roads);
  if (check == all_false){
    throw std::logic_error("NO ROADS");}

  size_t value = 0;
  uint64_t generated = generator1_();
  uint64_t generated_to_delimeter = generator2_();

  RoadOrient orient = RoadOrient::VERTRICAL;
  if (check.first && check.second)
  {
    orient = generated % 2 == 0 ? RoadOrient::HORIZONTAL : RoadOrient::VERTRICAL;
  }
  else if (check.first)
  {
    orient = RoadOrient::HORIZONTAL;
  }
  value = roads.at(orient).size();

  std::uniform_int_distribution<> dis(0, value - 1);
  size_t index = dis(generator1_);
  return roads.at(orient)[index];
}

std::string PlayerTokens::NumToHex(uint64_t num)
{
  std::stringstream ss;
  ss << std::hex << num;
  std::stringstream ss2;
  ss2 << std::setw(16) << std::setfill('0') << ss.str();
  return ss2.str();
}

model::DogCoordinates PlayerTokens::GetRandomRoadPoint(std::shared_ptr<model::Road> road)
{
  const auto &begin = road->GetStart();
  const auto &end = road->GetEnd();

  double min, max;
  model::DogCoordinates result;

  if (road->IsVertical())
  {
    result.x = static_cast<double>(begin.x);
    min = std::min(begin.y, end.y);
    max = std::max(begin.y, end.y);
    result.y = RandomGenerate(min, max);
  }
  else
  {
    result.y = static_cast<double>(begin.y);
    min = std::min(begin.x, end.x);
    max = std::max(begin.x, end.x);
    result.x = RandomGenerate(min, max);
  }

  return result;
};

PlayerTokens Gen::GENERATOR;