#include "crystal/compiler/parser/lexer.hpp"
#include "crystal/compiler/parser/parse.hpp"

int main(const int argc, const char* const argv[]) {
  using namespace crystal::compiler;

  parser::Lexer lex = parser::Lexer::from_file("crystal/compiler/test.crystal");
  ast::Module   mod = parser::parse(lex);

  auto vert = mod.find_vertex_function("triangle_vert");
  if (vert != std::nullopt) {
    vert.value()->to_glsl(std::cout, mod);
  }

  return 0;
}
