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

#define LINE_SIZE 1
#define M_1_SQRTPI 0.5641895835477563 /* `1/sqrt(pi)`. */
#define DISC_RADIUS (M_1_SQRTPI * 1.05)
#define GRID_LINE_SMOOTH_START (0.5 + DISC_RADIUS)
#define GRID_LINE_SMOOTH_END (0.5 - DISC_RADIUS)
#define GRID_LINE_STEP(dist) smoothstep(GRID_LINE_SMOOTH_START, GRID_LINE_SMOOTH_END, dist)

vec3 plane_axes = vec3(1, 0, 1);

float get_grid(vec2 co, vec2 fwidthCos, vec2 grid_scale)
{
    vec2 half_size = grid_scale / 2.0;
    vec2 grid_domain = abs(mod(co + half_size, grid_scale) - half_size);
    grid_domain /= fwidthCos;
    float line_dist = min(grid_domain.x, grid_domain.y);
    return GRID_LINE_STEP(line_dist - LINE_SIZE);
}

vec3 get_axes(vec3 co, vec3 fwidthCos, float line_size)
{
    vec3 axes_domain = abs(co);
    axes_domain /= fwidthCos;
    return GRID_LINE_STEP(axes_domain - (line_size + LINE_SIZE));
}

vec4 grid(vec3 frag_pos, float scale)
{
    vec2 coord = frag_pos.xz;
    vec2 derivative = fwidth(coord);

    float gridA = get_grid(coord, derivative, vec2(scale, scale));
    vec4 color = vec4(0.2, 0.2, 0.2, 0.0);
    color.a = gridA;



    vec3 axes_dist, axes_fwidth;

    axes_dist.x = dot(coord.yx, plane_axes.xy);
    axes_fwidth.x = dot(derivative.yx, plane_axes.xy);

    axes_dist.z = dot(coord.xy, plane_axes.xy);
    axes_fwidth.z = dot(derivative.xy, plane_axes.xy);

    vec3 axes = get_axes(axes_dist, axes_fwidth, 0.1);

    color.a = max(color.a, axes.x);
    color.rgb = (axes.x < 1e-8) ? color.rgb : vec3(1, 0, 0);

    color.a = max(color.a, axes.z);
    color.rgb = (axes.z < 1e-8) ? color.rgb : vec3(0, 0, 1);

    return color;
}

float compute_depth(vec3 pos)
{
    vec4 clip_space_pos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float compute_linear_depth(vec3 pos)
{
    vec4 clip_space_pos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    float linear_depth = (2.0 * u_Near * u_Far) / (u_Far + u_Near - clip_space_depth * (u_Far - u_Near));
    return linear_depth / u_Far;
}

vec4 max4(vec4 a, vec4 b)
{
    return vec4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
}

void main()
{
    float t = -o_NearPoint.y / (o_FarPoint.y - o_NearPoint.y);
    vec3 frag_pos = o_NearPoint + t * (o_FarPoint - o_NearPoint);

    float linear_depth = compute_linear_depth(frag_pos);
    float fading = max(0, (0.5 - linear_depth));

    gl_FragDepth = ((gl_DepthRange.diff * compute_depth(frag_pos)) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;


    int start = 0;
    float prev;
    float curr;
    float next;
    float ypos = u_CameraPos.y;

    do
    {
        prev = pow(10, start - 1);
        curr = pow(10, start);
        next = pow(10, start + 1);
        start++;
    }
    while (u_CameraPos.y >= pow(10, start));

    vec4 grid_1 = grid(frag_pos, prev);
    vec4 grid_2 = grid(frag_pos, curr);
    vec4 grid_3 = grid(frag_pos, next);


    if (grid_1.w > 0) grid_1.w = 1 - smoothstep(curr, next, ypos);
    if (grid_2.w > 0) grid_2.w = smoothstep(curr, next, ypos);

    vec4 grid_color = max4(grid_1, grid_2);
//    grid_color = grid(frag_pos, 10);
//    vec4 grid_color = grid(frag_pos, 10) + u_CameraPos.y * 0.000000000000000001;



    o_FragColor = grid_color;
//    o_FragColor.a *= fading;
    o_FragColor.a *= (1.0 + fading * 0.000000000000001);
}
