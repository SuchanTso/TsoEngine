#pragma once
#include "glm/glm.hpp"

namespace Tso {

    class ViewportManager {
    public:
        /**
         * @brief 更新当前的视口信息。
         * @param position 视口左上角在窗口坐标系中的位置 (像素)。
         * @param size 视口的宽度和高度 (像素)。
         */
        static void SetViewportInfo(const glm::vec2& position, const glm::vec2& size);

        /**
         * @brief 获取视口在窗口中的左上角位置。
         * @return glm::vec2 视口位置。
         */
        static glm::vec2 GetPosition();

        /**
         * @brief 获取视口的尺寸。
         * @return glm::vec2 视口宽度和高度。
         */
        static glm::vec2 GetSize();
        
        /**
         * @brief 获取视口的宽度。
         * @return float 视口宽度。
         */
        static float GetWidth();
        
        /**
         * @brief 获取视口的高度。
         * @return float 视口高度。
         */
        static float GetHeight();

        /**
         * @brief 检查鼠标指针当前是否悬停在游戏视口区域内。
         * @return bool 如果鼠标在视口内，则为 true。
         */
        static bool IsMouseOver();

        /**
         * @brief 将窗口空间的坐标转换为视口空间的坐标。
         * @param windowCoords 鼠标在窗口坐标系中的位置 (左上角为0,0)。
         * @return glm::vec2 鼠标相对于视口左上角的位置。
         */
        static glm::vec2 WindowToViewport(const glm::vec2& windowCoords);
        
        /**
         * @brief 将视口空间的坐标转换为UI世界坐标系中的坐标。
         * @param viewportCoords 鼠标相对于视口左上角的位置。
         * @param virtualResolution UI的虚拟分辨率 (e.g., {1920, 1080})。
         * @return glm::vec2 鼠标在UI世界坐标系中的位置 (中心为0,0)。
         */
        static glm::vec2 ViewportToUIWorld(const glm::vec2& viewportCoords, const glm::vec2& virtualResolution);

    private:
        // 视口左上角在主窗口中的像素位置
        static glm::vec2 s_Position;
        // 视口的像素尺寸
        static glm::vec2 s_Size;
    };

} // namespace Tso
