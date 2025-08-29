//
//  Resource.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/29.
//
#include "TPch.h"
#include "Resource.h"
namespace Tso{
void Resource::Init(){
    s_Resource = CreateRef<Resource>();
    s_Resource->m_ResourceData = CreateRef<ResourceData>();
    auto fontPath = Project::GetResourcePath() + "assets/TimesNewRoman.ttf";
    s_Resource->m_ResourceData->font = CreateRef<Font>(fontPath);
}
}
