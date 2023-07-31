#include <gtest/gtest.h>

#include "../b_plus_tree/b_plus_node.h"

using namespace s21;

TEST(BPlusNodeTest, Constructor) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  EXPECT_TRUE(node->IsLeaf());
  EXPECT_EQ(node->Size(), 0);
  EXPECT_FALSE(node->Exists("test_key"));
}

TEST(BPlusNodeTest, IsLeaf) {
  auto leaf = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto internal = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  EXPECT_TRUE(leaf->IsLeaf());
  EXPECT_FALSE(internal->IsLeaf());
}

TEST(BPlusNodeTest, Size) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  EXPECT_EQ(node->Size(), 0u);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  node->Insert("key1", value1);
  EXPECT_EQ(node->Size(), 1u);

  node->Insert("key2", value2);
  EXPECT_EQ(node->Size(), 2u);

  node->Insert("key3", value3);
  EXPECT_EQ(node->Size(), 3u);

  node->Remove("key2");
  EXPECT_EQ(node->Size(), 2u);
}

TEST(BPlusNodeTest, InsertInternal) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto child1 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child2 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);

  node->Insert("key1", child1, false);
  node->Insert("key2", child2, false);
  node->Insert("key3", child2, false);

  EXPECT_EQ(node->Size(), 3u);
  EXPECT_TRUE(node->Exists("key1"));
  EXPECT_TRUE(node->Exists("key2"));
  EXPECT_TRUE(node->Exists("key3"));
}

TEST(BPlusNodeTest, InsertLeaf) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");

  bool success = node->Insert("foo", value1);
  EXPECT_TRUE(success);
  EXPECT_EQ(node->Size(), 1);
  EXPECT_TRUE(node->Exists("foo"));
  EXPECT_EQ(node->GetValue("foo").ToString(), value1.ToString());

  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  success = node->Insert("foo", value2);
  EXPECT_FALSE(success);
  EXPECT_TRUE(node->Exists("foo"));
  EXPECT_EQ(node->Size(), 1);
  EXPECT_EQ(node->GetValue("foo").ToString(), value1.ToString());

  success = node->Insert("bar", value2);
  EXPECT_TRUE(success);
  EXPECT_TRUE(node->Exists("bar"));
  EXPECT_EQ(node->Size(), 2);
  EXPECT_EQ(node->GetValue("bar").ToString(), value2.ToString());

  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  success = node->Insert("baz", value3);
  EXPECT_TRUE(success);
  EXPECT_TRUE(node->Exists("baz"));
  EXPECT_EQ(node->Size(), 3);
  EXPECT_EQ(node->GetValue("baz").ToString(), value3.ToString());
}

TEST(BPlusNodeTest, SplitInternal) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto child1 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child2 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child3 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child4 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child5 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child6 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  child1->SetParent(node);
  child2->SetParent(node);
  child3->SetParent(node);
  child4->SetParent(node);
  child5->SetParent(node);
  child6->SetParent(node);
  node->SetKeys({"key3", "key5", "key7", "key9"});
  child1->SetKeys({"key1", "key2"});
  child2->SetKeys({"key3", "key4"});
  child3->SetKeys({"key5", "key6"});
  child4->SetKeys({"key7", "key8"});
  child5->SetKeys({"key9", "key90"});
  node->SetChildren({child1, child2, child3, child4, child5, child6});

  auto new_node = node->Split();

  EXPECT_EQ(node->Size(), 2u);
  EXPECT_EQ(node->GetKeys().at(0), "key3");
  EXPECT_EQ(node->GetKeys().at(1), "key5");
  EXPECT_EQ(node->GetChildren().at(0), child1);
  EXPECT_EQ(node->GetChildren().at(1), child2);
  EXPECT_EQ(new_node->Size(), 2u);
  EXPECT_EQ(new_node->GetKeys().at(0), "key7");
  EXPECT_EQ(new_node->GetKeys().at(1), "key9");
  EXPECT_EQ(new_node->GetChildren().at(0), child4);
  EXPECT_EQ(new_node->GetChildren().at(1), child5);
}

TEST(BPlusNodeTest, SplitLeaf) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value4("Vasilev", "Vasiliy", "2002", "Moscow", "150");

  node->Insert("key1", value1);
  node->Insert("key2", value2);
  node->Insert("key3", value3);
  node->Insert("key4", value4);

  auto new_node = node->Split();

  EXPECT_EQ(node->Size(), 2u);
  EXPECT_EQ(node->GetValue("key1").ToString(), value1.ToString());
  EXPECT_EQ(node->GetValue("key2").ToString(), value2.ToString());
  EXPECT_EQ(new_node->Size(), 2u);
  EXPECT_EQ(new_node->GetValue("key3").ToString(), value3.ToString());
  EXPECT_EQ(new_node->GetValue("key4").ToString(), value4.ToString());
}

TEST(BPlusNodeTest, Delete) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto child1 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child2 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto child3 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  node->Insert("key1", child1, false);
  node->Insert("key2", child2, false);
  node->Insert("key3", child3, false);

  node->Delete("key2");

  EXPECT_EQ(node->Size(), 2);
  EXPECT_FALSE(node->Exists("key2"));
  EXPECT_TRUE(node->Exists("key1"));
  EXPECT_TRUE(node->Exists("key3"));
}

