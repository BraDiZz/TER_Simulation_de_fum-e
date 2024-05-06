#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textcoord;

// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 tc;

void main() {
    FragPos = vec3(ModelMatrix * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(ModelMatrix))) * normal; // Transformer la normale en coordonnées du monde
    tc=textcoord;
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(FragPos, 1.0);
}

