cmake_minimum_required(VERSION 3.22)
project(libtlv VERSION 0.0.1
        DESCRIPTION "Type Length Value protocol base on TCP"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

find_library(LIBEVENT2_CORE event_core)
find_library(cJSON cjson)

# The compiled library code is here
add_subdirectory(src)

add_subdirectory(tests)
