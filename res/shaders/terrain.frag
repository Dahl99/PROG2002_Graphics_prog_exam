#version 430 core

/** Structs */
struct PointLight {
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 2

/** Inputs */
in vec3 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;

/** Outputs */
out vec4 FragColor;

/** Uniforms */
uniform vec3 u_ViewPos;
uniform PointLight u_PointLights[NR_POINT_LIGHTS];

/** Functions */
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos);

void main()
{
	/** Calculating lighting from all point lights */
	vec3 blinn_phong;
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		blinn_phong += CalcPointLight(u_PointLights[i], normalize(v_Normal), v_FragPos, u_ViewPos);

	vec3 result = (blinn_phong.x + blinn_phong.y + blinn_phong.z) * v_Color;
	FragColor = vec4(result, 1.0);
//	FragColor = v_Color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
	/** Ambient Lighting */
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.color;

	/** Diffuse Lighting */
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.color;

	/** Blinn-Phong Specular Lighting */
	int shininess = 48;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = spec * light.color;

	/** Attenuation */
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

//    return (ambient + diffuse + specular);
    return (ambient + diffuse + specular);

} 