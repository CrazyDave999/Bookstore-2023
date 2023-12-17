//
// Created by Kingsly on 2023-12-11.
//

#ifndef BOOKSTORE_2023_ACCOUNT_HPP
#define BOOKSTORE_2023_ACCOUNT_HPP

#include <string>
#include <vector>
#include "Blocklist/blocklist.hpp"
#include "core.hpp"


namespace CrazyDave {

    /// todo 封装login_stack

    class AccountSystem;

    class Account;

    void change_all(std::string &old_ISBN, std::string &new_ISBN);

    bool check_privilege(int);

    Account &get_current_account();

    CrazyDave::String<21> &get_current_select();

    bool is_selected();

    class Account {
        friend AccountSystem;

        friend bool check_privilege(int);

    private:
        CrazyDave::String<31> user_id;
        CrazyDave::String<31> password;
        CrazyDave::String<31> user_name;
        int privilege;
    public:
        Account();

        Account(const char *_user_id, const char *_password, int _privilege, const char *_user_name);

//    Account(const Account &) = delete;
//
//    Account(const Account &&) = delete;
        bool operator<(const Account &rhs) const {
            return user_id < rhs.user_id;
        }

        bool operator==(const Account &rhs) const {
            return user_id == rhs.user_id;
        }

        bool check_password(const char *_password);

        void modify_password(const char *_password);
    };

    class AccountSystem : public System {
    private:
//        CrazyDave::BlockList<CrazyDave::String<31>, Account> blockList{"tmp/account_index", "tmp/account_data"};
        CrazyDave::BlockList<CrazyDave::String<31>, Account> blockList{"account_index", "account_data"};
    public:
        AccountSystem();

        bool login(const char *user_id, const char *password);

        bool logout();

        bool modify_password(const char *user_id, const char *cur_pw, const char *new_pw);

        bool user_add(const char *user_id, const char *password, int privilege, const char *user_name);

        bool register_account(const char *user_id, const char *password, const char *user_name);

        bool remove(const char *user_id);
    };


#endif //BOOKSTORE_2023_ACCOUNT_HPP

}

