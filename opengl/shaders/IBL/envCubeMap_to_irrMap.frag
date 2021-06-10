#version 330 core
out vec4 fragColor;

in vec3 fragPos;

uniform samplerCube u_envCubeMap;

void main()
{
	vec3 N = normalize(fragPos);
	vec3 T = normalize(cross(vec3(0,1,0), N));
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	vec3 irrandiance = vec3(0);

	const float PI = 3.14159265;
	float sampleDelta = 0.025;
	float sampleCount = 0;
	for(float phi =0; phi < 2 * PI; phi += sampleDelta)
	{
		for(float theta =0; theta < PI / 2; theta += sampleDelta)
		{
			float x = sin(theta) * cos(phi);
			float y = sin(theta) * sin(phi);
			float z = cos(theta);
			vec3 sampleDir = normalize(TBN * vec3(x,y,z));
			// vec3 sampleDir = T * x + B * y + N * z;
			// sampleDir = normalize(sampleDir);
			vec3 color = texture(u_envCubeMap, sampleDir).rgb * cos(theta) * sin(theta);
			irrandiance += color;
			sampleCount += 1;
		}
	} 
	irrandiance = PI * irrandiance / sampleCount;
	fragColor = vec4(irrandiance, 1.0f);
}