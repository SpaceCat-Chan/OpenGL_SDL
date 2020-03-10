#version 430 core

in Material {
	vec3 Color;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
} material;

in VertexInfo {
	in vec3 Position;
	in vec3 Normal;
	in vec3 LightDirection;
	float LightDistance;
	in vec2 UV;
} Fragment;


uniform vec3 u_LightColor;
uniform sampler2D u_Texture;

layout(location = 0) out vec4 out_Color;

void main(void) {

	vec3 Ambient = material.Ambient * material.Color * u_LightColor;

	vec3 N = Fragment.Normal;
	vec3 L = Fragment.LightDirection;

	float DiffusePower = max(dot(N, L), 0.0);
	vec3 Diffuse = material.Diffuse * DiffusePower * u_LightColor;
	

	vec3 R = reflect(-L, N);
	vec3 P = normalize(Fragment.Position * -1);

	float SpecularPower = pow(max(dot(P, R), 0.0), material.Shininess);
	vec3 Specular = material.Specular * SpecularPower * u_LightColor;

	out_Color = vec4((Ambient + Diffuse + Specular) * texture(u_Texture, Fragment.UV).rgb, 1);
}