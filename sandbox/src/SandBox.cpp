#include "TSO.h"
//entry point --------------------------------
#include "Tso/Core/EntryPoint.h"
//--------------------------------------------
#include "SandBox2D.h"
namespace Tso{
class SandBox :public Tso::Application {
public:
    SandBox()
    
    {
        PushLayer(new RuntimeLayer());
    }
    ~SandBox() {
        
    }
    
    
};

Tso::Application* Tso::CreateApplication() {
    return new SandBox();
}
}

