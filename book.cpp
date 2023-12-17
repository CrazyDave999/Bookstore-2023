#include "book.hpp"

namespace CrazyDave {

    Book::Book(const char *ISBN, const char *_name, const char *_author, const char *_keywords, double _price) : ISBN(
            ISBN),
                                                                                                                 name(_name),
                                                                                                                 author(_author),
                                                                                                                 price(_price),
                                                                                                                 kw_num(0),
                                                                                                                 stock(0) {
        auto kw_vec = split(_keywords, "|");
        for (auto str: kw_vec) {
            keywords[kw_num++] = str;
        }
    }

    Book::Book() = default;

    Book::Book(const Book &rhs) : ISBN(rhs.ISBN), name(rhs.name), author(rhs.author), kw_num(rhs.kw_num),
                                  price(rhs.price),
                                  stock(rhs.stock) {
        for (std::size_t i = 0; i < kw_num; ++i) {
            keywords[i] = rhs.keywords[i];
        }
    }

    Book &Book::operator=(const Book &rhs) {
        if (this == &rhs)return *this;
        ISBN = rhs.ISBN;
        name = rhs.name;
        author = rhs.author;
        kw_num = rhs.kw_num;
        for (std::size_t i = 0; i < kw_num; ++i) {
            keywords[i] = rhs.keywords[i];
        }
        price = rhs.price;
        stock = rhs.stock;
        return *this;
    }


    std::ostream &operator<<(std::ostream &os, Book &rhs) {
        os << rhs.ISBN << '\t' << rhs.name << '\t' << rhs.author << '\t';
        for (std::size_t i = 0; i < rhs.kw_num; ++i) {
            os << rhs.keywords[i];
            if (i < rhs.kw_num - 1) {
                os << '|';
            }
        }
        os << '\t' << std::fixed << std::setprecision(2) << rhs.price << '\t' << rhs.stock << '\n';
        return os;
    }

    void BookSystem::insert_book(Book &book) {
        ISBN_list.insert(book.ISBN, book);
        name_list.insert(book.name, book.ISBN);
        author_list.insert(book.author, book.ISBN);
        for (std::size_t i = 0; i < book.kw_num; ++i) {
            keyword_list.insert(book.keywords[i], book.ISBN);
        }
    }

    void BookSystem::remove_book(Book &book) {
        ISBN_list.remove(book.ISBN, book);
        name_list.remove(book.name, book.ISBN);
        author_list.remove(book.author, book.ISBN);
        for (std::size_t i = 0; i < book.kw_num; ++i) {
            keyword_list.remove(book.keywords[i], book.ISBN);
        }
    }


    BookSystem::BookSystem() : System() {}

    bool BookSystem::show(const std::vector<std::string> &args) {
        // 仅有一种有效
        if (!check_privilege(1))return false;
        if (args.size() > 1)return false;
        if (!args.empty()) {
            if (std::regex_match(args[0], std::regex{"^-ISBN=.*"})) {
                auto arr = ISBN_list.find(args[0].substr(6).c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    std::cout << arr[0];
                }
            } else if (std::regex_match(args[0], std::regex{R"(^-name=".*"$)"})) {
                auto arr = name_list.find(args[0].substr(7, args[0].length() - 8).c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    for (auto &pISBN: arr) {
                        auto book = ISBN_list.find(pISBN)[0];
                        std::cout << book;
                    }
                }
            } else if (std::regex_match(args[0], std::regex{R"(^-author=".*"$)"})) {
                auto arr = author_list.find(args[0].substr(9, args[0].length() - 10).c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    for (auto &pISBN: arr) {
                        auto book = ISBN_list.find(pISBN)[0];
                        std::cout << book;
                    }
                }
            } else if (std::regex_match(args[0], std::regex{R"(^-keyword=".*"$)"})) {
                auto arr = keyword_list.find(args[0].substr(10, args[0].length() - 11).c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    for (auto &pISBN: arr) {
                        auto book = ISBN_list.find(pISBN)[0];
                        std::cout << book;
                    }
                }
            }
        } else {
            auto arr = ISBN_list.find_all();
            if (arr.empty()) {
                std::cout << '\n';
            } else {
                for (auto &book: arr) {
                    std::cout << book;
                }
            }
        }
        return true;
    }

    std::pair<bool, double> BookSystem::buy(const char *ISBN, int quantity) {
        if (!check_privilege(1))return {false, 0};
        if (quantity <= 0)return {false, 0};
        auto arr = ISBN_list.find(ISBN);
        if (arr.empty())return {false, 0};
        auto &book = arr[0];
        if (book.stock < quantity)return {false, 0};
        remove_book(book);
        book.stock -= quantity;
        insert_book(book);
        double val = quantity * book.price;
        core->l_sys->add_value(val);
        return {true, val};
    }

    bool BookSystem::select(const char *ISBN) {
        if (!check_privilege(3))return false;
        get_current_select() = ISBN;

        auto arr = ISBN_list.find(ISBN);
        if (arr.empty()) {
            Book book(ISBN, "", "", "", 0);
            insert_book(book);
        }
        return true;
    }

    bool BookSystem::modify(const std::vector<std::string> &args) {
        if (!check_privilege(3))return false;
        if (!is_selected())return false;
        auto ISBN = get_current_select();
        auto arr = ISBN_list.find(ISBN);
//        if (arr.empty())return false;
        auto &book = arr[0];
        std::string pISBN;
        std::string name;
        std::string author;
        std::string keywords;
        double price = -1;
        for (auto &arg: args) {
            if (std::regex_match(arg, std::regex{"^-ISBN=.*"})) {
                if (ISBN != arg.substr(6).c_str()) pISBN = arg.substr(6);
                else return false;
            } else if (std::regex_match(arg, std::regex{R"(^-name=".*"$)"})) {
                name = arg.substr(7, arg.length() - 8);
            } else if (std::regex_match(arg, std::regex{R"(^-author=".*"$)"})) {
                author = arg.substr(9, arg.length() - 10);
            } else if (std::regex_match(arg, std::regex{R"(^-keyword=".*"$)"})) {
                keywords = arg.substr(10, arg.length() - 11);
            } else if (std::regex_match(arg, std::regex{"^-price=.*"})) {
                price = std::stod(arg.substr(7), nullptr);
            }
        }
        if (!pISBN.empty()) {
            auto parr = ISBN_list.find(pISBN.c_str());
            if (!parr.empty()) {
                return false; // 冲突
            }
        }
//        std::unordered_set<std::string> us;
        auto kw_vec = split(keywords, "|");
//        for (auto &str: kw_vec) {
//            if (us.count(str)) {
//                return false; // 重复
//            }
//            us.insert(str);
//        }

        remove_book(book);
        if (!pISBN.empty()) {
            std::string old(book.ISBN.c_str());
            change_all(old, pISBN);
            book.ISBN = pISBN;
        }
        if (!name.empty())book.name = name;
        if (!author.empty())book.author = author;
        if (!keywords.empty()) {
            book.kw_num = 0;
            for (auto &str: kw_vec) {
                book.keywords[book.kw_num++] = str;
            }
        }
        if (price != -1)book.price = price;
        insert_book(book);
        return true;
    }

    bool BookSystem::import(int quantity, double total_cost) {
        if (!check_privilege(3))return false;
        if (quantity <= 0)return false;
        if (total_cost <= 0)return false;
        if (!is_selected())return false;
        auto book = ISBN_list.find(get_current_select())[0];
        remove_book(book);
        book.stock += quantity;
        insert_book(book);
        core->l_sys->add_value(-total_cost);
        return true;
    }
}