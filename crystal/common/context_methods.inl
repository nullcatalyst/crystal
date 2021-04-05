[[nodiscard]] constexpr uint32_t    screen_width() const;
[[nodiscard]] constexpr uint32_t    screen_height() const;
[[nodiscard]] constexpr RenderPass& screen_render_pass();

void          set_active();
void          wait();
CommandBuffer next_frame();

void change_resolution(uint32_t width, uint32_t height);

Texture create_texture(const TextureDesc& desc);

RenderPass create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures);
RenderPass create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
    const std::tuple<const Texture&, AttachmentDesc>                        depth_texture);
void set_clear_color(RenderPass& render_pass, uint32_t attachment, ClearValue clear_value);
void set_clear_depth(RenderPass& render_pass, ClearValue clear_value);

Library  create_library(const std::string_view library_file_path);
Pipeline create_pipeline(Library& library, RenderPass& render_pass, const PipelineDesc& desc);

UniformBuffer create_uniform_buffer(const size_t byte_length);
UniformBuffer create_uniform_buffer(const void* const data_ptr, const size_t byte_length);
void          update_uniform_buffer(UniformBuffer& uniform_buffer, const void* const data_ptr,
                                    const size_t byte_length);

VertexBuffer create_vertex_buffer(const size_t byte_length);
VertexBuffer create_vertex_buffer(const void* const data_ptr, const size_t byte_length);
void         update_vertex_buffer(VertexBuffer& vertex_buffer, const void* const data_ptr,
                                  const size_t byte_length);

IndexBuffer create_index_buffer(const size_t byte_length);
IndexBuffer create_index_buffer(const uint16_t* const data_ptr, const size_t byte_length);
void        update_index_buffer(IndexBuffer& index_buffer, const uint16_t* const data_ptr,
                                const size_t byte_length);

Mesh create_mesh(const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings);
Mesh create_mesh(const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
                 const IndexBuffer& index_buffer);

// Templated convenience helpers.

template <typename T>
UniformBuffer create_uniform_buffer(const T& value) {
  return create_uniform_buffer(&value, sizeof(T));
}

template <typename T>
void update_uniform_buffer(UniformBuffer& uniform_buffer, const T& value) {
  return update_uniform_buffer(uniform_buffer, &value, sizeof(T));
}

template <typename T>
VertexBuffer create_vertex_buffer(const std::initializer_list<T> list) {
  return create_vertex_buffer(list.begin(), sizeof(T) * list.size());
}

template <typename Container>
VertexBuffer create_vertex_buffer(const Container& container) {
  return create_vertex_buffer(container.data(), sizeof(container.data()[0]) * container.size());
}

template <typename T>
void update_vertex_buffer(VertexBuffer& vertex_buffer, const std::initializer_list<T> list) {
  return update_vertex_buffer(vertex_buffer, list.begin(), sizeof(T) * list.size());
}

template <typename Container>
void update_vertex_buffer(VertexBuffer& vertex_buffer, const Container& container) {
  return update_vertex_buffer(vertex_buffer, container.data(),
                              sizeof(container.data()[0]) * container.size());
}

template <typename Container>
IndexBuffer create_index_buffer(const Container& container) {
  static_assert(std::is_same<decltype(container.data()[0]), uint16_t>::value ||
                    std::is_same<decltype(container.data()[0]), uint16_t&>::value ||
                    std::is_same<decltype(container.data()[0]), const uint16_t&>::value,
                "index buffer type must be [uint16_t]");
  return create_index_buffer(container.data(), sizeof(uint16_t) * container.size());
}

IndexBuffer create_index_buffer(const std::initializer_list<uint16_t> list) {
  return create_index_buffer(list.begin(), sizeof(uint16_t) * list.size());
}

template <typename Container>
void update_index_buffer(IndexBuffer& index_buffer, const Container& container) {
  static_assert(std::is_same<decltype(container.data()[0]), uint16_t>::value ||
                    std::is_same<decltype(container.data()[0]), uint16_t&>::value ||
                    std::is_same<decltype(container.data()[0]), const uint16_t&>::value,
                "index buffer type must be [uint16_t]");
  return update_index_buffer(index_buffer, container.begin(),
                             sizeof(container.begin()[0]) * container.size());
}

void update_index_buffer(IndexBuffer& index_buffer, const std::initializer_list<uint16_t> list) {
  return update_index_buffer(index_buffer, list.begin(), sizeof(uint16_t) * list.size());
}
