//
//  AnimationController.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/12.
//

#include "TPch.h"
#include "AnimationController.h"

namespace Tso{

    void AnimationController::AddState(const std::string& name, Ref<AnimationClip> clip) {
        m_States[name] = { name, clip };
        if (!m_CurrentState) m_CurrentState = &m_States[name];
    }

    void AnimationController::Play(const std::string& stateName) {
        if (m_States.find(stateName) != m_States.end()) {
            if (m_CurrentState->Name != stateName) {
                m_CurrentState = &m_States[stateName];
                m_TimeAccumulator = 0.0f;
                m_CurrentFrameIdx = 0;
            }
        }
    }

    glm::vec2 AnimationController::Update(float ts) {
        if (!m_CurrentState || !m_CurrentState->Clip) return glm::vec2(-1.f);

        const auto& frames = m_CurrentState->Clip->GetFrames();
        if (frames.empty()) return glm::vec2(-1.f);

        m_TimeAccumulator += ts;

        // 检查是否需要切换到下一帧
        if (m_TimeAccumulator >= frames[m_CurrentFrameIdx].Duration) {
            m_TimeAccumulator -= frames[m_CurrentFrameIdx].Duration;
            
            // 下一帧
            m_CurrentFrameIdx++;

            // 循环处理
            if (m_CurrentFrameIdx >= frames.size()) {
                if (m_CurrentState->Clip->IsLooping()) {
                    m_CurrentFrameIdx = 0;
                } else {
                    m_CurrentFrameIdx = (int)frames.size() - 1; // 停在最后一帧
                }
            }
        }

        return frames[m_CurrentFrameIdx].SpriteIndex;
    }

    void AnimationController::RenameState(const std::string& oldName, const std::string& newName) {
        if (m_States.find(oldName) == m_States.end()) return;
        if (m_States.find(newName) != m_States.end()) return; // 名字已存在

        AnimationState state = m_States[oldName];
        state.Name = newName;
        m_States.erase(oldName);
        m_States[newName] = state;
        
        if (m_DefaultStateName == oldName) m_DefaultStateName = newName;
    }

}
