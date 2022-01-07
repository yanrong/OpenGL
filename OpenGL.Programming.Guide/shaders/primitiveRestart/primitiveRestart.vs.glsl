#version 330 core

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

out vec4 fColor;

void main(void)
{
    const vec4 pos[3] = vec4[3](vec4(-0.3, -0.3, 0.0, 1.0),
        vec4(0.3, -0.3, 0.0, 1.0), vec4(-0.3, 0.3, 0.0, 1.0) );
    fColor = vColor;
    gl_Position = projectionMatrix * (modelMatrix * vPosition);
}