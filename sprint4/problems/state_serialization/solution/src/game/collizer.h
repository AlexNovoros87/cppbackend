#pragma once
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
    struct ItemMOD
    {
        geom::Point2D position;
        double width;
        // ТИП КОЛЛИЗИИ ОФИС ИЛИ ЛУТ
        ItemType type_colision;
    };

    /*Gatherer { geom::Point2D start_pos;
   geom::Point2D end_pos; double width;*/
    struct GathererDog
    {
        geom::Point2D start_pos;
        geom::Point2D end_pos;
        double width;
        // ИНИЦИАТОР СОБЫТИЯ
        std::shared_ptr<model::Dog> initiator;
    };

    /* struct GatheringEvent {size_t item_id;
    size_t gatherer_id; double sq_distance; double time;*/
    struct GatheringEventMOD
    {
        double sq_distance;
        double time;
        // ТИП КОЛЛИЗИИ ОФИС ИЛИ ЛУТ
        ItemType event_kind;
        // ИНИЦИАТОР СОБЫТИЯ
        std::shared_ptr<model::Dog> initiator;
        // МЕСТО СОБЫТИЯ
        model::DogCoordinates where;
    };

    class Collizer 
    {
    public:
        size_t ItemsCount() const ;
        size_t GatherersCount() const ;
        const ItemMOD &GetCollisionPoint(size_t idx) const;
        const GathererDog &GetInitiator(size_t idx) const;
        void AddOffice(const model::Office &office);
        void AddLootObject(const model::LootCoordinates &coord);
        void AddGatherers(std::vector<GathererDog> gatherers);

    private:
        /*
        НИЖЕ МЕТОДЫ ТОЛЬКО РАДИ РЕАЛИЗАЦИИ ИНТЕРФЕЙСА ПОЛЬЗУЮСЬ ДРУГИМИ
        Причина 1: Структуры имеют более расширенныц набор полей по сравению что дали в прекоде
        Причина 2: Методы жутко непроизводительные. Конструировать объект чтобы просто посмотреть на его
                   поля - очень расточительно!
        */
        std::vector<ItemMOD> items_;
        std::vector<GathererDog> gatherers_;
    };

    std::vector<GatheringEventMOD> FindGatherEventsOrder(const Collizer &provider);

}
