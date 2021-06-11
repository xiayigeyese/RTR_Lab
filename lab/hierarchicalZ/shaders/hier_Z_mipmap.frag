#version 330 core
in vec2 fragTexCoords;

uniform sampler2D u_depthTex;

void main()
{
	float depth = texture(u_depthTex, fragTexCoords).r;
	gl_FragDepth = depth;
}