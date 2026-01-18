//
//  Prefab.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/18.
//

#include "TPch.h"
#include "Prefab.h"
#include "Tso/Scene/Component.h"
#include "Tso/Scene/Seriealizer.h"

namespace Tso {

    Prefab::Prefab() {
        m_PrefabScene = CreateRef<Scene>();
    }

    Prefab::Prefab(const std::string& path):m_Path(path){
        m_PrefabScene = CreateRef<Scene>();
    }
    Ref<Prefab> Prefab::Create(const std::string& path){
        return Deserialize(path);
    }

    Prefab::~Prefab() {
        m_PrefabScene = nullptr;
    }

    void Prefab::Create(Entity sourceEntity) {
        std::string name = "Prefab";
        m_Entity = m_PrefabScene->CopyEntity(sourceEntity);
    }

    Entity Prefab::Instantiate(Scene* targetScene) {
        Entity newEntity = targetScene->CopyEntity(m_Entity);
        TSO_CORE_TRACE("Insantiate prefab {}" , newEntity.GetUUID());
        return newEntity;
    }

    void Prefab::Serialize(const std::string& path) {
        if (!m_Entity) { // 检查实体有效性
            TSO_CORE_WARN("Invalid prefab entity, cannot serialize.");
            return;
        }

        YAML::Emitter out;
        out << YAML::BeginMap; // Root Map
        
        // 元数据
        out << YAML::Key << "Prefab" << YAML::Value << m_Entity.GetComponent<TagComponent>().m_Name;
        
        // 实体数据 (复用 SceneSerializer 的逻辑)
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        
        // 调用现有的序列化函数
        // 注意：SeriealizeEntity 内部输出了 BeginMap/EndMap，所以这里它是 Sequence 的一项
        Seriealizer::SeriealizeEntity(out, m_Entity);
        
        // 如果 Prefab 支持子物体 (Hierarchy)，这里还需要递归序列化子节点
        // 假设目前只处理单层
        
        out << YAML::EndSeq; // End Entities
        out << YAML::EndMap; // End Root

        std::ofstream fout(path);
        if (fout) {
            fout << out.c_str();
            TSO_CORE_INFO("Saved Prefab to: {0}", path);
        } else {
            TSO_CORE_ERROR("Unable to save prefab to path: {0}", path);
        }
    }

    Ref<Prefab> Prefab::Deserialize(const std::string &path){
        Ref<Prefab> prefab = CreateRef<Prefab>(path);

        Seriealizer serializer(prefab->m_PrefabScene.get());
        Tso::Buffer fileBuffer = Tso::VirtualFileSystem::ReadFile(path);

        if (!fileBuffer.IsValid()) {
            TSO_CORE_ASSERT(false, "Failed to load file from VFS: {0}", path);
            return nullptr;
        }

        YAML::Node data;
        try {
            std::string yamlString(fileBuffer.DataPtr(), fileBuffer.Size());
            data = YAML::Load(yamlString);
        }
        catch (YAML::ParserException e)
        {
            TSO_CORE_ERROR("Failed to load prefab file '{0}'\n     {1}", path, e.what());
            return nullptr;
        }
        auto entities = data["Entities"];
        if (entities) {
            for (auto entityNode : entities) {
                Entity sourceEntity = serializer.DeserializeEntity(entityNode);
                prefab->m_Entity = sourceEntity;
                break;//one entity in a prefab for now
            }
        }
        
        return prefab;
    }

}
