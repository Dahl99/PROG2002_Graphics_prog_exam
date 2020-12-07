#version 430 core

/** Inputs */
in vec2 v_TexCoords;
in vec4 v_Color;

/** Outputs */
out vec4 FragColor;

/** Uniforms */
layout(binding = 0) uniform sampler2D u_Texture;


void main()
{
	vec4 color;
	if (v_Color.x == 0.0)
		color = texture(u_Texture, v_TexCoords);
	else
		color = v_Color;

	FragColor = color;
}