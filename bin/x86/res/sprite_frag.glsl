#version 330
in vec2 TexCoord;
out vec4 outColor;
uniform sampler2D Texture;
uniform vec4 Modulate;

void main() {
	outColor = texture(Texture, TexCoord) * Modulate;
}