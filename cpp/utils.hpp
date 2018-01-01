#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "lib/json.hpp"
using json = nlohmann::json;
using namespace std;

string exec(string cmd) {
  array<char, 128> buffer;
  string result;
  shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) throw runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
      result += buffer.data();
  }
  return result;
}

json get_json(string url) {
  auto raw_json = exec("curl -s --header 'User-Agent: Kanttiinit CLI' '" + url + "'");
  return json::parse(raw_json);
}

json get(string endpoint) {
  auto raw_json = exec("curl -sS 'https://kitchen.kanttiinit.fi/" + endpoint + "'");
  return json::parse(raw_json);
}

struct geo_location {
  double latitude;
  double longitude;
};

pair<bool, struct geo_location> get_location(string address) {
  auto response = get_json("http://maps.googleapis.com/maps/api/geocode/json?sensor=false&address=" + address);
  vector<json> results = response["results"];
  if (results.size()) {
    auto location = results[0]["geometry"]["location"];
    double lat = location["lat"];
    double lon = location["lng"];
    return make_pair(true, geo_location { lat, lon });
  }
  return make_pair(false, geo_location {});
}

string to_lower_case(string input) {
  locale loc;
  string output = "";
  for (string::size_type i = 0; i < input.length(); ++i) {
    output += tolower(input[i], loc);
  }
  return output;
}

// from https://stackoverflow.com/a/4654718/1810160
bool is_number(const string& s) {
  string::const_iterator it = s.begin();
  if (*it == '-') {
    it++;
  }
  while (it != s.end() && isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

#endif