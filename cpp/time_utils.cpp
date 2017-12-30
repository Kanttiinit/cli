#include <string>

using namespace std;

// from https://stackoverflow.com/a/4654718/1810160
bool is_number(const string& s) {
  string::const_iterator it = s.begin();
  if (*it == '-') {
    it++;
  }
  while (it != s.end() && isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

namespace TimeUtils {
  tm parse_day(string day) {
    tm tm;
    if (!strptime(day.c_str(), "%Y-%m-%d", &tm)) {
      time_t t = time(0);
      tm = *localtime(&t);

      if (is_number(day)) {
        int n = atoi(day.c_str());
        tm.tm_mday += n;
        mktime(&tm);
      }
    }
    return tm;
  }

  string format(tm date, string format, size_t size) {
    char output[size];
    strftime(output, size, format.c_str(), &date);
    return string(output);
  }

  string format_now(string format, size_t size) {
    auto t = time(0);
    auto tm = localtime(&t);
    return TimeUtils::format(*tm, format, size);
  }

  int get_weekday(tm tm) {
    time_t t = mktime(&tm);
    char today[2];
    strftime(today, 2, "%u", localtime(&t));
    return atoi(today) - 1;
  }
  
  struct time_of_day {
    int hours;
    int minutes;
  };

  time_of_day parse_time_of_day(string time_of_day) {
    size_t colon_pos = time_of_day.find(":");
    string hours = time_of_day.substr(0, colon_pos);
    string minutes = time_of_day.substr(colon_pos + 1, time_of_day.length());
    struct time_of_day out = { atoi(hours.c_str()), atoi(minutes.c_str()) };
    return out;
  }

  bool is_before(string a, string b) {
    auto a_tod = parse_time_of_day(a);
    auto b_tod = parse_time_of_day(b);
    if (a_tod.hours == b_tod.hours) {
      return a_tod.minutes < b_tod.minutes;
    }
    return a_tod.hours < b_tod.hours;
  }

  bool is_open(string str) {
    string open = str.substr(0, str.find(" - "));
    string close = str.substr(str.find(" - ") + 3, str.length());
    auto t = time(0);
    char now[6];
    strftime(now, 6, "%H:%M", localtime(&t));
    
    return is_before(open, now) && is_before(now, close);
  }

  string time_until(tm t) {
    double seconds = difftime(time(0), mktime(&t));
    int hours = seconds / 60 / 60;
    seconds -= hours * 60 * 60;
    int minutes = seconds / 60;
    return to_string(hours) + " hours " + to_string(minutes) + " minutes";
  }
}