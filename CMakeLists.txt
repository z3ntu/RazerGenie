cmake_minimum_required(VERSION 2.6)
project(kcm_razerdrivers)

add_executable(kcm_razerdrivers main.cpp)

install(TARGETS kcm_razerdrivers RUNTIME DESTINATION bin)
