#version 330 core


in vec2 uv2;

// Ouput data
out vec4 color;

uniform sampler2D text;

in vec4 boundsAndheight;


void main(){

    color = texture(text,uv2);
}
