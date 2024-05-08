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
    class Project;
	class EditorLayer : public Layer {
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

        std::string LoadScene(const std::filesystem::path& scenePath);

        void CopyEntity(const Entity& entity);

        void SaveScene();

        void NewProject();

        bool LoadProject(const std::filesystem::path& path);

        void OpenProject(const std::filesystem::path& path);

        void SaveProject();

        bool OpenProject();

        std::string SaveSceneAs();

    private:

        Ref<FrameBuffer> m_FrameBuffer;
        Ref<Scene>      m_Scene;
        Ref<Project> m_Project = nullptr;
        SceneHierarchyPanel m_Panel;
        Entity m_CameraEntity;
        Entity m_HoveredEntity;
        Entity m_CopyEntity;

        std::string m_ScenePath = "";
        bool m_StartScene = false;

        float m_MouseX = 0.f, m_MouseY = 0.f;
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        bool m_UpdateViewportSize = false;
        glm::vec2 m_ViewportBounds[2];


        glm::vec2 m_ViewportSize = { 720.0 , 1280.0 };
};

}
