#pragma once

#include <iostream>

#include <glad/glad.h>

#include "shader.h"
#include "uniform.h"

class ShaderProgram
{
public:
	ShaderProgram():m_handler(0) {}

	ShaderProgram(const ShaderProgram&) = delete;

	ShaderProgram(ShaderProgram&& shaderProgram) noexcept
		:m_handler(shaderProgram.getHandler())
	{
		shaderProgram.m_handler = 0;
	}

	ShaderProgram& operator=(const ShaderProgram&) = delete;

	ShaderProgram& operator=(ShaderProgram&& shaderProgram) noexcept
	{
		destroy();
		m_handler = shaderProgram.getHandler();
		shaderProgram.m_handler = 0;
		return *this;
	}

	~ShaderProgram()
	{
		destroy();
	}
	
	void destroy()
	{
		if (m_handler)
		{
			glDeleteProgram(m_handler);
			m_handler = 0;
		}
	}

	/*template<typename ...ShaderType>
	ShaderProgram(const Shader<ShaderType>& ...shaders)
		:m_handler(0)
	{
		std::vector<GLuint> shaderHandlers;
		for(auto shader: shaders)
		{
			shaderHandlers.push_back(shader.getHandler());
		}
		initShaders(shaderHandlers);
	}*/

	ShaderProgram(const std::initializer_list<GLuint> shaderHandlers)
		: m_handler(0)
	{
		initShaders(shaderHandlers);
	}
    
	void initShaders(const std::initializer_list<GLuint> shaderHandlers)
	{
		destroy();
		m_handler = glCreateProgram();
		for (auto shaderHandler : shaderHandlers)
		{
			glAttachShader(m_handler, shaderHandler);
		}
		glLinkProgram(m_handler);

		GLint success;
		GLchar infoLog[1024];
		glGetProgramiv(m_handler, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_handler, 1024, nullptr, infoLog);
			std::cout << "error shader program linking error " << infoLog << std::endl;
		}
		for (auto shaderHandler : shaderHandlers)
		{
			glDetachShader(m_handler, shaderHandler);
		}
	}

	void use() const 
	{
		assert(m_handler);
		glUseProgram(m_handler);
	}

	void unUse() const
	{
		assert(m_handler);
		glUseProgram(0);
	}

	GLuint getHandler() const
	{
		return m_handler;
	}

	template<typename T>
	UniformVariable<T> getUniformVariable(const GLchar* name) const
	{
		assert(m_handler);
		GLint location = glGetUniformLocation(m_handler, name);
		return UniformVariable<T>(location);
	}
	
	template<typename T>
	void setUniformValue(const UniformVariable<T>& uniformVariable, const T& value)
	{
		assert(m_handler);
		// uniformVariable.setVariableValue(value);
		setProgramUniform(m_handler, uniformVariable.getLocation(), value);
	}

	template<typename T>
	void setUniformValue(const std::string& name, const T& value)
	{
		assert(m_handler);
		// setUniform(glGetUniformLocation(m_handler, name.c_str()), value);
		setProgramUniform(m_handler, glGetUniformLocation(m_handler, name.c_str()), value);
	}
	
private:
	GLuint m_handler;
};