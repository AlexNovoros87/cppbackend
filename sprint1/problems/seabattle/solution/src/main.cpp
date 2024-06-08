#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"
#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>

static const std::string DEFEAT = "D|";
static const std::string KILL = "K|";
static const std::string INJ = "I|";
static const std::string MISS = "M|";
static const std::string TURN = "T|";

static const char STOP_SYMBOL = '|';
namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

std::ostream &operator<<(std::ostream &os, SeabattleField::ShotResult res)
{
    switch (res)
    {
    case (SeabattleField::ShotResult::HIT):
        os << "INJURED" << std::endl;
        break;
    case (SeabattleField::ShotResult::KILL):
        os << "KILL" << std::endl;
        break;
    case (SeabattleField::ShotResult::MISS):
        os << "MISS" << std::endl;
        break;
    }
    return os;
}

void PrintFieldPair(const SeabattleField &left, const SeabattleField &right)
{
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i)
    {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket &socket)
{
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec)
    {
        return std::nullopt;
    }
    return {{buf.data(), sz}};
}

static bool WriteExact(tcp::socket &socket, std::string_view data)
{
    boost::system::error_code ec;
    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);
    return !ec;
}

class SeabattleAgent
{
public:
    SeabattleAgent(const SeabattleField &field)
        : my_field_(field)
    {
    }

    void StartGame(tcp::socket &socket, bool my_initiative)
    {
        std::cout << "SIDE GAME STARTED" << std::endl;
        bool my_turn = my_initiative;
        boost::system::error_code ec;
        PrintFields();
        while (!IsGameEnded())
        {

            if (my_turn)
            {
                net::streambuf stream_buf;
                std::string bullet_adress = "";

                while (true)
                {
                    std::cout << "Type your turn" << std::endl;
                    std::cin >> bullet_adress;
                    if (!IsCorrerctShot(bullet_adress))
                    {
                        std::cout << "Mistake on enter" << std::endl;
                    }
                    else break;
                }

                bullet_adress.push_back(STOP_SYMBOL);
                bool wr = WriteExact(socket, bullet_adress);
                if (!wr)
                {
                    std::cout << "Mistake on transfer" << std::endl;
                    std::abort();
                    break;
                }

                net::read_until(socket, stream_buf, STOP_SYMBOL, ec); // if enemy defeated
                std::string answer_data{std::istreambuf_iterator<char>(&stream_buf),
                                        std::istreambuf_iterator<char>()};

                if (answer_data == DEFEAT)
                {
                    std::cout << "CONGRATULATIONS!!! YOU WON!!!!" << std::endl;
                    break;
                }
                    ActionsFromEnemyField(bullet_adress, answer_data, my_turn);
                    PrintFields();
                
            }

            else
            {

                std::cout << "Await enemy" << std::endl;
                net::streambuf stream_buf;
                net::read_until(socket, stream_buf, STOP_SYMBOL, ec);
                std::string answer_data{std::istreambuf_iterator<char>(&stream_buf),
                                        std::istreambuf_iterator<char>()};

                if (answer_data == DEFEAT)
                {
                    std::cout << "CONGRATULATIONS!!! YOU WON!!!!" << std::endl;
                    break;
                }

                answer_data.pop_back();
                auto enemy_shoot_coordinates = ParseMove(answer_data);

                auto enemy_result = my_field_.Shoot(enemy_shoot_coordinates.value().second,
                                                    enemy_shoot_coordinates.value().first);

                ActionsFromMyField(answer_data, enemy_result, my_turn);

                if (my_field_.IsLoser())
                {
                    bool wr = WriteExact(socket, DEFEAT);
                    std::cout << "CONGRATULATIONS!!! YOU LOOSE!!!!";
                    break;
                };
                SendResultToAttacker(socket, enemy_result);
                if (enemy_result == SeabattleField::ShotResult::MISS)
                {
                    my_turn = true;
                }
                PrintFields();
            }
        };
    }

private:
    bool ActionsFromEnemyField(std::string bullet,
                               const std::string &answer_data,
                               bool &myturn)
    {

        bullet.pop_back();

        auto shoot_coords = ParseMove(bullet);
        int x = shoot_coords.value().second;
        int y = shoot_coords.value().first;
        if (answer_data == MISS)
        {
            other_field_.MarkMiss(x, y);
            myturn = false;
        }
        else if (answer_data == INJ)
        {
            other_field_.MarkHit(x, y);
            myturn = true;
        }
        else if (answer_data == KILL)
        {
            other_field_.MarkKill(x, y);
            myturn = true;
        }
    }

