#version 330 core

uniform sampler2D spriteTexture;

out vec4 fColor;

void main(){
    const vec4 color1 = vec4(0.6, 0.0, 0.0, 1.0);
    const vec4 color2 = vec4(0.9, 0.7, 1.0, 0.0);

    vec2 tmp = gl_PointCoord - vec2(0.5);
    float f = dot(tmp, tmp);

    if (f > 0.25)
        discard;

    fColor = mix(color1, color2, smoothstep(0.1, 0.25, f));
}