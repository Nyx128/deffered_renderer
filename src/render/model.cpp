#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "model.hpp"
#include <iostream>

#include "utils/scope_timer.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace render {

    model::model(const std::string& path) {
        load_model(path);
    }

    void model::draw(const core::shader& shader_program) {
        glm::mat4 mat = get_model_matrix();
        for (const auto& m : meshes) {
            m->draw(shader_program, mat);
        }
    }
    
    glm::mat4 model::get_model_matrix() const {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, position);
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        mat = glm::scale(mat, scale);
        return mat;
    }

    void model::load_model(const std::string& path) {
        tinygltf::Model gltf_model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = false;
        if (path.find(".glb") != std::string::npos) {
             ret = loader.LoadBinaryFromFile(&gltf_model, &err, &warn, path);
        } else {
             ret = loader.LoadASCIIFromFile(&gltf_model, &err, &warn, path);
        }

        if (!warn.empty()) {
             std::cout << "GLTF Warning: " << warn << std::endl;
        }

        if (!err.empty()) {
             std::cout << "GLTF Error: " << err << std::endl;
        }

        if (!ret) {
             std::cout << "Failed to parse GLTF: " << path << std::endl;
             return;
        }

        const tinygltf::Scene& scene = gltf_model.scenes[gltf_model.defaultScene > -1 ? gltf_model.defaultScene : 0];
        for (int node_idx : scene.nodes) {
            process_node(gltf_model, gltf_model.nodes[node_idx], glm::mat4(1.0f));
        }
    }

    void model::process_node(const tinygltf::Model& model, const tinygltf::Node& node, const glm::mat4& parent_transform) {
        glm::vec3 pos = glm::vec3(0.0f);
        glm::vec3 rot = glm::vec3(0.0f);
        glm::vec3 scl = glm::vec3(1.0f);

        if (node.translation.size() == 3) {
            pos = glm::vec3(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2]));
        }

        if (node.rotation.size() == 4) {
            glm::quat q = glm::quat(static_cast<float>(node.rotation[3]), static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]));
            rot = glm::degrees(glm::eulerAngles(q));
        }

        if (node.scale.size() == 3) {
            scl = glm::vec3(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2]));
        }

        if (node.mesh > -1) {
            const auto& gltf_mesh = model.meshes[node.mesh];
            for (const auto& primitive : gltf_mesh.primitives) {
                utils::scope_timer model_timer("model_timer");
                // We only support triangles for now
                if (primitive.mode != TINYGLTF_MODE_TRIANGLES) continue;

                std::vector<vertex_gpu> vertices;
                std::vector<unsigned int> indices;

                // --- Attributes ---
                const float* positions = nullptr;
                const float* normals = nullptr;
                const float* uvs = nullptr;
                
                int vertex_count = 0;

                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                    const auto& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
                    const auto& buffer_view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[buffer_view.buffer];
                    positions = reinterpret_cast<const float*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                    vertex_count = accessor.count;
                }

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const auto& accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                    const auto& buffer_view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[buffer_view.buffer];
                    normals = reinterpret_cast<const float*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                }

                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                    const auto& accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    const auto& buffer_view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[buffer_view.buffer];
                    uvs = reinterpret_cast<const float*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                }

                if (vertex_count > 0) {
                    vertices.reserve(vertex_count);
                }

                // Pack into vertex_gpu (vec4s)
                for (int i = 0; i < vertex_count; i++) {
                    vertex_gpu v;
                    
                    if (positions) {
                        v.position = glm::vec4(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2], 1.0f);
                    } else {
                        v.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                    }

                    if (normals) {
                        v.normal = glm::vec4(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2], 0.0f);
                    } else {
                        v.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
                    }

                    if (uvs) {
                        v.uv = glm::vec4(uvs[i * 2 + 0], uvs[i * 2 + 1], 0.0f, 0.0f);
                    } else {
                        v.uv = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                    }
                    
                    vertices.push_back(v);
                }

                // --- Indices ---
                if (primitive.indices >= 0) {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& buffer_view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[buffer_view.buffer];

                    indices.reserve(accessor.count);
                    
                    // Indices can be unsigned byte, short, or int
                    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                         const unsigned short* buf = reinterpret_cast<const unsigned short*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                         for (size_t i = 0; i < accessor.count; ++i) {
                             indices.push_back(static_cast<unsigned int>(buf[i]));
                         }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                         const unsigned int* buf = reinterpret_cast<const unsigned int*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                         for (size_t i = 0; i < accessor.count; ++i) {
                             indices.push_back(buf[i]);
                         }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                         const unsigned char* buf = reinterpret_cast<const unsigned char*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                         for (size_t i = 0; i < accessor.count; ++i) {
                             indices.push_back(static_cast<unsigned int>(buf[i]));
                         }
                    }
                }

                auto new_mesh = std::make_unique<mesh>(std::move(vertices), std::move(indices));
                new_mesh->position = pos;
                new_mesh->rotation = rot;
                new_mesh->scale = scl;
                meshes.push_back(std::move(new_mesh));
            }
        }

        for (int child_idx : node.children) {
            process_node(model, model.nodes[child_idx], glm::mat4(1.0f));
        }
    }



}
