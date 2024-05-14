#version 430 core
layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in mat4 a_Model;

out vec3 o_Normal;
out vec3 o_FragPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    gl_Position = u_Projection * u_View * a_Model * a_Pos;
    o_Normal = mat3(transpose(inverse(a_Model))) * a_Normal;
    o_FragPos = vec3(a_Model * a_Pos);
}
