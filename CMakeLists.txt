cmake_minimum_required(VERSION 3.16)
project(MedLogApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

file(GLOB_RECURSE SOURCES
    "model/*.cpp"
    "view/*.cpp"
    "controller/*.cpp"
    "main.cpp"
)

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/view
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

target_link_libraries(main PRIVATE
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Sql
)
