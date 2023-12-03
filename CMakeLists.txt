cmake_minimum_required(VERSION 3.27)
project(aoc2023)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -gdwarf")

SET(CMAKE_CXX_STANDARD 23)
enable_testing()

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
