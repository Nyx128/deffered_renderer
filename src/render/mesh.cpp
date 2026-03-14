#include "mesh.hpp"
#include <glad/glad.h>
#include "utils/resource_tracker.hpp"

namespace render {

    mesh::mesh(std::vector<vertex_gpu> vertices, std::vector<unsigned int> indices)
        : vertices(std::move(vertices)), indices(std::move(indices))
    {
        size_t size = this->vertices.size() * sizeof(vertex_gpu) + this->indices.size() * sizeof(unsigned int);
        utils::resource_tracker::allocate_ram(size);
        setup_mesh();
    }

    mesh::~mesh() {
        size_t size = this->vertices.size() * sizeof(vertex_gpu) + this->indices.size() * sizeof(unsigned int);
        utils::resource_tracker::free_ram(size);
    }

    void mesh::setup_mesh()
    {
        vertex_buffer = std::make_unique<core::buffer>(
            this->vertices.data(), 
            this->vertices.size() * sizeof(vertex_gpu), 
            core::buffer_usage::static_draw
        );

        if (!this->indices.empty()) {
            index_buffer = std::make_unique<core::buffer>(
                this->indices.data(),
                this->indices.size() * sizeof(unsigned int),
                core::buffer_usage::static_draw
            );
        }
    }

    void mesh::draw(const core::shader& shader_program, const glm::mat4& parent_transform) const
    {
        glm::mat4 model_matrix = parent_transform * get_local_matrix();
        shader_program.set_mat4("model", model_matrix);

        vertex_buffer->bind(0);

        if (index_buffer && !indices.empty()) {
             index_buffer->bind(1);
             glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(indices.size()));
        } else {

            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        }
    }

}
