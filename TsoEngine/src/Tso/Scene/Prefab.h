//
//  Prefab.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/18.
//
#pragma once
#ifndef Prefab_hpp
#define Prefab_hpp
#include "Tso/Project/Resource.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Entity.h"

namespace Tso {

    class Prefab {
    public:
        // 创建空 Prefab
        Prefab();
        Prefab(const std::string& path);
        ~Prefab();

        // 从现有 Entity 创建 Prefab (用于编辑器 "Create Prefab")
        // 这会把 entity 的数据复制到 m_PrefabScene 中
        void Create(Entity entity);
        
        static Ref<Prefab> Create(const std::string& path);

        // 获取模板 Entity (通常用于预览或复制)
        Entity GetEntity() { return m_Entity; }
        
        // 实例化到目标场景 (核心功能)
        Entity Instantiate(Ref<Scene> targetScene);
        
        static Ref<Prefab> Deserialize(const std::string& path);
        
        void Serialize(const std::string& path);
        
        std::string& GetPath(){return m_Path;}

    private:
        // Prefab 拥有一个私有的微型 Scene，只为了存放这个模板 Entity
        // 这样可以利用 Scene 现有的组件管理机制
        Ref<Scene> m_PrefabScene;
        Entity m_Entity;
        std::string m_Path;
    };
}


#endif /* Prefab_hpp */
