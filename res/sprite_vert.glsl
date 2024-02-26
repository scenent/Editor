#version 330
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 ProjectionMatrix;
uniform vec2 TexSize;

void main() {
	vec4 pos = vec4(aPos, 1.0);
	pos.x *= TexSize.x;
	pos.y *= TexSize.y;
	gl_Position = pos * ProjectionMatrix;
	TexCoord = aTexCoord;
}