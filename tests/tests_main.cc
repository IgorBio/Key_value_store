#include <gtest/gtest.h>

#include "bplus_node_tests.h"
#include "bplus_tree_tests.h"
#include "hash_table_tests.h"
#include "tests_self_balancing_binary_search_tree.h"
#include "value_tests.h"

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
