#version 430 core
out vec4 fragColor;

in vec2 fragTexCoords;

uniform sampler2D imgTex;

void main()
{
	fragColor = vec4(texture(imgTex, fragTexCoords).rgb, 1.0f);
}