    void ActionsFromMyField(
        std::string bullet,
        SeabattleField::ShotResult res,
        bool &myturn)
    {

        auto shoot_coords = ParseMove(bullet);
        int x = shoot_coords.value().second;
        int y = shoot_coords.value().first;

        switch (res)
        {
        case (SeabattleField::ShotResult::HIT):
            my_field_.MarkHit(x, y);
            myturn = false;
            break;
        case (SeabattleField::ShotResult::KILL):
            my_field_.MarkKill(x, y);
            myturn = false;
            break;
        case (SeabattleField::ShotResult::MISS):
            my_field_.MarkMiss(x, y);
            myturn = true;
            break;
        }
    };

    bool SendResultToAttacker(tcp::socket &s, SeabattleField::ShotResult res)
    {
        bool sended;
        switch (res)
        {
        case (SeabattleField::ShotResult::HIT):
            sended = WriteExact(s, INJ);
            break;
        case (SeabattleField::ShotResult::KILL):
            sended = WriteExact(s, KILL);
            break;
        case (SeabattleField::ShotResult::MISS):
            sended = WriteExact(s, MISS);
            break;
            return sended;
        }
    }

    // преобразует текстовое представление клетки, состоящее из буквы и цифры в координаты.
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view &sv)
    {
        if (sv.size() != 2)
            return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8)
            return std::nullopt;
        if (p2 < 0 || p2 > 8)
            return std::nullopt;

        return {{p1, p2}};
    }

    // преобразует координаты в текстовое представление клетки.
    static std::string MoveToString(std::pair<int, int> move)
    {
        char buff[] = {static_cast<char>(move.first) + 'A', static_cast<char>(move.second) + '1'};
        return {buff, 2};
    }

    bool IsCorrerctShot(const std::string &shot)
    {
        if (shot.size() != 2)
            return false;
        if (shot[0] < 'A' || shot[0] > 'H')
            return false;
        if (shot[1] < '1' || shot[1] > '8')
            return false;
        return true;
    }

    // выводит в cout два поля: игрока и соперника.
    void PrintFields() const
    {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const
    {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField &field, unsigned short port)
{
    SeabattleAgent agent(field);
    net::io_context io; // контекст
    boost::system::error_code ec;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), port)); // слушатель порта
    tcp::socket socket{io};                                     // сокет

    std::cout << "Waiting for connection..."sv << std::endl;
    acceptor.accept(socket, ec);

    if (ec)
    {
        std::cout << "Can't accept connection"sv << std::endl;
        StartServer(field, port);
    }
    std::cout << "SERVER ";
    agent.StartGame(socket, false);
    // TODO: реализуйте самостоятельно
};

void StartClient(const SeabattleField &field, const std::string &ip_str, unsigned short port)
{
    SeabattleAgent agent(field);
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str), port);

    if (ec)
    {
        std::cout << "Wrong IP format"sv << std::endl;
        std::abort();
    }
    net::io_context io;
    tcp::socket socket{io};

    socket.connect(endpoint, ec);

    if (ec)
    {
        std::cout << "Can't connect to server"sv << std::endl;
        std::abort;
    }

    std::cout << "CLIENT ";
    agent.StartGame(socket, true);
};

int main(int argc, const char **argv)
{
    if (argc != 3 && argc != 4)
    {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }
    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3)
    {
        StartServer(fieldL, std::stoi(argv[2]));
    }
    else if (argc == 4)
    {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
