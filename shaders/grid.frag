#version 430 core

out vec4 o_FragColor;
in vec3 o_NearPoint;
in vec3 o_FarPoint;

uniform float u_Near;
uniform float u_Far;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_CameraPos;

float steps[] = {0.001f, 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f, 10000.0f};

vec4 grid(vec3 frag_pos, float scale) {
    vec2 coord = frag_pos.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;

    float line = min(grid.x, grid.y);
    float minimum_z = min(derivative.y, 1);
    float minimum_x = min(derivative.x, 1);

    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    // z axis
    if(frag_pos.x > -0.1 * minimum_x && frag_pos.x < 0.1 * minimum_x)
    {
        color.z = 1.0;
    }
    // x axis
    if(frag_pos.z > -0.1 * minimum_z && frag_pos.z < 0.1 * minimum_z)
    {
        color.x = 1.0;
    }
    return color;
}

float compute_depth(vec3 pos) {
    vec4 clip_space_pos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float compute_linear_depth(vec3 pos) {
    vec4 clip_space_pos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    float linear_depth = (2.0 * u_Near * u_Far) / (u_Far + u_Near - clip_space_depth * (u_Far - u_Near));
    return linear_depth / u_Far;
}

vec4 max4(vec4 a, vec4 b) {
    return vec4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
}

void main()
{
    float t = -o_NearPoint.y / (o_FarPoint.y - o_NearPoint.y);
    vec3 frag_pos = o_NearPoint + t * (o_FarPoint - o_NearPoint);

    float linear_depth = compute_linear_depth(frag_pos);
    float fading = max(0, (0.5 - linear_depth));

    gl_FragDepth = ((gl_DepthRange.diff * compute_depth(frag_pos)) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;



    float prev = 0.10f;
    float curr = 1.0f;
    float next = 10.0f;
    float ypos = u_CameraPos.y;

    for (int i = 0; u_CameraPos.y >= pow(10, i); i++)
    {
        prev = pow(10, i - 1);
        curr = pow(10, i);
        next = pow(10, i + 1);
    }

    vec4 grid_1 = grid(frag_pos, 1 / prev);
    vec4 grid_2 = grid(frag_pos, 1 / curr);
    vec4 grid_3 = grid(frag_pos, 1 / next);


    if (grid_1.w > 0) grid_1.w = 1 - smoothstep(curr, next, ypos);
    if (grid_2.w > 0) grid_2.w = smoothstep(curr, next, ypos);

    vec4 grid_color = max4(grid_1, grid_2);
//    vec4 grid_color = grid(frag_pos, 10) + u_CameraPos.y * 0.000000000000000001;



    o_FragColor = grid_color;
    o_FragColor.a *= (1 + fading * 0.000000000000001);
}
