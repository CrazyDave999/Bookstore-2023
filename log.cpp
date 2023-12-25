#include "log.hpp"

//
// Created by Kingsly on 2023-12-11.
//
namespace CrazyDave {
    const char *sys_name[3]{"[Account]", "[Book]", "[Log]"};
    // 0 for account, 1 for book, 2 for log
    const char *op_name[19]{"LOGIN", "LOGOUT", "REGISTER", "MDFPW", "ADDUSER", "DELETE", "SHOW", "BUY", "SELECT",
                            "MDFISBN", "MDFNAME", "MDFATH", "MDFKW", "MDFPRICE", "IMPORT", "SHOWFN", "RPFINANCE",
                            "RPEMPLOYEE",
                            "LOG"};

    // 0 for login, 1 for logout, 2 for register, 3 for passwd, 4 for useradd, 5 for delete,
    // 6 for show, 7 for buy, 8 for select, 9 for modify ISBN, 10 for modify name, 11 for modify author,
    // 12 for modify keyword, 13 for modify price, 14 for import, 15 for show finance, 16 for report finance,
    // 17 for report employee, 18 for log
    LogSystem::LogSystem() : System() {}

    void LogSystem::add_value(double val) {
        int cur = value_list.size();
        value_list.insert(cur + 1, val);
    }

    bool LogSystem::show_finance(int num) {
        if (!check_privilege(7))return false;
        int sz = value_list.size();
        if (num > sz)return false;
        if (num == 0) {
            std::cout << '\n';
            return true;
        }
        auto arr = value_list.find_all();
        if (num < 0)num = arr.size();
        double income = 0, outcome = 0;
        for (auto itr = arr.end() - num; itr != arr.end(); ++itr) {
            if (*itr > 0)income += *itr;
            else outcome -= *itr;
        }
        std::cout << std::fixed << std::setprecision(2) << "+ " << income << " - " << outcome << '\n';

        add_log(false, 2, current_user_id(), 15, "", "");
        return true;
    }

    bool LogSystem::report_finance() {
        if (!check_privilege(7))return false;
        std::cout << "Here is financial report of the bookstore.\n";
        std::cout << std::left << std::setw(7) << "LogID" << '\t' << std::left << std::setw(30) << "UserID" << '\t'
                  << std::left << std::setw(6)
                  << "Type" << '\t'
                  << std::left << std::setw(10) << "Quantity" << "\tvalue\n";
        auto arr = finance_list.find_all();
        for (auto &log: arr) {
            std::cout << log;
        }
        std::cout << "Financial report has been printed successfully.\n";
        add_log(false, 2, current_user_id(), 16, "", "");
        return true;
    }

    bool CrazyDave::LogSystem::report_employee() {
        if (!check_privilege(7))return false;
        std::cout << "Here is the work report of all the employees.\n";
        std::cout << std::left << std::setw(9) << "System" << '\t' << std::left << std::setw(30)
                  << "UserID" << '\t' << std::left << std::setw(10) << "Operation" << '\t'
                  << std::left << std::setw(60) << "Object UserID/ISBN" << '\t' << "Other Message" << '\n';
        auto arr = log_list.find_all();
        auto pos = work_list.find_all();
        for (auto &i: pos) {
            std::cout << arr[i];
        }
        std::cout << "Work report has been printed successfully.\n";
        add_log(false, 2, current_user_id(), 17, "", "");
        return true;
    }

    bool CrazyDave::LogSystem::log() {
        if (!check_privilege(7))return false;
        std::cout << "Here is the log of bookstore system.\n";
        std::cout << std::left << std::setw(9) << "System" << '\t' << std::left << std::setw(30)
                  << "UserID" << '\t' << std::left << std::setw(10) << "Operation" << '\t'
                  << std::left << std::setw(60) << "Object UserID/ISBN" << '\t' << "Other Message" << '\n';
        auto arr = log_list.find_all();
        for (auto &log: arr) {
            std::cout << log;
        }
        std::cout << "\nHere is the financial report.\n";
        std::cout << std::left << std::setw(7) << "LogID" << '\t' << std::left << std::setw(30) << "UserID" << '\t'
                  << std::left << std::setw(6)
                  << "Type" << '\t'
                  << std::left << std::setw(10) << "Quantity" << "\tvalue\n";
        auto farr = finance_list.find_all();
        for (auto &log: farr) {
            std::cout << log;
        }

        std::cout << "Log has been printed successfully.\n";
        add_log(false, 2, current_user_id(), 18, "", "");
        return true;
    }

    void CrazyDave::LogSystem::add_finance_log(const String<31> &_user_id,
                                               const String<21> &ISBN, std::size_t _type, std::size_t _quantity,
                                               double _value) {
        int cur = finance_list.size();
        FinanceLog log(cur + 1, _user_id, ISBN, _type, _quantity, _value);
        finance_list.insert(cur + 1, log);
    }

    void LogSystem::add_log(bool is_employee, std::size_t _sys_type, const String<31> &_user_id, std::size_t _type,
                            const String<61> _obj, const String<150> _message) {
        int cur = log_list.size();
        Log log(cur, _sys_type, _user_id, _type, _obj, _message);
        log_list.insert(cur, log);
        if (is_employee) {
            work_list.insert(_user_id, cur);
        }
    }

    FinanceLog::FinanceLog(std::size_t _id, const String<31> &_user_id,
                           const String<21> &ISBN, std::size_t _type, std::size_t _quantity,
                           double _value) : id(_id), user_id(_user_id), ISBN(ISBN), type(_type),
                                            quantity(_quantity), value(_value) {}

    std::ostream &operator<<(std::ostream &os, FinanceLog &rhs) {
        os << "#" << std::right << std::setw(6) << std::setfill('0') << rhs.id << std::setfill(' ') << '\t' << std::left
           << std::setw(30)
           << rhs.user_id << '\t'
           << std::left << std::setw(6);
        if (rhs.type) {
            os << "IMPORT";
        } else {
            os << "BUY";
        }
        os << '\t' << std::left << std::setw(10) << rhs.quantity << '\t' << rhs.value << '\n';
        return os;
    }

    Log::Log(std::size_t _id, std::size_t _sys_type, const String<31> &_user_id, std::size_t _type,
             const String<61> &_obj,
             const String<150> &_message)
            : id(_id), sys_type(_sys_type), user_id(_user_id), type(_type), obj(_obj), message(_message) {}

    std::ostream &operator<<(std::ostream &os, Log &rhs) {
        os << std::left << std::setw(9) << sys_name[rhs.sys_type] << '\t' << std::left << std::setw(30)
           << rhs.user_id << '\t' << std::left << std::setw(8) << op_name[rhs.type] << '\t'
           << std::left << std::setw(60) << rhs.obj << '\t' << rhs.message << '\n';
        return os;
    }

}