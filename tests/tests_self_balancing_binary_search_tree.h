#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <thread>

#include "../avl_tree/self_balancing_binary_search_tree.h"

using namespace s21;

Value val("Pupkin", "Vasya", "1992", "Moskow", "23");

std::vector<std::string> vec_check_val = {
    "Vasiliev Ivan 2001 Rostov 55",       "Ivanov Vasiliy 2000 Moscow 55",
    "Petrov Petr 1990 Tver 100",          "Ivanov Ivan 2000 Moscow 55",
    "Sidorov Sergei 1980 Novosibirsk 50", "Vasilev Vasiliy 2000 Moscow 150",
    "Urantaev Dima 1993 Nsk 1234",        "Panov Savelii 1998 Barnaul 1",
    "Pechenin Maxim 1990 Ekb 999"};

void InsertNodes(SelfBalancingBinarySearchTree& avl_tree) {
  avl_tree.Set("F", val);
  avl_tree.Set("B", val);
  avl_tree.Set("G", val);
  avl_tree.Set("A", val);
  avl_tree.Set("D", val);
  avl_tree.Set("I", val);
  avl_tree.Set("C", val);
  avl_tree.Set("E", val);
  avl_tree.Set("H", val);
}

TEST(AVLTreeTest, Set1) {
  SelfBalancingBinarySearchTree avl_tree;
  EXPECT_TRUE(avl_tree.Set("10", val));
}

TEST(AVLTreeTest, Set2) {
  SelfBalancingBinarySearchTree avl_tree;
  EXPECT_TRUE(avl_tree.Set("10", val));
  EXPECT_TRUE(avl_tree.Set("5", val));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, SetFourElements) {
  SelfBalancingBinarySearchTree avl_tree;
  EXPECT_TRUE(avl_tree.Set("g", val));
  EXPECT_TRUE(avl_tree.Set("d", val));
  EXPECT_TRUE(avl_tree.Set("x", val));
  EXPECT_TRUE(avl_tree.Set("a", val));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, CheckBalanceAfterSet1) {
  SelfBalancingBinarySearchTree avl_tree;
  EXPECT_TRUE(avl_tree.Set("g", val));
  EXPECT_TRUE(avl_tree.Set("d", val));
  EXPECT_TRUE(avl_tree.Set("x", val));
  EXPECT_TRUE(avl_tree.Set("b", val));
  EXPECT_TRUE(avl_tree.Set("y", val));
  EXPECT_TRUE(avl_tree.Set("z", val));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
  EXPECT_FALSE(avl_tree.Exists("a"));
  EXPECT_TRUE(avl_tree.Exists("x"));
  EXPECT_TRUE(avl_tree.Exists("g"));
}

TEST(AVLTreeTest, CheckBalanceAfterSet2) {
  SelfBalancingBinarySearchTree avl_tree;
  EXPECT_TRUE(avl_tree.Set("g", val));
  EXPECT_FALSE(avl_tree.Set("g", val));
  EXPECT_TRUE(avl_tree.Set("d", val));
  EXPECT_TRUE(avl_tree.Set("x", val));
  EXPECT_TRUE(avl_tree.Set("b", val));
  EXPECT_TRUE(avl_tree.Set("y", val));
  EXPECT_TRUE(avl_tree.Set("z", val));
  EXPECT_TRUE(avl_tree.Set("l", val));
  EXPECT_TRUE(avl_tree.Set("a", val));
  EXPECT_TRUE(avl_tree.Set("99", val));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
  EXPECT_TRUE(avl_tree.Exists("a"));
  EXPECT_TRUE(avl_tree.Exists("x"));
  EXPECT_TRUE(avl_tree.Exists("g"));
  EXPECT_FALSE(avl_tree.Exists("k"));
}

TEST(AVLTreeTest, LoopSet) {
  SelfBalancingBinarySearchTree avl_tree;
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(avl_tree.Set(std::to_string(i), val));
    EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
  }
  avl_tree.MakeDotFile("tree_100_elements");
}

TEST(AVLTreeTest, DeleteLeafNode) {
  SelfBalancingBinarySearchTree avl_tree;
  InsertNodes(avl_tree);

  // Deleting a node without children (leaf node)
  EXPECT_TRUE(avl_tree.Del("A"));
  EXPECT_FALSE(avl_tree.Exists("A"));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, DeleteNodeWithOneChild) {
  SelfBalancingBinarySearchTree avl_tree;
  InsertNodes(avl_tree);

  // Deleting a node with one child
  EXPECT_TRUE(avl_tree.Del("D"));
  EXPECT_FALSE(avl_tree.Exists("D"));
  EXPECT_TRUE(avl_tree.Exists("C"));
  EXPECT_TRUE(avl_tree.Exists("E"));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, DeleteNodeWithTwoChildren) {
  SelfBalancingBinarySearchTree avl_tree;
  InsertNodes(avl_tree);

  // Deleting a node with two children
  EXPECT_TRUE(avl_tree.Del("B"));
  EXPECT_FALSE(avl_tree.Exists("B"));
  EXPECT_TRUE(avl_tree.Exists("A"));
  EXPECT_TRUE(avl_tree.Exists("C"));
  EXPECT_TRUE(avl_tree.Exists("D"));
  EXPECT_TRUE(avl_tree.Exists("E"));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, DeleteRootWithOneChild) {
  SelfBalancingBinarySearchTree avl_tree;
  EXPECT_TRUE(avl_tree.Set("A", val));
  EXPECT_TRUE(avl_tree.Set("Z", val));
  // Deleting root with one child
  EXPECT_TRUE(avl_tree.Del("A"));
  EXPECT_FALSE(avl_tree.Exists("A"));
}

