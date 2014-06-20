#version 330
uniform mat3 LIGHTGLgl_NormalMatrix;
uniform mat4 LIGHTGLgl_ModelViewMatrix;
uniform mat4 LIGHTGLgl_ProjectionMatrix;
uniform mat4 LIGHTGLgl_ModelViewProjectionMatrix;

in vec4 LIGHTGLgl_Vertex;

vec4 ftransform() {
	return LIGHTGLgl_ModelViewProjectionMatrix * LIGHTGLgl_Vertex;
}

uniform sampler2D water;
out vec3 position;

void main() {
	vec4 info = texture2D(water, LIGHTGLgl_Vertex.xy * 0.5 + 0.5);
	position = LIGHTGLgl_Vertex.xzy;
	position.y += info.r;
	gl_Position = LIGHTGLgl_ModelViewProjectionMatrix * vec4(position, 1.0);
}
