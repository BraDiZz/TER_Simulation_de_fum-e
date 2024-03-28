#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 deplacement;
layout(location = 2) in float life;
layout(location = 3) in float velocity;

// Values that stay constant for the whole mesh.
uniform int currentMesh;
uniform vec3 wind;
uniform float gravity;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 Position;
out float Life;
out float Velocity;


void main() {

	vec3 p = position;
	vec3 d = deplacement;
	float l = life;
	float v = velocity;

	if(currentMesh==0){

		l--;
        if(l>0.){
        	p+=d;
        	p+=wind;
        	p+=vec3(0,v,0);
        	v+=gravity;
        	Position=p;
        	Life=l;
        	Velocity=v;

        }else{
        	Life=l;
                
            
        }

	}

    // Output position of the vertex, in clip space : MVP * position
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position,1.);
}

