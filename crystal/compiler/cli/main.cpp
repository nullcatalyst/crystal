#include "cli11/cli11.hpp"
#include "crystal/common/proto/proto.hpp"
#include "crystal/compiler/parser/lexer.hpp"
#include "crystal/compiler/parser/parse.hpp"
#include "util/fs/path.hpp"

int main(const int argc, const char* const argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  using namespace crystal::compiler;

  cli::App app("App description");

  {  // Convert to glsl.
    const auto cmd = app.add_subcommand("glsl");

    std::string input_file_name;
    cmd->add_option("-i,--input", input_file_name, "Input file name")->required();
    std::string output_directory = "";
    cmd->add_option("-o,--output", output_directory, "Output directory");
    bool pretty = false;
    cmd->add_flag("--pretty", pretty, "Output the shaders in a human readable form");

    cmd->final_callback([&]() {
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
    const auto cmd = app.add_subcommand("cpp");

    std::string input_file_name;
    cmd->add_option("-i,--input", input_file_name, "Input file name")->required();
    std::string output_file_name = "";
    cmd->add_option("-o,--output", output_file_name, "Output file");

    cmd->final_callback([&]() {
      parser::Lexer lex = parser::Lexer::from_file(input_file_name);
      ast::Module   mod = parser::parse(lex);

      if (output_file_name.size() == 0) {
        output_file_name = util::fs::replace_extension(input_file_name, "crystal", "hpp");
      }

      std::ofstream output_file(output_file_name);
      util::msg::debug("outputting cpphdr file [", output_file_name, "]");
      mod.to_cpphdr(output_file, crystal::compiler::ast::CppOutputOptions{
                                     .cpp17 = true,
                                 });
    });
  }

  {  // Convert to crystallib.
    const auto cmd = app.add_subcommand("lib");

    std::string input_file_name;
    cmd->add_option("-i,--input", input_file_name, "Input file name")->required();
    std::string output_file_name = "";
    cmd->add_option("-o,--output", output_file_name, "Output file");

    cmd->final_callback([&]() {
      parser::Lexer lex = parser::Lexer::from_file(input_file_name);
      ast::Module   mod = parser::parse(lex);

      if (output_file_name.size() == 0) {
        output_file_name = util::fs::replace_extension(input_file_name, "crystal", "crystallib");
      }

      std::ofstream output_file(output_file_name, std::ios_base::out | std::ios_base::binary);
      util::msg::debug("outputting crystallib file [", output_file_name, "]");
      mod.to_crystallib(output_file, crystal::compiler::ast::CrystallibOutputOptions{
                                         .glslang_validator_path = "glslangValidator",
                                         .spirv_link_path        = "spirv-link",
                                     });
    });
  }

  CLI11_PARSE(app, argc, argv);
  return 0;
}
