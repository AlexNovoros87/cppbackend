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
        size_t ItemsCount() const;
        const ItemMOD &GetCollisionPoint(size_t idx) const;
        const GathererDog &GetInitiator(size_t idx) const;
        void AddOffice(const model::Office &office);
        void AddLootObject(const model::LootCoordinates &coord);
        size_t GatherersCount() const override;
        void AddGatherers(std::vector<GathererDog> gatherers);

    private:
        /*
        НИЖЕ МЕТОДЫ ТОЛЬКО РАДИ РЕАЛИЗАЦИИ ИНТЕРФЕЙСА ПОЛЬЗУЮСЬ ДРУГИМИ
        Причина 1: Структуры имеют более расширенныц набор полей по сравению что дали в прекоде
        Причина 2: Методы жутко непроизводительные. Конструировать объект чтобы просто посмотреть на его
                   поля - очень расточительно!    
        */
        Item GetItem(size_t idx) const override;
        Gatherer GetGatherer(size_t idx) const override;

        std::vector<ItemMOD> items_;
        std::vector<GathererDog> gatherers_;
    };

    std::vector<GatheringEventMOD> FindGatherEventsOrder(const Collizer &provider);

}
