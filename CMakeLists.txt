# The first line states the minimum CMake version to use with this project
# and is requires by CMake.
cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 11)

# Set <PROJNAME> to be your project name. We set CXX (C++) here as the target language.
project(DS_Wet_1 VERSION 0.1.0 LANGUAGES CXX)

# If you want to see "under the hood" of CMake, uncomment this:
set(CMAKE_VERBOSE_MAKEFILE ON)

# Define variables containing compiler flags for gcc
set(DS_FLAGS_DEBUG "-std=c++11 -pedantic-errors -Wall -Werror")
set(DS_FLAGS_RELEASE "${DS_FLAGS_DEBUG} -DNDEBUG")

# Set the flags for gcc
set(CMAKE_CPP_FLAGS ${DS_FLAGS_DEBUG})


# Add the names of your source files here
set(CPP_FILES ./main1.cpp ./library1.cpp ./PlayersManager.cpp ./Group.cpp ./Player.cpp)
# ====== sortedList ======= set(CPP_FILES ./part1/examDetails.cpp ./part1/main.cpp)

add_executable(DS_Wet_1_Staff_Test ${CPP_FILES})