//
//  SubTexture2D.hpp
//  TsoEngine
//
//  Created by SuchanTso on 2023/11/7.
//
#pragma once
#ifndef SubTexture2D_hpp
#define SubTexture2D_hpp

#include <stdio.h>
#include "Tso/Core/Core.h"
#include "Texture.h"


namespace Tso {
class SubTexture2D{
public:
    SubTexture2D(const Ref<Texture2D>& texture , const glm::vec2& min , const glm::vec2& max);
    
    static Ref<SubTexture2D> CreateByCoord(const Ref<Texture2D>& texture , const glm::vec2& spriteSize , const glm::vec2& index , const glm::vec2& size);
    
    Ref<Texture2D>& GetTexture(){return m_Texture;}
    
    std::vector<glm::vec2>& GetTexCoords(){return m_Coords;}
    
    void RecalculateCoords(const glm::vec2& spriteSize , const glm::vec2& index , const glm::vec2& size);
    
private:
    Ref<Texture2D> m_Texture = nullptr;
    std::vector<glm::vec2> m_Coords;
    
};
}

#endif /* SubTexture2D_hpp */
