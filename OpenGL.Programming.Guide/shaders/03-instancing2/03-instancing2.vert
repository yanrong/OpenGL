#version 330 core

//position and normal are regular vertex attribute
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

//color is pre-instance attribute
layout (location = 2) in vec4 vColor;

//model matrix will be used as a pre-instance transformation matrix
//note that a mat4 consumes 4 consecutive location, so this will
//acutally sit in location 3, 4, 5, 6
layout (location = 3) in mat4 modelMatrix;

//the view matrix and the projection matrix are constant across a draw
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

//the output of the vertex shader(matched to the fragment shader)
out VERTEX {
    vec3 normal;
    vec4 color;
} vertex;

void main(){
    //construct a mode-view matrix fron the uniform view matrix,and the per-instance model matrix
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    //transform position by model-view matrix, then by projection matrix
    gl_Position = projectionMatrix * (modelViewMatrix * vPosition);
    //transform the normal by the upper-left-3x3-submatrix of the model-view matrix
    vertex.normal = mat3(modelViewMatrix) * vNormal;
    //pass the pre-instance color through to the fragment shader.
    vertex.color = vColor;
}
