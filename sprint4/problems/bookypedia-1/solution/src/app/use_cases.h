#pragma once

#include <string>
#include <vector>
#include "../defstr.h"

namespace app
{

    class UseCases
    {
    public:
        virtual void AddAuthor(const std::string &name) = 0;
        virtual void ShowAuthors(std::ostream &os) = 0;
        virtual std::vector<ui::detail::AuthorInfo> GetAuthorVec() = 0;

        virtual void AddBook(ui::detail::AddBookParams book) = 0;
        virtual void ShowBooks(std::ostream &os) = 0;
        virtual void ShowBooksByAuthor(std::ostream &os, std::string author) = 0;
        virtual std::vector<ui::detail::BookInfo> GetBookVec() = 0;
        virtual std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author) = 0;

    protected:
        ~UseCases() = default;
    };

} // namespace app
