//
//  ImguiLayer.hpp
//  TsoEngine
//
//  Created by user on 2023/4/12.
//

#pragma once
#include "Tso/Core/Layer.h"
#include "Tso/Event/ApplicationEvent.h"
#include "Tso/Event/MouseEvent.h"
#include "Tso/Event/KeyEvent.h"

namespace Tso{

class TSO_API ImGuiLayer : public Layer{
public:
    ImGuiLayer();
    ~ImGuiLayer();
    
    
    virtual void OnAttach()override;
    
    virtual void OnDetach()override;
    virtual void OnImGuiRender()override;
    
    virtual void OnEvent(Event& e)override;
    
    void BlockEvents(const bool& block){ m_BlockEvent = block;};

    void Begin();
    void End();
    
private:
    float m_Time;
    bool m_BlockEvent = false;
};

}
