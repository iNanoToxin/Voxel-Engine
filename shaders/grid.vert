#version 430 core

layout (location = 0) in vec4 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 nearPoint;
out vec3 farPoint;


vec3 UnprojectPoint(float x, float y, float z) {
    vec4 clipCoords = vec4(x, y, z, 1.0);

    vec4 viewCoords = inverse(projection) * clipCoords;
    viewCoords = viewCoords.xyzw / viewCoords.w;

    vec4 worldCoords = inverse(view) * viewCoords;

    return  worldCoords.xyz / worldCoords.w;
}


void main()
{
    nearPoint = UnprojectPoint(aPos.x, aPos.z, 1.0);
    farPoint = UnprojectPoint(aPos.x, aPos.z, -1.0);
    gl_Position = vec4(aPos.x, aPos.z, 0.0, 1.0);
//    gl_Position = projection * view * vec4(aPos, 1.0);
}
