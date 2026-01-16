//
//  ContentBrowserPanel.hpp
//  TsoEditor
//
//  Created by 左斯诚 on 2026/1/15.
//
#pragma once
#ifndef ContentBrowserPanel_hpp
#define ContentBrowserPanel_hpp

#include <filesystem>
#include "Tso/Renderer/Texture.h"

namespace Tso {
    class SceneHierarchyPanel;
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel();
        void OnImGuiRender();
        void ImportAsset(const std::string& filter);
        void SetSceneHierarchyPanel(SceneHierarchyPanel* panel);

    private:
        void DrawCreateMaterialPopup();
        void DrawCreateShaderPopup();
        void DrawCreateScriptPopup();
    private:
        // 核心：当前显示的目录
        std::filesystem::path m_CurrentDirectory;
        std::filesystem::path m_BaseDirectory; // Project Root (Assets folder)

        // 图标资源
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
        Ref<Texture2D> m_ShaderIcon;
        // ... 其他图标
        
        enum class DeferredAction {
                None,
                CreateMaterial,
                CreateShader,
                CreateScript,
                ImportTexture,
                ImportShader,
                ImportFont,
            };
            
        DeferredAction m_NextAction = DeferredAction::None;
        SceneHierarchyPanel* m_SceneHPanel = nullptr;
    };
}

#endif /* ContentBrowserPanel_hpp */
