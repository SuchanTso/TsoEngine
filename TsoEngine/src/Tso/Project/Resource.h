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


namespace Tso{
class SubTexture2D;
struct ResourceData{
    Ref<Font> font;
    std::unordered_map<std::string, Ref<Texture2D>> textures;
};

class Resource{
public:
    static void Init();
    static void ShutDown();
    static Ref<ResourceData>GetResource(){return s_Resource ? s_Resource->m_ResourceData : nullptr;}
    static Ref<Texture2D> GetTextureByPath( std::string& path);
private:
    inline static Ref<Resource> s_Resource;
    Ref<ResourceData> m_ResourceData;
};
}

#endif /* Resource_hpp */
