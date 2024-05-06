#version 330 core

in float fragment_life;
in vec2 TexCoords; 
in vec3 FragPos;
out vec4 color;

uniform vec3 c;
uniform sampler2D particleTexture;
uniform sampler2D particleTexture2;
uniform float transp;
uniform float deltaTime;
uniform float melange;
uniform float amplitude;


uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float intensity;

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

    
    vec3 camera=vec3(0.0f, 0.0f,6.0f);
    vec3 Normal=vec3(0.f,0.f,1.f);
    Normal=normalize(Normal);

    vec3 ambiante=0.5*lightColor;
    vec3 L=lightPos-FragPos;
    vec3 diffuse=lightColor*dot(L,Normal);

    vec3 R=2*dot(Normal,L)*Normal-L;
    R=normalize(R);
    vec3 V=camera-FragPos;
    V=normalize(V);
    vec3 spec=lightColor*0.5*pow(max(dot(R,V),0.f),16);

    vec3 acc = (ambiante+diffuse+spec)*intensity;



    color = vec4(coloracc.rgb * c, coloracc.a * 2. * transparency);
    //color = vec4(coloracc.a,0.,0. ,1.);

    


}


