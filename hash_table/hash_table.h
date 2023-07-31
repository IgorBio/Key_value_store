#ifndef TRANSACTIONS_HASH_TABLE_HASH_TABLE_H_
#define TRANSACTIONS_HASH_TABLE_HASH_TABLE_H_

#include <fstream>
#include <memory>

#include "../common/abstract_store.h"

namespace s21 {

/**
 * @brief In-memory key-value store based on a hash table.
 *
 * This class implements an in-memory key-value store using a hash table. It
 * provides methods to set and retrieve key-value pairs, check for existence,
 * delete records, update values, retrieve keys, rename keys, and perform
 * various other operations.
 */
class HashTable : public AbstractStore {
 public:
  explicit HashTable(std::size_t capacity = kTableCapacity);

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

 private:
  struct Node {
    Key key;
    Value value;
    std::shared_ptr<Node> next;

    Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
  };

  static constexpr std::size_t kTableCapacity = 256;
  static constexpr float kMaxLoadFactor = 0.75;

  std::size_t capacity_;
  std::size_t size_;
  std::vector<std::shared_ptr<Node>> table_;

  std::size_t HashFunction(const Key& key) const;
  std::shared_ptr<Node> FindNode(const Key& key) const;
  void InsertNode(const Key& key, const Value& value);
  void DeleteNode(const Key& key);
  void Resize(std::size_t new_size);
};

}  // namespace s21

#endif  // TRANSACTIONS_HASH_TABLE_HASH_TABLE_H_
