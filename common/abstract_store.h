#ifndef TRANSACTIONS_ABSTRACT_STORE_H_
#define TRANSACTIONS_ABSTRACT_STORE_H_

#include <optional>
#include <string>
#include <vector>

#include "value.h"

namespace s21 {

using Key = std::string;

/**
 * @brief An abstract class defining a key-value store interface.
 *
 * This class defines a set of operations that a key-value store should support.
 * It is an abstract class and cannot be instantiated directly. Instead,
 * specific implementations of the key-value store can inherit from this class
 * and provide their own implementation of its pure virtual functions.
 */
class AbstractStore {
 public:
  virtual ~AbstractStore() = default;

  virtual bool Set(const Key& key, const Value& value) = 0;
  virtual std::optional<Value> Get(const Key& key) const = 0;
  virtual bool Exists(const Key& key) const = 0;
  virtual bool Del(const Key& key) = 0;
  virtual std::vector<Key> Keys() const = 0;
  virtual std::vector<Value> ShowAll() const = 0;
  virtual bool Update(const Key& key, const std::string& value) = 0;
  virtual bool Rename(const Key& old_key, const Key& new_key) = 0;
  virtual std::size_t Upload(const std::string& file_name) = 0;
  virtual std::size_t Export(const std::string& file_name) const = 0;
  virtual std::optional<std::size_t> TTL(const Key& key) const = 0;
  virtual std::vector<Key> Find(const std::string& value) const = 0;
  virtual void DeleteExpiredElements() = 0;
};

}  // namespace s21

#endif  // TRANSACTIONS_ABSTRACT_STORE_H_
