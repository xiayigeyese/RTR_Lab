#version 330 core
out vec4 fragColor;

in vec2 fragTexCoords;

uniform sampler2D u_depthTex;

void main()
{
	 float depth = texture(u_depthTex, fragTexCoords).r;
	 // float depth = textureLod(u_depthTex, fragTexCoords, 5.0).r;
	 fragColor = vec4(vec3(pow(depth, 30)), 1.0f);
	// fragColor = vec4(texture(u_depthTex, fragTexCoords).rgb, 1.0);
}