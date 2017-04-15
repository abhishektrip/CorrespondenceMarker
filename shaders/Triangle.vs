#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
out vec2 UV;
out vec3 colorValue;
void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	colorValue = vertexPosition_modelspace;
	UV = vertexUV;
	//gl_Position.xyz = vertexPosition_modelspace;
    //gl_Position.w = 1.0;

}