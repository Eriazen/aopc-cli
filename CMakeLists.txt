cmake_minimum_required(VERSION 3.10.0)
project(aopc-cli VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(NATS_BUILD_EXAMPLES OFF CACHE BOOL "Disable NATS examples" FORCE)

FetchContent_Declare(
    nats_c
    GIT_REPOSITORY https://github.com/nats-io/nats.c.git
    GIT_TAG        v3.12.0
)

FetchContent_MakeAvailable(nats_c)

add_executable(aopc-cli)

target_sources(aopc-cli PRIVATE
    src/main.cpp
    src/io/cmdParser.cpp
)

target_include_directories(aopc-cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(aopc-cli PRIVATE nats)