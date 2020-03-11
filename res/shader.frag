#version 430 core

in Material {
	float Shininess;
} material;

in VertexInfo {
	in vec3 Tangent_ModelPosition;
	in vec3 Tangent_Normal;
	in vec3 Tangent_LightDirection;
	float Tangent_LightDistance;
	in vec2 UV;
} Fragment;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform vec3 u_LightColor;
uniform sampler2D u_Texture;
uniform sampler2D u_Specular;
uniform sampler2D u_Bump;
uniform bool u_UseBumpMap;

layout(location = 0) out vec4 out_Color;

void main(void) {

	vec3 Ambient = texture(u_Texture, Fragment.UV).rgb * u_LightColor;

	vec3 Tangent_Normal;
	if(u_UseBumpMap) {
		Tangent_Normal = -texture(u_Bump, Fragment.UV).xyz;
		Tangent_Normal = Tangent_Normal * 2.0 - 1.0;
		Tangent_Normal = vec3(Tangent_Normal.x, Tangent_Normal.y, Tangent_Normal.z);
		Tangent_Normal = normalize(Tangent_Normal);
	}
	else {
		Tangent_Normal = Fragment.Tangent_Normal;
	}

	vec3 N = Tangent_Normal;
	vec3 L = Fragment.Tangent_LightDirection;

	float DiffusePower = max(dot(N, L), 0.0);
	vec3 Diffuse = texture(u_Texture, Fragment.UV).rgb * DiffusePower * u_LightColor;
	

	vec3 P = normalize(Fragment.Tangent_ModelPosition * -1);
	vec3 H = normalize(L + P);

	float SpecularPower = pow(max(dot(N, H), 0.0), material.Shininess);
	vec3 Specular = texture(u_Specular, Fragment.UV).rgb * SpecularPower * u_LightColor;

	float Attenuation = 1 / (Fragment.Tangent_LightDistance);

	Ambient *= Attenuation;
	Diffuse *= Attenuation;
	Specular *= Attenuation;

	out_Color = vec4((Ambient + Diffuse + Specular), 1);
}