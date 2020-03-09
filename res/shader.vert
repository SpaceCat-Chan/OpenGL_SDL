#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_UV;
layout(location = 2) in vec3 in_Normal;
//layout(location = 3) in vec3 in_Color;
//ayout(location = 4) in vec3 in_Ambient;
//layout(location = 5) in vec3 in_Diffuse;
//layout(location = 6) in vec3 in_Specular;

out Material {
	vec3 Color;
	float Ambient;
	float Diffuse;
	float Specular;
} material;

//out vec3 Position;
//out vec3 Normal;
//out vec3 LightDirection;
//out vec2 UV;

uniform mat4 MVP;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform vec3 u_LightPosition;

void main(void) {

	material.Color = vec3(0.1, 0.3, 0.5);
	material.Ambient = 0.7;
	material.Diffuse = 0;
	material.Specular = 0;

	//UV = in_UV;


	//Position = (u_Model * vec4(in_Position, 1)).xyz;
	//Normal = (u_View * u_Model * vec4(in_Normal, 0)).xyz;


	//vec3 EyeDirection_cameraspace = vec3(0,0,0) - ( u_View * u_Model * vec4(in_Position, 1)).xyz;

	//vec3 LightPosition_cameraspace = ( u_View * vec4(u_LightPosition, 1)).xyz;
	//LightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;

	//Color = (u_View * vec4(in_Position, 1)).xyz; //normalize(( u_View * u_Model * vec4(in_Position, 1)).xyz);

	gl_Position = MVP * vec4(in_Position, 1);
}