#version 330
uniform mat3 LIGHTGLgl_NormalMatrix;
uniform mat4 LIGHTGLgl_ModelViewMatrix;
uniform mat4 LIGHTGLgl_ProjectionMatrix;
uniform mat4 LIGHTGLgl_ModelViewProjectionMatrix;

in vec4 LIGHTGLgl_Vertex;

vec4 ftransform() {
	return LIGHTGLgl_ModelViewProjectionMatrix * LIGHTGLgl_Vertex;
}

out vec2 coord;
void main() {
	coord = LIGHTGLgl_Vertex.xy * 0.5 + 0.5;
	gl_Position = vec4(LIGHTGLgl_Vertex.xyz, 1.0);
}
