#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace core
{
    class shader
    {
    public:
        shader(const std::string &vertex_path, const std::string &fragment_path);
        ~shader();

        shader(const shader &) = delete;
        shader &operator=(const shader &) = delete;

        void bind() const;
        void unbind() const;

        bool reload();

        GLuint id() const { return m_program; }

    private:
        GLuint m_program;
        std::string m_vertex_path;
        std::string m_fragment_path;

        std::string read_file(const std::string &filepath);
        GLuint compile_shader(GLenum type, const std::string &source);
        void link_program(GLuint vertex_shader, GLuint fragment_shader);
        void check_compile_errors(GLuint shader, const std::string &type);
        void check_link_errors(GLuint program);

        GLint get_uniform_location(const std::string &name) const;
        mutable std::unordered_map<std::string, GLint> m_uniform_location_cache;

    public:
        void set_int(const std::string &name, int value) const;
        void set_float(const std::string &name, float value) const;
        void set_vec2(const std::string &name, const glm::vec2 &value) const;
        void set_vec3(const std::string &name, const glm::vec3 &value) const;
        void set_vec4(const std::string &name, const glm::vec4 &value) const;
        void set_mat3(const std::string &name, const glm::mat3 &mat) const;
        void set_mat4(const std::string &name, const glm::mat4 &mat) const;
    };

} // namespace core
