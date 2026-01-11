//
//  UISystem.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/14.
//
#include "TPch.h"
#include "UISystem.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Entity.h"
#include "Tso/Scene/Component.h"
#include "Tso/Core/Keycode.h"
#include "Tso/Core/MouseButtonCode.h"
#include "Tso/Core/Application.h"
#include "Tso/Renderer/Renderer2D.h"
#include "Tso/Renderer/ViewportManager.h"
#include "Tso/Renderer/Renderer2DMaterial.h"

namespace Tso{

    UISystem::UISystem(Scene* scene):m_Scene(scene){
//        m_VirtualResolutionX = 1920;
//        m_VirtualResolutionY = 1280;
        auto uiCamera = m_Scene->GetUICamera();
        if(uiCamera){
            uiCamera->SetOrthographicSize(UISystem::VirtualResolutionY);
            uiCamera->SetAspectRatio((UISystem::VirtualResolutionX * 1.f) / UISystem::VirtualResolutionY);
        }
    }

    glm::vec2 UISystem::ConvertScreenToUIWorld(float mouseX, float mouseY, int screenWidth, int screenHeight) {
        // 假设虚拟分辨率是 1920x1080
        float uiX = (mouseX / screenWidth) * 1920.0f;
        float uiY = ((mouseY / screenHeight) * 1080.0f); // Y轴通常需要翻转
        return { uiX, uiY };
    }

    glm::vec4 UISystem::WorldToScreenRect(TransformComponent& transform) {

        // 1. 获取UI元素在UI世界坐标系中的位置和大小
        glm::vec3 worldPos = transform.GetPos();
        glm::vec3 worldSize = transform.GetScale();

        // 2. 计算UI元素在UI世界中的包围盒
        glm::vec2 minWorld = { worldPos.x - worldSize.x / 2.0f, worldPos.y - worldSize.y / 2.0f };
        glm::vec2 maxWorld = { worldPos.x + worldSize.x / 2.0f, worldPos.y + worldSize.y / 2.0f };

        // 3. 将UI世界坐标线性映射到 [0, 1] 的归一化空间
        float normX = (minWorld.x + UISystem::VirtualResolutionX / 2.0f) / VirtualResolutionX;
        float normY = (minWorld.y + UISystem::VirtualResolutionY / 2.0f) / UISystem::VirtualResolutionY;
        float normWidth = worldSize.x / VirtualResolutionX;
        float normHeight = worldSize.y / UISystem::VirtualResolutionY;

        // 4. 将归一化坐标映射到当前实际的视口像素坐标
        glm::vec2 viewportPos = ViewportManager::GetPosition();
        glm::vec2 viewportSize = ViewportManager::GetSize();

        float screenX = viewportPos.x + normX * viewportSize.x;
        // Y轴需要翻转，因为屏幕坐标通常是左上角为(0,0)，而我们的UI世界是左下角
        float screenY = viewportPos.y + (1.0f - (normY + normHeight)) * viewportSize.y;
        float screenWidthPx = normWidth * viewportSize.x;
        float screenHeightPx = normHeight * viewportSize.y;

        return { screenX, screenY, screenWidthPx, screenHeightPx };
    }

    void UISystem::OnUpdate(TimeStep ts) {
//        if (auto scene_sp = m_Scene.lock()) {
        
        HandleInteraction(m_Scene);
            
            // 处理文本输入
            if (m_FocusedInputField != 0) {
                Entity focusedEntity = m_Scene->GetEntityByUUID(m_FocusedInputField);
                if (focusedEntity && focusedEntity.HasComponent<InputFieldComponent>() && focusedEntity.HasComponent<TextComponent>()) {
                    auto& inputField = focusedEntity.GetComponent<InputFieldComponent>();
                    auto& textComp = focusedEntity.GetComponent<TextComponent>();
                    // 从 Input 系统获取本帧输入的字符
                    std::string typedChars = Input::GetTypedCharactersThisFrame();
                    if (!typedChars.empty()) {
                        textComp.Text += typedChars;
                        if (inputField.OnValueChanged.valid()) {
                            inputField.OnValueChanged(textComp.Text);
                        }
                        TSO_CORE_INFO("typed:{}" , textComp.Text);
                    }
                    
                    // 处理退格键
                    if (Input::IsKeyPressed(TSO_KEY_BACKSPACE) && !textComp.Text.empty()) {
                        textComp.Text.pop_back();
                        if (inputField.OnValueChanged.valid()) {
                            inputField.OnValueChanged(textComp.Text);
                        }
                    }
                    
                    // 处理回车键提交
                    if (Input::IsKeyPressed(TSO_KEY_ENTER) && inputField.OnSubmit.valid()) {
                        inputField.OnSubmit(textComp.Text);
                        m_FocusedInputField = 0; // 提交后取消聚焦
                        inputField.IsFocused = false;
                    }
                }
            }
            Render(m_Scene);
//        }
    }

