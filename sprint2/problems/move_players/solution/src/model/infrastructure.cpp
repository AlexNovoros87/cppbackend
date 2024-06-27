#include "infrastructure.h"
namespace model
{
    using namespace std::literals;

    const Rectangle &Building::GetBounds() const noexcept
    {
        return bounds_;
    }

    const std::string &Building::GetJsonType() const
    {
        return json_;
    }

    Building::Building(Rectangle bounds) noexcept
        : bounds_{bounds}
    {
        json_.clear();
        json_.append("{ \"x\": ").append(std::to_string(bounds.position.x)).append(", ").append("\"y\": ").append(std::to_string(bounds.position.y)).append(", ").append("\"w\": ").append(std::to_string(bounds.size.width)).append(", ").append("\"h\": ").append(std::to_string(bounds.size.height)).append(" }");
    }
} // namespace model

// ROAD
namespace model
{
    Road::Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}, end_{end_x, start.y}
    {
        json_.clear();
        json_.append("{ \"x0\": ").append(std::to_string(start.x)).append(", \"y0\": ").append(std::to_string(start.y)).append(", \"x1\": ").append(std::to_string(end_x)).append(" }");
    }

    Road::Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}, end_{start.x, end_y}
    {
        json_.clear();
        json_.append("{ \"x0\": ").append(std::to_string(start.x)).append(", \"y0\": ").append(std::to_string(start.y)).append(", \"y1\": ").append(std::to_string(end_y)).append(" }");
    }

    const std::string &Road::GetJsonType() const
    {
        return json_;
    }

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

}
// OFFICE
namespace model
{

    Office::Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}, position_{position}, offset_{offset}
    {
        json_.clear();
        json_.append("{ \"id\": \"").append(*id_).append("\", \"x\": ").append(std::to_string(position.x)).append(", \"y\": ").append(std::to_string(position.y)).append(", \"offsetX\": ").append(std::to_string(offset.dx)).append(", \"offsetY\": ").append(std::to_string(offset.dy)).append(" }");
    }

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

    const std::string &Office::GetJsonType() const
    {
        return json_;
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

    Map::Map(Id id, std::string name, double speed) noexcept
        : id_(std::move(id)), name_(std::move(name)), map_spd_(speed)
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
        roads_.emplace_back(road);
    }

    void Map::AddBuilding(const Building &building)
    {
        buildings_.emplace_back(building);
    }

}
std::ostream& operator<<(std::ostream& os,const model::Road& road){
 os<<"Road Start: "<<road.GetStart()<<"  End: "<<road.GetEnd()<<"   ";
 if(road.IsHorizontal()) os<< "HORIZONTAL"; else os<< "VERTICAL"; os;
return os;
}