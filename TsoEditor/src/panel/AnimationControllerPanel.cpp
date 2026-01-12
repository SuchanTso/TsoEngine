//
//  AnimationControllerPanel.cpp
//  TsoEditor
//
//  Created by 左斯诚 on 2026/1/12.
//
#include "TPch.h"
#include "AnimationControllerPanel.h"
#include "Tso/Project/Resource.h"
#include "misc/cpp/imgui_stdlib.h"


namespace Tso {

    AnimationControllerPanel::AnimationControllerPanel() {}

    void AnimationControllerPanel::SetContext(Ref<AnimationController> controller) {
        m_CurrentController = controller;
        m_SelectedStateName = "";
    }

    void AnimationControllerPanel::OnImGuiRender() {
        ImGui::Begin("Animator Controller Editor");

        if (!m_CurrentController) {
            ImGui::Text("No Controller selected.");
            // 这里可以加一个 Button 创建新的 Controller
            ImGui::End();
            return;
        }

        // 布局：左侧列表，右侧详情
        static float leftWidth = 200.0f;
        
        // --- 左侧：状态列表 ---
        ImGui::BeginChild("StateList", ImVec2(leftWidth, 0), true);
        
        ImGui::Text("States");
        ImGui::Separator();
        
        if (ImGui::Button("Add New State")) {
            // 生成唯一名字
            std::string baseName = "New State";
            std::string name = baseName;
            int idx = 0;
            auto& states = m_CurrentController->GetStatesMutable();
            while (states.find(name) != states.end()) {
                name = baseName + " " + std::to_string(++idx);
            }
            m_CurrentController->AddState(name, nullptr); // 创建空状态
        }

        ImGui::Separator();

        auto& states = m_CurrentController->GetStatesMutable();
        // 因为在循环中可能会修改 Key (重命名) 或 删除，不能直接迭代修改
        // 我们只做显示和选择
        for (auto& [name, state] : states) {
            bool isSelected = (m_SelectedStateName == name);
            
            // 默认状态显示为绿色
            if (name == m_CurrentController->GetDefaultState()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
            }
            
            if (ImGui::Selectable(name.c_str(), isSelected)) {
                m_SelectedStateName = name;
            }

            if (name == m_CurrentController->GetDefaultState()) {
                ImGui::PopStyleColor();
            }

            // 右键菜单
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Set as Default")) {
                    m_CurrentController->SetDefaultState(name);
                }
                if (ImGui::MenuItem("Delete")) {
                    // 标记删除，稍后处理
                    // 为简化代码这里不写复杂的延迟删除逻辑，实际建议用 DeferredAction 模式
                    // 这里简单演示直接删会有迭代器失效风险
                    // 建议只在 Popup 关闭后处理删除
                }
                ImGui::EndPopup();
            }
        }

        ImGui::EndChild(); // End Left

        ImGui::SameLine();

        // --- 右侧：状态详情 ---
        ImGui::BeginChild("StateDetails", ImVec2(0, 0), true);
        
        if (!m_SelectedStateName.empty() && states.find(m_SelectedStateName) != states.end()) {
            AnimationState& state = states[m_SelectedStateName];
            
            ImGui::Text("State Details");
            ImGui::Separator();

            // 1. 修改名字
            // 注意：修改 Map 的 Key 比较麻烦，这里演示 UI
            static char buffer[64] = "";
            std::strcpy(buffer, state.Name.c_str());
//            ImGui::InputText("Test State", stateBuffer, sizeof(stateBuffer));

            if (ImGui::InputText("Name", buffer , sizeof(buffer))) {
                // 如果按回车或者失去焦点，执行重命名逻辑
                 if (ImGui::IsItemDeactivatedAfterEdit()) {
                     std::string bufferStr = buffer;
                     if (!bufferStr.empty() && bufferStr != state.Name) {
                         m_CurrentController->RenameState(state.Name, bufferStr);
                         m_SelectedStateName = buffer; // 更新选中名
                     }
                 }
            }

            // 2. 绑定的 Animation Clip
            ImGui::Text("Motion (Clip):");
            
            std::string clipName = state.Clip ? state.Clip->GetName() : "None";
            ImGui::Button(clipName.c_str(), ImVec2(-1, 30)); // 宽按钮

            // --- 拖拽接收 Clip ---
            if (ImGui::BeginDragDropTarget()) {
                // 假设 Payload 是 RESOURCE_ANIM_CLIP
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_ANIM_CLIP")) {
                    UUID clipUUID = *(const UUID*)payload->Data;
                    state.Clip = Resource::GetResource<AnimationClip>(clipUUID);
                }
                ImGui::EndDragDropTarget();
            }
            
            // 3. 是否是默认状态
            bool isDefault = (m_CurrentController->GetDefaultState() == state.Name);
            if (ImGui::Checkbox("Default State", &isDefault)) {
                if (isDefault) m_CurrentController->SetDefaultState(state.Name);
            }
            
        } else {
            ImGui::Text("Select a state to edit.");
        }

        ImGui::EndChild(); // End Right

        ImGui::End();
    }
}
