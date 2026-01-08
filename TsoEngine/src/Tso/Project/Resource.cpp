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

Ref<Texture2D> Resource::GetTexture(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textureMap.find(uuid) == s_Resource->m_ResourceData->textureMap.end()){
        TSO_CORE_WARN("Texture {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->textureMap[uuid];
}

Ref<Material> Resource::GetMaterial(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->materialMap.find(uuid) == s_Resource->m_ResourceData->materialMap.end()){
        TSO_CORE_WARN("Material {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->materialMap[uuid];
}

Ref<Shader> Resource::GetShader(const UUID &uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->shaderMap.find(uuid) == s_Resource->m_ResourceData->shaderMap.end()){
        TSO_CORE_WARN("Shader {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->shaderMap[uuid];
}

//void Resource::AddShader(const UUID &uuid, Ref<Shader> shader){
//    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
//    if(s_Resource->m_ResourceData->shaderMap.find(uuid) != s_Resource->m_ResourceData->shaderMap.end()){
//        TSO_CORE_WARN("Duplicated shader {} added in the asset, replacing..." , uuid);
//    }
//    s_Resource->m_ResourceData->shaderMap[uuid] = shader;
//}
//
//void Resource::AddTexture(const UUID &uuid, Ref<Texture2D> texture){
//    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
//    if(s_Resource->m_ResourceData->textureMap.find(uuid) != s_Resource->m_ResourceData->textureMap.end()){
//        TSO_CORE_WARN("Duplicated texture {} added in the asset, replacing..." , uuid);
//    }
//    s_Resource->m_ResourceData->textureMap[uuid] = texture;
//}
//
//void Resource::AddMaterial(const UUID &uuid, Ref<Material> material){
//    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
//    if(s_Resource->m_ResourceData->materialMap.find(uuid) != s_Resource->m_ResourceData->materialMap.end()){
//        TSO_CORE_WARN("Duplicated material {} added in the asset, replacing..." , uuid);
//    }
//    s_Resource->m_ResourceData->materialMap[uuid] = material;
//}

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


}
