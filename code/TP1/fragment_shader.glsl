#version 330 core

in float fragment_life;
in vec2 TexCoords; 
out vec4 color;

uniform vec3 c;
uniform sampler2D particleTexture;
uniform sampler2D particleTexture2;
uniform float transp;
uniform float deltaTime;
uniform float melange;
uniform float amplitude;




float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {

	vec2 noiseUV = TexCoords * 5.0 + vec2(deltaTime * 0.1); // Utilisation de time pour animer le bruit
    float noise = rand(noiseUV);

    //vec4 coloracc = mix(texture(particleTexture, TexCoords),texture(particleTexture2, TexCoords),melange);
    //vec4 coloracc = texture(particleTexture2, TexCoords);
    vec4 coloracc = texture(particleTexture, TexCoords+noise*amplitude);
    
    float transparency = 1.0 - (1.0 / (fragment_life/transp)); 


    if(coloracc.a<0.1){
 		discard;
    }

    color = vec4(coloracc.rgb * c, coloracc.a * 2. * transparency);
    //color = vec4(coloracc.a,0.,0. ,1.);

    


}


