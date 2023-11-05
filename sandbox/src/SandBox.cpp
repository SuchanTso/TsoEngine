#include "TSO.h"
//entry point --------------------------------
#include "Tso/Core/EntryPoint.h"
//--------------------------------------------

#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#ifdef TSO_PLATFORM_MACOSX
#include <unistd.h>
#endif
#include "Tso/Renderer/OrthographicCameraController.h"
#include "rapidjson/document.h"
#include "SandBox2D.h"
#include "BigWealth/BigWealthGameLayer.h"




class TestLayer : public Tso::Layer{
public:
  TestLayer():
    Layer("testLayer"),
    m_CameraController(1280.0 / 720 , true),
    m_TrianglePos(glm::vec3(0.f))
  {
      m_VertexArray.reset(Tso::VertexArray::Create());
      m_BackGroundVertexArray.reset(Tso::VertexArray::Create());


      float vertices[5 * 4] = {
          -0.5f , -0.5f , 0.0f , 0.0f , 0.0f,
           0.5f , -0.5f , 0.0f , 1.0f , 0.0f,
          -0.5f ,  0.5f , 0.0f , 0.0f , 1.0f,
           0.5f ,  0.5f , 0.0f , 1.0f , 1.0f
      };

      Tso::Ref<Tso::VertexBuffer> vertexBuffer;
      vertexBuffer.reset(Tso::VertexBuffer::Create(vertices, sizeof(vertices)));

      Tso::Ref<Tso::VertexBuffer> backgroundVertexBuffer;
      backgroundVertexBuffer.reset(Tso::VertexBuffer::Create(vertices, sizeof(vertices)));

      {
          Tso::BufferLayout layout = {
              {Tso::ShaderDataType::Float3 , "a_Position"},
              {Tso::ShaderDataType::Float2 , "a_TexCoord"}
          };

          vertexBuffer->SetLayout(layout);
          backgroundVertexBuffer->SetLayout(layout);
      }

      uint32_t indices[6] = { 0 , 1 , 2 , 1 , 3, 2};

      Tso::Ref<Tso::IndexBuffer> indexBuffer;
      indexBuffer.reset(Tso::IndexBuffer::Create(indices, 6));

      Tso::Ref<Tso::IndexBuffer> backgroundIndexBuffer;
      backgroundIndexBuffer.reset(Tso::IndexBuffer::Create(indices, 6));
      //m_IndexBuffer->Bind();

      m_VertexArray->AddVertexBuffer(vertexBuffer);
      m_VertexArray->SetIndexBuffer(indexBuffer);

      m_BackGroundVertexArray->AddVertexBuffer(backgroundVertexBuffer);
      m_BackGroundVertexArray->SetIndexBuffer(backgroundIndexBuffer);

      m_ShaderLibrary = std::make_shared<Tso::ShaderLibrary>();


#ifdef TSO_PLATFORM_MACOSX

      const int MAXPATH=250;
      char buffer[MAXPATH];
      getcwd(buffer, MAXPATH);
      TSO_CORE_INFO("The current directory is: {0}", buffer);
#endif
      
      std::string lp = "asset/lp2.png";
      std::string b6_9 = "asset/6_9.jpg";
      
      m_Shader = m_ShaderLibrary->Load("asset/shader/Texture.glsl");
      m_BackgroundShader = m_ShaderLibrary->Load("asset/shader/Background.glsl");

      m_Texture = Tso::Texture2D::Create(lp);
      m_BackGroundTexture = Tso::Texture2D::Create(b6_9);



      m_Shader->Bind();
      m_Shader->SetInt("u_Texture", 0);

      m_BackgroundShader->Bind();
      m_BackgroundShader->SetInt("u_Texture", 0);

  }

    virtual void OnImGuiRender() override{
        ImGui::Begin("setting");
        ImGui::ColorEdit3("shaderColor", glm::value_ptr(m_TriangleColor));
        ImGui::End();


    }


    void OnUpdate(Tso::TimeStep ts)override{
        Tso::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });

        Tso::RenderCommand::Clear();
        
        m_CameraController.OnUpdate(ts);

        if (Tso::Input::IsKeyPressed(TSO_KEY_I)) {
            m_TrianglePos.y += m_MoveSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_K)) {
            m_TrianglePos.y -= m_MoveSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_J)) {
            m_TrianglePos.x -= m_MoveSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_L)) {
            m_TrianglePos.x += m_MoveSpeed * ts;
        }


        glm::mat4 triangleTransform = glm::translate(glm::mat4(1.0f), m_TrianglePos);
        
        
        


        Tso::Renderer::BeginScene(m_CameraController.GetCamera());


        m_BackGroundTexture->Bind();

        Tso::Renderer::Submit(m_BackGroundVertexArray, m_BackgroundShader, triangleTransform);//dst

        m_Texture->Bind();

        Tso::Renderer::Submit(m_VertexArray, m_Shader, triangleTransform);//src


        Tso::Renderer::EndScene();
    }
    
    void OnEvent(Tso::Event& event)override{
        //TSO_INFO("testLayer Event:{0}",event.ToString());
        m_CameraController.OnEvent(event);
    }

private:
    Tso::Ref<Tso::Shader> m_Shader , m_BackgroundShader;
    Tso::Ref<Tso::VertexArray> m_VertexArray , m_BackGroundVertexArray;
    Tso::Ref<Tso::ShaderLibrary> m_ShaderLibrary;

    Tso::OrthographicCameraController m_CameraController;
    glm::vec3 m_CameraPosition = glm::vec3(0.f);
    float m_CameraRotation = 0;
    float m_MoveSpeed = 5.f;
    float m_RotationSpeed = 180.0f;

    Tso::Ref<Tso::Texture2D> m_Texture , m_BackGroundTexture;

    glm::vec3 m_TrianglePos;

    glm::vec3 m_TriangleColor = glm::vec3(0.8 , 0.3 , 0.2);

    
};

class SandBox :public Tso::Application {
public:
	SandBox() 
       
    {
        PushLayer(new SandBox2D());
	}
	~SandBox() {

	}


};

Tso::Application* Tso::CreateApplication() {
	return new SandBox();
}

