cmake_minimum_required(VERSION 3.15)

project(Transactions VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/avl_tree
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/hash_table
    ${CMAKE_SOURCE_DIR}/b_plus_tree
    ${CMAKE_SOURCE_DIR}/console
    ${CMAKE_SOURCE_DIR}/common
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/avl_tree/self_balancing_binary_search_tree.h
    ${CMAKE_SOURCE_DIR}/common/abstract_store.h
    ${CMAKE_SOURCE_DIR}/common/value.h
    ${CMAKE_SOURCE_DIR}/hash_table/hash_table.h
    ${CMAKE_SOURCE_DIR}/b_plus_tree/b_plus_tree.h
    ${CMAKE_SOURCE_DIR}/b_plus_tree/b_plus_node.h
    ${CMAKE_SOURCE_DIR}/console/console.h
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/console/console.cc
    ${CMAKE_SOURCE_DIR}/avl_tree/self_balancing_binary_search_tree.cc
    ${CMAKE_SOURCE_DIR}/hash_table/hash_table.cc
    ${CMAKE_SOURCE_DIR}/b_plus_tree/b_plus_tree.cc
    ${CMAKE_SOURCE_DIR}/b_plus_tree/b_plus_node.cc
    ${CMAKE_SOURCE_DIR}/common/value.cc
)

add_executable(
    ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${CMAKE_SOURCE_DIR}/main.cc
)

add_subdirectory(tests)

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    -lpthread
    -std=c++17
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    -fsanitize=address
)

find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    message(STATUS "cppcheck found: ${CPPCHECK}")
    list(
        APPEND CPPCHECK_ARGS
        "--enable=all"
        "--inconclusive"
        "--quiet"
        "--language=c++"
        "--std=c++17"
        "--suppress=missingInclude"
        "--suppress=unusedFunction"
        "--suppress=functionStatic"
        "--suppress=knownConditionTrueFalse"
    )
    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK} ${CPPCHECK_ARGS} ${HEADERS} ${SOURCES}
    )
else()
    message(STATUS "cppcheck not found")
endif()
