#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_{authors}, books_{books} {
    }

    void AddAuthor(const std::string& name) override;
    void ShowAuthors(std::ostream& os) override;
    std::vector<ui::detail::AuthorInfo> GetAuthorVec() override;


    void AddBook(ui::detail::AddBookParams book) override;
    std::vector<ui::detail::BookInfo> GetBookVec() override;
    std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author) override;

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
