#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include "order.h"
// Класс "Кафетерий". Готовит хот-доги
class Cafeteria {
public:
    explicit Cafeteria(net::io_context& io)
        : io_{io} {
    }

    
    // Асинхронно готовит хот-дог и вызывает handler, как только хот-дог будет готов.
    // Этот метод может быть вызван из произвольного потока
    void OrderHotDog(HotDogHandler handler) {
       mtx_.lock();
       OrderPack pack;
       pack.handler = std::move(handler);
       pack.gas_cooker = gas_cooker_;
       pack.store = &store_;
       pack.io = &io_;
       std::make_shared<Order>(++prepare_id_, std::move(pack))->MakeOrder();
       mtx_.unlock();
    }

private:
    net::io_context& io_;
    // Используется для создания ингредиентов хот-дога
    Store store_;
    // Газовая плита. По условию задачи в кафетерии есть только одна газовая плита на 8 горелок
    // Используйте её для приготовления ингредиентов хот-дога.
    // Плита создаётся с помощью make_shared, так как GasCooker унаследован от
    // enable_shared_from_this.
    std::shared_ptr<GasCooker> gas_cooker_ = std::make_shared<GasCooker>(io_);
    std::atomic_int prepare_id_{0};
    std::mutex mtx_;
    
};
