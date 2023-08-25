include_directories(../include)
include_directories(/usr/include/mariadb)
find_library(mariadb NAMES mariadb PATHS /usr/lib/arm-linux-gnueabihf)

add_library(tsqueue STATIC TSQueue.cpp)

add_library(config STATIC Config.cpp)

add_library(dbquery STATIC DBQuery.cpp)
target_link_libraries(dbquery mariadb)

add_library(logging STATIC Logging.cpp)
target_link_libraries(logging ${Boost_LIBRARIES})

add_library(performance STATIC Performance.cpp)
target_link_libraries(performance ${Boost_LIBRARIES} logging)

add_library(resource STATIC Resource.cpp)
target_link_libraries(resource tsqueue logging dbquery)

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} httpserver microhttpd config performance resource)