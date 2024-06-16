#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "tagged.h"

namespace model
{
    using Dimension = int;
    using Coord = Dimension;

    struct Point
    {
        Coord x, y;
    };

    struct Size
    {
        Dimension width, height;
    };

    struct Rectangle
    {
        Point position;
        Size size;
    };

    struct Offset
    {
        Dimension dx, dy;
    };

    class Road
    {
        struct HorizontalTag
        {
             HorizontalTag() = default;
        };

        struct VerticalTag
        {
             VerticalTag() = default;
        };

    public:
        constexpr static HorizontalTag HORIZONTAL{};
        constexpr static VerticalTag VERTICAL{};

        Road(HorizontalTag, Point start, Coord end_x) noexcept;
        Road(VerticalTag, Point start, Coord end_y) noexcept;

        const std::string &GetJsonType() const
        {
            return json_;
        }

        bool IsHorizontal() const noexcept
        {
            return start_.y == end_.y;
        }

        bool IsVertical() const noexcept
        {
            return start_.x == end_.x;
        }

        Point GetStart() const noexcept
        {
            return start_;
        }

        Point GetEnd() const noexcept
        {
            return end_;
        }

    private:
        Point start_;
        Point end_;
        std::string json_;
    };

    class Building
    {
    public:
        explicit Building(Rectangle bounds) noexcept;

        const Rectangle &GetBounds() const noexcept
        {
            return bounds_;
        }

        const std::string &GetJsonType() const
        {
            return json_;
        }

    private:
        Rectangle bounds_;
        std::string json_;
    };

    class Office
    {
    public:
        using Id = util::Tagged<std::string, Office>;

        Office(Id id, Point position, Offset offset) noexcept;
        const Id &GetId() const noexcept
        {
            return id_;
        }

        Point GetPosition() const noexcept
        {
            return position_;
        }

        Offset GetOffset() const noexcept
        {
            return offset_;
        }

        const std::string &GetJsonType() const
        {
            return json_;
        }

    private:
        Id id_;
        Point position_;
        Offset offset_;
        std::string json_;
    };

    class Map
    {
    public:
        using Id = util::Tagged<std::string, Map>;
        using Roads = std::vector<Road>;
        using Buildings = std::vector<Building>;
        using Offices = std::vector<Office>;

        Map(Id id, std::string name) noexcept
            : id_(std::move(id)), name_(std::move(name))
        {
        }

        const Id &GetId() const noexcept
        {
            return id_;
        }

        const std::string &GetName() const noexcept
        {
            return name_;
        }

        const Buildings &GetBuildings() const noexcept
        {
            return buildings_;
        }

        const Roads &GetRoads() const noexcept
        {
            return roads_;
        }

        const Offices &GetOffices() const noexcept
        {
            return offices_;
        }

        void AddRoad(const Road &road)
        {
            roads_.emplace_back(road);
        }

        void AddBuilding(const Building &building)
        {
            buildings_.emplace_back(building);
        }

        void AddOffice(Office office);

    private:
        using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

        Id id_;
        std::string name_;
        Roads roads_;
        Buildings buildings_;

        OfficeIdToIndex warehouse_id_to_index_;
        Offices offices_;
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

} // namespace model