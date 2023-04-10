#pragma once
#include "Core.h"
#include "Window.h"
namespace Tso {
	class TSO_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private :
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	//todo:define it in client
	Application* CreateApplication();
}
