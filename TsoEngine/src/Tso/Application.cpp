#include "TPch.h"
#include "Application.h"
#include "GLFW/glfw3.h"
#include "Input.h"
#include "glm/glm.hpp"
#include "Tso/Renderer/Renderer.h"

//temp
// 
//#include "Event/ApplicationEvent.h"
namespace Tso {
#define BIND_EVENT_FN(x) std::bind(&Application::x,this , std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

	Application::Application() 
        :m_Camera(-1.6f , 1.6f , -0.9f , 0.9f)
    {
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create({"TsoEngine",1280,720}));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        RenderCommand::Init();
        
        m_ImGuiLayer = new ImGuiLayer;
        m_LayerStack.PushOverlay(m_ImGuiLayer);

        
	}

	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
        
        EventDispatcher dispatcher(e);
        
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
        
        for (auto it = m_LayerStack.end() - 1; it != m_LayerStack.begin(); it--) {


            (*it)->OnEvent(e);

            if(e.m_Handled){
                break;
            }
        }
        
		//TSO_CORE_INFO("{0}", e.ToString());
        //printf("[%s]\n",e.ToString().c_str());
	}


    void Application::PushLayer(Layer *layer){
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer *overlay){
        m_LayerStack.PushOverlay(overlay);
    }
    

	void Application::Run() {
        

		while (m_Running) {

            float time = (float)glfwGetTime();

            TimeStep ts = time - m_LastTime;

            m_LastTime = time;

 
            for(auto layer : m_LayerStack){
                layer->OnUpdate(ts);
            }

            m_ImGuiLayer->Begin();

            for (auto layer : m_LayerStack) {
                layer->OnImGuiRender();
            }

            m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

bool Application::OnWindowClosed(const WindowCloseEvent &e){
    m_Running = false;
    return true;
}
}