TEST(AVLTreeTest, DeleteRootWithTwoChildren) {
  SelfBalancingBinarySearchTree avl_tree;
  InsertNodes(avl_tree);

  // Deleting root with two children
  EXPECT_TRUE(avl_tree.Del("F"));
  EXPECT_FALSE(avl_tree.Exists("F"));
  EXPECT_TRUE(avl_tree.Exists("G"));
  EXPECT_TRUE(avl_tree.Exists("B"));
  EXPECT_TRUE(avl_tree.Exists("A"));
  EXPECT_TRUE(avl_tree.Exists("D"));
  EXPECT_TRUE(avl_tree.Exists("I"));
  EXPECT_TRUE(avl_tree.Exists("C"));
  EXPECT_TRUE(avl_tree.Exists("E"));
  EXPECT_TRUE(avl_tree.Exists("H"));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, DeleteNonexistentNode) {
  SelfBalancingBinarySearchTree avl_tree;
  InsertNodes(avl_tree);

  // Try deleting a non-existent node
  EXPECT_FALSE(avl_tree.Del("Z"));
  EXPECT_FALSE(avl_tree.Exists("Z"));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, DeleteLoop) {
  SelfBalancingBinarySearchTree avl_tree;
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(avl_tree.Set(std::to_string(i), val));
    EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
  }
  for (int i = 0; i < 100; i += 2) {
    EXPECT_TRUE(avl_tree.Del(std::to_string(i)));
    EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
  }
  avl_tree.MakeDotFile("tree_with_delet_node");
  // Try deleting a non-existent node
  EXPECT_FALSE(avl_tree.Del("Z"));
  EXPECT_FALSE(avl_tree.Exists("Z"));
  EXPECT_TRUE(std::abs(avl_tree.GetBalance(avl_tree.GetRootKey())) <= 1);
}

TEST(AVLTreeTest, GetValue) {
  SelfBalancingBinarySearchTree avl_tree;
  InsertNodes(avl_tree);
  EXPECT_TRUE(avl_tree.Get("A").value() ==
              Value("Pupkin", "Vasya", "1992", "Moskow", "23"));
  EXPECT_FALSE(avl_tree.Get("A").value() ==
               Value("Pupkin", "Petr", "2000", "Nsk", "23"));
}

TEST(AVLTreeTest, Export) {
  SelfBalancingBinarySearchTree avl_tree;
  int i = 1;
  for (const auto& val : vec_check_val) {
    avl_tree.Set(std::to_string(i++), Value::FromString(val));
  }
  avl_tree.Export("data_for_test.dat");
}

TEST(AVLTreeTest, TestKeys) {
  SelfBalancingBinarySearchTree avl_tree;
  auto check_size = avl_tree.Upload("data_for_test.dat");
  EXPECT_EQ(check_size, 9);
  auto vec_key = avl_tree.Keys();
  EXPECT_EQ(vec_key.size(), 9);
  for (auto i = 1u; i <= vec_key.size(); ++i) {
    EXPECT_EQ(std::to_string(i), vec_key[i - 1]);
  }
}

TEST(AVLTreeTest, TestShowAll) {
  SelfBalancingBinarySearchTree avl_tree;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St.Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  EXPECT_TRUE(avl_tree.Set("key1", value1));
  EXPECT_TRUE(avl_tree.Set("key2", value2));
  EXPECT_TRUE(avl_tree.Set("key3", value3));
  EXPECT_TRUE(avl_tree.Set("key4", value4));
  EXPECT_EQ(avl_tree.ShowAll(),
            std::vector<Value>({value1, value2, value3, value4}));
}

TEST(AVLTreeTest, UpdateTest) {
  SelfBalancingBinarySearchTree avl_tree;
  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St.Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  EXPECT_TRUE(avl_tree.Set("key1", value1));
  EXPECT_TRUE(avl_tree.Set("key2", value2));
  EXPECT_TRUE(avl_tree.Set("key3", value3));
  EXPECT_TRUE(avl_tree.Set("key4", value4));

  EXPECT_TRUE(avl_tree.Update("key1", "Pupkin Vasya 1992 Moskow 23"));

  EXPECT_TRUE(avl_tree.Update("key2", "Panov Sava - Nsk -"));

  EXPECT_TRUE(avl_tree.Get("key1").value() ==
              Value("Pupkin", "Vasya", "1992", "Moskow", "23"));

  EXPECT_TRUE(avl_tree.Get("key2").value() ==
              Value("Panov", "Sava", "1990", "Nsk", "100"));
}

