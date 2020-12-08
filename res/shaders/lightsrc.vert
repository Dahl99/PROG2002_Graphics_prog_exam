#version 430 core

/** Inputs */
layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoords;

/** Outputs */
out vec2 v_TexCoords;
out vec4 v_Color;

/** Uniforms */
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
	v_TexCoords = a_TexCoords;
}