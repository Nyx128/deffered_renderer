#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace core {

    class compute_shader {
    public:
        compute_shader(const std::string& path);
        ~compute_shader();

        void bind() const;
        void unbind() const;
        
        // Dispatch the compute shader
        void dispatch(unsigned int x, unsigned int y, unsigned int z);
        
        // Wait/Barrier
        void wait_memory(GLbitfield barriers);

        // Uniforms
        void set_int(const std::string& name, int value) const;
        void set_float(const std::string& name, float value) const;
        void set_vec3(const std::string& name, const glm::vec3& value) const;
        void set_vec4(const std::string& name, const glm::vec4& value) const;

    private:
        std::string m_path;
        GLuint m_program;
        mutable std::unordered_map<std::string, GLint> m_uniform_cache;

        std::string read_file(const std::string& path);
        GLint get_uniform_location(const std::string& name) const;
    };

}
