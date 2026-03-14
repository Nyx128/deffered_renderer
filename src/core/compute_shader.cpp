#include "compute_shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace core {

    compute_shader::compute_shader(const std::string& path) : m_path(path), m_program(0) {
        std::string code = read_file(path);
        const char* src = code.c_str();

        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Compute Shader Compilation Error (" << path << "):\n" << infoLog << std::endl;
        }

        m_program = glCreateProgram();
        glAttachShader(m_program, shader);
        glLinkProgram(m_program);

        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if(!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_program, 512, NULL, infoLog);
            std::cerr << "Compute Shader Linking Error:\n" << infoLog << std::endl;
        }

        glDeleteShader(shader);
    }

    compute_shader::~compute_shader() {
        glDeleteProgram(m_program);
    }

    void compute_shader::bind() const {
        glUseProgram(m_program);
    }

    void compute_shader::unbind() const {
        glUseProgram(0);
    }

    void compute_shader::dispatch(unsigned int x, unsigned int y, unsigned int z) {
        glDispatchCompute(x, y, z);
    }

    void compute_shader::wait_memory(GLbitfield barriers) {
        glMemoryBarrier(barriers);
    }

    std::string compute_shader::read_file(const std::string& path) {
        std::ifstream file(path);
        std::stringstream ss;
        if(file.is_open()) {
            ss << file.rdbuf();
            file.close();
            return ss.str();
        } else {
            std::cerr << "Failed to open compute shader: " << path << std::endl;
            return "";
        }
    }

    GLint compute_shader::get_uniform_location(const std::string& name) const {
        if(m_uniform_cache.find(name) != m_uniform_cache.end())
            return m_uniform_cache[name];
        
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        m_uniform_cache[name] = loc;
        return loc;
    }

    void compute_shader::set_int(const std::string& name, int value) const {
        glUniform1i(get_uniform_location(name), value);
    }

    void compute_shader::set_float(const std::string& name, float value) const {
        glUniform1f(get_uniform_location(name), value);
    }

    void compute_shader::set_vec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(get_uniform_location(name), 1, &value[0]);
    }
    
    void compute_shader::set_vec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(get_uniform_location(name), 1, &value[0]);
    }

}
