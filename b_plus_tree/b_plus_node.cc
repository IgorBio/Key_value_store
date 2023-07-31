#include "b_plus_node.h"

namespace s21 {

/**
 * @brief Constructs a new BPlusNode object.
 *
 * This constructor creates a new BPlusNode object with the specified type. The
 * type can be either kLeaf or kInternal, depending on whether the node is a
 * leaf node or an internal node in the B+ tree. By default, the parent_ and
 * next_ member variables are set to null values.
 *
 * @param type The type of the node (either kLeaf or kInternal).
 */
BPlusNode::BPlusNode(const NodeType type) : type_(type), parent_(), next_() {}

/**
 * @brief Checks if the node is a leaf node.
 * @return true if the node is a leaf node, false otherwise.
 */
bool BPlusNode::IsLeaf() const { return type_ == NodeType::kLeaf; }

/**
 * @brief Returns the number of keys in the node.
 *
 * This method returns the number of keys in the node, which is equal to the
 * number of values if the node is a leaf node, or one less than the number of
 * children if the node is an internal node.
 *
 * @return The number of keys in the node.
 */
std::size_t BPlusNode::Size() const { return keys_.size(); }

/**
 * @brief Checks if a key exists in the node.
 *
 * This method performs a binary search on the node's keys to determine if the
 * given key exists in the node.
 *
 * @param key The key to search for.
 * @return true if the key exists in the node, false otherwise.
 */
bool BPlusNode::Exists(const Key& key) const {
  return std::binary_search(keys_.begin(), keys_.end(), key);
}

/**
 * @brief Inserts a new key into an internal node of the B+ tree.
 *
 * Inserts the given key and child node into the internal node in sorted order.
 * The child node's parent pointer is set to the current node.
 *
 * @param key The key to insert
 * @param node The child node to insert
 */
void BPlusNode::Insert(const Key& key, NodePtr node, bool odd) {
  auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
  node->parent_ = shared_from_this();
  children_.insert(children_.begin() + std::distance(keys_.begin(), it) + odd,
                   node);
  keys_.insert(it, key);
}

/**
 * @brief Inserts a new key-value pair into the leaf node.
 *
 * Inserts a new key-value pair into the leaf node. The key is inserted in
 * sorted order, and the corresponding value is inserted at the same index as
 * the key. If the key already exists in the node, the method returns false and
 * does not insert the key-value pair.
 *
 * @param key The key to be inserted.
 * @param value The value to be inserted.
 * @return true if the key-value pair was successfully inserted, false if the
 * key already exists in the node.
 */
bool BPlusNode::Insert(const Key& key, const Value& value) {
  auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
  if (it != keys_.end() and *it == key) {
    return false;
  }
  values_.insert(values_.begin() + std::distance(keys_.begin(), it), value);
  keys_.insert(it, key);
  return true;
}

/**
 * @brief Splits the node into two nodes during B+ tree insertion.
 *
 * This method splits the node into two nodes by dividing its keys at the
 * midpoint. If the node is a leaf node, it also splits the associated values
 * and updates the next pointers accordingly. If the node is an internal node,
 * it splits the child nodes and updates their parent pointers.
 *
 * @return A shared pointer to the newly created node resulting from the split.
 */
BPlusNode::NodePtr BPlusNode::Split() {
  const std::size_t mid = Size() / 2;
  NodePtr new_node = std::make_shared<BPlusNode>(
      IsLeaf() ? NodeType::kLeaf : NodeType::kInternal);
  std::move(keys_.begin() + mid, keys_.end(),
            std::back_inserter(new_node->keys_));
  keys_.resize(mid);
  if (IsLeaf()) {
    std::move(values_.begin() + mid, values_.end(),
              std::back_inserter(new_node->values_));
    values_.resize(mid);
    new_node->next_ = std::move(next_);
    next_ = new_node;
  } else {
    std::move(children_.begin() + mid + 1, children_.end(),
              std::back_inserter(new_node->children_));
    children_.resize(mid + 1);
    for (NodePtr& child : new_node->children_) {
      child->parent_ = new_node;
    }
  }
  return new_node;
}

/**
 * @brief Deletes an internal key and its corresponding child node.
 *
 * This method deletes the specified key and its corresponding child node from
 * the internal node.
 *
 * @param key The key to be deleted.
 * @param odd Specifies whether to remove the child node pointer at the next
 * index (true) or the same index as the deleted key (false). Defaults to true.
 */
void BPlusNode::Delete(const Key& key, bool odd) {
  auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
  if (it != keys_.end() and *it == key) {
    auto idx = std::distance(keys_.begin(), it);
    children_.erase(children_.begin() + idx + odd);
    keys_.erase(it);
  }
}

/**
 * @brief Removes a key-value pair from a leaf node.
 *
 * This method searches for the given key in the node's keys vector using binary
 * search. If the key is found, the corresponding value is removed from the
 * node's values vector and the key is removed from the node's keys vector.
 *
 * @param key The key to remove from the leaf node.
 * @return true if the key was found and removed, false otherwise.
 */
bool BPlusNode::Remove(const Key& key) {
  auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
  if (it == keys_.end() or *it != key) return false;
  auto idx = std::distance(keys_.begin(), it);
  keys_.erase(it);
  values_.erase(values_.begin() + idx);
  return true;
}

/**
 * @brief Redistributes keys, values, and children nodes with another node
 * during a reducing tree after deletion.
 *
 * If the current node is a leaf node, it redistributes keys and values with the
 * given `node`. If the current node is an internal node, it redistributes keys
 * and children with the given `node`. This method is used during the B+ tree
 * reducing process.
 *
 * @param src The source node to redistribute data.
 * @param node The node that receives data from `src`.
 */
void BPlusNode::Redistribute(NodePtr src, NodePtr node) {
  NodePtr parent = parent_.lock();
  if (!parent) return;

  const Key& key = (src == node) ? src->keys_.back() : src->keys_.front();
  const NodePtr left_node = (src == node) ? src : shared_from_this();

  auto it = std::lower_bound(parent->children_.begin(), parent->children_.end(),
                             left_node);
  const auto idx = std::distance(parent->children_.begin(), it);
  if (IsLeaf()) {
    const NodePtr right_node = (src == node) ? shared_from_this() : src;
    Insert(key, src->GetValue(key));
    src->Remove(key);
    parent->keys_[idx] = right_node->keys_.front();
  } else {
    const NodePtr child =
        (src == node) ? src->children_.back() : src->children_.front();
    Insert(parent->keys_[idx], child, src != node);
    parent->keys_[idx] = key;
    src->Delete(key, src == node);
  }
}

/**
 * @brief Merges the current node with the specified node.
 *
 * This method is used to merge the current node (leaf or internal) with the
 * specified node when the current node has insufficient keys/children after a
 * deletion operation. The merge operation will update the keys, values, and
 * children of the current node based on the keys, values, and children of the
 * specified node.
 *
 * @param node The node to merge with the current node.
 */
void BPlusNode::Merge(NodePtr node) {
  NodePtr parent = parent_.lock();
  if (!parent) return;

  auto it = std::lower_bound(parent->children_.begin(), parent->children_.end(),
                             node);
  const auto idx = std::distance(parent->children_.begin(), it);

  if (IsLeaf()) {
    parent->keys_.erase(parent->keys_.begin() + idx - 1);
    parent->children_.erase(parent->children_.begin() + idx);
    std::move(node->keys_.begin(), node->keys_.end(),
              std::back_inserter(keys_));
    std::move(node->values_.begin(), node->values_.end(),
              std::back_inserter(values_));
    next_ = node->next_;
  } else {
    keys_.push_back(parent->keys_[idx - 1]);
    keys_.insert(keys_.end(), node->keys_.begin(), node->keys_.end());
    children_.insert(children_.end(), node->children_.begin(),
                     node->children_.end());
    for (NodePtr& child : node->children_) {
      child->parent_ = shared_from_this();
    }
    parent->children_.erase(it);
    parent->keys_.erase(parent->keys_.begin() + idx - 1);
  }
}

/**
 * @brief Returns the value associated with the given key.
 *
 * @param key The key to look up in this node's keys_ vector.
 * @return An optional Value object if the key is found, or std::nullopt
 * otherwise.
 */
Value& BPlusNode::GetValue(const Key& key) {
  auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
  auto idx = std::distance(keys_.begin(), it);
  return values_[idx];
}

}  // namespace s21
