#version 330
precision highp float;
uniform mat3 LIGHTGLgl_NormalMatrix;
uniform mat4 LIGHTGLgl_ModelViewMatrix;
uniform mat4 LIGHTGLgl_ProjectionMatrix;
uniform mat4 LIGHTGLgl_ModelViewProjectionMatrix;

uniform sampler2D texture;
uniform vec2 delta;
in vec2 coord;
out vec4 outputF;
void main() {
    /* get vertex info */
    vec4 info = texture2D(texture, coord);

    /* update the normal */
    vec3 dx = vec3(delta.x, texture2D(texture, vec2(coord.x + delta.x, coord.y)).r - info.r, 0.0);
    vec3 dy = vec3(0.0, texture2D(texture, vec2(coord.x, coord.y + delta.y)).r - info.r, delta.y);
    info.ba = normalize(cross(dy, dx)).xz;

    outputF = info;
}