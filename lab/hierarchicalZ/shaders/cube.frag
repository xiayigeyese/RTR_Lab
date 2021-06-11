#version 330 core
out vec4 fragColor;

in vec2 fragTexCoords;

uniform  sampler2D u_texMap;

void main()
{
	fragColor = vec4(texture(u_texMap, fragTexCoords).rgb, 1.0f);
}