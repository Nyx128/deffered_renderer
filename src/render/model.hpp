#pragma once

#include "mesh.hpp"
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tinygltf.h>

namespace render {

    class model {
    public:
        std::vector<std::unique_ptr<mesh>> meshes;
        model(const std::string& path);
        // Empty constructor for manual creation
        model() = default;
        
        // Transform
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        void draw(const core::shader& shader_program);
        
        glm::mat4 get_model_matrix() const;

    private:
        void load_model(const std::string& path);
        void process_node(const tinygltf::Model& model, const tinygltf::Node& node, const glm::mat4& parent_transform);
    };

}
