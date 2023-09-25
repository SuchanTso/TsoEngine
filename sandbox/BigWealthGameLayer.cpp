#include "BigWealthGameLayer.h"

BigWealthGameLayer::BigWealthGameLayer()
    :Layer("BigWealthLayer"),
    m_CameraController(1280.0 / 720, true)
{
    m_ShaderLibrary = std::make_shared<Tso::ShaderLibrary>();

    m_Shader = m_ShaderLibrary->Load("asset/shader/Shader2D.glsl");
    Tso::Renderer2D::Init(m_Shader);

    std::string lp = "asset/lp2.png";

    m_Texture = Tso::Texture2D::Create(lp);


}

void BigWealthGameLayer::OnImGuiRender()
{
}

void BigWealthGameLayer::OnUpdate(Tso::TimeStep ts)
{
}

void BigWealthGameLayer::OnEvent(Tso::Event& event)
{
}
