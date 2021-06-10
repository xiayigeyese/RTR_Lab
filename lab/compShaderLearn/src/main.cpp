#include <opengl/opengl.h>
#include <app/app.h>

using namespace std;
using namespace glm;

struct QuadVertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
};

void initQuad(VertexArray& vao, VertexBuffer<QuadVertex>& vbo)
{
	array<QuadVertex, 4> quadVertices =
	{
		QuadVertex{ {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
		QuadVertex{ { -1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
		QuadVertex{ { 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
		QuadVertex{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
	};
	vbo.setData(quadVertices.data(), quadVertices.size());
	VertexAttrib positionAttrib = { 0, 3, GL_FLOAT, offsetof(QuadVertex, position) };
	VertexAttrib texCoordsAttrib = { 1,2,GL_FLOAT, offsetof(QuadVertex, texCoords) };
	unsigned int bindingIndex = 0;
	vao.bindVertexBuffer(bindingIndex, vbo, 0);
	vao.bindVertexArrayAttrib(bindingIndex, positionAttrib, GL_FALSE);
	vao.bindVertexArrayAttrib(bindingIndex, texCoordsAttrib, GL_FALSE);
}


int main()
{
	// init app
	const int width = 512, height = 512;
	Application app("csLearn", width, height);
	GLFWwindow* window = app.getWindow();
	Input* input = app.getInput();

	// init camera
	Camera camera = Camera::perspectiveCamera(
		vec3(0, 0, 1),
		vec3(0, 0, 0),
		vec3(0, 1, 0),
		45.0f,
		static_cast<float>(width) / height,
		0.1f,
		100.0f
	);
	CameraController cameraController(&camera, input);

	// quad
	VertexArray quadVAO;
	VertexBuffer<QuadVertex> quadVBO;
	initQuad(quadVAO, quadVBO);

	// load shader
	ShaderProgram quadShader({
		VertexShader::loadFromFile("lab/compShaderLearn/shaders/quad.vert").getHandler(),
		FragmentShader::loadFromFile("lab/compShaderLearn/shaders/quad.frag").getHandler()
	});
	ShaderProgram rayTraceShader({
		ComputerShader::loadFromFile("lab/compShaderLearn/shaders/image.comp").getHandler()
	});
	
	// computer shader image
	Texture2D texImage;
	const int texWidth = 512, texHeight = 512;
	texImage.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	texImage.setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	texImage.setTexFormat(1, GL_RGBA32F, texWidth, texHeight);
	glBindImageTexture(0, texImage.getHandler(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// shader uniform setting
	quadShader.setUniformValue("imgTex", 0);
	
	// render
	while(!glfwWindowShouldClose(window))
	{
		app.getKeyPressInput();
		cameraController.processKeyPressInput();

		rayTraceShader.use();
		glDispatchCompute(texWidth, texHeight, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quadShader.use();
		glActiveTexture(0);
		texImage.bindTexUnit(0);
		quadVAO.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		quadVAO.unbind();
		quadShader.unUse();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	
	return 0;
}