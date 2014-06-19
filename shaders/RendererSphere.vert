#version 330
uniform mat3 LIGHTGLgl_NormalMatrix;
uniform mat4 LIGHTGLgl_ModelViewMatrix;
uniform mat4 LIGHTGLgl_ProjectionMatrix;
uniform mat4 LIGHTGLgl_ModelViewProjectionMatrix;

attribute vec4 LIGHTGLgl_Vertex;

vec4 ftransform() {
    return LIGHTGLgl_ModelViewProjectionMatrix * LIGHTGLgl_Vertex;
}

uniform vec3 sphereCenter;
uniform float sphereRadius;


varying vec3 position;
void main() {
    position = sphereCenter + LIGHTGLgl_Vertex.xyz * sphereRadius;
    gl_Position = LIGHTGLgl_ModelViewProjectionMatrix * vec4(position, 1.0);
}
