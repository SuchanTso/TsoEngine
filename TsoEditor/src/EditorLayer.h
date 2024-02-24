#pragma once

#include "TSO.h"
#include "glm/glm.hpp"
#include "Tso/Core/TimeStep.h"
#include "Tso/Renderer/OrthographicCameraController.h"
#include "Tso/Renderer/Renderer2D.h"
#include "Tso/Renderer/FrameBuffer.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Entity.h"
#include "panel/SceneHierarchyPanel.h"

namespace Tso {
    class Entity;
	class EditorLayer : public Layer {
        struct MovaData {
            glm::vec2 originPos;
            glm::vec2 targetPos;
            float startTime;

        };

    public:
        EditorLayer();
        ~EditorLayer() = default;

        virtual void OnImGuiRender() override;

        virtual void OnUpdate(TimeStep ts)override;

        virtual void OnEvent(Event& event)override;

    private:

        bool OnMouseButton(MouseButtonPressedEvent& e);

        bool OnMouseMove(MouseMovedEvent& e);

        std::string LoadScene();

        void CopyEntity(const Entity& entity);

        void SaveScene();

        std::string SaveSceneAs();

    private:

        glm::vec3 m_TrianglePos;
        Ref<Shader> m_Shader;
        Ref<ShaderLibrary> m_ShaderLibrary;
        Ref<Texture2D> m_Texture, m_TileTexture;
        Ref<SubTexture2D> m_subTexture, m_sub1;
        Ref<FrameBuffer> m_FrameBuffer;
        Ref<Scene>      m_Scene;
        SceneHierarchyPanel m_Panel;
        Entity m_CameraEntity;
        Entity m_HoveredEntity;
        Entity m_CopyEntity;

        float m_MoveSpeed = 1.0f;
        bool m_LpMovable = false;
        float m_Time = 0.0;
        MovaData m_MoveData;
        std::string m_ScenePath = "";
        bool m_StartScene = false;

        float m_MouseX = 0.f, m_MouseY = 0.f;
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        glm::vec2 m_ViewportBounds[2];


        glm::vec2 m_ViewportSize = { 720.0 , 1280.0 };
};

}
