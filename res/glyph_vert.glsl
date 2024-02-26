#version 330 core
layout (location = 0) in vec4 aPos;
out vec2 TexCoord;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform float Time;
uniform bool Rainbow_Enabled;

void main() {
        gl_Position = vec4(aPos.xy, 0.0, 1.0) * ViewMatrix * ProjectionMatrix;
        TexCoord = aPos.zw;
}