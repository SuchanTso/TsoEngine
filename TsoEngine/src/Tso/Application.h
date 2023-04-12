#pragma once
#include "Core.h"
#include "Window.h"
#include "Event/ApplicationEvent.h"
#include "LayerStack.h"
namespace Tso {
	class TSO_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
        
        bool OnWindowClosed(const WindowCloseEvent& e);
        
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* layer);
        
        inline static Application& Get(){return *s_Instance;}
        
        inline Window& GetWindow(){return *m_Window;}

	private :
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
        LayerStack m_LayerStack;
        
    private:
        static Application * s_Instance;
        
	};

	//todo:define it in client
	Application* CreateApplication();
}
