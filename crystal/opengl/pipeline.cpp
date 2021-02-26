#include "crystal/opengl/pipeline.hpp"

#include "crystal/opengl/context.hpp"
#include "crystal/opengl/library.hpp"
#include "util/fs/file.hpp"
#include "util/fs/path.hpp"

namespace crystal::opengl {

namespace {

GLuint compile_shader(const GLenum shader_type, const std::string& shader_source) {
  const GLchar* const c_source = shader_source.c_str();
  GLuint              shader   = glCreateShader(shader_type);
  GL_ASSERT(glShaderSource(shader, 1, &c_source, nullptr), "binding source to shader");
  GL_ASSERT(glCompileShader(shader), "compiling shader");

  GLint log_length;
  GL_ASSERT(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length),
            "getting shader info log length");

  if (log_length > 0) {
    std::vector<GLchar> log(log_length);
    GL_ASSERT(glGetShaderInfoLog(shader, log_length, &log_length, log.data()),
              "getting shader info log");
    std::cerr << "[OpenGL] Shader compile log: " << log.data() << "\n";
  }

  GLint status;
  GL_ASSERT(glGetShaderiv(shader, GL_COMPILE_STATUS, &status), "getting shader compile status");
  if (status == 0) {
    std::cerr << "[OpenGL] Failed to compile shader\n";
    return 0;
  }

  return shader;
}

GLuint compile_program(GLuint program, const std::string& vertex_source) {
  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);

  // Attach the shaders to our program.
  GL_ASSERT(glAttachShader(program, vertex_shader), "attaching vertex shader to shader program");

  // Delete the shaders since they are now attached to the program, which will retain a reference
  // to them.
  GL_ASSERT(glDeleteShader(vertex_shader), "deleting vertex shader");

  GL_ASSERT(glLinkProgram(program), "linking shader program");

  // Assert that the program was successfully linked.
  GLint log_length = 0;
  GL_ASSERT(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length),
            "getting shader program info log length");
  if (log_length > 0) {
    std::vector<GLchar> log(log_length);
    GL_ASSERT(glGetProgramInfoLog(program, log_length, &log_length, log.data()),
              "getting shader info log");
    std::cerr << "[OpenGL] Program link log:\n" << log.data() << "\n";
  }

  GLint status = 0;
  GL_ASSERT(glGetProgramiv(program, GL_LINK_STATUS, &status), "getting shader program link status");
  if (status == 0) {
    std::cerr << "[OpenGL] Failed to link shader\n";
    GL_ASSERT(glDeleteProgram(program), "deleting shader program");
    return 0;
  }

  return program;
}

GLuint compile_program(GLuint program, const std::string& vertex_source,
                       const std::string& fragment_source) {
  GLuint vertex_shader   = compile_shader(GL_VERTEX_SHADER, vertex_source);
  GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

  // Attach the shaders to our program.
  GL_ASSERT(glAttachShader(program, vertex_shader), "attaching vertex shader to shader program");
  GL_ASSERT(glAttachShader(program, fragment_shader),
            "attaching fragment shader to shader program");

  // Delete the shaders since they are now attached to the program, which will retain a reference
  // to them.
  GL_ASSERT(glDeleteShader(vertex_shader), "deleting vertex shader");
  GL_ASSERT(glDeleteShader(fragment_shader), "deleting fragment shader");

  GL_ASSERT(glLinkProgram(program), "linking shader program");

  // Assert that the program was successfully linked.
  GLint log_length = 0;
  GL_ASSERT(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length),
            "getting shader program info log length");
  if (log_length > 0) {
    std::vector<GLchar> log(log_length);
    GL_ASSERT(glGetProgramInfoLog(program, log_length, &log_length, log.data()),
              "getting shader info log");
    std::cerr << "[OpenGL] Program link log:\n" << log.data() << "\n";
  }

  GLint status = 0;
  GL_ASSERT(glGetProgramiv(program, GL_LINK_STATUS, &status), "getting shader program link status");
  if (status == 0) {
    std::cerr << "[OpenGL] Failed to link shader\n";
    GL_ASSERT(glDeleteProgram(program), "deleting shader program");
    return 0;
  }

  return program;
}

}  // namespace

