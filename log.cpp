#include "log.hpp"

//
// Created by Kingsly on 2023-12-11.
//
CrazyDave::LogSystem::LogSystem() : System() {}

void CrazyDave::LogSystem::add_value(double val) {
    int cur = value_list.size();
    value_list.insert(cur + 1, val);
}

bool CrazyDave::LogSystem::show_finance(int num) {
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
    return true;
}

bool CrazyDave::LogSystem::report_finance() {
    return false;
}

bool CrazyDave::LogSystem::report_employee() {
    return false;
}

bool CrazyDave::LogSystem::log() {
    return false;
}