    void UISystem::HandleInteraction(Scene*scene) {
        auto viewportSize = ViewportManager::GetSize();
        glm::vec2 mousePos = ConvertScreenToUIWorld(Input::GetViewportMouseX(), Input::GetViewportMouseY(), viewportSize.x , viewportSize.y);
        bool mousePressed = Input::IsMouseButtonPressed(TSO_MOUSE_BUTTON_LEFT);
        bool mouseClicked = Input::IsMouseButtonClicked(TSO_MOUSE_BUTTON_LEFT); // 单击事件

        // 1. 处理按钮
        auto buttonView = scene->GetAllEntitiesWith<UITransformComponent , TransformComponent, ButtonComponent>();
        for (auto e : buttonView) {
            
            auto& button = e.GetComponent<ButtonComponent>();
            auto& uiTransformc = e.GetComponent<UITransformComponent>();
            button.IsPressed = false; // 避免保持按下状态

            // 简单的AABB命中检测
            glm::vec4 screenRect = {uiTransformc.UIpos.x , uiTransformc.UIpos.x + uiTransformc.UISize.x , uiTransformc.UIpos.y , uiTransformc.UIpos.y + uiTransformc.UISize.y};
//            TSO_CORE_INFO("screenX[{} , {}] , screenY[{} , {}]" , screenRect.x , screenRect.y , screenRect.z , screenRect.w);

            bool isHovered = (mousePos.x >= screenRect.x && mousePos.x <= screenRect.y &&
                              mousePos.y >= screenRect.z && mousePos.y <= screenRect.w);
            
            button.IsHovered = isHovered;
//            TSO_CORE_INFO("Hover:[{}] , Click:[{}]" , isHovered ? 1 : 0 , mouseClicked ? 1 : 0);
            if (isHovered && mouseClicked) {
                TSO_CORE_INFO("clicked Button");
                button.IsPressed = true;
                if (button.OnClick.valid()) {
                    sol::protected_function onClickFunc = button.OnClick;
                    // 执行Lua回调
                    auto result = onClickFunc();
                    if (!result.valid()) {
                        // [MODIFIED] 获取并打印详细的Lua错误信息
                        sol::error err = result;
                        TSO_CORE_ERROR("Lua error in button OnClick callback: {}", err.what());
                    }
                }
            }
        }

        // 2. 处理输入框聚焦
        if (mouseClicked) {
            // 先取消所有输入框的聚焦
            if (m_FocusedInputField != 0) {
                Entity lastFocused = scene->GetEntityByUUID(m_FocusedInputField);
                 if(lastFocused) lastFocused.GetComponent<InputFieldComponent>().IsFocused = false;
                 m_FocusedInputField = 0;
            }

            auto inputView = scene->GetAllEntitiesWith<TransformComponent, InputFieldComponent>();
            for (auto e : inputView) {
                auto& inputc = e.GetComponent<InputFieldComponent>();
                auto& uiTransformc = e.GetComponent<UITransformComponent>();

                glm::vec4 screenRect = {uiTransformc.UIpos.x , uiTransformc.UIpos.x + uiTransformc.UISize.x , uiTransformc.UIpos.y , uiTransformc.UIpos.y + uiTransformc.UISize.y};
//                TSO_CORE_INFO("screenX[{} , {}] , screenY[{} , {}]" , screenRect.x , screenRect.y , screenRect.z , screenRect.w);
                bool isHovered = (mousePos.x >= screenRect.x && mousePos.x <= screenRect.y &&
                                  mousePos.y >= screenRect.z && mousePos.y <= screenRect.w);
                if (isHovered) {
                    auto& inputField = e.GetComponent<InputFieldComponent>();
                    inputField.IsFocused = true;
                    m_FocusedInputField = e.GetUUID();
                    break; // 只聚焦一个
                }
            }
        }
    }

void UISystem::Render(Scene*scene) {
//    auto windowWidth = Application::Get().GetWindow().GetWidth();
//    auto windowHeight = Application::Get().GetWindow().GetHeight();
//    auto windowPosX = Application::Get().GetWindow().GetPosX();
//    auto viewportSize = ViewportManager::GetSize();
//    auto viewportPos = ViewportManager::GetPosition();
    auto UITxetView = scene->GetAllEntitiesWith<TextComponent , UITransformComponent , Renderable>();
    for(auto e : UITxetView){
        if(e.HasComponent<ButtonComponent>() || e.HasComponent<InputFieldComponent>())continue;//deal them below
        auto& transform = e.GetComponent<TransformComponent>();
        auto& uiTransformc = e.GetComponent<UITransformComponent>();
        auto& renderc = e.GetComponent<Renderable>();
        
        glm::vec2 layoutPos = uiTransformc.UIpos;
        glm::vec2 layoutSize = uiTransformc.UISize;
        float layoutRotation = uiTransformc.Rotation_Z;
        
        glm::vec3 worldPos;
        worldPos.x = layoutPos.x - UISystem::VirtualResolutionX * 0.5f + layoutSize.x * 0.5;
        worldPos.y = layoutPos.y - UISystem::VirtualResolutionY * 0.5f + layoutSize.y * 0.5f;
        worldPos.z = transform.GetPos().z;
        
        glm::mat4 worldTransfrom = glm::translate(glm::mat4(1.0f), worldPos)
        * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
        * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});
        auto& textComp = e.GetComponent<TextComponent>();
        if(textComp.TextFont){
            MaterialInstanceComponent* matIns = e.HasComponent<MaterialInstanceComponent>() ? &e.GetComponent<MaterialInstanceComponent>() : nullptr;
            Renderer2DMaterial::DrawString(textComp.TextFont, worldTransfrom, textComp.Text, textComp.textParam, (int)e, renderc.textMat , matIns);

        }
    }
    
    // 渲染按钮
    auto buttonView = scene->GetAllEntitiesWith<TransformComponent, ButtonComponent , UITransformComponent>();
    for (auto e : buttonView) {
//        TSO_CORE_INFO("rendering button {} " , e.GetUUID());
        auto& transform = e.GetComponent<TransformComponent>();
        auto& button = e.GetComponent<ButtonComponent>();
        auto& uiTransformc = e.GetComponent<UITransformComponent>();
        
        glm::vec2 layoutPos = uiTransformc.UIpos;
        glm::vec2 layoutSize = uiTransformc.UISize;
        float layoutRotation = uiTransformc.Rotation_Z;
        
        glm::vec3 worldPos;
        worldPos.x = layoutPos.x - UISystem::VirtualResolutionX * 0.5f + layoutSize.x * 0.5;
        worldPos.y = layoutPos.y - UISystem::VirtualResolutionY * 0.5f + layoutSize.y * 0.5f;
        worldPos.z = transform.GetPos().z;
        
        glm::mat4 worldTransfrom = glm::translate(glm::mat4(1.0f), worldPos)
        * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
        * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});

        
        glm::vec4 color = button.NormalColor;
        if (button.IsHovered) color = button.HoverColor;
        if (button.IsPressed) {
            color = button.PressedColor;
            TSO_CORE_INFO("button clicked when rendering");
        }
        if(!e.HasComponent<Renderable>())continue;
        auto& renderCp = e.GetComponent<Renderable>();
        if(!renderCp.material) continue;
        renderCp.material->SetPureColor(color);
