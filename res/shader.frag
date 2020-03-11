#version 430 core

in Material {
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
uniform sampler2D u_Specular;
uniform sampler2D u_Bump;
uniform bool u_UseBumpMap;

layout(location = 0) out vec4 out_Color;

void main(void) {

	vec3 Ambient = texture(u_Texture, Fragment.UV).rgb * u_LightColor;

	vec3 Normal;
	if(u_UseBumpMap) {
		texture(u_Bump, Fragment.UV);
		Normal = normalize(Normal * 2 - 1);
	}
	else {
		Normal = Fragment.Normal;
	}

	vec3 N = Normal;
	vec3 L = Fragment.LightDirection;

	float DiffusePower = max(dot(N, L), 0.0);
	vec3 Diffuse = texture(u_Texture, Fragment.UV).rgb * DiffusePower * u_LightColor;
	

	vec3 R = reflect(-L, N);
	vec3 P = normalize(Fragment.Position * -1);

	float SpecularPower = pow(max(dot(P, R), 0.0), material.Shininess);
	vec3 Specular = texture(u_Specular, Fragment.UV).rgb * SpecularPower * u_LightColor;

	out_Color = vec4(Ambient + Diffuse + Specular, 1);
}