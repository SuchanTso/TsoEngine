//
//  Resource.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/29.
//
#pragma once
#ifndef Resource_hpp
#define Resource_hpp
#include "Tso/Core/Core.h"
#include "Tso/Renderer/Font.h"
#include "Tso/Core/UUID.h"



namespace Tso{
class SubTexture2D;
class Material;
class AnimationClip;
class Prefab;
struct ResourceData{
    Ref<Font> font;
    std::unordered_map<std::string, Ref<Texture2D>> textures;//save for now. delete once uuid system done build
    std::unordered_map<UUID, Ref<Texture2D>> textureMap;
    std::unordered_map<UUID, Ref<Material>>  materialMap;
    std::unordered_map<UUID, Ref<Shader>> shaderMap;
    std::unordered_map<UUID, Ref<Font>> fontMap;
    std::unordered_map<UUID, Ref<AnimationClip>> animationClipMap;
    std::unordered_map<UUID, Ref<Prefab>> prefabMap;
//    std::unordered_map<std::string , UUID> textureNameMap;
//    std::unordered_map<std::string , UUID> materialNameMap;
//    std::unordered_map<std::string , UUID> shaderNameMap;
//    std::unordered_map<std::string , UUID> fontNameMap;
//    std::unordered_map<std::string , UUID> animationClipNameMap;
    
    std::unordered_map<std::string, UUID> resourcePathMap;// full path to UUID
    std::unordered_map<UUID, std::string>resourceUUIDPathMap;//UUID to full path
};

class Resource{
public:
    static void Init();
    static void InitProject();
    static void ShutDown();
    static Ref<ResourceData>GetResource(){return s_Resource ? s_Resource->m_ResourceData : nullptr;}
    static Ref<Texture2D> GetTextureByPath( std::string& path);
    
    template<typename T>
    static void AddResource(const std::string& name,const UUID& uuid , Ref<T>resource);
    
    template<>
    static void AddResource<Shader>(const std::string& name , const UUID &uuid, Ref<Shader> resource);
    
    template<>
    static void AddResource<Texture2D>(const std::string& name , const UUID &uuid, Ref<Texture2D> resource);
    
    template<>
    static void AddResource<Material>(const std::string& name , const UUID &uuid, Ref<Material> resource);
    
    template<>
    static void AddResource<Prefab>(const std::string& name , const UUID &uuid, Ref<Prefab> resource);
    
    template<>
    static void AddResource<Font>(const std::string& name , const UUID &uuid, Ref<Font> resource);
    
    template<>
    static void AddResource<AnimationClip>(const std::string& name , const UUID &uuid, Ref<AnimationClip> resource);

    template<typename T>
    static Ref<T> GetResource(const UUID& uuid);
    template<typename T>
    static std::unordered_map<UUID , Ref<T>>& GetResourceMap();
    
    static std::vector<std::filesystem::path> GetAllResourceToExport();
    
    static UUID GetUUIDFromPath(const std::string& path);
    
    static UUID ImportAsset(const std::filesystem::path& path);
    
    static UUID ReadUUIDFromMeta(const std::filesystem::path& path);

private:
    inline static Ref<Resource> s_Resource;
    Ref<ResourceData> m_ResourceData;
};
}

#endif /* Resource_hpp */
