#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "tagged.h"
#include "../req_helper/declaration_structs.h"

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
        const std::string &GetJsonType() const;
        bool IsHorizontal() const noexcept;
        bool IsVertical() const noexcept;
        Point GetStart() const noexcept;
        Point GetEnd() const noexcept;

    private:
        Point start_;
        Point end_;
        std::string json_;
    };

    class Building
    {
    public:
        explicit Building(Rectangle bounds) noexcept;
        const Rectangle &GetBounds() const noexcept;
        const std::string &GetJsonType() const;

    private:
        Rectangle bounds_;
        std::string json_;
    };

    class Office
    {
    public:
        using Id = util::Tagged<std::string, Office>;

        Office(Id id, Point position, Offset offset) noexcept;
        const Id &GetId() const noexcept;
        Point GetPosition() const noexcept;
        Offset GetOffset() const noexcept;
        const std::string &GetJsonType() const;

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

        Map(Id id, std::string name, double speed) noexcept;
        const Id &GetId() const noexcept;
        const std::string &GetName() const noexcept;
        const Buildings &GetBuildings() const noexcept;
        const Roads &GetRoads() const noexcept;
        const Offices &GetOffices() const noexcept;
        void AddRoad(const Road &road);
        void AddBuilding(const Building &building);
        void AddOffice(Office office);
        double ChangeSpeed(double speed){
            map_spd_ = speed;
            return map_spd_;
        }
        
        double GetMapSpeed() const{
            return map_spd_;
        }
    
    
    
    private:
        using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

        Id id_;
        std::string name_;
        double map_spd_;

        
        Roads roads_;
        Buildings buildings_;
        OfficeIdToIndex warehouse_id_to_index_;
        Offices offices_;
    };


} // namespace model
std::ostream& operator<<(std::ostream& os,const model::Road& road);