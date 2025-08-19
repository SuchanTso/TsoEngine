//
//  UISystem.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/14.
//

#ifndef UISystem_hpp
#define UISystem_hpp
#pragma once
#include "Tso/Core/UUID.h"
#include "Tso/Scene/Component.h"
namespace Tso{
class Scene;
class SceneCamera;
class UISystem{
public:
    static constexpr uint32_t VirtualResolutionX = 1920;
    static constexpr uint32_t VirtualResolutionY = 1080;
    UISystem() = delete;
    UISystem(Scene* scene);
    ~UISystem() = default;
    void OnUpdate(TimeStep ts);
    void Render(Scene*scene);

    static glm::vec2 ConvertScreenToUIWorld(float mouseX, float mouseY, int screenWidth, int screenHeight);
    static glm::vec4 WorldToScreenRect(TransformComponent& transform);
private:
    void HandleInteraction(Scene*scene);
private:
    Scene* m_Scene;
    UUID m_FocusedInputField = 0; // 当前聚焦的输入框实体ID
    

};
}

#endif /* UISystem_hpp */
