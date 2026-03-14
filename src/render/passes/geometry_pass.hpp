#pragma once

#include "../render_pass.hpp"
#include "../model.hpp"
#include "../camera.hpp"
#include "core/shader.hpp"

namespace render {

    class geometry_pass : public IRenderPass {
    public:
        geometry_pass(model* scene_model, camera* cam, core::shader* shader)
            : m_model(scene_model), m_camera(cam), m_shader(shader) {
                glCreateVertexArrays(1, &m_dummy_vao);
            }
            
            void execute(const framebuffer* input, framebuffer* output) override {
                output->bind();
                glEnable(GL_DEPTH_TEST);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                m_shader->bind();
                
                glm::mat4 projection = m_camera->get_projection_matrix();
                glm::mat4 view = m_camera->get_view_matrix();
                
                m_shader->set_mat4("projection", projection);
                m_shader->set_mat4("view", view);
                m_shader->set_mat4("model", glm::mat4(1.0f));
                
                glBindVertexArray(m_dummy_vao);
                m_model->draw(*m_shader);
                
                output->unbind();
            }

    private:
        model* m_model;
        camera* m_camera;
        core::shader* m_shader;
        GLuint m_dummy_vao =0;
    };

}
