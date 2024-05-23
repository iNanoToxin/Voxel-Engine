#version 430 core

layout (location = 0) in vec4 a_Pos;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_CameraPos;

out vec3 local_pos;
out vec3 plane_axes;
out vec3 grid_size;

void main()
{
    local_pos = vec3(a_Pos.x, a_Pos.y, 0.0);
    plane_axes = vec3(1, 1, 0);
    grid_size = vec3(5, 5, 5);

    vec3 real_pos = u_CameraPos * plane_axes + local_pos * grid_size;

    gl_Position = u_Projection * u_View * vec4(real_pos, 1.0);
}
