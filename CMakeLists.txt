cmake_minimum_required(VERSION 3.10)

project(CloudStorage)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

include_directories(
    Crow/include
    include
    /usr/include/mysql
)

add_executable(
    cloud
    src/main.cpp
)

target_link_libraries(
    cloud
    mysqlclient
)
