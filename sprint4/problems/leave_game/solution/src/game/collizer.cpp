#include "collizer.h"

namespace collision_detector
{

size_t Collizer::ItemsCount() const
        {
            return items_.size();
        };

        const ItemMOD& Collizer::GetCollisionPoint(size_t idx) const
        {
#ifndef MAX_SPEED_RESPONSE
            assert(!items_.empty());
            assert(idx <= items_.size() - 1);
#endif

            return items_[idx];
        };

        const GathererDog& Collizer::GetInitiator(size_t idx) const
        {
#ifndef MAX_SPEED_RESPONSE
            assert(!gatherers_.empty());
            assert(idx <= gatherers_.size() - 1);
#endif

            return gatherers_[idx];
        };

        void Collizer::AddOffice(const model::Office &office)
        {
            ItemMOD itm;
            itm.position = {static_cast<double>(office.GetPosition().x),
                            static_cast<double>(office.GetPosition().y)};
            itm.type_colision = ItemType::OFFICE;

            itm.width = PlayConditions::office_weigth;
            items_.push_back(std::move(itm));
        };

        void Collizer::AddLootObject(const model::LootCoordinates &coord)
        {
            ItemMOD itm;
            itm.position = {coord.x, coord.y};
            itm.type_colision = ItemType::LOST_THING;
            itm.width = PlayConditions::tools_weigth;
            items_.push_back(std::move(itm));
        }

        size_t Collizer::GatherersCount() const
        {
            return gatherers_.size();
        };

        void Collizer::AddGatherers(std::vector<GathererDog> gatherers)
        {
            gatherers_ = std::move(gatherers);
        };
}

namespace collision_detector
{
std::vector<GatheringEventMOD> FindGatherEventsOrder(const Collizer& provider){
    std::vector<GatheringEventMOD> detected_events;

    static auto eq_pt = [](geom::Point2D p1, geom::Point2D p2) {
        return p1.x == p2.x && p1.y == p2.y;
    };

    for (size_t g = 0; g < provider.GatherersCount(); ++g) {
        
        const GathererDog& gatherer = provider.GetInitiator(g);
          if (eq_pt(gatherer.start_pos, gatherer.end_pos)) {
            continue;
          }
    
    
        for (size_t i = 0; i < provider.ItemsCount(); ++i) {
            const ItemMOD& item = provider.GetCollisionPoint(i);
            auto collect_result = TryCollectPoint(gatherer.start_pos, gatherer.end_pos, item.position);

            if (collect_result.IsCollected(gatherer.width + item.width)) {
                GatheringEventMOD evt;
                evt.sq_distance = collect_result.sq_distance;
                evt.time = collect_result.proj_ratio;
                evt.event_kind = item.type_colision;
                evt.initiator = gatherer.initiator;     
                evt.where = {item.position.x, item.position.y};
                detected_events.push_back(std::move(evt));
            }
        }

    }

    std::sort(detected_events.begin(), detected_events.end(),
              [](const GatheringEventMOD& e_l, const GatheringEventMOD& e_r) {
                  return e_l.time < e_r.time;
              });

    return detected_events;
}
}