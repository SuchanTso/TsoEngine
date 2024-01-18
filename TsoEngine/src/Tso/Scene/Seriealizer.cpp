#include "TPch.h"
#include "Seriealizer.h"
#include "Tso/Scene/Entity.h"
#include "Tso/Scene/Component.h"
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "Tso/Renderer/Texture.h"


namespace YAML
{
    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}

namespace Tso {
Seriealizer::Seriealizer(Scene* scene)
:m_Scene(scene)
{
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
}

static void SeriealizeEntity(YAML::Emitter& out, Entity& entity)
{
    out << YAML::BeginMap; // Entity
    out << YAML::Key << "Entity" << YAML::Value << entity.GetEntityID();

    if (entity.HasComponent<TagComponent>())
    {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent

        auto& tag = entity.GetComponent<TagComponent>().m_Name;
        out << YAML::Key << "Tag" << YAML::Value << tag.c_str();

        out << YAML::EndMap; // TagComponent
    }

    if (entity.HasComponent<TransformComponent>())
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent

        auto& comp = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "Translate" << YAML::Value << comp.GetPos();
        out << YAML::Key << "Rotation" << YAML::Value << comp.GetRotate();
        out << YAML::Key << "Scale" << YAML::Value << comp.GetScale();

        out << YAML::EndMap; // TransformComponent
    }

    if (entity.HasComponent<CameraComponent>())
    {
        out << YAML::Key << "CamermaComponent";
        out << YAML::BeginMap; // CameramComponent

        auto& comp = entity.GetComponent<CameraComponent>();
        out << YAML::Key << "ProjectionType" << YAML::Value << (int)comp.m_Camera.GetProjectionType();
        out << YAML::Key << "ProjectionNearClip" << YAML::Value << comp.m_Camera.GetProjectionNearClip();
        out << YAML::Key << "ProjectionFarClip" << YAML::Value << comp.m_Camera.GetProjectionFarClip();
        out << YAML::Key << "ProjectionFov" << YAML::Value << comp.m_Camera.GetProjectionFov();

        out << YAML::Key << "OrthographicNear" << YAML::Value << comp.m_Camera.GetOrthographicNearClip();
        out << YAML::Key << "OrthographicFar" << YAML::Value << comp.m_Camera.GetOrthographicFarClip();
        out << YAML::Key << "OrthographicSize" << YAML::Value << comp.m_Camera.GetOrthographicSize();
        
        out << YAML::Key << "Primary" << YAML::Value << comp.m_Pramiary;
        
        out << YAML::Key << "FixedAspect" << YAML::Value << comp.FixedAspectRatio;
        
        out << YAML::EndMap; // CameramComponent
    }
    
    if (entity.HasComponent<Renderable>())
    {
        out << YAML::Key << "RenderableComponent";
        out << YAML::BeginMap; // RenderableComponent
        auto& comp = entity.GetComponent<Renderable>();
        out << YAML::Key << "Type" << YAML::Value << (int)comp.type;
        
        out << YAML::Key << "Color" << YAML::Value << comp.m_Color;
        
        
        if(comp.subTexture){
            out << YAML::Key << "TexturePath" << YAML::Value << comp.subTexture->GetTexture()->GetPath();
            out << YAML::Key << "SubTexture" << YAML::Value << comp.isSubtexture;
            out << YAML::Key << "SpriteSize" << YAML::Value << comp.spriteSize;
            out << YAML::Key << "SpriteIndex" << YAML::Value << comp.textureIndex;
            out << YAML::Key << "TextureSize" << YAML::Value << comp.textureSize;
            
        }
        out << YAML::EndMap; // RenderableComponent
    }


    if (entity.HasComponent<Rigidbody2DComponent>())
    {
        out << YAML::Key << "Rigidbody2DComponent";
        out << YAML::BeginMap; // RenderableComponent
        auto& comp = entity.GetComponent<Rigidbody2DComponent>();
        out << YAML::Key << "Type" << YAML::Value << (int)comp.Type;

        out << YAML::Key << "FixRotation" << YAML::Value << comp.FixedRotation;

        out << YAML::EndMap; // RenderableComponent
    }

