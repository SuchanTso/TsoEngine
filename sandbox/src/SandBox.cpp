#include "TSO.h"
//entry point --------------------------------
#include "Tso/Core/EntryPoint.h"
//--------------------------------------------

#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#ifdef TSO_PLATFORM_MACOSX
#include <unistd.h>
#endif
#include "Tso/Renderer/OrthographicCameraController.h"
#include "SandBox2D.h"
#include "BigWealth/BigWealthGameLayer.h"


class SandBox :public Tso::Application {
public:
	SandBox() 
       
    {
        PushLayer(new SandBox2D());
	}
	~SandBox() {

	}


};

Tso::Application* Tso::CreateApplication() {
	return new SandBox();
}

