#include "buffer.hpp"
#include <iostream>
#include <utility>
#include "utils/resource_tracker.hpp"

namespace core {

buffer::buffer(size_t size, buffer_usage usage) 
    : m_id(0), m_size(size), m_usage(usage) {
    create();
    glNamedBufferData(m_id, m_size, nullptr, static_cast<GLenum>(m_usage));
    utils::resource_tracker::allocate_vram(m_size);
}

buffer::buffer(const void* data, size_t size, buffer_usage usage)
    : m_id(0), m_size(size), m_usage(usage) {
    create();
    glNamedBufferData(m_id, m_size, data, static_cast<GLenum>(m_usage));
    utils::resource_tracker::allocate_vram(m_size);
}

buffer::~buffer() {
    destroy();
}

buffer::buffer(buffer&& other) noexcept
    : m_id(other.m_id), m_size(other.m_size), m_usage(other.m_usage) {
    other.m_id = 0;
    other.m_size = 0;
}

buffer& buffer::operator=(buffer&& other) noexcept {
    if (this != &other) {
        destroy();
        m_id = other.m_id;
        m_size = other.m_size;
        m_usage = other.m_usage;
        other.m_id = 0;
        other.m_size = 0;
    }
    return *this;
}

void buffer::bind(GLuint binding_point) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, m_id);
}

void buffer::unbind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void buffer::upload(const void* data, size_t size, size_t offset) {
    if (offset + size > m_size) {
        std::cerr << "Buffer upload exceeds buffer size" << std::endl;
        return;
    }
    glNamedBufferSubData(m_id, offset, size, data);
}

void buffer::resize(size_t new_size, buffer_usage usage) {
    utils::resource_tracker::free_vram(m_size);
    m_size = new_size;
    m_usage = usage;
    glNamedBufferData(m_id, m_size, nullptr, static_cast<GLenum>(m_usage));
    utils::resource_tracker::allocate_vram(m_size);
}

void* buffer::map(GLenum access) {
    return glMapNamedBuffer(m_id, access);
}

void buffer::unmap() {
    glUnmapNamedBuffer(m_id);
}

void buffer::create() {
    glCreateBuffers(1, &m_id);
}

void buffer::destroy() {
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
        utils::resource_tracker::free_vram(m_size);
    }
}

} // namespace core
