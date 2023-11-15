cmake_minimum_required(VERSION 3.26)
project(UniSearch)

find_package(SQLite3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lsqlite3")

add_executable(UniSearch main.cpp
        src/auth.cpp
        src/trie.cpp
        src/dashboard.cpp
        include/dashboard.h
)
