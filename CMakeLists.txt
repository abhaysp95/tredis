cmake_minimum_required(VERSION 3.13)

project(redis-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

find_package(spdlog CONFIG REQUIRED)

    # Or use the header-only version
    # find_package(spdlog CONFIG REQUIRED)
    # target_link_libraries(main PRIVATE spdlog::spdlog_header_only)

add_executable(server ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(server PRIVATE asio asio::asio)
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE spdlog::spdlog)
