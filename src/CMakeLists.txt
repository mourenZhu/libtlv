# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

include_directories(${PROJECT_SOURCE_DIR}/include)

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB TLV_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/tlv/*.h")

#find_library(LIBEVENT2_CORE event_core)

# Make an automatic library - will be static or dynamic based on user setting
add_library(tlv_core STATIC tlv.c log.c ${TLV_HEADER_LIST})
#target_include_directories(tlv_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(tlv_event STATIC tlvevent.c log.c ${TLV_HEADER_LIST})
#target_include_directories(tlv_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(tlv_server STATIC tlvserver.c log.c  ${TLVJSON_HEADER_LIST})
#target_include_directories(tlv_server PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(tlv_client STATIC tlvclient.c log.c ${TLVJSON_HEADER_LIST})
#target_include_directories(tlv_client PUBLIC ${PROJECT_SOURCE_DIR}/include)


install(FILES ${TLV_HEADER_LIST} DESTINATION include/tlv)
#install(FILES "${PROJECT_SOURCE_DIR}/include/tlv/tlvevent.h" DESTINATION include/tlv)
#install(FILES "${PROJECT_SOURCE_DIR}/include/tlv/tlvserver.h" DESTINATION include/tlv)
#install(FILES "${PROJECT_SOURCE_DIR}/include/tlv/tlvclient.h" DESTINATION include/tlv)

install(TARGETS tlv_core tlv_event tlv_server tlv_client LIBRARY DESTINATION lib)

