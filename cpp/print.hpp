#ifndef PRINT_H
#define PRINT_H

#include "lib/termcolor.hpp"

using namespace std;

namespace Print {
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