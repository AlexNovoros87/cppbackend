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

        Dog(std::string name)
        {
            if (name == "")
                throw std::logic_error("Dog without name??");
            name_ = std::move(name);
            id_=ids_++;
        }

        const std::string &GetName() const
        {
            return name_;
        }

        int GetId() const
        {
            return id_;
        }

        void ChangeName(std::string name)
        {
            name_ = std::move(name);
        }

    private:
        std::string name_;
        static uint32_t ids_;
        uint32_t id_;
    };
    // uint32_t Dog::id_;

    class GameSession
    {
    public:
        GameSession(Map mp) : map_(std::move(mp)){};

        std::shared_ptr<Dog> JoinGame(std::shared_ptr<Dog> dog_ptr)
        {
            auto id = dog_ptr->GetId();
    
            dogs_[id] = dog_ptr;
            ++players;
       
            return dogs_.at(id);
        }

        void LeaveGame(uint32_t id)
        {
            if (dogs_.count(id) > 0)
                dogs_.erase(id);
            --players;
        }
        const std::map<uint32_t, std::shared_ptr<Dog>> &GetDogs() const
        {
            return dogs_;
        }

        void PrintDogs() const
        {
            for (auto &&dog : dogs_)
            {
                std::cout << "id: " << dog.first << " name: " << dog.second->GetName() << std::endl;
            }
        }

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

        const Maps &GetMaps() const noexcept
        {
            return maps_;
        }

        const Map *FindMap(const Map::Id &id) const noexcept
        {
            if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end())
            {
                return &maps_.at(it->second);
            }
            return nullptr;
        }

    private:
        using MapIdHasher = util::TaggedHasher<Map::Id>;
        using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

        std::vector<Map> maps_;
        MapIdToIndex map_id_to_index_;
    };

}