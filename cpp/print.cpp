#include "lib/termcolor.hpp"

namespace Print {
  void dimmed(std::string message) {
    std::cout << termcolor::dark << message << termcolor::reset;
  }

  void bold(std::string message) {
    std::cout << termcolor::bold << message << termcolor::reset;
  }

  void green(std::string message) {
    std::cout << termcolor::green << message << termcolor::reset;
  }

  void red(std::string message) {
    std::cout << termcolor::red << message << termcolor::reset;
  }

  void basic(std::string message) {
    std::cout << message;
  }
}
