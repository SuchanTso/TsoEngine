#pragma once
#include "Event.h"

namespace Tso {

	class TSO_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x),m_MouseY(y) {}

		float GetX()const { return m_MouseX; }
		float GetY()const { return m_MouseY; }

		std::string ToString()const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << " , " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)



	private:
		float m_MouseX;
		float m_MouseY;
	};

}

