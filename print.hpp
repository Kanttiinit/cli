#ifndef PRINT_H
#define PRINT_H

#include "lib/termcolor.hpp"

using namespace std;

namespace Print {
  string current_progress = "";

  void progress(string message) {
    cout << message;
    int length_diff = current_progress.length() - message.length();
    for (int i = 0; i < length_diff; i++) {
      cout << " ";
    }
    cout << "\r";
    current_progress = message;
  }

  void erase_progress() {
    cout << "\r";
    for (size_t i = 0; i < current_progress.length(); i++) {
      cout << " ";
    }
  }

  void dimmed(string message) {
    cout << termcolor::dark << message << termcolor::reset;
  }

  void bold(string message) {
    cout << termcolor::bold << message << termcolor::reset;
  }

  void green(string message) {
    cout << termcolor::green << message << termcolor::reset;
  }

  void red(string message) {
    cout << termcolor::red << message << termcolor::reset;
  }

  void basic(string message) {
    cout << message;
  }
}

#endif