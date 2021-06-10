#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "input.h"

class Application
{
public:
	Application(const std::string& title, const int width, const int height);

	~Application()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void getKeyPressInput() const;

	[[nodiscard]] GLFWwindow* getWindow() const
	{
		return m_window;
	}

	[[nodiscard]] Input* getInput() const
	{
		return m_input;
	}

private:
	void setEventsCallback() const;

private:
	GLFWwindow* m_window;
	Input* m_input;
};