#include "./log_utils.hpp"

std::string parse_crlf(std::string_view input) {
  std::string result;
  size_t n = input.size();
  for (size_t i = 0; i < n; i++) {
    switch (input[i]) {
    case '\r':
      result += "\\r";
      break;
    case '\n':
      result += "\\n";
      break;
    default:
      result += input[i];
    }
  }
  return result;
}
