#include "infrastructure.h"

namespace frontend
{
    std::unordered_map<std::string, boost::json::array> FrontEndLoot::frontend_loot_;
    boost::json::array FrontEndLoot::empty_;
    //ПРОВЕРКИ ВКЛЮЧАЮТСЯ ВКЛЮЧЕНИЕМ МАКРОСА НО СУЩЕСТВЕННО ПОНИЗЯТ ПРОИЗВОДИТЕЛЬНОСТЬ...
 
        void FrontEndLoot::AddMapLoot(std::string map_name, boost::json::array arr)
        {
            frontend_loot_.insert({std::move(map_name), std::move(arr)});
        }
        
        const boost::json::array& FrontEndLoot::GetLootToFrontend(const std::string &map_name)
        {
#ifndef MAX_SPEED_RESPONSE
            if (frontend_loot_.count(map_name) == 0)
                return empty_;
#endif
            return frontend_loot_.at(map_name);
        }

        int FrontEndLoot::GetPriceScore(const std::string &map_name, size_t order_num)
        {
#ifndef MAX_SPEED_RESPONSE
            if (frontend_loot_.count(map_name) == 0)
                return 0;
            if (frontend_loot_.at(map_name).empty())
                return 0;
            if (frontend_loot_.at(map_name).size() - 1 < order_num)
                return 0;
            if (frontend_loot_.at(map_name)[order_num].as_object().count("value") == 0)
                return 0;
            if (!frontend_loot_.at(map_name)[order_num].as_object().at("value").is_number())
                return 0;
#endif

           return static_cast<int>(frontend_loot_.at(map_name)[order_num].as_object().at("value").as_int64());
        }
  
}

namespace model
{
    using namespace std::literals;

    const Rectangle &Building::GetBounds() const noexcept
    {
        return bounds_;
    }

    Building::Building(Rectangle bounds) noexcept
        : bounds_{bounds}
    {}
} // namespace model

// ROAD
namespace model
{
    Road::Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}, end_{end_x, start.y}
    {}

    Road::Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}, end_{start.x, end_y}
    {}

   

    bool Road::IsHorizontal() const noexcept
    {
        return start_.y == end_.y;
    }

    bool Road::IsVertical() const noexcept
    {
        return start_.x == end_.x;
    }

    Point Road::GetStart() const noexcept
    {
        return start_;
    }

    Point Road::GetEnd() const noexcept
    {
        return end_;
    }

    bool Road::CheckValidComparison(const Road &rhs) const
    {
        return ((this->IsHorizontal() && rhs.IsHorizontal()) || (this->IsVertical() && rhs.IsVertical()));
    }

    bool Road::operator<(const Road &rhs) const
    {
        if (!CheckValidComparison(rhs))
            throw std::logic_error("Vertical and Horisontal can nor be compared");
        if (this->IsVertical())
        {
            return (this->GetStart().y < rhs.GetStart().y);
        }
        else
        {
            return (this->GetStart().x < rhs.GetStart().x);
        }
        return false;
    }

    bool Road::operator>(const Road &rhs) const
    {
        return !(*this < rhs);
    };

    bool Road::operator==(const Road &rhs) const
    {
        return ((GetStart() == rhs.GetStart()) && (GetEnd() == rhs.GetEnd()));
    }

    bool Road::operator!=(const Road &rhs) const
    {
        return !(*this == rhs);
    }

}
// OFFICE
namespace model
{

    Office::Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}, position_{position}, offset_{offset}
    {}

    const Office::Id &Office::GetId() const noexcept
    {
        return id_;
    }

    Point Office::GetPosition() const noexcept
    {
        return position_;
    }

    Offset Office::GetOffset() const noexcept
    {
        return offset_;
    }
}

// MAP
namespace model
{

    void Map::AddOffice(Office office)
    {
        if (warehouse_id_to_index_.contains(office.GetId()))
        {
            throw std::invalid_argument("Duplicate warehouse");
        }

        const size_t index = offices_.size();
        Office &o = offices_.emplace_back(std::move(office));
        try
        {
            warehouse_id_to_index_.emplace(o.GetId(), index);
        }
        catch (...)
        {
            // Удаляем офис из вектора, если не удалось вставить в unordered_map
            offices_.pop_back();
            throw;
        }
    }

    Map::Map(Id id, std::string name, double speed , size_t bag_capacity) noexcept
        : id_(std::move(id)), name_(std::move(name)), map_spd_(speed) , bag_capacity_(bag_capacity)
    {
    }

    const Map::Id &Map::GetId() const noexcept
    {
        return id_;
    }

    const std::string &Map::GetName() const noexcept
    {
        return name_;
    }

    const Map::Buildings &Map::GetBuildings() const noexcept
    {
        return buildings_;
    }

    const Map::Roads &Map::GetRoads() const noexcept
    {
        return roads_;
    }

    const Map::Offices &Map::GetOffices() const noexcept
    {
        return offices_;
    }

    void Map::AddRoad(const Road &road)
    {

        if (road.IsHorizontal())
        {

            assert(road.GetStart().y == road.GetEnd().y);
            roads_.push_back(std::make_shared<Road>(road));
        }
        else
        {
            assert(road.GetStart().x == road.GetEnd().x);
            roads_.push_back(std::make_shared<Road>(road));
        }
    }

    void Map::AddBuilding(const Building &building)
    {
        buildings_.emplace_back(building);
    }

  

    double Map::ChangeSpeed(double speed)
    {
        map_spd_ = speed;
        return map_spd_;
    }

    double Map::GetMapSpeed() const
    {
        return map_spd_;
    }

}
std::ostream &operator<<(std::ostream &os, const model::Road &road)
{

    if (road.IsHorizontal())
    {

        assert(road.GetStart().y == road.GetEnd().y);
        os << "Road Start X: " << std::min(road.GetStart().x, road.GetEnd().x) << "  End X: " << std::max(road.GetStart().x, road.GetEnd().x) << " Y: " << road.GetEnd().y << "  HORIZONTAL";
    }
    else
    {
        assert(road.GetStart().x == road.GetEnd().x);
        os << "Road Start Y: " << std::min(road.GetStart().y, road.GetEnd().y) << "  End Y: " << std::max(road.GetStart().y, road.GetEnd().y) << " X: " << road.GetEnd().x << "  VERTICAL";
    }
    return os;
}


