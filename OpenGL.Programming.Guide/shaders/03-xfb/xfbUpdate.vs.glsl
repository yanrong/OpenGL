#version 410

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform int triangleCount;

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 velocity;

out vec4 positionOut;
out vec3 velocityOut;

uniform samplerBuffer geometryTbo;
uniform float timeStep = 0.02;

bool intersect(vec3 origin, vec3 direction, vec3 v0, vec3 v1, vec3 v2, out vec3 point)
{
    vec3 u, v, n;
    vec3 w0, w;
    float r, a, b;

    u = (v1 - v0);
    v = (v2 - v0);
    n = cross(u, v);

    //if (length(n) < 0.1) {
    //    return false;
    //}

    w0 = origin - v0;
    a = -dot(n, w0);
    b = dot(n, direction);
    //if (abs(b) < 0.1) {
    //    return false;
    //}

    r = a / b;
    if (r < 0.0 || r > 1.0) {
        return false;
    }

    point = origin + r * direction;

    float uu, uv, vv, wu, wv, D;

    uu = dot(u, u);
    uv = dot(u, v);
    vv = dot(v, v);
    w = point - v0;
    wu = dot(w, u);
    wv = dot(w, v);
    D = uv * uv - uu * vv;

    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0) {
        return false;
    }
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0) {
        return false;
    }

    return true;
}

vec3 reflectVector(vec3 v, vec3 n)
{
    return v - 2.0 * dot(v, n) * n;
}

void main(){
    vec3 accelleration = vec3(0.0, -0.3, 0.0);
    vec3 newVelocity = velocity + accelleration * timeStep;
    vec4 newPosition = position + vec4(newVelocity * timeStep, 0.0);
    vec3 v0, v1, v2;
    vec3 point;

    for (int i = 0; i < triangleCount; i++) {
        v0 = texelFetch(geometryTbo, i * 3).xyz;
        v1 = texelFetch(geometryTbo, i * 3 + 1).xyz;
        v2 = texelFetch(geometryTbo, i * 3 + 2).xyz;

        if (intersect(position.xyz, position.xyz - newPosition.xyz, v0, v1, v2 ,point)) {
            vec3 n = normalize(cross(v1 - v0, v2 - v0));
            newPosition = vec4(point + reflectVector(newPosition.xyz - point, n), 1.0);
            newVelocity = 0.8 * reflectVector(newVelocity, n);
        }
    }

    if (newPosition.y < -40.0) {
        newPosition = vec4(-newPosition.x * 0.3, position.y + 80.0, 0.0, 1.0);
        newVelocity *= vec3(0.2, 0.1, -0.3);
    }
    velocityOut = newVelocity * 0.9999;
    positionOut = newPosition;

    gl_Position = projectionMatrix * (modelMatrix * position);
}