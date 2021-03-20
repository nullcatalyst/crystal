#include "crystal/compiler/parser/lexer.hpp"

#include <array>
#include <tuple>

#include "absl/container/flat_hash_map.h"
#include "crystal/compiler/parser/grammar.hpp"

namespace crystal::compiler::parser {

namespace {

static const absl::flat_hash_map<std::string, int> KEYWORDS{
    {"namespace", TOK_KW_NAMESPACE}, {"struct", TOK_KW_STRUCT},       {"vertex", TOK_KW_VERTEX},
    {"fragment", TOK_KW_FRAGMENT},   {"pipeline", TOK_KW_PIPELINE},   {"uniform", TOK_KW_UNIFORM},
    {"texture", TOK_KW_TEXTURE},     {"instanced", TOK_KW_INSTANCED}, {"return", TOK_KW_RETURN},
};

inline std::tuple<unsigned long long /* value */, int /* digits */,
                  std::string::const_iterator /* next */>
parse_int(std::string::const_iterator start, std::string::const_iterator end) {
  unsigned long long value  = 0;
  int                digits = 0;

  while (start != end) {
    char c = *start;
    if (!(c >= '0' && c <= '9')) {
      break;
    }
    ++start;

    value = 10 * value + (c - '0');
    digits += 1;
  }

  return std::make_tuple(value, digits, start);
}

}  // namespace

Lexer Lexer::from_file(std::string file_name) {
  const auto contents = util::fs::read_file_string(file_name);
  // util::msg::debug("[", file_name, "] contents:\n--- BEGIN ---\n", contents, "\n--- END ---");
  return Lexer{contents};
}

Token Lexer::next() {
  const auto end = buffer_.cend();
  while (next_ != end) {
    const char c = *next_;

    // Ignore whitespace.
    if (isspace(c)) {
      ++next_;
      continue;
    }

    // Ignore comments.
    if (c == '/' && (next_ + 1) != end && *(next_ + 1) == '/') {
      next_ += 2;  // Skip '//'.
      while (next_ != end && *next_ != '\n') {
        ++next_;
      }
      continue;
    }

    // "[^"]*"
    if (c == '\"') {
      ++next_;
      const auto start = next_;
      while (next_ != end && *next_ != '\"') {
        ++next_;
      }
      const std::string_view iden(&*start, next_ - start);
      if (next_ != end) {
        ++next_;
      }
      return Token{TOK_LIT_STR, iden};
    }

    // [0-9]+(\.[0-9]+)?
    if (c >= '0' && c <= '9') {
      const auto [value, digits, next] = parse_int(next_, end);
      next_                            = next;

      if (next_ != end && *next_ == '.' && (next_ + 1) != end && *(next_ + 1) >= '0' &&
          *(next_ + 1) <= '9') {
        // It most likely results in a more accurate floating point value to take the parsed
        // integer and multiplying by the appropriate power of 10, rather than accumulating error
        // by repeatedly multiplying by `0.1`.
        static const std::array<double, 32> FRACTIONALS{
            0.0,     1.0e-1,  1.0e-2,  1.0e-3,  1.0e-4,  1.0e-5,  1.0e-6,  1.0e-7,
            1.0e-8,  1.0e-9,  1.0e-10, 1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14, 1.0e-15,
            1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19, 1.0e-20, 1.0e-21, 1.0e-22, 1.0e-23,
            1.0e-24, 1.0e-25, 1.0e-26, 1.0e-27, 1.0e-28, 1.0e-29, 1.0e-30, 1.0e-31,
        };

        ++next_;  // skip the '.'
        const auto [frac_value, digits, next] = parse_int(next_, end);
        next_                                 = next;

        if (digits >= 32) {
          fprintf(stderr, "cannot parse floating point value: too many fractional digits");
          abort();
        }

        return Token{TOK_LIT_FLOAT, static_cast<double>(value) + frac_value * FRACTIONALS[digits]};
      }  // ^^^ Parse float fractional component, eg: ".123".

      return Token{TOK_LIT_INT, value};
    }

    // [_a-zA-Z][_a-zA-Z0-9]*
    if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
      const auto start = next_;
      do {
        ++next_;
      } while (next_ != end &&
               (*next_ == '_' || (*next_ >= 'a' && *next_ <= 'z') ||
                (*next_ >= 'A' && *next_ <= 'Z') || (*next_ >= '0' && *next_ <= '9')));

      const std::string_view iden(&*start, next_ - start);

      if (iden == "false") {
        return Token{TOK_LIT_BOOL, false};
      } else if (iden == "true") {
        return Token{TOK_LIT_BOOL, true};
      }

      const auto it = KEYWORDS.find(iden);
      if (it != KEYWORDS.cend()) {
        return Token{it->second};
      }

      return Token{TOK_LIT_IDEN, iden};
    }

    switch (c) {
      case ':':
        if ((next_ + 1) != end && *(next_ + 1) == ':') {
          next_ += 2;
          return Token{TOK_OP_COLONCOLON};
        }
        ++next_;
        return Token{TOK_OP_COLON};

      case ';':
        ++next_;
        return Token{TOK_OP_SEMICOLON};

      case '.':
        ++next_;
        return Token{TOK_OP_PERIOD};

      case ',':
        ++next_;
        return Token{TOK_OP_COMMA};

      case '=':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_EQUALEQUAL};
        }
        ++next_;
        return Token{TOK_OP_EQUAL};

      case '!':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_EXCLAIMEQUAL};
        }
        break;
        // ++next_;
        // return Token{TOK_OP_GREATER};

      case '<':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_LESSEQUAL};
        }
        ++next_;
        return Token{TOK_OP_LESS};

      case '>':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_GREATEREQUAL};
        }
        ++next_;
        return Token{TOK_OP_GREATER};

      case '+':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_PLUSEQUAL};
        }
        ++next_;
        return Token{TOK_OP_PLUS};

      case '-':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_MINUSEQUAL};
        }
        if ((next_ + 1) != end && *(next_ + 1) == '>') {
          next_ += 2;
          return Token{TOK_OP_RARROW};
        }
        ++next_;
        return Token{TOK_OP_MINUS};

      case '*':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_STAREQUAL};
        }
        ++next_;
        return Token{TOK_OP_STAR};

      case '/':
        if ((next_ + 1) != end && *(next_ + 1) == '=') {
          next_ += 2;
          return Token{TOK_OP_SLASHEQUAL};
        }
        ++next_;
        return Token{TOK_OP_SLASH};

      case '(':
        ++next_;
        return Token{TOK_OP_LRNDBRACKET};

      case ')':
        ++next_;
        return Token{TOK_OP_RRNDBRACKET};

      case '{':
        ++next_;
        return Token{TOK_OP_LCRLBRACKET};

      case '}':
        ++next_;
        return Token{TOK_OP_RCRLBRACKET};

      default:
        break;
    }

    fprintf(stderr, "unexpected token %c [%d]\n", c, c);
    abort();
  }

  return Token{0};
}

}  // namespace crystal::compiler::parser
