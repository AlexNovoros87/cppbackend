#include "postgres.h"
#include <vector>
#include <pqxx/pqxx>
#include <iostream>

namespace postgres
{

    using namespace std::literals;
    using pqxx::operator"" _zv;

    std::string AuthorRepositoryImpl::Save(const domain::Author &author, pqxx::work &work)
    {
         work.exec_prepared(p::INSERT_AUTHOR, author.GetId().ToString(), author.GetName());
         return author.GetId().ToString();
    }

    std::vector<ui::detail::AuthorInfo> AuthorRepositoryImpl::GetAuthorVec(pqxx::work &work)
    {
        std::vector<ui::detail::AuthorInfo> vc;
        
         //  pqxx::zview query_text = "SELECT * FROM authors ORDER BY name"_zv;
        for (auto [id, name] : work.exec_prepared(p::GET_ALL_AUTHORS_SORT_BY_NAME).iter<std::string,std::string>())
        {
            vc.push_back({std::move(id), std::move(name)});
        }
        return vc;
    }

    bool AuthorRepositoryImpl::HavingAuthor(const std::string &name, pqxx::work &work)
    {

        std::string query_text = "SELECT name FROM authors WHERE name = '" + name + "';";
        auto result = work.query01<std::string>(query_text);
        return result.has_value() ? true : false;
    }

} // namespace postgres

namespace postgres
{

    std::vector<ui::detail::BookInfo> BookRepositoryImpl::GetBooksByTitle(std::string title, pqxx::work &work)
    {

        std::vector<ui::detail::BookInfo> book_ord;

        /*
        //VAR1:
        for (auto [book_id, author_id, title, year] :
                 work.exec_prepared(p::GetBooksByTitle1, title).iter<std::string, std::string, std::string, std::string>())
                 {
                    //
                 }

        //VAR2;
        work.exec_prepared(p::GetBooksByTitle1, title).for_each(
            [&book_ord,&work](std::string book_id, std::string author_id, std::string title, std::string year){

             std::string request = "SELECT name FROM authors WHERE id = '" + std::move(author_id) + "';";
             std::string author_nm = std::get<0>(work.query1<std::string>(std::move(request)));

             book_ord.push_back({std::move(title), stoi(year),
                                std::move(author_nm), std::move(book_id)});
            }
          );
         */

        // auto query_text = "SELECT * FROM books WHERE title ='" + title + "';";

        for (auto [book_id, title, year, author_id] :
             work.exec_prepared(p::GET_ALL_BOOKS_BY_TITLE, title).iter<std::string, std::string, std::string, std::string>())
        {
            
            // std::string request = "SELECT name FROM authors WHERE id = '" + std::move(author_id) + "';";
            // std::string author_nm = std::get<0>(work.query1<std::string>(std::move(request)));
            auto row =  work.exec_prepared1(p::GET_AUTHOR_NANE_BY_AUTHOR_ID,author_id);
            auto author_nm = row[0].as<std::string>();
            

            book_ord.push_back({std::move(title), stoi(year),
                                std::move(author_nm), std::move(book_id)

            });
        }

       
        // std::sort(book_ord.begin(), book_ord.end(), [](const ui::detail::BookInfo &lhs, const ui::detail::BookInfo &rhs)
        //           {
                    
        //             if (lhs.author_name != rhs.author_name){
        //             return lhs.author_name < rhs.author_name;
        //         }
        //         return lhs.publication_year < rhs.publication_year;
                      
        //     });

         return book_ord;
    };

    void BookRepositoryImpl::Save(ui::detail::AddBookParams book, std::string book_id, pqxx::work &work)
    {
                                                                                                                                        
        // НА ДОБАВЛЕНИЕ В БАЗУ КНИГИ
        work.exec_prepared(p::INSERT_BOOK, book_id, book.title, std::to_string(book.publication_year), book.author_id);
        
        
        for (const std::string &tag : book.tags)
        {
            // std::string req = "INSERT INTO book_tags (book_id, tag) VALUES("+ work.quote(book_id) + ", " + work.quote(tag) + ");";
            // ВСТАВИТЬ ТЕГИ
            work.exec_prepared(p::INSERT_TAG, book_id, tag);
        }
    };

    std::vector<ui::detail::BookInfo> BookRepositoryImpl::GetBookVec(pqxx::work &work)
    {
        // nazvanie, god publicazii
        std::vector<ui::detail::BookInfo> book_ord;
        // auto query_text = "SELECT * FROM books;";
        // work.query<std::string, std::string, std::string, std::string>(query_text))

        for (auto [book_id, title, year, author_id] : 
        work.exec_prepared(p::GET_ALL_BOOKS).iter<std::string,std::string,int,std::string>())
        {
            // std::string request = "SELECT name FROM authors WHERE id = '" + std::move(author_id) + "';";
            // std::string author_nm = std::get<0>(work.query1<std::string>(std::move(request)));
            auto row = work.exec_prepared1(p::GET_AUTHOR_NANE_BY_AUTHOR_ID,author_id);
            std::string author_nm = row[0].as<std::string>();
            book_ord.push_back({std::move(title), year, std::move(author_nm), std::move(book_id)});
        }

        std::sort(book_ord.begin(), book_ord.end(), [](const ui::detail::BookInfo &lhs, const ui::detail::BookInfo &rhs)
                  {
                  
                      
                if(lhs.title != rhs.title){
                    return lhs.title < rhs.title;
                }
                else if (lhs.author_name != rhs.author_name){
                    return lhs.author_name < rhs.author_name;
                }
                return lhs.publication_year < rhs.publication_year;      
                      
               });

        return book_ord;
    };

    std::vector<ui::detail::BookInfo> BookRepositoryImpl::GetBookVecByAuthor(std::string author, pqxx::work &work)
    {
        std::vector<ui::detail::BookInfo> book_ord;

        //std::string query_text = "SELECT title, publication_year FROM books WHERE author_id = '" + author + "';";
        //for (auto [title, year] : work.query<std::string, int>(query_text))
        
        for (auto [title, year] : work.exec_prepared(p::GET_TITLE_PUBYEAR_BY_AUTHOR_ID,author).iter<std::string, int>())
        {
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