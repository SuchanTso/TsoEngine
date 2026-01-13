#pragma once
#include "TSO.h"
#include "Tso/Core/Layer.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Project/Project.h"
#include "Tso/Core/VirtualFileSystem.h"

namespace Tso {

    class RuntimeLayer : public Layer {
    public:
        RuntimeLayer() : Layer("Runtime") {}

        void OnAttach() override ;

        void OnDetach() override {
            if (m_Scene) m_Scene->OnSceneStop();
            VirtualFileSystem::Shutdown();
        }
        
        virtual void OnEvent(Event& e)override;

        void OnUpdate(TimeStep ts) override;
        
        // Runtime 不需要 ImGui，或者只留极少的 Debug
        void OnImGuiRender() override {}
    private:
        bool OnWindowResize(WindowResizeEvent& e);
    private:
        Ref<Project> m_Project;
        Ref<Scene> m_Scene;
        unsigned int m_WindowPosX = 0;
        unsigned int m_WindowPosY = 0;
        unsigned int m_WindowWidth = 0;
        unsigned int m_WindowHeight = 0;
    };
}
