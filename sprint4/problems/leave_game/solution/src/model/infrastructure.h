#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <boost/json.hpp>
#include <set>
#include "tagged.h"
#include "../req_helper/declaration_structs.h"

// Класс предназначен для отдачи фронденду обьектов лута в виде JSON
namespace frontend
{
    //ПРОВЕРКИ ВКЛЮЧАЮТСЯ ВКЛЮЧЕНИЕМ МАКРОСА НО СУЩЕСТВЕННО ПОНИЗЯТ ПРОИЗВОДИТЕЛЬНОСТЬ...
    class FrontEndLoot
    {
    public:
        static void AddMapLoot(std::string map_name, boost::json::array arr);
        static const boost::json::array& GetLootToFrontend(const std::string &map_name);
        static int GetPriceScore(const std::string &map_name, size_t order_num);

    private:
        static std::unordered_map<std::string, boost::json::array> frontend_loot_;
        static boost::json::array empty_;
        FrontEndLoot() = delete;
    };
}

namespace model
{

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
        bool IsHorizontal() const noexcept;
        bool IsVertical() const noexcept;

        bool CheckValidComparison(const Road &rhs) const;
        bool operator<(const Road &rhs) const;
        bool operator>(const Road &rhs) const;
        bool operator==(const Road &rhs) const;
        bool operator!=(const Road &rhs) const;

        Point GetStart() const noexcept;
        Point GetEnd() const noexcept;

    private:
        Point start_;
        Point end_;
    };

    class Building
    {
    public:
        explicit Building(Rectangle bounds) noexcept;
        const Rectangle &GetBounds() const noexcept;
      

    private:
        Rectangle bounds_;
    };

    class Office
    {
    public:
        using Id = util::Tagged<std::string, Office>;

        Office(Id id, Point position, Offset offset) noexcept;
        const Id &GetId() const noexcept;
        Point GetPosition() const noexcept;
        Offset GetOffset() const noexcept;

    private:
        Id id_;
        Point position_;
        Offset offset_;
       
    };

    class Map
    {
    public:
        using Id = util::Tagged<std::string, Map>;
        using Roads = std::vector<std::shared_ptr<Road>>;
        using Buildings = std::vector<Building>;
        using Offices = std::vector<Office>;

        Map(Id id, std::string name, double speed, size_t bag_capacity) noexcept;
        const Id &GetId() const noexcept;
        const std::string &GetName() const noexcept;
        const Buildings &GetBuildings() const noexcept;
        const Roads &GetRoads() const noexcept;
        const Offices &GetOffices() const noexcept;
        void AddRoad(const Road &road);
        void AddBuilding(const Building &building);
        void AddOffice(Office office);
        double ChangeSpeed(double speed);
        double GetMapSpeed() const;
        size_t GetBagCapacity() const { return bag_capacity_; }

    private:
        using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;
        Id id_;
        std::string name_;
        double map_spd_ = 1.;
        size_t bag_capacity_ = 3;
        Roads roads_;
        Buildings buildings_;
        OfficeIdToIndex warehouse_id_to_index_;
        Offices offices_;
    };

} // namespace model
std::ostream &operator<<(std::ostream &os, const model::Road &road);