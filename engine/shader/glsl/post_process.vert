#version 410 core

layout(location = 0) out vec2 texcoord;
layout(location = 1) out int instance_id;

void main()
{
    const vec3 fullscreen_triangle_positions[3] =
        vec3[3](vec3(3.0, 1.0, 0.5), vec3(-1.0, 1.0, 0.5), vec3(-1.0, -3.0, 0.5));
    gl_Position = vec4(fullscreen_triangle_positions[gl_InstanceID], 1.0);

    const vec2 fullscreen_triangle_tex_coords[3] =
        vec2[3](vec2(2.0, 1.0), vec2(0.0, 1.0), vec2(0.0, -1.0));
    texcoord = fullscreen_triangle_tex_coords[gl_InstanceID];

    instance_id = gl_InstanceID;
}
