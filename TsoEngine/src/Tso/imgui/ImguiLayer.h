//
//  ImguiLayer.hpp
//  TsoEngine
//
//  Created by user on 2023/4/12.
//

#pragma once
#include "Tso/Layer.h"
#include "Tso/Event/ApplicationEvent.h"
#include "Tso/Event/MouseEvent.h"
#include "Tso/Event/KeyEvent.h"

namespace Tso{

class TSO_API ImGuiLayer : public Layer{
public:
    ImGuiLayer();
    ~ImGuiLayer();
    
    void OnUpdate()override;
    
    void OnEvent(Event& e)override;
    
    void OnAttach()override;
    
    void OnDetach()override;
    
private:
    bool OnMouseButtonPressedEvent(MouseButtonPressedEvent & e);
    bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent & e);
    
    bool OnMouseMovedEvent(MouseMovedEvent& e);
    bool OnMouseScrolledEvent(MouseScrolledEvent& e);
    
    bool OnKeyPressedEvent(KeyPressedEvent& e);
    bool OnKeyReleasedEvent(KeyReleasedEvent& e);
    bool OnKeyTypedEvent(KeyTypedEvent& e);
    
    bool OnWindowResizedEvent(WindowResizeEvent& e);

    
private:
    float m_Time;
};

}
