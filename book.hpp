#ifndef BOOKSTORE_2023_BOOK_HPP
#define BOOKSTORE_2023_BOOK_HPP

#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "Blocklist/blocklist.hpp"
#include "core.hpp"
#include "log.hpp"
#include <unordered_set>

namespace CrazyDave {
    class Account;

    class BookSystem;

    void change_all(String<21> &old_ISBN, String<21> &new_ISBN);

    class Book {
        friend CrazyDave::BookSystem;
    private:
        String<21> ISBN;
        String<61> name;
        String<61> author;
        int kw_num;
        String<61> keywords[31];
        double price;
        int stock;
    public:
        Book();

        Book(const char *ISBN, const char *_name, const char *_author, const char *_keywords, double _price);

        Book(const Book &rhs);

        Book &operator=(const Book &rhs);

        bool operator<(const Book &rhs) const {
            return ISBN < rhs.ISBN;
        }

        bool operator==(const Book &rhs) const {
            return ISBN == rhs.ISBN;
        }

        friend std::ostream &operator<<(std::ostream &os, Book &rhs);
    };

    class BookSystem : public System {
    private:
//        BlockList<CrazyDave::String<21>, Book> ISBN_list{"tmp/ISBN_index", "tmp/ISBN_data"};
//        BlockList<CrazyDave::String<61>, CrazyDave::String<21>> name_list{"tmp/name_index", "tmp/name_data"};
//        BlockList<CrazyDave::String<61>, CrazyDave::String<21>> author_list{"tmp/author_index", "tmp/author_data"};
//        BlockList<CrazyDave::String<61>, CrazyDave::String<21>> keyword_list{"tmp/keyword_index",
//                                                                             "tmp/keyword_data"};
        BlockList<CrazyDave::String<21>, Book> ISBN_list{"ISBN_index", "ISBN_data"};
        BlockList<CrazyDave::String<61>, CrazyDave::String<21>> name_list{"name_index", "name_data"};
        BlockList<CrazyDave::String<61>, CrazyDave::String<21>> author_list{"author_index", "author_data"};
        BlockList<CrazyDave::String<61>, CrazyDave::String<21>> keyword_list{"keyword_index",
                                                                             "keyword_data"};


        void insert_book(Book &book);

        void remove_book(Book &book);

    public:
        BookSystem();

        bool show(const std::vector<const char *> &args);

        std::pair<bool, double> buy(const char *ISBN, int quantity);

        bool select(const char *ISBN);

        bool modify(const std::vector<const char *> &args);

        bool import(int quantity, double total_cost);
    };
}
#endif //BOOKSTORE_2023_BOOK_HPP
