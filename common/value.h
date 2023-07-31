#ifndef TRANSACTIONS_VALUE_H_
#define TRANSACTIONS_VALUE_H_

#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>

namespace s21 {

/**
 * @brief Class representing a value stored in the key-value store.
 *
 * This class stores information about a students in the form of a last name,
 * first name, birth year, city, and number of coins. It also supports an
 * optional time-to-live (TTL) parameter, which specifies how long the value
 * should be stored in the key-value store before it is automatically deleted.
 */
class Value {
 public:
  enum TypeValidation { kDate, kCoin, kTTL };
  Value() = default;
  Value(const std::string &last_name, const std::string &first_name,
        const std::string &birth_year, const std::string &city,
        const std::string &coins,
        std::optional<std::string> ttl = std::nullopt);

  void Update(const std::string &value);
  std::optional<std::size_t> TTL() const;
  bool IsExpired() const;
  std::string ToQuotedString() const;
  std::string ToString() const;
  static Value FromString(std::string value);
  bool Match(const std::string &value) const;
  bool operator==(const Value &other) const;

 private:
  std::size_t CalculateElapsedTime() const;
  static std::string ValidateNumber(const std::string &input,
                                    const TypeValidation &type);

  static std::tuple<std::string, std::string, std::string, std::string,
                    std::string, std::optional<std::string>>
  ParseValueFields(std::istringstream &ss);

  std::string last_name_;
  std::string first_name_;
  std::string birth_year_;
  std::string city_;
  std::string coins_;
  std::optional<std::string> ttl_;
  std::chrono::time_point<std::chrono::system_clock> creation_time_;
};

}  // namespace s21

#endif  // TRANSACTIONS_VALUE_H_
