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

int main()
{
	// init app
	const int width = 1024, height = 800;
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
		100.0f
	);
	CameraController cameraController(&camera, input);

	// constant var
	const int particleGroupSize = 1024;
	const int particleGroupCount = 1024 * 8;
	const int particleCount = particleGroupSize * particleGroupCount;
	// const int particleCount = 10000000;
	const int maxAttractors = 64;
	
	// particle buffer
	Buffer particlePositionBuffer, particleVelocityBuffer;
	particlePositionBuffer.setData(nullptr, particleCount * sizeof(glm::vec4), GL_DYNAMIC_COPY);
	particleVelocityBuffer.setData(nullptr, particleCount * sizeof(glm::vec4), GL_DYNAMIC_COPY);
	vec4* positions = static_cast<vec4*>(glMapNamedBufferRange(particlePositionBuffer.getHandle(), 0, particleCount * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for(int i=0;i<particleCount;i++)
	{
		positions[i] = vec4(randVec3(-20.0f, 20.0f), randf());
	}
	glUnmapNamedBuffer(particlePositionBuffer.getHandle());

	vec4* velocities = static_cast<vec4*>(glMapNamedBufferRange(particleVelocityBuffer.getHandle(), 0, particleCount * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (int i = 0; i < particleCount; i++)
	{
		velocities[i] = vec4(randVec3(-0.1f, 0.1f), 0);
	}
	glUnmapNamedBuffer(particleVelocityBuffer.getHandle());
	// init position and velocity
	/*vector<vec4> particlePositions(particleCount);
	vector<vec4> particleVelocities(particleCount);
	for(int i=0;i<particleCount;i++)
	{
		vec3 pos = randVec3(-1000.0f, 1000.0f);
		particlePositions[i] = vec4(pos.x, pos.y, pos.z, randf());
		vec3 vel = randVec3(-0.1f, 0.1f);
		particleVelocities[i] = vec4(vel.x, vel.y, vel.z, 0);
	}
	particlePositionBuffer.setData(particlePositions.data(), particleCount * sizeof(glm::vec4), GL_DYNAMIC_COPY);
	particleVelocityBuffer.setData(particleVelocities.data(), particleCount * sizeof(glm::vec4), GL_DYNAMIC_COPY);*/
	

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
	attractorBuffer.setData(attractorMass.data(), attractorMass.size() * sizeof(float), GL_STATIC_DRAW);
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
	
	// render
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	// glPointSize(2.0f);
	while(!glfwWindowShouldClose(window))
	{
		app.getKeyPressInput();
		cameraController.processKeyPressInput();

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