#version 330
in vec3 aPos;
uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
	vec4 pos = vec4(aPos, 1.0);
	gl_Position = pos * WorldMatrix * ViewMatrix * ProjectionMatrix;
}