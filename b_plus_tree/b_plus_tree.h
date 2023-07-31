#ifndef TRANSACTIONS_B_PLUS_TREE_B_PLUS_TREE_H_
#define TRANSACTIONS_B_PLUS_TREE_B_PLUS_TREE_H_

#include <iostream>
#include <unordered_map>

#include "../common/abstract_store.h"
#include "b_plus_node.h"

namespace s21 {

/**
 * @brief In-memory key-value store based on B+ trees.
 *
 * This class implements an in-memory key-value store using a self-balancing
 * binary search tree in the form of a B+ tree. It provides methods to set and
 * retrieve key-value pairs, check for existence, delete records, update values,
 * retrieve keys, rename keys, and perform various other operations.
 */
class BPlusTree : public AbstractStore {
 public:
  using NodePtr = BPlusNode::NodePtr;

  explicit BPlusTree(std::size_t degree);

  bool Set(const Key& key, const Value& value) override;
  std::optional<Value> Get(const Key& key) const override;
  bool Exists(const Key& key) const override;
  bool Del(const Key& key) override;
  bool Update(const Key& key, const std::string& new_value) override;
  std::vector<Key> Keys() const override;
  bool Rename(const Key& old_key, const Key& new_key) override;
  std::optional<std::size_t> TTL(const Key& key) const override;
  std::vector<Key> Find(const std::string& value) const override;
  std::vector<Value> ShowAll() const override;
  std::size_t Upload(const std::string& file_path) override;
  std::size_t Export(const std::string& file_path) const override;
  void DeleteExpiredElements() override;

  void ToDot(const std::string& file_name) const;
  void Show() const;

 private:
  NodePtr FindLeaf(NodePtr node, const Key& key) const;
  void Expand(NodePtr left, NodePtr right, const Key& key);
  std::pair<NodePtr, NodePtr> Adjacents(NodePtr node);
  void Reduce(NodePtr node);

  NodePtr root_;
  NodePtr leaf_;
  std::size_t degree_;
};

}  // namespace s21

#endif  // TRANSACTIONS_B_PLUS_TREE_B_PLUS_TREE_H_
