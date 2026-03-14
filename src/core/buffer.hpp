#pragma once

#include <glad/glad.h>
#include <cstddef>

namespace core
{
    enum class buffer_usage
    {
        static_draw = GL_STATIC_DRAW,
        dynamic_draw = GL_DYNAMIC_DRAW,
        stream_draw = GL_STREAM_DRAW
    };

    class buffer
    {
    public:
        buffer(size_t size, buffer_usage usage);
        buffer(const void *data, size_t size, buffer_usage usage);
        ~buffer();

        buffer(const buffer &) = delete;
        buffer &operator=(const buffer &) = delete;

        buffer(buffer &&other) noexcept;
        buffer &operator=(buffer &&other) noexcept;

        void bind(GLuint binding_point) const;
        void unbind() const;

        void upload(const void *data, size_t size, size_t offset = 0);
        void resize(size_t new_size, buffer_usage usage);

        void *map(GLenum access = GL_READ_WRITE);
        void unmap();

        GLuint id() const { return m_id; }
        size_t size() const { return m_size; }

    private:
        GLuint m_id;
        size_t m_size;
        buffer_usage m_usage;

        void create();
        void destroy();
    };

} // namespace core
