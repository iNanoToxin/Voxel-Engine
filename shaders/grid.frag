#version 430 core

out vec4 FragColor;

in vec3 nearPoint; // nearPoint calculated in vertex shader
in vec3 farPoint; // farPoint calculated in vertex shader


void main()
{
    float t = -nearPoint.z / (farPoint.z - nearPoint.z);
    FragColor = vec4(1.0, 0.0, 0.0, 1.0 * float(t > 0));
}
