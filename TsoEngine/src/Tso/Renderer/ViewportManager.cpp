#include "TPch.h"
#include "ViewportManager.h"
#include "Tso/Core/Input.h" // 需要 Input 系统来获取全局鼠标位置

namespace Tso {

    // 初始化静态成员变量
    glm::vec2 ViewportManager::s_Position = { 0.0f, 0.0f };
    glm::vec2 ViewportManager::s_Size = { 0.0f, 0.0f };

    void ViewportManager::SetViewportInfo(const glm::vec2& position, const glm::vec2& size) {
        s_Position = position;
        s_Size = size;
//        TSO_CORE_INFO("set viewportINfo :[{} , {}] , size:[{} , {}]" , position.x , position.y , size.x , size.y);
    }

    glm::vec2 ViewportManager::GetPosition() {
        return s_Position;
    }

    glm::vec2 ViewportManager::GetSize() {
        return s_Size;
    }
    
    float ViewportManager::GetWidth() {
        return s_Size.x;
    }

    float ViewportManager::GetHeight() {
        return s_Size.y;
    }

    bool ViewportManager::IsMouseOver() {
        // 使用 Input 系统提供的窗口空间鼠标坐标
        auto [mouseX, mouseY] = Input::GetMousePosition();
        
        // 检查点是否在矩形内
        return mouseX >= s_Position.x && mouseX < s_Position.x + s_Size.x &&
               mouseY >= s_Position.y && mouseY < s_Position.y + s_Size.y;
    }

    glm::vec2 ViewportManager::WindowToViewport(const glm::vec2& windowCoords) {
        return { windowCoords.x - s_Position.x, windowCoords.y - s_Position.y };
    }
    
    glm::vec2 ViewportManager::ViewportToUIWorld(const glm::vec2& viewportCoords, const glm::vec2& virtualResolution) {
        if (s_Size.x <= 0 || s_Size.y <= 0) {
            return { 0.0f, 0.0f }; // 避免除以零
        }
        
        // 将 [0, viewportWidth] 映射到 [-virtualWidth/2, virtualWidth/2]
        float uiX = (viewportCoords.x / s_Size.x) * virtualResolution.x - virtualResolution.x / 2.0f;
        // 将 [0, viewportHeight] 映射到 [virtualHeight/2, -virtualHeight/2] (Y轴翻转)
        float uiY = -((viewportCoords.y / s_Size.y) * virtualResolution.y - virtualResolution.y / 2.0f);
        
        return { uiX, uiY };
    }

} // namespace Tso
