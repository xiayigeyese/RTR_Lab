#version 330 core
out vec2 fragColor;

in vec2 fragTexCoords;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint num);
vec3 ImportanceSampleGGX(vec2 randomNumber, vec3 N, float roughness);

float G_Schlick_GGX(float NdotV, float k);
float G_Smith_IBL(float NdotL, float NdotV, float roughness);
vec2 integrateBRDF(float NdotV, float roughness);

const float PI = 3.14159265359;

void main()
{
	vec2 brdf = integrateBRDF(fragTexCoords.x, fragTexCoords.y);
	fragColor = brdf;
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

float G_Schlick_GGX(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1 - k) + k);
}

float G_Smith_IBL(float NdotL, float NdotV, float roughness)
{
	float k = roughness * roughness / 2.0;
	return G_Schlick_GGX(NdotL, k) * G_Schlick_GGX(NdotV, k);
}

vec2 integrateBRDF(float NdotV, float roughness)
{
	vec3 N = vec3(0, 0, 1.0);
	vec3 V = vec3(0);
	V.x = sqrt(1.0 - NdotV * NdotV);
	V.y = 0;
	V.z = NdotV;

	const uint sampleCount = 1024u;
	float A = 0, B = 0;
	for(uint i=0u;i<sampleCount;i++)
	{
		vec2 randomNumber = Hammersley(i, sampleCount);
		vec3 H = ImportanceSampleGGX(randomNumber, N, roughness);
		vec3 L = normalize(2.0 * dot(H, V) * H - V);

		float NdotL = max(L.z, 0);
		float NdotH = max(H.z, 0);
		float VdotH = max(dot(V, H), 0);

		if (NdotL > 0.0)
		{	
			float G = G_Smith_IBL(NdotL, NdotV, roughness);
			float G_vis = G * VdotH / (NdotH * NdotV);

			float Fc = pow( 1 - VdotH, 5.0);
			A += (1 - Fc) * G_vis;
			B += Fc * G_vis;
		}
	}
	A /= sampleCount;
	B /= sampleCount;

	return vec2(A, B);
}