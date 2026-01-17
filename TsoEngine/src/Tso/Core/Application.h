#pragma once
#include "Core.h"
#include "Window.h"
#include "Tso/Event/ApplicationEvent.h"
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
		Application(const std::string& windowName = "TsoEngine");
		virtual ~Application();

		void Run();
        bool IsShowClosePrompt() const { return m_ShowClosePrompt; }
        void SetShowClosePrompt(bool show) { m_ShowClosePrompt = show; }
        void ForceClose();

		void OnEvent(Event& e);

        bool OnWindowResizeEvent(WindowResizeEvent& e);

        bool OnWindowClosed(const WindowCloseEvent& e);
        bool OnKeyEvent(const KeyEvent& e);
        
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        
        ImGuiLayer* GetGUILayer(){return m_ImGuiLayer;}
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* layer);
        void OnClose(){m_Running = false;}
        
        inline static Application& Get(){return *s_Instance;}
        
        inline Window& GetWindow(){return *m_Window;}

	private :
		std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
        LayerStack m_LayerStack;
        float m_LastTime;
        bool m_Minimized = false;
        
    private:
        static Application * s_Instance;

        Ref<Shader> m_Shader;
        Ref<VertexArray> m_VertexArray;
        bool m_ShowClosePrompt = false; // 是否显示保存提示
        bool m_BlockClose = false;      // 是否拦截了关闭事件

//        OrthographicCamera m_Camera;

        
	};

	//todo:define it in client
	Application* CreateApplication();
}
