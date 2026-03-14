#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace core
{

    shader::shader(const std::string &vertex_path, const std::string &fragment_path)
        : m_program(0), m_vertex_path(vertex_path), m_fragment_path(fragment_path)
    {
        reload();
    }

    shader::~shader()
    {
        if (m_program != 0)
        {
            glDeleteProgram(m_program);
        }
    }

    void shader::bind() const
    {
        glUseProgram(m_program);
    }

    void shader::unbind() const
    {
        glUseProgram(0);
    }

    bool shader::reload()
    {
        std::string vertex_code = read_file(m_vertex_path);
        std::string fragment_code = read_file(m_fragment_path);

        if (vertex_code.empty() || fragment_code.empty())
        {
            std::cerr << "Failed to read shader files during reload" << std::endl;
            return false;
        }

        // Compile shaders
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        const char *v_src = vertex_code.c_str();
        glShaderSource(vertex_shader, 1, &v_src, nullptr);
        glCompileShader(vertex_shader);

        // Check vertex shader compilation
        GLint v_success;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &v_success);
        if (!v_success)
        {
            GLchar info_log[1024];
            glGetShaderInfoLog(vertex_shader, 1024, nullptr, info_log);
            std::cerr << "Vertex shader compilation error during reload:\n"
                      << info_log << std::endl;
            glDeleteShader(vertex_shader);
            return false;
        }

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *f_src = fragment_code.c_str();
        glShaderSource(fragment_shader, 1, &f_src, nullptr);
        glCompileShader(fragment_shader);

        // Check fragment shader compilation
        GLint f_success;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &f_success);
        if (!f_success)
        {
            GLchar info_log[1024];
            glGetShaderInfoLog(fragment_shader, 1024, nullptr, info_log);
            std::cerr << "Fragment shader compilation error during reload:\n"
                      << info_log << std::endl;
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            return false;
        }

        // Create new program
        GLuint new_program = glCreateProgram();
        glAttachShader(new_program, vertex_shader);
        glAttachShader(new_program, fragment_shader);
        glLinkProgram(new_program);

        // Check for linking errors
        GLint success;
        glGetProgramiv(new_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar info_log[1024];
            glGetProgramInfoLog(new_program, 1024, nullptr, info_log);
            std::cerr << "Shader linking error during reload:\n"
                      << info_log << std::endl;

            // Clean up failed program
            glDeleteProgram(new_program);
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            return false;
        }

        // Delete old program and replace with new one
        if (m_program != 0)
        {
            glDeleteProgram(m_program);
        }
        m_program = new_program;

        // Delete shaders as they're linked into our program now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return true;
    }

    std::string shader::read_file(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open shader file: " << filepath << std::endl;
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    GLuint shader::compile_shader(GLenum type, const std::string &source)
    {
        GLuint shader = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        check_compile_errors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

        return shader;
    }

    void shader::link_program(GLuint vertex_shader, GLuint fragment_shader)
    {
        m_program = glCreateProgram();
        glAttachShader(m_program, vertex_shader);
        glAttachShader(m_program, fragment_shader);
        glLinkProgram(m_program);

        check_link_errors(m_program);
    }

    void shader::check_compile_errors(GLuint shader, const std::string &type)
    {
        GLint success;
        GLchar info_log[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, info_log);
            std::cerr << "Shader compilation error (" << type << "):\n"
                      << info_log << std::endl;
        }
    }

    void shader::check_link_errors(GLuint program)
    {
        GLint success;
        GLchar info_log[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 1024, nullptr, info_log);
            std::cerr << "Shader linking error:\n"
                      << info_log << std::endl;
        }
    }

    GLint shader::get_uniform_location(const std::string &name) const
    {
        if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
        {
            return m_uniform_location_cache[name];
        }

        GLint location = glGetUniformLocation(m_program, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning: uniform '" << name << "' doesn't exist" << std::endl;
        }

        m_uniform_location_cache[name] = location;
        return location;
    }

    void shader::set_int(const std::string &name, int value) const
    {
        glUniform1i(get_uniform_location(name), value);
    }

    void shader::set_float(const std::string &name, float value) const
    {
        glUniform1f(get_uniform_location(name), value);
    }

    void shader::set_vec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(get_uniform_location(name), 1, &value[0]);
    }

    void shader::set_vec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(get_uniform_location(name), 1, &value[0]);
    }

    void shader::set_vec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(get_uniform_location(name), 1, &value[0]);
    }

    void shader::set_mat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
    }

    void shader::set_mat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
    }

} // namespace core
