#include "use_cases_impl.h"

#include "../domain/author.h"
#include <iostream>

namespace app
{
    using namespace domain;

    std::string UseCasesImpl::AddAuthor(const std::string &name, pqxx::work& work)
    {
      
      return  authors_.Save({AuthorId::New(), name}, work);
      
    }

   
    std::vector<ui::detail::AuthorInfo> UseCasesImpl::GetAuthorVec(pqxx::work& work)
    {
        return authors_.GetAuthorVec(work);
    };

    
    bool UseCasesImpl::HavingAuthor(const std::string& name, pqxx::work& work){

        return authors_.HavingAuthor(name, work);
    }
    
    ////////////////////////////////////////////////////////////////////
    void UseCasesImpl::AddBook(ui::detail::AddBookParams book, pqxx::work& work)
    {
        books_.Save(std::move(book), BookId::New().ToString(), work);
       
    };
 
    std::vector<ui::detail::BookInfo> UseCasesImpl::GetBookVec(pqxx::work& work)
    {
        return books_.GetBookVec(work);
    };
    std::vector<ui::detail::BookInfo> UseCasesImpl::GetBookVecByAuthor(std::string author, pqxx::work& work)
    {
        return books_.GetBookVecByAuthor(std::move(author),work);
    };

    std::vector<ui::detail::BookInfo> UseCasesImpl::GetBooksByTitle(std::string title,pqxx::work& work){
         return books_.GetBooksByTitle(std::move(title), work);
    }

} // namespace app
