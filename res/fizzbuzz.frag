#version 430 core
#define MaxLightAmount 8

in Material {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
} material;

in VertexInfo {
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
} Fragment;

uniform float u_Lerp_Value;

uniform mat4 u_Model;
uniform mat4 u_Model_Old;
uniform mat4 u_View;
uniform sampler2D u_Texture;
uniform sampler2D u_Specular;
uniform sampler2D u_Bump;
uniform sampler2D u_Disp;
uniform bool u_UseBumpMap;
uniform bool u_UseDispMap;
uniform bool u_UseTextures;

uniform uint u_AmountOfLights;
uniform vec3 u_LightColor[MaxLightAmount];
uniform vec3 u_Camera_LightPosition[MaxLightAmount];
uniform vec3 u_LightAttenuationPacked[MaxLightAmount];
uniform float u_LightCutoffAngle[MaxLightAmount];

uniform bool u_FullBright;

vec2 CalculateDisplacement(vec3 Normal, vec2 UV);

layout(location = 0) out vec4 out_Color;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 FizzBuzz(vec2 UV)
{
	int X = int(floor(map(UV.x, 0, 1, 0, 1080)));
	int Y = int(floor(map(UV.y, 0, 1, 0, 1080)));

	int Number = X + Y * 1080;

	vec3 ResultColor = {0, 0, 0};
	if (Number % 3 == 0)
	{
		ResultColor.r = 1;
	}
	if (Number % 5 == 0)
	{
		ResultColor.g = 1;
	}
	if (Number % 7 == 0)
	{
		ResultColor.b = 1;
	}
	return ResultColor;
}

void main(void) {

	vec2 UV;
	if(u_UseDispMap) {
		UV = CalculateDisplacement(Fragment.Tangent_Normal, Fragment.UV);
	}
	else {
		UV = Fragment.UV;
	}


	vec3 Tangent_Normal;
	if(u_UseBumpMap && u_UseTextures) {
		Tangent_Normal = texture(u_Bump, UV).xyz;
		Tangent_Normal = Tangent_Normal * 2.0 - 1.0;
		Tangent_Normal = vec3(Tangent_Normal.x, -Tangent_Normal.y, Tangent_Normal.z);
		Tangent_Normal = normalize(Tangent_Normal);
	}
	else {
		Tangent_Normal = Fragment.Tangent_Normal;
	}

	vec3 LightColors;

	vec3 FizzBuzzResult = FizzBuzz(UV);

	if(u_FullBright)
	{
		LightColors = FizzBuzzResult;
	}

	for(int i=0; i < u_AmountOfLights; i++) {
		vec3 Ambient = u_LightColor[i] * material.Ambient;


		vec3 P = normalize(Fragment.Tangent_CameraPosition - Fragment.Tangent_ModelPosition);

		float Theta = dot(normalize(u_Camera_LightPosition[i] - Fragment.Camera_ModelPosition), -Fragment.Camera_LightPointingDirection[i]);

		vec3 Diffuse = vec3(0, 0, 0);
		vec3 Specular = vec3(0, 0, 0);

		Ambient *= FizzBuzzResult;

		if(Theta > u_LightCutoffAngle[i])
		{
			vec3 N = Tangent_Normal;
			vec3 L = Fragment.Tangent_LightDirection[i];

			float DiffusePower = max(dot(N, L), 0.0);
			Diffuse = DiffusePower * u_LightColor[i] * material.Diffuse;


			Diffuse *= FizzBuzzResult;


			vec3 H = normalize(L + P);

			float SpecularPower = pow(max(dot(N, H), 0.0), material.Shininess);
			Specular = SpecularPower * u_LightColor[i] * material.Specular;

			Specular *= FizzBuzzResult;
		}
		
		float Attenuation = 1 / (u_LightAttenuationPacked[i].x + 
								u_LightAttenuationPacked[i].y * Fragment.Tangent_LightDistance[i] + 
								u_LightAttenuationPacked[i].z * Fragment.Tangent_LightDistance[i] * Fragment.Tangent_LightDistance[i]);

		Ambient *= Attenuation;
		Diffuse *= Attenuation;
		Specular *= Attenuation;


		LightColors += Ambient + Diffuse + Specular;
	}

	out_Color = vec4(LightColors, 1);
}

vec2 CalculateDisplacement(vec3 Normal, vec2 UV) {
	vec3 OrigViewDir = normalize(Fragment.Tangent_CameraPosition - Fragment.Tangent_ModelPosition);

	const float MinLayers = 8, MaxLayers = 16;

	float Layers = mix(MaxLayers, MinLayers, abs(dot(vec3(0.0, 0.0, 1.0), OrigViewDir)));
	float LayerDepth = 1.0/Layers;

	float CurrentLayer = 0.0;

	vec2 ViewDir = OrigViewDir.xy * 0.1;
	vec2 ShiftAmount = ViewDir/Layers;

	vec2 UVResult = UV;
	float Displacement = texture(u_Disp, UVResult).r;

	while(CurrentLayer < Displacement) {
		UVResult -= ShiftAmount;

		Displacement = texture(u_Disp, UVResult).r;

		CurrentLayer += LayerDepth;
	}

	vec2 PrevUV = UVResult + ShiftAmount;

	float AfterDepth = Displacement - CurrentLayer;
	float BeforeDepth = texture(u_Disp, PrevUV).r - CurrentLayer + LayerDepth;

	float Weight = AfterDepth / (AfterDepth - BeforeDepth);
	UVResult = PrevUV * Weight + UVResult * (1.0 - Weight); 

	if(UVResult.x < 0 || UVResult.x > 1 || UVResult.y < 0 || UVResult.y > 1) {
		discard;
	}
	return UVResult;

	//return UV;
}