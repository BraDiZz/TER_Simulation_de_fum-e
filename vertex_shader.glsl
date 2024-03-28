#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out uv2;

void main() {
    // Output position of the vertex, in clip space : MVP * position
    uv2 = uv;

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position,1.);
}