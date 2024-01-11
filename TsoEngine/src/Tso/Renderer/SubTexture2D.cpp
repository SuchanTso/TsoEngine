//
//  SubTexture2D.cpp
//  TsoEngine
//
//  Created by SuchanTso on 2023/11/7.
//
#include "TPch.h"
#include "SubTexture2D.h"

namespace Tso{

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture , const glm::vec2& min , const glm::vec2& max)
:m_Texture(texture)
{
    m_Coords.resize(4);
    
    m_Coords[0] = {min.x , min.y};
    m_Coords[1] = {max.x , min.y};
    m_Coords[2] = {max.x , max.y};
    m_Coords[3] = {min.x , max.y};
    
}
Ref<SubTexture2D> SubTexture2D::CreateByCoord(const Ref<Texture2D>& texture , const glm::vec2& spriteSize , const glm::vec2& index , const glm::vec2& size){
    
    auto texWidth = texture->GetWidth();
    auto texHeight = texture->GetHeight();
    glm::vec2 min = { index.x * spriteSize.x / texWidth  , index.y * spriteSize.y / texHeight };
    glm::vec2 max = { (index.x + size.x) * spriteSize.x / texWidth , (index.y + size.y) * spriteSize.y / texHeight };
    
    return std::make_shared<SubTexture2D>(texture , min , max);
}

void SubTexture2D::RecalculateCoords(const glm::vec2& spriteSize , const glm::vec2& index , const glm::vec2& size){
    auto texWidth = m_Texture->GetWidth();
    auto texHeight = m_Texture->GetHeight();
    glm::vec2 min = { index.x * spriteSize.x / texWidth  , index.y * spriteSize.y / texHeight };
    glm::vec2 max = { (index.x + size.x) * spriteSize.x / texWidth , (index.y + size.y) * spriteSize.y / texHeight };
    
    m_Coords[0] = {min.x , min.y};
    m_Coords[1] = {max.x , min.y};
    m_Coords[2] = {max.x , max.y};
    m_Coords[3] = {min.x , max.y};
}


}
