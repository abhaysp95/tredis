#ifndef __RESP_PARSER_HPP_
#define __RESP_PARSER_HPP_

// simple resp parser for now
#include <expected>
#include <string_view>

std::expected<std::string_view, bool> resp_parser(std::string_view command);

#endif
