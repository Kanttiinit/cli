#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "lib/json.hpp"
using json = nlohmann::json;

std::string exec(std::string cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
      result += buffer.data();
  }
  return result;
}

json get_json(std::string url) {
  auto raw_json = exec("curl -s --header 'User-Agent: Kanttiinit CLI' '" + url + "'");
  return json::parse(raw_json);
}

json get(std::string endpoint) {
  auto raw_json = exec("curl -sS 'https://kitchen.kanttiinit.fi/" + endpoint + "'");
  return json::parse(raw_json);
}

std::string join_json_array(json json_array, std::string delimit) {
  std::string output = "";
  for (json::iterator it = json_array.begin(); it != json_array.end(); ++it) {
    std::string value = *it;
    output += value;
    if (it + 1 != json_array.end()) {
      output += delimit;
    }
  }
  return output;
}

struct geo_location {
  double latitude;
  double longitude;
};

std::pair<bool, struct geo_location> get_location(std::string address) {
  auto response = get_json("http://maps.googleapis.com/maps/api/geocode/json?sensor=false&address=" + address);
  std::vector<json> results = response["results"];
  if (results.size()) {
    auto location = results[0]["geometry"]["location"];
    double lat = location["lat"];
    double lon = location["lng"];
    return std::make_pair(true, geo_location { lat, lon });
  }
  return std::make_pair(false, geo_location {});
}

std::string to_lower_case(std::string input) {
  std::locale loc;
  std::string output = "";
  for (std::string::size_type i = 0; i < input.length(); ++i)
    output += std::tolower(input[i],loc);
  return output;
}