#include "fbo.hpp"
#include <iostream>
#include "utils/resource_tracker.hpp"

namespace render {

    framebuffer::framebuffer(int width, int height) 
        : m_width(width), m_height(height) {
        create_framebuffer();
    }

    framebuffer::~framebuffer() {
        delete_framebuffer();
    }

    void framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);
    }

    void framebuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebuffer::resize(int width, int height) {
        if (m_width == width && m_height == height) return;
        m_width = width;
        m_height = height;
        create_framebuffer();
    }

    void framebuffer::create_framebuffer() {
        // Clean up if exists
        delete_framebuffer();

        // 1. Generate FBO
        glCreateFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // Position (RGB16F)
        glCreateTextures(GL_TEXTURE_2D, 1, &m_position_texture);
        glBindTexture(GL_TEXTURE_2D, m_position_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_position_texture, 0);

        // Normal (RGB16F)
        glCreateTextures(GL_TEXTURE_2D, 1, &m_normal_texture);
        glBindTexture(GL_TEXTURE_2D, m_normal_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal_texture, 0);

        // Albedo (RGBA)
        glCreateTextures(GL_TEXTURE_2D, 1, &m_albedo_texture);
        glBindTexture(GL_TEXTURE_2D, m_albedo_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedo_texture, 0);

        // Draw Buffers
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // Depth/Stencil RBO
        glCreateRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Track VRAM: 8+8+4+4 = 24 bytes per pixel
        size_t size = static_cast<size_t>(m_width) * m_height * 24;
        utils::resource_tracker::allocate_vram(size);
    }

    void framebuffer::delete_framebuffer() {
        if (m_fbo) {
            size_t size = static_cast<size_t>(m_width) * m_height * 24;
            utils::resource_tracker::free_vram(size);

            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
        if (m_position_texture) {
            glDeleteTextures(1, &m_position_texture);
            m_position_texture = 0;
        }
        if (m_normal_texture) {
            glDeleteTextures(1, &m_normal_texture);
            m_normal_texture = 0;
        }
        if (m_albedo_texture) {
            glDeleteTextures(1, &m_albedo_texture);
            m_albedo_texture = 0;
        }
        if (m_rbo) {
            glDeleteRenderbuffers(1, &m_rbo);
            m_rbo = 0;
        }
    }

}