    if (entity.HasComponent<BoxCollider2DComponent>())
    {
        out << YAML::Key << "BoxCollider2DComponent";
        out << YAML::BeginMap; // RenderableComponent
        auto& comp = entity.GetComponent<BoxCollider2DComponent>();
        out << YAML::Key << "Size" << YAML::Value << comp.Size;

        out << YAML::Key << "Offset" << YAML::Value << comp.Offset;

        out << YAML::Key << "Density" << YAML::Value << comp.Density;
        out << YAML::Key << "Friction" << YAML::Value << comp.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << comp.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << comp.RestitutionThreshold;

        out << YAML::EndMap; // RenderableComponent
    }

    out << YAML::EndMap; // Entity
    
}





void Seriealizer::SeriealizeScene(const std::string& path)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    
    m_Scene->m_Registry.each([&](auto entityID)
                             {
        Entity entity = { entityID, m_Scene };
        if (entityID == entt::null)
            return;
        
        SeriealizeEntity(out, entity);
    });
    out << YAML::EndSeq;
    out << YAML::EndMap;
    
    std::ofstream fout(path);
    if(fout){
        fout << out.c_str();
    }
    else{
        TSO_CORE_ERROR("unable to save path = {0}" , path.c_str());
    }
}

bool Seriealizer::DeseriealizeScene(const std::string& path){
    bool res = true;
    YAML::Node data;
    try
    {
        data = YAML::LoadFile(path);
    }
    catch (YAML::ParserException e)
    {
        TSO_CORE_ERROR("Failed to load .teScene file '{0}'\n     {1}", path, e.what());
        return false;
    }
    
    if (!data["Scene"])
        return false;
    
    std::string sceneName = data["Scene"].as<std::string>();
    TSO_CORE_TRACE("Deserializing scene '{0}'", sceneName);
    
    auto entities = data["Entities"];
    if(entities){
        for(auto entity : entities){
            
            std::string name;
            auto tagComponent = entity["TagComponent"];
            if(tagComponent){
                name = tagComponent["Tag"].as<std::string>();
            }
            Entity deserializedEntity = m_Scene->CreateEntity(name);
            auto transformComponent = entity["TransformComponent"];
            if(transformComponent){
                auto& transform = deserializedEntity.GetComponent<TransformComponent>();
                transform.SetPos(transformComponent["Translate"] ? transformComponent["Translate"].as<glm::vec3>() : glm::vec3(0.0f));
                transform.SetRotate(transformComponent["Rotation"] ? transformComponent["Rotation"].as<glm::vec3>() : glm::vec3(0.0f));
                transform.SetScale(transformComponent["Scale"] ? transformComponent["Scale"].as<glm::vec3>() : glm::vec3(1.0f));
            }
            
            auto cameraComponent = entity["CamermaComponent"];
            if(cameraComponent){
                auto& cameraComp = deserializedEntity.AddComponent<CameraComponent>();
                auto& camera = cameraComp.m_Camera;
                camera.SetProjectionType(SceneCamera::ProjectionType(cameraComponent["ProjectionType"] ? cameraComponent["ProjectionType"].as<int>() : 0));
                camera.SetProjectionFov(cameraComponent["ProjectionFov"] ? cameraComponent["ProjectionFov"].as<float>() : glm::radians(45.f));
                camera.SetProjectionNearClip(cameraComponent["ProjectionNearClip"] ? cameraComponent["ProjectionNearClip"].as<float>() : 0.01f);
                camera.SetProjectionFarClip(cameraComponent["ProjectionFarClip"] ? cameraComponent["ProjectionFarClip"].as<float>() : 1000.f);
                
                camera.SetOrthographicSize(cameraComponent["OrthographicSize"] ? cameraComponent["OrthographicSize"].as<float>() : 10.f);
                camera.SetOrthographicNearClip(cameraComponent["OrthographicNear"] ? cameraComponent["OrthographicNear"].as<float>() : -1.0f);
                camera.SetOrthographicFarClip(cameraComponent["OrthographicFar"] ? cameraComponent["OrthographicFar"].as<float>() : 1.f);
                cameraComp.m_Pramiary = cameraComponent["Primary"] ? cameraComponent["Primary"].as<bool>() : false;
                cameraComp.FixedAspectRatio = cameraComponent["FixedAspect"] ? cameraComponent["FixedAspect"].as<bool>() : false;
                
            }
            
            auto renderComponent = entity["RenderableComponent"];
            if(renderComponent){
                auto& renderComp = deserializedEntity.GetComponent<Renderable>();
                renderComp.m_Color = renderComponent["Color"] ? renderComponent["Color"].as<glm::vec4>() : glm::vec4(1.0f);
                renderComp.type = (RenderType)(renderComponent["Type"] ? renderComponent["Type"].as<int>() : 0);
                renderComp.isSubtexture = renderComponent["SubTexture"] ? renderComponent["SubTexture"].as<bool>() : false;
                if(renderComponent["TexturePath"]){
                    auto texturePath = renderComponent["TexturePath"].as<std::string>();
                    auto texture = GetTextureByPath(texturePath);
                    glm::vec2 spriteSize = renderComponent["SpriteSize"] ? renderComponent["SpriteSize"].as<glm::vec2>() : glm::vec2(1.0f , 1.0f);
                    glm::vec2 spriteIndex = renderComponent["SpriteIndex"] ? renderComponent["SpriteIndex"].as<glm::vec2>() : glm::vec2(0.0f , 0.0f);
                    glm::vec2 texSize = renderComponent["TextureSize"] ? renderComponent["TextureSize"].as<glm::vec2>() : glm::vec2(1.0f , 1.0f);
                    renderComp.spriteSize = spriteSize;
                    renderComp.textureIndex = spriteIndex;
                    renderComp.textureSize = texSize;
                    renderComp.subTexture = SubTexture2D::CreateByCoord(texture, spriteSize, spriteIndex, texSize);
                }
            }

            auto RigidBoxComponent = entity["Rigidbody2DComponent"];
            if (RigidBoxComponent) {
                auto& rbc = deserializedEntity.AddComponent< Rigidbody2DComponent>();
                rbc.FixedRotation = RigidBoxComponent["FixedRotation"] ? RigidBoxComponent["FixedRotation"].as<bool>() : false; 
                rbc.Type = Rigidbody2DComponent::BodyType(RigidBoxComponent["Type"] ? RigidBoxComponent["Type"].as<int>() : 0);

            }


            auto box2dcollide = entity["BoxCollider2DComponent"];
            if (box2dcollide) {
                auto& rbc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                rbc.Density = box2dcollide["Density"] ? box2dcollide["Density"].as<float>() : 1.0f;
                rbc.Friction = box2dcollide["Friction"] ? box2dcollide["Friction"].as<float>() : 0.5f;
                rbc.Restitution = box2dcollide["Restitution"] ? box2dcollide["Restitution"].as<float>() : 2.0f;
                rbc.RestitutionThreshold = box2dcollide["RestitutionThreshold"] ? box2dcollide["RestitutionThreshold"].as<float>() : 0.5f;

                rbc.Size = box2dcollide["Size"] ? box2dcollide["Size"].as<glm::vec2>() : glm::vec2(0.5f , 0.5f);
                rbc.Offset = box2dcollide["Offset"] ? box2dcollide["Offset"].as<glm::vec2>() : glm::vec2(0.0f, 0.0f);
            }
            
            
            
        }
    }
    
    return res;
}

Ref<Texture2D> Seriealizer::GetTextureByPath(std::string& path){
    if(m_TextureCache.find(path) == m_TextureCache.end()){
        m_TextureCache[path] = Texture2D::Create(path);
    }
    return m_TextureCache[path];
}



	
}
