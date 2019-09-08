project("lua")
cmake_minimum_required(VERSION 2.8)

file(GLOB LUA_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.c
                   ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

add_library(lua ${LUA_SRCS})

target_include_directories(lua PUBLIC
                           ${CMAKE_CURRENT_LIST_DIR}/src)
