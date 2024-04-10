#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in float life;

// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out float vertex_life;

void main() {
    // Output position of the vertex, in clip space : MVP * position
    vec3 pos = vec3(position.x,position.y,position.z);
    vertex_life=life;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(pos,1.);
}

