cmake_minimum_required(VERSION 3.10.0)
project(aopc-cli VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wconversion -Wsign-conversion -pedantic")

set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
)

FetchContent_MakeAvailable(json)

find_package(SQLite3 REQUIRED)

add_executable(aopc-cli)

target_sources(aopc-cli PRIVATE
    ${source_dir}/main.cpp
    ${source_dir}/core/commandHandler.cpp
    ${source_dir}/commands/helpCommand.cpp
    ${source_dir}/commands/exitCommand.cpp
    ${source_dir}/commands/invalidCommand.cpp
    ${source_dir}/commands/priceCommand.cpp
    ${source_dir}/io/argParser.cpp
    ${source_dir}/db/itemDatabase.cpp
)

target_include_directories(aopc-cli PRIVATE
    ${include_dir}
)

target_link_libraries(aopc-cli PRIVATE nlohmann_json::nlohmann_json SQLite3::SQLite3)