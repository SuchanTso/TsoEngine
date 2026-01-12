//
//  Resource.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/29.
//
#include "TPch.h"
#include "Resource.h"
#include "Tso/Renderer/Texture.h"
#include "Tso/Renderer/Material.h"
#include "Tso/Animation/AnimationClip.h"

namespace Tso{
void Resource::Init(){
    s_Resource = CreateRef<Resource>();
    s_Resource->m_ResourceData = CreateRef<ResourceData>();
    auto fontPath = Project::GetResourcePath() + "assets/TimesNewRoman.ttf";
    s_Resource->m_ResourceData->font = CreateRef<Font>(fontPath);
}
Ref<Texture2D> Resource::GetTextureByPath( std::string& path){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textures.find(path) == s_Resource->m_ResourceData->textures.end()){
        s_Resource->m_ResourceData->textures[path] = Texture2D::Create(path);
    }
    return s_Resource->m_ResourceData->textures[path];
}

//===============================================GetResource============================================================
template<>Ref<Shader>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->shaderMap.find(uuid) == s_Resource->m_ResourceData->shaderMap.end()){
        TSO_CORE_WARN("Shader {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->shaderMap[uuid];
}

template<>Ref<Texture2D>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textureMap.find(uuid) == s_Resource->m_ResourceData->textureMap.end()){
        TSO_CORE_WARN("Texture {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->textureMap[uuid];
}

template<>Ref<Material>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->materialMap.find(uuid) == s_Resource->m_ResourceData->materialMap.end()){
        TSO_CORE_WARN("Material {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->materialMap[uuid];
}

template<>Ref<Font>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->fontMap.find(uuid) == s_Resource->m_ResourceData->fontMap.end()){
        TSO_CORE_WARN("Material {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->fontMap[uuid];
}

template<>Ref<AnimationClip>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->animationClipMap.find(uuid) == s_Resource->m_ResourceData->animationClipMap.end()){
        TSO_CORE_WARN("Material {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->animationClipMap[uuid];
}
//===============================================GetResource============================================================


//=============================================GetResourceMap===========================================================
template<>std::unordered_map<UUID , Ref<Shader>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->shaderMap;
}

template<>std::unordered_map<UUID , Ref<Material>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->materialMap;
}

template<>std::unordered_map<UUID , Ref<Texture2D>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->textureMap;
}

template<>std::unordered_map<UUID , Ref<Font>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->fontMap;
}

template<>std::unordered_map<UUID , Ref<AnimationClip>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->animationClipMap;
}

//=============================================GetResourceMap===========================================================

//===============================================AddResource============================================================
template<> void Resource::AddResource<Shader>(const std::string& name , const UUID &uuid, Ref<Shader> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->shaderMap.find(uuid) != s_Resource->m_ResourceData->shaderMap.end()){
        TSO_CORE_WARN("Duplicated shader {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->shaderMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
}

template<> void Resource::AddResource<Material>(const std::string& name , const UUID &uuid, Ref<Material> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->materialMap.find(uuid) != s_Resource->m_ResourceData->materialMap.end()){
        TSO_CORE_WARN("Duplicated material {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->materialMap[uuid] = resource;
    resource->SetUUID(uuid);
}

template<> void Resource::AddResource<Texture2D>(const std::string& name , const UUID &uuid, Ref<Texture2D> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textureMap.find(uuid) != s_Resource->m_ResourceData->textureMap.end()){
        TSO_CORE_WARN("Duplicated texture {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->textureMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
}

template<> void Resource::AddResource<Font>(const std::string& name , const UUID &uuid, Ref<Font> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->fontMap.find(uuid) != s_Resource->m_ResourceData->fontMap.end()){
        TSO_CORE_WARN("Duplicated Font {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->fontMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
}

template<> void Resource::AddResource<AnimationClip >(const std::string& name , const UUID &uuid, Ref<AnimationClip> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->animationClipMap.find(uuid) != s_Resource->m_ResourceData->animationClipMap.end()){
        TSO_CORE_WARN("Duplicated Font {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->animationClipMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
}

//===============================================AddResource============================================================


}
