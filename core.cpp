//
// Created by Kingsly on 2023-12-14.
//
#include "core.hpp"

CrazyDave::Core::Core(AccountSystem *_a_sys, BookSystem *_b_sys, LogSystem *_l_sys) : a_sys(_a_sys), b_sys(_b_sys),
                                                                                      l_sys(_l_sys) {}


void CrazyDave::System::load_core(CrazyDave::Core *_core) {
    core = _core;
}
