#include "cli11/cli11.hpp"
#include "crystal/compiler/parser/lexer.hpp"
#include "crystal/compiler/parser/parse.hpp"
#include "util/fs/path.hpp"

int main(const int argc, const char* const argv[]) {
  using namespace crystal::compiler;

  cli::App app("App description");

  {  // Convert to glsl.
    const auto glsl_cmd = app.add_subcommand("glsl");

    std::string input_file_name;
    glsl_cmd->add_option("-i,--input", input_file_name, "Input file name")->required();
    std::string output_directory = "";
    glsl_cmd->add_option("-o,--output", output_directory, "Output directory");
    bool pretty = false;
    glsl_cmd->add_flag("--pretty", pretty, "Output the shaders in a human readable form");

    glsl_cmd->final_callback([&]() {
      parser::Lexer lex = parser::Lexer::from_file(input_file_name);
      ast::Module   mod = parser::parse(lex);

      for (auto& vertex_function : mod.vertex_functions()) {
        const std::string output_file_name =
            util::fs::path_join(output_directory, vertex_function->name() + ".vert.glsl");
        std::ofstream output_file(output_file_name);
        util::msg::debug("outputting vertex file [", output_file_name, "]");

        if (pretty) {
          vertex_function->to_pretty_glsl(output_file, mod);
        } else {
          vertex_function->to_glsl(output_file, mod);
        }
      }

      for (auto& fragment_function : mod.fragment_functions()) {
        const std::string output_file_name =
            util::fs::path_join(output_directory, fragment_function->name() + ".frag.glsl");
        std::ofstream output_file(output_file_name);
        util::msg::debug("outputting fragment file [", output_file_name, "]");

        if (pretty) {
          fragment_function->to_pretty_glsl(output_file, mod);
        } else {
          fragment_function->to_glsl(output_file, mod);
        }
      }
    });
  }

  {  // Convert to cpp.
    const auto cpp_cmd = app.add_subcommand("cpp");

    std::string input_file_name;
    cpp_cmd->add_option("-i,--input", input_file_name, "Input file name")->required();
    std::string output_file_name = "";
    cpp_cmd->add_option("-o,--output", output_file_name, "Output file")->required();

    cpp_cmd->final_callback([&]() {
      parser::Lexer lex = parser::Lexer::from_file(input_file_name);
      ast::Module   mod = parser::parse(lex);

      std::ofstream output_file(output_file_name);
      util::msg::debug("outputting cpphdr file [", output_file_name, "]");
      mod.to_cpphdr(output_file);
    });
  }

  CLI11_PARSE(app, argc, argv);
  return 0;
}
