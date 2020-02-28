#version 430

//todo https://github.com/opengl-tutorials/ogl/blob/a9fe43fedef827240ce17c1c0f07e83e2680909a/tutorial08_basic_shading/StandardShading.fragmentshader

in vec3 Color;
in vec3 Position;
in vec3 Normal;
in vec3 LightDirection;
//in vec2 UV;

uniform vec3 u_AmbientColor;

layout(location = 0) out vec4 out_Color;

void main(void) {

    float LightPower = clamp(dot(normalize(Normal), normalize(LightDirection)), 0.0, 1.0);

    //out_Color = vec4(Color, 1);
    out_Color = vec4((Color * LightPower) + u_AmbientColor, 1);
}