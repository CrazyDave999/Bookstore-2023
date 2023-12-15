cmake_minimum_required(VERSION 3.22)
project(Bookstore_2023)

set(CMAKE_CXX_STANDARD 17)

add_executable(Bookstore_2023 main.cpp account.cpp book.cpp utils.cpp log.cpp core.cpp)
