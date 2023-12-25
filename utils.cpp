#include "utils.hpp"

namespace CrazyDave {
    wchar_t special[]{0x00B7, 0x2014, 0xFF08, 0xFF09, 0xFF1A};

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

    std::vector<std::wstring> split(const std::wstring &str, const std::wstring &delimiter) {
        std::vector<std::wstring> tokens;
        std::size_t start = 0;
        std::size_t end = str.find(delimiter);

        while (end != std::wstring::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }

        tokens.push_back(str.substr(start));

        return std::move(tokens);
    }

    std::wstring parse_utf8(const std::string &str) {
        std::size_t i = 0;
        std::wstring wstr;
        for (; i < str.length(); ++i) {
            char byte = str[i];
            wchar_t ubyte = 0;
            int num = 0;

            if ((byte & 0x80) == 0) {
                // ASCII 字符，1 字节
                ubyte = byte;
                num = 1;
            } else if ((byte & 0xE0) == 0xC0) {
                // 2 字节字符
                ubyte = byte & 0x1F;
                num = 2;
            } else if ((byte & 0xF0) == 0xE0) {
                // 3 字节字符
                ubyte = byte & 0x0F;
                num = 3;
            }

            // 处理多字节字符的其他字节
            for (int j = 1; j < num; ++j) {
                byte = str[++i];
                ubyte = (ubyte << 6) | (byte & 0x3F);
            }
            wstr.push_back(ubyte);
        }
        return std::move(wstr);
    }

    bool chk_nm_au(const std::string &arg) {
        auto wstr = parse_utf8(arg);
        if (wstr.empty() || wstr.length() > 60)
            return false;
        for (auto &c: wstr) {
            if (c == 0x20 || c == 0x21 || (c >= 0x23 && c <= 0x7E) || (c >= 0x4E00 && c <= 0x9FFF)) {
                continue;
            }
            bool flag = false;
            for (auto &sp: special) {
                if (c == sp) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                return false;
            }
        }
        return true;
    }

    bool chk_kw(const std::string &arg) {
        auto wstr = parse_utf8(arg);
        auto len = wstr.length();
        if (wstr.empty() || wstr.length() > 60)
            return false;
        if (wstr[0] == '|' || wstr[len - 1] == '|')
            return false;
        for (int i = 0; i < len; ++i) {
            wchar_t c = wstr[i];
            if (c == '|' && i < len - 1 && wstr[i + 1] == '|') {
                return false;
            }
            if (c == 0x20 || c == 0x21 || (c >= 0x23 && c <= 0x7E) || (c >= 0x4E00 && c <= 0x9FFF)) {
                continue;
            }
            bool flag = false;
            for (auto &sp: special) {
                if (c == sp) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                return false;
            }
        }
        std::unordered_set<std::wstring> us;
        auto arr = split(wstr, std::wstring{'|'});
        for (auto &x: arr) {
            if (us.count(x)) {
                return false;
            }
            us.insert(x);
        }

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
//            patterns.emplace_back(R"([\x20-\x21\x23-\x7E]{1,60})");
            return chk_nm_au(arg);
        } else if (type == "Keyword") {
//            patterns.emplace_back(R"([\x20-\x21\x23-\x7E]{1,60})");
//            patterns.emplace_back(R"(([^\|]+\|)*[^\|]+)");
            return chk_kw(arg);
        } else if (type == "Quantity" || type == "Count") {
            patterns.emplace_back("\\d{1,10}");
        } else if (type == "Price" || type == "TotalCost") {
            patterns.emplace_back(R"(^(?=.{1,13}$)\d*(\.\d*)?$)");
            patterns.emplace_back("^(?!\\.$).*$"); // 排除.
        }
        for (auto &p: patterns) {
            if (!std::regex_match(arg, p)) {
                return false;
            }
        }
//        if (type == "Keyword") {
//            std::unordered_set<std::string> us;
//            auto arr = split(arg, "|");
//            for (auto &x: arr) {
//                if (us.count(x)) {
//                    return false;
//                }
//                us.insert(x);
//            }
//        }
        return true;
    }

    bool check_input(const std::string &op, const std::vector<std::string> &args) {
        if (op == "exit" || op == "quit") {
            return args.empty();
        } else if (op[0] == '#') {
            return true;
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
//                        assert(false);
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
        } else if (op == "report") {
            if (args.size() != 1) {
                return false;
            } else if (args[0] != "finance" && args[0] != "employee") {
                return false;
            }
        } else if (op == "log") {
            if (!args.empty()) {
                return false;
            }
        } else if (op == "$get_account") {
            return true;
        } else {
            return false;
        }
        return true;
    }
}