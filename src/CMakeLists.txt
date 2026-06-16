include_directories(../include)
include_directories(/usr/include/mariadb)

find_library(mariadb NAMES mariadb PATHS /usr/lib/aarch64-linux-gnu)

add_executable(${TARGET}
    main.cpp
    Config.cpp
    DBQuery.cpp
    DBQ.cpp
    Logging.cpp
    Resource.cpp
    Post_Json.cpp
    RegisterDevice.cpp
    GetLastEntry.cpp
    HTTPResources.cpp
    ListDevices.cpp
    GetHistory.cpp
    GetSingleData.cpp
    AuthResources.cpp
    AuthUser.cpp
    RegisterUser.cpp
    AuthServ.cpp
    AuthSession.cpp
)

target_link_libraries(${TARGET}
    httpserver
    microhttpd
    ${Boost_LIBRARIES}
    mariadb
    mariadbcpp
    ssl
    crypto
)

install(TARGETS ${TARGET} DESTINATION /usr/local/bin)
