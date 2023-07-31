#include "b_plus_tree.h"

namespace s21 {

/**
 * @brief Constructs a BPlusTree object with the specified degree.
 *
 * @param degree The degree of the B+ tree. Determines the maximum number of
 * children for each internal node.
 */
BPlusTree::BPlusTree(std::size_t degree)
    : root_(std::make_shared<BPlusNode>(BPlusNode::NodeType::kLeaf)),
      leaf_(root_),
      degree_(degree) {}

/**
 * @brief Sets the value for the specified key in the key-value store.
 *
 * @param key The key to set.
 * @param value The value associated with the key.
 * @return True if the key-value pair is successfully set, false otherwise.
 */
bool BPlusTree::Set(const Key& key, const Value& value) {
  NodePtr leaf = FindLeaf(root_, key);

  if (leaf->Exists(key)) {
    return false;
  }
  leaf->Insert(key, value);

  if (leaf->Size() == degree_) {
    NodePtr new_leaf = leaf->Split();
    Expand(leaf, new_leaf, new_leaf->GetKeys().front());
  }

  return true;
}

/**
 * @brief Retrieves the value associated with the specified key from the
 * key-value store.
 *
 * @param key The key to retrieve.
 * @return An optional containing the value if the key is found, or an empty
 * optional otherwise.
 */
std::optional<Value> BPlusTree::Get(const Key& key) const {
  NodePtr leaf = FindLeaf(root_, key);
  if (leaf->Exists(key)) {
    return leaf->GetValue(key);
  }
  return std::nullopt;
}

/**
 * @brief Checks if a record with the given key exists in the key-value store.
 *
 * @param key The key to check.
 * @return True if the key exists, false otherwise.
 */
bool BPlusTree::Exists(const Key& key) const {
  NodePtr leaf = FindLeaf(root_, key);
  return leaf->Exists(key);
}

/**
 * @brief Deletes the record with the specified key from the key-value store.
 *
 * @param key The key to delete.
 * @return True if the record is successfully deleted, false otherwise.
 */
bool BPlusTree::Del(const Key& key) {
  NodePtr leaf = FindLeaf(root_, key);
  if (!leaf->Exists(key)) {
    return false;
  }

  leaf->Remove(key);
  Reduce(leaf);

  return true;
}

/**
 * @brief Updates the value associated with the specified key in the key-value
 * store.
 *
 * @param key The key to update.
 * @param new_value The new value to set.
 * @return True if the value is successfully updated, false otherwise.
 */
bool BPlusTree::Update(const Key& key, const std::string& new_value) {
  NodePtr leaf = FindLeaf(root_, key);
  if (!leaf->Exists(key)) {
    return false;
  }

  leaf->GetValue(key).Update(new_value);

  return true;
}

/**
 * @brief Retrieves all the keys stored in the B+ tree.
 *
 * @return A vector containing all the keys.
 */
std::vector<Key> BPlusTree::Keys() const {
  std::vector<Key> keys;
  NodePtr leaf = leaf_;

  while (leaf) {
    keys.insert(keys.end(), leaf->GetKeys().begin(), leaf->GetKeys().end());
    leaf = leaf->GetNext();
  }

  return keys;
}

/**
 * @brief Renames a key in the B+ tree.
 *
 * @param old_key The old key to rename.
 * @param new_key The new key to replace the old key.
 * @return True if the rename is successful, false otherwise.
 */
bool BPlusTree::Rename(const Key& old_key, const Key& new_key) {
  NodePtr leaf = FindLeaf(root_, old_key);
  if (!leaf->Exists(old_key) or leaf->Exists(new_key)) {
    return false;
  }

  Value value = leaf->GetValue(old_key);
  Del(old_key);
  return Set(new_key, value);
}

/**
 * @brief Retrieves the time-to-live (TTL) of a key in the B+ tree.
 *
 * @param key The key to retrieve TTL for.
 * @return An optional containing the TTL if available, or an empty optional
 * otherwise.
 */
std::optional<std::size_t> BPlusTree::TTL(const Key& key) const {
  NodePtr leaf = FindLeaf(root_, key);
  if (leaf->Exists(key)) {
    return leaf->GetValue(key).TTL();
  }

  return std::nullopt;
}

/**
 * @brief Finds all keys that have the given value in the B+ tree.
 *
 * @param value The value to search for.
 * @return A vector containing all the keys with the given value.
 */
std::vector<Key> BPlusTree::Find(const std::string& value) const {
  std::vector<Key> keys;
  NodePtr leaf = leaf_;

  while (leaf) {
    std::copy_if(leaf->GetKeys().begin(), leaf->GetKeys().end(),
                 std::back_inserter(keys), [&](const Key& key) {
                   return leaf->GetValue(key).Match(value);
                 });
    leaf = leaf->GetNext();
  }

  return keys;
}

/**
 * @brief Shows all the key-value pairs stored in the B+ tree.
 *
 * @return A vector containing all the values.
 */
std::vector<Value> BPlusTree::ShowAll() const {
  std::vector<Value> values;
  NodePtr leaf = leaf_;

  while (leaf) {
    values.insert(values.end(), leaf->GetValues().begin(),
                  leaf->GetValues().end());
    leaf = leaf->GetNext();
  }

  return values;
}

/**
 * @brief Uploads key-value pairs from a file and inserts them into the B+ tree.
 *
 * @param file_path The path to the file containing key-value pairs.
 * @return The number of key-value pairs uploaded successfully.
 */
std::size_t BPlusTree::Upload(const std::string& file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::invalid_argument("Invalid file_path");
  }

