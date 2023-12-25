#include "TPch.h"
#include "Component.h"

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

void TransformComponent::OnUpdate(TimeStep ts)
{
    
}
    
void TransformComponent::AddComponent(const glm::vec3 &pos){
    TSO_CORE_INFO("Add Transform component !! and pos = {0}" , pos.x);
}



}
