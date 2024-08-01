#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/author.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Author& author) override;
    void ShowMyAuthors(std::ostream& os) override; 
    std::vector<ui::detail::AuthorInfo> GetAuthorVec() override;

private:
    pqxx::connection& connection_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

     void Save(ui::detail::AddBookParams book, std::string book_id) override;
     std::vector<ui::detail::BookInfo> GetBookVec() override;
     std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author) override;

private:
    pqxx::connection& connection_;
};


class Database {
public:
    explicit Database(pqxx::connection connection);

    AuthorRepositoryImpl& GetAuthors() & {
        return authors_;
    }

    BookRepositoryImpl& GetBooks()& {
        return books_;
    }

private:
    pqxx::connection connection_;
    AuthorRepositoryImpl authors_{connection_};
    BookRepositoryImpl books_{connection_};
};

}  // namespace postgres