#version 430 core

layout (location = 0) in vec4 a_Pos;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 o_NearPoint;
out vec3 o_FarPoint;

vec3 unproject_point(float x, float y, float z)
{
    mat4 view_inverse = inverse(u_View);
    mat4 proj_inverse = inverse(u_Projection);
    vec4 unprojected_point = view_inverse * proj_inverse * vec4(x, y, z, 1.0);
    return unprojected_point.xyz / unprojected_point.w;
}

void main()
{
    o_NearPoint = unproject_point(a_Pos.x, a_Pos.y, 0.0);
    o_FarPoint = unproject_point(a_Pos.x, a_Pos.y, 1.0);
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);
}
