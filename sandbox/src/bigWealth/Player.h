#pragma once
#include "TSO.h"
#include "glm/glm.hpp"
#include "Tso/Renderer/Renderer2D.h"


class Player{
    
    struct AnimationData{
        float startTime;
        float duration;
        glm::vec3 targetPosition;
        bool movable;
    };
    
    
public:
    Player(){}
    Player(const std::string& modelPath , const uint32_t& playerID);
    ~Player() = default;
    void OnLoad(const glm::vec3& startPosition);
    short OnGamble(const uint32_t& nowPlayerID);
    void OnRender();
    
private:
    uint32_t m_PlayerID = 0;//0 is an invalid ID but for default
    std::string m_ModelPath = "";
    Tso::Ref<Tso::Texture2D> m_ModelTexture;
    
    long long m_Wealth = 0;
    
    glm::vec3 m_Position ;
    
    std::queue<AnimationData> m_Animations;
};
