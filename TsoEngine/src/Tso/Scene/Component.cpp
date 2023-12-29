#include "TPch.h"
#include "Component.h"
#include "Tso/Renderer/Renderer2D.h"

namespace Tso {



TransformComponent::TransformComponent(const glm::mat4& transform)
{
    
}

TransformComponent::TransformComponent(const TransformComponent& transform)
{
    
}

TransformComponent::TransformComponent(const glm::vec3& pos)
    :m_Pos(pos)
{
    TSO_CORE_INFO("get pos = [{0} , {1} , {2}]", pos.x, pos.y, pos.z);
}






Renderable::Renderable(const glm::vec4& color)
    :m_Color(color)
{
}

void Renderable::Render(const glm::vec3& pos)
{
    Renderer2D::DrawQuad(pos, glm::vec2(1.0, 1.0), m_Color);
}



TagComponent::TagComponent(const std::string& name)
    :m_Name(name)
{

}

}
