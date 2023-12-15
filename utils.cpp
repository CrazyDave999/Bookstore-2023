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

    std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
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

}