set -x
mkdir -p bins

DBG_OPTS="-Wall --std=c++2b -lfmt -gdwarf -O0"
ASAN_OPTS="-fsanitize=undefined -fsanitize=address -Wall --std=c++2b -lfmt -gdwarf -O0"
OPTS="-Wall --std=c++2b -lfmt -g -O3"

clang++ $OPTS -o bins/day1 day1.cpp
#clang++ $OPTS -o bins/day2 day2.cpp
#clang++ $OPTS -o bins/day3 day3.cpp
#clang++ $OPTS -o bins/day4 day4.cpp
#clang++ $OPTS -o bins/day5 day5.cpp
#clang++ $OPTS -o bins/day6 day6.cpp
#clang++ $OPTS -o bins/day7 day7.cpp
#clang++ $OPTS -o bins/day8 -lboost_system day8.cpp
#clang++ $OPTS -o bins/day9 -lboost_system day9.cpp
#clang++ $OPTS -o bins/day10 day10.cpp
#clang++ $OPTS -o bins/day11 day11.cpp
#clang++ $OPTS -o bins/day12 day12.cpp
#clang++ $OPTS -o bins/day13 day13.cpp
#clang++ $OPTS -o bins/day14 day14.cpp
#clang++ $OPTS -o bins/day15 day15.cpp
#clang++ $OPTS -o bins/day16 day16.cpp
#clang++ $OPTS -o bins/day17 day17.cpp
#clang++ $OPTS -o bins/day18 day18.cpp
#clang++ $OPTS -o bins/day19 day19.cpp
