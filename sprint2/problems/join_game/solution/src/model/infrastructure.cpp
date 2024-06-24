#include "infrastructure.h"
namespace model
{
    using namespace std::literals;

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

    
    Road::Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}, end_{end_x, start.y}
    {
        json_.clear();
        json_.append("{ \"x0\": ").append(std::to_string(start.x)).append(", \"y0\": ").append(std::to_string(start.y))
             .append(", \"x1\": ").append(std::to_string(end_x)).append(" }");
    }

    Road::Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}, end_{start.x, end_y}
    {
        json_.clear();
        json_.append("{ \"x0\": ").append(std::to_string(start.x)).append(", \"y0\": ").append(std::to_string(start.y))
             .append(", \"y1\": ").append(std::to_string(end_y)).append(" }");
    }

    Building::Building(Rectangle bounds) noexcept
        : bounds_{bounds}
    {
        json_.clear();
        json_.append("{ \"x\": ").append(std::to_string(bounds.position.x)).append(", ").append("\"y\": ")
             .append(std::to_string(bounds.position.y)).append(", ").append("\"w\": ").append(std::to_string(bounds.size.width))
             .append(", ").append("\"h\": ").append(std::to_string(bounds.size.height)).append(" }");
    }

    Office::Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}, position_{position}, offset_{offset}
    {
        json_.clear();
        json_.append("{ \"id\": \"").append(*id_).append("\", \"x\": ").append(std::to_string(position.x))
             .append(", \"y\": ").append(std::to_string(position.y)).append(", \"offsetX\": ")
             .append(std::to_string(offset.dx)).append(", \"offsetY\": ").append(std::to_string(offset.dy)).append(" }");
    }

} // namespace model
