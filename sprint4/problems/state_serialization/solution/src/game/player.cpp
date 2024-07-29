#include "game.h"
#include "../tokenizer/tokenizer.h"
#include "../req_helper/model_help.h"

using namespace model;

model::DogCoordinates GenerateRandomDogPosition(const SortedSessionRoads &graph)
{
    auto randomroad = Gen::GENERATOR.RoadPositionRandom(graph);

#ifdef LOGGING
    LogRandomRoad(randomroad);
#endif

#ifdef FORCE
    return {0, 0};
#else
    return Gen::GENERATOR.GetRandomRoadPoint(randomroad);
#endif
}

// PLAYER
namespace api
{
    uint32_t Player::ids_;
    Player::Player(std::string dog,
                   std::shared_ptr<model::GameSession> session,
                   const SortedSessionRoads &s_roads,
                   bool rand_dog)

        : session_(session),
          sorted_roads_(s_roads),
          need_randomize_dog_(rand_dog),
          my_dog_(std::make_shared<model::Dog>(dog)),
          token_(Gen::GENERATOR.GenerateHEXToken()),
          id_(ids_++)
    {

        DogCoordinates rand_coord;
        if (rand_dog)
        {
            rand_coord = GenerateRandomDogPosition(sorted_roads_);
        }
        else
        {
            const auto &roads = session_->GetMap().GetRoads();
            if (roads.empty())
            {
                throw std::logic_error("No roads in session");
            }
            auto st_crd = roads[0]->GetStart();
            rand_coord = {static_cast<double>(st_crd.x), static_cast<double>(st_crd.y)};
        }
        my_dog_->SetDogCoordinates(rand_coord);

#ifdef LOGGING
        LogToCURL(dog, token_, coord);
#endif
    }

    const std::string &Player::GetToken() const
    {
        return token_;
    }

    uint32_t Player::GetId() const
    {
        return id_;
    }

    std::shared_ptr<model::Dog> Player::PlayersDog() const
    {
        return my_dog_;
    }

    std::shared_ptr<model::GameSession> Player::PlayersSession() const
    {
        return session_;
    }

    uint32_t Player::GetDogId() const
    {
        return my_dog_->GetId();
    }

     uint32_t Player::GetPlayersCount()
    {
        return ids_;
    }

    bool Player::NeedRandom() const
    {
        return need_randomize_dog_;
    }

    Player::Player(std::shared_ptr<model::GameSession> session,
                   const SortedSessionRoads &s_roads,
                   bool rand_dog,
                   std::shared_ptr<model::Dog> sobaka,
                   std::string tok,
                   uint32_t id

                   ) : session_(session),
                       sorted_roads_(s_roads),
                       need_randomize_dog_(rand_dog),
                       my_dog_(sobaka),
                       token_(std::move(tok)),
                       id_(id)
    {
    }

    void Player::SetStaticPlayersCount(uint32_t plc)
    {
        ids_ = plc;
    }

};