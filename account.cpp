#include "account.hpp"

namespace CrazyDave {
    std::vector<std::pair<Account, CrazyDave::String<21>>> login_stack;

    void change_all(std::string &old_ISBN, std::string &new_ISBN) {
        for (auto &pr: login_stack) {
            if (pr.second == old_ISBN) {
                pr.second = new_ISBN;
            }
        }
    }

    Account::Account(const char *_user_id, const char *_password, int _privilege, const char *_user_name)
            : user_id(_user_id), password(_password), privilege(_privilege), user_name(_user_name) {}

    Account::Account() : user_id(""), password(""), privilege(0), user_name("") {

    }

    bool Account::check_password(const char *_password) {
        return password == _password;
    }

    void Account::modify_password(const char *_password) {
        password = _password;
    }

    bool check_privilege(int privilege) {
        int cur_privilege;
        if (login_stack.empty()) {
            cur_privilege = 0;
        } else {
            cur_privilege = get_current_account().privilege;
        }
        return cur_privilege >= privilege;
    }


    String<31> current_user_id() {
        if (login_stack.empty()) {
            return "visitor";
        } else {
            return get_current_account().user_id;
        }
    }

    bool is_employee() {
        if (login_stack.empty()) {
            return false;
        }
        return get_current_account().privilege == 3;
    }

    Account &get_current_account() {
        return login_stack.back().first;
    }

    String<21> &get_current_select() {
        return login_stack.back().second;
    }

    bool is_selected() {
        return get_current_select() != "";
    }

    AccountSystem::AccountSystem() : System() {
        if (blockList.get_is_new()) {
//            std::cout << "Insert root\n"; // debug
            Account root("root", "sjtu", 7, "LJB");
            blockList.insert(root.user_id, root);
//            std::cout << "Try to find" << root.user_id << "\n"; // debug
//            std::cout << blockList.find("root").size() << '\n'; // debug
//            std::cout << "Success\n"; // debug
        }
    }


    bool AccountSystem::login(const char *user_id, const char *password) {
        auto &cur = get_current_account();
//        std::cout << "Try to find" << user_id << "\n"; // debug
        auto arr = blockList.find(user_id);
//        if (arr.empty())std::cout << "Not found\n"; // debug
        if (arr.empty()) return false;
        auto &user = arr[0];
        if ((password[0] == '$' && !login_stack.empty() && cur.privilege > user.privilege) ||
            user.check_password(password)) {
            login_stack.emplace_back(user, "");
            core->l_sys->add_log(is_employee(), 0, current_user_id(), 0, user_id, "");
            return true;
        }
//        std::cout << "Password incorrect\n"; // debug
        return false;
    }

    bool AccountSystem::logout() {
        if (!check_privilege(1))return false;
        login_stack.pop_back();
        auto &cur = get_current_account();
        core->l_sys->add_log(is_employee(), 0, current_user_id(), 1, "", "");
        return true;
    }

    bool AccountSystem::modify_password(const char *user_id, const char *cur_pw, const char *new_pw) {
        if (login_stack.empty())return false;
        auto &cur = get_current_account();
        if (cur.privilege < 1)return false;
        auto arr = blockList.find(user_id);
        if (arr.empty())return false;
        auto &user = arr[0];
        if ((cur_pw[0] == '$' && cur.privilege == 7) || user.check_password(cur_pw)) {
            std::string msg(user.password);
            msg += " -> ";
            msg += new_pw;

            blockList.remove(user_id, user);
            user.modify_password(new_pw);
            blockList.insert(user_id, user);

            core->l_sys->add_log(is_employee(), 0, current_user_id(), 3, user_id, msg);
            return true;
        }
        return false;
    }

    bool AccountSystem::user_add(const char *user_id, const char *password, int privilege,
                                 const char *user_name) {
        if (!check_privilege(3))return false;
        auto &cur = get_current_account();
        if (privilege >= cur.privilege)return false;
        auto arr = blockList.find(user_id);
        if (!arr.empty())return false;
        Account user(user_id, password, privilege, user_name);
        blockList.insert(user_id, user);
        std::string msg("password: ");
        msg += password;
        msg += ", privilege: ";
        msg += std::to_string(privilege);
        msg += ", user_name: ";
        msg += user_name;
        core->l_sys->add_log(is_employee(), 0, current_user_id(), 4, user_id, msg);
        return true;
    }

    bool AccountSystem::register_account(const char *user_id, const char *password, const char *user_name) {
        auto arr = blockList.find(user_id);
        if (!arr.empty())return false;
        Account user(user_id, password, 1, user_name);
        blockList.insert(user_id, user);
        std::string msg("password: ");
        msg += password;
        msg += ", user_name: ";
        msg += user_name;
        core->l_sys->add_log(is_employee(), 0, current_user_id(), 2, user_id, msg);
        return true;
    }


    bool AccountSystem::remove(const char *user_id) {
        if (!check_privilege(7))return false;
        auto arr = blockList.find(user_id);
        if (arr.empty())return false;
        for (auto &pr: login_stack) {
            if (pr.first.user_id == user_id) {
                return false;
            }
        }
        auto &user = arr[0];
        blockList.remove(user_id, user);
        core->l_sys->add_log(is_employee(), 0, current_user_id(), 5, user_id, "");
        return true;
    }

    bool AccountSystem::get_account() {
        if (login_stack.empty()) {
            std::cout << "visitor\n";
        } else {
            Account &user = get_current_account();
            std::cout << user.user_id << '\t' << user.password << '\t';
            if (user.user_name == "") {
                std::cout << ' ';
            } else {
                std::cout << user.user_name;
            }
            std::cout << '\t' << user.privilege << '\t';
            auto ISBN = get_current_select();
            if (ISBN == "") {
                std::cout << ' ';
            } else {
                std::cout << ISBN;
            }
            std::cout << "\t\n";
        }
        return true;
    }
}




