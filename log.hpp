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

    class WorkLog {

    };

    class FinanceLog {

    };

    class LogSystem : public System {
    private:
//        BlockList<int, WorkLog> work_list{"tmp/work_index", "tmp/work_data"};
//        BlockList<int, FinanceLog> finance_list{"tmp/finance_index", "tmp/finance_data"};
//        BlockList<int, double> value_list{"tmp/value_index", "tmp/value_data"};
        BlockList<int, WorkLog> work_list{"work_index", "work_data"};
        BlockList<int, FinanceLog> finance_list{"finance_index", "finance_data"};
        BlockList<int, double> value_list{"value_index", "value_data"};
    public:
        LogSystem();

        void add_value(double val);

        bool show_finance(int num);

        bool report_finance();

        bool report_employee();

        bool log();
    };

}
#endif