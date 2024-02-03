#include "TPch.h"
#include "EditorLayer.h"
#include "imgui.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Entity.h"
#include "Tso/Scene/Component.h"
#include "glm/gtc/type_ptr.hpp"
#include "Tso/Scene/Entity.h"
#include "Tso/Scene/Seriealizer.h"
#include "Tso/Utils/PlatformUtils.h"


namespace Tso {

    static void PrintFunc() {
        TSO_CORE_INFO("hello world from c++ by c#");
    }

    EditorLayer::EditorLayer()
        :Layer("EditorLayer"),
        m_TrianglePos(glm::vec3(0.f))
    {
        m_ShaderLibrary = std::make_shared<ShaderLibrary>();

        m_Shader = m_ShaderLibrary->Load("asset/shader/Shader2D.glsl");
        Renderer2D::Init(m_Shader);
        m_Scene = std::make_shared<Scene>();
        m_Panel.SetContext(m_Scene);

        std::string lp = "asset/lp2.png";

        std::string tileMap = "asset/tilemap_packed.png";


        m_Texture = Texture2D::Create(lp);
        m_TileTexture = Texture2D::Create(tileMap);
        m_subTexture = SubTexture2D::CreateByCoord(m_TileTexture, { 16.0 , 16.0 }, { 2.0 , 3.0 }, { 1.0 , 1.0 });
        m_sub1 = SubTexture2D::CreateByCoord(m_TileTexture, { 16.0 , 16.0 }, { 6.0 , 0.0 }, { 1.0 , 1.0 });

        FrameBufferInfo info ;
        info.width = (uint32_t)m_ViewportSize.x;
        info.height = (uint32_t)m_ViewportSize.y;
        info.format = {RGBA8 , RGBA8 , DEPTH24_STENCIL8};
        //{ (uint32_t)m_ViewportSize.x , (uint32_t)m_ViewportSize.y , false };
        m_FrameBuffer = FrameBuffer::Create(info);

        //Scripting script;
        //auto test = script.LoadCSharpAssembly("../TsoEngine-ScriptCore/Build/TsoEngine-ScriptCore.dll");
        ////script.PrintAssemblyTypes(test);
        //MonoClass* p2 = script.GetClassInAssembly(test, "MyNamespace", "Program");
        //MonoObject* objP = script.CreateInstance(p2);
        //mono_add_internal_call("MyNamespace.Program::PrintString", &PrintFunc);

        //script.CallMethod(p2, objP, "PrintFloatVar");

        
    }


