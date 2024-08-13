#pragma once
#include "infrastructure.h"
#include <stdexcept>
#include <memory>
#include <iostream>
#include <functional>
#include <map>

namespace serialization{
class DogRepr;
class ArchiveSession;
}


namespace model
{
    struct Loot
    {
        size_t id;
        size_t type;
        bool operator == (const Loot& rhs) const{
          return (id == rhs.id && type == rhs.type);
        }
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
        bool IsNeededTStopDog(const DogCoordinates &coords, const Limits &lim);
        void StopDog();
        void TakeScores(int score);
        void PickUpLoot(Loot loot);
        void ThrowLoot();
        const std::vector<Loot> &GetLootBag() const;
        int Score() const;
        int DogsCount() const { return ids_; }
        std::string GrabName();

    private:
       friend class serialization::DogRepr;

       void RestoreId(uint32_t id)
        {
            id_ = id;
        }

        static void RestoreIdStatic(uint32_t id)
        {
            ids_ = id;
        }

        void RestoreLoot(std::vector<Loot> bag)
        {
            bag_ = std::move(bag);
        }

        std::string name_;
        uint32_t id_;
        DogCoordinates coordinates_{0., 0.};
        DogSpeed speed_{0., 0.};
        NSWE_Direction direction_ = NSWE_Direction::NORTH;
        std::vector<Loot> bag_;
        int score_ = 0;
        static uint32_t ids_;

        
    };

    class GameSession
    {
     public:
        struct Hasher
        {
            size_t operator()(const LootCoordinates &pos) const
            {
               // Комбинируем хэш значения x и y
               size_t hash = std::hash<double>()(pos.x * 37. + pos.y);
               return hash;
            };
        private:
        
        
        };

        explicit GameSession(Map mp);
        std::shared_ptr<Dog> JoinGame(std::shared_ptr<Dog> dog_ptr);
        void LeaveGame(uint32_t id);
        const std::map<uint32_t, std::shared_ptr<Dog>> &GetDogs() const;
        void PrintDogs() const;
        double GetSessionSpeed() const;
        const model::Map &GetMap() const;
        void AddLoot(LootCoordinates coords, size_t num);
        const std::unordered_map<LootCoordinates, Loot, Hasher> &GetLootList() const;
        std::unordered_map<LootCoordinates, Loot, Hasher> &GetLootListNotConst() { return loot_obj_; }
        
        
        size_t GetLootOrder() const{
            return add_loot_order_;
        } 

         size_t GetPlayersCount() const{
            return players;
        } 

    private:
       friend class serialization::ArchiveSession;
       void RestoreSession(std::map<uint32_t, std::shared_ptr<Dog>> dogs,
                            int pl, 
                            std::unordered_map<LootCoordinates, Loot, GameSession::Hasher> loot_obj,
                            size_t add_loot_order){

             dogs_ = std::move(dogs);
             players = pl;
             loot_obj_ = std::move(loot_obj);
             add_loot_order_ = add_loot_order;
       
       }
       
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

std::ostream& operator<<(std::ostream& os, model::Loot lt);