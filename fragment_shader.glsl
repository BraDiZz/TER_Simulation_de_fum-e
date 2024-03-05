#version 330 core

in vec2 fragTexCoord;

// Ouput data
out vec3 color;

uniform sampler2D textureSampler;

void main(){
     vec3 texColor = texture(textureSampler, fragTexCoord).rgb;
     
     color =vec3(1., 1. ,1.);
}


// #version 330 core

// in vec3 particlePosition; // Entrée de position de la particule depuis le vertex shader
// out vec4 color;

// uniform float particleSize; // Variable uniforme pour la taille des particules

// void main() {
//     // Calculez la distance du fragment par rapport au centre de la particule
//     float distance = length(gl_PointCoord - vec2(0.5));

//     // Utilisez la distance pour déterminer la couleur
//     if (distance > 0.5) {
//         discard; // Masquer les fragments en dehors de la particule
//     }

//     color = vec4(1.0, 1.0, 1.0, 1.0); // Couleur blanche (à ajuster selon vos besoins)
// }