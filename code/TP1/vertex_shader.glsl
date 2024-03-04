#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertices_position_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = ProjectionMatrix * ViewMatrix * vertices_position_modelspace;
}