//        Renderer2D::DrawQuad(worldTransfrom, color , int(e));
        std::vector<glm::vec2> defaultTextCoord = { {0,0}, {1,0}, {1,1}, {0,1} };
        std::vector<glm::vec2> texCoord = renderCp.subTexture ? renderCp.subTexture->GetTexCoords() : defaultTextCoord;

        MaterialInstanceComponent* matIns = e.HasComponent<MaterialInstanceComponent>() ? &e.GetComponent<MaterialInstanceComponent>() : nullptr;
        Renderer2DMaterial::DrawQuad(worldTransfrom, texCoord, int(e), renderCp.material, matIns);
        
        // 如果按钮上有文字
        if (e.HasComponent<TextComponent>()) {
            auto& textComp = e.GetComponent<TextComponent>();
            if(textComp.TextFont){
                float xOffset = textComp.textParam.offset.x ;/// UISystem::VirtualResolutionX ;
                float yOffset = textComp.textParam.offset.y ;/// UISystem::VirtualResolutionY ;
                glm::mat4 textTransform = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos.x + xOffset , worldPos.y + yOffset  , worldPos.z + 0.01f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
                * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});
//                Renderer2D::DrawString(textComp.TextFont, textTransform, textComp.Text , textComp.textParam , (int)(e));
                Renderer2DMaterial::DrawString(textComp.TextFont, textTransform, textComp.Text, textComp.textParam, (int)e, renderCp.textMat , matIns);

            }
        }
    }
    
    // 渲染输入框
    auto inputView = scene->GetAllEntitiesWith<TransformComponent, InputFieldComponent, TextComponent , UITransformComponent , Renderable>();
    for (auto e : inputView) {
        auto& transform = e.GetComponent<TransformComponent>();
        auto& inputField = e.GetComponent<InputFieldComponent>();
        auto& textComp = e.GetComponent<TextComponent>();
        auto& uiTransformc = e.GetComponent<UITransformComponent>();
        auto& renderc = e.GetComponent<Renderable>();
        MaterialInstanceComponent* matIns = e.HasComponent<MaterialInstanceComponent>() ? &e.GetComponent<MaterialInstanceComponent>() : nullptr;


        
        glm::vec2 layoutPos = uiTransformc.UIpos;
        glm::vec2 layoutSize = uiTransformc.UISize;
        float layoutRotation = uiTransformc.Rotation_Z;
        float xOffset = textComp.textParam.offset.x ;/// UISystem::VirtualResolutionX ;
        float yOffset = textComp.textParam.offset.y ;/// UISystem::VirtualResolutionY ;
        
        glm::vec3 worldPos;
        worldPos.x = layoutPos.x - UISystem::VirtualResolutionX * 0.5f + layoutSize.x * 0.5;
        worldPos.y = layoutPos.y - UISystem::VirtualResolutionY * 0.5f + layoutSize.y * 0.5f;
        worldPos.z = transform.GetPos().z;
        
        glm::mat4 worldTransfrom = glm::translate(glm::mat4(1.0f), worldPos)
        * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
        * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});
        
        // 背景
