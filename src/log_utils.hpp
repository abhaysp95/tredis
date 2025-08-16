#ifndef __LOG_UTILS_HPP_
#define __LOG_UTILS_HPP_

#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

namespace spd = spdlog;

std::string parse_crlf(std::string_view input);

#endif
