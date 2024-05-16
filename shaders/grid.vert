#version 430 core

layout (location = 0) in vec4 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 nearPoint;
out vec3 farPoint;


vec3 UnprojectPoint(float x, float y, float z)
{
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0);
    farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
//    gl_Position = projection * view * vec4(aPos, 1.0);
}
