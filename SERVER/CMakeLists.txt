cmake_minimum_required(VERSION 3.26)
project(NidhoggRemoteKeyboard C)

set(CMAKE_C_STANDARD 11)

add_executable(NidhoggRemoteKeyboard main.c)

target_link_libraries(NidhoggRemoteKeyboard ws2_32.lib)
