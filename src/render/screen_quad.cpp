#include "screen_quad.hpp"
#include "fbo.hpp"
#include "camera.hpp"
#include <vector>

namespace render {

    screen_quad::screen_quad() 
        : m_shader("shaders/screen.vert", "shaders/screen.frag") {
        setup_quad();
    }

    screen_quad::~screen_quad() {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
    }

    void screen_quad::setup_quad() {
        // Quad vertices (Pos x, y, UV u, v)
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glCreateVertexArrays(1, &m_vao);
        glCreateBuffers(1, &m_vbo);

        glNamedBufferStorage(m_vbo, sizeof(quadVertices), quadVertices, 0);

        glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, 4 * sizeof(float));

        // Position
        glEnableVertexArrayAttrib(m_vao, 0);
        glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_vao, 0, 0);

        // UV
        glEnableVertexArrayAttrib(m_vao, 1);
        glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayAttribBinding(m_vao, 1, 0);
    }

    void screen_quad::draw(GLuint texture_id) const {
        glDisable(GL_DEPTH_TEST);
        
        m_shader.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        m_shader.set_int("screenTexture", 0);

        render();
    }

    void screen_quad::render() const {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

}
