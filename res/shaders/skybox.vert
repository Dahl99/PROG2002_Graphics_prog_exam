#version 430 core

/** Inputs */
layout(location = 0) in vec3 a_Pos;

/** Outputs */
out vec3 v_TexCoords;

/** Uniforms */
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_TexCoords = a_Pos;
    vec4 pos = u_Projection * u_View * vec4(a_Pos, 1.0);
    gl_Position = pos.xyww;
} 