    void EditorLayer::OnImGuiRender()
    {


        //    static bool show = true;
        //    ImGui::ShowDemoWindow(&show);

            // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
            // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
            // In this specific demo, we are not using DockSpaceOverViewport() because:
            // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
            // - we allow the host window to have padding (when opt_padding == true)
            // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
            // TL;DR; this demo is more complicated than what you would normally use.
            // If we removed all the options we are showcasing, this demo would become:
            //     void ShowExampleAppDockSpace()
            //     {
            //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            //     }

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        else
        {
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Editor"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.

                
                if (ImGui::MenuItem("New", NULL, false)) {
                    m_Scene.reset();
                    m_Scene = std::make_shared<Scene>();
                    m_Panel.SetContext(m_Scene);
                }

                if (ImGui::MenuItem("Save", "Ctrl + S")) {
                    SaveScene();
                }

                if (ImGui::MenuItem("Save As..", "Ctrl + S")) {
                    SaveSceneAs();
                }
                
                if (ImGui::MenuItem("Load", "Ctrl + L")) {
                    m_ScenePath = LoadScene();
                }

                if (ImGui::MenuItem("Close", NULL, false)){
                    dockSpaceOpen = false;
                    Application::Get().OnClose();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
{

            ImGui::Begin("RenderInfo");

            ImGui::Text("Render2DInfo");

            auto stat = Renderer2D::GetStat();

            ImGui::Text("DrawCalls : %d ", stat.DrawCalls);
            ImGui::Text("QuadsCount : %d ", stat.QuadCount);
            ImGui::Text("QuadVertices : %d", stat.GetTotalVertexCount());
            ImGui::Text("QuadIndices : %d", stat.GetTotalIndexCount());

            if (ImGui::Button("Play")) {
                m_StartScene = !m_StartScene;
                if (m_StartScene) {
                    m_Scene->OnScenePlay();
                }
                else {
                    m_Scene->OnSceneStop();
                }
            }
            ImGui::SameLine();
            ImGui::Text("%s", m_StartScene ? "play" : "stop");

            ImGui::End();

            ImGui::Begin("Viewport");
            m_ViewportFocused = ImGui::IsWindowFocused();
            Application::Get().GetGUILayer()->BlockEvents(!m_ViewportFocused);

            auto content = ImGui::GetContentRegionAvail();
            if (content.x > 0.f && content.y > 0.f && (content.x != m_ViewportSize.x || content.y != m_ViewportSize.y)) {
                m_ViewportSize = { content.x , content.y };
                m_FrameBuffer->Resize(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
                
//                if(!camera.FixedAspectRatio){
//                    camera.m_Camera.SetViewportSize(m_ViewportSize.x , m_ViewportSize.y);
//                }
            }
            uint32_t fbId = m_FrameBuffer->GetColorAttachment(1);

            ImGui::Image((void*)fbId, ImVec2{ m_ViewportSize.x , m_ViewportSize.y },ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

            ImGui::End();
}
        m_Panel.OnGuiRender();
        ImGui::End();
    }



    void EditorLayer::OnUpdate(TimeStep ts)
    {

        Renderer2D::ResetStat();

        m_FrameBuffer->Bind();
        
        m_Scene->OnUpdate(ts);
        
        m_FrameBuffer->UnBind();

        m_Time += ts.GetSecond();


        /*if (Input::IsKeyPressed(TSO_KEY_SPACE)) {
            m_StartScene = !m_StartScene;
            if (m_StartScene) {
                m_Scene->OnScenePlay();
            }
            else {
                m_Scene->OnSceneStop();
            }
        }*/

    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButton));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMove));

    }

    bool EditorLayer::OnMouseButton(MouseButtonPressedEvent& e) {

        if (e.GetButton() == TSO_MOUSE_BUTTON_1) {

            float width = 1280.f, height = 720.f;

            float asp = width / height;

            float targetX = m_MouseX / width * asp * 4 - 2 * asp;
            float targetY = m_MouseY / height * asp * 2 - asp;
            //TSO_INFO("mouse position = ({0} , {1}) , and caled pos = ({2} , {3})", m_MouseX, m_MouseY, targetX, targetY);


            m_MoveData.startTime = m_Time;
            m_MoveData.targetPos = glm::vec2(targetX, -targetY);
            m_MoveData.originPos = m_TrianglePos;
            m_LpMovable = true;
            return true;
        }
        return false;
    }

    bool EditorLayer::OnMouseMove(MouseMovedEvent& e) {


        m_MouseX = e.GetX();
        m_MouseY = e.GetY();

        return false;
    }

    std::string EditorLayer::LoadScene()
    {
        auto ScenePath = FileDialogs::OpenFile("Tso Scene(*.teScene)\0 * .teScene\0");
        if (!ScenePath.empty()) {
            if (m_Scene != nullptr) {
                m_Scene.reset();
                m_Scene = std::make_shared<Scene>();
                m_Panel.SetContext(m_Scene);
            }
            Seriealizer seriealizer(m_Scene.get());
            seriealizer.DeseriealizeScene(ScenePath);
        }
        return ScenePath;
    }

    void EditorLayer::SaveScene()
    {
        if (m_ScenePath.empty()) {
            m_ScenePath = SaveSceneAs();
        }
        else {
            Seriealizer seriealizer(m_Scene.get());
            seriealizer.SeriealizeScene(m_ScenePath);
        }
    }

    std::string EditorLayer::SaveSceneAs()
    {
        auto savePath = FileDialogs::SaveFile("Tso Scene(*.teScene)\0 * .teScene\0");
        if (!savePath.empty()) {
            Seriealizer seriealizer(m_Scene.get());
            seriealizer.SeriealizeScene(savePath);
        }
        return savePath;
    }

}
