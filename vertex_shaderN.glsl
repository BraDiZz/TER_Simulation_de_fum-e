// #version 330 core

// // Input vertex data, different for all executions of this shader.
// layout(location = 0) in vec3 vertices_position_modelspace;
// layout(location = 1) in vec2 uv;

// uniform mat4 Model;
// uniform mat4 View; // o_uv0=uv;
// uniform mat4 Projection;
// uniform int mode;

// uniform sampler2D HMPlan;

// out vec2 uv2;
// uniform vec3 heightBounds;
// out vec4 boundsAndheight;


// //TODO create uniform transformations matrices Model View Projection
// // Values that stay constant for the whole mesh.

// void main(){

//         uv2=uv;
//         //mode heightmap
//         // if(mode==1){
//         //         float height = texture(HMPlan, uv).r;
//         //         boundsAndheight=vec4(heightBounds,height);
        
//         //         //height plus vertices.y pour gérer les offset en y
//         //         vec3 poswithheight=vec3(vertices_position_modelspace[0],height+vertices_position_modelspace.y,vertices_position_modelspace[2]);
//         //         gl_Position =   Projection*View*Model*vec4(poswithheight,1);
        
//         // //mode classique
//         // }else{
//         //         gl_Position =   Projection*View*Model*vec4(vertices_position_modelspace,1);
//         //         boundsAndheight=vec4(0,0,0,0);
//         // }
//         gl_Position =   Projection*View*Model*vec4(vertices_position_modelspace,1);
//         boundsAndheight=vec4(0,0,0,0);

// }

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 uv;

uniform mat4 Model;
uniform mat4 Projection; 
uniform mat4 View;

out vec2 uv2;

uniform sampler2D HMPlan;
uniform int mode;

uniform vec3 heightBounds;
out vec4 boundsAndheight;       

void main(){
        uv2=uv;

        // float height = texture(HMPlan, uv).r;
        // boundsAndheight=vec4(heightBounds,height);

        mat4 MVP = Projection * View * Model;
        
        //height plus vertices.y pour gérer les offset en y
        // vec3 poswithheight=vec3(vertices_position_modelspace[0],height+vertices_position_modelspace.y,vertices_position_modelspace[2]);
        // gl_Position =   MVP*vec4(poswithheight,1);

        gl_Position = MVP * vec4(vertices_position_modelspace,1);
}

