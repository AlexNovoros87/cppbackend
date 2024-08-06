#pragma once
#include <string>
#include <string_view>
#include <set>
#include<pqxx/transaction>



namespace p
{
constexpr pqxx::zview INSERT_BOOK = "insert book";
constexpr pqxx::zview INSERT_TAG = "insert_tag";
constexpr pqxx::zview INSERT_AUTHOR = "insert_author"; 


 constexpr pqxx::zview DELETE_AUTHOR_BY_ID = "DELETE_AUTHOR_BY_ID"; 
 constexpr pqxx::zview DELETE_BOOK_BY_ID = "DELETE_BOOK_BY_ID";
 constexpr pqxx::zview DELETE_TAGS_BY_BOOK_ID = "DELETE_TAG_BY_BOOK_ID";

//NEW ---> OLD
 constexpr pqxx::zview UPDATE_AUTHOR_NAME_IN_AUTHORS = "UPDATE_AUTHOR_NAME"; 
 constexpr pqxx::zview UPDATE_BOOK_TITLE_BY_ID = "UPDATE_BOOK_TITLE_BY_ID"; 
 constexpr pqxx::zview UPDATE_BOOK_YEAR_BY_ID = "UPDATE_BOOK_YEAR_BY_ID";
// constexpr pqxx::zview 
// constexpr pqxx::zview 






constexpr pqxx::zview GET_AUTHOR_ID_BY_BOOK_ID = "GET_AUTHOR_NAME_BY_BOOK_ID";




//BOOKS
constexpr pqxx::zview GET_ALL_BOOKS_BY_TITLE = "GetBooksByTitle";
constexpr pqxx::zview GET_TITLE_PUBYEAR_BY_AUTHOR_ID = "GET_TITLE_PUBYEAR_BY_BOOK_ID";
constexpr pqxx::zview GET_TAG_BY_BOOK_ID = "GET_TAG_BY_BOOK_ID";
constexpr pqxx::zview GET_ALL_BOOKS = "GET_ALL_BOOKS";
constexpr pqxx::zview GET_BOOK_ID_BY_AUTHOR_ID = "GET_BOOK_ID_BY_AUTHOR_ID";
// constexpr pqxx::zview 
// constexpr pqxx::zview 
// constexpr pqxx::zview 
// constexpr pqxx::zview 
// constexpr pqxx::zview 



constexpr pqxx::zview GET_ALL_AUTHORS_SORT_BY_NAME = "GET_AUTHORS_SORT_BY_NAME";
constexpr pqxx::zview GET_AUTHOR_NAME_BY_NAME = "GET_AUTHOR_BY_NAME";
constexpr pqxx::zview GET_AUTHOR_NANE_BY_AUTHOR_ID = "GET_AUTHOR_NANE_BY_ID";
constexpr pqxx::zview GET_AUTHOR_ID_BY_AUTHOR_NAME = "GET_AUTHOR_ID_BY_AUTHOR_NAME";
// constexpr pqxx::zview 
// constexpr pqxx::zview 
// constexpr pqxx::zview 
// constexpr pqxx::zview 
// constexpr pqxx::zview 







} // namespace p





namespace ui {

namespace detail {

struct AddBookParams {
    std::string title;
    std::string author_id;
    int publication_year = 0;
    std::set<std::string> tags;
};

struct BookRefreshes
{
    std::string book_id;
    std::optional<std::string> new_title;
    std::optional<int> publication_year;
    std::optional<std::set<std::string>> tags;
};

struct AuthorInfo {
    std::string id;
    std::string name;
};

struct BookInfo {
    std::string title;
    int publication_year;
    std::string author_name;
    std::string book_id;
    std::set<std::string> tags;
};

struct TagParams{
   std::string book_id;
   std::set<std::string> tags;
};



}  // namespace deta

}