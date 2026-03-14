#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/window.hpp"

#include <vector>

namespace render {

    // Default camera values
    const float YAW_DEFAULT         = -90.0f;
    const float PITCH_DEFAULT       =  0.0f;
    const float SPEED_DEFAULT       =  2.5f;
    const float SENSITIVITY_DEFAULT =  0.1f;
    const float ZOOM_DEFAULT        =  45.0f;

    class camera {
    public:
        // Camera Attributes
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;

        float yaw;
        float pitch;
        float movement_speed;
        float mouse_sensitivity;
        float zoom;

        camera(const core::window& window, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW_DEFAULT, float pitch = PITCH_DEFAULT) 
            : front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED_DEFAULT), mouse_sensitivity(SENSITIVITY_DEFAULT), zoom(ZOOM_DEFAULT), m_window(window) {
            this->position = position;
            this->world_up = up;
            this->yaw = yaw;
            this->pitch = pitch;
            update_camera_vectors();
        }

        glm::mat4 get_view_matrix() {
            return glm::lookAt(position, position + front, up);
        }

        glm::mat4 get_projection_matrix() const {
             return glm::perspective(glm::radians(zoom), static_cast<float>(m_window.get_width()) / static_cast<float>(m_window.get_height()), 0.1f, 100.0f);
        }

        void update(float delta_time) {
            process_keyboard(delta_time);
            process_mouse();
        }

    private:
        const core::window& m_window;
        double last_mouse_x = 0.0;
        double last_mouse_y = 0.0;
        bool first_mouse = true;

        void process_keyboard(float delta_time) {
            float velocity = movement_speed * delta_time;
            
            if (m_window.is_key_held(GLFW_KEY_W))
                position += front * velocity;
            if (m_window.is_key_held(GLFW_KEY_S))
                position -= front * velocity;
            if (m_window.is_key_held(GLFW_KEY_A))
                position -= right * velocity;
            if (m_window.is_key_held(GLFW_KEY_D))
                position += right * velocity;
            if (m_window.is_key_held(GLFW_KEY_Q))
                position += up * velocity;
            if (m_window.is_key_held(GLFW_KEY_E))
                position -= up * velocity;
        }

        void process_mouse(GLboolean constrain_pitch = true) {
            if (m_window.is_mouse_button_down(GLFW_MOUSE_BUTTON_RIGHT)) {
                double xpos, ypos;
                m_window.get_cursor_pos(xpos, ypos);

                if (first_mouse) {
                    last_mouse_x = xpos;
                    last_mouse_y = ypos;
                    first_mouse = false;
                }

                float xoffset = static_cast<float>(xpos - last_mouse_x);
                float yoffset = static_cast<float>(last_mouse_y - ypos); 

                last_mouse_x = xpos;
                last_mouse_y = ypos;

                xoffset *= mouse_sensitivity;
                yoffset *= mouse_sensitivity;

                yaw   += xoffset;
                pitch += yoffset;

                if (constrain_pitch) {
                    if (pitch > 89.0f)
                        pitch = 89.0f;
                    if (pitch < -89.0f)
                        pitch = -89.0f;
                }

                update_camera_vectors();
            } else {
                first_mouse = true;
            }
        }

        // Calculates the front vector from the Camera's (updated) Euler Angles
        void update_camera_vectors() {
            // Calculate the new Front vector
            glm::vec3 new_front;
            new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            new_front.y = sin(glm::radians(pitch));
            new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(new_front);
            // Also re-calculate the Right and Up vector
            right = glm::normalize(glm::cross(front, world_up));  // Normalize the vectors
            up    = glm::normalize(glm::cross(right, front));
        }
    };
}
