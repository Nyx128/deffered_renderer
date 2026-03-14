#pragma once

#include "../render_pass.hpp"
#include "../camera.hpp"
#include "core/compute_shader.hpp"
#include "utils/scope_timer.hpp"

namespace render {

    class lighting_pass : public IRenderPass {
    public:
        lighting_pass(camera* cam, int width, int height)
            : m_camera(cam), m_width(width), m_height(height), m_shader("shaders/lighting.comp") {}

        void execute(const framebuffer* input, framebuffer* output) override {
            m_shader.bind();

            // Bind G-Buffer textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, input->get_position_texture());
            m_shader.set_int("gPosition", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, input->get_normal_texture());
            m_shader.set_int("gNormal", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, input->get_albedo_texture());
            m_shader.set_int("gAlbedo", 2);

            // Bind output texture as image (use albedo slot for color output)
            glBindImageTexture(0, output->get_albedo_texture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

            m_shader.set_vec3("lightPos", glm::vec3(2.0f, 4.0f, 3.0f)); 
            m_shader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            m_shader.set_vec3("viewPos", m_camera->position);

            unsigned int workgroupsX = (m_width+16)/16;
            unsigned int workgroupsY = (m_height+16)/16;
            m_shader.dispatch(workgroupsX, workgroupsY, 1);

            // Memory barrier
            m_shader.wait_memory(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        void on_resize(int width, int height) override {
            m_width = width;
            m_height = height;
        }

    private:
        camera* m_camera;
        int m_width;
        int m_height;
        core::compute_shader m_shader;
    };

}
