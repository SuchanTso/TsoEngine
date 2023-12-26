#include "TPch.h"
#include "Component.h"
#include "Tso/Renderer/Renderer2D.h"

namespace Tso {

template<typename T , typename Arg>
void Component::AddComponent(const Arg &&arg ...){
    return AddComponent<T>(std::forward<T>(arg));
}


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


void TransformComponent::OnUpdate(TimeStep ts)
{
    
}




Renderable::Renderable(const glm::vec4& color)
    :m_Color(color)
{
}

void Renderable::Render(const glm::vec3& pos)
{
    Renderer2D::DrawQuad(pos, glm::vec2(1.0, 1.0), m_Color);
}

void Renderable::OnUpdate(TimeStep ts)
{
}

}
