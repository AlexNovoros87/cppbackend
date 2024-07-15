
#include "order.h"

class Restaurant {
public:
    explicit Restaurant(net::io_context& io)
        : io_(io) {
    }

    /*
    Реализуйте метод Restaurant::MakeHamburger. Сначала сгенерируйте id заказа, 
    а потом создайте Order функцией std::make_shared. У созданного объекта заказа 
    вызовите метод Execute и верните id заказа клиенту.
    */
    int MakeHamburger(bool with_onion, OrderHandler handler) {
        const int order_id = ++next_order_id_;
        std::make_shared<Order>(io_, order_id, with_onion, std::move(handler))->Execute();
        return order_id;
    }

private:
    net::io_context& io_;
    int next_order_id_ = 0;
};


