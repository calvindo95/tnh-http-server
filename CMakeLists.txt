cmake_minimum_required(VERSION 3.6)

# The project name
project(https)
set(TARGET "tnh-server")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER /usr/bin/gcc CACHE PATH "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_DYN_LINK -pthread -g" CACHE STRING "compile flags" FORCE)

find_package(GTest REQUIRED)
find_package(Boost COMPONENTS log log_setup REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(nlohmann_json 3.2.0 REQUIRED)

## add directories
add_subdirectory(src)
#add_subdirectory(test)