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

class TSO_API MouseScrolledEvent :public Event {
public:
    MouseScrolledEvent(const float XOffset, const float YOffset)
        :m_XOffset(XOffset), m_YOffset(YOffset) {}

    float GetXOffset() { return m_XOffset; }
    float GetYOffset() { return m_YOffset; }



    std::string ToString()const override {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << m_XOffset << " , " << m_YOffset;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float m_XOffset;
    float m_YOffset;
};

class TSO_API MouseButtonEvent :public Event{
public:
    MouseButtonEvent(const int Button)
    :m_Button(Button){}
    
    int GetButton(){return m_Button;}
    
    
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
    
protected:
    int m_Button;
};

class TSO_API MouseButtonPressedEvent :public MouseButtonEvent {
public:
    MouseButtonPressedEvent(int Button)
    :MouseButtonEvent(Button){}
    
    std::string ToString()const override {
        std::stringstream ss;
        ss << "MousePressedEvent: " << m_Button;
        return ss.str();
    }
    
    
    EVENT_CLASS_TYPE(MouseButtonPressed)
    
};

class TSO_API MouseButtonReleasedEvent :public MouseButtonEvent {
public:
    MouseButtonReleasedEvent(int Button)
    :MouseButtonEvent(Button){}
    
    std::string ToString()const override {
        std::stringstream ss;
        ss << "MouseReleasedEvent: " << m_Button;
        return ss.str();
    }
    
    
    EVENT_CLASS_TYPE(MouseButtonReleased)
    
};

}

