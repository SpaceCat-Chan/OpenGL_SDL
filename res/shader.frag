#version 430 core

in Material {
	vec3 Color;
	float Ambient;
	float Diffuse;
	float Specular;
} material;

in vec3 Position;
//in vec3 view_Normal;
//in vec3 view_LightDirection;
//in vec2 UV;

uniform vec3 u_LightColor;

layout(location = 0) out vec4 out_Color;

void main(void) {

	vec3 AmbientColor = material.Ambient * material.Color * u_LightColor;

	//float Diffuse = clamp(dot(normalize(view_Normal), normalize(view_LightDirection)), 0.0, 1.0);

	//out_Color = vec4(Color, 1);
	//out_Color = vec4(1.0, 1.0, 1.0, 1.0);
	out_Color = vec4(Position, 1);
	//out_Color = vec4((material.Color * material.Diffuse) + AmbientColor, 1);
}