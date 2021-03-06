#pragma once

#include <string_view>

namespace crystal::compiler::parser {

using UintValue  = unsigned long long;
using FloatValue = double;

struct Token {
  int type = 0;

  bool             bool_value  = 0;
  UintValue        int_value   = 0;
  FloatValue       float_value = 0.0;
  std::string_view string_value;

  constexpr Token() = default;
  constexpr Token(int type) : type(type) {}
  constexpr Token(int type, bool value) : type(type), bool_value(value) {}
  constexpr Token(int type, UintValue value) : type(type), int_value(value) {}
  constexpr Token(int type, FloatValue value) : type(type), float_value(value) {}
  constexpr Token(int type, std::string_view value) : type(type), string_value(value) {}
};

}  // namespace crystal::compiler::parser
