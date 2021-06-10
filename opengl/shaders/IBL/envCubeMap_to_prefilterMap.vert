#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 fragPos;

uniform mat4 u_mvp;

void main()
{
	fragPos = aPos;
	gl_Position = u_mvp * vec4(aPos, 1.0f);
}