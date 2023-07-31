#include <gtest/gtest.h>

#include <thread>

#include "../common/value.h"

using namespace s21;

TEST(ValueTest, Constructor) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55");
  EXPECT_EQ(v.TTL(), std::nullopt);
}

TEST(ValueTest, ConstructorWithTTL) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55", "10");
  EXPECT_EQ(v.TTL(), 10u);
  EXPECT_FALSE(v.IsExpired());
}

TEST(ValueTest, ToQuotedString) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55");
  EXPECT_EQ(v.ToQuotedString(), "\"Ivanov\" \"Ivan\" 2001 \"Rostov\" 55");
}

TEST(ValueTest, ToString) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55");
  EXPECT_EQ(v.ToString(), "Ivanov Ivan 2001 Rostov 55");
}

TEST(ValueTest, FromString) {
  std::string value = "\"Ivanov\" \"Ivan\" 2001 \"Rostov\" 55";
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55");
  EXPECT_EQ(Value::FromString(value), v);
}

TEST(ValueTest, Update) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55");
  v.Update("Petrov - - Moscow 123");
  EXPECT_EQ(v.ToString(), "Petrov Ivan 2001 Moscow 123");
  v.Update("- - 1970 - -");
  EXPECT_EQ(v.ToString(), "Petrov Ivan 1970 Moscow 123");
  EXPECT_EQ(v.TTL(), std::nullopt);
}

TEST(ValueTest, TTL) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55", "2");
  EXPECT_EQ(v.TTL(), 2u);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(v.TTL(), 1u);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(v.TTL(), 0u);
}

TEST(ValueTest, Match) {
  Value v1("Ivanov", "Ivan", "2001", "Rostov", "55");
  EXPECT_TRUE(v1.Match("Ivanov Ivan 2001 Rostov 55"));
  EXPECT_TRUE(v1.Match("Ivanov - - - - 2"));
  EXPECT_FALSE(v1.Match("Ivanov Ivan 1970 Rostov 55"));
}

TEST(ValueTest, IsExpired) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55", "1");
  EXPECT_FALSE(v.IsExpired());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_TRUE(v.IsExpired());

  Value value("Ivanov", "Ivan", "2001", "Rostov", "55");
  EXPECT_FALSE(value.IsExpired());
}

TEST(ValueTest, Exceptions) {
  Value v("Ivanov", "Ivan", "2001", "Rostov", "55", "10");
  EXPECT_THROW(v.Update("Ivanov Ivan aaa Rostov 55 20"), std::invalid_argument);
  EXPECT_THROW(v.Update("Ivanov Ivan 2001 Rostov aaa 20"),
               std::invalid_argument);
  EXPECT_THROW(v.Update("Ivanov Ivan 2001 Rostov 55 aaa"),
               std::invalid_argument);
  EXPECT_THROW(v.Update("Ivanov Ivan 2001 Rostov 55 20 aaa"),
               std::invalid_argument);
  EXPECT_THROW(v.Update("Ivanov Ivan 2001 Rostov"), std::invalid_argument);
  EXPECT_THROW(Value v("Ivanov", "Ivan", "aaa", "Rostov", "55", "10"),
               std::invalid_argument);
}
