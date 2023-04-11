#include "TPch.h"
#include "Application.h"
#include "GLFW/glfw3.h"
//#include "Event/ApplicationEvent.h"
namespace Tso {
#define BIND_EVENT_FN(x) std::bind(&Application::x,this , std::placeholders::_1)

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
        
        EventDispatcher dispatcher(e);
        
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
        
        for(auto it = m_LayerStack.end();it != m_LayerStack.begin();){
            (*--it)->OnEvent(e);
            if(e.m_Handled){
                break;
            }
        }
        
		TSO_CORE_INFO("{0}", e.ToString());
        printf("[%s]\n",e.ToString().c_str());
	}


    void Application::PushLayer(Layer *layer){
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer *overlay){
        m_LayerStack.PushOverlay(overlay);
    }
    

	void Application::Run() {
        
		while (m_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
            
            for(auto layer : m_LayerStack){
                layer->OnUpdate();
            }
			m_Window->OnUpdate();
		}
	}

bool Application::OnWindowClosed(const WindowCloseEvent &e){
    m_Running = false;
    return true;
}
}
