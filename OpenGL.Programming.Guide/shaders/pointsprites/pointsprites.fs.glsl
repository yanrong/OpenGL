#version 330 core

uniform sampler2D spriteTexture;

out vec4 fColor;

void main(){
    fColor = texture(spriteTexture, vec2(gl_PointCoord.x, 1.0 - gl_PointCoord.y));
}