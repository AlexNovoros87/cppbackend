#pragma once
#include "../domain/author_fwd.h"
#include "use_cases.h"
#include <memory>
#include <pqxx/result>
#include <iostream>

namespace app
{
    using pqxx::operator"" _zv;
    class UseCasesImpl : public UseCases
    {
    public:
        explicit UseCasesImpl(std::string adress) : connection_(adress)
        {
            BuildTables();
            Prepares_requests();
        }

        pqxx::work StartTranzaction() override
        {
            return pqxx::work{connection_};
        }

        std::string AddAuthor(const std::string &name, pqxx::work &work) override;
        std::vector<ui::detail::AuthorInfo> GetAuthorVec(pqxx::work &work) override;
        bool HavingAuthor(const std::string &name, pqxx::work &work) override;

        void AddBook(ui::detail::AddBookParams book, pqxx::work &work) override;
        std::vector<ui::detail::BookInfo> GetBookVec(pqxx::work &work) override;
        std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author, pqxx::work &work) override;
        std::vector<ui::detail::BookInfo> GetBooksByTitle(std::string title, pqxx::work &work) override;

        void DeleteBook(const std::string &bookid, pqxx::work &work) override
        {
           std::cout<<bookid<<std::endl;
            
             // std::string del_tags = "DELETE FROM book_tags WHERE book_id = '" + bookid + "';";
            work.exec_prepared(p::DELETE_TAGS_BY_BOOK_ID,bookid);
            // std::string del_book = "DELETE FROM books WHERE id = '" + bookid + "';";
            work.exec_prepared(p::DELETE_BOOK_BY_ID, bookid);
                    
           
        };

        void DeleteAuthor(const std::string &name, pqxx::work &work) override
        {

            //std::string find_auth = "SELECT id FROM authors WHERE name = '" + name + "';";
            //auto [auth_id] = work.query1<std::string>(find_auth);
            auto row = work.exec_prepared1(p::GET_AUTHOR_ID_BY_AUTHOR_NAME,name);
            std::string auth_id = row[0].as<std::string>();
            
            std::vector<std::string> book_idis;
         //   std::string get_auth_books = "SELECT id FROM books WHERE author_id = " + work.quote(auth_id) + ";";

            for (auto [bk_id] : work.exec_prepared(p::GET_BOOK_ID_BY_AUTHOR_ID, auth_id).iter<std::string>())
            {
            
                book_idis.push_back(std::move(bk_id));
            }
            
            for (auto &&bk : book_idis)
            {
                DeleteBook(bk, work);
            }
  
          //  std::string delauth = "DELETE FROM authors WHERE id = " + work.quote(auth_id) + ";";
          work.exec_prepared(p::DELETE_AUTHOR_BY_ID,auth_id);
        
        };

        void UpdateAuthor(const std::string &oldname, const std::string &newname, pqxx::work &work) override
        {
          // "UPDATE authors SET name = '" + newname + "' WHERE name = '" + oldname + "';";
           work.exec_prepared(p::UPDATE_AUTHOR_NAME_IN_AUTHORS,newname,oldname);
        };

        void EditBook(const ui::detail::BookRefreshes &refreshes, pqxx::work &work) override
        {

             if (refreshes.new_title)
            {
               // "UPDATE books SET title = '" + *refreshes.new_title + "' WHERE id = '" + refreshes.book_id + "';";
                work.exec_prepared(p::UPDATE_BOOK_YEAR_BY_ID, *refreshes.new_title, refreshes.book_id );
            }

            if (refreshes.publication_year)
            {
               //"UPDATE books SET publication_year = '" + std::to_string(*refreshes.publication_year) + "' WHERE id = '" + refreshes.book_id + "';";
               work.exec_prepared(p::UPDATE_BOOK_YEAR_BY_ID,*refreshes.publication_year, refreshes.book_id );
            }

            if (refreshes.tags)
            {
                 // "DELETE FROM book_tags WHERE book_id = " + work.quote(refreshes.book_id) + " ;";
                 work.exec_prepared(p::DELETE_TAGS_BY_BOOK_ID,refreshes.book_id);

                for (const std::string &tag : *refreshes.tags)
                {
                    //"INSERT INTO book_tags (book_id, tag) 
                    //VALUES(" + work.quote(refreshes.book_id) + ", " + work.quote(tag) + ");";
                    work.exec_prepared(p::INSERT_TAG, refreshes.book_id, tag);   
                }
            };
        };

