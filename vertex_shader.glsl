#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.

uniform mat4 Model;  // Model matrix
uniform mat4 View;  // View matrix
uniform mat4 Projection;  // Projection matrix


void main(){
        // TODO : Output position of the vertex, in clip space : MVP * position
        mat4 MVP = Projection * View * Model;
        gl_Position = MVP * vec4(vertices_position_modelspace,1.);

}

// #version 330 core

// layout(location = 0) in vec3 vertices_position_modelspace;

// uniform mat4 MVP;  // Combined Model-View-Projection matrix

// void main() {
//     gl_Position = MVP * vec4(vertices_position_modelspace, 1.0);
// }
