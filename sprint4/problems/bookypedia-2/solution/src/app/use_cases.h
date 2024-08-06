#pragma once

#include <string>
#include <vector>
#include "../defstr.h"
#include "../postgres/postgres.h"

namespace app
{

    class UseCases
    {
    public:
        virtual std::string AddAuthor(const std::string &name, pqxx::work& work) = 0;
        virtual std::vector<ui::detail::AuthorInfo> GetAuthorVec(pqxx::work& work) = 0;
        virtual bool HavingAuthor(const std::string& name, pqxx::work& work) = 0;
        virtual pqxx::work StartTranzaction() = 0;


        virtual void AddBook(ui::detail::AddBookParams book, pqxx::work& work) = 0;
        virtual std::vector<ui::detail::BookInfo> GetBookVec(pqxx::work& work) = 0;
        virtual std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author, pqxx::work& work) = 0;
        virtual std::vector<ui::detail::BookInfo> GetBooksByTitle(std::string title,pqxx::work& work) = 0;
        virtual std::set<std::string> GetTagsByBookId(std::string bookid,pqxx::work& work) = 0;

        
        virtual void DeleteAuthor(const std::string &name, pqxx::work& work) = 0;
        virtual void DeleteBook(const std::string &bookid, pqxx::work& work) = 0;
        virtual void UpdateAuthor(const std::string &oldname,const std::string &newname, pqxx::work& work) = 0;
        virtual void EditBook(const ui::detail::BookRefreshes& refreshes, pqxx::work& work) = 0;
    
    
    protected:
        ~UseCases() = default;
    };

} // namespace app
