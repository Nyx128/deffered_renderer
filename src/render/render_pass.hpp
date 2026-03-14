#pragma once

#include "fbo.hpp"

namespace render {

    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;

        // Core execution method
        // input: The result of the previous pass (Read-Only)
        // output: Where this pass writes to (Write-Only)
        virtual void execute(const framebuffer* input, framebuffer* output) = 0;

        // Optional: Resize handler
        virtual void on_resize(int width, int height) {}
    };

}
