cmake_minimum_required(VERSION 3.6)
project(Lumberjack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Tree.cpp Tree.h Forest.cpp Forest.h)

set(SOURCE_ANNEALING solutions/lumberjack_annealing.cpp Solution.cpp Solution.h)
add_executable(judge_easy judges/judge.cpp oi.h ${SOURCE_FILES})
add_executable(solution_annealing ${SOURCE_FILES} ${SOURCE_ANNEALING})
add_executable(maciej_ant solutions/maciej_ant_cpu.cpp)
add_executable(krzysiek_ant solutions/krzysiek_ant_cpu.cpp)
add_executable(greedy_cpu solutions/greedy_cpu.cpp)
add_executable(annealing_2 solutions/annealing_2.cpp)