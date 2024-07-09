#include "game_dog_session.h"
using namespace std::literals;
// GAME
namespace model
{

    void Game::AddMap(Map map)
    {
        const size_t index = maps_.size();
        if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted)
        {
            throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
        }
        else
        {
            try
            {
                maps_.emplace_back(std::move(map));
            }
            catch (...)
            {
                map_id_to_index_.erase(it);
                throw;
            }
        }
    }

    const Map *Game::FindMap(const Map::Id &id) const noexcept
    {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end())
        {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

    const Game::Maps &Game::GetMaps() const noexcept
    {
        return maps_;
    }

}

// GAMESESS
namespace model
{

    GameSession::GameSession(Map mp) : map_(std::move(mp)){};
    std::shared_ptr<Dog> GameSession::JoinGame(std::shared_ptr<Dog> dog_ptr)
    {
        auto id = dog_ptr->GetId();
        dogs_[id] = dog_ptr;
        ++players;
        return dogs_.at(id);
    }

    void GameSession::LeaveGame(uint32_t id)
    {
        if (dogs_.count(id) > 0)
            dogs_.erase(id);
        --players;
    }
    const std::map<uint32_t, std::shared_ptr<Dog>> &GameSession::GetDogs() const
    {
        return dogs_;
    }

    void GameSession::PrintDogs() const
    {
        std::cout << "MAP SPEED IS " << GetSessionSpeed() << std::endl;
        for (auto &&dog : dogs_)
        {
            std::cout << "id: " << dog.first << " name: " << dog.second->GetName() << std::endl;
        }
    }

   
    double GameSession::GetSessionSpeed() const
    {
        return map_.GetMapSpeed();
    }

    const model::Map &GameSession::GetMap() const
    {
        return map_;
    }

}

// DOG
namespace model
{
    uint32_t Dog::ids_;
    const DogCoordinates &Dog::GetDogCoord() const { return coordinates_; };
    const DogSpeed &Dog::GetDodSpd() const { return speed_; };
    NSWE_Direction Dog::GetOrientation() const { return direction_; }

    Dog::Dog(std::string name)
    {

        name_ = std::move(name);
        id_ = ids_++;
    }

    const std::string &Dog::GetName() const { return name_; }

    int Dog::GetId() const { return id_; }

    void Dog::ChangeName(std::string name) { name_ = std::move(name); }

    void Dog::SetDogSpeedAndDirection(DogSpeed spd, NSWE_Direction dir)
    {
        if(dir == NSWE_Direction::STOP){
            StopDog();
            return;
        }
        
        speed_ = std::move(spd);
        direction_ = dir;
        
    }
    DogCoordinates &Dog::SetDogCoordinates(double x, double y)
    {
        coordinates_ = {x, y};
        return coordinates_;
    }

    DogCoordinates &Dog::SetDogCoordinates(DogCoordinates coord)
    {
        coordinates_ = std::move(coord);
        return coordinates_;
    }

}