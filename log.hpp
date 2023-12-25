#ifndef BOOKSTORE_2023_LOG_HPP
#define BOOKSTORE_2023_LOG_HPP

#include "Blocklist/blocklist.hpp"
#include "core.hpp"
#include <iomanip>

namespace CrazyDave {
    class Account;

    bool check_privilege(int);

    Account &get_current_account();

    CrazyDave::String<21> &get_current_select();

    bool is_selected();

    String<31> current_user_id();

    class Log {
    private:
        std::size_t id{};
        std::size_t sys_type{}; // 0 for account, 1 for book, 2 for log
        String<31> user_id;
        std::size_t type{};
        String<61> obj; // user_id or ISBN
        String<150> message;

    public:
        Log() = default;

        Log(std::size_t _id, std::size_t _sys_type, const String<31> &_user_id, std::size_t _type,
            const String<61> &_obj,
            const String<150> &_message);

        bool operator<(const Log &rhs) const {
            return id < rhs.id;
        }

        bool operator==(const Log &rhs) const {
            return id == rhs.id;
        }

        friend std::ostream &operator<<(std::ostream &os, Log &rhs);
    };

    class FinanceLog {
    private:
        std::size_t id{};
        String<31> user_id;
        String<21> ISBN;
        std::size_t type{}; // 0 for buy, 1 for import
        std::size_t quantity{};
        double value{};
    public:
        FinanceLog() = default;

        FinanceLog(std::size_t _id, const String<31> &_user_id, const String<21> &ISBN, std::size_t _type,
                   std::size_t _quantity, double _value);

        bool operator<(const FinanceLog &rhs) const {
            return id < rhs.id;
        }

        bool operator==(const FinanceLog &rhs) const {
            return id == rhs.id;
        }

        friend std::ostream &operator<<(std::ostream &os, FinanceLog &rhs);
    };

    class LogSystem : public System {
    private:
        BlockList<int, Log> log_list{"./tmp/log_index", "./tmp/log_data"};
        BlockList<String<31>, int> work_list{"./tmp/work_index", "./tmp/work_data"};
        BlockList<int, FinanceLog> finance_list{"./tmp/finance_index", "./tmp/finance_data"};
        BlockList<int, double> value_list{"./tmp/value_index", "./tmp/value_data"};
//        BlockList<int, WorkLog> work_list{"work_index", "work_data"};
//        BlockList<int, FinanceLog> finance_list{"finance_index", "finance_data"};
//        BlockList<int, double> value_list{"value_index", "value_data"};
    public:
        LogSystem();

        void add_value(double val);

        void add_finance_log(const String<31> &_user_id, const String<21> &ISBN, std::size_t _type,
                             std::size_t _quantity, double _value);

        void add_log(bool is_employee, std::size_t _sys_type, const String<31> &_user_id, std::size_t _type,
                     String<61> _obj,
                     String<150> _message);

        bool show_finance(int num);

        bool report_finance();

        bool report_employee();

        bool log();
    };

}
#endif