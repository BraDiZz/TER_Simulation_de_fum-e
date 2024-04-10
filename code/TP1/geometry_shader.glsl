#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in float vertex_life[];

out float fragment_life;

out vec2 TexCoords; 

uniform int size_p;


void main() {
    fragment_life=vertex_life[0];

    vec4 pos = gl_in[0].gl_Position;
    float size = 0.01*size_p; // size of the quad

    //vec2 texCoordOffset = pos.xy * 0.5 + 0.5;
    //vec2 texCoordOffset = pos.xy;

    vec2 texCoordOffset = vec2(0.,0.);
    
    /*
    gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(size, -size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.2, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(-size, size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.0, 0.2);
    EmitVertex();

    gl_Position = pos + vec4(size, size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.2, 0.2);
    EmitVertex();

    EndPrimitive();
    */


     
    gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(size, -size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(1, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(-size, size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(0.0, 1);
    EmitVertex();

    gl_Position = pos + vec4(size, size, 0.0, 0.0);
    TexCoords = texCoordOffset + vec2(1, 1);
    EmitVertex();

    EndPrimitive();


}


