#include "BigWealthGameLayer.h"

BigWealthGameLayer::BigWealthGameLayer()
    :Layer("BigWealthLayer"),
    m_CameraController(1280.0 / 720, true)
{
    m_ShaderLibrary = std::make_shared<Tso::ShaderLibrary>();

    m_Shader = m_ShaderLibrary->Load("asset/shader/Shader2D.glsl");
    Tso::Renderer2D::Init(m_Shader);

//    std::string lp = "asset/lp2.png";
//
//    m_Texture = Tso::Texture2D::Create(lp);
    m_BigWealthLogic.SetGameState(GameState::LogIn);


}

void BigWealthGameLayer::OnImGuiRender()
{
}

void BigWealthGameLayer::OnUpdate(Tso::TimeStep ts)
{
    switch (m_BigWealthLogic.GetGameState()) {
        case GameState::None:
            //do nothing
            break;
            
        case GameState::LogIn:
            m_BigWealthLogic.OnLogin();
            break;
            
        case GameState::Init:
            m_BigWealthLogic.OnInit("");
            break;
            
        case GameState::Ready:
            break;
            
        case GameState::Actiong:
            m_BigWealthLogic.OnAction();
            break;
            
        case GameState::Gambling:
            m_BigWealthLogic.OnGambling();
            break;
            
        case GameState::Calculate:
            
            break;
        default:
            break;
    }
    
    Tso::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
    Tso::RenderCommand::Clear();
    Tso::Renderer2D::BeginScene(m_CameraController.GetCamera());
    m_BigWealthLogic.OnRender();
    Tso::Renderer2D::EndScene();
}

void BigWealthGameLayer::OnEvent(Tso::Event& event)
{
}
