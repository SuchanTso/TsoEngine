#pragma once
#include "Core.h"
#include "Window.h"
#include "Event/ApplicationEvent.h"
#include "LayerStack.h"
#include "Tso/imgui/ImguiLayer.h"
#include "Tso/Renderer/Shader.h"
#include "Tso/Renderer/Buffer.h"
#include "Tso/Renderer/VertexArray.h"

#include "Tso/Renderer/OrthographicCamera.h"
#include "Tso/Core/TimeStep.h"

namespace Tso {
	class TSO_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
        
        bool OnWindowClosed(const WindowCloseEvent& e);
        bool OnKeyEvent(const KeyEvent& e);
        
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* layer);
        
        inline static Application& Get(){return *s_Instance;}
        
        inline Window& GetWindow(){return *m_Window;}

	private :
		std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
        LayerStack m_LayerStack;
        float m_LastTime;
        
    private:
        static Application * s_Instance;

        Ref<Shader> m_Shader;
        Ref<VertexArray> m_VertexArray;

//        OrthographicCamera m_Camera;

        
	};

	//todo:define it in client
	Application* CreateApplication();
}
