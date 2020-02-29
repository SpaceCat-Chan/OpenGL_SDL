#version 430

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_UV;
layout(location = 2) in vec3 in_Normal;

out vec3 Color;
out vec3 Position;
out vec3 Normal;
out vec3 LightDirection;
//out vec2 UV;

uniform mat4 MVP;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform vec3 u_Color;
uniform vec3 u_LightPosition;

void main(void) {
    //UV = in_UV;


    Color = u_Color;
    Position = (u_Model * vec4(in_Position, 1)).xyz;
    Normal = (u_View * u_Model * vec4(in_Normal, 0)).xyz;


	vec3 EyeDirection_cameraspace = vec3(0,0,0) - ( u_View * u_Model * vec4(in_Position, 1)).xyz;
    
	vec3 LightPosition_cameraspace = ( u_View * vec4(u_LightPosition, 1)).xyz;
	LightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;

    //Color = (u_View * vec4(in_Position, 1)).xyz; //normalize(( u_View * u_Model * vec4(in_Position, 1)).xyz);

    gl_Position = MVP * vec4(in_Position, 1);
}