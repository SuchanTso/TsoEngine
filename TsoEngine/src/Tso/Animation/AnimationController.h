//
//  AnimationController.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/12.
//
#pragma once
#ifndef AnimationController_hpp
#define AnimationController_hpp
#include "AnimationClip.h"
#include <unordered_map>

namespace Tso {
    struct AnimationState {
        std::string Name;
        Ref<AnimationClip> Clip;
        // std::vector<Transition> Transitions; 
    };

    // 运行时控制器
    class AnimationController {
    public:
        AnimationController() = default;
        ~AnimationController() = default;
        void AddState(const std::string& name, Ref<AnimationClip> clip);

        void Play(const std::string& stateName);//state change

        glm::vec2 Update(float ts);
        
        std::string GetCurrentStateName() const { return m_CurrentState ? m_CurrentState->Name : "None"; }
        
        std::unordered_map<std::string, AnimationState>& GetStatesMutable() { return m_States; }
            
        void RemoveState(const std::string& name) {m_States.erase(name);}
            
        void SetDefaultState(const std::string& name) {m_DefaultStateName = name;}
        std::string GetDefaultState() const { return m_DefaultStateName; }

        void RenameState(const std::string& oldName, const std::string& newName);
        
        const std::unordered_map<std::string, AnimationState>& GetStates(){return m_States;}

    private:
        std::unordered_map<std::string, AnimationState> m_States;
        AnimationState* m_CurrentState = nullptr;
        std::string m_DefaultStateName = "";

        
        float m_TimeAccumulator = 0.0f;
        int m_CurrentFrameIdx = 0;
    };
}


#endif /* AnimationController_hpp */
