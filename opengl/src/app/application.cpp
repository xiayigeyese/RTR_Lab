#include <iostream>

#include <app/application.h>

Application::Application(const std::string& title, const int width, const int height)
	: m_window(nullptr), m_input(nullptr)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	m_window = window;
	glfwMakeContextCurrent(window);

	// after make glfw context
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLAD");
	}
	
	glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
	m_input = new Input();
	setEventsCallback();
}

void Application::setEventsCallback() const
{
	// lambda can implicit convert to function pointer with null capture

	auto framebufferSizeCallback = [](GLFWwindow* win, int width, int height)->void
	{
		glViewport(0, 0, width, height);
	};
	glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

	auto keyCallback = [](GLFWwindow* win, int key, int scancode, int action, int mods)->void
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(win, true);
		}
	};
	glfwSetKeyCallback(m_window, keyCallback);

	auto scrollCallback = [](GLFWwindow* win, double xOffset, double yOffset) -> void
	{
		auto& app = *(static_cast<Application*>(glfwGetWindowUserPointer(win)));
		app.m_input->updateZoomOffset(yOffset);
	};
	glfwSetScrollCallback(m_window, scrollCallback);

	auto cursorPosCallback = [](GLFWwindow* win, double xPos, double yPos) -> void
	{
		auto& app = *(static_cast<Application*>(glfwGetWindowUserPointer(win)));
		app.m_input->updateCursorPos(xPos, yPos);
	};
	glfwSetCursorPosCallback(m_window, cursorPosCallback);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Application::getKeyPressInput() const
{
	m_input->setCurrentFrameTime(static_cast<float>(glfwGetTime()));

	m_input->setKeyStatus(Input::KEY_W, Input::KEY_STATUS{ glfwGetKey(m_window, GLFW_KEY_W) });
	m_input->setKeyStatus(Input::KEY_S, Input::KEY_STATUS{ glfwGetKey(m_window, GLFW_KEY_S) });
	m_input->setKeyStatus(Input::KEY_A, Input::KEY_STATUS{ glfwGetKey(m_window, GLFW_KEY_A) });
	m_input->setKeyStatus(Input::KEY_D, Input::KEY_STATUS{ glfwGetKey(m_window, GLFW_KEY_D) });
}

