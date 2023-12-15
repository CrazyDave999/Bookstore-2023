#ifndef BOOKSTORE_2023_UTILS_HPP
#define BOOKSTORE_2023_UTILS_HPP

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <list>

namespace CrazyDave {
    template<size_t len>
    class String {
        char str[len]{'\0'};
    public:
        String() = default;

        String(const char *s) {
            strcpy(str, s);
        }

        explicit operator const char *() {
            return str;
        }

        operator const std::string() {
            return std::move(std::string(str));
        }

        char &operator[](int pos) {
            return str[pos];
        }

        String &operator=(const String &rhs) {
            if (this == &rhs)return *this;
            strcpy(str, rhs.str);
            return *this;
        }

        String &operator=(const char *s) {
            strcpy(str, s);
            return *this;
        }

        String &operator=(const std::string &s) {
            strcpy(str, s.c_str());
            return *this;
        }


        bool operator==(const String &rhs) const {
            return !strcmp(str, rhs.str);
        }

        bool operator!=(const String &rhs) const {
            return strcmp(str, rhs.str);
        }

        bool operator<(const String &rhs) const {
            return strcmp(str, rhs.str) < 0;
        }

        friend int cmp_str(const String &lhs, const String &rhs) {
            return strcmp(lhs.str, rhs.str);
        }

        friend std::istream &operator>>(std::istream &is, String &rhs) {
            return is >> rhs.str;
        }

        friend std::ostream &operator<<(std::ostream &os, const String &rhs) {
            return os << rhs.str;
        }
    };

    std::vector<const char *> split(const char *str, const char *del);

    std::vector<std::string> split(const std::string &str, const std::string &delimiter);

    bool is_numeric_string(const char *str);

    bool is_interger_string(const char *str);

    using std::fstream;

    template<class T>
    int cmp(const T &lhs, const T &rhs) {
        if (lhs < rhs)return -1;
        else return rhs < lhs;
    }


    template<class T>
    int binary_search(const T *first, int len, const T &val) {
        int l = 0, r = len;
        while (l <= r) {
            int mid = (l + r) >> 1;
            int flag = cmp(first[mid], val);
            if (flag < 0) {
                l = mid + 1;
            } else if (flag > 0) {
                r = mid - 1;
            } else {
                return mid;
            }
        }
        return -1;
    }

}
#endif // BOOKSTORE_2023_UTILS_HPP