        std::set<std::string> GetTagsByBookId(std::string bookid, pqxx::work &work) override
        {

           // std::string query = "SELECT tag FROM book_tags WHERE book_id = '" + bookid + "';";
            std::set<std::string> result;
            for (auto [name] : work.exec_prepared(p::GET_TAG_BY_BOOK_ID,bookid).iter<std::string>())
            {
                result.insert(std::move(name));
            };
            return result;
        };

    private:
        pqxx::connection connection_;
        postgres::AuthorRepositoryImpl authors_;
        postgres::BookRepositoryImpl books_;

        void Prepares_requests()
        {

            // ADDBOOK
            connection_.prepare(p::INSERT_BOOK, 
            "INSERT INTO books (id, title, publication_year, author_id) VALUES($1, $2, $3, $4);"_zv);
            connection_.prepare(p::INSERT_TAG, "INSERT INTO book_tags (book_id, tag) VALUES($1, $2);"_zv);

            // ADDAUTHOR
            connection_.prepare(p::INSERT_AUTHOR, R"(
                           INSERT INTO authors (id, name) VALUES ($1, $2)
                           ON CONFLICT (id) DO UPDATE SET name=$2;
                           )"_zv);
            connection_.prepare(p::DELETE_AUTHOR_BY_ID,"DELETE FROM authors WHERE id = $1;"_zv);
            connection_.prepare(p::DELETE_BOOK_BY_ID, "DELETE FROM books WHERE id = $1;"_zv);
            connection_.prepare(p::DELETE_TAGS_BY_BOOK_ID, "DELETE FROM book_tags WHERE book_id = $1;"_zv);

            ////////////////////////////////////////////////////////////////////////////////////////////////
            connection_.prepare(p::UPDATE_AUTHOR_NAME_IN_AUTHORS,"UPDATE authors SET name = $1 WHERE name = $2;");
            connection_.prepare(p::UPDATE_BOOK_TITLE_BY_ID, "UPDATE books SET title = $1 WHERE id = $2;");
            connection_.prepare(p::UPDATE_BOOK_YEAR_BY_ID, "UPDATE books SET publication_year = $1 WHERE id = $2;");


            /////////////////////////////////////////////////////////////////
            connection_.prepare(p::GET_ALL_BOOKS_BY_TITLE, "SELECT * FROM books WHERE title = $1;"_zv);
            connection_.prepare(p::GET_ALL_BOOKS, "SELECT * FROM books;"_zv);
            connection_.prepare(p::GET_TITLE_PUBYEAR_BY_AUTHOR_ID, 
            "SELECT title, publication_year FROM books WHERE author_id = $1;"_zv);
            connection_.prepare(p::GET_BOOK_ID_BY_AUTHOR_ID,"SELECT id FROM books WHERE author_id = $1;"_zv);
            
            
            
            connection_.prepare(p::GET_ALL_AUTHORS_SORT_BY_NAME, "SELECT * FROM authors ORDER BY name;"_zv);
            connection_.prepare(p::GET_AUTHOR_NAME_BY_NAME, "SELECT name FROM authors WHERE name = $1;"_zv);
            connection_.prepare(p::GET_AUTHOR_NANE_BY_AUTHOR_ID,"SELECT name FROM authors WHERE id = $1;"_zv);
            connection_.prepare(p::GET_AUTHOR_ID_BY_AUTHOR_NAME ,"SELECT id FROM authors WHERE name = $1 ;"_zv);
            
            
            ///////////////////////////////////////////////////////////////////////////////////////////
            
            
            
            
             connection_.prepare(p::GET_TAG_BY_BOOK_ID,"SELECT tag FROM book_tags WHERE book_id = $1;"_zv);
            
            
             
             
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
            // connection_.prepare(,);
        
        
        
        
        
        }

        void BuildTables()
        {

            pqxx::work work{connection_};
            work.exec_params(R"(
CREATE TABLE IF NOT EXISTS authors 
(
    id UUID CONSTRAINT firstindex PRIMARY KEY,
    name varchar(100) NOT NULL UNIQUE 
);

)"_zv);

            work.exec_params(R"(

CREATE TABLE IF NOT EXISTS books
(
    id UUID PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    publication_year INT,
    author_id UUID,
    CONSTRAINT fk_authors
        FOREIGN KEY(author_id)
        REFERENCES authors(id)
);

)"_zv);

            work.exec_params(R"(
    CREATE TABLE IF NOT EXISTS book_tags 
(
    book_id UUID,
    tag varchar(30) NOT NULL,
    CONSTRAINT fk_books
        FOREIGN KEY(book_id)
        REFERENCES books(id)
);
)"_zv);

            work.commit();
        }
    };

} // namespace app
