//
//  AnimationPanel.cpp
//  TsoEditor
//
//  Created by 左斯诚 on 2026/1/12.
//
#include "TPch.h"
#include "AnimationPanel.h"
#include "Tso/Project/Resource.h"
#include "Tso/Renderer/SubTexture2D.h"
#include "Tso/Utils/PlatformUtils.h"

namespace Tso {

    AnimationPanel::AnimationPanel() {
        
    }


    void AnimationPanel::OnImGuiRender() {
        ImGui::Begin("Animation Editor");

        if (!m_CurrentClip) {
            if (ImGui::Button("Create New Clip")) {
                m_CurrentClip = CreateRef<AnimationClip>("New Anim");
                Resource::AddResource(m_CurrentClip->GetName(), UUID(),m_CurrentClip);
            }
            ImGui::End();
            return;
        }

        // --- 顶部区域：标题 + 拖拽源 ---
        std::string title = "Editing: " + m_CurrentClip->GetName();
        
        // 绘制一个宽按钮作为标题和拖拽手柄
        ImGui::Button(title.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30.0f));

        // 拖拽源逻辑
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            UUID clipUUID = m_CurrentClip->GetUUID();
            // 只有 UUID 有效时才允许拖拽
            if (clipUUID != 0) {
                ImGui::SetDragDropPayload("RESOURCE_ANIM_CLIP", &clipUUID, sizeof(UUID));
                ImGui::Text("Assign Clip: %s", m_CurrentClip->GetName().c_str());
            }
            ImGui::EndDragDropSource();
        }

        // --- 工具栏：Save 按钮 ---
        // 放在标题栏下面
        if (ImGui::Button("Save Asset")) {
            std::string filePath = FileDialogs::SaveFile("Animation (*.anim)\0*.anim\0");
            std::string name = std::filesystem::path(filePath).stem().string();
            if (!filePath.empty()) {
                // 确保后缀正确
                if (filePath.find(".anim") == std::string::npos)
                    filePath += ".anim";
                m_CurrentClip->SetPath(filePath);
                m_CurrentClip->SetName(name);
                m_CurrentClip->Deseralize(filePath);
                
                TSO_CORE_INFO("Saved Animation Clip to: {0}", filePath);
            }
        }
        
        ImGui::SameLine();
        ImGui::Text("UUID: %llu", (uint64_t)m_CurrentClip->GetUUID()); // 调试显示 UUID

        ImGui::Separator();

        // --- 分栏布局 (保持不变) ---
        static float leftPaneWidth = 300.0f;
        
        // Left Pane
        ImGui::BeginChild("LeftPane", ImVec2(leftPaneWidth, 0), true);
        DrawProperties();
        ImGui::Separator();
        DrawPreview();
        ImGui::EndChild();

        ImGui::SameLine();

        // Right Pane
        ImGui::BeginChild("RightPane", ImVec2(0, 0), true);
        DrawTimeline();
        ImGui::EndChild();

        ImGui::End();
    }

    void AnimationPanel::DrawProperties() {
            ImGui::Text("Properties");
            
            // 1. 名称
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, m_CurrentClip->GetName().c_str());
            if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                m_CurrentClip->SetName(std::string(buffer));
            }

            // 2. 循环设置
            bool loop = m_CurrentClip->IsLooping();
            if (ImGui::Checkbox("Loop", &loop)) {
                m_CurrentClip->SetLooping(loop);
            }
            
            // 3. 关联纹理 (Sprite Sheet)
            ImGui::Separator();
            ImGui::Text("Sprite Sheet:");
            
            // 显示当前纹理名称
            std::string texName = "None";
            if (m_CurrentClip->GetTextureUUID() != 0) {
                auto tex = Resource::GetResource<Texture2D>(m_CurrentClip->GetTextureUUID());
                if (tex) {
                    texName = tex->GetName();
                    m_PreviewTexture = tex;
                }
            }
            
            ImGui::Button(texName.c_str(), ImVec2(-1, 0)); // 全宽按钮作为 Drop Target
            
            // 拖拽接收
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE")) {
                    UUID uuid = *(const UUID*)payload->Data;
                    m_CurrentClip->SetTextureUUID(uuid);
                    m_PreviewTexture = Resource::GetResource<Texture2D>(uuid);
                }
                ImGui::EndDragDropTarget();
            }
        }

    void AnimationPanel::DrawTimeline() {
            ImGui::Text("Timeline");
            ImGui::SameLine();
            if (ImGui::Button("Add Frame")) {
                // 添加一帧，默认 index 0, 0.1s
                m_CurrentClip->AddFrame(glm::vec2(0.f),glm::vec2(1.f), 0.1f);
            }

            ImGui::Separator();

            //以此列出每一帧
            // 列表头
            ImGui::Columns(4, "TimelineColumns"); // 3列：Index, Duration, Sprite ID
            ImGui::Text("Frame #"); ImGui::NextColumn();
            ImGui::Text("Duration (s)"); ImGui::NextColumn();
            ImGui::Text("Sprite Index"); ImGui::NextColumn();
            ImGui::Text("Sprite Size"); ImGui::NextColumn();
            ImGui::Separator();

            auto& frames = m_CurrentClip->GetFramesMutable(); // 需要一个能修改的 getter
            int frameToDelete = -1;

            for (int i = 0; i < frames.size(); i++) {
                auto& frame = frames[i];
                
                // Col 1: Frame ID (可点击选择)
                char label[32];
                sprintf(label, "Frame %d", i);
                if (ImGui::Selectable(label, m_SelectedFrame == i, ImGuiSelectableFlags_SpanAllColumns)) {
                    m_SelectedFrame = i;
                }
                // 右键菜单删除
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete Frame")) frameToDelete = i;
                    ImGui::EndPopup();
                }
                ImGui::NextColumn();

                // Col 2: Duration
                ImGui::PushID(i);
                ImGui::DragFloat("##dur", &frame.Duration, 0.01f, 0.01f, 10.0f);
                ImGui::PopID();
                ImGui::NextColumn();

                // Col 3: Sprite Index
                // 这里用 DragInt 修改，或者后面可以做一个可视化选取器
                ImGui::PushID(i);
                ImGui::DragFloat2("##idx", glm::value_ptr(frame.SpriteIndex));

                ImGui::PopID();
                ImGui::NextColumn();
                
                ImGui::PushID(i);
                ImGui::DragFloat2("##size", glm::value_ptr(frame.SpriteSize));

                ImGui::PopID();
                ImGui::NextColumn();
            }
            
            ImGui::Columns(1); // 恢复

            if (frameToDelete != -1) {
                frames.erase(frames.begin() + frameToDelete);
                if (m_SelectedFrame == frameToDelete) m_SelectedFrame = -1;
            }
        }
    
    void AnimationPanel::DrawPreview() {
            ImGui::Text("Preview");
            
            // 播放控制
            if (ImGui::Button(m_IsPlaying ? "Stop" : "Play")) {
                m_IsPlaying = !m_IsPlaying;
                if (m_IsPlaying) m_PreviewTime = 0.0f; // 重置
            }

            // 模拟 Update
            if (m_IsPlaying) {
                m_PreviewTime += ImGui::GetIO().DeltaTime;
                
                // 计算当前帧
                float time = 0.0f;
                const auto& frames = m_CurrentClip->GetFrames();
                int currentIdx = 0;
                
                // 简单的累加查找 (实际可以用 Update 函数封装)
                for (int i = 0; i < frames.size(); i++) {
                    if (m_PreviewTime >= time && m_PreviewTime < time + frames[i].Duration) {
                        currentIdx = i;
                        break;
                    }
                    time += frames[i].Duration;
                }
                
                // 循环逻辑
                if (m_PreviewTime >= m_CurrentClip->GetTotalDuration()) {
                    if (m_CurrentClip->IsLooping()) {
                        m_PreviewTime = 0.0f;
                    } else {
                        m_PreviewTime = m_CurrentClip->GetTotalDuration();
                        currentIdx = (int)frames.size() - 1;
                        m_IsPlaying = false; // 停止
                    }
                }
                m_PreviewFrameIndex = currentIdx;
            } else {
                // 如果没播放，显示选中的帧，或者第0帧
                if (m_SelectedFrame >= 0 && m_SelectedFrame < m_CurrentClip->GetFrames().size())
                    m_PreviewFrameIndex = m_SelectedFrame;
                else
                    m_PreviewFrameIndex = 0;
            }

            // 绘制图像
            if (m_PreviewTexture && !m_CurrentClip->GetFrames().empty()) {
                glm::vec2 spriteIdx = m_CurrentClip->GetFrames()[m_PreviewFrameIndex].SpriteIndex;
                glm::vec2 spriteSize = m_CurrentClip->GetFrames()[m_PreviewFrameIndex].SpriteSize;
                auto subTex = SubTexture2D::CreateByCoord(m_PreviewTexture, spriteSize, spriteIdx, glm::vec2(1.f));
                // 计算 UV
                // 这里假设你需要根据 spriteIdx 计算 UV。
                // 如果你的引擎有 SubTexture2D::CreateFromIndex 之类的方法，可以用它
                // 在 ImGui 中，我们需要传入 UV0 和 UV1
                
                // 假设是 10x10 的网格 (临时硬编码，以后要在 Clip 里保存 SpriteSheet 的行列数)
                
                auto texCoord = subTex->GetTexCoords();
                // 注意 OpenGL Y轴翻转
                ImGui::Image((void*)(uintptr_t)m_PreviewTexture->GetTextureID(),
                             ImVec2(128, 128),
                             ImVec2(texCoord[3][0],texCoord[3][1]),
                             ImVec2(texCoord[1][0],texCoord[1][1])  // uv0 (左上) -> OpenGL (左下)
                             ); // uv1 (右下) -> OpenGL (右上)
            } else {
                // 没图显示个方框
                ImGui::Button("No Texture", ImVec2(128, 128));
            }
        }
}
