#version 430

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_UV;

//out vec2 out_UV;

uniform mat4 MVP;

void main(void) {
    //out_UV = in_UV;

    gl_Position = MVP * vec4(in_Position, 1);
}