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
        std::string msg;
        if (!args.empty()) {
            if (std::regex_match(args[0], std::regex{"^-ISBN=.*"})) {
                auto ISBN = args[0].substr(6);
                auto arr = ISBN_list.find(ISBN.c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    std::cout << arr[0];
                }
                msg += "ISBN: ";
                msg += ISBN;
            } else if (std::regex_match(args[0], std::regex{R"(^-name=".*"$)"})) {
                auto name = args[0].substr(7, args[0].length() - 8);
                auto arr = name_list.find(name.c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    for (auto &pISBN: arr) {
                        auto book = ISBN_list.find(pISBN)[0];
                        std::cout << book;
                    }
                }
                msg += "name: ";
                msg += name;
            } else if (std::regex_match(args[0], std::regex{R"(^-author=".*"$)"})) {
                auto author = args[0].substr(9, args[0].length() - 10);
                auto arr = author_list.find(author.c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    for (auto &pISBN: arr) {
                        auto book = ISBN_list.find(pISBN)[0];
                        std::cout << book;
                    }
                }
                msg += "author: ";
                msg += author;
            } else if (std::regex_match(args[0], std::regex{R"(^-keyword=".*"$)"})) {
                auto keyword = args[0].substr(10, args[0].length() - 11);
                auto arr = keyword_list.find(keyword.c_str());
                if (arr.empty()) {
                    std::cout << '\n';
                } else {
                    for (auto &pISBN: arr) {
                        auto book = ISBN_list.find(pISBN)[0];
                        std::cout << book;
                    }
                }
                msg += "keyword: ";
                msg += keyword;
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
            msg += "all";
        }
        core->l_sys->add_log(is_employee(), 1, current_user_id(), 6, "", msg);
        return true;
    }

    std::pair<bool, double> BookSystem::buy(const char *ISBN, int quantity) {
        if (!check_privilege(1))return {false, 0};
        if (quantity <= 0)return {false, 0};
        auto arr = ISBN_list.find(ISBN);
        if (arr.empty())return {false, 0};
        auto &book = arr[0];
        if (book.stock < quantity)return {false, 0};

        std::string msg;
        msg += "quantity: ";
        msg += std::to_string(quantity);

        remove_book(book);
        book.stock -= quantity;
        insert_book(book);
        double val = quantity * book.price;
        msg += ", cost: ";
        msg += std::to_string(val);
        core->l_sys->add_value(val);
        core->l_sys->add_finance_log(get_current_account().user_id, ISBN, 0, quantity, val);
        core->l_sys->add_log(is_employee(), 1, current_user_id(), 7, ISBN, msg);
        return {true, val};
    }

    bool BookSystem::select(const char *ISBN) {
        if (!check_privilege(3))return false;
        get_current_select() = ISBN;

        std::string msg;

        auto arr = ISBN_list.find(ISBN);
        if (arr.empty()) {
            Book book(ISBN, "", "", "", 0);
            insert_book(book);
            msg += "new book created";
        }
        core->l_sys->add_log(is_employee(), 1, current_user_id(), 8, ISBN, msg);
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
        auto kw_vec = split(keywords, "|");

        remove_book(book);
        if (!pISBN.empty()) {
            std::string old(book.ISBN.c_str());
            change_all(old, pISBN);
            book.ISBN = pISBN;
            std::string msg;
            msg += old;
            msg += " -> ";
            msg += pISBN;
            core->l_sys->add_log(is_employee(), 1, current_user_id(), 9, ISBN.c_str(), msg);
        }
        if (!name.empty()) {
            std::string msg;
            msg += book.name;
            msg += " -> ";
            msg += name;
            book.name = name;
            core->l_sys->add_log(is_employee(), 1, current_user_id(), 10, ISBN.c_str(), msg);
        }
        if (!author.empty()) {
            std::string msg;
            msg += book.author;
            msg += " -> ";
            msg += author;
            book.author = author;
            core->l_sys->add_log(is_employee(), 1, current_user_id(), 11, ISBN.c_str(), msg);

        }
        if (!keywords.empty()) {
            std::string msg;
            for (int i = 0; i < book.kw_num - 1; ++i) {
                msg += book.keywords[i];
                msg += '|';
            }
            if (book.kw_num > 0) {
                msg += book.keywords[book.kw_num - 1];
            }
            msg += " -> ";

            book.kw_num = 0;
            for (auto &str: kw_vec) {
                book.keywords[book.kw_num++] = str;
            }

            for (int i = 0; i < book.kw_num - 1; ++i) {
                msg += book.keywords[i];
                msg += '|';
            }
            if (book.kw_num > 0) {
                msg += book.keywords[book.kw_num - 1];
            }
            core->l_sys->add_log(is_employee(), 1, current_user_id(), 12, ISBN.c_str(), msg);
        }
        if (price != -1) {
            std::string msg;
            msg += std::to_string(book.price);
            msg += " -> ";
            msg += std::to_string(price);
            book.price = price;
            core->l_sys->add_log(is_employee(), 1, current_user_id(), 13, ISBN.c_str(), msg);
        }
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

        std::string msg;
        msg += "quantity: ";
        msg += std::to_string(quantity);
        msg += ", total cost: ";
        msg += std::to_string(total_cost);
        core->l_sys->add_value(-total_cost);
        core->l_sys->add_finance_log(get_current_account().user_id, get_current_select(), 1, quantity, total_cost);
        core->l_sys->add_log(is_employee(), 1, current_user_id(), 14, get_current_select().c_str(), msg);
        return true;
    }
}