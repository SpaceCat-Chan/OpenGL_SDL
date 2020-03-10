#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_UV;
layout(location = 2) in vec3 in_Normal;
layout(location = 3) in float in_Shininess;

out Material {
	float Shininess;
} material;

out VertexInfo {
	vec3 Position;
	out vec3 Normal;
	out vec3 LightDirection;
	float LightDistance;
	out vec2 UV;
} vertex;


uniform mat4 MVP;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform vec3 u_LightPosition;

void main(void) {
	material.Shininess = in_Shininess;

	vertex.UV = in_UV;

	vertex.Normal = normalize(vec3(u_View * u_Model * vec4(in_Normal, 0)));

	vertex.LightDirection = u_LightPosition - vec3(u_View * u_Model * vec4(in_Position, 1));
	vertex.LightDistance = length(vertex.LightDirection);
	vertex.LightDirection = normalize(vertex.LightDirection);

	vertex.Position = (u_View * u_Model * vec4(in_Position, 1)).xyz;
	//Normal = (u_View * u_Model * vec4(in_Normal, 0)).xyz;


	//vec3 EyeDirection_cameraspace = vec3(0,0,0) - ( u_View * u_Model * vec4(in_Position, 1)).xyz;

	//vec3 LightPosition_cameraspace = ( u_View * vec4(u_LightPosition, 1)).xyz;
	//LightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;

	//Color = (u_View * vec4(in_Position, 1)).xyz; //normalize(( u_View * u_Model * vec4(in_Position, 1)).xyz);

	gl_Position = MVP * vec4(in_Position, 1);
}