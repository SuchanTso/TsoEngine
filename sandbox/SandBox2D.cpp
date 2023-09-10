#include "SandBox2D.h"

SandBox2D::SandBox2D()
	:Layer("testLayer"),
    m_CameraController(1280.0 / 720, true),
    m_TrianglePos(glm::vec3(0.f))
{


}


void SandBox2D::OnImGuiRender()
{
}

void SandBox2D::OnUpdate(Tso::TimeStep ts)
{
}

void SandBox2D::OnEvent(Tso::Event& event)
{
}
