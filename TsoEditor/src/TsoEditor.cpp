#include "TSO.h"
//entry point --------------------------------
#include "Tso/Core/EntryPoint.h"
//--------------------------------------------
#include "EditorLayer.h"



namespace Tso {
	class TsoEditor :public Application {
	public:
		TsoEditor()
			:Application("Tso Editor")
		{
			PushLayer(new EditorLayer());
		}
		~TsoEditor() {

		}


	};

	Application* CreateApplication() {
		return new TsoEditor();
	}
}

