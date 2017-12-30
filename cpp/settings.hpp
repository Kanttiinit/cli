#include <string>
#include <map>
#include <iostream>
#include <fstream>

namespace Settings {
  using list = std::map<std::string, std::string>;
  const std::string path = std::string(getenv("HOME")) + "/.kanttiinit";
  const std::string delimiter = "===";

  void set(std::string key, std::string value);
  list get_all();
  void set_all(list settings);
  std::pair<bool, std::string> get(std::string key);
  std::string get(std::string key, std::string default_value);
}

void Settings::set(std::string key, std::string value) {
  Settings::list all = Settings::get_all();
  all.emplace(key, value);
  Settings::set_all(all);
}

std::pair<bool, std::string> Settings::get(std::string key) {
  Settings::list all = Settings::get_all();
  if (all.find(key) != all.end()) {
    return std::make_pair(true, all[key]);
  }
  return std::make_pair(false, "");
}

std::string Settings::get(std::string key, std::string default_value) {
  auto s = Settings::get(key);
  return s.first ? s.second : default_value;
}

Settings::list Settings::get_all() {
  std::ifstream file(Settings::path);
  Settings::list list;
  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      size_t pos = line.find(Settings::delimiter);
      if (pos != std::string::npos) {
        list.emplace(line.substr(0, pos), line.substr(pos + 3, line.length()));
      }
    }
    file.close();
  }
  return list;
}

void Settings::set_all(Settings::list settings) {
  std::ofstream file(Settings::path);
  for (auto& s : settings) {
    file << s.first << Settings::delimiter << s.second << std::endl;
  }
  file.close();
}
