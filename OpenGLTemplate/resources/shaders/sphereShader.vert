#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;



// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec4 p;
out vec3 n;

void main()
{	

	// Transform the vertex spatial position using the projection and modelview matrices
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0);
	
	// Get the vertex normal and vertex position in eye coordinates
	n = normalize(matrices.normalMatrix * inNormal);
	p = matrices.modelViewMatrix * vec4(inPosition, 1.0f);

	
} 

