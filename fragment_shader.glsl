#version 330 core

// Ouput data
out vec4 color;

uniform vec3 c;

uniform sampler2D text;

in vec2 uv2;

void main(){
        // color =vec4(c,1.);
        color = texture(text,uv2);
}
