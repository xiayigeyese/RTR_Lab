#pragma once

#include <functional>
#include <map>
#include <array>

class Input
{
public:
	// key
	static constexpr int KEY_W = 0;
	static constexpr int KEY_S = 1;
	static constexpr int KEY_A = 2;
	static constexpr int KEY_D = 3;
	// follow glfw macro
	enum class KEY_STATUS
	{
		RELEASE, PRESS, REPEAT
	};
	
	Input() :m_firstMouse(true),
		m_lastPosX(0.0), m_lastPosY(0.0), m_xOffset(0.0), m_yOffset(0.0),
		m_zoomOffset(0.0),
		m_lastFrameTime(0.0),
	    m_deltaFrameTime(0.0),
	    m_keyStatus({ KEY_STATUS::RELEASE })
	{
		m_mouseMoveHandlers = {};
		m_wheelScrollHandlers = {};
		m_mouseMoveKeyId = 1;
		m_wheelScrollKeyId = 1;
	}

	// mouse Movement

	void updateCursorPos(const double posX, const double posY)
	{
		if (m_firstMouse)
		{
			m_lastPosX = posX;
			m_lastPosY = posY;
			m_firstMouse = false;
		}
		m_xOffset = posX - m_lastPosX;
		m_yOffset = posY - m_lastPosY;
		m_lastPosX = posX;
		m_lastPosY = posY;

		for (auto& f : m_mouseMoveHandlers)
		{
			f.second(m_xOffset, m_yOffset);
		}
	}

	unsigned int addMouseMoveEventHandler(std::function<void(double, double)> func)
	{
		m_mouseMoveHandlers[m_mouseMoveKeyId++] = func;
		return m_mouseMoveKeyId - 1;
	}

	void updateMouseMoveEventHandler(const unsigned int keyId, std::function<void(double, double)> func)
	{
		m_mouseMoveHandlers[keyId] = func;
	}

	void deleteMouseMoveEventHandler(const unsigned int keyId)
	{
		m_mouseMoveHandlers.erase(keyId);
	}

	// mouse wheel scroll

	void updateZoomOffset(const double zoomOffset)
	{
		m_zoomOffset = zoomOffset;

		for (auto& f : m_wheelScrollHandlers)
		{
			f.second(m_zoomOffset);
		}
	}

	unsigned int addWheelScrollEventHandler(std::function<void(double)> func)
	{
		m_wheelScrollHandlers[m_wheelScrollKeyId++] = func;
		return m_wheelScrollKeyId - 1;
	}

	void updateWheelScrollEventHandler(const unsigned int keyId, std::function<void(double)> func)
	{
		m_wheelScrollHandlers[keyId] = func;
	}

	void deleteWheelScrollEventHandler(const unsigned int keyId)
	{
		m_wheelScrollHandlers.erase(keyId);
	}

	// key pressed
	
	void setCurrentFrameTime(const double currentFrameTime)
	{
		m_deltaFrameTime = currentFrameTime - m_lastFrameTime;
		m_lastFrameTime = currentFrameTime;
	}

	[[nodiscard]] double getDeltaFrameTime() const
	{
		return m_deltaFrameTime;
	}

	void setKeyStatus(int key, const KEY_STATUS keyStatus)
	{
		m_keyStatus[key] = keyStatus;
	}

	KEY_STATUS getKeyStatus(int key)
	{
		return m_keyStatus[key];
	}

private:
	bool m_firstMouse;
	double m_lastPosX, m_lastPosY, m_xOffset, m_yOffset;
	double m_zoomOffset;
	double m_lastFrameTime, m_deltaFrameTime;

	// use map for de_attach handler, vector is worse
	int m_mouseMoveKeyId, m_wheelScrollKeyId;
	std::map<unsigned int, std::function<void(double, double)>> m_mouseMoveHandlers;
	std::map<unsigned int, std::function<void(double)>> m_wheelScrollHandlers;

	// key status manage
	std::array<KEY_STATUS, 4> m_keyStatus;
};