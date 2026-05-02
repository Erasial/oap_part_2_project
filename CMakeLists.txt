cmake_minimum_required(VERSION 3.16)
project(MedLogApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGCRYPT REQUIRED libgcrypt)


file(GLOB_RECURSE CORE_SOURCES
    "model/*.cpp"
    "controller/*.cpp"
)

file(GLOB_RECURSE VIEW_SOURCES
    "view/*.cpp"
)

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

target_link_libraries(core PRIVATE
    Qt6::Core
    Qt6::Sql
    ${LIBGCRYPT_LIBRARIES}
)

target_include_directories(core PRIVATE ${LIBGCRYPT_INCLUDE_DIRS})
target_compile_options(core PRIVATE ${LIBGCRYPT_CFLAGS_OTHER})

add_executable(main
    ${VIEW_SOURCES}
    main.cpp
)

target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/view
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

target_link_libraries(main PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    core
)

enable_testing()

add_executable(tests
    tests/RepositoryTests.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

target_compile_definitions(tests PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(tests PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Test
    core
)

add_test(NAME MedLogAppTests COMMAND tests)
