#include <iostream>
#include <vector>
#include "window.h"
#include "rendering/shader.h"
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "rendering/vao.h";
#include "rendering/buffer_object.h";


int main()
{
    VoxelEngine::Window window(2560 / 2, 1440 / 2, "Voxel Engine", true);

    const char* vertex_path = R"(C:\Users\dylan\JetBrains\CLionProjects\Voxel-Engine\shaders\shader.vert)";
    const char* fragment_path = R"(C:\Users\dylan\JetBrains\CLionProjects\Voxel-Engine\shaders\shader.frag)";
    VoxelEngine::Shader shader(vertex_path, fragment_path);

    const char* grid_vertex_path = R"(C:\Users\dylan\JetBrains\CLionProjects\Voxel-Engine\shaders\grid.vert)";
    const char* grid_fragment_path = R"(C:\Users\dylan\JetBrains\CLionProjects\Voxel-Engine\shaders\grid.frag)";
    VoxelEngine::Shader grid_shader(grid_vertex_path, grid_fragment_path);

    // const char* grid_vertex_path2 = R"(C:\Users\dylan\JetBrains\CLionProjects\Voxel-Engine\shaders\grid_overlay.vert)";
    // const char* grid_fragment_path2 = R"(C:\Users\dylan\JetBrains\CLionProjects\Voxel-Engine\shaders\grid_overlay.frag)";
    // VoxelEngine::Shader grid_shader2(grid_vertex_path2, grid_fragment_path2);

    std::vector<glm::mat4> mat;
    VoxelEngine::Vao cube_vao;
    VoxelEngine::Vbo cube_vbo, cube_ibo;
    {
        float vertices[216] = {
            // back face (CCW winding)
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // top-right
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // top-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,   // top-left
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // bottom-left
            // front face (CCW winding)
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // bottom-left
            // left face (CCW)
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,   // top-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,   // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // bottom-left
            // right face (CCW)
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,    // bottom-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,   // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,    // top-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,    // top-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,     // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,    // bottom-left
            // bottom face (CCW)
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  // bottom-right
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,   // top-right
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,   // top-right
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,  // top-left
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, // bottom-left
            // top face (CCW)
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // bottom-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // top-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // top-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // bottom-left
        };

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        mat.push_back(model);


        cube_vbo.data(cube_vao, sizeof(vertices), vertices);
        cube_vao.link(cube_vbo, 6 * sizeof(float));
        cube_vao.addAttribute(3, GL_FLOAT);
        cube_vao.addAttribute(3, GL_FLOAT);


        cube_ibo.data(cube_vao, sizeof(glm::mat4) * mat.size(), mat.data());
        cube_vao.link(cube_ibo, sizeof(glm::mat4));
        cube_vao.addAttribute(4, GL_FLOAT);
        cube_vao.addAttribute(4, GL_FLOAT);
        cube_vao.addAttribute(4, GL_FLOAT);
        cube_vao.addAttribute(4, GL_FLOAT);

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
    }





    VoxelEngine::Camera camera(&window);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    VoxelEngine::Vao grid_vao;
    VoxelEngine::Vbo grid_vbo;
    VoxelEngine::Ebo grid_ebo;

    grid_vbo.data(grid_vao, sizeof(vertices), vertices);
    grid_ebo.data(grid_vao, sizeof(indices), indices);

    grid_vao.link(grid_vbo, 3 * sizeof(float));
    grid_vao.addAttribute(3, GL_FLOAT);





    u32 block_index = glGetUniformBlockIndex(grid_shader.m_Id, "Properties");

    float near = camera.getNear();
    float far = camera.getFar();

    u32 ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4) + sizeof(glm::vec3) + 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.getViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::inverse(camera.getViewMatrix())));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.getProjectionMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.getPosition()));
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4) + sizeof(glm::vec3), sizeof(float), &near);
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(float), sizeof(float), &far);
    glUniformBlockBinding(grid_shader.m_Id, block_index, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    ImGuiIO* io = &ImGui::GetIO();
    io->AddMouseButtonEvent(GLFW_MOUSE_BUTTON_1, true);

    while (!window.shouldClose())
    {
        window.clear(20, 20, 20);
        glClear(GL_DEPTH_BUFFER_BIT);

        if (!io->WantCaptureMouse && (glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS || glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS))
        {
            camera.capture();
        }
        else
        {
            camera.release();
        }
        camera.updatePosition();

        {
            shader.use();
            shader.setMat4("u_View", camera.getViewMatrix());
            shader.setMat4("u_Projection", camera.getProjectionMatrix());
            shader.setVec3("u_ViewPos", camera.getPosition());

            shader.setVec3("u_Light.position", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3("u_Light.ambient", glm::vec3(0.5f));
            shader.setVec3("u_Light.diffuse", glm::vec3(0.5f));
            shader.setVec3("u_Light.specular", glm::vec3(1.0f));

            shader.setVec3("u_Material.ambient", glm::vec3(1.0f, 0.3f, 0.3f));
            shader.setVec3("u_Material.diffuse", glm::vec3(1.0f, 0.3f, 0.3f));
            shader.setVec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            shader.setFloat("u_Material.shininess", 64.0f);

            // mat[0] = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)), (float) glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
            // glBindBuffer(GL_ARRAY_BUFFER, ibo);
            // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mat.size(), mat.data(), GL_STATIC_DRAW);

            // glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            cube_vao.bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mat.size());
        }


        grid_shader.use();

        GLbyte* ptr = (GLbyte*) glMapBufferRange(GL_UNIFORM_BUFFER, 0, 3 * sizeof(glm::mat4) + sizeof(glm::vec3), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

        if (ptr)
        {
            memcpy(ptr, glm::value_ptr(camera.getViewMatrix()), sizeof(glm::mat4));
            memcpy(ptr + sizeof(glm::mat4), glm::value_ptr(glm::inverse(camera.getViewMatrix())), sizeof(glm::mat4));
            memcpy(ptr + 2 * sizeof(glm::mat4), glm::value_ptr(camera.getProjectionMatrix()), sizeof(glm::mat4));
            memcpy(ptr + 3 * sizeof(glm::mat4), glm::value_ptr(camera.getPosition()), sizeof(glm::vec3));
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }


        // glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.getViewMatrix()));
        // glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::inverse(camera.getViewMatrix())));
        // glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.getProjectionMatrix()));
        // glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.getPosition()));
        // glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4) + sizeof(glm::vec3), sizeof(float), &near);
        // glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(float), sizeof(float), &far);
        // glUniformBlockBinding(grid_shader.m_Id, block_index, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);


        grid_vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


        if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.getWindow(), true);
        }


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Voxel Enginess"))
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("Data", 3, flags))
            {
                glm::vec3 camera_position = camera.getPosition();
                glm::vec3 camera_front = camera_position + camera.getFront() * 5.0f;

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
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("%.2f, %.2f, %.2f", camera_position.x, camera_position.y, camera_position.z);
                ImGui::PopStyleColor();
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
                ImGui::Text("Camera Front (5)");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("%.2f, %.2f, %.2f", camera_front.x, camera_front.y, camera_front.z);
                ImGui::PopStyleColor();
                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Copy"))
                {
                    std::stringstream pos;
                    pos << "glm::vec3(";
                    pos << std::to_string(camera_front.x) << ", ";
                    pos << std::to_string(camera_front.y) << ", ";
                    pos << std::to_string(camera_front.z) << ")";
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
                    pos << std::to_string(x_position) << ", ";
                    pos << std::to_string(y_position);
                    glfwSetClipboardString(window.getWindow(), pos.str().c_str());
                }
                ImGui::PopID();

                ImGui::PushID(3);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Application average");
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
                ImGui::Text("%.3f ms/frame", 1000.0f / io->Framerate);
                ImGui::PopStyleColor();
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
                ImGui::Text("%.1f FPS", io->Framerate);
                ImGui::PopStyleColor();
                ImGui::PopID();

                ImGui::EndTable();

                ImGui::SliderFloat("Camera Position X", &camera.m_Position.x, -10000.0f, 10000.0f, "%.03f");
                ImGui::SliderFloat("Camera Position Y", &camera.m_Position.y, -10000.0f, 10000.0f, "%.03f");
                ImGui::SliderFloat("Camera Position Z", &camera.m_Position.z, -10000.0f, 10000.0f, "%.03f");
            }
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swap();
    }
    return 0;
}
