#include "collizer.h"
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
                evt.item_id = i;
                evt.gatherer_id = g;
                evt.sq_distance = collect_result.sq_distance;
                evt.time = collect_result.proj_ratio;
                evt.event_kind = item.type_colision;
                evt.initiator = gatherer.initiator;     
                evt.where = {item.position.x, item.position.y};
                detected_events.push_back(evt);
            }
        }

    }

    std::sort(detected_events.begin(), detected_events.end(),
              [](const GatheringEvent& e_l, const GatheringEvent& e_r) {
                  return e_l.time < e_r.time;
              });

    return detected_events;
}
}