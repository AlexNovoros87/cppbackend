#pragma once
#include "hamlog.h" 
#include "tchecker.h"




class Order : public std::enable_shared_from_this<Order> {
private:
    net::io_context& io_;
    int id_;
    bool with_onion_;
    OrderHandler handler_;
    Logger logger_{std::to_string(id_)};

    Timer roast_timer_{io_, 1ms};
    Timer marinade_timer_{io_, 1ms};

    Hamburger hamburger_;
    bool onion_marinaded_ = false;

    bool delivered_ = false; // Заказ доставлен?

/*
Чтобы обнаружить состояние гонки в классе Order, объявите в нём 
поле std::atomic_int counter_, изначально равное нулю. 

В методах OnRoasted и OnOnionMarinaded, которые потенциально 
могут быть вызваны в разных потоках, объявите переменную 
ThreadChecker, передав в конструктор на поле counter_. 
assert в деструкторе ThreadChecker выстрелит, если OnRoasted и 
OnOnionMarinaded будут вызваны в пересекающиеся моменты времени 
у одного и того же заказа. 

Чтобы повысить вероятность этого события, временно установите 
таймеры прожаривания и маринования на одну миллисекунду.
*/

std::atomic_int counter_{0};
net::strand<net::io_context::executor_type> strand_{net::make_strand(io_)};


/*

*/

public:
    Order(net::io_context& io, int id, bool with_onion, OrderHandler handler)
        : io_{io}
        , id_{id}
        , with_onion_{with_onion}
        , handler_{std::move(handler)} {
    }

    // Запускает асинхронное выполнение заказа
    void Execute() {
        logger_.LogMessage("Order has been started."sv);
        RoastCutlet();
        if (with_onion_) {
            MarinadeOnion(); 
        }
    }


private:
    /*
    В MarinadeOnion и RoastCutlet воспользуйтесь функцией boost:asio::bind_executor, 
    чтобы создать «обёртку» для функции-обработчика события таймера. 
    Эта функция-обёртка выполнит вложенную в неё функцию, воспользовавшись 
    последовательным исполнителем strand_. В результате методы OnRoasted и 
    OnOnionMarinaded у одного и того же заказа будут выполняться строго 
    последовательно, даже если вызваны в разных потоках.
   */
    
    
    void MarinadeOnion() {
        logger_.LogMessage("Start marinading onion"sv);
        marinade_timer_.async_wait(
            // OnOnionMarinaded будет вызван последовательным исполнителем strand_
            net::bind_executor(strand_, [self = shared_from_this()](sys::error_code ec) {
                self->OnOnionMarinaded(ec);
            }));
    }

    void RoastCutlet() {
        logger_.LogMessage("Start roasting cutlet"sv);
        roast_timer_.async_wait(
            // OnRoasted будет вызван последовательным исполнителем strand_
            net::bind_executor(strand_, [self = shared_from_this()](sys::error_code ec) {
                self->OnRoasted(ec);
            }));
    }

    void OnOnionMarinaded(sys::error_code ec) {
         ThreadChecker th(counter_);
        if (ec) {
            logger_.LogMessage("Marinade onion error: "s + ec.message());
        } else {
            logger_.LogMessage("Onion has been marinaded."sv);
            onion_marinaded_ = true;
        }
        CheckReadiness(ec);
    }
    
    
    void OnRoasted(sys::error_code ec) {
      ThreadChecker th(counter_);
       if (ec) {
            logger_.LogMessage("Roast error : "s + ec.message());
        } else {
            logger_.LogMessage("Cutlet has been roasted."sv);
            hamburger_.SetCutletRoasted();
        }
        CheckReadiness(ec);
    }

       /*
       По окончании обеих асинхронных операций нужно проверить 
       готовность заказа. Для этого вызовите метод CheckReadiness.
       */
         //////////////////////////////////////////////////////////////////
        void CheckReadiness(sys::error_code ec) {
        if (delivered_) {
            // Выходим, если заказ уже доставлен либо клиента уведомили об ошибке
            return;
        }
        if (ec) {
            // В случае ошибки уведомляем клиента о невозможности выполнить заказ
            return Deliver(ec);
        }

        // Самое время добавить лук
        if (CanAddOnion()) {
            logger_.LogMessage("Add onion"sv);
            hamburger_.AddOnion();
        }

        // Если все компоненты гамбургера готовы, упаковываем его
        if (IsReadyToPack()) {
            Pack();
        }
    }

    void Deliver(sys::error_code ec) {
        // Защита заказа от повторной доставки
        delivered_ = true;
        // Доставляем гамбургер в случае успеха либо nullptr, если возникла ошибка
        handler_(ec, id_, ec ? nullptr : &hamburger_);
    }

    [[nodiscard]] bool CanAddOnion() const {
        // Лук можно добавить, если котлета обжарена, лук замаринован, но пока не добавлен
        return hamburger_.IsCutletRoasted() && onion_marinaded_ && !hamburger_.HasOnion();
    }

    [[nodiscard]] bool IsReadyToPack() const {
        // Если котлета обжарена и лук добавлен, как просили, гамбургер можно упаковывать
        return hamburger_.IsCutletRoasted() && (!with_onion_ || hamburger_.HasOnion());
    }

    void Pack() {
        logger_.LogMessage("Packing"sv);

        // Просто потребляем ресурсы процессора в течение 0,5 с.
        auto start = steady_clock::now();
        while (steady_clock::now() - start < 500ms) {
        }

        hamburger_.Pack();
        logger_.LogMessage("Packed"sv);

        Deliver({});
    }


}; 