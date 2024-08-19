cmake_minimum_required(VERSION 3.22)
project(ssl_demo VERSION 0.0.1
        DESCRIPTION "ssl_demo"
        LANGUAGES C)

SET(EXECUTABLE_OUTPUT_PATH ../../bin)

find_library(LIBEVENT2_CORE event_core)
find_library(LIBEVENT2_OPENSSL event_openssl)
find_package(OpenSSL REQUIRED)


add_executable(libevent_ssl_echo_server libevent_ssl_echo_server.c)
target_link_libraries(libevent_ssl_echo_server PRIVATE ${LIBEVENT2_CORE} ${LIBEVENT2_OPENSSL} OpenSSL::SSL OpenSSL::Crypto)

add_executable(libevent_ssl_echo_client libevent_ssl_echo_client.c)
target_link_libraries(libevent_ssl_echo_client PRIVATE ${LIBEVENT2_CORE} ${LIBEVENT2_OPENSSL} OpenSSL::SSL OpenSSL::Crypto)
