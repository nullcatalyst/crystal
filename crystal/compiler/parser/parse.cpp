#include "crystal/compiler/parser/parse.hpp"

#include <cstdlib>  // malloc, free

// lemon declarations:
void* ParseAlloc(void* (*malloc_proc)(size_t));
void  ParseFree(void* parser, void (*free_proc)(void*));
void  Parse(void* parser, int token_type, ::crystal::compiler::parser::Token token_value,
            ::crystal::compiler::ast::Module* mod);

namespace crystal::compiler::parser {

ast::Module parse(Lexer& lex) {
  ast::Module mod;
  mod.add_base_types();

  void* lemon = ParseAlloc([](size_t size) { return calloc(1, size); });

  Token tok;
  do {
    tok = lex.next();
    Parse(lemon, tok.type, tok, &mod);
  } while (tok.type != 0);

  ParseFree(lemon, free);
  return mod;
}

}  // namespace crystal::compiler::parser
