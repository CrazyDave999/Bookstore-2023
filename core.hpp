//
// Created by Kingsly on 2023-12-14.
//

#ifndef BOOKSTORE_2023_CORE_HPP
#define BOOKSTORE_2023_CORE_HPP

namespace CrazyDave {
    class AccountSystem;

    class BookSystem;

    class LogSystem;

    class Core {
        friend AccountSystem;
        friend BookSystem;
        friend LogSystem;
    private:
        AccountSystem *a_sys;
        BookSystem *b_sys;
        LogSystem *l_sys;
    public:
        Core(AccountSystem *_a_sys, BookSystem *_b_sys, LogSystem *_l_sys);
    };

    class System {
    protected:
        Core *core;
    public:
        void load_core(Core *_core);
    };
}
#endif //BOOKSTORE_2023_CORE_HPP
