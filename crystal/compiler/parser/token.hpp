#pragma once

#include <string_view>

namespace crystal::compiler::parser {

struct Token {
  int type = 0;

  unsigned long long int_value   = 0;
  long double        float_value = 0.0;
  std::string_view   string_value;

  constexpr Token() = default;
  constexpr Token(int type) : type(type) {}
  constexpr Token(int type, unsigned long long value) : type(type), int_value(value) {}
  constexpr Token(int type, long double value) : type(type), float_value(value) {}
  constexpr Token(int type, std::string_view value) : type(type), string_value(value) {}
};

}  // namespace crystal::compiler::parser
