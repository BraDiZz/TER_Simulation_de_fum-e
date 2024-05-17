#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 tc;

out vec4 FragColor;

uniform vec3 c;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float intensity;
uniform sampler2D texture_plan;
uniform float deltaTime;

uniform int plan;


void main(){
        
        vec3 camera=vec3(0.0f, 0.0f,  6.0f);

        vec3 ambiante=0.5*lightColor;
        vec3 L=lightPos-FragPos;
        vec3 diffuse=lightColor*dot(L,Normal);

        vec3 R=2*dot(Normal,L)*Normal-L;
        R=normalize(R);
        vec3 V=camera-FragPos;
        V=normalize(V);
        vec3 spec=lightColor*0.5*pow(max(dot(R,V),0.f),16);

        vec3 acc = (ambiante+diffuse+spec)*intensity;

        
        if(plan==1){
            vec3 coloracc = texture(texture_plan,tc).rgb;
            FragColor=vec4(acc*coloracc,1.0);   
        }else{
               FragColor=vec4(acc,1.0); 
        }


}