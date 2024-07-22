#version 430 core


out vec4 o_FragColor;


in vec3 plane_axes;
in vec3 local_pos;
in vec3 grid_size;

uniform float u_StartTime;
uniform float u_Time;

layout (std140, binding = 0) uniform Properties
{
    mat4 view;
    mat4 viewInverse;
    mat4 projection;
    vec3 position;
    float near;
    float far;
} properties;

#define M_1_SQRTPI 0.5641895835477563 /* `1/sqrt(pi)`. */
#define DISC_RADIUS (M_1_SQRTPI * 1.05)
#define GRID_LINE_SMOOTH_START (0.5 + DISC_RADIUS)
#define GRID_LINE_SMOOTH_END (0.5 - DISC_RADIUS)
#define GRID_LINE_STEP(dist) smoothstep(GRID_LINE_SMOOTH_START, GRID_LINE_SMOOTH_END, dist)

#define linearstep(p0, p1, v) (clamp(((v) - (p0)) / abs((p1) - (p0)), 0.0, 1.0))

//float steps[] = { 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f, 10000.0f };
float steps[] = { 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f };
#define STEPS_LEN 6

#define LINE_SIZE 0.01
bool flag_test(uint flag, uint val) { return (flag & val) != 0u; }
bool flag_test(int flag, uint val) { return flag_test(uint(flag), val); }
bool flag_test(int flag, int val) { return (flag & val) != 0; }

#define PLANE_XY (1 << 0)
#define PLANE_XZ (1 << 1)
#define PLANE_YZ (1 << 2)

#define SHOW_AXIS_X (1 << 3)
#define SHOW_AXIS_Y (1 << 4)
#define SHOW_AXIS_Z (1 << 5)


#define grid_flag (PLANE_XZ | SHOW_AXIS_X | SHOW_AXIS_Z)



float get_grid(vec2 co, vec2 fwidthCos, vec2 grid_scale)
{
    vec2 half_size = grid_scale / 2.0;
    // Triangular wave pattern, amplitude is [0, half_size].
    vec2 grid_domain = abs(mod(co + half_size, grid_scale) - half_size);
    // Modulate by the absolute rate of change of the coordinates
    // (make line have the same width under perspective).
    grid_domain /= fwidthCos;
    // Collapse waves.
    float line_dist = min(grid_domain.x, grid_domain.y);
    return GRID_LINE_STEP(line_dist - LINE_SIZE);
}

vec3 get_axes(vec3 co, vec3 fwidthCos, float line_size)
{
    vec3 axes_domain = abs(co);
    // Modulate by the absolute rate of change of the coordinates
    // (make line have the same width under perspective).
    axes_domain /= fwidthCos;
    return GRID_LINE_STEP(axes_domain - (line_size + LINE_SIZE));
}

