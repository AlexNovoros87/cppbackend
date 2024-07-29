#pragma once
#include "infrastructure.h"
#include <stdexcept>
#include <memory>
#include <iostream>
#include <functional>
#include <map>
#include <mutex>

namespace model
{
    struct Loot{
       size_t id;
       size_t type;
    };
    
    class Dog
    {

    public:
        Dog() = default;
        explicit Dog(std::string name);
        const std::string &GetName() const;
        int GetId() const;
        void ChangeName(std::string name);
        const DogCoordinates &GetDogCoord() const;
        const DogSpeed &GetDodSpd() const;
        void SetDogSpeedAndDirection(DogSpeed spd, NSWE_Direction dir);
        DogCoordinates &SetDogCoordinates(double x, double y);
        DogCoordinates &SetDogCoordinates(DogCoordinates coord);
        NSWE_Direction GetOrientation() const;
        void IsNeededTStopDog(const DogCoordinates &coords, const Limits &lim);
        void StopDog();
        void PickUpLoot(Loot loot){
            bag_.push_back(std::move(loot));
        }
        void ThrowLoot(){
          bag_.clear();
        }

        const std::vector<Loot>& GetLootBag(){
            return bag_;
        }

    private:
        std::string name_;
        static uint32_t ids_;
        uint32_t id_;
        DogCoordinates coordinates_{0., 0.};
        DogSpeed speed_{0., 0.};
        NSWE_Direction direction_ = NSWE_Direction::NORTH;
        std::vector<Loot> bag_;
       

    };

    class GameSession
    {
    private:
      struct Hasher
        {
            size_t operator()(const LootCoordinates& pos) const
            {
                size_t h1 = hasher_(pos.x);
                size_t h2 = hasher_(pos.y);
                return h1 + h2 * 37.;
            };
        
        private:
        std::hash<double> hasher_;
        };
    
     public:
        
        explicit GameSession(Map mp);
        std::shared_ptr<Dog> JoinGame(std::shared_ptr<Dog> dog_ptr);
        void LeaveGame(uint32_t id);
        const std::map<uint32_t, std::shared_ptr<Dog>> &GetDogs() const;
        void PrintDogs() const;
        double GetSessionSpeed() const;
        const model::Map &GetMap() const;
        void AddLoot(LootCoordinates coords, size_t num);
        const std::unordered_map<LootCoordinates, Loot , Hasher>& GetLootList() const;
        std::unordered_map<LootCoordinates, Loot , Hasher>& GetLootListNotConst(){
            return loot_obj_;
        }
        
    private:
        model::Map map_;
        std::map<uint32_t, std::shared_ptr<Dog>> dogs_;
        int players = 0;
        std::unordered_map<LootCoordinates, Loot, Hasher> loot_obj_;
        size_t add_loot_order_ = 0;
    };

    class Game
    {
    public:
        using Maps = std::vector<Map>;
        void AddMap(Map map);
        const Maps &GetMaps() const noexcept;
        const Map *FindMap(const Map::Id &id) const noexcept;

    private:
        using MapIdHasher = util::TaggedHasher<Map::Id>;
        using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
        std::vector<Map> maps_;
        MapIdToIndex map_id_to_index_;
    };

}