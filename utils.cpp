#include "utils.hpp"

namespace CrazyDave {
    std::vector<const char *> split(const char *str, const char *del) {
        std::vector<const char *> res{};
        char tmp[strlen(str) + 1];
        strcpy(tmp, str);
        char *token;
        token = strtok(tmp, del);
        while (token != nullptr) {
            res.push_back(token);
            token = strtok(nullptr, del);
        }
        return std::move(res);
    }

    std::vector<std::string> split(const std::string &str, const std::string &delimiter) {
        std::vector<std::string> tokens;
        std::size_t start = 0;
        std::size_t end = str.find(delimiter);

        while (end != std::string::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }

        tokens.push_back(str.substr(start));

        return std::move(tokens);
    }

    bool is_numeric_string(const char *str) {
        std::size_t i = 0;
        if (str[0] == '+' || str[0] == '-') {
            ++i;
        }
        bool has_digit = false, has_point = false;
        for (; str[i]; ++i) {
            if (std::isdigit(str[i])) {
                has_digit = true;
            } else if (str[i] == '.') {
                if (has_point) {
                    return false;
                }
                has_point = true;
            } else {
                return false;
            }
        }
        if (!has_digit) {
            return false;
        }
        return true;
    }

    bool is_interger_string(const char *str) {
        std::size_t i = 0;
        if (str[0] == '+' || str[0] == '-') {
            ++i;
        }
        for (; str[i]; ++i)
            if (!std::isdigit(str[i]))
                return false;
        return true;
    }

    bool check_arg(const std::string &type, const std::string &arg) {
        std::vector<std::regex> patterns;
        if (type == "UserID" || type == "Password" || type == "CurrentPassword" || type == "NewPassword") {
            patterns.emplace_back("^\\w{1,30}$");
        } else if (type == "Username") {
            patterns.emplace_back("[\\x20-\\x7E]{1,30}");
        } else if (type == "Privilege") {
            patterns.emplace_back("[137]");
        } else if (type == "ISBN") {
            patterns.emplace_back("[\\x20-\\x7E]{1,20}");
        } else if (type == "BookName" || type == "Author") {
            patterns.emplace_back(R"([\x20-\x21\x23-\x7E]{1,60})");
        } else if (type == "Keyword") {
            patterns.emplace_back(R"([\x20-\x21\x23-\x7E]{1,60})");
            patterns.emplace_back(R"(([^\|]+\|)*[^\|]+)");
        } else if (type == "Quantity" || type == "Count") {
            patterns.emplace_back("\\d{1,10}");
        } else if (type == "Price" || type == "TotalCost") {
            patterns.emplace_back(R"(^(?=.{1,13}$)-?\d*(\.\d*)?$)");
            patterns.emplace_back("^(?!-?\\.$).*$"); // 排除.和-.
        }
        for (auto &p: patterns) {
            if (!std::regex_match(arg, p)) {
                return false;
            }
        }
        if (type == "Keyword") {
            std::unordered_set<std::string> us;
            auto arr = split(arg, "|");
            for (auto &x: arr) {
                if (us.count(x)) {
                    return false;
                }
                us.insert(x);
            }
        }
        return true;
    }