  Key key;
  std::string value;

  std::size_t count = 0u;
  while (file >> key) {
    std::getline(file >> std::ws, value);
    Set(key, Value::FromString(value));
    ++count;
  }
  file.close();
  return count;
}

/**
 * @brief Exports key-value pairs from the B+ tree to a file.
 *
 * @param file_path The path to the file to export key-value pairs to.
 * @return The number of key-value pairs exported successfully.
 */
std::size_t BPlusTree::Export(const std::string& file_path) const {
  std::ofstream file(file_path);
  if (!file.is_open()) {
    throw std::invalid_argument("Invalid file_path");
  }

  std::size_t count = 0u;
  NodePtr leaf = leaf_;

  while (leaf) {
    for (Key& key : leaf->GetKeys()) {
      file << key << " " << leaf->GetValue(key).ToQuotedString() << "\n";
      ++count;
    }
    leaf = leaf->GetNext();
  }

  file.close();
  return count;
}

/**
 * @brief Deletes expired elements from the b+ tree.
 */
void BPlusTree::DeleteExpiredElements() {
  std::vector<Key> keys = Keys();
  std::vector<Value> values = ShowAll();
  for (auto i = 0u; i < keys.size(); ++i) {
    if (values[i].TTL() == 0u) {
      Del(keys[i]);
    }
  }
}

/**
 * @brief Generates a DOT file representing the B+ tree.
 *
 * This method generates a DOT file that visually represents the structure of
 * the B+ tree. The DOT file can be rendered into an image using Graphviz or a
 * similar tool.
 *
 * @param file_name The name of the output DOT file.
 */
void BPlusTree::ToDot(const std::string& file_name) const {
  std::ofstream ofs;
  ofs.open(file_name + ".dot", std::ios::trunc);
  ofs << "digraph {\nnode [margin=0 fontsize=8 width=0.5 shape=box]\n";

  std::unordered_map<NodePtr, std::string> node_names;
  std::size_t idx = 0;
  std::function<void(NodePtr)> name_nodes = [&](NodePtr node) {
    if (!node) return;
    node_names[node] = "node" + std::to_string(idx++);
    for (NodePtr child : node->GetChildren()) name_nodes(child);
  };
  name_nodes(root_);

  std::function<void(NodePtr)> write_nodes = [&](NodePtr node) {
    if (!node) return;
    ofs << "\"" << node_names[node] << "\"";
    ofs << "[label=\"";
    for (Key el : node->GetKeys()) ofs << el << "\\n";
    ofs << "\", color=darkgreen, style=filled, fillcolor=palegreen, "
           "shape=circle]\n";

    if (node->IsLeaf()) {
      ofs << "{ rank=same;";
      NodePtr next_node = node->GetNext();
      if (next_node) {
        ofs << "\"" << node_names[node] << "\"";
        ofs << " -> ";
        ofs << "\"" << node_names[next_node] << "\"";
        ofs << " [style=dotted]\n";
      }
      ofs << "}\n";
    } else {
      for (NodePtr child : node->GetChildren()) {
        ofs << "\"" << node_names[node] << "\"";
        ofs << " -> ";
        ofs << "\"" << node_names[child] << "\"";
        ofs << "\n";
      }
    }

    for (NodePtr child : node->GetChildren()) write_nodes(child);
  };
  write_nodes(root_);

  ofs << "}";
}

