#include "Application.h"
#include <stdio.h>
#include "Event/ApplicationEvent.h"
namespace Tso {
	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {
        WindowResizeEvent e(1280,720);
        
        printf("e :[%s]\n",e.ToString().c_str());
		while (true) {
			//printf("welcome end less loop in suchantso engine\n");
		}
	}
}
