#pragma once

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>           
#include <glm/exponential.hpp>        
#include <glm/geometric.hpp>

class Camera
{
public:
	explicit Camera(glm::vec3 position, glm::vec3 center, glm::vec3 worldUp,
		float zoom, float aspect, float nearPlane, float farPlane)
		:m_position(position),
		 m_front(glm::vec3(0, 0, -1)),
		 m_up(glm::vec3(1.0f)),
		 m_right(glm::vec3(1)),
		 m_worldUp(worldUp),
	     m_zoom(zoom),
	     m_aspect(aspect),
	     m_near(nearPlane),
	     m_far(farPlane)
	{
		// view 
		m_front = normalize(center - position);
		m_worldUp = normalize(worldUp);
		
		/*if (1 - abs(dot(m_front, m_worldUp)) < 0.01f)
		{
			m_worldUp.x += 1;
		}*/
		
		m_right = normalize(cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_front));

		glm::vec3 front = glm::normalize(m_front);
		m_pitch = glm::degrees(glm::asin(front.y));
		m_yaw = glm::degrees(glm::asin(front.z / (glm::sqrt(1 - front.y * front.y))));
	}

	static Camera perspectiveCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp, 
		float zoom, float aspect, float nearPlane, float farPlane)
	{
		return Camera(position, target, worldUp, zoom, aspect, nearPlane, farPlane);
	}

	void moveFront(const float distance)
	{
		m_position += distance * m_front;
	}

	void moveBackward(const float distance)
	{
		m_position -= distance * m_front;
	}

	void moveLeft(const float distance)
	{
		m_position -= distance * m_right;
	}

	void moveRight(const float distance)
	{
		m_position += distance * m_right;
	}

	void moveUp(const float distance)
	{
		m_position += distance * m_up;
	}

	void moveDown(const float distance)
	{
		m_position -= distance * m_up;
	}

	void turnHorizontal(const float distance)
	{
		m_yaw += distance;
	}

	void turnVertical(const float distance)
	{
		m_pitch += distance;
		m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
	}

	void updateZoom(const float zoomOffset)
	{
		m_zoom -= zoomOffset;
		m_zoom = glm::clamp(m_zoom, 1.0f, 60.0f);
	}
	
	[[nodiscard]] float getZoom() const 
	{
		return m_zoom;
	}

	[[nodiscard]] glm::vec3 getPosition() const
	{
		return m_position;
	}

	[[nodiscard]] glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(m_position, m_position + m_front,  m_up);
	}

	[[nodiscard]] glm::mat4 getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(m_zoom), m_aspect, m_near, m_far);
	}
	
	void updateViewAttribute()
	{
		glm::vec3 front;
		front.x = glm::cos(glm::radians(m_pitch)) * glm::cos(glm::radians(m_yaw));
		front.y = glm::sin(glm::radians(m_pitch));
		front.z = glm::cos(glm::radians(m_pitch)) * glm::sin(glm::radians(m_yaw));
		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}

private:
	// view attribute
	glm::vec3 m_position, m_front, m_up, m_right, m_worldUp;
	// angle
	float m_yaw, m_pitch;
	// perspective attribute
	float m_zoom, m_aspect, m_near, m_far;
};

