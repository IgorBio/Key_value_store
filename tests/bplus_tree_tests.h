#include <gtest/gtest.h>

#include "../b_plus_tree/b_plus_tree.h"

using namespace s21;

TEST(BPlusTreeTest, Constructor) {
  const std::size_t degree = 3;
  BPlusTree tree(degree);

  EXPECT_EQ(tree.ShowAll().size(), 0u);
  EXPECT_TRUE(tree.Keys().empty());
}

TEST(BPlusTreeTest, Set) {
  BPlusTree tree(6);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  EXPECT_TRUE(tree.Set("key1", value1));
  EXPECT_TRUE(tree.Set("key2", value2));
  EXPECT_TRUE(tree.Set("key3", value3));
  EXPECT_FALSE(tree.Set("key1", value4));
}

TEST(BPlusTreeTest, Get) {
  BPlusTree tree(3);
  Value value("Vasilev", "Ivan", "2000", "Moscow", "55");
  tree.Set("key", value);

  auto result = tree.Get("key");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(value.ToString(), result.value().ToString());

  result = tree.Get("unknown_key");
  EXPECT_FALSE(result.has_value());
}

TEST(BPlusTreeTest, Exists) {
  BPlusTree tree(5);
  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);

  EXPECT_TRUE(tree.Exists("key1"));
  EXPECT_TRUE(tree.Exists("key2"));
  EXPECT_TRUE(tree.Exists("key3"));
  EXPECT_FALSE(tree.Exists("unknown_key"));
}

TEST(BPlusTreeTest, Del) {
  BPlusTree tree(30);
  tree.Show();
  tree.Set("key1", Value());
  tree.Set("key2", Value());
  tree.Set("key3", Value());
  tree.Set("key4", Value());
  tree.Set("key5", Value());
  tree.Set("key6", Value());
  tree.Set("key7", Value());
  tree.Set("key8", Value());
  tree.Set("key9", Value());
  tree.Set("key90", Value());
  tree.Set("key91", Value());
  tree.Set("key92", Value());
  tree.Set("key93", Value());
  tree.Set("key94", Value());
  tree.Set("key95", Value());
  tree.Set("key96", Value());
  tree.Set("key97", Value());
  EXPECT_TRUE(tree.Del("key3"));
  EXPECT_TRUE(tree.Del("key4"));
  EXPECT_TRUE(tree.Del("key90"));
  EXPECT_TRUE(tree.Del("key97"));
  EXPECT_TRUE(tree.Del("key96"));
  EXPECT_TRUE(tree.Del("key95"));

  EXPECT_FALSE(tree.Del("unknown_key"));

  EXPECT_FALSE(tree.Exists("key3"));
  EXPECT_EQ(tree.Get("key3"), std::nullopt);

  EXPECT_TRUE(tree.Exists("key1"));
  EXPECT_TRUE(tree.Exists("key9"));

  tree.ToDot("./graphviz");
  // tree.Show();
}

TEST(BPlusTreeTest, Update) {
  BPlusTree tree(12);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);
  EXPECT_TRUE(tree.Update("key1", "- - 1999 Msk 90"));
  EXPECT_TRUE(tree.Update("key2", "- - - Msk -"));
  EXPECT_TRUE(tree.Update("key3", "Ivanov Ivan 2000 Moscow 55"));
  EXPECT_TRUE(tree.Get("key1").value().Match("Ivanov Ivan 1999 Msk 90"));
  EXPECT_TRUE(tree.Get("key2").value().Match("Petrov Petr 1990 Msk 100"));
  EXPECT_TRUE(tree.Get("key3").value().Match("Ivanov Ivan 2000 Moscow 55"));
  EXPECT_FALSE(tree.Update("unknown_key", "- - - - -"));
}

TEST(BPlusTreeTest, Keys) {
  BPlusTree tree(9);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);
  tree.Set("key4", value4);
  EXPECT_EQ(tree.Keys(), std::vector<Key>({"key1", "key2", "key3", "key4"}));
}

TEST(BPlusTreeTest, Rename) {
  BPlusTree tree(10);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);
  tree.Set("key4", value4);
  EXPECT_TRUE(tree.Rename("key3", "key5"));
  EXPECT_FALSE(tree.Rename("key1", "key2"));
  EXPECT_EQ(tree.Get("key5").value().ToString(), value3.ToString());
  EXPECT_FALSE(tree.Get("key3"));
}

TEST(BPlusTreeTest, TTL) {
  BPlusTree tree(4);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55", "1");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100", "3");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);
  EXPECT_EQ(tree.TTL("key1"), 1u);
  EXPECT_EQ(tree.TTL("key2"), 3u);
  EXPECT_EQ(tree.TTL("key3"), std::nullopt);
  EXPECT_EQ(tree.TTL("key4"), std::nullopt);
}

TEST(BPlusTreeTest, Find) {
  BPlusTree tree(8);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");
  Value value5("Ivanov", "Petr", "2000", "Moscow", "55");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value1);
  tree.Set("key4", value3);
  tree.Set("key5", value4);
  tree.Set("key6", value1);
  tree.Set("key7", value5);
  EXPECT_EQ(tree.Find("Ivanov - 2000 Moscow 55"),
            std::vector<Key>({"key1", "key3", "key6", "key7"}));
}

TEST(BPlusTreeTest, ShowAll) {
  BPlusTree tree(7);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);
  tree.Set("key4", value4);
  EXPECT_EQ(tree.ShowAll(),
            std::vector<Value>({value1, value2, value3, value4}));
}

TEST(BPlusTreeTest, Export) {
  BPlusTree tree(5);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "Tver", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  tree.Set("key1", value1);
  tree.Set("key2", value2);
  tree.Set("key3", value3);
  tree.Set("key4", value4);
  EXPECT_EQ(tree.Export("./export.dat"), 4u);
}

TEST(BPlusTreeTest, Upload) {
  BPlusTree tree(5);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "Tver", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  EXPECT_EQ(tree.Upload("./export.dat"), 4u);
  EXPECT_EQ(tree.Get("key1").value().ToQuotedString(), value1.ToQuotedString());
  EXPECT_EQ(tree.Get("key2").value().ToQuotedString(), value2.ToQuotedString());
  EXPECT_EQ(tree.Get("key3").value().ToQuotedString(), value3.ToQuotedString());
  EXPECT_EQ(tree.Get("key4").value().ToQuotedString(), value4.ToQuotedString());
}