void main()
{
    vec3 frag_pos = local_pos * grid_size;
    vec3 dFdxPos = dFdx(frag_pos);
    vec3 dFdyPos = dFdy(frag_pos);
    vec3 fwidthPos = abs(dFdxPos) + abs(dFdyPos);
    frag_pos += properties.position * plane_axes;

    mat4 viewInv = properties.viewInverse;

    vec2 grid_pos;
    vec2 grid_fwidth;
    if (flag_test(grid_flag, PLANE_XZ))
    {
        grid_pos = frag_pos.xz;
        grid_fwidth = fwidthPos.xz;
    }
    else if (flag_test(grid_flag, PLANE_YZ))
    {
        grid_pos = frag_pos.yz;
        grid_fwidth = fwidthPos.yz;
    }
    else
    {
        grid_pos = frag_pos.xy;
        grid_fwidth = fwidthPos.xy;
    }


    float grid_res = dot(dFdxPos, viewInv[0].xyz);
    grid_res *= 4;

    int step_idx = STEPS_LEN - 1;
    for (int i = STEPS_LEN - 2; i >= 0; i--)
    {
        if (grid_res < steps[i])
        {
            step_idx = i;
        }
    }

    float scale0 = step_idx > 0 ? steps[step_idx - 1] : 0.0;
    float scaleA = steps[step_idx];
    float scaleB = steps[min(step_idx + 1, STEPS_LEN - 1)];
    float scaleC = steps[min(step_idx + 2, STEPS_LEN - 1)];

    float blend = 1.0 - smoothstep(scale0, scaleA, grid_res + grid_res);
    blend = blend * blend * blend;


    float gridA = get_grid(grid_pos, grid_fwidth, vec2(scaleA, scaleA));
    float gridB = get_grid(grid_pos, grid_fwidth, vec2(scaleB, scaleB));
    float gridC = get_grid(grid_pos, grid_fwidth, vec2(scaleC, scaleC));

    vec4 color_grid = vec4(0.2, 0.2, 0.2, 1.0);
    vec4 color_grid_emphasis = vec4(1.0, 1.0, 0.0, 1.0);

    vec4 grid_color = color_grid;
    grid_color.a = gridA * blend;
    grid_color = mix(grid_color, mix(color_grid, color_grid_emphasis, blend), gridB);
    grid_color = mix(grid_color, color_grid_emphasis, gridC);

    if (flag_test(grid_flag, SHOW_AXIS_X | SHOW_AXIS_Y | SHOW_AXIS_Z))
    {
        vec3 axes_dist, axes_fwidth;

        if (flag_test(grid_flag, SHOW_AXIS_X))
        {
            axes_dist.x = dot(frag_pos.yz, plane_axes.yz);
            axes_fwidth.x = dot(fwidthPos.yz, plane_axes.yz);
        }

        if (flag_test(grid_flag, SHOW_AXIS_Y))
        {
            axes_dist.y = dot(frag_pos.xz, plane_axes.xz);
            axes_fwidth.y = dot(fwidthPos.xz, plane_axes.xz);
        }

        if (flag_test(grid_flag, SHOW_AXIS_Z))
        {
            axes_dist.z = dot(frag_pos.xy, plane_axes.xy);
            axes_fwidth.z = dot(fwidthPos.xy, plane_axes.xy);
        }

        vec3 axes = get_axes(axes_dist, axes_fwidth, 0.1);

        if (flag_test(grid_flag, SHOW_AXIS_X))
        {
            grid_color.a = max(grid_color.a, axes.x);
            grid_color.rgb = (axes.x < 1e-8) ? grid_color.rgb : vec3(1, 0, 0);
        }

        if (flag_test(grid_flag, SHOW_AXIS_Y))
        {
            grid_color.a = max(grid_color.a, axes.y);
            grid_color.rgb = (axes.y < 1e-8) ? grid_color.rgb : vec3(0, 1, 0);
        }

        if (flag_test(grid_flag, SHOW_AXIS_Z))
        {
            grid_color.a = max(grid_color.a, axes.z);
            grid_color.rgb = (axes.z < 1e-8) ? grid_color.rgb : vec3(0, 0, 1);
        }
    }

    float view_distance = 100.0f;
    bool is_persp = true;

    float dist, fade;
    if (is_persp)
    {
        vec3 V = properties.position - frag_pos;
        dist = length(V);
        V /= dist;

        float angle;
        if (flag_test(grid_flag, PLANE_XZ))
        {
            angle = V.y;
        }
        else if (flag_test(grid_flag, PLANE_YZ))
        {
            angle = V.x;
        }
        else
        {
            angle = V.z;
        }

        angle = 1.0 - abs(angle);
        angle *= angle;
        fade = 1.0 - angle * angle;
        fade *= 1.0 - smoothstep(0.0, view_distance, dist - view_distance);
    }
    else
    {
        dist = gl_FragCoord.z * 2.0 - 1.0;
        dist = clamp(dist, 0.0, 1.0);
        fade = 1.0 - smoothstep(0.0, 0.5, dist - 0.5);
        dist = 1.0;

//        if (flag_test(grid_flag, PLANE_XY)) {
//            float angle = 1.0 - abs(drw_view.viewinv[2].z);
//            dist = 1.0 + angle * 2.0;
//            angle *= angle;
//            fade *= 1.0 - angle * angle;
//        }
    }


    o_FragColor = grid_color;
    o_FragColor.a *= fade;
}
