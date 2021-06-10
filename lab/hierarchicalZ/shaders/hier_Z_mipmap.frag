#version 330 core
out float fragColor;

in vec2 fragTexCoords;

uniform sampler2D u_depthTex;
// uniform sampler2D u_lastMipSize;
uniform int u_mipLevel;

void main()
{
    // vec3 depthColor = vec3(texture(u_depthTex, fragTexCoords).r);
	// gl_FragDepth = texture(u_depthTex, fragTexCoords).r;
	// fragColor = vec4(depthColor, 1.0f);
	fragColor = textureLod(u_depthTex, fragTexCoords, u_mipLevel).r;
}