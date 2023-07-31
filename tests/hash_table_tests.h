#include <gtest/gtest.h>

#include "../hash_table/hash_table.h"

using namespace s21;

TEST(HashTableTest, Set) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  EXPECT_TRUE(table.Set("key1", value1));
  EXPECT_TRUE(table.Set("key2", value2));
  EXPECT_TRUE(table.Set("key3", value3));
  EXPECT_FALSE(table.Set("key1", value4));
}

TEST(HashTableTest, Get) {
  HashTable table;
  Value value("Vasilev", "Ivan", "2000", "Moscow", "55");
  table.Set("key", value);

  auto result = table.Get("key");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(value.ToString(), result.value().ToString());

  result = table.Get("unknown_key");
  EXPECT_FALSE(result.has_value());
}

TEST(HashTableTest, Exists) {
  HashTable table;
  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);

  EXPECT_TRUE(table.Exists("key1"));
  EXPECT_TRUE(table.Exists("key2"));
  EXPECT_TRUE(table.Exists("key3"));
  EXPECT_FALSE(table.Exists("unknown_key"));
}

TEST(HashTableTest, Del) {
  HashTable table;
  table.Set("key1", Value());
  table.Set("key2", Value());
  table.Set("key3", Value());
  table.Set("key4", Value());
  table.Set("key5", Value());
  table.Set("key6", Value());
  table.Set("key7", Value());
  table.Set("key8", Value());
  table.Set("key9", Value());
  table.Set("key90", Value());
  table.Set("key91", Value());
  table.Set("key92", Value());
  table.Set("key93", Value());
  table.Set("key94", Value());
  table.Set("key95", Value());
  table.Set("key96", Value());
  table.Set("key97", Value());
  EXPECT_TRUE(table.Del("key3"));
  EXPECT_TRUE(table.Del("key4"));
  EXPECT_TRUE(table.Del("key90"));
  EXPECT_TRUE(table.Del("key97"));
  EXPECT_TRUE(table.Del("key96"));
  EXPECT_TRUE(table.Del("key95"));

  EXPECT_FALSE(table.Del("unknown_key"));

  EXPECT_FALSE(table.Exists("key3"));
  EXPECT_EQ(table.Get("key3"), std::nullopt);

  EXPECT_TRUE(table.Exists("key1"));
  EXPECT_TRUE(table.Exists("key9"));
}

TEST(HashTableTest, Update) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);
  EXPECT_TRUE(table.Update("key1", "- - 1999 Msk 90"));
  EXPECT_TRUE(table.Update("key2", "- - - Msk -"));
  EXPECT_TRUE(table.Update("key3", "Ivanov Ivan 2000 Moscow 55"));
  EXPECT_TRUE(table.Get("key1").value().Match("Ivanov Ivan 1999 Msk 90"));
  EXPECT_TRUE(table.Get("key2").value().Match("Petrov Petr 1990 Msk 100"));
  EXPECT_TRUE(table.Get("key3").value().Match("Ivanov Ivan 2000 Moscow 55"));
  EXPECT_FALSE(table.Update("unknown_key", "- - - - -"));
}

TEST(HashTableTest, Keys) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);
  table.Set("key4", value4);
  EXPECT_EQ(table.Keys().size(), 4u);
}

TEST(HashTableTest, Rename) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);
  table.Set("key4", value4);
  EXPECT_TRUE(table.Rename("key3", "key5"));
  EXPECT_FALSE(table.Rename("key1", "key2"));
  EXPECT_EQ(table.Get("key5").value().ToString(), value3.ToString());
  EXPECT_FALSE(table.Get("key3"));
}

TEST(HashTableTest, TTL) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55", "1");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100", "3");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);
  EXPECT_EQ(table.TTL("key1"), 1u);
  EXPECT_EQ(table.TTL("key2"), 3u);
  EXPECT_EQ(table.TTL("key3"), std::nullopt);
  EXPECT_EQ(table.TTL("key4"), std::nullopt);
}

TEST(HashTableTest, Find) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");
  Value value5("Ivanov", "Petr", "2000", "Moscow", "55");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value1);
  table.Set("key4", value3);
  table.Set("key5", value4);
  table.Set("key6", value1);
  table.Set("key7", value5);
  EXPECT_EQ(table.Find("Ivanov - 2000 Moscow 55").size(), 4u);
}

TEST(HashTableTest, ShowAll) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);
  table.Set("key4", value4);
  EXPECT_EQ(table.ShowAll().size(), 4u);
}

TEST(HashTableTest, Export) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "Tver", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  table.Set("key1", value1);
  table.Set("key2", value2);
  table.Set("key3", value3);
  table.Set("key4", value4);
  EXPECT_EQ(table.Export("./export.dat"), 4u);
}

TEST(HashTableTest, Upload) {
  HashTable table;

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "Tver", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  EXPECT_EQ(table.Upload("./export.dat"), 4u);
  EXPECT_EQ(table.Get("key1").value().ToQuotedString(),
            value1.ToQuotedString());
  EXPECT_EQ(table.Get("key2").value().ToQuotedString(),
            value2.ToQuotedString());
  EXPECT_EQ(table.Get("key3").value().ToQuotedString(),
            value3.ToQuotedString());
  EXPECT_EQ(table.Get("key4").value().ToQuotedString(),
            value4.ToQuotedString());
}

TEST(HashTableTest, Resize) {
  HashTable table;
  std::vector<std::string> keys(1024);
  std::vector<Value> values(1024);
  std::generate(keys.begin(), keys.end(),
                [n = 0]() mutable { return "key" + std::to_string(n++); });
  std::generate(values.begin(), values.end(), []() { return Value(); });
  for (std::size_t i = 0; i < 1024; ++i) {
    table.Set(keys[i], values[i]);
  }
  EXPECT_EQ(table.Keys().size(), 1024u);
}
