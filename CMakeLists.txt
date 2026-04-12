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

FetchContent_Declare(
    isocline
    GIT_REPOSITORY https://github.com/daanx/isocline.git
    GIT_TAG        v1.0.9
)

FetchContent_MakeAvailable(isocline)

find_package(SQLite3 REQUIRED)
find_package(CURL REQUIRED HTTPS)

add_executable(aopc-cli)

target_sources(aopc-cli PRIVATE
    ${source_dir}/main.cpp
    ${source_dir}/core/commandHandler.cpp
    ${source_dir}/core/settings.cpp
    ${source_dir}/commands/helpCommand.cpp
    ${source_dir}/commands/exitCommand.cpp
    ${source_dir}/commands/priceCommand.cpp
    ${source_dir}/commands/configCommand.cpp
    ${source_dir}/io/argParser.cpp
    ${source_dir}/io/apiManager.cpp
    ${source_dir}/db/itemDatabase.cpp
    ${source_dir}/io/formatter.cpp
)

target_include_directories(aopc-cli PRIVATE
    ${include_dir}
)

add_custom_command(TARGET aopc-cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/database $<TARGET_FILE_DIR:aopc-cli>/database
    COMMENT "Copying database folder to build directory"
)

add_custom_command(TARGET aopc-cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/config $<TARGET_FILE_DIR:aopc-cli>/config
    COMMENT "Copying config folder to build directory"
)

target_link_libraries(aopc-cli PRIVATE nlohmann_json::nlohmann_json SQLite3::SQLite3 CURL::libcurl isocline)