/**
 * @brief Displays the contents of the B+ tree on the console.
 *
 * This method prints the contents of the B+ tree to the console, including all
 * keys. It is primarily used for debugging and testing purposes.
 */
void BPlusTree::Show() const {
  std::function<void(NodePtr, std::size_t)> print_node =
      [&](NodePtr node, std::size_t level) {
        if (!node) return;

        for (std::size_t i = 0; i < node->GetKeys().size(); ++i) {
          if (!node->IsLeaf()) {
            print_node(node->GetChildren()[i], level + 1);
          }
          for (std::size_t j = 0; j < level; ++j) {
            std::cout << "----";
          }
          std::cout << "[" << node->GetKeys()[i] << "]" << std::endl;

          if (!node->IsLeaf()) {
            for (std::size_t j = 0; j <= level; ++j) {
              std::cout << "----";
            }
            std::cout << "|" << std::endl;
          }
        }

        if (!node->IsLeaf()) {
          print_node(node->GetChildren().back(), level + 1);
        }
      };

  print_node(root_, 0);
}

/**
 * @brief Find the leaf node where the given key should be located.
 *
 * @param node The current node to start the search from.
 * @param key The key to search for.
 * @return The leaf node where the key should be located.
 */
BPlusTree::NodePtr BPlusTree::FindLeaf(BPlusTree::NodePtr node,
                                       const Key& key) const {
  if (node->IsLeaf()) return node;
  auto it =
      std::upper_bound(node->GetKeys().begin(), node->GetKeys().end(), key);
  auto idx = std::distance(node->GetKeys().begin(), it);
  return FindLeaf(node->GetChildren()[idx], key);
}

/**
 * @brief Split a full node into two and adjust the parent node accordingly.
 *
 * @param left The left node to split.
 * @param right The right node to split.
 * @param key The key that caused the split.
 */
void BPlusTree::Expand(NodePtr left, NodePtr right, const Key& key) {
  if (left == root_) {
    root_ = std::make_shared<BPlusNode>(BPlusNode::NodeType::kInternal);
    left->SetParent(root_);
    right->SetParent(root_);
    root_->AddChild(left);
    root_->AddChild(right);
    root_->AddKey(key);
    return;
  }

  NodePtr parent = left->GetParent().lock();
  parent->Insert(key, right);

  if (parent->Size() < degree_) return;

  NodePtr new_node = parent->Split();
  Expand(parent, new_node, new_node->GetKeys().front());
  new_node->DelKey(0u);
}

/**
 * @brief Get the adjacent nodes of the given node.
 *
 * @param node The node to find the adjacent nodes for.
 * @return A pair containing the left and right adjacent nodes.
 */
std::pair<BPlusTree::NodePtr, BPlusTree::NodePtr> BPlusTree::Adjacents(
    BPlusTree::NodePtr node) {
  NodePtr left = nullptr, right = nullptr;

  NodePtr parent = node->GetParent().lock();
  if (parent) {
    auto it =
        std::upper_bound(parent->GetKeys().begin(), parent->GetKeys().end(),
                         node->GetKeys().front());

    if (it != parent->GetKeys().begin()) {
      std::size_t idx = std::distance(parent->GetKeys().begin(), it - 1);
      left = parent->GetChildren()[idx];
    }

    if (it != parent->GetKeys().end()) {
      std::size_t idx = std::distance(parent->GetKeys().begin(), it + 1);
      right = parent->GetChildren()[idx];
    }
  }

  return {left, right};
}

/**
 * @brief Reduce the number of keys in a node by merging with adjacent nodes.
 *
 * @param node The node to reduce.
 */
void BPlusTree::Reduce(NodePtr node) {
  if (!node) return;

  if (node == root_ and !node->IsLeaf() and node->Size() == 0) {
    root_ = node->GetChildren().front();
    root_->GetParent().reset();
    return;
  }

  if (node == root_ or node->Size() >= (degree_ + !node->IsLeaf()) / 2) {
    return;
  }

  auto [left, right] = Adjacents(node);

  if (left and left->Size() > (degree_ + 1) / 2) {
    node->Redistribute(left, left);
  } else if (right and right->Size() > (degree_ + 1) / 2) {
    node->Redistribute(right, node);
  } else if (left) {
    if (!node->IsLeaf() and node->Size()) return;
    left->Merge(node);
    Reduce(left->GetParent().lock());
  } else if (right) {
    if (!node->IsLeaf() and node->Size()) return;
    node->Merge(right);
    Reduce(node->GetParent().lock());
  }
}

}  // namespace s21
