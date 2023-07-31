#ifndef __SELF_BALANCING_BINARY_SEARCH_TREE_H__
#define __SELF_BALANCING_BINARY_SEARCH_TREE_H__

#include <cstddef>
#include <fstream>
#include <functional>
#include <memory>

#include "../common/abstract_store.h"

namespace s21 {

class SelfBalancingBinarySearchTree : public AbstractStore {
 public:
  bool Set(const Key& key, const Value& value) override;
  bool Exists(const Key& key) const override;
  bool Del(const Key& key) override;
  std::optional<Value> Get(const Key& key) const override;
  std::vector<Key> Keys() const override;
  std::vector<Value> ShowAll() const override;
  bool Update(const Key& key, const std::string& new_value) override;
  bool Rename(const Key& old_key, const Key& new_key) override;
  std::size_t Upload(const std::string& file_name) override;
  std::size_t Export(const std::string& file_name) const override;
  std::vector<Key> Find(const std::string& value) const override;
  std::optional<std::size_t> TTL(const Key& key) const override;
  void MakeDotFile(const std::string& file_name) const;
  int GetBalance(const Key& key) const;
  const Key GetRootKey() const;
  void DeleteExpiredElements() override;

 private:
  struct AVLNode {
    AVLNode(const Key& k, const Value& v)
        : key(k), value(v), left(nullptr), right(nullptr), height(0){};
    Key key;
    Value value;
    std::unique_ptr<AVLNode> left;
    std::unique_ptr<AVLNode> right;
    int height;
  };

  int GetBalance(const std::unique_ptr<AVLNode>& node) const;
  int GetHeight(const std::unique_ptr<AVLNode>& node) const;
  int GetHeight(const AVLNode* node) const;
  void InOrderTraversal(const std::unique_ptr<AVLNode>& node,
                        std::vector<Key>& keys,
                        std::vector<Value>& values) const;
  void InsertHelper(std::unique_ptr<AVLNode>& node, const Key& key,
                    const Value& value);
  std::optional<AVLNode*> FindNode(const std::unique_ptr<AVLNode>& node,
                                   const Key& key) const;
  std::unique_ptr<AVLNode> DeletHelper(std::unique_ptr<AVLNode> node,
                                       const Key& key);
  void UpdateHeight(std::unique_ptr<AVLNode>& node);
  void RotateLeft(std::unique_ptr<AVLNode>& node);
  void RotateRight(std::unique_ptr<AVLNode>& node);
  void BalanceNode(std::unique_ptr<AVLNode>& node);
  const AVLNode* FindMin(const AVLNode* node) const;

  std::unique_ptr<SelfBalancingBinarySearchTree::AVLNode> root_;
};

}  // namespace s21

#endif  // __SELF_BALANCING_BINARY_SEARCH_TREE_H__