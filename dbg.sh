set -x

# old and busted
#mkdir -p bins

#DBG_OPTS="-Wall --std=c++2b -lfmt -gdwarf -O0"
#ASAN_OPTS="-fsanitize=undefined -fsanitize=address -Wall --std=c++2b -lfmt -gdwarf -O0"
#OPTS="-Wall --std=c++2b -lfmt -g -O3"

#clang++ $OPTS -o bins/day1 day1.cpp
#clang++ $OPTS -o bins/day2 day2.cpp

## new hotness
cmake -B build -GNinja -DCMAKE_BUILD_TYPE=Debug
#cmake -B build -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
