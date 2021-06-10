#include <iostream>
#include <vector>
#include <opengl/opengl.h>
#include <app/app.h>

using namespace std;
using namespace glm;

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
	const int width = 1024, height = 1024;
	Application app("Hierarchi Z", width, height);
	GLFWwindow* window = app.getWindow();
	Input* input = app.getInput();

	// set camera
	Camera camera = Camera::perspectiveCamera(
		vec3(-2, 2, -4),
		vec3(0,0,0),
		vec3(0,1,0),
		45.0f,
		static_cast<float>(width) / height,
		0.1f,
		100.0f
	);
	CameraController cameraController(&camera, input);

	// set geometry
	Cube cube;
	Quad quad;

	// load texture
	Texture2D woodTex, containerTex;
	woodTex.loadFromFile("resources/textures/wood.png", Texture2D::MAX_MIPMAP);
	containerTex.loadFromFile("resources/textures/container2.png", Texture2D::MAX_MIPMAP);

	// depth texture
	Texture2D depthTex;
	int mipLevels = 1 + static_cast<int>(std::floorf(std::log2f(std::fmaxf(width, height))));
	depthTex.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	depthTex.setTexFilterParameter(GL_NEAREST, GL_NEAREST);
	depthTex.setTexFormat(mipLevels, GL_DEPTH_COMPONENT32F, width, height);
	depthTex.genTexMipMap();

	// color texture
	Texture2D hiZImg;
	hiZImg.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	hiZImg.setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	hiZImg.setTexFormat(1, GL_RGBA16F, width, height);
	
	// load shader
	ShaderProgram hiZShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/cube.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/cube.frag").getHandler()
	});
	ShaderProgram hiZMipMapShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.frag").getHandler()
	});
	ShaderProgram showZBufferShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/quad.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/quad.frag").getHandler()
	});
	
	// FBO
	Framebuffer hiZFBO, hiZMipMapFBO;
	Renderbuffer hiZMipMapFBODepth;
	hiZFBO.attachTexture2D(GL_DEPTH_ATTACHMENT, depthTex, 0);
	hiZFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, hiZImg);

	Texture2D depthTex2;
	int mipWidth = width / 2, mipHeight = height / 2;
	depthTex2.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	depthTex2.setTexFilterParameter(GL_NEAREST, GL_NEAREST);
	depthTex2.setTexFormat(1, GL_R16F, mipWidth, mipHeight);
	hiZMipMapFBODepth.allocateStorage(GL_DEPTH_COMPONENT, mipWidth, mipHeight);
	hiZMipMapFBO.attachRenderBuffer(GL_DEPTH_ATTACHMENT, hiZMipMapFBODepth);
	hiZMipMapFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, depthTex2);
	
	// shader set uniform
	UniformVariable<glm::mat4> hiZS_vs_mvp = hiZShader.getUniformVariable<glm::mat4>("u_mvp");
	hiZShader.setUniformValue("u_texMap", 0);
	hiZMipMapShader.setUniformValue("u_depthTex", 0);
	UniformVariable<int> hiZMipMapS_fs_mipLevel = hiZMipMapShader.getUniformVariable<int>("u_mipLevel");
	showZBufferShader.setUniformValue("u_depthTex", 0);
	
    // position
	int cubeCounts = 3000;
	vector<mat4> modelMatrices(cubeCounts);
	float rangeW = 100.f;
	for(int i=0;i< cubeCounts;i++)
	{
		vec2 randomNumber = hammersley(i, cubeCounts);
		randomNumber = randomNumber * rangeW;
		
		glm::mat4 model = mat4(1.0f);
		model = glm::translate(model, glm::vec3(randomNumber.x, 0, randomNumber.y));
		float scale = (static_cast<float>(rand() % 20)) / 100.0f + 0.10f;
		model = glm::scale(model, glm::vec3(scale));
		// float rotAngle = 1.0f * static_cast<float>(rand() % 360);
		// model = glm::rotate(model, rotAngle, glm::vec3(0.4, 0.6, 0.8));
		modelMatrices[i] = model;
	}

	int columnCount = 10;
	vector<mat4> columnMatrices(columnCount);
	for(int i=0;i<columnCount;i++)
	{
		vec2 randomNumber = hammersley(i, columnCount);
		randomNumber = randomNumber * rangeW / 1.5f;
		glm::mat4 model = mat4(1.0f);
		model = glm::translate(model, glm::vec3(randomNumber.x, 1, randomNumber.y));
		model = glm::scale(model, glm::vec3(1, 2, 1));
		columnMatrices[i] = model;
	}

	// render
	glEnable(GL_DEPTH_TEST);
	while(!glfwWindowShouldClose(window))
	{
		app.getKeyPressInput();
		cameraController.processKeyPressInput();
	
		glViewport(0, 0, width, height);
		hiZFBO.bind();
		glClearColor(0.1f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hiZShader.use();
		glActiveTexture(0);
		woodTex.bindTexUnit(0);
		mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
		for(int i=0;i< cubeCounts;i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * modelMatrices[i]);
			cube.draw();
		}
		glActiveTexture(0);
		containerTex.bindTexUnit(0);
		for(int i=0;i<columnCount;i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * columnMatrices[i]);
			cube.draw();
		}	
		hiZShader.unUse();
		hiZFBO.unbind();

		/*glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		showZBufferShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		quad.draw();
		showZBufferShader.unUse();*/

		/*hiZMipMapFBO.bind();
		hiZMipMapShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		int mipWidth = width, mipHeight = height;
		for(int i=1;i < mipLevels;i++)
		{
			mipWidth /= 2;
			mipHeight /= 2;
			mipWidth = mipWidth > 0 ? mipWidth : 1;
			mipHeight = mipHeight > 0 ? mipHeight : 1;
			glViewport(0, 0, mipWidth, mipHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			hiZMipMapFBODepth.allocateStorage(GL_DEPTH_COMPONENT, mipWidth, mipHeight);
			hiZMipMapFBO.attachRenderBuffer(GL_DEPTH_ATTACHMENT, hiZMipMapFBODepth);
			hiZMipMapFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, depthTex, i);
			hiZMipMapShader.setUniformValue(hiZMipMapS_fs_mipLevel, i - 1);
			quad.draw();
		}
		hiZMipMapShader.unUse();
		hiZMipMapFBO.unbind();*/

		hiZMipMapFBO.bind();
		hiZMipMapShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		glViewport(0, 0, mipWidth, mipHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hiZMipMapShader.setUniformValue(hiZMipMapS_fs_mipLevel, 0);
		quad.draw();
		hiZMipMapShader.unUse();
		hiZMipMapFBO.unbind();
		

		// glViewport(0, 0, width / 2, height / 2);
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		showZBufferShader.use();
		glActiveTexture(0);
		depthTex2.bindTexUnit(0);
		depthTex2.setTexParameteri(GL_TEXTURE_BASE_LEVEL, 1);
		depthTex2.setTexParameteri(GL_TEXTURE_MAX_LEVEL, 1);
		quad.draw();
		showZBufferShader.unUse();

		
		/*glDisable(GL_DEPTH_TEST);
		hiZMipMapShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		quad.draw();
		hiZMipMapShader.unUse();*/

		/*hiZMipMapShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		int currentWidth = width, currentHeight = height;
		for(int i=1;i<mipLevels;i++)
		{
			currentWidth /= 2;
			currentHeight /= 2;
			currentWidth = currentWidth > 0 ? currentWidth : 1;
			currentHeight = currentHeight > 0 ? currentHeight : 1;
			glViewport(0, 0, currentWidth, currentHeight);
			depthTex.setTexParameteri(GL_TEXTURE_BASE_LEVEL, i - 1);
			depthTex.setTexParameteri(GL_TEXTURE_MAX_LEVEL, i - 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex.getHandler(), i);
			quad.draw();
		}
		hiZMipMapShader.unUse();*/

		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	
	return 0;
}