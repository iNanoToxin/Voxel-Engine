#include <iostream>
#include <vector>
#include "window.h"
#include "rendering/shader.h"
#include "camera.h"

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
    u32 vao, vbo, ibo;
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
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));

        mat.push_back(model);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ARRAY_BUFFER, ibo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mat.size(), mat.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), nullptr);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
    }





    VoxelEngine::Camera camera(&window);
    camera.lookAt(glm::vec3(0.0f, 0.0f, -5.0f));
    camera.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    camera.forward(-4.0f);

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

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




    while (!window.shouldClose())
    {
        window.clear(20, 20, 20);
        glClear(GL_DEPTH_BUFFER_BIT);

        if (glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
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

            glBindVertexArray(vao);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mat.size());
        }


        grid_shader.use();
        grid_shader.setFloat("u_Near", camera.getNear());
        grid_shader.setFloat("u_Far", camera.getFar());
        grid_shader.setMat4("u_Projection", camera.getProjectionMatrix());
        grid_shader.setMat4("u_View", camera.getViewMatrix());
        grid_shader.setVec3("u_CameraPos", camera.getPosition());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


        if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.getWindow(), true);
        }

        window.swap();
    }
    return 0;
}
