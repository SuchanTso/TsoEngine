#include "TPch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"

namespace Tso {


Entity Scene::CreateEntity(const std::string& name){
    entt::entity entityID = m_Registry.create();
    std::string entityName = name.length() > 0 ? name : ("blankEntity" + std::to_string(m_EntityCount));
    Entity res = Entity(entityID , this , name);
    res.AddComponent<TransformComponent>(glm::vec3(0.0 , 0.0 , 0.9));
    res.AddComponent<Renderable>(glm::vec4(0.6f, 0.3f, 0.2f, 1.0f));
    res.AddComponent<TagComponent>(entityName);
    m_EntityCount++;
    return res;
}

void Scene::OnUpdate(TimeStep ts)
{
    m_Time += ts.GetSecond();

    /*auto& view = m_Registry.view<NativeScriptComponent>();
    for (auto& e : view) {
        auto& nsc = m_Registry.get<NativeScriptComponent>(e);
        if (!nsc.Instance)
        {
            TSO_CORE_INFO("test content = {0}", nsc.test.c_str());

            if (!nsc.InstantiateScript)
                continue;
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = Entity{ e, this };
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnUpdate(ts);
    }*/

    m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
        {
            // TODO: Move to Scene::OnScenePlay
            if (!nsc.Instance)
            {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entity, this };
                nsc.Instance->OnCreate();
            }

            nsc.Instance->OnUpdate(ts);
        });

   



    /*const auto& view = m_Registry.view<TransformComponent>();
    for (auto& entity : view) {
        TransformComponent& comp = view.get<TransformComponent>(entity);
        glm::vec3& pos = comp.GetPos();
        
        if (comp.GetRand() < 0) {
            TSO_CORE_INFO("get rand = {0}", comp.GetRand());
            comp.SetRand(m_Time);
        }
        pos.x = cos(m_Time + comp.GetRand());
        pos.y = sin(m_Time + comp.GetRand());
    }*/

    auto group = m_Registry.view<Renderable , TransformComponent>();
    for (auto& entity : group) {
        const auto& [render, trans] = group.get<Renderable, TransformComponent>(entity);
        auto pos = trans.GetPos();
        
        render.Render(pos);
        
    }

    
}

std::vector<std::string> Scene::GetSceneEntityNames()
{
    std::vector<std::string> res(m_EntityCount);
    int i = 0;
    auto view = m_Registry.view<TagComponent>();
    for (auto& e : view) {
        if (i >= m_EntityCount) {
            break;
        }
        res[i++] = view.get<TagComponent>(e).GetTagName();
    }
    return res;
}



}