//        glm::vec4 bgColor = inputField.IsFocused ? inputField.FocusedColor : glm::vec4(0.8f);
//        Renderer2D::DrawQuad(worldTransfrom, bgColor , int(e));
        std::vector<glm::vec2> defaultTextCoord = { {0,0}, {1,0}, {1,1}, {0,1} };
        std::vector<glm::vec2> texCoord = renderc.subTexture ? renderc.subTexture->GetTexCoords() : defaultTextCoord;
        Renderer2DMaterial::DrawQuad(worldTransfrom, texCoord, int(e), renderc.material, matIns);

        
        
        
        // 文本
        if (!textComp.Text.empty() && textComp.TextFont) {
            glm::mat4 textTransform = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos.x + xOffset, worldPos.y +yOffset, worldPos.z + 0.01f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
            * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});
//            Renderer2D::DrawString(textComp.TextFont, textTransform, inputField.Text , textComp.textParam , (int)(e));
            Renderer2DMaterial::DrawString(textComp.TextFont, textTransform, textComp.Text, textComp.textParam, (int)e, renderc.textMat , matIns);

            
        } else if (!inputField.IsFocused && textComp.TextFont) {
            glm::mat4 textTransform = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos.x , worldPos.y , worldPos.z + 0.01f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
            * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});
//            Renderer2D::DrawString(textComp.TextFont, textTransform, inputField.PlaceholderText , textComp.textParam , (int)(e));
            Renderer2DMaterial::DrawString(textComp.TextFont, textTransform, inputField.PlaceholderText, textComp.textParam, (int)e, renderc.textMat , matIns);

        }
        
        // (可选) 绘制光标
        if (inputField.IsFocused) {
            // ... 在文本末尾绘制一个闪烁的竖线 ...
        }
    }
    // 渲染UI image
//    auto imageView = scene->GetAllEntitiesWith<TransformComponent, Renderable , UITransformComponent>();
//    for (auto e : imageView) {
//        auto& transform = e.GetComponent<TransformComponent>();
//        auto& render = e.GetComponent<Renderable>();
//        auto& uiTransformc = e.GetComponent<UITransformComponent>();
//
//        
//        glm::vec2 layoutPos = uiTransformc.UIpos;
//        glm::vec2 layoutSize = uiTransformc.UISize;
//        float layoutRotation = uiTransformc.Rotation_Z;
//        
//        glm::vec3 worldPos;
//        worldPos.x = layoutPos.x - UISystem::VirtualResolutionX * 0.5f + layoutSize.x * 0.5;
//        worldPos.y = layoutPos.y - UISystem::VirtualResolutionY * 0.5f + layoutSize.y * 0.5f;
//        worldPos.z = transform.GetPos().z;
//        
//        glm::mat4 worldTransfrom = glm::translate(glm::mat4(1.0f), worldPos)
//        * glm::rotate(glm::mat4(1.0f), glm::radians(layoutRotation), {0, 0, 1})
//        * glm::scale(glm::mat4(1.0f), {layoutSize.x, layoutSize.y, 1.0f});
//        
//        if(render.type == RenderType::PureColor){
//            Renderer2D::DrawQuad(worldTransfrom,render.m_Color , (int)3);
//        }
//        else{
//            if(render.isSubtexture){
//                Renderer2D::DrawQuad(worldTransfrom,render.subTexture , (int)3);
//            }
//            else{
//                if(render.subTexture && render.subTexture->GetTexture())
//                    Renderer2D::DrawQuad(worldTransfrom,render.subTexture->GetTexture() , (int)3);
//            }
//        }
//    }

    }
}
