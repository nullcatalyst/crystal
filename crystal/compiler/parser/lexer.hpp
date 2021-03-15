#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include "crystal/compiler/parser/token.hpp"
#include "util/fs/file.hpp"

namespace crystal::compiler::parser {

struct Lexer {
  std::string                 buffer_ = "";
  std::string::const_iterator next_   = std::end(std::string());

public:
  static Lexer from_file(std::string file_name);

  constexpr Lexer() = default;

  Lexer(const std::string& buffer) : buffer_(buffer), next_(buffer_.cbegin()) {}

  Lexer(std::string&& buffer) : buffer_(std::move(buffer)), next_(buffer_.cbegin()) {}

  Token next();
};

}  // namespace crystal::compiler::parser
