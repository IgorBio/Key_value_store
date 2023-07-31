include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        58d77fa8070e8cec2dc1ed015d66b454c8d78850 # release-1.12.1
)

FetchContent_MakeAvailable(googletest)

target_compile_options(gtest PRIVATE "-w")
target_compile_options(gmock PRIVATE "-w") 

add_executable(
    tests_transactions
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}/avl_tree/self_balancing_binary_search_tree.cc
    ${CMAKE_SOURCE_DIR}/b_plus_tree/b_plus_tree.cc
    ${CMAKE_SOURCE_DIR}/b_plus_tree/b_plus_node.cc
    ${CMAKE_SOURCE_DIR}/hash_table/hash_table.cc
    ${CMAKE_SOURCE_DIR}/tests/bplus_tree_tests.h
    ${CMAKE_SOURCE_DIR}/tests/bplus_node_tests.h
    ${CMAKE_SOURCE_DIR}/tests/hash_table_tests.h
    ${CMAKE_SOURCE_DIR}/tests/tests_main.cc
    ${CMAKE_SOURCE_DIR}/tests/value_tests.h
    ${CMAKE_SOURCE_DIR}/common/value.cc
)

target_link_libraries(tests_transactions PUBLIC gtest)

target_include_directories(tests_transactions 
  PUBLIC 
  ${CMAKE_SOURCE_DIR}/avl_tree
  ${CMAKE_SOURCE_DIR}/console
  ${CMAKE_SOURCE_DIR}/hash_table
  ${CMAKE_SOURCE_DIR}/b_plus_tree
  ${CMAKE_SOURCE_DIR}/common
)

target_compile_options(
    tests_transactions
    PUBLIC
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    -std=c++17
)