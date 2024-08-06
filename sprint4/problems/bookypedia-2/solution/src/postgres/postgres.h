#pragma once

#include "../domain/author.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:


    std::string Save(const domain::Author& author,pqxx::work& work) override;
    std::vector<ui::detail::AuthorInfo> GetAuthorVec(pqxx::work& work) override;
    bool HavingAuthor(const std::string& name, pqxx::work& work);


};

class BookRepositoryImpl : public domain::BookRepository {
public:
  
     void Save(ui::detail::AddBookParams book, std::string book_id, pqxx::work& work) override;
     std::vector<ui::detail::BookInfo> GetBookVec(pqxx::work& work) override;
     std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author, pqxx::work& work) override;
     std::vector<ui::detail::BookInfo> GetBooksByTitle(std::string title,pqxx::work& work) override;


};

}  // namespace postgres