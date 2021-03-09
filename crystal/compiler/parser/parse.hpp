#pragma once

#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/parser/lexer.hpp"

namespace crystal::compiler::parser {

ast::Module parse(Lexer& lex);

}  // namespace crystal::compiler::parser
