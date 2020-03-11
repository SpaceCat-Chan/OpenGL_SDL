#version 430 core

layout(location = 0) in vec3 in_Model_Position;
layout(location = 1) in vec2 in_UV;
layout(location = 2) in vec3 in_Model_Normal;
layout(location = 3) in float in_Shininess;
layout(location = 4) in vec3 in_Model_Tangent;

out Material {
	float Shininess;
} material;

out VertexInfo {
	vec3 Tangent_ModelPosition;
	out vec3 Tangent_Normal;
	out vec3 Tangent_LightDirection;
	float Tangent_LightDistance;
	out vec2 UV;
} vertex;


uniform mat4 MVP;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform vec3 u_Camera_LightPosition;

void main(void) {

	vec3 N = normalize(vec3(u_View * u_Model * vec4(in_Model_Normal, 0.0)));
	vec3 T = normalize(vec3(u_View * u_Model * vec4(in_Model_Tangent, 0.0)));

	T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));


	material.Shininess = in_Shininess;

	vertex.UV = in_UV;

	vertex.Tangent_Normal = normalize(TBN * vec3(u_View * u_Model * vec4(in_Model_Normal, 0)));

	vertex.Tangent_LightDirection = u_Camera_LightPosition - vec3(u_View * u_Model * vec4(in_Model_Position, 1));
	vertex.Tangent_LightDistance = length(vertex.Tangent_LightDirection);
	vertex.Tangent_LightDirection = TBN * normalize(vertex.Tangent_LightDirection);

	vertex.Tangent_ModelPosition = TBN * (u_View * u_Model * vec4(in_Model_Position, 1)).xyz;
	//Normal = (u_View * u_Model * vec4(in_Normal, 0)).xyz;


	//vec3 EyeDirection_cameraspace = vec3(0,0,0) - ( u_View * u_Model * vec4(in_Position, 1)).xyz;

	//vec3 LightPosition_cameraspace = ( u_View * vec4(u_LightPosition, 1)).xyz;
	//LightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;

	//Color = (u_View * vec4(in_Position, 1)).xyz; //normalize(( u_View * u_Model * vec4(in_Position, 1)).xyz);

	gl_Position = MVP * vec4(in_Model_Position, 1);
}