    bool check_input(const std::string &op, const std::vector<std::string> &args) {
        if (op == "exit" || op == "quit") {
            return args.empty();
        } else if (op == "su") {
            if (args.size() == 1) {
                if (!check_arg("UserID", args[0])) {
                    return false;
                }
            } else if (args.size() == 2) {
                if (!check_arg("UserID", args[0]) || !check_arg("Password", args[1])) {
                    return false;
                }
            } else {
                return false;
            }
        } else if (op == "logout") {
            if (!args.empty()) {
                return false;
            }
        } else if (op == "register") {
            if (args.size() != 3) {
                return false;
            } else if (!check_arg("UserID", args[0]) || !check_arg("Password", args[1]) ||
                       !check_arg("Username", args[2])) {
                return false;
            }
        } else if (op == "passwd") {
            if (args.size() == 2) {
                if (!check_arg("UserID", args[0]) || !check_arg("NewPassword", args[1])) {
                    return false;
                }
            } else if (args.size() == 3) {
                if (!check_arg("UserID", args[0]) || !check_arg("CurrentPassword", args[1]) ||
                    !check_arg("NewPassword", args[2])) {
                    return false;
                }
            } else {
                return false;
            }
        } else if (op == "useradd") {
            if (args.size() != 4) {
                return false;
            } else {
                if (!check_arg("UserID", args[0]) || !check_arg("Password", args[1]) || !check_arg("Privilege", args[2])
                    || !check_arg("Username", args[3])) {
                    return false;
                }
            }
        } else if (op == "delete") {
            if (args.size() != 1) {
                return false;
            } else {
                if (!check_arg("UserID", args[0])) {
                    return false;
                }
            }
        } else if (op == "show") {
            if (args.empty()) {
                return true;
            } else if (args.size() == 1) {
                if (args[0] == "finance") {
                    return true;
                } else {
                    if (std::regex_match(args[0], std::regex{"^-ISBN=.*"})) {
                        if (!check_arg("ISBN", args[0].substr(6))) {
                            return false;
                        }
                    } else if (std::regex_match(args[0], std::regex{R"(^-name=".*"$)"})) {
                        if (!check_arg("BookName", args[0].substr(7, args[0].length() - 8))) {
                            return false;
                        }
                    } else if (std::regex_match(args[0], std::regex{R"(^-author=".*"$)"})) {
                        if (!check_arg("Author", args[0].substr(9, args[0].length() - 10))) {
                            return false;
                        }
                    } else if (std::regex_match(args[0], std::regex{R"(^-keyword=".*"$)"})) {
                        if (args[0].substr(10, args[0].length() - 11).find('|') != std::string::npos) {
                            return false;
                        }
                        if (!check_arg("Keyword", args[0].substr(10, args[0].length() - 11))) {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            } else if (args.size() == 2) {
                if (args[0] == "finance") {
                    if (!check_arg("Count", args[1])) {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (op == "buy") {
            if (args.size() != 2) {
                return false;
            } else if (!check_arg("ISBN", args[0]) || !check_arg("Quantity", args[1])) {
                return false;
            }
        } else if (op == "select") {
            if (args.size() != 1) {
                return false;
            } else if (!check_arg("ISBN", args[0])) {
                return false;
            }
        } else if (op == "modify") {
            if (args.empty() || args.size() > 5) {
                return false;
            }
            std::unordered_set<std::string> us;
            for (auto &arg: args) {
                if (std::regex_match(arg, std::regex{"^-ISBN=.*"})) {
                    if (us.count("ISBN")) {
                        return false;
                    }
                    if (!check_arg("ISBN", arg.substr(6))) {
                        return false;
                    }
                    us.insert("ISBN");
                } else if (std::regex_match(arg, std::regex{R"(^-name=".*"$)"})) {
                    if (us.count("name")) {
                        return false;
                    }
                    if (!check_arg("BookName", arg.substr(7, arg.length() - 8))) {
                        return false;
                    }
                    us.insert("name");
                } else if (std::regex_match(arg, std::regex{R"(^-author=".*"$)"})) {
                    if (us.count("author")) {
                        return false;
                    }
                    if (!check_arg("Author", arg.substr(9, arg.length() - 10))) {
                        return false;
                    }
                    us.insert("author");
                } else if (std::regex_match(arg, std::regex{R"(^-keyword=".*"$)"})) {
                    if (us.count("keyword")) {
                        return false;
                    }
                    if (!check_arg("Keyword", arg.substr(10, arg.length() - 11))) {
                        return false;
                    }
                    us.insert("keyword");
                } else if (std::regex_match(arg, std::regex{"^-price=.*"})) {
                    if (us.count("price")) {
                        return false;
                    }
                    if (!check_arg("Price", arg.substr(7))) {
                        return false;
                    }
                    us.insert("price");
                } else {
                    return false;
                }
            }
        } else if (op == "import") {
            if (args.size() != 2) {
                return false;
            } else if (!check_arg("Quantity", args[0]) || !check_arg("TotalCost", args[1])) {
                return false;
            }
        } else {
            return false;
        }
        return true;
    }
}