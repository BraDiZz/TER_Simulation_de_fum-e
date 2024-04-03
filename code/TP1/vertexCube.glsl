#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position,1.);
}

