#version 330 core
out vec4 color;

uniform int texture_mode;

uniform struct PointLight {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
} light;

uniform	sampler2D texture1;
uniform	sampler2D texture2;

uniform struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 emission;
	float shiness;
} material;

in Vertex{
vec2 texcoord;
vec3 normal;
vec3 lightDir;
vec3 viewDir;
float distance;
vec3 vert_color;
} Vert;

void main()
{
	vec3 normal = normalize(Vert.normal);
	vec3 lightDir = normalize(Vert.lightDir);
	vec3 viewDir = normalize(Vert.viewDir);

	float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * Vert.distance + light.attenuation[2] * Vert.distance*Vert.distance);
	color = material.emission;
	color += material.ambient*light.ambient*attenuation;
	float Ndot = max(dot(normal, lightDir), 0.0);
	color += material.diffuse*light.diffuse*Ndot*attenuation;
	float RdotVpow = max(pow(dot(reflect(-lightDir, normal), viewDir), material.shiness), 0.0);
	color += material.specular*light.specular*RdotVpow*attenuation;

	if (texture_mode == 0) {
		color *= texture(texture2, Vert.texcoord);
	} else if (texture_mode == 2) {
		color *= mix(texture(texture1, Vert.texcoord), texture(texture2, Vert.texcoord), 0.6);
	} else {
		color *= mix(texture(texture1, Vert.texcoord), vec4(0.0, 1.0, 0.0, 1.0), 0.2);
	}
}