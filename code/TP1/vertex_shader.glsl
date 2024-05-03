#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float life;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform float size_cube;
uniform vec3 position_cube;

out float vertex_life;

void main() {
    vec3 pos = vec3(position.x,position.y,position.z);
    vertex_life=life;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(pos,1.);
}