uint32_t Pipeline::next_id_ = 0;

Pipeline::Pipeline(Pipeline&& other)
    : ctx_(other.ctx_),
      id_(other.id_),
      program_(other.program_),
      cull_mode_(other.cull_mode_),
      depth_test_(other.depth_test_),
      depth_write_(other.depth_write_),
      blend_src_(other.blend_src_),
      blend_dst_(other.blend_dst_),
      bindings_(std::move(other.bindings_)) {
  other.ctx_         = nullptr;
  other.id_          = 0;
  other.program_     = 0;
  other.cull_mode_   = CullMode::None;
  other.depth_test_  = DepthTest::Never;
  other.depth_write_ = DepthWrite::Disable;
  other.blend_src_   = AlphaBlend::Zero;
  other.blend_dst_   = AlphaBlend::Zero;
}

Pipeline& Pipeline::operator=(Pipeline&& other) {
  destroy();

  ctx_         = other.ctx_;
  id_          = other.id_;
  program_     = other.program_;
  cull_mode_   = other.cull_mode_;
  depth_test_  = other.depth_test_;
  depth_write_ = other.depth_write_;
  blend_src_   = other.blend_src_;
  blend_dst_   = other.blend_dst_;
  bindings_    = std::move(other.bindings_);

  other.ctx_         = nullptr;
  other.id_          = 0;
  other.program_     = 0;
  other.cull_mode_   = CullMode::None;
  other.depth_test_  = DepthTest::Never;
  other.depth_write_ = DepthWrite::Disable;
  other.blend_src_   = AlphaBlend::Zero;
  other.blend_dst_   = AlphaBlend::Zero;

  return *this;
}

Pipeline::~Pipeline() { destroy(); }

void Pipeline::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  glDeleteProgram(program_);

  ctx_         = nullptr;
  id_          = 0;
  program_     = 0;
  cull_mode_   = CullMode::None;
  depth_test_  = DepthTest::Never;
  depth_write_ = DepthWrite::Disable;
  blend_src_   = AlphaBlend::Zero;
  blend_dst_   = AlphaBlend::Zero;
  bindings_.resize(0);
}

Pipeline::Pipeline(Context& ctx, Library& library, const PipelineDesc& desc)
    : ctx_(&ctx),
      id_(++next_id_),
      cull_mode_(desc.cull_mode),
      depth_test_(desc.depth_test),
      depth_write_(desc.depth_write),
      blend_src_(desc.blend_src),
      blend_dst_(desc.blend_dst) {
  if (desc.fragment != nullptr) {
    std::string vertex_source =
        util::fs::read_file_string(util::fs::path_join(library.path_, desc.vertex));
    std::string fragment_source =
        util::fs::read_file_string(util::fs::path_join(library.path_, desc.fragment));

    GLuint program = 0;
    GL_ASSERT(program = glCreateProgram(), "creating shader program");
    program_ = compile_program(program, vertex_source, fragment_source);
  } else {
    // Vertex-only shader.
    std::string vertex_source =
        util::fs::read_file_string(util::fs::path_join(library.path_, desc.vertex));

    GLuint program = 0;
    GL_ASSERT(program = glCreateProgram(), "creating shader program");
    program_ = compile_program(program, vertex_source);
  }

  bindings_.resize(desc.vertex_attributes.size());
  std::transform(
      desc.vertex_attributes.begin(), desc.vertex_attributes.end(), bindings_.begin(),
      [&desc](const auto& attribute) {
        return Binding{
            /* .id             = */ attribute.id,
            /* .offset         = */ attribute.offset,
            /* .buffer_index   = */ attribute.buffer_index,
            /* .stride         = */ desc.vertex_buffers.begin()[attribute.buffer_index].stride,
            /* .step_function  = */
            desc.vertex_buffers.begin()[attribute.buffer_index].step_function,
        };
      });
}

}  // namespace crystal::opengl
