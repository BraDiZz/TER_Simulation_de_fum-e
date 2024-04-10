#version 330 core

in float fragment_life;
in vec2 TexCoords; 
out vec4 color;

uniform vec3 c;
uniform sampler2D particleTexture;
uniform float transp;


void main() {
    vec4 coloracc = texture(particleTexture, TexCoords);


    float transparency = 1.0 - (1.0 / (fragment_life/transp)); 
    
    color = vec4(coloracc.rgb * c, coloracc.a * transparency);
    //color = vec4(vec3(1.,1.,1.) * c,transparency);


}


