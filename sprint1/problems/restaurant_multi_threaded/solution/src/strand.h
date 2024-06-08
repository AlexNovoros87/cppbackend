#include"includes.h"
/*
Для запуска потока используется класс std::jthread. Он появился в C++20 
и похож на класс  std::thread . Разница в том, что jthread в своём 
деструкторе автоматически выполняет операцию  join, которая дожидается 
окончания работы потока, а стандартный поток  требует вызвать join либо 
detach явно до вызова деструктора. Ещё у jthread есть методы для 
остановки работы потока, но в этом уроке они нам не понадобятся.

Чтобы jthread не разрушился сразу после своего создания и не 
заблокировал текущий поток, созданные потоки помещаются в std::vector. 
Перед выходом из RunWorkers деструктор вектора вызовет деструкторы 
потоков, которые находятся в векторе. И текущий поток дождётся 
окончания их работы.
*/

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
} 


/*
Чтобы сделать выполнение функций строго последовательным, в Boost.Asio есть 
последовательный исполнитель под названием Strand  (англ. «нитка бус»). 
Простой способ создать его — функция boost::asio::make_strand. 
Передав ей ссылку на io_context, вы получите strand, который будет 
последовательно выполнять функции io_context.

Чтобы в многопоточном приложении обработчики асинхронных операций 
выполнялись строго последовательно, нужно привязать их выполнение 
к одному strand. Для обработчиков, привязанным к разным экземплярам strand,
последовательное выполнение друг относительно друга не гарантируется.

Чтобы обработчики таймеров выполнялись внутри strand, используем функцию 
boost:asio::bind_executor. Она создаст функцию-обёртку, которая выполнит 
обработчик в указанном исполнителе. В нашем случае, в strand:

*/

void ExampleWithStrand () {
    const unsigned num_threads = 2;
    net::io_context io{num_threads};

    net::steady_timer t1{io, 400ms};
    net::steady_timer t2{io, 600ms};
    net::steady_timer t3{io, 800ms};
    net::steady_timer t4{io, 1000ms};

    // Эта лямбда-функция вернёт обработчик таймера, который выведет текст и заблокирует
    // текущий поток на 1 секунду
    auto make_timer_handler = [](int index) {
        return [index](sys::error_code) {
            std::osyncstream{std::cout} << "Enter #"sv << index << std::endl;
            // Блокируем текущий поток на 1 секунду, чтобы обработчики,
            // выполняющиеся в разных потоках, пересекались во времени
            std::this_thread::sleep_for(1s);
            std::osyncstream{std::cout} << " Exit #"sv << index << std::endl;
        };
    };

    // Привяжем к таймерам обработчики, которые выполнятся
    // через 400, 600, 800 и 1000 миллисекунд соответственно
    auto strand1 = net::make_strand(io);
    auto strand2 = net::make_strand(io);

    // обработчики таймеров t1 и t2 будут выполнены строго последовательно
    t1.async_wait(net::bind_executor(strand1, make_timer_handler(1)));
    t2.async_wait(net::bind_executor(strand1, make_timer_handler(2)));

    // обработчики таймеров t3 и t4 будут выполнены строго последовательно
    t3.async_wait(net::bind_executor(strand2, make_timer_handler(3)));
    t4.async_wait(net::bind_executor(strand2, make_timer_handler(4)));

    RunWorkers(num_threads, [&io] {
        io.run();
    });
}


/*
Последовательное выполнение не гарантирует, что обработчики одного strand 
всегда будут вызваны в одном потоке. Гарантируется лишь то, что очередной 
обработчик внутри strand не начнёт выполняться, пока предыдущий не 
завершит свою работу. Например, при трёх рабочих потоках обработчик t2 мог 
начать выполняться в потоке №3, но не раньше, чем завершится обработчик t1. 


Говорят, что две операции упорядочены, если одна из них гарантированно 
видит все записи в память, произведённые другой. Отсутствие упорядоченности 
может приводить к гонке. Упорядоченность всегда определяется явно, согласно 
правилам. Например, операции, которые выполняются одним потоком, 
упорядочены. Упорядоченности можно достичь и через использование 
мьютекса или атомарной переменной. Но если никаких примитивов синхронизации 
не используется, то в многопоточном приложении гарантировать упорядоченность 
нельзя. Даже если вы уверены, что одна операция выполнится хронологически 
позже другой, вовсе не факт, что они будут упорядочены. 
Это связано с особенностями архитектуры многоядерных и многопроцессорных 
систем. Strand гарантирует упорядоченность операций — это более сильное 
требование, чем просто последовательное выполнение.
*/