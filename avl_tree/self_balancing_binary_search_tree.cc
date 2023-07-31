#include "self_balancing_binary_search_tree.h"

namespace s21 {
/**
 * @brief Set the value associated with the given key in the self-balancing
 * binary search tree.
 *
 * @param key The key to set the value for.
 * @param value The value to set for the given key.
 *
 * @return true if the value was set successfully, false if the key already
 * exists in the tree.
 */
bool SelfBalancingBinarySearchTree::Set(const Key& key, const Value& value) {
  if (FindNode(root_, key).has_value()) return false;
  InsertHelper(root_, key, value);
  return true;
}

/**
 * @brief Retrieves the value associated with the given key in the
 * self-balancing binary search tree.
 *
 * @param key The key to search for in the tree.
 *
 * @return An optional value that is associated with the given key if it exists
 * in the tree, otherwise returns std::nullopt.
 */
std::optional<Value> SelfBalancingBinarySearchTree::Get(const Key& key) const {
  auto result = FindNode(root_, key);
  if (result.has_value()) {
    return result.value()->value;
  }
  return std::nullopt;
}

/**
 * @brief Check if a given key exists in the self-balancing binary search tree.
 *
 * @param key the key to search for
 *
 * @return true if the key exists in the tree, false otherwise
 */
bool SelfBalancingBinarySearchTree::Exists(const Key& key) const {
  return FindNode(root_, key).has_value();
}

/**
 * @brief Deletes a node with the specified key from the self-balancing binary
 * search tree.
 *
 * @param key the key of the node to be deleted
 *
 * @return true if the node was successfully deleted, false otherwise
 */
bool SelfBalancingBinarySearchTree::Del(const Key& key) {
  if (!FindNode(root_, key).has_value()) return false;
  root_ = DeletHelper(std::move(root_), key);
  return true;
}

/**
 * @brief Finds the minimum value in a self-balancing binary search tree.
 *
 * @param node a pointer to the root node of the tree
 *
 * @return a const pointer to the node containing the minimum value
 */
const SelfBalancingBinarySearchTree::AVLNode*
SelfBalancingBinarySearchTree::FindMin(const AVLNode* node) const {
  while (node->left) {
    node = node->left.get();
  }
  return node;
}

/**
 * @brief Deletes a node with the specified key from the self-balancing binary
 * search tree.
 *
 * @param node A unique pointer to the root node of the tree.
 * @param key The key of the node to be deleted.
 *
 * @return A unique pointer to the root node after the deletion.
 */
std::unique_ptr<SelfBalancingBinarySearchTree::AVLNode>
SelfBalancingBinarySearchTree::DeletHelper(std::unique_ptr<AVLNode> node,
                                           const Key& key) {
  if (node == nullptr) return nullptr;
  if (key < node->key) {
    node->left = DeletHelper(std::move(node->left), key);
  } else if (key > node->key) {
    node->right = DeletHelper(std::move(node->right), key);
  } else {
    if (node->left == nullptr || node->right == nullptr) {
      node = (node->left == nullptr) ? std::move(node->right)
                                     : std::move(node->left);
    } else {
      const AVLNode* min_right = FindMin(node->right.get());
      node->key = min_right->key;
      node->value = min_right->value;
      node->right = DeletHelper(std::move(node->right), min_right->key);
    }
  }
  if (node != nullptr) {
    UpdateHeight(node);
    BalanceNode(node);
  }
  return node;
}

/**
 * @brief Find a node with the given key in the self-balancing binary search
 * tree.
 *
 * @param node A pointer to the root node of the tree to search.
 * @param key The key to search for.
 *
 * @return An optional pointer to the node with the given key, or std::nullopt
 * if the key is not found.
 */
std::optional<SelfBalancingBinarySearchTree::AVLNode*>
SelfBalancingBinarySearchTree::FindNode(const std::unique_ptr<AVLNode>& node,
                                        const Key& key) const {
  if (node == nullptr) return std::nullopt;
  if (node->key == key) return node.get();
  if (node->key > key) {
    return FindNode(node->left, key);
  } else {
    return FindNode(node->right, key);
  }
}

/**
 * @brief Recursively inserts a new node with the given key and value into the
 * self-balancing binary search tree.
 *
 * @param node A reference to a unique pointer to an AVLNode representing
 *             the root node of the tree or subtree.
 * @param key The key of the new node to be inserted.
 * @param value The value of the new node to be inserted.
 */
void SelfBalancingBinarySearchTree::InsertHelper(std::unique_ptr<AVLNode>& node,
                                                 const Key& key,
                                                 const Value& value) {
  if (node == nullptr) {
    node = std::make_unique<AVLNode>(key, value);
    return;
  }
  if (key < node->key) {
    InsertHelper(node->left, key, value);
  } else {
    InsertHelper(node->right, key, value);
  }
  UpdateHeight(node);
  BalanceNode(node);
}

/**
 * @brief Perform an in-order traversal of a self-balancing binary search tree.
 *
 * @param node A pointer to the root node of the tree.
 * @param keys A vector to store the keys of the nodes in the tree.
 * @param values A vector to store the values of the nodes in the tree.
 */
void SelfBalancingBinarySearchTree::InOrderTraversal(
    const std::unique_ptr<AVLNode>& node, std::vector<Key>& keys,
    std::vector<Value>& values) const {
  if (node == nullptr) return;
  InOrderTraversal(node->left, keys, values);
  keys.push_back(node->key);
  values.push_back(node->value);
  InOrderTraversal(node->right, keys, values);
}

/**
 * @brief Retrieves all the keys in the self-balancing binary search tree.
 *
 * @return A vector containing all the keys in the tree.
 */
std::vector<Key> SelfBalancingBinarySearchTree::Keys() const {
  std::vector<Key> vec_keys;
  std::vector<Value> vec_values;
  InOrderTraversal(root_, vec_keys, vec_values);
  return vec_keys;
}

/**
 * @brief Retrieves all values stored in a self-balancing binary search tree.
 *
 * @return A vector containing all the values in the tree.
 *
 * @throws None
 */
std::vector<Value> SelfBalancingBinarySearchTree::ShowAll() const {
  std::vector<Value> vec_values;
  std::vector<Key> vec_keys;
  InOrderTraversal(root_, vec_keys, vec_values);
  return vec_values;
}

/**
 * @brief Updates the value associated with the given key in the self-balancing
 * binary search tree.
 *
 * @param key the key to update
 * @param new_value the new value to associate with the key
 *
 * @return true if the update was successful, false otherwise
 */
bool SelfBalancingBinarySearchTree::Update(const Key& key,
                                           const std::string& new_value) {
  auto node = FindNode(root_, key);
  if (!node.has_value()) return false;
  node.value()->value.Update(new_value);
  return true;
}

/**
 * @brief Renames a node in the self-balancing binary search tree.
 *
 * @param old_key the key of the node to be renamed
 * @param new_key the new key for the renamed node
 *
 * @return true if the node was successfully renamed, false otherwise
 */
bool SelfBalancingBinarySearchTree::Rename(const Key& old_key,
                                           const Key& new_key) {
  auto node = FindNode(root_, old_key);
  if (!node.has_value()) return false;
  Value tmp_val = node.value()->value;
  Del(old_key);
  return Set(new_key, tmp_val);
}

/**
 * @brief Uploads data from a file into a self-balancing binary search tree.
 *
 * @param file_name The name of the file to upload.
 *
 * @return The number of keys uploaded from the file.
 *
 * @throws std::invalid_argument If the file cannot be opened.
 */
std::size_t SelfBalancingBinarySearchTree::Upload(
    const std::string& file_name) {
  std::ifstream file(file_name);
  if (!file.is_open()) throw std::invalid_argument("File can't be opened");
  Key read_key;
  std::string read_value;
  std::size_t count_keys = 0;
  while (file >> read_key) {
    std::getline(file >> std::ws, read_value);
    Set(read_key, Value::FromString(read_value));
    ++count_keys;
  }
  file.close();
  return count_keys;
}

/**
 * @brief Export the contents of the self-balancing binary search tree to a
 * file.
 *
 * @param file_name The name of the file to export the contents to.
 *
 * @return The number of keys exported.
 *
 * @throws std::invalid_argument If the file cannot be opened.
 */
std::size_t SelfBalancingBinarySearchTree::Export(
    const std::string& file_name) const {
  std::ofstream file(file_name);
  if (!file.is_open()) throw std::invalid_argument("File can't be opened");

  std::size_t count_keys = 0;
  auto keys = Keys();
  auto values = ShowAll();
  for (auto i = 0u; i < keys.size(); ++i) {
    file << keys[i] << " " << values[i].ToQuotedString() << "\n";
    ++count_keys;
  }
  file.close();
  return count_keys;
}

/**
 * @brief Returns the Time To Live (TTL) value associated with the given key in
 * the self-balancing binary search tree.
 *
 * @param key the key to search for in the tree
 *
 * @return an optional size_t value representing the TTL, or std::nullopt if
 *         the key is not found
 */
std::optional<std::size_t> SelfBalancingBinarySearchTree::TTL(
    const Key& key) const {
  auto node = FindNode(root_, key);
  if (!node.has_value()) return std::nullopt;
  return node.value()->value.TTL();
}

/**
 * @brief Finds all the keys in the self-balancing binary search tree that match
 * the given value.
 *
 * @param value the value to match
 *
 * @return a vector of keys that match the given value
 */
std::vector<Key> SelfBalancingBinarySearchTree::Find(
    const std::string& value) const {
  std::vector<Key> result_match;
  std::vector<Key> keys = Keys();
  std::vector<Value> value_storage = ShowAll();
  for (auto i = 0u; i < keys.size(); ++i) {
    if (value_storage[i].Match(value)) {
      result_match.push_back(keys[i]);
    }
  }
  return result_match;
}

/**
 * @brief Returns the height of the given AVLNode.
 *
 * @param node a unique pointer to an AVLNode
 *
 * @return the height of the AVLNode, or -1 if the node is null
 */
int SelfBalancingBinarySearchTree::GetHeight(
    const std::unique_ptr<AVLNode>& node) const {
  return (node == nullptr) ? -1 : node->height;
}

/**
 * @brief Updates the height of a node in a self-balancing binary search tree.
 *
 * @param node a reference to the node to update the height of
 */
void SelfBalancingBinarySearchTree::UpdateHeight(
    std::unique_ptr<AVLNode>& node) {
  node->height = std::max(GetHeight(node->left), GetHeight(node->right)) + 1;
}

/**
 * @brief Calculates the balance factor of a given node in a self-balancing
 * binary search tree.
 *
 * @param node A pointer to the node for which the balance factor is being
 * calculated.
 *
 * @return The balance factor of the node, which is the difference between the
 * height of its right subtree and the height of its left subtree.
 */
int SelfBalancingBinarySearchTree::GetBalance(
    const std::unique_ptr<AVLNode>& node) const {
  return (node == nullptr) ? 0 : GetHeight(node->right) - GetHeight(node->left);
}

/**
 * @brief RotateLeft function rotates the given node to the left in a
 * self-balancing binary search tree.
 *
 * @param node the node to be rotated
 */
void SelfBalancingBinarySearchTree::RotateLeft(std::unique_ptr<AVLNode>& node) {
  std::unique_ptr<AVLNode> buffer = std::move(node->right);
  node->right = std::move(buffer->left);
  buffer->left = std::move(node);
  node = std::move(buffer);
  UpdateHeight(node->left);
  UpdateHeight(node);
}

/**
 * @brief Rotates a node to the right in a Self-Balancing Binary Search Tree.
 *
 * @param node The node to be rotated.
 */
void SelfBalancingBinarySearchTree::RotateRight(
    std::unique_ptr<AVLNode>& node) {
  std::unique_ptr<AVLNode> buffer = std::move(node->left);
  node->left = std::move(buffer->right);
  buffer->right = std::move(node);
  node = std::move(buffer);
  UpdateHeight(node->right);
  UpdateHeight(node);
}

/**
 * @brief Balances a node in a self-balancing binary search tree.
 *
 * @param node A reference to a unique_ptr to the AVLNode to be balanced.
 */
void SelfBalancingBinarySearchTree::BalanceNode(
    std::unique_ptr<AVLNode>& node) {
  if (GetBalance(node) == -2) {
    if (GetBalance(node->left) > 0) {
      RotateLeft(node->left);
    }
    RotateRight(node);
  } else if (GetBalance(node) == 2) {
    if (GetBalance(node->right) < 0) {
      RotateRight(node->right);
    }
    RotateLeft(node);
  }
}

/**
 * @brief Generates a dot file representation of the self-balancing binary
 * search tree.
 *
 * @param file_name the name of the file to be generated
 *
 * @throws std::invalid_argument if the file cannot be opened
 */
void SelfBalancingBinarySearchTree::MakeDotFile(
    const std::string& file_name) const {
  std::ofstream file;
  file.open(file_name + ".dot", std::ios::trunc);
  if (!file.is_open()) throw std::invalid_argument("File can't be opened");
  file << "digraph BST {\n";
  file << "    node [shape=circle, style=filled, fillcolor=green];\n";
  std::function<void(const std::unique_ptr<AVLNode>& node)> writeNodes =
      [&](const std::unique_ptr<AVLNode>& node) {
        if (node == nullptr) {
          return;
        }
        file << "    " << node->key << ";\n";
        if (node->left != nullptr) {
          file << "    " << node->key << " -> " << node->left->key << ";\n";
          writeNodes(node->left);
        }
        if (node->right != nullptr) {
          file << "    " << node->key << " -> " << node->right->key << ";\n";
          writeNodes(node->right);
        }
      };
  writeNodes(root_);
  file << "}";
  file.close();
}

/**
 * @brief Calculates the balance factor of a node in a self-balancing binary
 * search tree.
 *
 * @param key the key of the node to calculate the balance factor for
 *
 * @return the balance factor of the node
 */
int SelfBalancingBinarySearchTree::GetBalance(const Key& key) const {
  auto node = FindNode(root_, key);
  return (node.value() == nullptr)
             ? 0
             : GetHeight(node.value()->right) - GetHeight(node.value()->left);
}

/**
 * @brief Returns the height of the given AVLNode.
 *
 * @param node a pointer to the AVLNode
 *
 * @return the height of the AVLNode; -1 if the node is nullptr
 */
int SelfBalancingBinarySearchTree::GetHeight(const AVLNode* node) const {
  return (node == nullptr) ? -1 : node->height;
}

/**
 * @brief Returns the key of the root node in the self-balancing binary search
 * tree.
 *
 * @return The key of the root node, or an empty string if the tree is empty.
 */
const Key SelfBalancingBinarySearchTree::GetRootKey() const {
  if (root_ == nullptr) return "";
  return root_->key;
}

/**
 * @brief Deletes expired elements from the self-balancing binary search tree.
 */
void SelfBalancingBinarySearchTree::DeleteExpiredElements() {
  std::vector<Key> vec_keys;
  std::vector<Value> vec_values;
  InOrderTraversal(root_, vec_keys, vec_values);
  for (auto i = 0u; i < vec_keys.size(); ++i) {
    if (vec_values[i].TTL() == 0u) {
      root_ = DeletHelper(std::move(root_), vec_keys[i]);
    }
  }
}
}  // namespace s21