#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

namespace Settings {
  using list = map<string, string>;
  const string path = string(getenv("HOME")) + "/.kanttiinit";
  const string delimiter = "===";

  void set(string key, string value);
  list get_all();
  void set_all(list settings);
  pair<bool, string> get(string key);
  string get(string key, string default_value);
}

void Settings::set(string key, string value) {
  Settings::list all = Settings::get_all();
  if (all.find(key) != all.end()) {
    all.erase(key);
  }
  all.emplace(key, value);
  Settings::set_all(all);
}

pair<bool, string> Settings::get(string key) {
  Settings::list all = Settings::get_all();
  if (all.find(key) != all.end()) {
    return make_pair(true, all[key]);
  }
  return make_pair(false, "");
}

string Settings::get(string key, string default_value) {
  auto s = Settings::get(key);
  return s.first ? s.second : default_value;
}

Settings::list Settings::get_all() {
  ifstream file(Settings::path);
  Settings::list list;
  if (file.is_open()) {
    string line;
    while (getline(file, line)) {
      size_t pos = line.find(Settings::delimiter);
      if (pos != string::npos) {
        list.emplace(line.substr(0, pos), line.substr(pos + 3, line.length()));
      }
    }
    file.close();
  }
  return list;
}

void Settings::set_all(Settings::list settings) {
  ofstream file(Settings::path);
  for (auto& s : settings) {
    file << s.first << Settings::delimiter << s.second << endl;
  }
  file.close();
}
