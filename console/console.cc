#include "console.h"

namespace s21 {
constexpr size_t kWidth = 60u;

void Console::AddItem(Item item) { menu_.push_back(item); }

void Console::Run() {
  system("clear");
  AddItem({"Choose key-value store", [this] { ChooseStore(); }});
  AddItem({"Enter command", [this] { EnterCommand(); }});
  AddItem({"Run research", [this] { RunResearch(); }});
  AddItem({"Print help", [this] { PrintHelp(); }});
  MainLoop();
}

void Console::ChooseStore() {
  std::string text;
  system("clear");
  ChooseStoreMenu();
  int choice = InputNumber(3, Menu::kChooseStore);
  system("clear");

  if (choice == 1) {
    store_ = std::make_unique<HashTable>();
    type_ = "Hash Table";
    text = "Switched to hash table store.";
  } else if (choice == 2) {
    store_ = std::make_unique<SelfBalancingBinarySearchTree>();
    type_ = "AVL tree";
    text = "Switched to self-balancing binary search tree store.";
  } else if (choice == 3) {
    store_ = std::make_unique<BPlusTree>(kDegree);
    type_ = "B+ tree";
    text = "Switched to B+ tree store.";
  }
  if (!text.empty()) {
    PrintMessage(text, Color::kMagenta);
    std::cout << " 🚀\n";
  }
}

void Console::EnterCommand() {
  std::string command;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  system("clear");
  PrintMessage("ENTER COMMAND", Color::kCyan);
  std::cout << "\n" << Align("Enter Q to return", kWidth) << "\n";

  while (true) {
    std::cout << std::endl;
    std::getline(std::cin, command);

    std::istringstream iss(command);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{}};

    if (tokens.empty()) {
      continue;
    }

    const std::string& cmd = tokens[0];
    store_->DeleteExpiredElements();
    if (cmd == "SET") {
      Set(tokens);
    } else if (cmd == "GET") {
      Get(tokens);
    } else if (cmd == "EXISTS") {
      Exists(tokens);
    } else if (cmd == "DEL") {
      Del(tokens);
    } else if (cmd == "UPDATE") {
      Update(tokens);
    } else if (cmd == "KEYS") {
      Keys(tokens);
    } else if (cmd == "RENAME") {
      Rename(tokens);
    } else if (cmd == "TTL") {
      TTL(tokens);
    } else if (cmd == "FIND") {
      Find(tokens);
    } else if (cmd == "SHOWALL") {
      ShowAll(tokens);
    } else if (cmd == "UPLOAD") {
      Upload(tokens);
    } else if (cmd == "EXPORT") {
      Export(tokens);
    } else if (cmd == "Q") {
      break;
    } else {
      std::cout << "Invalid command. Please try again.\n";
    }
  }
  system("clear");
}

void Console::Set(const std::vector<std::string>& tokens) {
  if (tokens.size() == 7 or (tokens.size() == 9 and tokens[7] == "EX")) {
    try {
      std::string key = tokens[1];
      std::string last_name = tokens[2];
      std::string first_name = tokens[3];
      std::string birth_year = tokens[4];
      std::string city = tokens[5];
      std::string coins = tokens[6];
      std::optional<std::string> ttl =
          tokens.size() == 9 ? std::make_optional(tokens[8]) : std::nullopt;

      Value value(last_name, first_name, birth_year, city, coins, ttl);
      if (store_->Set(key, value)) {
        std::cout << "> OK\n";
      } else {
        std::cout << "> ERROR: unable to set value for key \"" + key + "\"\n";
      }
    } catch (const std::exception& e) {
      std::cout << "> ";
      std::cout << e.what() << "\n";
    }
  } else {
    std::cout << "> ERROR: invalid SET command\n";
  }
}

void Console::Get(const std::vector<std::string>& tokens) {
  if (tokens.size() == 2) {
    std::string key = tokens[1];
    std::optional<Value> value = store_->Get(key);
    if (value.has_value()) {
      std::cout << "> " << value->ToString() << "\n";
    } else {
      std::cout << "> (null)\n";
    }
  } else {
    std::cout << "> ERROR: invalid GET command\n";
  }
}

void Console::Exists(const std::vector<std::string>& tokens) {
  if (tokens.size() == 2) {
    std::string key = tokens[1];
    bool exists = store_->Exists(key);
    std::string ans = exists ? "true" : "false";
    std::cout << "> " << ans << "\n";
  } else {
    std::cout << "> ERROR: invalid EXISTS command\n";
  }
}

