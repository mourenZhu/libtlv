cmake_minimum_required(VERSION 3.22)
project(simple_server_client VERSION 0.0.1
        DESCRIPTION "tlvlib example simple server client"
        LANGUAGES C)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_library(LIBEVENT2_CORE event_core)
find_library(cJSON cjson)
find_library(tlv_core tlv_core)
find_library(tlv_event tlv_event)
find_library(tlv_server tlv_server)
find_library(tlv_client tlv_client)

add_executable(client client.c)
add_executable(server server.c)

target_link_libraries(client PRIVATE ${tlv_client} ${tlv_event} ${tlv_core} ${LIBEVENT2_CORE} ${cJSON})
target_link_libraries(server PRIVATE ${tlv_server} ${tlv_event} ${tlv_core} ${LIBEVENT2_CORE} ${cJSON})

add_executable(client_v6 client_v6.c)
add_executable(server_v6 server_v6.c)

target_link_libraries(client_v6 PRIVATE ${tlv_client} ${tlv_event} ${tlv_core} ${LIBEVENT2_CORE} ${cJSON})
target_link_libraries(server_v6 PRIVATE ${tlv_server} ${tlv_event} ${tlv_core} ${LIBEVENT2_CORE} ${cJSON})
