#ifndef TRANSACTIONS_CONSOLE_CONSOLE_H_
#define TRANSACTIONS_CONSOLE_CONSOLE_H_

#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <vector>

#include "../avl_tree/self_balancing_binary_search_tree.h"
#include "../b_plus_tree/b_plus_tree.h"
#include "../hash_table/hash_table.h"

namespace s21 {

enum class Color { kRed, kGreen, kBlue, kYellow, kGrey, kCyan, kMagenta, kEnd };
enum class Menu { kMain, kChooseStore, kResearch };

struct Item {
  std::string title = "Default";
  std::function<void()> item;
};

constexpr std::size_t kDegree = 10;

class Console {
 public:
  void Run();

 private:
  void ChooseStore();
  void EnterCommand();
  void RunResearch();
  void Set(const std::vector<std::string>& tokens);
  void Get(const std::vector<std::string>& tokens);
  void Exists(const std::vector<std::string>& tokens);
  void Del(const std::vector<std::string>& tokens);
  void Update(const std::vector<std::string>& tokens);
  void Keys(const std::vector<std::string>& tokens);
  void Rename(const std::vector<std::string>& tokens);
  void TTL(const std::vector<std::string>& tokens);
  void Find(const std::vector<std::string>& tokens);
  void ShowAll(const std::vector<std::string>& tokens);
  void Upload(const std::vector<std::string>& tokens);
  void Export(const std::vector<std::string>& tokens);
  void PrintHelp() const;
  void AddItem(Item item);
  int InputNumber(int items, Menu menu) const;
  void MainLoop();
  void ShowMenu(Menu menu) const;
  void MainMenu() const;
  void ChooseStoreMenu() const;
  void PrintMessage(const std::string& str, Color color) const;
  std::string Align(const std::string& str, std::size_t width) const;
  std::string GetColor(Color color) const;

 private:
  std::unique_ptr<AbstractStore> store_ = std::make_unique<BPlusTree>(kDegree);
  std::vector<Item> menu_;
  std::string type_ = "B+ tree";
};

}  // namespace s21

#endif  // TRANSACTIONS_CONSOLE_CONSOLE_H_
