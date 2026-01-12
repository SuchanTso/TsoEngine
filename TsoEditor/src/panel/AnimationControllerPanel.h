//
//  AnimationControllerPanel.hpp
//  TsoEditor
//
//  Created by 左斯诚 on 2026/1/12.
//
#pragma once
#ifndef AnimationControllerPanel_hpp
#define AnimationControllerPanel_hpp

#include "Tso/Animation/AnimationController.h"

namespace Tso {
    class AnimationControllerPanel {
    public:
        AnimationControllerPanel();
        void OnImGuiRender();
        
        // 设置当前要编辑的 Controller
        void SetContext(Ref<AnimationController> controller);

    private:
        Ref<AnimationController> m_CurrentController;
        std::string m_SelectedStateName; // 当前选中的状态名
    };
}

#endif /* AnimationControllerPanel_hpp */
