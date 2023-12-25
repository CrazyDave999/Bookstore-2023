#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include "book.hpp"
#include "account.hpp"
#include "log.hpp"
#include "core.hpp"



CrazyDave::AccountSystem asys;
CrazyDave::BookSystem bsys;
CrazyDave::LogSystem lsys;
CrazyDave::Core core(&asys, &bsys, &lsys);

int main() {
    asys.load_core(&core);
    bsys.load_core(&core);
    lsys.load_core(&core);

    std::ios::sync_with_stdio(false);
    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        if (std::regex_search(line, std::regex{"[\t\v\f]"})) {
            std::cout << "Invalid\n";
            continue;
        }
        std::string op, arg;
        std::vector<std::string> str_args;

        ss >> op;
        if (op.length() == 0) {
            continue;
        }
        while (ss >> arg) {
            str_args.push_back(arg);
        }

        std::vector<const char *> args{};
        for (auto &str: str_args) {
            args.push_back(str.c_str());
        }

        bool flag = false;
        if (!CrazyDave::check_input(op, str_args)) {
            flag = false;
        } else {
            if (op == "exit" || op == "quit") {
                break;
            } else if (op[0] == '#') {
                continue;
            } else if (op == "su") {
                if (args.size() == 1) {
                    flag = asys.login(args[0], "$");
                } else if (args.size() == 2) {
                    flag = asys.login(args[0], args[1]);
                }
            } else if (op == "logout") {
                flag = asys.logout();
            } else if (op == "register") {
                flag = asys.register_account(args[0], args[1], args[2]);
            } else if (op == "passwd") {
                if (args.size() == 2) {
                    flag = asys.modify_password(args[0], "$", args[1]);
                } else if (args.size() == 3) {
                    flag = asys.modify_password(args[0], args[1], args[2]);
                }
            } else if (op == "useradd") {
                flag = asys.user_add(args[0], args[1], std::stoi(args[2]), args[3]);
            } else if (op == "delete") {
                flag = asys.remove(args[0]);
            } else if (op == "show") {
                if (!args.empty() && !strcmp(args[0], "finance")) {
                    if (args.size() == 1) {
                        flag = lsys.show_finance(-1);
                    } else if (args.size() == 2) {
                        flag = lsys.show_finance(std::stoi(args[1]));
                    }
                } else {
                    flag = bsys.show(str_args);
                }
            } else if (op == "buy") {
                auto pr = bsys.buy(args[0], std::stoi(args[1]));
                flag = pr.first;
                if (flag) {
                    std::cout << std::fixed << std::setprecision(2) << pr.second << '\n';
                }
            } else if (op == "select") {
                flag = bsys.select(args[0]);
            } else if (op == "modify") {
                flag = bsys.modify(str_args);
            } else if (op == "import") {
                flag = bsys.import(std::stoi(args[0]), std::stod(args[1]));
            } else if (op == "report") {
                if (str_args[0] == "finance") {
                    flag = lsys.report_finance();
                } else if (str_args[0] == "employee") {
                    flag = lsys.report_employee();
                }
            } else if (op == "log") {
                flag = lsys.log();
            } else if (op == "$get_account") {
                flag = CrazyDave::AccountSystem::get_account();
            }
        }
        if (!flag) {
//            std::cout << "Invalid " << line << "\n"; // debug
            std::cout << "Invalid\n";
        } else {
            std::cout << "Success\n";
        }
    }
    return 0;
}