#version 330 core

in vec2 TexCoords; 
out vec4 color;

uniform vec3 c;
uniform bool isParticle;

uniform sampler2D particleTexture;

void main() {
//     if (isParticle) { 
//         color = texture(particleTexture, TexCoords);
//     } else { 
//         color = vec4(c, 1.0);
//     }
        color = texture(particleTexture, TexCoords);
}