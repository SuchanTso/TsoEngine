//
//  AnimationPanel.hpp
//  TsoEditor
//
//  Created by 左斯诚 on 2026/1/12.
//
#pragma once
#ifndef AnimationPanel_hpp
#define AnimationPanel_hpp
#include "Tso/Animation/AnimationClip.h"
#include "Tso/Animation/AnimationController.h"
#include "Tso/Renderer/Texture.h"

namespace Tso {

    class AnimationPanel {
    public:
        AnimationPanel();
        void OnImGuiRender();

        void SetContext(Ref<AnimationClip> clip){m_CurrentClip = clip;} // 设置当前正在编辑的 Clip

    private:
        void DrawTimeline();
        void DrawPreview();
        void DrawProperties();

    private:
        Ref<AnimationClip> m_CurrentClip;
        Ref<Texture2D> m_PreviewTexture; // 当前 Clip 关联的纹理
        
        bool m_IsPlaying = false;
        float m_PreviewTime = 0.0f;
        int m_PreviewFrameIndex = 0;
        
        // 编辑器状态
        int m_SelectedFrame = -1;
    };
}

#endif /* AnimationPanel_hpp */
