#version 430
//todo https://github.com/opengl-tutorials/ogl/blob/a9fe43fedef827240ce17c1c0f07e83e2680909a/tutorial08_basic_shading/StandardShading.vertexshader

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_UV;
layout(location = 2) in vec3 in_Normal;

out vec3 Color;
out vec3 Position;
out vec3 Normal;
//out vec2 out_UV;

uniform mat4 MVP;
uniform vec3 u_LightPosition;
uniform vec3 u_Color;

void main(void) {
    //out_UV = in_UV;

    float LightPower = clamp(dot(normalize(in_Normal), normalize(in_LightPosition)), 0.0, 1.0);

    Color = u_Color * LightPower;

    gl_Position = MVP * vec4(in_Position, 1);
}