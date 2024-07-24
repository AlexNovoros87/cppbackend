#include "../colis_detect/collision_detector.h"
#include "../req_helper/declaration_structs.h"
#include "../model/game_dog_session.h"
#include <memory>

namespace collision_detector
{

    struct PlayConditions
    {
        static constexpr double tools_weigth = 0.;
        static constexpr double player_weigth = 0.6;
        static constexpr double office_weigth = 0.5;
    private:
        PlayConditions() = delete;
    };

    enum class ItemType
    {
        OFFICE,
        LOST_THING
    };

    /*Item geom::Point2D position;double width;*/
    struct ItemMOD : public Item
    {
        // ТИП КОЛЛИЗИИ ОФИС ИЛИ ЛУТ
        ItemType type_colision;
    };

    /*Gatherer { geom::Point2D start_pos;
   geom::Point2D end_pos; double width;*/
    struct GathererDog : public Gatherer
    {
        // ИНИЦИАТОР СОБЫТИЯ
        std::shared_ptr<model::Dog> initiator;
    };

    /* struct GatheringEvent {size_t item_id;
    size_t gatherer_id; double sq_distance; double time;*/
    struct GatheringEventMOD : public GatheringEvent
    {
        // ТИП КОЛЛИЗИИ ОФИС ИЛИ ЛУТ
        ItemType event_kind;
        // ИНИЦИАТОР СОБЫТИЯ
        std::shared_ptr<model::Dog> initiator;
        // МЕСТО СОБЫТИЯ
        model::DogCoordinates where;
    };

    class Collizer : public ItemGathererProvider
    {
    public:
        size_t ItemsCount() const
        {
            return items_.size();
        };

        const ItemMOD &GetCollisionPoint(size_t idx) const
        {
            //ПРОВЕРОК НЕ ДЕЛАЮ ВО ИМЯ СКОРОСТИ
            return items_[idx];
        };

        const GathererDog &GetInitiator(size_t idx) const
        {
            //ПРОВЕРОК НЕ ДЕЛАЮ ВО ИМЯ СКОРОСТИ
            return gatherers_[idx];
        };

        void AddOffice(const model::Office &office)
        {
            ItemMOD itm;
            itm.position = {static_cast<double>(office.GetPosition().x),
                            static_cast<double>(office.GetPosition().y)};
            itm.type_colision = ItemType::OFFICE;

            itm.width = PlayConditions::office_weigth;
            items_.push_back(std::move(itm));
        };

        void AddLootObject(const model::LootCoordinates &coord)
        {
            ItemMOD itm;
            itm.position = {coord.x, coord.y};
            itm.type_colision = ItemType::LOST_THING;
            itm.width = PlayConditions::tools_weigth;
            items_.push_back(std::move(itm));
        }

        size_t GatherersCount() const override
        {
            return gatherers_.size();
        };

        void AddGatherers(std::vector<GathererDog> gatherers)
        {
            gatherers_ = std::move(gatherers);
        };

    private:
        Item GetItem(size_t idx) const override
        {
            return {};
        };

        Gatherer GetGatherer(size_t idx) const override
        {
            return {};
        };

        std::vector<ItemMOD> items_;
        std::vector<GathererDog> gatherers_;
    };

    std::vector<GatheringEventMOD> FindGatherEventsOrder(const Collizer &provider);

}
