#include "game.h"

namespace api
{

    PlayObserver::PlayObserver(const Play &play) : play_(play)
    {
    }
    // ПЕЧАТАЕТ ДОРОГИ ИГРЫ
    void PlayObserver::PrintSessionRoads() const /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        for (auto session : play_.sorted_by_type_roads_)
        {
            std::cout << "Session name: " << session.first << std::endl
                      << "HORIZONTAL:" << std::endl;
            if (session.second.count(RoadOrient::HORIZONTAL))
            {
                for (auto &&road : session.second.at(RoadOrient::HORIZONTAL))
                {
                    std::cout << *road << std::endl;
                }
            };
            std::cout << "VERTICAL:" << std::endl;
            if (session.second.count(RoadOrient::VERTRICAL))
            {
                for (auto &&road : session.second.at(RoadOrient::VERTRICAL))
                {
                    std::cout << *road << std::endl;
                }
            };
            std::cout << "------------------------------------------------------------" << std::endl
                      << std::endl
                      << std::endl;
        }
    }
    // ПЕЧАТАЕТ СЕССИИ ИГРЫ
    void PlayObserver::PrintSessions() const
    {
        for (auto &&session : play_.sessions_)
        {
            std::cout << "stored_by: " << session.first << std::endl;
            std::cout << "Name: " << session.second->GetMap().GetName() << " ID: " << *session.second->GetMap().GetId() << std::endl;
            session.second->PrintDogs();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////
    const decltype(PlayObserver::play_.sorted_by_type_roads_) &PlayObserver::SortedRoads() const
    {
        return play_.sorted_by_type_roads_;
    }

    const decltype(PlayObserver::play_.token_players_) &PlayObserver::GetTokenPlayers() const
    {
        return play_.token_players_;
    }

    /////////////////////////////////////////

    const decltype(PlayObserver::play_.sessions_) &PlayObserver::GetSessionList() const
    {
        return play_.sessions_;
    }
    /////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////

    // ВОЗВРАЩАЕТ ИГРОКА ПО ЕГО ТОКЕНУ
    std::shared_ptr<Player> PlayObserver::FindByToken(const std::string &token) const
    {
        if (play_.token_players_.count(token) > 0)
            return play_.token_players_.at(token);
        return nullptr;
    }

    // ВОЗВРАЩАЕТ СЕССИЮ ПО ИМЕНИ
    std::shared_ptr<model::GameSession> PlayObserver::GetSession(const std::string &name) const
    {
        if (play_.sessions_.count(name) > 0)
        {
            return play_.sessions_.at(name);
        }
        return nullptr;
    }

    // ВОЗВРАЩАЕТ ВСЕ ИГРОВЫЕ СЕССИИ
    const std::unordered_map<std::string, std::shared_ptr<model::GameSession>> &PlayObserver::GameSessions() const
    {
        return play_.sessions_;
    }

    // ВОЗВРАЩАЕТ СКОРОСТЬ ПО УМОЛЧАНИЮ
    double PlayObserver::GetDefaultSpeed() const { return play_.def_speed_; }

    // ВОЗВРАЩАЕТ ИГРОФОЙ ГРАФ ТОЧЕК ДОРОГ
    const GameGraph &PlayObserver::Graph() const { return play_.graph_; }

    // ВОЗВРАЩАЕТ В АВТОМАТИЧЕСКОМ  ЛИ РЕЖИМЕ ПРОИСХОДИТ
    // УПРАВЛЕНИЕ ИГРОВЫМ ВРЕМЕНМ
    bool PlayObserver::AutoTick() const { return play_.auto_tick_; }

    // ВОЗВРАЩАЕТ НУЖНО ЛИ РАНДОМНО ГЕНЕРИРОВАТЬ СОБАК НА КАРТЕ
    bool PlayObserver::NeedRandom() const { return play_.random_; }

    // ИМЕЕТСЯ ЛИ СЕССИЯ С ТАКИМ ИМЕНЕМ
    bool PlayObserver::HavingSession(const std::string &session) const
    {
        return (play_.sessions_.count(session) > 0);
    };
    // ИМЕЕТСЯ ЛИ КАРТА С ТАКИМ ИМЕНЕМ
    bool PlayObserver::HavingMap(const std::string &map) const
    {
        return (play_.game_.FindMap(model::Map::Id(map)) != nullptr);
    }

    // ВОЗВРАЩАЕТ ДЖИСОН ПРЕДСТАВЛЕНИЕ О СЕССИЯХ И ИГРОКАХ ГДЕ СЕЙЧАС ИГРОК
    std::string PlayObserver::GetJSONSession(std::shared_ptr<Player> player) const
    {
        if (play_.token_players_.count(player->GetToken()) == 0)
            return {};

        auto dogs = player->PlayersSession()->GetDogs();
        boost::json::value obj{{"w", ""}};
        for (const auto &dog : dogs)
        {
            obj.as_object()[std::to_string(dog.second->GetId())] = dog.second->GetName();
        };
        obj.as_object().erase("w");
        return (boost::json::serialize(obj));
    };

    void PlayObserver::PrintPlayers() const
    {
        for (auto &&pl : play_.token_players_)
        {
            std::cout << "TOKEN:" << pl.first << "NAME:" << pl.second->PlayersDog()->GetName()
                      << std::endl << "LOOT:" << std::endl;
            pl.second->PlayersSession()->PrintDogs();
        }
    }

};
