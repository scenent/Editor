#version 330 core
in vec2 TexCoord;
out vec4 color;

uniform float Time;
uniform sampler2D text;
uniform vec4 textColor;
uniform bool Rainbow_Enabled;

void main() {    
    if (Rainbow_Enabled){
        vec3 rainbowColor = vec3(
            abs(sin(Time)), 
            abs(sin(Time + 2.0)), 
            abs(sin(Time + 4.0))
        );
        vec4 sampled = vec4(rainbowColor, texture(text, TexCoord).r);
        color = textColor * sampled;
    }
    else{
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoord).r);
        color = textColor * sampled;
    }
}
