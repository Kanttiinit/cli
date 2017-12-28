#include <iostream>
#include <ctime>

#include "lib/termcolor.hpp"
#include "lib/cxxopts.hpp"

#include "print.cpp"
#include "utils.cpp"
#include "time_utils.cpp"

cxxopts::Options options("Kanttiinit CLI", "Kanttiinit.fi command-line interface.");

void show_menus(std::string query, cxxopts::ParseResult args) {
  auto restaurants = get("restaurants?" + query);
  std::tm date = TimeUtils::parse_day(args["day"].as<std::string>());
  std::string day = TimeUtils::format(date, "%Y-%m-%d", 11);
  std::string filter = args.count("filter") ? to_lower_case(args["filter"].as<std::string>()) : "";
  int n_restaurants = args.count("number") ? args["number"].as<int>() : -1;

  // form string of restaurant IDs and fetch menus for them
  std::string restaurant_ids = "";
  for (auto& restaurant : restaurants) {
    int id = restaurant["id"];
    restaurant_ids += std::to_string(id) + ",";
  }
  auto menus = get("menus?restaurants=" + restaurant_ids + "days=" + day);
  
  // sort restaurants if restaurants are queried by location
  // (in that case they are already sorted by distance in the HTTP response)
  if (query.find("location") == std::string::npos) {
    std::sort(restaurants.begin(), restaurants.end(), [](json a, json b) {
      return a["name"] < b["name"];
    });
  }

  Print::basic("\n" + TimeUtils::format(date, "%A %e. of %B %Y", 30) + "\n\n");
  int restaurant_counter = 0;
  for (auto& restaurant : restaurants) {

    // determine if restaurant is opened, either by date or by time of day
    auto opening_hours = restaurant["openingHours"][TimeUtils::get_weekday(date)];
    bool opened = false;
    if (opening_hours.is_string()) {
      std::string h = opening_hours;
      opened = TimeUtils::is_open(h);
    }

    if (!opened && args.count("hide-closed")) {
      continue;
    }

    int id = restaurant["id"];
    auto days = menus[std::to_string(id)];
    auto courses = days[day];
    std::string name = restaurant["name"];
    std::string address = restaurant["address"];
    std::string url = restaurant["url"];
    Print::bold(name + " ");
    
    if (opening_hours.is_string()) {
      if (opened) {
        Print::green(opening_hours);
      } else {
        Print::dimmed(opening_hours);
      }
    } else {
      Print::basic("closed");
    }
    Print::basic("\n");

    if (restaurant["distance"].is_number_float()) {
      float distance = restaurant["distance"];
      Print::dimmed(std::to_string(distance) + " meters away\n");
    }

    if (args.count("address")) {
      Print::dimmed(address + "\n");
    }

    if (args.count("url")) {
      Print::dimmed(url + "\n");
    }

    for (auto& course : courses) {
      std::string title = course["title"];

      // skip this course if a filter keyword is used, and it isn't present in the course title
      if (args.count("filter")) {
        if (to_lower_case(title).find(filter) == std::string::npos) {
          continue;
        }
      }
      
      json properties = course["properties"];
      std::string prop_string = join_json_array(properties, ", ");
      std::cout << "◦ " << title << " " << termcolor::dark << prop_string << termcolor::reset << "\n";
    }
    if (courses.begin() == courses.end()) {
      Print::basic("No menu.\n");
    }
    Print::basic("\n");

    // stop if only a certain amount of restaurants is wanted, and that amount is reached
    restaurant_counter++;
    if (n_restaurants > 0 && restaurant_counter == n_restaurants) {
      break;
    }
  }
}

void process_args(cxxopts::ParseResult args) {
  if (args.count("help")) {
    Print::basic(options.help());
  } else if (args.count("version")) {
    Print::basic("You are running version 1.0.0\n");
  } else if (args.count("query") && args["query"].as<std::string>().length() > 0) {
    show_menus("query=" + args["query"].as<std::string>(), args);
  } else if (args.count("geo")) {
    auto location_query = args["geo"].as<std::string>();
    auto location = get_location(location_query);
    if (location.first) {
      show_menus("location=" + std::to_string(location.second.latitude) + "," + std::to_string(location.second.longitude), args);
    } else {
      Print::red("Could not resolve location: " + location_query + "\n");
    }
  }
}

int main(int argc, char *argv[]) {
  options.add_options()
    ("q,query", "Search restaurants by restaurant or area name.", cxxopts::value<std::string>())
    ("g,geo", "Search restaurants by location.", cxxopts::value<std::string>())
    ("d,day", "Specify day.", cxxopts::value<std::string>()->default_value("0"))
    ("f,filter", "Filter courses by keyword.", cxxopts::value<std::string>())
    ("n,number", "Show only n restaurants.", cxxopts::value<int>())
    ("v,version", "Display version.")
    ("a,address", "Show restaurant address.")
    ("u,url", "Show restaurant URL.")
    ("h,hide-closed", "Hide closed restaurants.")
    ("help", "Display help.");

  try {
    auto args = options.parse(argc, argv);
    process_args(args);
  } catch (cxxopts::argument_incorrect_type e) {
    Print::red(e.what());
  }
}
