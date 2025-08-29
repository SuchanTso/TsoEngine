//
//  Resource.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/29.
//
#include "TPch.h"
#include "Resource.h"
#include "Tso/Renderer/Texture.h"
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

}
