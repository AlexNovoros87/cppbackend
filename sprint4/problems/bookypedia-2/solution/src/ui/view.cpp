#include "view.h"

#include <boost/algorithm/string.hpp>
#include <cassert>
#include <iostream>
#include <sstream>
#include <variant>

#include "../app/use_cases.h"
#include "../menu/menu.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui
{
    namespace detail
    {

        std::string ReplaceSpaces(const std::string &word)
        {
            std::string new_word;
            char lastch = 'Q';
            for (char ch : word)
            {
                if (ch != ' ')
                {
                    new_word.push_back(ch);
                    lastch = ch;
                }
                else
                {
                    if (lastch != ' ')
                        new_word.push_back(ch);
                    lastch = ch;
                }
            }
            return new_word;
        };

        std::set<std::string> TagContainer(std::string str)
        {

            boost::algorithm::trim(str);
            std::set<std::string> result;
            std::set<std::string> pushback;

            boost::algorithm::split(result, str, boost::is_any_of(","));
            for (auto word : result)
            {

                boost::algorithm::trim(word);
                if (!word.empty())
                {
                    word = ReplaceSpaces(word);
                    pushback.insert(word);
                }
            }
            return pushback;
        }

        std::ostream &operator<<(std::ostream &out, const AuthorInfo &author)
        {
            out << author.name;
            return out;
        }

        std::ostream &operator<<(std::ostream &out, const BookInfo &book)
        {
            out << book.title << ", " << book.publication_year;
            return out;
        }

    } // namespace detail

    template <typename T>
    void PrintVector(std::ostream &out, const std::vector<T> &vector)
    {
        int i = 1;
        for (auto &value : vector)
        {
            out << i++ << " " << value << std::endl;
        }
    }

    constexpr std::string_view zp = ", ";
    constexpr std::string_view by = " by ";

    ui::detail::BookRefreshes View::GetBookRefreshes(ui::detail::BookInfo info) const
    {
        ui::detail::BookRefreshes refs;
        refs.book_id = std::move(info.book_id);

        output_ << "Enter new title or empty line to use the current one (" << info.title << "):" << std::endl;
        std::string newtitle;
        std::getline(input_, newtitle);
        boost::algorithm::trim(newtitle);
        if (!newtitle.empty())
        {
            refs.new_title = std::move(newtitle);
        }

        output_ << "Enter publication year or empty line to use the current one (" << info.publication_year << "):" << std::endl;
        std::string new_pyear;
        std::getline(input_, new_pyear);
        boost::algorithm::trim(new_pyear);
        if (!new_pyear.empty())
        {
            size_t count;
            int transf = std::stoi(new_pyear, &count);
            if (count != new_pyear.size())
                throw std::runtime_error("Bad Year");
            refs.publication_year = transf;
        }

        output_ << "Enter tags (current tags: ";
        size_t i = 0;
        for (auto &&tag : info.tags)
        {
            output_ << tag;

            if (i != info.tags.size() - 1)
            {
                output_ << zp;
            }
            ++i;
        }
        output_ << "):" << std::endl;

        std::string new_tags;
        std::getline(input_, new_tags);
        boost::algorithm::trim(new_tags);

        refs.tags = ui::detail::TagContainer(new_tags);

        return refs;
    };

    void PrintVectorWithoutTitles(std::ostream &out, const std::vector<ui::detail::BookInfo> &vector)
    {
        int i = 1;
        for (auto &value : vector)
        {
            out << i << " " << value.title << by << value.author_name << zp << value.publication_year << std::endl;
            ++i;
        }
    }

    void PrintFullDecriptionOfBook(std::ostream &out, const ui::detail::BookInfo &book)
    {
        out << "Title: " << book.title << std::endl;
        out << "Author: " << book.author_name << std::endl;
        out << "Publication year: " << book.publication_year << std::endl;
        if (!book.tags.empty())
        {
            out << "Tags: ";
            size_t i = 0;
            for (auto &&tag : book.tags)
            {
                out << tag;

                if (i != book.tags.size() - 1)
                {
                    out << zp;
                }
                ++i;
            }
            out << std::endl;
        }
    }

    View::View(menu::Menu &menu, app::UseCases &use_cases, std::istream &input, std::ostream &output)
        : menu_{menu}, use_cases_{use_cases}, input_{input}, output_{output}
    {
        menu_.AddAction( //
            "AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1)
            // ����
            // [this](auto& cmd_input) { return AddAuthor(cmd_input); }
        );
        menu_.AddAction("AddBook"s, "<pub year> <title>"s, "Adds book"s,
                        std::bind(&View::AddBook, this, ph::_1));
        menu_.AddAction("ShowAuthors"s, {}, "Show authors"s, std::bind(&View::ShowAuthors, this));
        menu_.AddAction("ShowBooks"s, {}, "Show books"s, std::bind(&View::ShowBooks, this));
        menu_.AddAction("ShowAuthorBooks"s, {}, "Show author books"s,
                        std::bind(&View::ShowAuthorBooks, this));

        menu_.AddAction("ShowBook"s, "<?name>", "Show book by name or will be choise"s,
                        std::bind(&View::ShowBook, this, ph::_1));

        menu_.AddAction("EditAuthor"s, "<?name>", "Edits Authors"s,
                        std::bind(&View::EditAuthor, this, ph::_1));

        menu_.AddAction("DeleteAuthor"s, "<?name>", "Delete book by name or will be choise"s,
                        std::bind(&View::DeleteAuthor, this, ph::_1));

        menu_.AddAction("DeleteBook"s, "<?name>", "Delete book by name or will be choise"s,
                        std::bind(&View::DeleteBook, this, ph::_1));

        menu_.AddAction("EditBook"s, "<?name>", "Edit book by name or will be choise"s,
                        std::bind(&View::EditBook, this, ph::_1));
    }

    bool View::DeleteAuthor(std::istream &cmd_input) const
    {
        try
        {
            auto work = use_cases_.StartTranzaction();
            std::string authorname;
            std::getline(cmd_input, authorname);
            boost::algorithm::trim(authorname);

            std::string name_to_erase;
            // ЕСЛИ НЕ ПУСТОЕ ВВЕДЕННОЕ ИМЯ
            if (!authorname.empty())
            {
                // ПРИСВОИТЬ ИМЯ ДЛЯ УДАЛЕНИЯ
                name_to_erase = std::move(authorname);
            }
            else
            {
                // ПОЛУЧАЕМ АВТОРОВ
                auto authors = GetAuthors(work);
                // ЕСЛИ АВТОРОВ НЕТ
                if (authors.empty())
                    throw std::runtime_error("");

                // ПЕЧАТАЕМ АВТООВ
                PrintVector(output_, authors);

                //  output_<< "Enter author or empty yo cancel"<<std::endl;
                std::string tmp;
                std::getline(input_, tmp);
                boost::algorithm::trim(tmp);
                // ЕСЛИ ОТМЕНА - ИСКЛЮЧЕНИЕ
                if (tmp.empty())
                    throw std::runtime_error("");

                size_t g;
                int ind = std::stoi(tmp, &g);

                // ЕСЛИ НЕПРАВИЛЬНЫЙ ВВОД НОМЕРА
                if (g != tmp.size())
                    throw std::runtime_error("");
                // ЕСЛИ НЕПРАВИЛЬНЫЙ НОМЕР КОНТЕЙНЕРА
                if (ind < 1 || ind > authors.size())
                    throw std::runtime_error("");

                // ПРИСВОИТЬ ИМЯ ДЛЯ УДАЛЕНИЯ
                name_to_erase = authors[ind - 1].name;
            }
            // ЕСЛИ НЕТ АВТОРА
            if (!use_cases_.HavingAuthor(name_to_erase, work))
            {
                throw std::runtime_error("");
            }
            use_cases_.DeleteAuthor(name_to_erase, work);
            work.commit();
        }
        catch (const std::exception &ex)
        {
            //  std::cerr<<ex.what();
            output_ << "Failed to delete author" << std::endl;
            return true;
        }
        catch (...)
        {

            output_ << "Failed to delete author" << std::endl;
            return true;
        }

        return true;
    };

    bool View::EditBook(std::istream &cmd_input) const
    {

        try
        {
            auto work = use_cases_.StartTranzaction();
            std::string bookname;
            std::getline(cmd_input, bookname);
            boost::algorithm::trim(bookname);

            std::vector<ui::detail::BookInfo> books;

            if (bookname.empty())
                books = GetBooks(work);
            else
                books = use_cases_.GetBooksByTitle(bookname, work);

            if (books.empty())
            {
                output_ << "Book not found" << std::endl;
                return true;
            }

            ui::detail::BookRefreshes REFRESHES;

            if (books.size() == 1 && !bookname.empty())
                REFRESHES = GetBookRefreshes(std::move(books[0]));
            else
            {
                PrintVectorWithoutTitles(output_, books);
                output_ << "Enter the book # or empty line to cancel:" << std::endl;

                std::string tmp;
                std::getline(input_, tmp);
                if (tmp.empty()){
                    output_ << "Book not found" << std::endl;
                    return true;
                }

                size_t t;
                int ind = std::stoi(tmp, &t);
                if (t != tmp.size())
                {
                    output_ << "Book not found" << std::endl;
                    return true;
                }

                if (ind < 1 || ind > books.size())
                {
                    output_ << "Book not found" << std::endl;
                    return true;
                }

                REFRESHES = GetBookRefreshes(std::move(books[ind - 1]));
            }

            use_cases_.EditBook(REFRESHES, work);
            work.commit();
            return true;
        }
        catch (const std::exception &ex)
        {
            output_ << "Book not found" << std::endl;
            return true;
        }

        return true;
    };

    bool View::EditAuthor(std::istream &cmd_input) const
    {

        try
        {
            auto work = use_cases_.StartTranzaction();
            std::string authorname;
            std::getline(cmd_input, authorname);
            boost::algorithm::trim(authorname);

            if (!authorname.empty())
            {
                if (!use_cases_.HavingAuthor(authorname, work))
                {
                    throw std::runtime_error("NO AUTHOR");
                }

                output_ << "Enter new name:" << std::endl;
                std::string newname;
                std::getline(input_, newname);
                use_cases_.UpdateAuthor(authorname, newname, work);
            }
            else
            {
                auto v = GetAuthors(work);
                output_ << "Select Author:" << std::endl;
                PrintVector(output_, v);
                output_ << "Enter author # or empty line to cancel" << std::endl;

                std::string tmp;
                std::getline(input_, tmp);
                if (tmp.empty())
                    throw std::runtime_error("autor adding abort");

                size_t t;
                int ind = std::stoi(tmp, &t);
                if (t != tmp.size())
                    throw std::runtime_error("NON CORR NUM");

                if (ind < 1 || ind > v.size())
                    throw std::runtime_error("NON CORR NUM");

                output_ << "Enter new name:" << std::endl;
                std::string newname;
                std::getline(input_, newname);
                boost::algorithm::trim(newname);

                use_cases_.UpdateAuthor(v[ind - 1].name, newname, work);
            }
            work.commit();
            return true;
        }
        catch (const std::exception &ex)
        {
            output_ << "Failed to edit author" << std::endl;
            return true;
        }

        return true;
    };

    bool View::DeleteBook(std::istream &cmd_input) const
    {
        try
        {
            auto work = use_cases_.StartTranzaction();
            std::string bookname;
            std::getline(cmd_input, bookname);
            boost::algorithm::trim(bookname);
            std::vector<ui::detail::BookInfo> books;

            if (!bookname.empty())
            {
                // ИЩЕМ ПО НАЗВАНИЮ
                books = use_cases_.GetBooksByTitle(bookname, work);
                if (books.empty())
                {
                    output_ << "Book not found" << std::endl;
                    return true;
                }
            }
            else
            {
                // ДОСТАЕМ ВСЕ
                books = GetBooks(work);
            }

            if (books.empty())
            {
                // ЕСЛИ КНИГ НЕТ
                output_ << "Failed to delete book" << std::endl;
                return true;
            }
            if (books.size() == 1)
            {
                // ЕСЛИ ОДНА ТО УДАЛЯЕМ
                use_cases_.DeleteBook(books[0].book_id, work);
            }
            else
            {
                // ПЕЧАТАЕМ СПИСОК КНИГ
                PrintVectorWithoutTitles(output_, books);
                output_ << "Enter the book # or empty line to cancel:" << std::endl;

                std::string tmp;
                std::getline(input_, tmp);
                // ЕСЛИ ОТМЕНА УДАЛЕНИЯ
                if (tmp.empty())
                {
                    return true;
                }

                size_t t;
                int ind = stoi(tmp, &t);

                // ЕСЛМ ВВЕЛИ НЕЧТО КРОМЕ ЧИСЛА
                if (t != tmp.size())
                {
                    output_ << "Failed to delete book" << std::endl;
                    return true;
                }

                // ЕСЛИ НЕВЕРНЫЙ ИНДЕКС КОНТЕЙНЕРА
                if (ind < 1 || ind > books.size())
                {
                    output_ << "Failed to delete book" << std::endl;
                    return true;
                }

                use_cases_.DeleteBook(books[ind - 1].book_id, work);
            }
            work.commit();
            return true;
        }
        catch (std::exception &ex)
        {
            output_ << "Failed to delete book" << std::endl;
            return true;
        }

        return true;
    };
    // READY
    bool View::ShowBook(std::istream &cmd_input) const
    {

        std::string showbook;
        std::getline(cmd_input, showbook);
        boost::algorithm::trim(showbook);

        auto work = use_cases_.StartTranzaction();
        std::vector<ui::detail::BookInfo> books;

        if (showbook.empty())
        {
            books = GetBooks(work);
        }
        else
        {
            books = use_cases_.GetBooksByTitle(showbook, work);
        }
        if (books.empty())
            return true;
        if (books.size() == 1 && !showbook.empty())
        {
            books[0].tags = use_cases_.GetTagsByBookId(books[0].book_id, work);
            PrintFullDecriptionOfBook(output_, books[0]);
            return true;
        }

        PrintVectorWithoutTitles(output_, books);
        output_ << "Enter the book # or empty line to cancel:" << std::endl;
        std::string num;
        std::getline(input_, num);

        if (num.empty())
            return true;

        int ind;
        try
        {
            size_t count_stoi;
            ind = stoi(num, &count_stoi);
            if (count_stoi != num.size())
                return true;
            if (ind < 1 || ind > books.size())
                return true;
        }
        catch (std::exception &ex)
        {
            return true;
        }

        books[ind - 1].tags = use_cases_.GetTagsByBookId(books[ind - 1].book_id, work);
        PrintFullDecriptionOfBook(output_, books[ind - 1]);
        return true;
    }

    bool View::AddAuthor(std::istream &cmd_input) const
    {
        try
        {
            std::string name;
            std::getline(cmd_input, name);

            boost::algorithm::trim(name);

            if (name.empty())
                throw std::logic_error("EMPTY AUTHOR NAME");

            auto work = use_cases_.StartTranzaction();
            use_cases_.AddAuthor(std::move(name), work);
            work.commit();
        }
        catch (const std::exception &)
        {
            output_ << "Failed to add author"sv << std::endl;
        }
        return true;
    }

    bool View::AddBook(std::istream &cmd_input) const
    {
        while (cmd_input.peek() == '\n')
            cmd_input.get();
        try
        {
            auto work = use_cases_.StartTranzaction();

            if (auto params = GetBookParams(cmd_input, work))
            {
                use_cases_.AddBook(std::move(*params), work);
                work.commit();
            }
        }
        catch (...)
        {
            output_ << "Failed to add book" << std::endl;
            return true;
        }

        return true;
    }

    bool View::ShowAuthorBooks() const
    {
        // TODO: handle error
        auto work = use_cases_.StartTranzaction();
        if (auto author_id = SelectAuthor(work))
        {
            boost::algorithm::trim(*author_id);
            if ((*author_id).empty())
                return false;
            PrintVector(output_, GetAuthorBooks(*author_id, work));
        }
        else
        {
            return false;
        }

        return true;
    }

    bool View::ShowAuthors() const
    {
        auto work = use_cases_.StartTranzaction();
        PrintVector(output_, GetAuthors(work));

        return true;
    }

    bool View::ShowBooks() const
    {
        auto work = use_cases_.StartTranzaction();
        PrintVectorWithoutTitles(output_, GetBooks(work));
        return true;
    }

    std::pair<int, std::string> YearAndName(std::istream &cmd_input)
    {

        std::string line;
        std::getline(cmd_input, line);

        boost::algorithm::trim(line);
        if (line.empty())
            throw std::runtime_error("bad line");

        auto pos = line.find(' ');
        if (pos == line.npos)
            throw std::runtime_error("bad line");

        std::string year_str = line.substr(0, pos);
        boost::algorithm::trim(year_str);

        std::string title = line.substr(pos);
        boost::algorithm::trim(title);
        if (title.empty())
            throw std::runtime_error("bad title");

        size_t t;
        int year = std::stoi(year_str, &t);
        if (t != year_str.size())
            throw std::runtime_error("bad year");

        return {year, std::move(title)};
    }

    std::optional<detail::AddBookParams> View::GetBookParams(std::istream &cmd_input, pqxx::work &work) const
    {

        detail::AddBookParams params;
        auto y_n = YearAndName(cmd_input);

        params.publication_year = y_n.first;
        params.title = std::move(y_n.second);

        output_ << "Enter author name or empty line to select from list:" << std::endl;
        std::string author_name;
        std::getline(input_, author_name);
        boost::algorithm::trim(author_name);

        // ЕСЛИ НЕ ПУСТАЯ СТОРКА И НЕТ АВТОРА ПО ИМЕНИ
        if (!author_name.empty() && !use_cases_.HavingAuthor(author_name, work))
        {
            output_ << "No author found. Do you want to add " << author_name << " (y/n)?" << std::endl;
            std::string ch;
            std::getline(input_, ch);
            if (ch != "y" && ch != "Y")
            {
                throw std::runtime_error("");
            }
            params.author_id = use_cases_.AddAuthor(author_name, work);
        }
        // ЕСЛИ АВТОРА НАДО ВЫБИРАТЬ ИЗ СПИСКА
        else
        {

            auto author_id = SelectAuthor(work);
            if (not author_id.has_value())
                return std::nullopt;
            else
            {
                params.author_id = author_id.value();
            }
        };

        // ВВОДИМ ТЕГИ
        std::string tagline;
        output_ << "Enter tags (comma separated):" << std::endl;

        std::getline(input_, tagline);
        boost::algorithm::trim(tagline);

        if (!tagline.empty())
        {
            params.tags = ui::detail::TagContainer(tagline);
        }
        return params;
    };

    std::optional<std::string> View::SelectAuthor(pqxx::work &work) const
    {

        // ПОЛУЧАЕМ АВТОРОВ
        auto authors = GetAuthors(work);
        // ЕСЛИ ИХ НЕТ - ИСКЛЮЧЕНИЕ
        if (authors.empty())
            return std::nullopt; // throw std::runtime_error("NoAuthors");

        output_ << "Select author:" << std::endl;
        PrintVector(output_, authors);
        output_ << "Enter author # or empty line to cancel" << std::endl;

        std::string str;
        if (!std::getline(input_, str))
        {
            throw std::runtime_error("getline err");
        }
        boost::algorithm::trim(str);
        if (str.empty())
            return std::nullopt;

        int author_idx;
        size_t t;

        author_idx = std::stoi(str, &t);
        if (t != str.size())
            return std::nullopt; // throw std::runtime_error("Error Stoi cont");

        if (author_idx < 1 || author_idx > authors.size())
        {
            throw std::runtime_error("Invalid author num");
        }
        return authors[author_idx - 1].id;
    }

    std::vector<detail::AuthorInfo> View::GetAuthors(pqxx::work &work) const
    {
        // У АВТОРОВ ВОЗВРАЩАЕМ ВЕКТОР ЕГО СТРУКТУР
        return use_cases_.GetAuthorVec(work);
    }

    std::vector<detail::BookInfo> View::GetBooks(pqxx::work &work) const
    {

        return use_cases_.GetBookVec(work);
    }

    std::vector<detail::BookInfo> View::GetAuthorBooks(const std::string &author_id, pqxx::work &work) const
    {

        return use_cases_.GetBookVecByAuthor(author_id, work);
    }

} // namespace ui
