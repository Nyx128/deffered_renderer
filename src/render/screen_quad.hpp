#pragma once

#include <glad/glad.h>
#include <memory>
#include "core/buffer.hpp" // Utilizing existing buffer wrapper if possible, or raw GL for simplicity since it's just a quad

#include "core/shader.hpp"
#include "fbo.hpp"

namespace render {
    class framebuffer; // Forward declaration
    class camera;      // Forward declaration

    class screen_quad {
    public:
        screen_quad();
        ~screen_quad();

        void draw(GLuint texture_id) const;
        void render() const;

    private:
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        core::shader m_shader;        // Debug/Pass-through shader

        
        void setup_quad();
    };

}
