#version 330 core

in float fragment_life;
in vec2 TexCoords; 
out vec4 color;

uniform vec3 c;
uniform sampler2D particleTexture;
uniform sampler2D particleTexture2;
uniform float transp;


void main() {
    //vec4 coloracc = mix(texture(particleTexture, TexCoords),texture(particleTexture2, TexCoords),0.5);
    vec4 coloracc = texture(particleTexture, TexCoords);
    float transparency = 1.0 - (1.0 / (fragment_life/transp)); 
    //float transparency = exp(-fragment_life / transp);

    color = vec4(coloracc.rgb * c, coloracc.a * 1 * transparency);
    //color = vec4((coloracc.rgb) * c, coloracc.a);
    //color = vec4(coloracc.a,0.,0. ,1.);

    if(coloracc.a<0.1){
 		discard;
    }


}


