#pragma once

#include <glad/glad.h>

namespace render {

    class framebuffer {
    public:
        framebuffer(int width, int height);
        ~framebuffer();

        // Delete copy
        framebuffer(const framebuffer&) = delete;
        framebuffer& operator=(const framebuffer&) = delete;

        void bind() const;
        void unbind() const;

        void resize(int width, int height);
        GLuint get_position_texture() const { return m_position_texture; }
        GLuint get_normal_texture() const { return m_normal_texture; }
        GLuint get_albedo_texture() const { return m_albedo_texture; }
        GLuint get_fbo() const { return m_fbo; }

    private:
        GLuint m_fbo = 0;
        GLuint m_position_texture = 0;
        GLuint m_normal_texture = 0;
        GLuint m_albedo_texture = 0;
        GLuint m_rbo = 0;
        int m_width;
        int m_height;

        void create_framebuffer();
        void delete_framebuffer();
    };

}
