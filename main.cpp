#include <iostream>
#include <unordered_map>
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "window.h"
#include "rendering/grid_overlay.h"
#include "rendering/shader.h"
#include "rendering/vao.h"

#include "utilities/json.h"
#include "utilities/PerlinNoise.hpp"

#define GET_DATA(PATH) DATA_PATH PATH
#define GET_SHADER(PATH) SHADERS_PATH PATH
#define GET_TEXTURE(PATH) TEXTURES_PATH PATH
#define CHUNK_SIZE 32

using nlohmann::json;

constexpr siv::PerlinNoise::seed_type seed = 123456u;
const siv::PerlinNoise perlin(seed);


enum class BlockType : u8
{

};

struct BlockData
{
    u8 blockType;
    u8 lightLevel;
};


class Chunk
{
    //  data[-----X----][-----Z----][-----Y----]
    int data[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {};
    glm::ivec3 m_Position;
public:
    explicit Chunk(const glm::ivec3& p_Position) : m_Position(p_Position) {}

    void generate(std::vector<glm::mat4>& mat) const
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    const int world_x = m_Position.x * CHUNK_SIZE + x;
                    const int world_y = m_Position.y * CHUNK_SIZE + y;
                    const int world_z = m_Position.z * CHUNK_SIZE + z;

                    // double noise = perlin.octave2D_01(world_x * 0.01, world_z * 0.01, 4);
                    // noise *= 25;
                    // noise = glm::floor(noise);
                    double noise = 32.0f;

                    if (world_y <= noise) mat.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(world_x, world_y, world_z) + glm::vec3(0.5)));
                }
            }
        }
    }
};

