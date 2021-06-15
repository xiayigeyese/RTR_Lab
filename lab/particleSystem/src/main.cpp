#include <iostream>
#include <vector>
#include <opengl/opengl.h>
#include <app/app.h>

using namespace std;
using namespace glm;

float randf()
{
	float res;
	unsigned int tmp;
	static unsigned int seed = 0xFFFF0C59;
	seed *= 16807;
	tmp = seed ^ (seed >> 4) ^ (seed << 15);
	*(reinterpret_cast<unsigned int*>(&res)) = (tmp >> 9) | 0x3F800000;
	return (res - 1.0f);
}

vec3 randVec3(float minmag = 0.0f, float maxmag = 1.0f)
{
	vec3 randV = vec3(randf() * 2.0f - 1.0f, randf() * 2.0f - 1.0f, randf() * 2.0f - 1.0f);
	randV = glm::normalize(randV);
	randV *= (randf() * (maxmag - minmag) + minmag);
	return randV;
}

double radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return static_cast<double>(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint num)
{
	return vec2(static_cast<float>(i) / static_cast<float>(num), radicalInverse_VdC(i));
}


int main()
{
	// init app
	const int width = 1536, height = 1024;
	Application app("particle system", width, height);
	GLFWwindow* window = app.getWindow();
	Input* input = app.getInput();

	// set camera
	Camera camera = Camera::perspectiveCamera(
		vec3(0, 0, 3),
		vec3(0, 0, 1),
		vec3(0, 1, 0),
		45.0f,
		static_cast<float>(width) / height,
		0.1f,
		1000.0f
	);
	CameraController cameraController(&camera, input);

	// constant var
	const int particleGroupSize = 1024;
	const int particleGroupCount = 1024 * 8;
	const int particleCount = particleGroupSize * particleGroupCount;
	const int maxAttractors = 1024 * 4;
	const float pi = 3.1415926535f;
	
	// particle buffer
	Buffer particlePositionBuffer, particleVelocityBuffer;
	particlePositionBuffer.setData(nullptr, particleCount * sizeof(glm::vec4), GL_DYNAMIC_COPY);
	particleVelocityBuffer.setData(nullptr, particleCount * sizeof(glm::vec4), GL_DYNAMIC_COPY);
	vec4* positions = static_cast<vec4*>(glMapNamedBufferRange(particlePositionBuffer.getHandle(), 0, particleCount * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for(int i=0;i<particleCount;i++)
	{
		positions[i] = vec4(randVec3(-10.0f, 10.0f), randf());
		
	}
	glUnmapNamedBuffer(particlePositionBuffer.getHandle());

	vec4* velocities = static_cast<vec4*>(glMapNamedBufferRange(particleVelocityBuffer.getHandle(), 0, particleCount * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (int i = 0; i < particleCount; i++)
	{
		// velocities[i] = vec4(randVec3(0.0f, 1.0f), 0);
		vec2 xy = hammersley(i, maxAttractors);
		float theta = xy.x * pi;
		float phi = xy.y * 2 * pi;
		float x = sin(theta) * cos(phi);
		float y = cos(theta);
		float z = sin(theta) * sin(phi);
		velocities[i] = vec4(x, y, z, 0);
	}
	glUnmapNamedBuffer(particleVelocityBuffer.getHandle());
	
	// particle texture buffer
	GLuint particlePositionTBO, particleVelocityTBO;
	glCreateTextures(GL_TEXTURE_BUFFER, 1, &particlePositionTBO);
	glTextureBuffer(particlePositionTBO, GL_RGBA32F, particlePositionBuffer.getHandle());
	glCreateTextures(GL_TEXTURE_BUFFER, 1, &particleVelocityTBO);
	glTextureBuffer(particleVelocityTBO, GL_RGBA32F, particleVelocityBuffer.getHandle());

	// attractor uniform buffer
	Buffer attractorBuffer;
	vector<float> attractorMass(maxAttractors);
	for(int i=0;i<maxAttractors;i++)
	{
		attractorMass[i] = 0.5f + 0.5f * randf();
	}
	attractorBuffer.setData(nullptr, maxAttractors * sizeof(glm::vec4), GL_STATIC_DRAW);
	vec4* attractor = static_cast<vec4*>(glMapNamedBufferRange(attractorBuffer.getHandle(), 0, maxAttractors * sizeof(glm::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	float rangeW = 5.0f;
	
	for(int i=0;i<maxAttractors;i++)
	{
		vec2 xy = hammersley(i, maxAttractors);
		float theta = xy.x * pi;
		float phi = xy.y * 2 * pi;
		float x = rangeW * sin(theta) * cos(phi);
		float y = rangeW * cos(theta);
		float z = rangeW * sin(theta) * sin(phi);
		// xy = (xy * 2.0f - 1.0f) * rangeW;
		// attractor[i] = vec4(xy.x, xy.y, 0, 0.5);
		attractor[i] = vec4(x, y, z, 0.5);
	}
	glUnmapNamedBuffer(attractorBuffer.getHandle());
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, attractorBuffer.getHandle());

	// particle vao
	VertexArray particleVAO;
	unsigned int bindingIndex = 0;
	glVertexArrayVertexBuffer(particleVAO.getHandler(), bindingIndex, particlePositionBuffer.getHandle(), 0, sizeof(glm::vec4));
	VertexAttrib positionAttrib = { 0, 4, GL_FLOAT, 0 };
	particleVAO.bindVertexArrayAttrib(bindingIndex, positionAttrib);

	// load shader
	ShaderProgram particleShader({
		VertexShader::loadFromFile("lab/particleSystem/shaders/ps.vert").getHandler(),
		FragmentShader::loadFromFile("lab/particleSystem/shaders/ps.frag").getHandler()
	});
	UniformVariable<glm::mat4> particleS_vs_mvp = particleShader.getUniformVariable<glm::mat4>("u_mvp");

	ShaderProgram particleMoveShader({
		ComputerShader::loadFromFile("lab/particleSystem/shaders/ps.comp").getHandler()
	});
	glBindImageTexture(0, particlePositionTBO, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, particleVelocityTBO, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	// start time
	double startTime = glfwGetTime();
	
	// render
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glPointSize(5.0f);
	while(!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		app.getKeyPressInput();
		cameraController.processKeyPressInput();

		float deltaTime = static_cast<float>(currentTime - startTime);
		/*vec4* attractor = static_cast<vec4*>(glMapNamedBufferRange(attractorBuffer.getHandle(), 0, maxAttractors * sizeof(glm::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		for(int i=0;i<maxAttractors;i++)
		{
			attractor[i] = vec4(
				sinf(deltaTime * static_cast<float>(i + 4) * 7.5f * 20.0f) * 50.0f,
				cosf(deltaTime * static_cast<float>(i + 7) * 3.9f * 20.0f) * 50.f,
				sinf(deltaTime * static_cast<float>(i+3) * 5.3f * 20.0f) * cosf(deltaTime * static_cast<float>(i+5) * 9.1f) * 100.0f,
				attractorMass[i]
			);
		}
		glUnmapNamedBuffer(attractorBuffer.getHandle());*/
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		particleMoveShader.use();
		glDispatchCompute(particleGroupCount, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		particleMoveShader.unUse();
	
		particleShader.use();
		particleShader.setUniformValue(particleS_vs_mvp, camera.getProjectionMatrix() * camera.getViewMatrix());
		particleVAO.bind();
		
		glDrawArrays(GL_POINTS, 0, particleCount);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}