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
#include "rendering/gl/vertex_array.h"
#include "rendering/gl/vertex_buffer.h"
#include "utilities/json.h"

#include "utilities/FastNoiseLite.h"
#include "game/chunk.h"


using nlohmann::json;

void insert_voxels(std::vector<block>& _data, const std::vector<block>& _blocks)
{
    for (uint32_t i = 0; i < _blocks.size(); i++)
    {
        _data.push_back(_blocks[i]);
    }
}

void insert_voxel_faces(std::vector<face>& _data, const std::vector<face>& _blocks)
{
    for (uint32_t i = 0; i < _blocks.size(); i++)
    {
        _data.push_back(_blocks[i]);
    }
}

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

    std::vector<block> voxel_data;



    #pragma region INIT_VOXEL_CUBE_MAP_ARRAY
    std::vector<std::array<std::string, 6>> block_data = json::parse(voxel_engine::util::read_file(GET_DATA("block_data.json")));

    for (uint32_t i = 0; i < block_data.size(); i++)
    {
        for (uint32_t j = 0; j < 6; j++)
        {
            block_data[i][j] = TEXTURES_PATH + block_data[i][j];
        }
    }

    voxel_engine::cube_map_array cube_map_array;
    cube_map_array.load_cube_map_array(block_data);
    cube_map_array.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    cube_map_array.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    cube_map_array.set_texture_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    cube_map_array.set_texture_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    cube_map_array.set_texture_parameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    #pragma endregion


    // 00 tex_id: [000000000] size: [x: 000000 y: 000000 z: 000000] face: [000]

    voxel_engine::shader voxel_shader(GET_SHADER("voxel.vert"), GET_SHADER("voxel.frag"));
    voxel_engine::vertex_array vertex_array;
    voxel_engine::vertex_buffer vertex_buffer(GL_ARRAY_BUFFER);


    voxel_engine::chunk chunk = voxel_engine::chunk(glm::vec3(0, 0, 0));

    std::vector<face> v0_faces = chunk.get_voxel_faces();
    std::vector<face> v0_faces_greedy = chunk.get_voxel_faces_greedy();

    std::vector<face> faces;
    insert_voxel_faces(faces, v0_faces);
    insert_voxel_faces(faces, v0_faces_greedy);

    // for (int32_t i = 0; i <= 64; i++)
    // {
    //     for (int32_t j = 0; j <= 64; j++)
    //     {
    //         voxel_engine::chunk chunk(glm::ivec3(i, 0, j));
    //         insert_voxel_faces(faces, chunk.get_voxel_faces_greedy());
    //     }
    // }

    vertex_array.bind_vertex_array();
    vertex_buffer.set_buffer_data(sizeof(face) * faces.size(), faces.data(), GL_STATIC_DRAW);

    vertex_array.set_stride(sizeof(face));
    vertex_array.add_attribute_float32(0, 4, GL_FLOAT, GL_FALSE, sizeof(float32_t));
    vertex_array.add_attribute_float32(1, 4, GL_FLOAT, GL_FALSE, sizeof(float32_t));
    vertex_array.add_attribute_float32(2, 4, GL_FLOAT, GL_FALSE, sizeof(float32_t));
    vertex_array.add_attribute_float32(3, 4, GL_FLOAT, GL_FALSE, sizeof(float32_t));
    vertex_array.add_attribute_int32(4, 1, GL_INT, sizeof(int32_t));
    vertex_array.add_attribute_int32(5, 1, GL_INT, sizeof(int32_t));

    vertex_array.update_attribute_per_instance(0, 1);
    vertex_array.update_attribute_per_instance(1, 1);
    vertex_array.update_attribute_per_instance(2, 1);
    vertex_array.update_attribute_per_instance(3, 1);
    vertex_array.update_attribute_per_instance(4, 1);
    vertex_array.update_attribute_per_instance(5, 1);

    voxel_engine::texture texture(GL_TEXTURE_2D);
    voxel_engine::texture_data image(GET_TEXTURE("blocks/smooth_stone.png"));

    texture.bind_texture();
    glTexImage2D(GL_TEXTURE_2D, 0, image.format, image.width, image.height, 0, image.format, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    texture.set_texture_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture.set_texture_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    while (!window.should_close())
    {
        static bool gl_fill = world_data.get<bool>("gl_fill");
        static bool show_skybox = world_data.get<bool>("show_skybox");
        static bool show_grid = world_data.get<bool>("show_grid");

        window.clear(20, 20, 20);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::vec3 camera_position = camera.position - glm::vec3(0.0f, 32.0f, 0.0f);
        const int32_t chunk_x = glm::floor(camera_position.x / CHUNK_SIZE);
        const int32_t chunk_y = glm::floor(camera_position.y / CHUNK_SIZE);
        const int32_t chunk_z = glm::floor(camera_position.z / CHUNK_SIZE);
        glm::ivec3 chunk_position(chunk_x, chunk_y, chunk_z);


        #pragma region INPUT_HANDLER
        if (!io->WantCaptureMouse && (glfwGetMouseButton(window.get_window(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ||
            glfwGetMouseButton(window.get_window(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS))
        {
            camera.capture();
        }
        else
        {
            camera.release();
        }
        if (glfwGetKey(window.get_window(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.get_window(), true);
        }
        camera.update_position();
        #pragma endregion

        #pragma region DRAW_CUBES
        glPolygonMode(GL_FRONT_AND_BACK, gl_fill ? GL_FILL : GL_LINE);

        /*voxel_shader.use();
        voxel_shader.set_mat4("u_View", camera.get_view_matrix());
        voxel_shader.set_mat4("u_Projection", camera.get_projection_matrix());
        voxel_shader.set_vec3("u_ViewPos", camera.position);
        voxel_shader.set_vec3("u_Light.position", camera.position + glm::vec3(0.0f, 100.0f, 0.0f));
        voxel_shader.set_vec3("u_Light.ambient", glm::vec3(0.5f));
        voxel_shader.set_vec3("u_Light.diffuse", glm::vec3(1.0f));
        voxel_shader.set_vec3("u_Light.specular", glm::vec3(1.0f));
        voxel_shader.set_vec3("u_Material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        voxel_shader.set_vec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        voxel_shader.set_float32("u_Material.shininess", 64.0f);
        // voxel_shader.set_int32("u_Material.diffuse", 0);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, atlas_texture);
        // voxel_shader.set_i32("u_TextureArray", texture_array);
        voxel_shader.set_int32("u_Texture", 0);
        cube_map_array.set_active_texture(GL_TEXTURE0);

        // voxel_data[0].transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, glm::sin(glfwGetTime()), 0.0));
        // voxel_data[1].block_type = static_cast<uint32_t>(glfwGetTime() * 30) % 3;

        vertex_array.bind_vertex_array();
        // vertex_buffer.set_buffer_data(sizeof(block) * voxel_data.size(), voxel_data.data(), GL_STATIC_DRAW);
        vertex_array.draw_arrays_instanced(GL_TRIANGLES, 0, 36, voxel_data.size());*/

        voxel_shader.use();
        voxel_shader.set_mat4("u_View", camera.get_view_matrix());
        voxel_shader.set_mat4("u_Projection", camera.get_projection_matrix());
        voxel_shader.set_vec3("u_ViewPos", camera.position);
        voxel_shader.set_vec3("u_Light.position", camera.position + glm::vec3(0.0f, 100.0f, 0.0f));
        voxel_shader.set_vec3("u_Light.ambient", glm::vec3(0.5f));
        voxel_shader.set_vec3("u_Light.diffuse", glm::vec3(1.0f));
        voxel_shader.set_vec3("u_Light.specular", glm::vec3(1.0f));
        voxel_shader.set_vec3("u_Material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        voxel_shader.set_vec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        voxel_shader.set_float32("u_Material.shininess", 64.0f);
        voxel_shader.set_int32("u_Texture", 0);
        texture.set_active_texture(GL_TEXTURE0);
        vertex_array.draw_arrays_instanced(GL_TRIANGLES, 0, 6, faces.size());

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
                ImGui::Text("%i", faces.size());
                ImGui::PopID();

                ImGui::PushID(2);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Triangle Count");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%i", faces.size() * 2);
                ImGui::PopID();

                ImGui::PushID(3);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Clear Voxels");
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("Clear"))
                {
                    faces.clear();
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

        world_data.set_vec3("camera_position", camera.position);
        world_data.set_vec3("camera_front", camera.front);
        world_data.set("gl_fill", gl_fill);
        world_data.set("show_skybox", show_skybox);
        world_data.set("show_grid", show_grid);
        world_data.save();
        window.swap();
    }

    return 0;
}
