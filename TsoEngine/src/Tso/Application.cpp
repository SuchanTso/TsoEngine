#include "TPch.h"
#include "Application.h"
#include "GLFW/glfw3.h"
#include "Input.h"
#include "glm/glm.hpp"
#include "Tso/Renderer/Renderer.h"
//#include "Event/ApplicationEvent.h"
namespace Tso {
#define BIND_EVENT_FN(x) std::bind(&Application::x,this , std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

	Application::Application() {
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create({"TsoEngine",1280,720}));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
        
        m_ImGuiLayer = new ImGuiLayer;
        m_LayerStack.PushOverlay(m_ImGuiLayer);

        m_VertexArray.reset(VertexArray::Create());


        float vertices[7 * 3] = {
            -0.5f , -0.5f , 0.0f , 0.8f , 0.2f , 0.8f , 1.0f ,
             0.5f , -0.5f , 0.0f , 0.2f , 0.3f , 0.8f , 1.0f ,
             0.0f ,  0.5f , 0.0f , 0.8f , 0.8f , 0.2f , 1.0f 
        };

        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

        

        {
            BufferLayout layout = {
                {ShaderDataType::Float3 , "a_Position"},
                {ShaderDataType::Float4 , "a_Color"}
            };

            vertexBuffer->SetLayout(layout);
        }

        uint32_t indices[3] = { 0 , 1 , 2 };

        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(IndexBuffer::Create(indices, 3));
        //m_IndexBuffer->Bind();

        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        std::string vertexSrc = R"(
            #version 330 core
           
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            out vec3 v_Position;
            out vec4 v_Color;


            void main(){
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = vec4(a_Position , 1.0);
            }
             
        )";


        std::string fragmentSrc = R"(
            #version 330 core
           
            layout(location = 0) out vec4 color;
            in vec3 v_Position;
            in vec4 v_Color;


            void main(){
                //color = vec4(v_Position * 0.5 + 0.5 , 1.0);
                color = v_Color;
            }
             
        )";


        m_Shader.reset(Shader::Create(vertexSrc , fragmentSrc));
	}

	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
        
        EventDispatcher dispatcher(e);
        
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
        
        for(auto it = m_LayerStack.end() - 1;it != m_LayerStack.begin();it--){
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

            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });

            RenderCommand::Clear();

            Renderer::BeginScene();

            m_Shader->Bind();

            Renderer::Submit(m_VertexArray);

            Renderer::EndScene();
            
            for(auto layer : m_LayerStack){
                layer->OnUpdate();
            }

            m_ImGuiLayer->Begin();

            for (auto layer : m_LayerStack) {
                layer->OnImGuiRender();
            }

            m_ImGuiLayer->End();

			m_Window->OnUpdate();

            m_Shader->UnBind();
		}
	}

bool Application::OnWindowClosed(const WindowCloseEvent &e){
    m_Running = false;
    return true;
}
}
