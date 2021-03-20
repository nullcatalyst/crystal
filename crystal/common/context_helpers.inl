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
  return create_vertex_buffer(list.begin(), sizeof(list.begin()[0]) * list.size());
}

template <typename Container>
VertexBuffer create_vertex_buffer(const Container& container) {
  return create_vertex_buffer(container.data(), sizeof(container.data()[0]) * container.size());
}

template <typename T>
void update_vertex_buffer(VertexBuffer& vertex_buffer, const std::initializer_list<T> list) {
  return update_vertex_buffer(list.begin(), sizeof(list.begin()[0]) * list.size());
}

template <typename Container>
void update_vertex_buffer(VertexBuffer& vertex_buffer, const Container& container) {
  return create_vertex_buffer(container.data(), sizeof(container.data()[0]) * container.size());
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
