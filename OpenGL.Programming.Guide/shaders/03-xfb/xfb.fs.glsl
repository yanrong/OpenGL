#version 410

layout (location = 0) out vec4 color;
in vec3 fNormal;

void main(){
    color = vec4(0.0, 0.2, 0.0, 1.0) + vec4(0.2, 0.5, 0.4, 1.0) * abs(fNormal.z)
    + vec4(0.8, 0.9, 0.7, 1.0) * pow(abs(fNormal.z), 70.0);
}