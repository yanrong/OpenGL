#version 410

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out vec4 worldPosition;
out vec3 fNormal;

void main(){
    vec4 pos = (modelMatrix * (position * vec4(1.0, 1.0, 1.0, 1.0)));
    worldPosition = pos;
    fNormal = normalize((modelMatrix * vec4(normal, 0.0)).xyz);
    gl_Position = projectionMatrix * pos;
}