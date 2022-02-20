#version 330 core

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec4 vPosition;

void main(){
    vec4 pos = projectionMatrix * (modelMatrix * vPosition);
    gl_PointSize = (1.0 - pos.z / pos.w) * 64.0;
    gl_Position = pos;
}