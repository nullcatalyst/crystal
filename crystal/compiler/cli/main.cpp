#include "cli11/cli11.hpp"
#include "crystal/compiler/parser/lexer.hpp"
#include "crystal/compiler/parser/parse.hpp"

int main(const int argc, const char* const argv[]) {
  using namespace crystal::compiler;

  cli::App app("App description");

  parser::Lexer lex = parser::Lexer::from_file("crystal/compiler/cli/test.crystal");
  ast::Module   mod = parser::parse(lex);

  auto vert = mod.find_vertex_function("triangle_vert");
  if (vert != std::nullopt) {
    vert.value()->to_glsl(std::cout, mod);
  }

  auto frag = mod.find_fragment_function("triangle_frag");
  if (frag != std::nullopt) {
    frag.value()->to_glsl(std::cout, mod);
  }

  return 0;
}
