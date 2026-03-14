#pragma once

#include <GLFW/glfw3.h>

namespace core {

    class imgui_context {
    public:
        imgui_context(GLFWwindow* window);
        ~imgui_context();

        void new_frame();
        void render();

    private:
        GLFWwindow* m_window;
    };

}