TEST(AVLTreeTest, UpdateTest2) {
  SelfBalancingBinarySearchTree avl_tree;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  EXPECT_TRUE(avl_tree.Set("key1", value1));
  EXPECT_TRUE(avl_tree.Set("key2", value2));
  EXPECT_TRUE(avl_tree.Set("key3", value3));
  EXPECT_TRUE(avl_tree.Update("key1", "- - 1999 Msk 90"));
  EXPECT_TRUE(avl_tree.Update("key2", "- - - Msk -"));
  EXPECT_TRUE(avl_tree.Update("key3", "Ivanov Ivan 2000 Moscow 55"));
  EXPECT_TRUE(avl_tree.Get("key1").value().Match("Ivanov Ivan 1999 Msk 90"));
  EXPECT_TRUE(avl_tree.Get("key2").value().Match("Petrov Petr 1990 Msk 100"));
  EXPECT_TRUE(avl_tree.Get("key3").value().Match("Ivanov Ivan 2000 Moscow 55"));
  EXPECT_FALSE(avl_tree.Update("unknown_key", "- - - - -"));
}

TEST(AVLTreeTest, RenameTest) {
  SelfBalancingBinarySearchTree avl_tree;
  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  EXPECT_TRUE(avl_tree.Set("key1", value1));
  EXPECT_TRUE(avl_tree.Set("key2", value2));
  EXPECT_TRUE(avl_tree.Set("key3", value3));

  EXPECT_TRUE(avl_tree.Rename("key1", "key99"));
  EXPECT_TRUE(avl_tree.Get("key99").value() == value1);

  EXPECT_TRUE(avl_tree.Rename("key3", "asdads"));
  EXPECT_TRUE(avl_tree.Get("asdads").value() == value3);

  EXPECT_FALSE(avl_tree.Rename("unknown_key", "unknown_key"));
}

TEST(AVLTreeTest, ExportTest) {
  SelfBalancingBinarySearchTree avl_tree;
  auto lines_upload = avl_tree.Upload("data_for_test.dat");
  auto lines_export = avl_tree.Export("check.dat");

  EXPECT_EQ(lines_upload, lines_export);

  auto vec_key = avl_tree.Keys();
  auto vec_value = avl_tree.ShowAll();

  std::vector<Key> vec_check_key = {"1", "2", "3", "4", "5",
                                    "6", "7", "8", "9"};

  for (auto i = 0u; i < vec_value.size(); ++i) {
    EXPECT_EQ(vec_check_val[i], vec_value[i].ToString());
    EXPECT_EQ(vec_check_key[i], vec_key[i]);
  }
}

TEST(AVLTreeTest, FindTest) {
  SelfBalancingBinarySearchTree avl_tree;
  avl_tree.Upload("data_for_test.dat");
  auto vec_key = avl_tree.Keys();
  auto find_key = avl_tree.Find("Ivanov - 2000 - 55");
  EXPECT_EQ(find_key[0], vec_key[1]);
  EXPECT_EQ(find_key[1], vec_key[3]);
}

TEST(AVLTreeTest, FindTest2) {
  SelfBalancingBinarySearchTree avl_tree;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");
  Value value5("Ivanov", "Petr", "2000", "Moscow", "55");

  avl_tree.Set("key1", value1);
  avl_tree.Set("key2", value2);
  avl_tree.Set("key3", value1);
  avl_tree.Set("key4", value3);
  avl_tree.Set("key5", value4);
  avl_tree.Set("key6", value1);
  avl_tree.Set("key7", value5);
  EXPECT_EQ(avl_tree.Find("Ivanov - 2000 Moscow 55"),
            std::vector<Key>({"key1", "key3", "key6", "key7"}));
}

TEST(AVLTreeTest, TTLTestExist) {
  SelfBalancingBinarySearchTree avl_tree;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55", "1");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100", "40");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "123");

  EXPECT_TRUE(avl_tree.Set("key1", value1));
  EXPECT_TRUE(avl_tree.Set("key2", value2));
  EXPECT_TRUE(avl_tree.Set("key3", value3));
  EXPECT_EQ(avl_tree.TTL("key1"), 1u);
  EXPECT_EQ(avl_tree.TTL("key2"), 40u);
  EXPECT_EQ(avl_tree.TTL("key3"), std::nullopt);
}

TEST(AVLTreeTest, TTLTestEnd) {
  SelfBalancingBinarySearchTree avl_tree;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55", "1");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100", "2");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "3", "4");

  EXPECT_TRUE(avl_tree.Set("key1", value1));
  EXPECT_TRUE(avl_tree.Set("key2", value2));
  EXPECT_TRUE(avl_tree.Set("key3", value3));
  std::this_thread::sleep_for(std::chrono::seconds(5));
  avl_tree.DeleteExpiredElements();
  EXPECT_FALSE(avl_tree.Exists("key1"));
  EXPECT_FALSE(avl_tree.Exists("key2"));
  EXPECT_FALSE(avl_tree.Exists("key3"));
}
