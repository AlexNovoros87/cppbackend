#pragma once
#include "infrastructure.h"
#include <stdexcept>
#include <memory>
#include <iostream>
#include <iostream>
#include <map>
#include <mutex>

namespace model
{

   class Dog
    {

    public:
        Dog() = default;
        Dog(std::string name);
        const std::string &GetName() const;
        int GetId() const;
        void ChangeName(std::string name);
        const DogCoordinates& GetDogCoord() const;
        const DogSpeed& GetDodSpd() const;
        void SetDogSpeedAndDirection(DogSpeed spd, NSWE_Direction dir);
        DogCoordinates& SetDogCoordinates(double x, double y);
        DogCoordinates& SetDogCoordinates(DogCoordinates coord);
        void IsNeededTStopDog(const DogCoordinates& coords, const Limits &lim)
        {  
             if(IsLessEqual(coords.x , lim.xmin) &&   std::isless(speed_.speed_x , 0.)){StopDog();}
             if(IsGreaterEqual(coords.x , lim.xmax) && std::isgreater(speed_.speed_x, 0.) ){StopDog();}
             if(IsLessEqual (coords.y  , lim.ymin)  && std::isless(speed_.speed_y , 0.)){StopDog();}
             if(IsGreaterEqual( coords.y  , lim.ymax)  &&  std::isgreater(speed_.speed_y , 0. )){StopDog();}
                  
        }
        NSWE_Direction GetOrientation() const;
        void StopDog(){
            speed_ = {0.,0.};
        } 
   
    private: 
        std::string name_;
        static uint32_t ids_;
        uint32_t id_;
        DogCoordinates coordinates_{0.,0.};
        DogSpeed speed_ {0.,0.};
        NSWE_Direction direction_ = NSWE_Direction::NORTH;
      
    };
    

    class GameSession
    {
    public:
        GameSession(Map mp);
        std::shared_ptr<Dog> JoinGame(std::shared_ptr<Dog> dog_ptr);
        void LeaveGame(uint32_t id);
        const std::map<uint32_t, std::shared_ptr<Dog>> &GetDogs() const;
        void PrintDogs() const;
        double GetSessionSpeed() const;
        const model::Map& GetMap() const;
  private:
        model::Map map_;
        std::map<uint32_t, std::shared_ptr<Dog>> dogs_;
        int players = 0;
      
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