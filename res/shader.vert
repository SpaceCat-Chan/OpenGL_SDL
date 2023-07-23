#version 420 core
#define MaxLightAmount 8

layout(location = 0) in vec3 in_Model_Position;
layout(location = 1) in vec2 in_UV;
layout(location = 2) in vec3 in_Model_Normal;
layout(location = 4) in vec3 in_Model_Tangent;
layout(location = 5) in vec3 in_Model_BiTangent;
layout(location = 6) in vec3 in_Ambient;
layout(location = 7) in vec3 in_Diffuse;
layout(location = 8) in vec3 in_Specular;
layout(location = 3) in float in_Shininess;

out Material {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
} material;

out VertexInfo {
	vec3 Tangent_ModelPosition;
	vec3 Tangent_Normal;
	vec3 Tangent_LightDirection[MaxLightAmount];
	float Tangent_LightDistance[MaxLightAmount];
	vec3 Camera_LightPointingDirection[MaxLightAmount];
	vec3 Camera_ModelPosition;
	vec3 Tangent_CameraPosition;
	vec2 UV;
	mat3 TBN;
	vec3 Normal;
	vec3 Tangent;
	vec3 BiTangent;
} vertex;


uniform mat4 MVP;
uniform mat4 MVP_Old;
uniform mat4 u_Model;
uniform mat4 u_Model_Old;
uniform mat4 u_View;

uniform float u_Lerp_Value;

uniform uint u_AmountOfLights;
uniform vec3 u_Camera_LightPosition[MaxLightAmount];
uniform vec3 u_LightDirection[MaxLightAmount];
uniform bool u_LightType[MaxLightAmount]; // false = point, true = directional

void main(void) {

	vec3 N = normalize(vec3(u_View * (u_Model * vec4(in_Model_Normal, 0.0))));
	vec3 T = normalize(vec3(u_View * (u_Model * vec4(in_Model_Tangent, 0.0))));
	vec3 B = normalize(vec3(u_View * (u_Model * vec4(in_Model_BiTangent, 0.0))));

	vec3 N_Old = normalize(vec3(u_View * (u_Model_Old * vec4(in_Model_Normal, 0.0))));
	vec3 T_Old = normalize(vec3(u_View * (u_Model_Old * vec4(in_Model_Tangent, 0.0))));
	vec3 B_Old = normalize(vec3(u_View * (u_Model_Old * vec4(in_Model_BiTangent, 0.0))));

	//T = normalize(T - dot(T, N) * N);
	//B = normalize(B - dot(B, N) * N);



	vertex.Normal = in_Model_Normal;
	vertex.Tangent = in_Model_Tangent;
	vertex.BiTangent = in_Model_BiTangent;

	mat3 TBN = transpose(mat3(T, B, N));
	mat3 TBN_Old = transpose(mat3(T_Old, B_Old, N_Old));
	vertex.TBN = mat3(T, B, N);

	material.Ambient = in_Ambient;
	material.Diffuse = in_Diffuse;
	material.Specular = in_Specular;
	material.Shininess = in_Shininess;

	vertex.UV = in_UV;

	vertex.Tangent_Normal = mix(
		normalize(TBN_Old * (u_View * u_Model_Old * vec4(in_Model_Normal, 0)).xyz),
		normalize(TBN * (u_View * u_Model * vec4(in_Model_Normal, 0)).xyz),
		u_Lerp_Value);

	vertex.Camera_ModelPosition = mix(
		vec3(u_View * u_Model_Old * vec4(in_Model_Position, 1)),
		vec3(u_View * u_Model * vec4(in_Model_Position, 1)),
		u_Lerp_Value);

	for(int i=0; i < u_AmountOfLights; i++) {
		if(u_LightType[i] == false)
		{
			vertex.Tangent_LightDirection[i] = u_Camera_LightPosition[i] - vertex.Camera_ModelPosition;
			vertex.Tangent_LightDistance[i] = length(vertex.Tangent_LightDirection[i]);
			vertex.Tangent_LightDirection[i] = mix(
				normalize(TBN_Old * normalize(vertex.Tangent_LightDirection[i])),
				normalize(TBN * normalize(vertex.Tangent_LightDirection[i])),
				u_Lerp_Value);
		}
		else
		{
			vertex.Tangent_LightDirection[i] = mix(
				normalize(TBN_Old * normalize(-u_LightDirection[i])),
				normalize(TBN * normalize(-u_LightDirection[i])),
				u_Lerp_Value);
			vertex.Tangent_LightDistance[i] = 0.01;
		}
		vertex.Camera_LightPointingDirection[i] = normalize(u_LightDirection[i]);
	}

	vertex.Tangent_ModelPosition = mix(
		TBN_Old * (u_View * (u_Model_Old * vec4(in_Model_Position, 1))).xyz,
		TBN * (u_View * (u_Model * vec4(in_Model_Position, 1))).xyz,
		u_Lerp_Value);


	vertex.Tangent_CameraPosition = mix(
		TBN_Old * vec3(0, 0, 0),
		TBN * vec3(0, 0, 0),
		u_Lerp_Value);
	//Normal = (u_View * u_Model * vec4(in_Normal, 0)).xyz;


	//vec3 EyeDirection_cameraspace = vec3(0,0,0) - ( u_View * u_Model * vec4(in_Position, 1)).xyz;

	//vec3 LightPosition_cameraspace = ( u_View * vec4(u_LightPosition, 1)).xyz;
	//LightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;

	//Color = (u_View * vec4(in_Position, 1)).xyz; //normalize(( u_View * u_Model * vec4(in_Position, 1)).xyz);

	gl_Position = mix(
		MVP_Old * vec4(in_Model_Position, 1),
		MVP * vec4(in_Model_Position, 1),
		u_Lerp_Value);
}
