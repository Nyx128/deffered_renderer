#include "window.hpp"
#include <iostream>
#include <cstdlib>

namespace core
{

    window::window(int width, int height, const std::string &title)
        : m_window(nullptr), m_width(width), m_height(height), m_title(title)
    {
        // Initialize key state arrays
        m_key_current.fill(false);
        m_key_previous.fill(false);

        init();

        // Setup key callback
        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, key_callback);
    }

    window::~window()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    void window::init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Request OpenGL debug context
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);


        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return;
        }
        glfwSwapInterval(1);

        #ifndef NDEBUG
        setup_debug_output();
        #endif
    }

    void window::setup_debug_output()
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        std::cout << "OpenGL debug output enabled" << std::endl;
    }

    void GLAPIENTRY window::opengl_debug_callback(GLenum source, GLenum type, GLuint id,
                                                   GLenum severity, GLsizei length,
                                                   const GLchar *message, const void *userParam)
    {
        // Ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " << message << std::endl;

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
        }
        std::cout << std::endl;

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
        }
        std::cout << std::endl;

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
        }
        std::cout << std::endl;
        std::cout << std::endl;

        // Terminate program on OpenGL errors
        if (type == GL_DEBUG_TYPE_ERROR)
        {
            std::cerr << "FATAL: OpenGL error detected, terminating program." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }


    bool window::should_close() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void window::poll_events()
    {
        update_key_states();
        glfwPollEvents();
    }

    void window::swap_buffers()
    {
        glfwSwapBuffers(m_window);
    }

    bool window::is_key_pressed(int key) const
    {
        if (key < 0 || key >= MAX_KEYS)
            return false;
        return m_key_current[key] && !m_key_previous[key];
    }

    bool window::is_key_released(int key) const
    {
        if (key < 0 || key >= MAX_KEYS)
            return false;
        return !m_key_current[key] && m_key_previous[key];
    }

    bool window::is_key_held(int key) const
    {
        if (key < 0 || key >= MAX_KEYS)
            return false;
        return m_key_current[key];
    }

    bool window::is_mouse_button_down(int button) const
    {
        if (!m_window) return false;
        return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
    }

    void window::get_cursor_pos(double& xpos, double& ypos) const 
    {
        if (!m_window) {
            xpos = 0.0;
            ypos = 0.0;
            return;
        }
        glfwGetCursorPos(m_window, &xpos, &ypos);
    }

    void window::update_key_states()
    {
        m_key_previous = m_key_current;
    }

    void window::key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
    {
        window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
        if (win && key >= 0 && key < MAX_KEYS)
        {
            if (action == GLFW_PRESS)
            {
                win->m_key_current[key] = true;
            }
            else if (action == GLFW_RELEASE)
            {
                win->m_key_current[key] = false;
            }
        }
    }

} // namespace core
