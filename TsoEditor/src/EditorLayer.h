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
        enum class FocusWindow {
            None = 0,
            Sceneview = 1,
            GameView = 2,
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

        std::string LoadScene(const std::filesystem::path& scenePath);

        void CopyEntity(const Entity& entity);

        void SaveScene();

        void NewProject();

        bool LoadProject(const std::filesystem::path& path);

        void OpenProject(const std::filesystem::path& path);

        void SaveProject();

        bool OpenProject();

        std::string SaveSceneAs();
        
        void DrawStartScreen();
        
        void DrawEditorInterface();



    private:

        Ref<FrameBuffer> m_FrameBuffer;
        Ref<Scene>      m_Scene;
        Ref<Project> m_Project = nullptr;
        SceneHierarchyPanel m_Panel;
        Ref<Entity> m_CameraEntity = nullptr;
        Entity m_HoveredEntity;
        Entity m_CopyEntity;

        std::string m_ScenePath = "";
        bool m_StartScene = false;

        float m_MouseX = 0.f, m_MouseY = 0.f;
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;

        bool m_GameViewFocused = false;
        bool m_GameViewHovered = false;

        bool m_UpdateViewportSize = false;
        glm::vec2 m_ViewportBounds[2];
        glm::vec2 m_GameViewBounds[2];


        glm::vec2 m_SceneVeiwSize = { 720.0 , 1280.0 };
        glm::vec2 m_GameViewSize = { 720.0 , 1280.0 };

        FocusWindow m_Focus = FocusWindow::None;
        
        Ref<Texture2D> m_IconStop = nullptr;
        Ref<Texture2D> m_IconPlay = nullptr;


};

}
