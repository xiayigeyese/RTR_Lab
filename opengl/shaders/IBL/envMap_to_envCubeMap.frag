#version 330 core
out vec4 fragColor;

in vec3 fragPos;

uniform sampler2D u_envMap;

const float PI = 3.14159265359;

vec2 getSphereUV(vec3 pos)
{
	float theta = asin(pos.y);
	float phi = atan(pos.z, pos.x);
	// u v : [0, 1]
	float v = theta / PI + 0.5;
	float u = phi / (2 * PI) + 0.5;
	return vec2(u, v);
}

void main()
{
	// xyz convert uv
	vec2 texCoords = getSphereUV(normalize(fragPos)); 
	vec3 color = texture(u_envMap, texCoords).rgb;
	fragColor = vec4(color, 1.0f);
}