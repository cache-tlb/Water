#version 330
uniform mat3 LIGHTGLgl_NormalMatrix;
uniform mat4 LIGHTGLgl_ModelViewMatrix;
uniform mat4 LIGHTGLgl_ProjectionMatrix;
uniform mat4 LIGHTGLgl_ModelViewProjectionMatrix;

in vec4 LIGHTGLgl_Vertex;
in vec4 LIGHTGLgl_TexCoord;
in vec3 LIGHTGLgl_Normal;

vec4 ftransform() {
	return LIGHTGLgl_ModelViewProjectionMatrix * LIGHTGLgl_Vertex;
}

const float poolHeight = 1.0;

out vec3 position;
void main() {
	position = LIGHTGLgl_Vertex.xyz;
	position.y = ((1.0 - position.y) * (7.0 / 12.0) - 1.0) * poolHeight;
	gl_Position = LIGHTGLgl_ModelViewProjectionMatrix * vec4(position, 1.0);
}