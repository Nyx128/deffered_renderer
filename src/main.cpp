#include "core/window.hpp"
#include "render/model.hpp"
#include "render/camera.hpp"
#include "render/fbo.hpp"
#include "render/screen_quad.hpp"
#include "core/shader.hpp"

#include "render/render_pipeline.hpp"
#include "render/passes/geometry_pass.hpp"
#include "render/passes/lighting_pass.hpp"

#include "core/imgui_context.hpp"
#include <imgui/imgui.h>

#include "utils/resource_tracker.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

//hack to pick discrete gpu
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{
    core::window window(WINDOW_WIDTH, WINDOW_HEIGHT, "render_graph");

    render::camera cam(window, glm::vec3(0.0f, 0.0f, 3.0f));

    // Resources
    core::shader geometry_shader("shaders/geometry.vert", "shaders/geometry.frag");
    std::unique_ptr<render::model> dragon = std::make_unique<render::model>("assets/models/DragonAttenuation.glb");
    render::screen_quad quad;

    // ImGui
    core::imgui_context imgui_ctx(window.get_native_window());

    // Pipeline
    render::render_pipeline pipeline(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Add Passes
    pipeline.add_pass(std::make_unique<render::geometry_pass>(dragon.get(), &cam, &geometry_shader));
    pipeline.add_pass(std::make_unique<render::lighting_pass>(&cam, WINDOW_WIDTH, WINDOW_HEIGHT));

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    while (!window.should_close())
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        cam.update(delta_time);

        GLuint output_tex = pipeline.render();

        imgui_ctx.new_frame();
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoNavFocus;
        
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);
        
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();
        
        // Viewport window
        ImGui::Begin("Viewport");
        
        // Calculate aspect ratio
        float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        
        // Calculate image size maintaining aspect ratio
        ImVec2 imageSize;
        float availAspect = availSize.x / availSize.y;
        
        if (availAspect > aspectRatio) {
            imageSize.y = availSize.y;
            imageSize.x = availSize.y * aspectRatio;
        } else {
            imageSize.x = availSize.x;
            imageSize.y = availSize.x / aspectRatio;
        }
        
        ImVec2 cursorPos = ImGui::GetCursorPos();
        cursorPos.x += (availSize.x - imageSize.x) * 0.5f;
        cursorPos.y += (availSize.y - imageSize.y) * 0.5f;
        ImGui::SetCursorPos(cursorPos);
        
        ImGui::Image((void*)(intptr_t)output_tex, imageSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        
        // Stats window
        ImGui::Begin("Stats");
        ImGui::Text("Performance");
        ImGui::Separator();
        ImGui::Text("FPS: %d", static_cast<int>(1.0f / delta_time));
        ImGui::Text("Frame Time: %.2f ms", delta_time * 1000.0f);
        
        ImGui::Spacing();
        ImGui::Text("Resources");
        ImGui::Separator();
        size_t ram = utils::resource_tracker::get_ram_usage();
        size_t vram = utils::resource_tracker::get_vram_usage();
        double ram_mb = ram / (1024.0 * 1024.0);
        double vram_mb = vram / (1024.0 * 1024.0);
        ImGui::Text("RAM: %.2f MB", ram_mb);
        ImGui::Text("VRAM: %.2f MB", vram_mb);
        ImGui::End();
        
        // Render ImGui to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        imgui_ctx.render();

        window.swap_buffers();
        window.poll_events();
    }

    return 0;
}
