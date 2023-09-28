
#include "Player.h"

Player::Player(const std::string& modelPath , const uint32_t& playerID)
:m_ModelPath(modelPath) , m_PlayerID(playerID)
{
}


void Player::OnLoad(const glm::vec3& startPosition){
    m_Position = startPosition;
    TSO_ASSERT(m_ModelPath.length() > 0 ,"Invalid modelPath when load player");
//    m_ModelTexture = Tso::Texture2D::Create(m_ModelPath);
}

short Player::OnGamble(const uint32_t& nowPlayerID){
    if(nowPlayerID == m_PlayerID){
        //TODO: random gambling
        return 1;
    }
    else{
        return -1;
    }
}

void Player::OnRender(){
    Tso::Renderer2D::DrawQuad(m_Position, 0.f, { 0.5 , 0.5 }, {0.2 , 0.8 , 0.3 , 0.2});
    
}


