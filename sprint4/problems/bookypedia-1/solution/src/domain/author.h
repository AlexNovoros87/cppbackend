#pragma once
#include <string>
#include <vector>

#include "../util/tagged_uuid.h"
#include "../defstr.h"

namespace domain {

namespace detail {
struct AuthorTag {};
struct BookTag {};



}  // namespace detail

using AuthorId = util::TaggedUUID<detail::AuthorTag>;
using BookId = util::TaggedUUID<detail::BookTag>;

class Author {
public:
    Author(AuthorId id, std::string name)
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const AuthorId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

private:
    AuthorId id_;
    std::string name_;
};

class AuthorRepository {
public:
    virtual void Save(const Author& author) = 0;
    virtual void ShowMyAuthors(std::ostream& os) = 0;
    virtual std::vector<ui::detail::AuthorInfo> GetAuthorVec() = 0;

protected:
    ~AuthorRepository() = default;
};


class Book {
public:
    Book(std::string id, std::string name, int date)
        : id_(std::move(id))
        , name_(std::move(name)),
          pub_date_(date) {
    }

    const std::string& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    int PublicationDate() const {
        return pub_date_;
    }

private:
    std::string id_;
    std::string name_;
    int pub_date_;
};



class BookRepository {
public:
    virtual void Save(ui::detail::AddBookParams book, std::string book_id) = 0;
    virtual void ShowMyBooks(std::ostream& os) = 0;
    virtual void ShowMyBooksByAuthor(std::ostream& os, std::string author) = 0;
    virtual std::vector<ui::detail::BookInfo> GetBookVec() = 0;
    virtual std::vector<ui::detail::BookInfo> GetBookVecByAuthor(std::string author) = 0;

protected:
    ~BookRepository() = default;
};









}  // namespace domain
