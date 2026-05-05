#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;


out vec3 color;
out vec2 texCoord;



uniform mat4 camMatrix;
uniform mat4 model;
uniform vec2 texOffset;
uniform vec2 texScale;

void main()
{
	gl_Position = camMatrix * model * vec4(aPos, 1.0);
	texCoord = aTex * texScale + texOffset;
}