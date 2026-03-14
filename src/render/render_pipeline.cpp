#include "render_pipeline.hpp"
#include <iostream>
#include <glad/glad.h>

namespace render {

    render_pipeline::render_pipeline(int width, int height)
        : m_width(width), m_height(height) {
        // Create Ping-Pong buffers
        m_ping = std::make_unique<framebuffer>(width, height);
        m_pong = std::make_unique<framebuffer>(width, height);
    }

    void render_pipeline::add_pass(std::unique_ptr<IRenderPass> pass) {
        m_passes.push_back(std::move(pass));
    }

    GLuint render_pipeline::render() {
        framebuffer* current_input = m_ping.get();
        framebuffer* current_output = m_pong.get();

        for (auto& pass : m_passes) {
            pass->execute(current_input, current_output);
            
            // Swap for next pass
            std::swap(current_input, current_output);
        }

        return current_input->get_albedo_texture();
    }

    void render_pipeline::resize(int width, int height) {
        m_width = width;
        m_height = height;
        m_ping->resize(width, height);
        m_pong->resize(width, height);
        
        for (auto& pass : m_passes) {
            pass->on_resize(width, height);
        }
    }

}
