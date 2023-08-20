#include "TSO.h"
#include <Platform/OpenGL/OpenGLShader.h>

#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>



class TestLayer : public Tso::Layer{
public:
  TestLayer():
    Layer("testLayer")
    ,m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) , m_TrianglePos(glm::vec3(0.f))
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


      std::string vertexSrc1 = R"(
            #version 330 core
           
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec2 a_TexCoord;

            uniform mat4 u_ProjViewMat;
            uniform mat4 u_Transform;

            out vec3 v_Position;
            out vec2 v_TexCoord;


            void main(){
                v_Position = a_Position;
                v_TexCoord = a_TexCoord;
                gl_Position = u_ProjViewMat * vec4(a_Position , 1.0);
            }
             
        )";


      std::string fragmentSrc = R"(
            #version 330 core
           
            layout(location = 0) out vec4 color;
            in vec3 v_Position;
            in vec2 v_TexCoord;

            uniform sampler2D u_Texture;

            void main(){
                color = texture(u_Texture , v_TexCoord);
                //color = vec4(v_TexCoord , 0.0 , 1.0f);
            }
             
        )";

      float test = pow(-2, -128);


      m_Shader.reset(Tso::Shader::Create("asset/shader/Texture.glsl"));
      m_BackgroundShader.reset(Tso::Shader::Create(vertexSrc1, fragmentSrc));
      m_Texture = Tso::Texture2D::Create(std::string("asset/lp2.png"));
      m_BackGroundTexture = Tso::Texture2D::Create(std::string("asset/6_9.jpg"));


      std::dynamic_pointer_cast<Tso::OpenGLShader>(m_Shader)->Bind();
      std::dynamic_pointer_cast<Tso::OpenGLShader>(m_Shader)->UploadInt("u_Texture", 0);

      std::dynamic_pointer_cast<Tso::OpenGLShader>(m_BackgroundShader)->Bind();
      std::dynamic_pointer_cast<Tso::OpenGLShader>(m_BackgroundShader)->UploadInt("u_Texture", 0);

  }

    virtual void OnImGuiRender() override{
        ImGui::Begin("setting");
        ImGui::ColorEdit3("shaderColor", glm::value_ptr(m_TriangleColor));
        ImGui::End();


    }


    void OnUpdate(Tso::TimeStep ts)override{
        Tso::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });

        Tso::RenderCommand::Clear();

        if (Tso::Input::IsKeyPressed(TSO_KEY_W)) {
            m_CameraPosition.y += m_MoveSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_S)) {
            m_CameraPosition.y -= m_MoveSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_A)) {
            m_CameraPosition.x -= m_MoveSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_D)) {
            m_CameraPosition.x += m_MoveSpeed * ts;
        }

        if (Tso::Input::IsKeyPressed(TSO_KEY_LEFT)) {
            m_CameraRotation += m_RotationSpeed * ts;
        }
        else if (Tso::Input::IsKeyPressed(TSO_KEY_RIGHT)) {
            m_CameraRotation -= m_RotationSpeed * ts;
        }

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


        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotationZ(m_CameraRotation);

        glm::mat4 triangleTransform = glm::translate(glm::mat4(1.0f), m_TrianglePos);
        
        
        


        Tso::Renderer::BeginScene(m_Camera);


        m_BackGroundTexture->Bind();

        Tso::Renderer::Submit(m_BackGroundVertexArray, m_BackgroundShader, triangleTransform);//dst

        m_Texture->Bind();

        Tso::Renderer::Submit(m_VertexArray, m_Shader, triangleTransform);//src


        Tso::Renderer::EndScene();
    }
    
    void OnEvent(Tso::Event& event)override{
        TSO_INFO("testLayer Event:{0}",event.ToString());
    }

private:
    Tso::Ref<Tso::Shader> m_Shader , m_BackgroundShader;
    Tso::Ref<Tso::VertexArray> m_VertexArray , m_BackGroundVertexArray;

    Tso::OrthographicCamera m_Camera;
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
        PushLayer(new TestLayer());
	}
	~SandBox() {

	}


};

Tso::Application* Tso::CreateApplication() {
	return new SandBox();
}
