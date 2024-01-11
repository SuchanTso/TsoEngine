#include "TPch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"
#include "Tso/Renderer/Renderer2D.h"


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

    m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
        {
            // TODO: Move to Scene::OnScenePlay
            if (!nsc.Instance && nsc.hasBind)
            {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entity, this };
                nsc.Instance->OnCreate();
            }
            if(nsc.hasBind)
                nsc.Instance->OnUpdate(ts);
        });

    auto view = m_Registry.view<TransformComponent, CameraComponent>();
    SceneCamera* mainCamera = nullptr;
    glm::mat4* mainCameraTransfrom = nullptr;

    for (auto& e : view) {
        auto& camera = view.get<CameraComponent>(e);
        if (camera.m_Pramiary) {
            mainCamera = &camera.m_Camera;
            auto& transfrom = view.get<TransformComponent>(e);
            mainCameraTransfrom = &transfrom.GetTransform();
        }
    }

    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
    RenderCommand::Clear();
    if (mainCamera) {
        Renderer2D::BeginScene(*mainCamera, *mainCameraTransfrom);
        
        auto group = m_Registry.view<Renderable , TransformComponent>();
        for (auto& entity : group) {
            const auto& [render, trans] = group.get<Renderable, TransformComponent>(entity);
            auto transform = trans.GetTransform();
            if(render.type == RenderType::PureColor){
                Renderer2D::DrawQuad(transform,render.m_Color);
            }
            else{
                if(render.isSubtexture){
                    Renderer2D::DrawQuad(transform,render.subTexture);
                }
                else{
                    Renderer2D::DrawQuad(transform,render.subTexture->GetTexture());
                }
            }
        }
        Renderer2D::EndScene();
    }

    
}

void Scene::DeleteEntity(Entity entity){
    m_Registry.destroy(entity);
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