void Console::Del(const std::vector<std::string>& tokens) {
  if (tokens.size() == 2) {
    std::string key = tokens[1];
    bool deleted = store_->Del(key);
    std::string ans = deleted ? "true" : "false";
    std::cout << "> " << ans << "\n";
  } else {
    std::cout << "> ERROR: invalid DEL command\n";
  }
}

void Console::Update(const std::vector<std::string>& tokens) {
  if (tokens.size() == 7) {
    try {
      std::string key = tokens[1];
      std::string last_name = tokens[2];
      std::string first_name = tokens[3];
      std::string birth_year = tokens[4];
      std::string city = tokens[5];
      std::string coins = tokens[6];

      Value value(last_name, first_name, birth_year, city, coins);
      if (store_->Update(key, value.ToString())) {
        std::cout << "> OK\n";
      } else {
        std::cout << "> ERROR: unable to update value for key \"" + key +
                         "\"\n";
      }
    } catch (const std::exception& e) {
      std::cout << "> ";
      std::cout << e.what() << "\n";
    }
  } else {
    std::cout << "> ERROR: invalid UPDATE command\n";
  }
}

void Console::Keys(const std::vector<std::string>& tokens) {
  if (tokens.size() == 1) {
    std::vector<std::string> keys = store_->Keys();
    if (keys.empty()) {
      std::cout << "> (null)\n";
    } else {
      std::size_t idx = 1;
      for (auto& key : keys) {
        std::cout << "> " << idx << ") " << key << std::endl;
        ++idx;
      }
    }
  } else {
    std::cout << "> ERROR: invalid KEYS command\n";
  }
}

void Console::Rename(const std::vector<std::string>& tokens) {
  if (tokens.size() == 3) {
    std::string old_key = tokens[1];
    std::string new_key = tokens[2];
    if (store_->Rename(old_key, new_key)) {
      std::cout << "> OK\n";
    } else {
      std::cout << "> ERROR: unable to rename key \"" + old_key + "\"\n";
    }
  } else {
    std::cout << "> ERROR: invalid RENAME command\n";
  }
}

void Console::TTL(const std::vector<std::string>& tokens) {
  if (tokens.size() == 2) {
    std::string key = tokens[1];
    std::optional<std::size_t> ttl = store_->TTL(key);
    if (ttl.has_value()) {
      std::cout << "> " << ttl.value() << "\n";
    } else {
      std::cout << "> (null)\n";
    }
  } else {
    std::cout << "> ERROR: invalid TTL command\n";
  }
}

void Console::Find(const std::vector<std::string>& tokens) {
  if (tokens.size() == 6) {
    try {
      std::string last_name = tokens[1];
      std::string first_name = tokens[2];
      std::string birth_year = tokens[3];
      std::string city = tokens[4];
      std::string coins = tokens[5];

      std::string value = last_name + " " + first_name + " " + birth_year +
                          " " + city + " " + coins;
      std::vector<std::string> keys = store_->Find(value);
      if (keys.empty()) {
        std::cout << "> (null)\n";
      } else {
        std::size_t idx = 1;
        for (auto& key : keys) {
          std::cout << "> " << idx << ") " << key << std::endl;
          ++idx;
        }
      }
    } catch (const std::exception& e) {
      std::cout << "> ";
      std::cout << e.what() << "\n";
    }
  } else {
    std::cout << "> ERROR: invalid FIND command\n";
  }
}

void Console::ShowAll(const std::vector<std::string>& tokens) {
  if (tokens.size() == 1) {
    std::vector<Value> values = store_->ShowAll();
    if (values.empty()) {
      std::cout << "> (null)\n";
    } else {
      std::size_t idx = 1;
      std::cout << "> № |" << Align("Last name", 15) << "|"
                << Align("First name", 14) << "|" << Align("Year", 6) << "|"
                << Align("City", 20) << "|" << Align("Coins", 7) << "|\n";
      for (auto& value : values) {
        std::istringstream ss(value.ToQuotedString());
        std::string last_name, first_name, birth_year, city, coins;
        ss >> last_name >> first_name >> birth_year >> city >> coins;
        std::cout << "> " << idx << "  " << Align(last_name, 16)
                  << Align(first_name, 15) << Align(birth_year, 6)
                  << Align(city, 22) << Align(coins, 7) << std::endl;
        ++idx;
      }
    }
  } else {
    std::cout << "> ERROR: invalid SHOWALL command\n";
  }
}

