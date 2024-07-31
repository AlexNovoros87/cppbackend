#include <iostream>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <pqxx/pqxx>
#include <map>
#include <stdexcept>
#include <optional>

struct SQL_BOOK
{
    int id;
    std::string title;
    std::string author;
    int year;
    std::optional<std::string> ISBN;
};

enum class TYPES
{
    add_book,
    all_books,
    exit
};

const std::map<std::string, TYPES> actions_way{
    {"add_book", TYPES::add_book},
    {"all_books", TYPES::all_books},
    {"exit", TYPES::exit}};

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

void MakeBookTable(pqxx::connection &connect)
{
    // Транзакция нужна, чтобы выполнять запросы.
    pqxx::work w(connect);

    // Используя транзакцию создадим таблицу в выбранной базе данных:
    w.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "id SERIAL PRIMARY KEY,"
        " title varchar(100) NOT NULL,"
        " author varchar(100) NOT NULL,"
        " year integer NOT NULL,"
        " ISBN char(13) UNIQUE "
        ");"_zv);

    // Применяем все изменения
    w.commit();
}

std::string AddBookRequest(pqxx::connection &connect, const boost::json::object &val)
{
    boost::json::object result{{"result", false}};
    try
    {
        std::string title = std::string(val.at("title").as_string());
        std::string author = std::string(val.at("author").as_string());
        int64_t year = val.at("year").as_int64();
        std::optional<std::string> ISBN;
        std::string zp = ", ";
        if (val.count("ISBN") > 0)
        {
            if (val.at("ISBN").is_string())
            {

                ISBN = std::string(val.at("ISBN").as_string());
            }
            else
            {
                ISBN = "NULL";
            }
        }
        else
        {
            ISBN = "NULL";
        };

        // Транзакция нужна, чтобы выполнять запросы.
        pqxx::work w(connect);

        auto q = w.exec(
            "INSERT INTO books (title, author, year, ISBN) VALUES (" + w.quote(title) + zp + w.quote(author) + zp + std::to_string(year) + zp + ISBN.value_or("NULL") + ");"

        );
        // Применяем все изменения
        w.commit();
        result.at("result") = true;
    }
    catch (const std::exception &ex)
    {
        return boost::json::serialize(result);
    };
    return boost::json::serialize(result);
}

std::string AllBooks(pqxx::connection &connect)
{
    pqxx::read_transaction r(connect);
    /*
    Транзакция имеет несколько методов для получения данных из базы:
    query1 — выполнить запрос, возвращающий одну строку; результат можно получить в виде std::tuple.
    query01 — выполнить запрос, возвращающий ноль или одну строку; результат можно получить в виде std::optional<std::tuple>.
    query — выполнить запрос, возвращающий произвольно количество строк; результат можно обойти циклом for.
    query_value — выполнить запрос, возвращающий одну строку из одного значения.
    Если запрос возвратил больше значений, чем ожидалось, будет выброшено исключение.
    */

            boost::json::array arr;

            // Перечислим фильмы до 2000 года.
            {
                auto query_text = "SELECT * FROM books ORDER BY year DESC , title , author, ISBN ;"_zv;

                // Выполняем запрос и итерируемся по строкам ответа
                // query — выполнить запрос, возвращающий произвольно количество строк; результат можно обойти циклом for.
                for (auto [id, title, author, year, ISBN] :
                     r.query<int, std::string,std::string, int, std::optional<std::string>>(query_text))
                {
                    boost::json::object obj;
                    obj["id"] = id;
                    obj["title"] = std::move(title);
                    obj["author"] = std::move(author);
                    obj["year"] = year;
                    obj["ISBN"] = ISBN ? boost::json::value(std::move(*ISBN)) : boost::json::value();
                
                   arr.push_back(std::move(obj));
                }
            }
    return boost::json::serialize(arr);
};

int main(int argc, const char *argv[])
{
    try
    {

        if (argc == 1)
        {
            std::cout << "Usage: db_example <conn-string>\n"sv;
            return EXIT_SUCCESS;
        }
        else if (argc != 2)
        {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }

        // Подключаемся к БД, указывая её параметры в качестве аргумента
        pqxx::connection conn{argv[1]};
        // Делаем таблицу
        MakeBookTable(conn);

        std::string sql_request;
        while (true)
        {
            std::getline(std::cin, sql_request);
            boost::json::value parsed = boost::json::parse(sql_request);
            TYPES type = actions_way.at(std::string(parsed.at("action").as_string()));
            
            if(type == TYPES::exit) break;
            switch (type)
            {
            case TYPES::add_book:
                std::cout << AddBookRequest(conn, parsed.at("payload").as_object()) << std::endl;
                break;

            case TYPES::all_books:
                std::cout << AllBooks(conn) << std::endl;
                break;
            }
        }
    }
    catch (std::exception& ex)
    {
      std::cerr<<ex.what()<<std::endl;
      return EXIT_FAILURE;
    
    }
   return 0;

};