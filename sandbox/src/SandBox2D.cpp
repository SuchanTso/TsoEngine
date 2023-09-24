#include "SandBox2D.h"

SandBox2D::SandBox2D()
	:Layer("testLayer"),
    m_CameraController(1280.0 / 720, true),
    m_TrianglePos(glm::vec3(0.f))
{
    m_ShaderLibrary = std::make_shared<Tso::ShaderLibrary>();

    m_Shader = m_ShaderLibrary->Load("asset/shader/Shader2D.glsl");
    Tso::Renderer2D::Init(m_Shader);
    
    std::string lp = "asset/lp2.png";

    m_Texture = Tso::Texture2D::Create(lp);
    
    m_TrianglePos = glm::vec3(0.0 , 0.0 , 0.0);
}


void SandBox2D::OnImGuiRender()
{
}

void SandBox2D::OnUpdate(Tso::TimeStep ts)
{
    
    Tso::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
    Tso::RenderCommand::Clear();

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

    m_CameraController.OnUpdate(ts);
    Tso::Renderer2D::BeginScene(m_CameraController.GetCamera());
    Tso::Renderer2D::DrawQuad({-0.5 , 0.5 , 0.0}, 0.f, {0.5 , 0.5}, {0.8 , 0.3 , 0.2 , 1.0});
    Tso::Renderer2D::DrawQuad({0.5 , 0.0 , 0.0}, 0.f, {1.0 , 1.0}, {0.2 , 0.8 , 0.3 , 0.2});
    Tso::Renderer2D::DrawQuad(m_TrianglePos, 0.f, { 0.5 , 0.5 }, m_Texture);





    Tso::Renderer2D::EndScene();
}

void SandBox2D::OnEvent(Tso::Event& event)
{
}