TEST(BPlusNodeTest, Remove) {
  BPlusNode node(BPlusNode::NodeType::kLeaf);
  Value value("Ivanov", "Ivan", "2000", "Moscow", "55");
  node.Insert("key1", value);
  node.Insert("key2", value);
  node.Insert("key3", value);

  EXPECT_TRUE(node.Remove("key2"));
  EXPECT_FALSE(node.Exists("key2"));
  EXPECT_EQ(node.Size(), 2);

  EXPECT_FALSE(node.Remove("key4"));
  EXPECT_EQ(node.Size(), 2);
}

TEST(BPlusNodeTest, RedistributeLeaf) {
  auto parent = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto left = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto right = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value4("Sidorov", "Sergei", "1980", "Novosibirsk", "50");
  Value value5("Vasilev", "Vasiliy", "2002", "Moscow", "150");
  left->Insert("key1", value1);
  left->Insert("key2", value2);
  right->Insert("key4", value4);
  right->Insert("key5", value5);
  left->SetNext(node);
  node->SetNext(right);

  left->SetParent(parent);
  right->SetParent(parent);
  parent->AddChild(left);
  parent->AddKey("key3");
  parent->AddChild(right);

  node->Redistribute(right, left);

  EXPECT_EQ(left->GetKeys().size(), 2);
  EXPECT_EQ(left->GetValues().size(), 2);
  EXPECT_EQ(left->GetKeys().at(0), "key1");
  EXPECT_EQ(left->GetKeys().at(1), "key2");
  EXPECT_EQ(left->GetValues().at(0).ToString(), value1.ToString());
  EXPECT_EQ(left->GetValues().at(1).ToString(), value2.ToString());
  EXPECT_EQ(right->GetKeys().size(), 2);
  EXPECT_EQ(right->GetValues().size(), 2);
  EXPECT_EQ(right->GetKeys().at(0), "key4");
  EXPECT_EQ(right->GetKeys().at(1), "key5");
  EXPECT_EQ(right->GetValues().at(0).ToString(), value4.ToString());
  EXPECT_EQ(parent->GetKeys().at(0), "key3");
}

TEST(BPlusNodeTest, RedistributeInternal) {
  auto parent = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);

  parent->AddChild(node);
  node->SetParent(parent);
  parent->AddKey("key1");
  node->AddKey("key2");

  parent->Redistribute(node, parent);

  EXPECT_EQ(parent->Size(), 1);
  EXPECT_EQ(node->Size(), 1);
  EXPECT_EQ(parent->GetKeys().at(0), "key1");
  EXPECT_EQ(node->GetKeys().at(0), "key2");
  EXPECT_EQ(parent->GetChildren().at(0), node);
  EXPECT_EQ(node->GetParent().lock(), parent);
}

TEST(BPlusNodeTest, MergeInternal) {
  auto parent = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto node1 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto node2 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto node3 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);

  parent->Insert("key1", node1, false);
  node1->Insert("key2", node2, false);
  node2->Insert("key3", node3, false);

  node1->Merge(node2);

  EXPECT_EQ(parent->Size(), 0);
  EXPECT_EQ(node1->Size(), 3);
  EXPECT_EQ(node1->GetKeys().at(0), "key2");
  EXPECT_EQ(node1->GetKeys().at(1), "key1");
  EXPECT_EQ(node1->GetKeys().at(2), "key3");
  EXPECT_EQ(node1->GetChildren().size(), 2);
  EXPECT_EQ(node1->GetChildren().at(0), node2);
  EXPECT_EQ(node1->GetChildren().at(1), node3);
  EXPECT_EQ(node2->Size(), 1);
  EXPECT_EQ(node2->GetChildren().at(0), node3);
}

TEST(BPlusNodeTest, MergeLeaf) {
  auto parent = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
  auto node1 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);
  auto node2 = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");

  node1->SetParent(parent);
  parent->Insert("foo", node1, false);
  node1->Insert("key1", value1);
  node2->Insert("key2", value2);

  node1->Merge(node2);

  EXPECT_EQ(node1->Size(), 2);
  EXPECT_TRUE(node1->Exists("key1"));
  EXPECT_TRUE(node1->Exists("key2"));
  EXPECT_EQ(node1->GetValues()[0].ToString(), value1.ToString());
  EXPECT_EQ(node1->GetValues()[1].ToString(), value2.ToString());
  EXPECT_EQ(parent->Size(), 0);
  EXPECT_FALSE(parent->Exists("foo"));
}

TEST(BPlusNodeTest, GetValue) {
  auto node = std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf);

  Value value1("Ivanov", "Ivan", "2000", "Moscow", "55");
  Value value2("Petrov", "Petr", "1990", "St. Petersburg", "100");
  Value value3("Sidorov", "Sergei", "1980", "Novosibirsk", "50");

  node->Insert("key1", value1);
  node->Insert("key2", value2);
  node->Insert("key3", value3);

  Value value = node->GetValue("key2");
  EXPECT_EQ(value2.ToString(), value.ToString());
}
