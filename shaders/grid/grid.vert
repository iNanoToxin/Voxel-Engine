#version 430 core

layout (location = 0) in vec2 a_Pos;
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

bool flag_test(uint flag, uint val) { return (flag & val) != 0u; }
bool flag_test(int flag, uint val) { return flag_test(uint(flag), val); }
bool flag_test(int flag, int val) { return (flag & val) != 0; }

#define PLANE_XY (1 << 0)
#define PLANE_XZ (1 << 1)
#define PLANE_YZ (1 << 2)

#define grid_flag PLANE_XZ

void main()
{
    if (flag_test(grid_flag, PLANE_XY))
    {
        local_pos = vec3(a_Pos.x, a_Pos.y, 0.0);
        plane_axes = vec3(1.0, 1.0, 0.0);
    }
    else if (flag_test(grid_flag, PLANE_XZ))
    {
        local_pos = vec3(a_Pos.x, 0.0, a_Pos.y);
        plane_axes = vec3(1.0, 0.0, 1.0);
    }
    else if (flag_test(grid_flag, PLANE_YZ))
    {
        local_pos = vec3(0.0, a_Pos.x, a_Pos.y);
        plane_axes = vec3(0.0, 1.0, 1.0);
    }
    else /* PLANE_IMAGE */
    {
        local_pos = vec3(a_Pos.xy * 0.5 + 0.5, 0.0);
        plane_axes = vec3(1.0, 1.0, 1.0);
    }

    grid_size = vec3(1, 1, 1) * 10000;

    vec3 real_pos = properties.position * plane_axes + local_pos * grid_size;


    real_pos.z = clamp(real_pos.z, -1e30, 1e30);
    local_pos.z = clamp(local_pos.z, -1.0, 1.0);

    gl_Position = properties.projection * (properties.view * vec4(real_pos, 1.0));
}
