#include "TPch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"
#include "Tso/Renderer/Renderer2D.h"
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Utils {
    b2BodyType Rigidbody2DTypeToBox2DBody(Tso::Rigidbody2DComponent::BodyType bodyType)
    {
        switch (bodyType)
        {
        case Tso::Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
        case Tso::Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
        case Tso::Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
        }

        TSO_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }
}


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


    if(m_PhysicWorld){
        const int32_t velocityIterations = 6;
        const int32_t positionIterations = 2;
        m_PhysicWorld->Step(ts, velocityIterations, positionIterations);

        // Retrieve transform from Box2D
        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2Body* body = (b2Body*)rb2d.RuntimeBody;
            
            const auto& position = body->GetPosition();
            transform.m_Translation.x = position.x;
            transform.m_Translation.y = position.y;
            transform.m_Rotation.z = body->GetAngle();

            
        }
    }

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

void Scene::OnScenePlay()
{
    m_PhysicWorld = new b2World({ 0.0f , -9.8f});
    m_PhysicsListener = new NativeContactListener();

    //NativeBehavior registry
    m_PhysicWorld->SetContactListener(m_PhysicsListener);

    //
    auto view = m_Registry.view<Rigidbody2DComponent>();
    for (auto e : view)
    {
        Entity entity = { e, this };
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        b2BodyDef bodyDef;
        bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef.position.Set(transform.m_Translation.x, transform.m_Translation.y);
        bodyDef.angle = transform.m_Rotation.z;

        b2Body* body = m_PhysicWorld->CreateBody(&bodyDef);
        body->SetFixedRotation(rb2d.FixedRotation);
        rb2d.RuntimeBody = body;

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

            b2PolygonShape boxShape;
            boxShape.SetAsBox(bc2d.Size.x * transform.m_Scale.x, bc2d.Size.y * transform.m_Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &boxShape;
            fixtureDef.density = bc2d.Density;
            fixtureDef.friction = bc2d.Friction;
            fixtureDef.restitution = bc2d.Restitution;
            fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
            body->CreateFixture(&fixtureDef);

            if (entity.HasComponent<NativeScriptComponent>()) {
                auto& nsc = entity.GetComponent<NativeScriptComponent>();
                body->GetUserData().pointer = reinterpret_cast<uintptr_t>(&nsc);
               
            }
            
        }

        
    }
}

void Scene::OnSceneStop()
{
    if (m_PhysicWorld) {
        delete m_PhysicWorld;
        m_PhysicWorld = nullptr;
    }
    if (m_PhysicsListener) {
        delete m_PhysicsListener;
        m_PhysicsListener = nullptr;
    }

}





}
