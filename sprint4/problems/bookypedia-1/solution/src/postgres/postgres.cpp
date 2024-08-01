#include "postgres.h"
#include <vector>
#include <pqxx/pqxx>
#include <iostream>

namespace postgres
{

    using namespace std::literals;
    using pqxx::operator"" _zv;

    void AuthorRepositoryImpl::Save(const domain::Author &author)
    {
        // Пока каждое обращение к репозиторию выполняется внутри отдельной транзакции
        // В будущих уроках вы узнаете про паттерн Unit of Work, при помощи которого сможете несколько
        // запросов выполнить в рамках одной транзакции.
        // Вы также может самостоятельно почитать информацию про этот паттерн и применить его здесь.
        pqxx::work work{connection_};
        work.exec_params(
            R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
            author.GetId().ToString(), author.GetName());
        work.commit();
    }

    void AuthorRepositoryImpl::ShowMyAuthors(std::ostream &os)
    {
        pqxx::read_transaction read_tr(connection_);
        pqxx::zview query_text = "SELECT id, name FROM authors ORDER BY name"_zv;
        int ida = 1;
        for (auto [id, name] : read_tr.query<std::string, std::string>(query_text))
        {
            os << ida++ << ". " << name << std::endl;
        }
    }

    std::vector<ui::detail::AuthorInfo> AuthorRepositoryImpl::GetAuthorVec()
    {
        std::vector<ui::detail::AuthorInfo> vc;

        pqxx::read_transaction read_tr(connection_);
        pqxx::zview query_text = "SELECT id, name FROM authors ORDER BY name"_zv;
        int ida = 1;
        for (auto [id, name] : read_tr.query<std::string, std::string>(query_text))
        {
            vc.push_back({std::move(id), std::move(name)});
        }
        return vc;
    }

    Database::Database(pqxx::connection connection)
        : connection_{std::move(connection)}
    {
        pqxx::work work{connection_};
        work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);

        work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL,
    title varchar(100) NOT NULL,
    publication_year integer

);
)"_zv);
        // коммитим изменения
        work.commit();
    }

} // namespace postgres

namespace postgres
{

    void BookRepositoryImpl::Save(ui::detail::AddBookParams book, std::string book_id)
    {
        pqxx::work work{connection_};
        work.exec_params("INSERT INTO books (id, author_id, title, publication_year) VALUES($1, $2, $3, $4);"_zv,
                         book_id, book.author_id, work.quote(book.title), std::to_string(book.publication_year));
        work.commit();
    };

    std::vector<ui::detail::BookInfo> BookRepositoryImpl::GetBookVec()
    {

        // nazvanie, god publicazii
        std::vector<ui::detail::BookInfo> book_ord;

        pqxx::read_transaction read_tr(connection_);

        auto query_text = "SELECT title, publication_year FROM books ORDER BY title;";

        for (auto [title, year] : read_tr.query<std::string, int>(query_text))
        {
            // title = title.substr(1);
            // title.pop_back();
            book_ord.push_back({title, year});
        }
        
       
        return book_ord;
    };
    std::vector<ui::detail::BookInfo> BookRepositoryImpl::GetBookVecByAuthor(std::string author)
    {

        // nazvanie, god publicazii
        std::vector<ui::detail::BookInfo> book_ord;
        pqxx::read_transaction read_tr(connection_);

        auto query_text = "SELECT title, publication_year FROM books WHERE author_id=" + read_tr.quote(author) + ';';

        for (auto [title, year] : read_tr.query<std::string, int>(query_text))
        {
            // title = title.substr(1);
            // title.pop_back();
            book_ord.push_back({title, year});
        }
        std::sort(book_ord.begin(), book_ord.end(), [](const ui::detail::BookInfo &lhs, const ui::detail::BookInfo &rhs)
                  {
                      if (lhs.publication_year == rhs.publication_year)
                      {
                          return lhs.title < rhs.title;
                      }
                      return lhs.publication_year < rhs.publication_year; });

        return book_ord;
    };

}