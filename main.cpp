#include "lib/termcolor.hpp"
#include "lib/cxxopts.hpp"

#include "print.hpp"
#include "utils.hpp"
#include "time_utils.hpp"
#include "settings.hpp"

using namespace std;

cxxopts::Options options("Kanttiinit CLI", "Kanttiinit.fi command-line interface.");

void show_menus(string query, cxxopts::ParseResult args) {
  string lang = Settings::get("lang", "fi");
  auto restaurants = get("restaurants?" + query + "&lang=" + lang);
  tm date = TimeUtils::parse_day(args["day"].as<string>());
  string day = TimeUtils::format(date, "%Y-%m-%d", 11);
  bool is_today = day == TimeUtils::format_now("%Y-%m-%d", 11);
  string filter = args.count("filter") ? to_lower_case(args["filter"].as<string>()) : "";
  int n_restaurants = args.count("number") ? args["number"].as<int>() : -1;

  // form string of restaurant IDs and fetch menus for them
  string restaurant_ids = accumulate(restaurants.begin(), restaurants.end(), string(""), [](string list, json::value_type restaurant) {
    int id = restaurant["id"];
    return list + to_string(id) + ",";
  });
  auto menus = get("menus?restaurants=" + restaurant_ids + "&days=" + day + "&lang=" + lang);
  
  // sort restaurants if restaurants are queried by location
  // (in that case they are already sorted by distance in the HTTP response)
  if (query.find("location") == string::npos) {
    sort(restaurants.begin(), restaurants.end(), [](json a, json b) {
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
      string h = opening_hours;
      opened = TimeUtils::is_open(h);
    }

    if (!opened && args.count("hide-closed")) {
      continue;
    }

    int id = restaurant["id"];
    auto days = menus[to_string(id)];
    auto courses = days[day];
    string name = restaurant["name"];
    string address = restaurant["address"];
    string url = restaurant["url"];
    Print::bold(name + " ");
    
    if (opening_hours.is_string()) {
      if (!is_today) {
        Print::basic(opening_hours);
      } else if (opened) {
        Print::green(opening_hours);
        Print::dimmed(" closes in " + TimeUtils::time_until(opening_hours));
      } else {
        Print::dimmed(opening_hours);
      }
    } else {
      Print::basic("closed");
    }
    Print::basic("\n");

    if (restaurant["distance"].is_number()) {
      int distance = restaurant["distance"];
      Print::dimmed(to_string(distance) + " meters away\n");
    }

    if (args.count("address")) {
      Print::dimmed(address + "\n");
    }

    if (args.count("url")) {
      Print::dimmed(url + "\n");
    }

    for (auto& course : courses) {
      string title = course["title"];

      // skip this course if a filter keyword is used, and it isn't present in the course title
      if (args.count("filter")) {
        if (to_lower_case(title).find(filter) == string::npos) {
          continue;
        }
      }
      
      json properties = course["properties"];
      string prop_string = "";
      if (properties.size()) {
        string last_prop = *(properties.end() - 1);
        prop_string = accumulate(properties.begin(), properties.end(), string(""), [last_prop](string list, string p) {
          return list + p + (p == last_prop ? "" : ", ");
        });
      }
      Print::basic("◦ " + title + " ");
      Print::dimmed(prop_string + "\n");
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

  if (restaurant_counter == 0) {
    Print::red("No restaurants matched your query.\n");
  }
}

void process_args(cxxopts::ParseResult args) {
  if (args.count("set-lang")) {
    auto lang = args["set-lang"].as<string>();
    if (lang == "fi" || lang == "en") {
      Settings::set("lang", lang);
    }
  }

  if (args.count("help")) {
    Print::basic(options.help() +
      "\nGet all restaurants in a specific area:\n"
      "kanttiinit -q otaniemi\n\n"
      "Get restaurants by restaurant name:\n"
      "kanttiinit -q unicafe\n\n"
      "Get restaurants close to a location:\n"
      "kanttiinit -g Otakaari 8\n\n"
      "Only list courses that match a certain keyword:\n"
      "kanttiinit -q töölö -f salad\n\n"
      "See menus for tomorrow:\n"
      "kanttiinit -q alvari -d 1\n"
    );
  } else if (args.count("version")) {
    Print::basic("1.0.0\n");
  } else if (args.count("query") && args["query"].as<string>().length() > 0) {
    show_menus("query=" + args["query"].as<string>(), args);
  } else if (args.count("geo")) {
    auto location_query = args["geo"].as<string>();
    auto location = get_location(location_query);
    if (location.first) {
      show_menus("location=" + to_string(location.second.latitude) + "," + to_string(location.second.longitude), args);
    } else {
      Print::red("Could not resolve location: " + location_query + "\n");
    }
  } else {
    Print::basic("Use either the -q or -g option to query restaurants. Display help with --help.\n");
  }
}

int main(int argc, char *argv[]) {
  options.add_options()
    ("q,query", "Search restaurants by restaurant or area name.", cxxopts::value<string>())
    ("g,geo", "Search restaurants by location.", cxxopts::value<string>())
    ("d,day", "Specify day.", cxxopts::value<string>()->default_value("0"))
    ("f,filter", "Filter courses by keyword.", cxxopts::value<string>())
    ("n,number", "Show only n restaurants.", cxxopts::value<int>())
    ("v,version", "Display version.")
    ("a,address", "Show restaurant address.")
    ("u,url", "Show restaurant URL.")
    ("h,hide-closed", "Hide closed restaurants.")
    ("set-lang", "Save the preferred language (fi or en).", cxxopts::value<string>())
    ("help", "Display help.");

  try {
    auto args = options.parse(argc, argv);
    process_args(args);
  } catch (cxxopts::argument_incorrect_type e) {
    Print::red(string(e.what()) + "\n");
  } catch (cxxopts::option_not_exists_exception e) {
    Print::red("Unknown option provided.\n");
  }
}