void Console::Upload(const std::vector<std::string>& tokens) {
  if (tokens.size() == 2) {
    try {
      std::string file_path = tokens[1];
      std::size_t count = store_->Upload(file_path);
      std::cout << "> OK " << count << "\n";
    } catch (const std::exception& e) {
      std::cout << "> ";
      std::cout << e.what() << "\n";
    }
  } else {
    std::cout << "> ERROR: invalid UPLOAD command\n";
  }
}

void Console::Export(const std::vector<std::string>& tokens) {
  if (tokens.size() == 2) {
    try {
      std::string file_path = tokens[1];
      std::size_t count = store_->Export(file_path);
      std::cout << "> OK " << count << "\n";
    } catch (const std::exception& e) {
      std::cout << "> ";
      std::cout << e.what() << "\n";
    }
  } else {
    std::cout << "> ERROR: invalid EXPORT command\n";
  }
}

void Console::PrintHelp() const {
  system("clear");
  std::cout
      << "Available commands and syntax:\n"
         "\tSET\t: SET <key> <Last name> <First name> <Year of birth> "
         "<City> <Number of coins> EX <Seconds>\n"
         "\t\t- Adds a key-value pair to the storage.\n"
         "\tGET\t: GET <key>\n"
         "\t\t- Retrieves the value associated with the key.\n"
         "\tEXISTS\t: EXISTS <key>\n"
         "\t\t- Checks if a record with the given key exists.\n"
         "\tDEL\t: DEL <key>\n"
         "\t\t- Deletes the key and its corresponding value.\n"
         "\tUPDATE\t: UPDATE <key> <Last name> <First name> <Year of "
         "birth> <City> <Number of coins>\n"
         "\t\t- Updates the value associated with the key. Use '-' for "
         "fields that should not be changed.\n"
         "\tKEYS\t: KEYS\n"
         "\t\t- Returns all the keys in the store.\n"
         "\tRENAME\t: RENAME <old_key> <new_key>\n"
         "\t\t- Renames keys.\n"
         "\tTTL\t: TTL <key>\n"
         "\t\t- Shows the remaining time to live for a temporal value.\n"
         "\tFIND\t: FIND <Last name> <First name> <Year of birth> <City> "
         "<Number of coins>\n"
         "\t\t- Restores the keys according to the given value. Use '-' for "
         "fields that should not participate in the search.\n"
         "\tSHOWALL\t: SHOWALL\n"
         "\t\t- Prints all the values in the key-value store.\n"
         "\tUPLOAD\t: UPLOAD <file_path>\n"
         "\t\t- Uploads data from a file.\n"
         "\tEXPORT\t: EXPORT <file_path>\n"
         "\t\t- Exports the data in the key-value store to a file.\n"
      << std::endl;
}

void Console::RunResearch() {
  std::cout << "Enter the number of items in the store (1 - 1M): ";
  int items_cnt = InputNumber(1e6, Menu::kResearch);

  std::cout << "Enter the number of iterations for each operation (1 - 1k): ";
  int iter_cnt = InputNumber(1e3, Menu::kResearch);

  std::vector<std::string> keys(items_cnt);
  std::generate(keys.begin(), keys.end(),
                [n = 0]() mutable { return "key" + std::to_string(n++); });

  // Generate random data for the store
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> years_dst(1950, 2023);
  std::uniform_int_distribution<> coins_dst(1, 1000);
  std::vector<Value> values(items_cnt);
  for (int i = 0; i < items_cnt; ++i) {
    std::string last_name = "Last" + std::to_string(i);
    std::string first_name = "First" + std::to_string(i);
    std::string birth_year = std::to_string(years_dst(gen));
    std::string city = "City" + std::to_string(i);
    std::string coins = std::to_string(coins_dst(gen));

    values[i] = Value(last_name, first_name, birth_year, city, coins);
  }

  // Measure time for adding an item
  {
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iter_cnt; ++i) {
      std::size_t index = std::rand() % items_cnt;
      store_->Set(keys[index], values[index]);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Average time for adding an item: "
              << duration.count() / iter_cnt << " μs\n";
  }

  // Measure time for getting an arbitrary item
  {
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iter_cnt; ++i) {
      std::size_t index = std::rand() % items_cnt;
      store_->Get(keys[index]);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Average time for getting an arbitrary item: "
              << duration.count() / iter_cnt << " μs\n";
  }

  // Measure time for getting a list of all elements
  {
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iter_cnt; ++i) {
      store_->Keys();
    }
    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Average time for getting a list of all elements: "
              << duration.count() / iter_cnt << " μs\n";
  }

  // Measure time for finding the item key by value
  {
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iter_cnt; ++i) {
      std::size_t index = std::rand() % items_cnt;
      store_->Find(values[index].ToString());
    }
    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Average time for finding the item key by value: "
              << duration.count() / iter_cnt << " μs\n";
  }

  // Measure time for deleting an item
  {
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iter_cnt; ++i) {
      std::size_t index = std::rand() % items_cnt;
      store_->Del(keys[index]);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Average time for deleting an item: "
              << duration.count() / iter_cnt << " μs\n";
  }
}

