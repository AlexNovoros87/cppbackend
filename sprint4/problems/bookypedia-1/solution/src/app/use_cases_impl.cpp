#include "use_cases_impl.h"

#include "../domain/author.h"

namespace app
{
    using namespace domain;

    void UseCasesImpl::AddAuthor(const std::string &name)
    {
        authors_.Save({AuthorId::New(), name});
    }

    void UseCasesImpl::ShowAuthors(std::ostream &os)
    {
        authors_.ShowMyAuthors(os);
    };

    std::vector<ui::detail::AuthorInfo> UseCasesImpl::GetAuthorVec()
    {
        return authors_.GetAuthorVec();
    };

    void UseCasesImpl::AddBook(ui::detail::AddBookParams book)
    {

        // Book(BookId id, std::string name, int date)
        books_.Save(std::move(book), BookId::New().ToString());
    };
 
    std::vector<ui::detail::BookInfo> UseCasesImpl::GetBookVec()
    {
        return books_.GetBookVec();
    };
    std::vector<ui::detail::BookInfo> UseCasesImpl::GetBookVecByAuthor(std::string author)
    {
        return books_.GetBookVecByAuthor(std::move(author));
    };

} // namespace app
