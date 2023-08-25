cmake_minimum_required(VERSION 3.6)

# The project name
project(https)
set(TARGET "server")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER /usr/bin/gcc CACHE PATH "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_DYN_LINK -pthread -g" CACHE STRING "compile flags" FORCE)
#add_compile_options(-pthread -g)


find_package(GTest REQUIRED)
find_package(Boost COMPONENTS log log_setup REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

#find_path(MariaDBClient_INCLUDE_DIR
#        NAMES mysql.h
#        PATH_SUFFIXES mariadb mysql
#        )
#
## library
#set(BAK_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
#set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX})
#find_library(MariaDBClient_LIBRARY
#        NAMES mariadb libmariadb mariadbclient libmariadbclient mysqlclient libmysqlclient
#        PATH_SUFFIXES mariadb mysql
#        )
#set(CMAKE_FIND_LIBRARY_SUFFIXES ${BAK_CMAKE_FIND_LIBRARY_SUFFIXES})
#
#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(MariaDBClient DEFAULT_MSG MariaDBClient_LIBRARY MariaDBClient_INCLUDE_DIR)
#
#if(MariaDBClient_FOUND)
#    if(NOT TARGET MariaDBClient::MariaDBClient)
#        add_library(MariaDBClient::MariaDBClient UNKNOWN IMPORTED)
#        set_target_properties(MariaDBClient::MariaDBClient PROPERTIES
#            INTERFACE_INCLUDE_DIRECTORIES "${MariaDBClient_INCLUDE_DIR}"
#            IMPORTED_LOCATION "${MariaDBClient_LIBRARY}")
#    endif()
#endif()
#
#mark_as_advanced(MariaDBClient_INCLUDE_DIR MariaDBClient_LIBRARY)
#
#set(MariaDBClient_LIBRARIES ${MariaDBClient_LIBRARY})
#set(MariaDBClient_INCLUDE_DIRS ${MariaDBClient_INCLUDE_DIR})

## add directories
add_subdirectory(src)
#add_subdirectory(test)