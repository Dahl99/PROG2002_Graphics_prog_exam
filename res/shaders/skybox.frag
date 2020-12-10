#version 430 core

/** Inputs */
in vec3 v_TexCoords;

/** Outputs */
out vec4 FragColor;

/** Uniforms */
layout(binding = 0) uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, v_TexCoords);
}