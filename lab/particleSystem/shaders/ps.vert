#version 430 core
layout (location = 0) in vec4 aPos;

out float intensity;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(aPos.xyz, 1.0f);
	intensity = aPos.w;
}