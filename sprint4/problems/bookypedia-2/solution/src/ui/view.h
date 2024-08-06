#pragma once
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>
#include "../defstr.h"



namespace menu {
class Menu;
}

namespace app {
class UseCases;
}

namespace ui {

 // namespace detail
class View {
public:
    View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output);

private:
    bool AddAuthor(std::istream& cmd_input) const;
    bool AddBook(std::istream& cmd_input) const;
    bool ShowAuthors() const;
    bool ShowBooks() const;
    bool ShowAuthorBooks() const;
    bool ShowBook(std::istream& cmd_input) const;
    bool EditAuthor(std::istream &cmd_input) const;
    bool DeleteAuthor(std::istream &cmd_input) const;
    bool DeleteBook(std::istream &cmd_input) const;
    bool EditBook(std::istream &cmd_input) const;
   
    std::optional<detail::AddBookParams> GetBookParams(std::istream& cmd_input, pqxx::work& work) const;
    std::optional<std::string> SelectAuthor(pqxx::work& work) const;
    std::vector<detail::AuthorInfo> GetAuthors(pqxx::work& work) const;
    std::vector<detail::BookInfo> GetBooks(pqxx::work& work) const;
    std::vector<detail::BookInfo> GetAuthorBooks(const std::string& author_id, pqxx::work& work) const;


    ui::detail::BookRefreshes GetBookRefreshes(ui::detail::BookInfo info) const;


    void ClearInput() const {
        while (input_.peek() == '\n' || input_.peek() == '\0' || input_.peek() == '\r' || input_.peek() == ' ')
        {
            input_.get();
        }
        
    }

    menu::Menu& menu_;
    app::UseCases& use_cases_;
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace ui