// #version 330 core

// layout (points) in;
// layout (triangle_strip, max_vertices = 6) out;

// in float vertex_life[];

// out float fragment_life;

// out vec2 TexCoords; 

// uniform int size_p;

// uniform float time;

// void main() {
//     fragment_life=vertex_life[0];

//     vec4 pos = gl_in[0].gl_Position;
//     float size = 0.01*size_p; // size of the quad

//     //vec2 texCoordOffset = pos.xy * 0.5 + 0.5;
//     vec2 texCoordOffset = pos.xy;

//     /*
//     //temps de déplacemrnt
//     float offsetX = time * 0.1; // Adjust speed here
//     float offsetY = time * -1; // Adjust speed here
//     vec2 texCoordOffset = vec2(pos.x + offsetX, pos.y + offsetY);*/

//     gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
//     TexCoords = texCoordOffset + vec2(0.0, 0.0);
//     EmitVertex();

//     gl_Position = pos + vec4(size, -size, 0.0, 0.0);
//     TexCoords = texCoordOffset + vec2(1.0, 0.0);
//     EmitVertex();

//     gl_Position = pos + vec4(-size, size, 0.0, 0.0);
//     TexCoords = texCoordOffset + vec2(0.0, 1.0);
//     EmitVertex();

//     gl_Position = pos + vec4(size, size, 0.0, 0.0);
//     TexCoords = texCoordOffset + vec2(1.0, 1.0);
//     EmitVertex();

//     EndPrimitive();
// }

#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in float vertex_life[];

out float fragment_life;

out vec2 TexCoords; 

uniform int size_p;

uniform float time;

void main() {
    fragment_life=vertex_life[0];

    vec4 pos = gl_in[0].gl_Position;
    float size = 0.01*size_p; // size of the quad

    vec2 texCoordOffset = pos.xy;

    gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.4, 0.6);
    EmitVertex();

    gl_Position = pos + vec4(size, -size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.6, 0.4);
    EmitVertex();

    gl_Position = pos + vec4(-size, size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.45, 0.5);
    EmitVertex();

    gl_Position = pos + vec4(size, size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.7, 0.2);
    EmitVertex();

    EndPrimitive();
}

