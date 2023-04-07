#pragma once
#include "Core.h"
namespace Tso {
	class TSO_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//todo:define it in client
	Application* CreateApplication();
}
