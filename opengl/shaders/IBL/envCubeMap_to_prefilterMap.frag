#version 330 core
out vec4 fragColor;

in vec3 fragPos;

uniform float u_envCubeMapWidth;

uniform samplerCube u_envCubeMap;
uniform float u_roughness;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint num);
vec3 ImportanceSampleGGX(vec2 randomNumber, vec3 N, float roughness);

float D_GGX_TR(float NdotH, float alpha);

const float PI = 3.14159265359f;

void main()
{
	vec3 N = normalize(fragPos);
	vec3 R = N;
	vec3 V = R;

	const uint sampleCount = 1024u;
	float totalWeight = 0.0;
	vec3 prefilterColor = vec3(0.0);
	// float envCubeMapWidth = 512.0f;
	float omegaS = 2 *  4 * PI / (6 * u_envCubeMapWidth * u_envCubeMapWidth);
	for(uint i=0u; i < sampleCount; i++)
	{
		vec2 randomNumber = Hammersley(i, sampleCount);
		vec3 H = ImportanceSampleGGX(randomNumber, N, u_roughness);
		vec3 L = normalize(2 * dot(H, V) * H - V);

		float NdotL = dot(N, L);
		
		if (NdotL > 0.0)
		{
		    // prefilter envCubeMap with mipmap to reduce aliasing
			float NdotH = max(dot(N, H), 0);
			float HdotV = max(dot(H, V), 0);
			float D = D_GGX_TR(NdotH, u_roughness);
			float pdf = D * NdotH / (4 * HdotV + 0.0001);
			float omegaP = 1.0f / (float(sampleCount) * pdf + 0.0001); 
			float mipLevel = max(0.5 * log2(omegaP / omegaP), 0.0f);
			mipLevel = (u_roughness == 0.0) ? 0.0 : mipLevel;
			prefilterColor += textureLod(u_envCubeMap, L, mipLevel).rgb * NdotL;
			
			// sample envCube directly
			// prefilterColor += texture(u_envCubeMap, L).rgb * NdotL;
			
			totalWeight += NdotL;
		}
	}
    prefilterColor /= totalWeight;
	fragColor = vec4(prefilterColor, 1.0f);
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint num)
{
	return vec2(float(i) / float(num), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 randomNumber, vec3 N, float roughness)
{
	float alpha = roughness * roughness;

	float phi = 2 * PI * randomNumber.x;
	float cosTheta = sqrt((1 - randomNumber.y) / (randomNumber.y * (alpha * alpha - 1) + 1));
	float sinTheta = sqrt(1 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates
	float x = sinTheta * cos(phi);
	float y = sinTheta * sin(phi);
	float z = cosTheta;

	// from tangent-space vector to world-space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 T = normalize(cross(up, N));
	vec3 B = normalize(cross(N, T));
	vec3 H = normalize(T * x + B * y + N * z);

	return H;
}


float D_GGX_TR(float NdotH, float alpha)
{
	float a = alpha * alpha;
	float b = NdotH;
	float c = b * b * (a - 1) + 1;
	return a / (PI * c * c);
}