void Console::ShowMenu(Menu menu) const {
  if (menu == Menu::kMain) MainMenu();
  if (menu == Menu::kChooseStore) ChooseStoreMenu();
}

void Console::MainMenu() const {
  PrintMessage("TRANSACTIONS", Color::kCyan);
  std::cout << "\n";
  std::cout << Align("Selected store: " + type_, kWidth);
  std::cout << "\n";
  PrintMessage("CHOOSE ITEM", Color::kCyan);
  std::cout << "\n\n";
  for (size_t i = 0; i < menu_.size(); ++i) {
    std::cout << "     " << i + 1 << ". " << menu_[i].title << std::endl;
  }
  std::cout << "     0. Exit\n\n";
  PrintMessage(" ", Color::kCyan);
  std::cout << "\n\n> ";
}

void Console::ChooseStoreMenu() const {
  PrintMessage("Choose key-value store:", Color::kCyan);
  std::cout << "\n\n";
  std::cout << "    1. Hash table\n";
  std::cout << "    2. Self-balancing binary search tree\n";
  std::cout << "    3. B+ tree\n";
  std::cout << "    0. Back to menu\n\n";
  PrintMessage(" ", Color::kCyan);
  std::cout << "\n\n> ";
}

void Console::MainLoop() {
  while (true) {
    ShowMenu(Menu::kMain);
    int choice = InputNumber(4, Menu::kMain) - 1;
    if (choice == -1) break;
    try {
      menu_[choice].item();
    } catch (const std::exception& e) {
      system("clear");
      PrintMessage(e.what(), Color::kRed);
      std::cout << " 🚨\n";
    }
    std::cout << std::endl;
  }
  system("clear");
  PrintMessage("HOPE YOU ENJOY!👋", Color::kCyan);
  std::cout << "\n\n";
}

int Console::InputNumber(int items, Menu menu) const {
  int input;
  while (true) {
    std::cin >> input;
    if (std::cin.fail() or input < 0 or input > items) {
      if (menu != Menu::kResearch) {
        system("clear");
      }
      PrintMessage("WRONG INPUT!", Color::kRed);
      std::cout << "\n\n";
      ShowMenu(menu);
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      return input;
    }
  }
}

void Console::PrintMessage(const std::string& str, Color color) const {
  std::cout << GetColor(color) << Align(str, kWidth) << GetColor(Color::kEnd);
}

std::string Console::Align(const std::string& str, std::size_t width) const {
  std::string aligned;
  std::string addition((width - str.size()) / 2, ' ');
  aligned.append(addition);
  aligned.append(str);
  aligned.append(addition);
  while (aligned.size() < width) aligned.append(" ");
  return aligned;
}

std::string Console::GetColor(Color color) const {
  switch (color) {
    case Color::kRed:
      return "\u001b[41;1m";
    case Color::kGreen:
      return "\u001b[42;1m";
    case Color::kYellow:
      return "\u001b[43;1m";
    case Color::kBlue:
      return "\u001b[44;1m";
    case Color::kMagenta:
      return "\u001b[45;1m";
    case Color::kCyan:
      return "\u001b[46;1m";
    case Color::kGrey:
      return "\u001b[47;1m";
    case Color::kEnd:
      return "\u001b[0m";
    default:
      return "";
  }
}

}  // namespace s21
