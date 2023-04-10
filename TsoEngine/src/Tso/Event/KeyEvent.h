//
//  KeyEvent.h
//  TsoEngine
//
//  Created by user on 2023/4/10.
//

#pragma once
#include "Event.h"
#include <sstream>

namespace Tso {

class TSO_API KeyEvent : public Event{
public:
    
    inline int GetKeycode()const{ return m_Keycode; }
    
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    
protected:
    KeyEvent(const int keycode)
    :m_Keycode(keycode){}
    
    
    
private:
    int m_Keycode;
};

class TSO_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(const int keycode, bool isRepeat = false)
        : KeyEvent(keycode), m_IsRepeat(isRepeat) {}
    
    bool IsRepeat() const { return m_IsRepeat; }
    
    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    bool m_IsRepeat;
};

class TSO_API KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(const int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

class TSO_API KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(const int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };



}
