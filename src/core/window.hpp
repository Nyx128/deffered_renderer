#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <array>

namespace core
{

    class window
    {
    public:
        window(int width, int height, const std::string &title);
        ~window();

        window(const window &) = delete;
        window &operator=(const window &) = delete;

        bool should_close() const;
        void poll_events();
        void swap_buffers();

        int get_width() const { return m_width; }
        int get_height() const { return m_height; }
        GLFWwindow* get_native_window() const { return m_window; }

        // Key state queries
        bool is_key_pressed(int key) const;
        bool is_key_released(int key) const;  
        bool is_key_held(int key) const;      
        
        // Mouse state queries
        bool is_mouse_button_down(int button) const;
        void get_cursor_pos(double& xpos, double& ypos) const;

    private:
        GLFWwindow *m_window;
        int m_width;
        int m_height;
        std::string m_title;

        static constexpr int MAX_KEYS = GLFW_KEY_LAST + 1;
        std::array<bool, MAX_KEYS> m_key_current;
        std::array<bool, MAX_KEYS> m_key_previous;

        void init();
        void setup_debug_output();
        void update_key_states();
        static void GLAPIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id,
                                                      GLenum severity, GLsizei length,
                                                      const GLchar *message, const void *userParam);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };

} // namespace core
