cmake_minimum_required(VERSION 3.27)
project(aoc2023)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -gdwarf")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")

SET(CMAKE_CXX_STANDARD 23)
enable_testing()
find_package(mimalloc 2.1 REQUIRED)
find_package(benchmark REQUIRED)

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE include/)

add_executable(day1 src/day1.cpp)
target_link_libraries(day1 fmt)
# add_test(NAME day1 COMMAND day1 WORKING_DIRECTORY ..)

add_executable(day2 src/day2.cpp)
target_link_libraries(day2 utils fmt)
# add_test(NAME day2 COMMAND day2 WORKING_DIRECTORY ..)

add_executable(day3 src/day3.cpp)
target_link_libraries(day3 utils fmt)

add_executable(day4 src/day4.cpp)
target_link_libraries(day4 utils fmt mimalloc-static)

add_executable(bench4 src/bench_day4.cpp)
target_link_libraries(bench4 utils fmt benchmark::benchmark mimalloc-static)

add_executable(day5 src/day5.cpp)
target_link_libraries(day5 utils fmt)

add_executable(day6 src/day6.cpp)
target_link_libraries(day6 utils fmt)

add_executable(day7 src/day7.cpp)
target_link_libraries(day7 utils fmt)

add_executable(day8 src/day8.cpp)
target_link_libraries(day8 utils fmt)

add_executable(day9 src/day9.cpp)
target_link_libraries(day9 utils fmt)

add_executable(day10 src/day10.cpp)
target_link_libraries(day10 utils fmt)

add_executable(day11 src/day11.cpp)
target_link_libraries(day11 utils fmt)
