#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in mat4 aInstanceMatrix;
layout (location = 6) in vec2 aInstanceTexOffset;

out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform vec2 texScale;

void main()
{
	gl_Position = camMatrix * aInstanceMatrix * vec4(aPos, 1.0);
	texCoord = aTex * texScale + aInstanceTexOffset;
}
