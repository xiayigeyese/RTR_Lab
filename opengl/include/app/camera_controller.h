#pragma once

#include "camera.h"
#include "input.h"

class CameraController
{
public:
	enum class MoveDirection
	{
		FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
	};
	
	CameraController()
		:m_camera(nullptr),
		m_input(nullptr),
		m_moveSpeed(2.5),
		m_mouseSensitivity(0.1),
		m_wheelScrollKeyId(0),
		m_mouseMoveKeyId(0)
	{}

	CameraController(const CameraController&) = delete;

	CameraController(CameraController&& controller) noexcept
		:m_camera(controller.m_camera),
	     m_input(controller.m_input),
		 m_moveSpeed(controller.m_moveSpeed),
	     m_mouseSensitivity(controller.m_mouseSensitivity),
		 m_wheelScrollKeyId(controller.m_wheelScrollKeyId),
		 m_mouseMoveKeyId(controller.m_mouseMoveKeyId)
	{
		controller.m_camera = nullptr;
		controller.m_input = nullptr;

		// update the callback in Input
		auto mouseMoveCallback = [this](const double xOffset, const double yOffset)->void {return this->mouseMove(xOffset, yOffset); };
		m_input->updateMouseMoveEventHandler(m_mouseMoveKeyId, mouseMoveCallback);
		auto wheelCallback = [this](const double offset)->void {return this->wheelScroll(offset); };
		m_input->updateWheelScrollEventHandler(m_wheelScrollKeyId, wheelCallback);
	}

	CameraController& operator=(const CameraController&) = delete;

	CameraController& operator=(CameraController&& controller) noexcept
	{
		m_camera = controller.m_camera;
		m_input = controller.m_input;
		controller.m_camera = nullptr;
		controller.m_input = nullptr;
		m_moveSpeed = controller.m_moveSpeed;
		m_mouseSensitivity = controller.m_mouseSensitivity;

		// update the callback in Input
		m_mouseMoveKeyId = controller.m_mouseMoveKeyId;
		auto mouseMoveCallback = [this](const double xOffset, const double yOffset)->void {return this->mouseMove(xOffset, yOffset); };
		m_input->updateMouseMoveEventHandler(m_mouseMoveKeyId, mouseMoveCallback);

		m_wheelScrollKeyId = controller.m_wheelScrollKeyId;
		auto wheelCallback = [this](const double offset)->void {return this->wheelScroll(offset); };
		m_input->updateWheelScrollEventHandler(m_wheelScrollKeyId, wheelCallback);
		
		return *this;
	}

	// use for normal
	explicit CameraController(Camera* camera, Input* input)
		: m_camera(camera),
		  m_input(input),
		  m_moveSpeed(2.0),
		  m_mouseSensitivity(0.1),
		  m_wheelScrollKeyId(0),
		  m_mouseMoveKeyId(0)
	{
		registerWheelScrollCallback();
		registerMouseMoveCallback();
	}

	~CameraController()
	{
		m_camera = nullptr;
		cancelMouseMoveCallback();
		cancelWheelScrollCallback();
		m_input = nullptr;
	}

	void setMoveSpeed(const double moveSpeed)
	{
		m_moveSpeed = moveSpeed;
	}

	void setMouseSensitivity(const double mouseSensitivity)
	{
		m_mouseSensitivity = mouseSensitivity;
	}
	
	void mouseMove(const double xOffset, const double yOffset) const
	{
		float yawDistance = static_cast<float>(xOffset * m_mouseSensitivity);
		float pitchDistance = static_cast<float>(-1.0 * yOffset * m_mouseSensitivity);
		m_camera->turnHorizontal(yawDistance);
		m_camera->turnVertical(pitchDistance);
		m_camera->updateViewAttribute();
	}

	void wheelScroll(const double zoomOffset) const
	{
		m_camera->updateZoom(static_cast<float>(zoomOffset));
	}

	[[nodiscard]] double getMoveSpeed() const
	{
		return m_moveSpeed;
	}

	[[nodiscard]] double getMouseSensitivity() const
	{
		return m_mouseSensitivity;
	}

	// mouse wheel scroll
	
	void registerWheelScrollCallback()
	{
		auto wheelCallback = [this](const double offset)->void {return this->wheelScroll(offset); };
		m_wheelScrollKeyId = m_input->addWheelScrollEventHandler(wheelCallback);
	}

	void cancelWheelScrollCallback()
	{
		if (m_wheelScrollKeyId != 0)
		{
			m_input->deleteWheelScrollEventHandler(m_wheelScrollKeyId);
			m_wheelScrollKeyId = 0;
		}
	}

	// mouse movement
	
	void registerMouseMoveCallback()
	{
		auto mouseMoveCallback = [this](const double xOffset, const double yOffset)->void {return this->mouseMove(xOffset, yOffset); };
		m_mouseMoveKeyId = m_input->addMouseMoveEventHandler(mouseMoveCallback);
	}

	void cancelMouseMoveCallback()
	{
		if (m_mouseMoveKeyId != 0)
		{
			m_input->deleteMouseMoveEventHandler(m_mouseMoveKeyId);
			m_mouseMoveKeyId = 0;
		}
	}

	// key press input

	void processKeyPressInput() const
	{
		const float distance = static_cast<float>(m_moveSpeed * m_input->getDeltaFrameTime());
		// W: Front
		if (m_input->getKeyStatus(Input::KEY_W) == Input::KEY_STATUS::PRESS)
		{
			m_camera->moveFront(distance);
			m_camera->updateViewAttribute();
		}
		// S: Backward
		if (m_input->getKeyStatus(Input::KEY_S) == Input::KEY_STATUS::PRESS)
		{
			m_camera->moveBackward(distance);
			m_camera->updateViewAttribute();
		}
		// A: Left
		if (m_input->getKeyStatus(Input::KEY_A) == Input::KEY_STATUS::PRESS)
		{
			m_camera->moveLeft(distance);
			m_camera->updateViewAttribute();
		}
		// D: Right
		if (m_input->getKeyStatus(Input::KEY_D) == Input::KEY_STATUS::PRESS)
		{
			m_camera->moveRight(distance);
			m_camera->updateViewAttribute();
		}
	}
	
private:
	Camera* m_camera;
	Input* m_input;
	double m_moveSpeed, m_mouseSensitivity;
	unsigned int m_wheelScrollKeyId, m_mouseMoveKeyId;
};