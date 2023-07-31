#include "hash_table.h"

namespace s21 {

HashTable::HashTable(std::size_t capacity)
    : capacity_(capacity), size_(0), table_(capacity, nullptr) {
  if (capacity_ < kTableCapacity) {
    capacity_ = kTableCapacity;
    table_.resize(capacity_, nullptr);
  }
}

bool HashTable::Set(const Key& key, const Value& value) {
  std::shared_ptr<Node> node = FindNode(key);
  if (node != nullptr) {
    return false;
  }
  InsertNode(key, value);
  return true;
}

std::optional<Value> HashTable::Get(const Key& key) const {
  std::shared_ptr<Node> node = FindNode(key);
  if (node != nullptr) {
    return node->value;
  }
  return std::nullopt;
}

bool HashTable::Exists(const Key& key) const {
  return FindNode(key) != nullptr;
}

bool HashTable::Del(const Key& key) {
  std::shared_ptr<Node> node = FindNode(key);
  if (node == nullptr) {
    return false;
  }
  DeleteNode(key);
  return true;
}

bool HashTable::Update(const Key& key, const std::string& new_value) {
  std::shared_ptr<Node> node = FindNode(key);
  if (node != nullptr) {
    node->value.Update(new_value);
    return true;
  }
  return false;
}

std::vector<Key> HashTable::Keys() const {
  std::vector<Key> keys;
  for (const auto& node : table_) {
    std::shared_ptr<Node> current = node;
    while (current != nullptr) {
      keys.push_back(current->key);
      current = current->next;
    }
  }
  return keys;
}

bool HashTable::Rename(const Key& old_key, const Key& new_key) {
  if (!Exists(old_key) or Exists(new_key)) {
    return false;
  }
  std::shared_ptr<Node> node = FindNode(old_key);
  DeleteNode(old_key);
  InsertNode(new_key, node->value);
  return true;
}

std::optional<std::size_t> HashTable::TTL(const Key& key) const {
  std::shared_ptr<Node> node = FindNode(key);
  if (node != nullptr) {
    return node->value.TTL();
  }
  return std::nullopt;
}

std::vector<Key> HashTable::Find(const std::string& value) const {
  std::vector<Key> keys;
  for (const auto& node : table_) {
    std::shared_ptr<Node> current = node;
    while (current != nullptr) {
      if (current->value.Match(value)) {
        keys.push_back(current->key);
      }
      current = current->next;
    }
  }
  return keys;
}

std::vector<Value> HashTable::ShowAll() const {
  std::vector<Value> values;
  for (const auto& node : table_) {
    std::shared_ptr<Node> current = node;
    while (current != nullptr) {
      values.push_back(current->value);
      current = current->next;
    }
  }
  return values;
}

std::size_t HashTable::Upload(const std::string& file_path) {
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

std::size_t HashTable::Export(const std::string& file_path) const {
  std::ofstream file(file_path);
  if (!file.is_open()) {
    throw std::invalid_argument("Invalid file_path");
  }

  std::size_t count = 0u;
  for (const auto& node : table_) {
    std::shared_ptr<Node> current = node;
    while (current != nullptr) {
      file << current->key << " " << current->value.ToQuotedString() << "\n";
      ++count;
      current = current->next;
    }
  }

  file.close();
  return count;
}

void HashTable::DeleteExpiredElements() {
  std::vector<Key> keys = Keys();
  std::vector<Value> values = ShowAll();
  for (auto i = 0u; i < keys.size(); ++i) {
    if (values[i].TTL() == 0u) {
      Del(keys[i]);
    }
  }
}

std::size_t HashTable::HashFunction(const Key& key) const {
  std::hash<Key> hash_func;
  return hash_func(key) % capacity_;
}

std::shared_ptr<HashTable::Node> HashTable::FindNode(const Key& key) const {
  std::size_t index = HashFunction(key);
  std::shared_ptr<Node> current = table_[index];
  while (current != nullptr) {
    if (current->key == key) {
      return current;
    }
    current = current->next;
  }
  return nullptr;
}

void HashTable::InsertNode(const Key& key, const Value& value) {
  if (static_cast<float>(size_ + 1) / capacity_ > kMaxLoadFactor) {
    Resize(capacity_ * 2);
  }
  std::size_t index = HashFunction(key);
  std::shared_ptr<Node> new_node = std::make_shared<Node>(key, value);

  if (table_[index] == nullptr) {
    table_[index] = new_node;
  } else {
    std::shared_ptr<Node> last_node = table_[index];
    while (last_node->next != nullptr) {
      last_node = last_node->next;
    }

    last_node->next = new_node;
  }
  ++size_;
}

void HashTable::DeleteNode(const Key& key) {
  std::size_t index = HashFunction(key);
  std::shared_ptr<Node> node = table_[index];
  std::shared_ptr<Node> prev_node = nullptr;

  while (node != nullptr) {
    if (node->key == key) {
      if (prev_node != nullptr) {
        prev_node->next = node->next;
      } else {
        table_[index] = node->next;
      }
      break;
    }
    prev_node = node;
    node = node->next;
  }
  --size_;
}

void HashTable::Resize(std::size_t new_size) {
  if (new_size < kTableCapacity) new_size = kTableCapacity;
  if (new_size == capacity_) return;

  std::vector<std::shared_ptr<Node>> new_table(new_size, nullptr);
  for (std::size_t i = 0; i < capacity_; ++i) {
    std::shared_ptr<Node> node = table_[i];
    while (node != nullptr) {
      std::size_t new_index = std::hash<Key>{}(node->key) % new_size;
      std::shared_ptr<Node> next_node = node->next;
      node->next = new_table[new_index];
      new_table[new_index] = node;
      node = next_node;
    }
  }

  capacity_ = new_size;
  table_ = std::move(new_table);
}

}  // namespace s21
