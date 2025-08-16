#include "./log_utils.hpp"
#include <expected>
#include <string>
#include <string_view>

std::expected<std::string_view, bool> resp_parser(std::string_view command) {
  const std::string LITERAL = "\r\n";
  spd::info("parsing command: {}, size: {}\n", parse_crlf(command),
            command.size());
  size_t idx = command.find(LITERAL);
  size_t count = 0;
  while (idx != std::string::npos) {
    count += 1;
    if (count == 2) {
      auto start = idx + LITERAL.size();
      auto end = command.find(LITERAL, idx + 1);
      // we don't need the starting "\r", hence no (end - start + 1)
      return command.substr(start, end - start);
    }
    idx = command.find(LITERAL, idx + 1);
  }
  return std::unexpected(false);
}
