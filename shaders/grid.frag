#version 430 core

out vec4 o_FragColor;

uniform float u_Near;
uniform float u_Far;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_CameraPos;

in vec3 plane_axes;
in vec3 local_pos;
in vec3 grid_size;

float steps[] = {0.001f, 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f, 10000.0f};

#define LINE_SIZE 0.01
#define M_1_SQRTPI 0.5641895835477563 /* `1/sqrt(pi)`. */
#define DISC_RADIUS (M_1_SQRTPI * 1.05)
#define GRID_LINE_SMOOTH_START (0.5 + DISC_RADIUS)
#define GRID_LINE_SMOOTH_END (0.5 - DISC_RADIUS)
#define GRID_LINE_STEP(dist) smoothstep(GRID_LINE_SMOOTH_START, GRID_LINE_SMOOTH_END, dist)

#define linearstep(p0, p1, v) (clamp(((v) - (p0)) / abs((p1) - (p0)), 0.0, 1.0))



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

void main()
{
    vec3 frag_pos = local_pos * grid_size;
    vec3 dFdxPos = dFdx(frag_pos);
    vec3 dFdyPos = dFdy(frag_pos);
    vec3 fwidthPos = abs(dFdxPos) + abs(dFdyPos);
    frag_pos += u_CameraPos * plane_axes;

    mat4 viewInv = inverse(u_View);

    float linear_depth = compute_linear_depth(frag_pos);
    float fading = max(0, (0.5 - linear_depth));

    vec2 grid_pos = frag_pos.xy;
    vec2 grid_fwidth = fwidthPos.xy;


//    gl_FragDepth = ((gl_DepthRange.diff * compute_depth(frag_pos)) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;


    float grid_res = dot(dFdxPos, viewInv[0].xyz);
    grid_res *= 4;

    int step = 7;
    for (int i = step; i >= 0; i--)
    {
        if (grid_res < steps[i])
        {
            step = i;
        }
    }

    float scale_a = step > 0 ? steps[step - 1] : 0.0;
    float scale_b = steps[step];
    float scale_c = steps[min(step + 1, 7)];
    float scale_d = steps[min(step + 2, 7)];

    float blend = 1.0 - linearstep(scale_a, scale_b, grid_res + grid_res);
//    blend = blend * blend * blend;


    float gridA = get_grid(grid_pos, grid_fwidth, vec2(scale_b, scale_b));
    float gridB = get_grid(grid_pos, grid_fwidth, vec2(scale_c, scale_c));
    float gridC = get_grid(grid_pos, grid_fwidth, vec2(scale_d, scale_d));

    vec4 color_grid = vec4(0.2, 0.2, 0.2, 1.0);
    vec4 color_grid_emphasis = vec4(1.0, 1.0, 0.0, 1.0);

    vec4 grid_color = color_grid;
    grid_color.a = gridA * blend;
    grid_color = mix(grid_color, mix(color_grid, color_grid_emphasis, blend), gridB);
    grid_color = mix(grid_color, color_grid_emphasis, gridC);

    vec3 axes_dist, axes_fwidth;

    axes_dist.x = dot(frag_pos.yz, plane_axes.yz);
    axes_fwidth.x = dot(fwidthPos.yz, plane_axes.yz);

    axes_dist.y = dot(frag_pos.xz, plane_axes.xz);
    axes_fwidth.y = dot(fwidthPos.xz, plane_axes.xz);

//    axes_dist.z = dot(frag_pos.xy, plane_axes.xy);
//    axes_fwidth.z = dot(fwidthPos.xy, plane_axes.xy);

    vec3 axes = get_axes(axes_dist, axes_fwidth, 0.1);

    grid_color.a = max(grid_color.a, axes.x);
    grid_color.rgb = (axes.x < 1e-8) ? grid_color.rgb : vec3(1, 0, 0);

    grid_color.a = max(grid_color.a, axes.y);
    grid_color.rgb = (axes.y < 1e-8) ? grid_color.rgb : vec3(0, 1, 0);

//    grid_color.a = max(grid_color.a, axes.z);
//    grid_color.rgb = (axes.z < 1e-8) ? grid_color.rgb : vec3(0, 0, 1);


    float dist, fade;
    dist = gl_FragCoord.z * 2.0 - 1.0;
    dist = clamp(dist, 0.0, 1.0);
    fade = 1.0 - smoothstep(0.0, 0.5, dist - 0.5);
    dist = 1.0;

    vec3 V = u_CameraPos - frag_pos;
    dist = length(V);
    V /= dist;

    float angle;
//    if (flag_test(grid_flag, PLANE_XZ)) {
//        angle = V.y;
//    }
//    else if (flag_test(grid_flag, PLANE_YZ)) {
//        angle = V.x;
//    }
//    else {
//        angle = V.z;
//    }
    angle = V.z;

    angle = 1.0 - abs(angle);
    angle *= angle;
    fade = 1.0 - angle * angle;
    fade *= 1.0 - smoothstep(0.0, 1.0, dist - 1.0);

//    if (flag_test(grid_flag, PLANE_XY)) {
//        float angle = 1.0 - abs(drw_view.viewinv[2].z);
//        dist = 1.0 + angle * 2.0;
//        angle *= angle;
//        fade *= 1.0 - angle * angle;
//    }


    o_FragColor = grid_color;
//    o_FragColor.a *= fade;
//    o_FragColor.a *= fading;
}
