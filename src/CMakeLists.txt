include_directories(../include)
include_directories(/usr/include/mariadb)
find_library(mariadb NAMES mariadb PATHS /usr/lib/arm-linux-gnueabihf)

add_library(config STATIC Config.cpp)
target_link_libraries(config logging)

add_library(dbquery STATIC DBQuery.cpp)
target_link_libraries(dbquery mariadb)

add_library(logging STATIC Logging.cpp)
target_link_libraries(logging ${Boost_LIBRARIES})

add_library(resource STATIC Resource.cpp)
target_link_libraries(resource logging dbquery)

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} httpserver microhttpd config resource)

install(TARGETS ${TARGET})