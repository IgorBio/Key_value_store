#ifndef TRANSACTIONS_B_PLUS_TREE_B_PLUS_NODE_H_
#define TRANSACTIONS_B_PLUS_TREE_B_PLUS_NODE_H_

#include <algorithm>
#include <fstream>
#include <memory>
#include <vector>

#include "../common/value.h"

namespace s21 {
using Key = std::string;

/**
 * @brief A node in a B+ tree.
 *
 * This class represents a node in a B+ tree. It can be either a leaf node or an
 * internal (non-leaf) node. Leaf nodes store key-value pairs, while non-leaf
 * nodes only store keys and pointers to child nodes.
 */
class BPlusNode : public std::enable_shared_from_this<BPlusNode> {
 public:
  using NodePtr = std::shared_ptr<BPlusNode>;
  using WeakPtr = std::weak_ptr<BPlusNode>;

  enum class NodeType { kLeaf, kInternal };

  explicit BPlusNode(const NodeType type);

  bool IsLeaf() const;
  std::size_t Size() const;
  bool Exists(const Key& key) const;
  void Insert(const Key& key, NodePtr node, bool odd = true);
  bool Insert(const Key& key, const Value& value);
  NodePtr Split();
  void Delete(const Key& key, bool odd = true);
  bool Remove(const Key& key);
  void Redistribute(NodePtr src, NodePtr node);
  void Merge(NodePtr node);
  Value& GetValue(const Key& key);

  std::vector<Key>& GetKeys() { return keys_; }
  std::vector<Value>& GetValues() { return values_; }
  std::vector<NodePtr>& GetChildren() { return children_; }
  WeakPtr GetParent() const { return parent_; }
  void SetParent(WeakPtr parent) { parent_ = parent; }
  void AddKey(const Key& key) { keys_.push_back(key); }
  void AddValue(const Value& value) { values_.push_back(value); }
  void AddChild(NodePtr child) { children_.push_back(child); }
  void DelKey(std::size_t idx) { keys_.erase(keys_.begin() + idx); }
  void DelValue(std::size_t idx) { values_.erase(values_.begin() + idx); }
  void DelChild(std::size_t idx) { children_.erase(children_.begin() + idx); }
  void DelKeys() { keys_.clear(); }
  void DelValues() { values_.clear(); }
  void DelChildren() { children_.clear(); }
  void SetKey(std::size_t idx, const Key& key) { keys_[idx] = key; }
  void SetKeys(const std::vector<Key>& keys) { keys_ = keys; }
  void SetValue(std::size_t idx, const Value& value) { values_[idx] = value; }
  void SetValues(const std::vector<Value>& values) { values_ = values; }
  void SetChildren(const std::vector<NodePtr>& ch) { children_ = ch; }
  NodePtr GetNext() const { return next_; }
  void SetNext(NodePtr next) { next_ = next; }

 private:
  NodeType type_;
  std::vector<Key> keys_;
  std::vector<Value> values_;
  std::vector<NodePtr> children_;
  WeakPtr parent_;
  NodePtr next_;
};
}  // namespace s21

#endif  // TRANSACTIONS_B_PLUS_TREE_B_PLUS_NODE_H_
