include_directories(${PROJECT_SOURCE_DIR}/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/tests)


find_library(LIBEVENT2_CORE event_core)
#find_library(cJSON cjson)
find_library(tlv_core tlv_core)
#find_library(tlvserver tlvserver)
#find_library(tlvclient tlvclient)
find_library(tlv_event tlv_event)


#add_executable(uthash_test uthash_test.c)
#add_executable(tlvevent_tlvhandlerhash_test tlvevent_tlvhandlerhash_test.c)
#
#target_link_libraries(tlvevent_tlvhandlerhash_test PRIVATE ${tlv_event} ${tlv_core} ${LIBEVENT2_CORE})


