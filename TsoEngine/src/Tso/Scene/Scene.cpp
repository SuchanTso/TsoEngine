#include "TPch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"

namespace Tso {


Entity Scene::CreateEntity(){
    entt::entity entityID = m_Registry.create();
    Entity res = Entity(entityID , this , "");
    res.AddComponent<TransformComponent>(glm::vec3(0.0 , 0.0 , 0.9));
    res.AddComponent<Renderable>(glm::vec4(0.6f, 0.3f, 0.2f, 1.0f));
    return res;
}

void Scene::OnUpdate(TimeStep ts)
{
    const auto& view = m_Registry.view<TransformComponent>();
    for (auto& entity : view) {
        TransformComponent& comp = view.get<TransformComponent>(entity);
        glm::vec3& pos = comp.GetPos();
        m_Time += ts.GetSecond();
        pos.x = cos(m_Time);
        pos.y = sin(m_Time);
    }

    auto group = m_Registry.view<Renderable , TransformComponent>();
    for (auto& entity : group) {
        const auto& [render, trans] = group.get<Renderable, TransformComponent>(entity);
        auto pos = trans.GetPos();
        
        render.Render(pos);
        
    }
}



}
