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
    :m_Translation(pos)
{
    //TSO_CORE_INFO("get pos = [{0} , {1} , {2}]", pos.x, pos.y, pos.z);
}






Renderable::Renderable(const glm::vec4& color)
    :m_Color(color)
{
}

void Renderable::Render(const glm::mat4& transfrom)
{
}



TagComponent::TagComponent(const std::string& name)
    :m_Name(name)
{

}

TextComponent::TextComponent(const std::string& fontPath)
:FontPath(fontPath)
{
    TextFont = std::make_shared<Font>(std::filesystem::path(fontPath));

    
}

TextComponent::TextComponent()
{
    

    
}

}
