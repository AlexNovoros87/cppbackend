#define _USE_MATH_DEFINES
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/collision_detector.h"
#include <catch2/catch_session.hpp>

// Напишите здесь тесты для функции collision_detector::FindGatherEvents


namespace collision_detector
{
   class ItemGathererProviderSon: public ItemGathererProvider {
public:
    virtual ~ItemGathererProviderSon() = default;
    
    size_t ItemsCount() const {
        return items_.size();
    };

    Item GetItem(size_t idx) const override{
        return items_[idx];
    };

    void AddItem(Item item) {
        items_.push_back(std::move(item));
    };

    size_t GatherersCount() const override{
        return gatherers_.size();
    };

    Gatherer GetGatherer(size_t idx) const override{
        return gatherers_[idx];
    };

    void AddGatherer(Gatherer gatherer) {
        gatherers_.push_back(std::move(gatherer));
    };
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

};
 // namespace collision_detector::

using namespace std::literals;

/*
Класс WithinRelMatcher используется в модульных тестах для сравнения значений 
с плавающей запятой с заданной относительной погрешностью. Это особенно полезно 
при работе с арифметикой с плавающей запятой, где точные сравнения могут не 
сработать из-за ошибок округления.

Пример использования
Чтобы проверить, находится ли значение x в пределах 1% от ожидаемого значения 
42.0, можно использовать следующий код:
cpp
CHECK(x == Catch::Matchers::WithinRel(42.0, 0.01));

Этот тест пройдет, если x находится в диапазоне от 41.58 до 42.42 (42.0 ± 1%).
Таким образом, WithinRel позволяет удобно и эффективно проверять соответствие 
значений с учетом возможных ошибок округления.
*/


namespace Catch {
template<>
struct StringMaker<collision_detector::GatheringEvent> {
  static std::string convert(collision_detector::GatheringEvent const& value) {
      std::ostringstream tmp;
      tmp << "(" << value.gatherer_id << "," << value.item_id << "," << value.sq_distance << "," << value.time << ")";

      return tmp.str();
  }
};
}  // namespace Catch 

constexpr double EPSILON = 1e-9;

TEST_CASE("1") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{12.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item);
    tst.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), EPSILON)); 
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
   
}


TEST_CASE("2") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{42.5, 0}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item1);
    tst.AddItem(item2);
    tst.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(tst);

    CHECK(events.size() == 1);

    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer.end_pos.x), EPSILON)); 
}

TEST_CASE("3") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{12.5, 0.5}, 0.0};
    collision_detector::Gatherer gatherer{{0, 0.1}, {22.5, 0.1}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item);
    tst.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.16, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), EPSILON)); 
}


TEST_CASE("4") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{12.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {12.5, 0}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item);
    tst.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), EPSILON)); 
}



TEST_CASE("5") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{0, 12.5}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {0, 22.5}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item);
    tst.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item.position.y/gatherer.end_pos.y), EPSILON)); 
}

TEST_CASE("6") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{12.5, 0}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer1{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::Gatherer gatherer2{{0, 0}, {10, 0}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item1);
    tst.AddItem(item2);
    tst.AddGatherer(gatherer1);
    tst.AddGatherer(gatherer2);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK(events.size() == 3);

    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer1.end_pos.x), EPSILON)); 

    CHECK(events[1].item_id == 0);
    CHECK(events[1].gatherer_id == 0);
    CHECK_THAT(events[1].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[1].time, WithinRel((item1.position.x/gatherer1.end_pos.x), EPSILON)); 

    CHECK(events[2].item_id == 1);
    CHECK(events[2].gatherer_id == 1);
    CHECK_THAT(events[2].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[2].time, WithinRel((item2.position.x/gatherer2.end_pos.x), EPSILON)); 
}


TEST_CASE("7") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{12.5, 0}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item1);
    tst.AddItem(item2);
    tst.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK(events.size() == 2);
   
    CHECK(events[1].item_id == 0);
    CHECK(events[1].gatherer_id == 0);
    CHECK_THAT(events[1].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[1].time, WithinRel((item1.position.x/gatherer.end_pos.x), EPSILON)); 
    
    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer.end_pos.x), EPSILON)); 

}



TEST_CASE("8") {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{0, 12.5}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer1{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::Gatherer gatherer2{{0, 0}, {0, 22.5}, 0.6};
    collision_detector::ItemGathererProviderSon tst;
    tst.AddItem(item1);
    tst.AddItem(item2);
    tst.AddGatherer(gatherer1);
    tst.AddGatherer(gatherer2);
    auto events = collision_detector::FindGatherEvents(tst);
    
    CHECK(events.size() == 2);
    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer1.end_pos.x), EPSILON)); 
    CHECK_THAT(events[1].sq_distance, WithinRel(0.0, EPSILON));
    CHECK_THAT(events[1].time, WithinRel((item1.position.y/gatherer2.end_pos.y), EPSILON)); 
    CHECK(events[1].item_id == 0);
    CHECK(events[1].gatherer_id == 1);
 
}


int main(int argc, char* argv[]) {
    
    int result = Catch::Session().run(argc, argv);
    return result;
} 
