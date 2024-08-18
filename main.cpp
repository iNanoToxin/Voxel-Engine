#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include "camera.h"
#include "window.h"
#include "common/constants.h"
#include "game/data/world_data.h"
#include "rendering/grid_overlay.h"
#include "rendering/shader.h"
#include "rendering/skybox.h"
#include "rendering/gl/cube_map_array.h"
#include "utilities/json.h"

#include "utilities/FastNoiseLite.h"
#include "game/chunk.h"
#include "game/chunk_map.h"
#include "rendering/gl/texture_2d_array.h"




int main()
{
    voxel_engine::window window(2560 / 2, 1440 / 2, "Voxel Engine", true);
    voxel_engine::camera camera(window);
    voxel_engine::grid_overlay grid_overlay;

    voxel_engine::world_data world_data(GET_DATA("world_data.json"));
    voxel_engine::skybox skybox(
        {
            GET_TEXTURE("skybox/right.jpg"),
            GET_TEXTURE("skybox/left.jpg"),
            GET_TEXTURE("skybox/top.jpg"),
            GET_TEXTURE("skybox/bottom.jpg"),
            GET_TEXTURE("skybox/front.jpg"),
            GET_TEXTURE("skybox/back.jpg")
        }
    );

    camera.position = world_data.get_vec3("camera_position");
    camera.look_at(camera.position + world_data.get_vec3("camera_front"));


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    ImGuiIO* io = &ImGui::GetIO();
    io->AddMouseButtonEvent(GLFW_MOUSE_BUTTON_1, true);



    #pragma region INIT_VOXEL_TEXTURE_2D_ARRAY
    std::vector<std::array<std::string, 6>> block_data = nlohmann::json::parse(voxel_engine::util::read_file(GET_DATA("block_data.json")));
    std::vector<std::string> texture_paths;

    for (uint32_t i = 0; i < block_data.size(); i++)
    {
        for (uint32_t j = 0; j < 6; j++)
        {
            texture_paths.push_back(TEXTURES_PATH + block_data[i][j]);
        }
    }

    voxel_engine::texture_2d_array texture_2d_array;
    texture_2d_array.load_texture_2d_array(texture_paths);
    texture_2d_array.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture_2d_array.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture_2d_array.set_texture_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture_2d_array.set_texture_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture_2d_array.set_texture_parameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    #pragma endregion


    voxel_engine::chunk_map map;


    voxel_engine::shader vp_shader("vp.vert", "vp.frag");

    uint32_t ssbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ssbo);

    std::vector<quad_data> ssbo_data;

    int32_t size = 8;

    for (int32_t y = 0; y < 2; y++)
    {
        for (int32_t x = 0; x < size; x++)
        {
            for (int32_t z = 0; z < size; z++)
            {
                voxel_engine::chunk* chunk = map.create_chunk(glm::ivec3(x, y, z));

                std::vector<quad_data> data = chunk->generate_mesh(&map);

                ssbo_data.insert(ssbo_data.end(), data.begin(), data.end());
            }
        }
    }

    // const uint x = (data.packed_data0 >> 0) & 63;
    // const uint y = (data.packed_data0 >> 6) & 63;
    // const uint z = (data.packed_data0 >> 12) & 63;
    // const uint w = (data.packed_data0 >> 18) & 63;
    // const uint h = (data.packed_data0 >> 24) & 63;
    // const uint face = (data.packed_data1) & 7;
    // const uint type = (data.packed_data1 >> 3) & 255;

    vp_shader.use();
    glBindVertexArray(vao);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_data.size() * sizeof(quad_data), ssbo_data.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);



    while (!window.should_close())
    {
        static bool gl_fill = world_data.get_unsafe("gl_fill") != false;
        static bool show_skybox = world_data.get_unsafe("show_skybox") != false;
        static bool show_grid = world_data.get_unsafe("show_grid") != false;

        window.clear(20, 20, 20);
        glClear(GL_DEPTH_BUFFER_BIT);

        voxel_engine::camera::update_camera(window, camera, io);


        #pragma region DRAW_CUBES
        glPolygonMode(GL_FRONT_AND_BACK, gl_fill ? GL_FILL : GL_LINE);
        vp_shader.use();
        vp_shader.set_mat4("u_View", camera.get_view_matrix());
        vp_shader.set_mat4("u_Projection", camera.get_projection_matrix());
        vp_shader.set_vec3("u_ViewPos", camera.position);
        // vp_shader.set_vec3("u_Light.position", camera.position + glm::vec3(0.0f, 100.0f, 0.0f));
        vp_shader.set_vec3("u_Light.position", glm::vec3(250.0, 1000.0, 750.0) * 10000.0f);
        vp_shader.set_vec3("u_Light.ambient", glm::vec3(0.5f));
        vp_shader.set_vec3("u_Light.diffuse", glm::vec3(1.0f));
        vp_shader.set_vec3("u_Light.specular", glm::vec3(1.0f));
        vp_shader.set_vec3("u_Material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        vp_shader.set_vec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        vp_shader.set_float32("u_Material.shininess", 64.0f);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, ssbo_data.size() * 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        #pragma endregion




        if (show_skybox) skybox.render(camera);
        if (show_grid) grid_overlay.render(camera);

        #pragma region IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(FLT_MAX, FLT_MAX));

        if (ImGui::Begin("Voxel Engine"))
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;

            if (ImGui::BeginTable("Data", 3, flags))
            {
                double x_position, y_position;
                glfwGetCursorPos(window.get_window(), &x_position, &y_position);

                ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Extra", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                ImGui::PushID(0);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Camera Position");
                ImGui::TableSetColumnIndex(1);
                float32_t column_width = ImGui::GetContentRegionAvail().x;
                ImGui::PushItemWidth(column_width);
                ImGui::DragFloat3("##camera_pos", &camera.position[0], 1.0f, 0.0f, 0.0f, "%.03f");
                ImGui::PopItemWidth();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec3(";
                    pos << std::to_string(camera.position.x) << ", ";
                    pos << std::to_string(camera.position.y) << ", ";
                    pos << std::to_string(camera.position.z) << ")";
                    glfwSetClipboardString(window.get_window(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(1);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Camera Yaw/Pitch");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, ImGui::GetStyle().ItemSpacing.y));
                column_width = ImGui::GetContentRegionAvail().x;
                float32_t item_spacing = ImGui::GetStyle().ItemSpacing.x;
                float32_t item_width = (column_width - item_spacing) / 2;
                ImGui::PushItemWidth(item_width);
                if (ImGui::DragFloat("##camera_yaw", &camera.yaw, 0.1f, 0.0f, 0.0f, "%.03f"))
                {
                    camera.contrain_angles();
                    camera.update_vectors();
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                ImGui::PushItemWidth(item_width);
                if (ImGui::DragFloat("##camera_pitch", &camera.pitch, 0.1f, -89.0f, 89.0f, "%.03f"))
                {
                    camera.contrain_angles();
                    camera.update_vectors();
                }
                ImGui::PopItemWidth();
                ImGui::PopStyleVar();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec2(";
                    pos << std::to_string(camera.yaw) << ", ";
                    pos << std::to_string(camera.pitch) << ")";
                    glfwSetClipboardString(window.get_window(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(2);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Look Direction");
                ImGui::TableSetColumnIndex(1);
                // ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                ImGui::Text("%.3f, %.3f, %.3f", camera.front.x, camera.front.y, camera.front.z);
                // ImGui::PopStyleColor();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec3(";
                    pos << std::to_string(camera.front.x) << ", ";
                    pos << std::to_string(camera.front.y) << ", ";
                    pos << std::to_string(camera.front.z) << ")";
                    glfwSetClipboardString(window.get_window(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(3);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Mouse Position");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                ImGui::Text("%.2f, %.2f", x_position, y_position);
                ImGui::PopStyleColor();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec2(";
                    pos << std::to_string(x_position) << ", ";
                    pos << std::to_string(y_position) << ")";
                    glfwSetClipboardString(window.get_window(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(4);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("App Info");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
                ImGui::Text("%.3f ms/frame", 1000.0f / io->Framerate);
                ImGui::PopStyleColor();
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
                ImGui::Text("%.1f fps", io->Framerate);
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
            ImGui::EndTable();

            if (ImGui::BeginTable("Chunk Data", 2, flags))
            {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                int32_t x = glm::floor(camera.position.x / CHUNK_SIZE);
                int32_t y = glm::floor(camera.position.y / CHUNK_SIZE);
                int32_t z = glm::floor(camera.position.z / CHUNK_SIZE);
                ImGui::PushID(0);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Current Chunk");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("Chunk{%i, %i, %i}", x, y, z);
                ImGui::PopID();

                ImGui::PushID(1);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Face Count");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%i", ssbo_data.size());
                ImGui::PopID();

                ImGui::PushID(2);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Triangle Count");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%i", ssbo_data.size() * 6);
                ImGui::PopID();

                ImGui::PushID(3);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Clear Voxels");
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("Clear"))
                {
                    // ssbo_data.clear();
                }
                ImGui::PopID();
            }
            ImGui::EndTable();

            ImGui::Checkbox("GL_FILL", &gl_fill);
            ImGui::Checkbox("SHOW_SKYBOX", &show_skybox);
            ImGui::Checkbox("SHOW_GRID", &show_grid);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #pragma endregion

        #pragma region SAVE_DATA
        static float32_t last_saved = glfwGetTime();
        if (glfwGetTime() - last_saved >= 0.1)
        {
            world_data.set_vec3("camera_position", camera.position);
            world_data.set_vec3("camera_front", camera.front);
            world_data.set("gl_fill", gl_fill);
            world_data.set("show_skybox", show_skybox);
            world_data.set("show_grid", show_grid);
            world_data.save();
            last_saved = glfwGetTime();
        }
        #pragma endregion
        window.swap();
    }

    return 0;
}
