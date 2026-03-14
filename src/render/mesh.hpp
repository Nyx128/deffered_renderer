#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "core/buffer.hpp"
#include "core/shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <memory>

namespace render {


    struct vertex_gpu {
        glm::vec4 position;
        glm::vec4 normal;
        glm::vec4 uv;
    };

    class mesh {
    public:

        std::vector<vertex_gpu> vertices;
        std::vector<unsigned int> indices;


        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f); // Euler angles (degrees)
        glm::vec3 scale = glm::vec3(1.0f);

        mesh(std::vector<vertex_gpu> vertices, std::vector<unsigned int> indices);
        ~mesh();

        mesh(mesh&&) noexcept = default;
        mesh& operator=(mesh&&) noexcept = default;


        mesh(const mesh&) = delete;
        mesh& operator=(const mesh&) = delete;

        void draw(const core::shader& shader_program, const glm::mat4& parent_transform = glm::mat4(1.0f)) const;
        
        glm::mat4 get_local_matrix() const {
             glm::mat4 mat = glm::mat4(1.0f);
             mat = glm::translate(mat, position);
             mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
             mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
             mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
             mat = glm::scale(mat, scale);
             return mat;
        }

    private:
        std::unique_ptr<core::buffer> vertex_buffer;
        std::unique_ptr<core::buffer> index_buffer; 
        
        void setup_mesh();
    };

}
