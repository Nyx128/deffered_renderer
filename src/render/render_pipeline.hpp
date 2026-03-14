#pragma once

#include "render_pass.hpp"
#include "fbo.hpp"
#include <vector>
#include <memory>

namespace render {

    class render_pipeline {
    public:
        render_pipeline(int width, int height); 
        ~render_pipeline() = default;

        void add_pass(std::unique_ptr<IRenderPass> pass);
        GLuint render();
        void resize(int width, int height);

    private:
        int m_width;
        int m_height;
        std::vector<std::unique_ptr<IRenderPass>> m_passes;
        
        std::unique_ptr<framebuffer> m_ping;
        std::unique_ptr<framebuffer> m_pong;
    };

}
