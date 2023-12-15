#include "account.hpp"

namespace CrazyDave {
    std::vector<std::pair<Account, CrazyDave::String<21>>> login_stack;

    void change_all(String<21> &old_ISBN, String<21> &new_ISBN){
        for (auto &pr: login_stack) {
            if (pr.second == old_ISBN) {
                pr.second = new_ISBN;
            }
        }
    }

    Account::Account(const char *_user_id, const char *_password, int _privilege, const char *_user_name)
            : user_id(
            _user_id), password(_password
    ), privilege(_privilege), user_name(_user_name) {}

    Account::Account() : user_id("$"), password("$"), privilege(0), user_name("$") {}

    bool Account::check_password(const char *_password) {
        return password == _password;
    }

    void Account::modify_password(const char *_password) {
        password = _password;
    }

    bool check_privilege(int privilege) {
        if (login_stack.empty()) {
            return false;
        }
        return get_current_account().privilege >= privilege;
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
            Account root("root", "sjtu", 7, "LJB");
            blockList.insert(root.user_id, root);
        }
    }


    bool AccountSystem::login(const char *user_id, const char *password) {
        auto &cur = get_current_account();
        auto arr = blockList.find(user_id);
        if (arr.empty()) return false;
        auto &user = arr[0];
        if ((password[0] == '$' && cur.privilege > user.privilege) || user.check_password(password)) {
            login_stack.emplace_back(user, "");
            return true;
        }
        return false;
    }

    bool AccountSystem::logout() {
        if (login_stack.empty())return false;
        auto &cur = get_current_account();
        if (cur.privilege < 1)return false;
        login_stack.pop_back();
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
            blockList.remove(user_id, user);
            user.modify_password(new_pw);
            blockList.insert(user_id, user);
            return true;
        }
        return false;
    }

    bool AccountSystem::user_add(const char *user_id, const char *password, int privilege,
                                 const char *user_name) {
        if (login_stack.empty())return false;
        if (!check_privilege(3))return false;
        auto &cur = get_current_account();
        if (privilege >= cur.privilege)return false;
        auto arr = blockList.find(user_id);
        if (!arr.empty())return false;
        Account user(user_id, password, privilege, user_name);
        blockList.insert(user_id, user);
        return true;
    }

    bool AccountSystem::register_account(const char *user_id, const char *password, const char *user_name) {
        auto arr = blockList.find(user_id);
        if (!arr.empty())return false;
        Account user(user_id, password, 1, user_name);
        blockList.insert(user_id, user);
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
        return true;
    }


}




