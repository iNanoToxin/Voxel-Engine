#version 430 core

layout (location = 0) in vec4 a_Pos;
layout (std140, binding = 0) uniform Properties
{
    mat4 view;
    mat4 viewInverse;
    mat4 projection;
    vec3 position;
    float near;
    float far;
} properties;

out vec3 local_pos;
out vec3 plane_axes;
out vec3 grid_size;

void main()
{
    local_pos = vec3(a_Pos.x, a_Pos.y, 0.0);
    plane_axes = vec3(1, 1, 0);
    grid_size = vec3(5, 5, 5);

    vec3 real_pos = properties.position * plane_axes + local_pos * grid_size;

    gl_Position = properties.projection * properties.view * vec4(real_pos, 1.0);
}
