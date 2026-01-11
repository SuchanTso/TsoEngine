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
struct ResourceData{
    Ref<Font> font;
    std::unordered_map<std::string, Ref<Texture2D>> textures;//save for now. delete once uuid system done build
    std::unordered_map<UUID, Ref<Texture2D>> textureMap;
    std::unordered_map<UUID, Ref<Material>>  materialMap;
    std::unordered_map<UUID, Ref<Shader>> shaderMap;
    std::unordered_map<UUID, Ref<Font>> fontMap;
    
    std::unordered_map<std::string , UUID> textureNameMap;
    std::unordered_map<std::string , UUID> materialNameMap;
    std::unordered_map<std::string , UUID> shaderNameMap;
    std::unordered_map<std::string , UUID> fontNameMap;
};

class Resource{
public:
    static void Init();
    static void ShutDown();
    static Ref<ResourceData>GetResource(){return s_Resource ? s_Resource->m_ResourceData : nullptr;}
    static Ref<Texture2D> GetTextureByPath( std::string& path);
//    static Ref<Texture2D> GetTexture(const UUID& uuid);
//    static Ref<Material>  GetMaterial(const UUID& uuid);
//    static Ref<Shader>    GetShader(const UUID& uuid);
//    static std::unordered_map<UUID , Ref<Shader>>& GetAllShaders(){return s_Resource->m_ResourceData->shaderMap;}
//    static std::unordered_map<UUID , Ref<Material>>& GetAllMaterials(){return s_Resource->m_ResourceData->materialMap;}
//    static std::unordered_map<UUID , Ref<Texture2D>>& GetAllTextures(){return s_Resource->m_ResourceData->textureMap;}
//    static std::unordered_map<UUID , Ref<Font>>& GetAllFonts(){return s_Resource->m_ResourceData->fontMap;}

//    static void AddTexture(const UUID& uuid , Ref<Texture2D> texture);
//    static void AddMaterial(const UUID& uuid , Ref<Material> material);
//    static void AddShader(const UUID& uuid , Ref<Shader> shader);
    
    template<typename T>
    static void AddResource(const std::string& name,const UUID& uuid , Ref<T>resource);
    template<typename T>
    static Ref<T> GetResource(const UUID& uuid);
    template<typename T>
    static std::unordered_map<UUID , Ref<T>>& GetResourceMap();
private:
    inline static Ref<Resource> s_Resource;
    Ref<ResourceData> m_ResourceData;
};
}

#endif /* Resource_hpp */