class ChunkManager
{
private:
    struct ivec3_hash
    {
        std::size_t operator()(const glm::ivec3 &in) const
        {
            std::size_t seed = 0;
            std::hash<int> hasher;
            seed ^= hasher(in.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hasher(in.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hasher(in.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };


    std::unordered_map<glm::ivec3, Chunk, ivec3_hash> chunks;

    void _create_chunk_at_chunk_position(const glm::ivec3& position, std::vector<glm::mat4>& mat)
    {
        Chunk chunk(position);
        chunk.generate(mat);
        chunks.emplace(position, chunk);
    }

    void _load_chunk_at_chunk_position(const glm::ivec3& position, std::vector<glm::mat4>& mat)
    {
        if (!chunks.contains(position))
        {
            _create_chunk_at_chunk_position(position, mat);
        }
    }

    void _load_chunk_at_world_position(const glm::vec3& position, std::vector<glm::mat4>& mat)
    {
        const int chunk_x = glm::floor(position.x / CHUNK_SIZE);
        const int chunk_y = glm::floor(position.y / CHUNK_SIZE);
        const int chunk_z = glm::floor(position.z / CHUNK_SIZE);
        _load_chunk_at_chunk_position(glm::ivec3(chunk_x, chunk_y, chunk_z), mat);
    }
public:
    void loadChunk(const glm::vec3& position, std::vector<glm::mat4>& mat)
    {
        _load_chunk_at_world_position(position, mat);
    }

    void loadChunk(const glm::ivec3& position, std::vector<glm::mat4>& mat)
    {
        _load_chunk_at_chunk_position(position, mat);
    }
};


int main()
{
    VoxelEngine::Window window(2560 / 2, 1440 / 2, "Voxel Engine", true);

    VoxelEngine::Shader shader(GET_SHADER("shader.vert"), GET_SHADER("shader.frag"));
    VoxelEngine::Camera camera(&window);
    VoxelEngine::GridOverlay grid_overlay;

    std::vector<glm::mat4> mat;
    VoxelEngine::Vao cube_vao;
    VoxelEngine::Vbo cube_vbo, cube_ibo;
    {
        // @formatter:off
        float vertices[288] = {
            // position[3], normal[3], tex_coord[2]
            // back face (CCW winding)
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,    // bottom-left
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,    // bottom-right
           -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,    // top-right
           -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,    // top-right
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,    // top-left
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,    // bottom-left
            // front face (CCW winding)
           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,    // bottom-left
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,    // bottom-right
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,    // top-right
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,    // top-right
           -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,    // top-left
           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,    // bottom-left
            // left face (CCW)
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
           -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
           -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,    // top-left
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
            // right face (CCW)
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,    // top-right
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,    // top-left
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
            // bottom face (CCW)
           -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,    // top-right
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,    // top-right
           -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,    // top-left
           -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
            // top face (CCW)
           -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,    // bottom-right
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,    // top-right
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,    // top-right
           -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,    // top-left
           -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,    // bottom-left
        };
        // @formatter:on

        cube_vbo.data(cube_vao, sizeof(vertices), vertices);
        cube_vao.link(cube_vbo, 8 * sizeof(float));
        cube_vao.addAttribute(3, GL_FLOAT);
        cube_vao.addAttribute(3, GL_FLOAT);
        cube_vao.addAttribute(2, GL_FLOAT);


        cube_ibo.data(cube_vao, sizeof(glm::mat4) * mat.size(), mat.data());
        cube_vao.link(cube_ibo, sizeof(glm::mat4));
        cube_vao.addAttribute(4, GL_FLOAT);
        cube_vao.addAttribute(4, GL_FLOAT);
        cube_vao.addAttribute(4, GL_FLOAT);
        cube_vao.addAttribute(4, GL_FLOAT);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }

    json data = json::parse(VoxelEngine::Util::read_file(GET_DATA("world_data.txt")));

    if (data["camera_position"] != nullptr && data["camera_front"] != nullptr)
    {
        glm::vec3 camera_position = glm::vec3(data["camera_position"]["x"], data["camera_position"]["y"], data["camera_position"]["z"]);
        glm::vec3 camera_front = glm::vec3(data["camera_front"]["x"], data["camera_front"]["y"], data["camera_front"]["z"]);

        camera.setPosition(camera_position);
        camera.lookAt(camera_position + camera_front);
    }




    ChunkManager chunk_manager;



    unsigned int texture = VoxelEngine::Util::load_texture(GET_TEXTURE("smooth_stone.png"));




    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    ImGuiIO* io = &ImGui::GetIO();
    io->AddMouseButtonEvent(GLFW_MOUSE_BUTTON_1, true);





    while (!window.shouldClose())
    {
        static bool gl_fill = (data["gl_fill"] == true);

        window.clear(20, 20, 20);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::vec3 camera_position = camera.getPosition() - glm::vec3(0.0f, 32.0f, 0.0f);
        const int chunk_x = glm::floor(camera_position.x / CHUNK_SIZE);
        const int chunk_y = glm::floor(camera_position.y / CHUNK_SIZE);
        const int chunk_z = glm::floor(camera_position.z / CHUNK_SIZE);
        glm::ivec3 chunk_position(chunk_x, chunk_y, chunk_z);

        for (int x = -3; x <= 3; x++)
        {
            for (int z = -3; z <= 3; z++)
            {
                chunk_manager.loadChunk(chunk_position + glm::ivec3(x, 0, z), mat);
            }
        }


        #pragma region INPUT_HANDLER
        if (!io->WantCaptureMouse && (glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ||
            glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS))
        {
            camera.capture();
        }
        else
        {
            camera.release();
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.getWindow(), true);
        }
        camera.updatePosition();
        #pragma endregion

        #pragma region DRAW_CUBES
        glPolygonMode(GL_FRONT_AND_BACK, gl_fill ? GL_FILL : GL_LINE);
        shader.use();
        shader.setMat4("u_View", camera.getViewMatrix());
        shader.setMat4("u_Projection", camera.getProjectionMatrix());
        shader.setVec3("u_ViewPos", camera.getPosition());

        shader.setVec3("u_Light.position", camera.getPosition() + glm::vec3(0.0f, 100.0f, 0.0f));
        shader.setVec3("u_Light.ambient", glm::vec3(0.5f));
        shader.setVec3("u_Light.diffuse", glm::vec3(1.0f));
        shader.setVec3("u_Light.specular", glm::vec3(1.0f));

        shader.setVec3("u_Material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        // shader.setVec3("u_Material.diffuse", glm::vec3(1.0f, 0.3f, 0.3f));
        shader.setVec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setFloat("u_Material.shininess", 64.0f);

        shader.setInt("u_Material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        cube_ibo.data(cube_vao, sizeof(glm::mat4) * mat.size(), mat.data());
        cube_vao.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mat.size());
        #pragma endregion

        #pragma region IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(FLT_MAX, FLT_MAX));

        if (ImGui::Begin("Voxel Engine"))
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;

            glm::vec3 camera_position = camera.getPosition();

            if (ImGui::BeginTable("Data", 3, flags))
            {
                double x_position, y_position;
                glfwGetCursorPos(window.getWindow(), &x_position, &y_position);

                ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Extra", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                ImGui::PushID(0);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Camera Position");
                ImGui::TableSetColumnIndex(1);
                float column_width = ImGui::GetContentRegionAvail().x;
                ImGui::PushItemWidth(column_width);
                ImGui::DragFloat3("##camera_pos", &camera.m_Position[0], 1.0f, 0.0f, 0.0f, "%.03f");
                ImGui::PopItemWidth();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec3(";
                    pos << std::to_string(camera_position.x) << ", ";
                    pos << std::to_string(camera_position.y) << ", ";
                    pos << std::to_string(camera_position.z) << ")";
                    glfwSetClipboardString(window.getWindow(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(1);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Camera Yaw/Pitch");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, ImGui::GetStyle().ItemSpacing.y));
                column_width = ImGui::GetContentRegionAvail().x;
                float item_spacing = ImGui::GetStyle().ItemSpacing.x;
                float item_width = (column_width - item_spacing) / 2;
                ImGui::PushItemWidth(item_width);
                if (ImGui::DragFloat("##camera_yaw", &camera.m_Yaw, 0.1f, 0.0f, 0.0f, "%.03f"))
                {
                    camera.contrainAngles();
                    camera.updateVectors();
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                ImGui::PushItemWidth(item_width);
                if (ImGui::DragFloat("##camera_pitch", &camera.m_Pitch, 0.1f, -89.99f, 89.99f, "%.03f"))
                {
                    camera.contrainAngles();
                    camera.updateVectors();
                }
                ImGui::PopItemWidth();
                ImGui::PopStyleVar();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec2(";
                    pos << std::to_string(camera.m_Yaw) << ", ";
                    pos << std::to_string(camera.m_Pitch) << ")";
                    glfwSetClipboardString(window.getWindow(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(2);
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
                    glfwSetClipboardString(window.getWindow(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(3);
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

                int x = glm::floor(camera_position.x / CHUNK_SIZE);
                int y = glm::floor(camera_position.y / CHUNK_SIZE);
                int z = glm::floor(camera_position.z / CHUNK_SIZE);
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
                ImGui::Text("Voxel Count");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%i", mat.size());
                ImGui::PopID();

                ImGui::PushID(2);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Triangle Count");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%i", mat.size() * 12);
                ImGui::PopID();
            }
            ImGui::EndTable();

            ImGui::Checkbox("GL_FILL", &gl_fill);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #pragma endregion

        #pragma region SAVE_DATA
        data["camera_position"]["x"] = camera.getPosition().x;
        data["camera_position"]["y"] = camera.getPosition().y;
        data["camera_position"]["z"] = camera.getPosition().z;
        data["camera_front"]["x"] = camera.getFront().x;
        data["camera_front"]["y"] = camera.getFront().y;
        data["camera_front"]["z"] = camera.getFront().z;
        data["gl_fill"] = gl_fill;
        VoxelEngine::Util::write_file(GET_DATA("world_data.txt"), data.dump(4));
        #pragma endregion

        grid_overlay.draw(camera);
        window.swap();
    }
    return 0;
}
