#pragma once
#include "includes.h"

class Hamburger {
public:
    [[nodiscard]] bool IsCutletRoasted() const {
        return cutlet_roasted_;
    }
    void SetCutletRoasted() {
        if (IsCutletRoasted()) {  // Котлету можно жарить только один раз
            throw std::logic_error("Cutlet has been roasted already"s);
        }
        cutlet_roasted_ = true;
    }

    [[nodiscard]] bool HasOnion() const {
        return has_onion_;
    }
    // Добавляем лук
    void AddOnion() {
        if (IsPacked()) {  // Если гамбургер упакован, класть лук в него нельзя
            throw std::logic_error("Hamburger has been packed already"s);
        }
        AssureCutletRoasted();  // Лук разрешается класть лишь после прожаривания котлеты
        has_onion_ = true;
    }

    [[nodiscard]] bool IsPacked() const {
        return is_packed_;
    }
    void Pack() {
        AssureCutletRoasted();  // Нельзя упаковывать гамбургер, если котлета не прожарена
        is_packed_ = true;
    }

private:
    // Убеждаемся, что котлета прожарена
    void AssureCutletRoasted() const {
        if (!cutlet_roasted_) {
            throw std::logic_error("Cutlet has not been roasted yet"s);
        }
    }

    bool cutlet_roasted_ = false;  // Обжарена ли котлета?
    bool has_onion_ = false;       // Есть ли лук?
    bool is_packed_ = false;       // Упакован ли гамбургер?
};

std::ostream& operator<<(std::ostream& os, const Hamburger& h);

/*
Программа будет многопоточной. Чтобы одновременный вывод в консоль из разных потоков 
не превращался в нечитаемое месиво, воспользуемся классом std::osyncstream из C++20, 
который обеспечит синхронизацию вывода. На его основе создадим класс Logger, 
делающий вывод более наглядным:
*/

class Logger {
public:
    explicit Logger(std::string id)
        : id_(std::move(id)) {
    }

    void LogMessage(std::string_view message) const {
        std::osyncstream os{std::cout};
        os << id_ << "> ["sv << duration<double>(steady_clock::now() - start_time_).count()
           << "s] "sv << message << std::endl;
    }

private:
    std::string id_;
    steady_clock::time_point start_time_{steady_clock::now()};
}; 


/*
Объявим класс Restaurant. Чтобы ресторан и его компоненты могли выполнять асинхронные 
операции, передадим в конструктор Restaurant ссылку на io_context. Для приготовления 
гамбургеров служит метод Restaurant::MakeHamburger. 
Приготовление гамбургера — асинхронная операция. Поэтому метод принимает 
функцию OrderHandler, которую вызовут, когда гамбургер будет готов, 
либо произойдёт ошибка.
*/

// Функция, которая будет вызвана по окончании обработки заказа
using OrderHandler = std::function<void(sys::error_code ec, int id, Hamburger* hamburger)>;


/*
Для хранения информации о заказе и управления его исполнением создайте класс Order. 
С каждым заказом связан его id, вид заказа — с луком или без, ссылка на io_context 
и функция-обработчик события о готовности заказа. Внутри каждого заказа будет 
находиться объект класса Logger, проинициализированный строкой-идентификатором 
заказа. 
Благодаря этому легко будет отличить вывод сведений о разных заказах.

Класс Order унаследуйте от шаблонного класса std::enable_shared_from_this. 
В качестве параметра шаблона передайте класс Order. Шаблон std::enable_shared_from_this 
позволяет внутри методов класса-наследника безопасно получать shared_ptr, 
ссылающийся на текущий объект. 
Экземпляры класса Order будут использовать это, чтобы продлить время жизни, 
пока выполняют асинхронные операции.
*/