#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 TexCoords; 

uniform bool isParticle;

void main() {
    vec4 pos = gl_in[0].gl_Position;
    float size = 0.1; // size of the quad


    gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
    TexCoords = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(size, -size, 0.0, 0.0);
    TexCoords = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(-size, size, 0.0, 0.0);
    TexCoords = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = pos + vec4(size, size, 0.0, 0.0);
    TexCoords = vec2(1.0, 1.0);
    EmitVertex();

    // Ne fonctionne pas..., obliger de créer un nv shader
    // if (isParticle) { 
    //     gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
    //     TexCoords = vec2(0.0, 0.0);
    //     EmitVertex();

    //     gl_Position = pos + vec4(size, -size, 0.0, 0.0);
    //     TexCoords = vec2(1.0, 0.0);
    //     EmitVertex();

    //     gl_Position = pos + vec4(-size, size, 0.0, 0.0);
    //     TexCoords = vec2(0.0, 1.0);
    //     EmitVertex();

    //     gl_Position = pos + vec4(size, size, 0.0, 0.0);
    //     TexCoords = vec2(1.0, 1.0);
    //     EmitVertex();
    // } else { 
    //     gl_Position = pos;
    //     TexCoords = vec2(0.0, 0.0);
    //     EmitVertex();
    // }

    EndPrimitive();
}