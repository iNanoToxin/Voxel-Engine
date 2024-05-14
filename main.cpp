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

    std::vector<glm::mat4> mat;

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));

    mat.push_back(model);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int ibo;
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



    VoxelEngine::Camera camera(&window);
    camera.lookAt(glm::vec3(0.0f, 0.0f, 5.0f));



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

        // mat[0] = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f)), (float) glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
        // glBindBuffer(GL_ARRAY_BUFFER, ibo);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mat.size(), mat.data(), GL_STATIC_DRAW);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mat.size());

        if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.getWindow(), true);
        }

        window.swap();
    }
    return 0;
}
