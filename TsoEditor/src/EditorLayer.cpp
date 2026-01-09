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
#include "Tso/Project/Project.h"
#include "Tso/Scripting/ScriptingEngine.h"
#include "Tso/Network/NetworkEngine.h"
#include "Tso/Renderer/ViewportManager.h"
#include "Tso/Renderer/Renderer2DMaterial.h"

namespace Tso {
    EditorLayer::EditorLayer()
        :Layer("EditorLayer")
    {
        Renderer2DMaterial::Init();
//        Renderer2D::Init();
        m_Scene = std::make_shared<Scene>();
        m_Panel.SetContext(m_Scene);

        FrameBufferInfo info ;
        info.width = (uint32_t)m_GameViewSize.x;
        info.height = (uint32_t)m_GameViewSize.y;
        info.format = {RGBA8 , RED_INTEGER , DEPTH24_STENCIL8};
        m_FrameBuffer = FrameBuffer::Create(info);
        m_CameraEntity = CreateRef<Entity>(m_Scene->CreateEntity("SceneCamera"));
        m_CameraEntity->AddComponent<CameraComponent>();
        m_Scene->SetSceneCamera(*m_CameraEntity);//TODO: there is a camera stuff that camera should not be rendered on game view while scene view does
        NetWorkEngine::RegistryScene(m_Scene);
        
        std::string ini_path = Project::GetResourcePath() + "assets/imgui.ini";
        ImGui::LoadIniSettingsFromDisk(ini_path.c_str());
        
    }

void EditorLayer::DrawStartScreen()
{
    // 使用全屏窗口标志
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // >>> 修改点：去掉了 ImGuiWindowFlags_NoBackground <<<
    // 这样窗口会有背景色，覆盖掉之前的界面残留
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    // 这里的 Begin 会绘制一个全屏的深色矩形背景
    ImGui::Begin("StartScreenBackground", nullptr, window_flags);
    ImGui::PopStyleVar();

    // 在背景之上绘制一个居中的模态窗口风格的面板
    ImVec2 center = viewport->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300));

    ImGuiWindowFlags panel_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
    
    if (ImGui::Begin("Welcome to TsoEngine", nullptr, panel_flags))
    {
        float windowWidth = ImGui::GetWindowSize().x;
        float textWidth = ImGui::CalcTextSize("Project Manager").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("Project Manager");
        
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        float buttonWidth = 200.0f;
        float buttonHeight = 40.0f;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("New Project", ImVec2(buttonWidth, buttonHeight))) {
            NewProject();
            m_Project = Project::GetActive();
        }

        ImGui::Spacing();
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Load Project", ImVec2(buttonWidth, buttonHeight))) {
            OpenProject();
            m_Project = Project::GetActive();
        }
        
        ImGui::Spacing();
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
        if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
             Application::Get().OnClose();
        }

        ImGui::End();
    }

    ImGui::End(); // End StartScreenBackground
}


    void EditorLayer::OnImGuiRender(){
        if (!Project::GetActive())
            {
                DrawStartScreen();
            }
            else
            {
                DrawEditorInterface();
            }
    }

void EditorLayer::DrawEditorInterface(){
    
    
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
        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("New Project", NULL, false)) {
                NewProject();
                m_Project = Project::GetActive();
            }
            if (ImGui::MenuItem("Load Project", NULL, false)) {
                OpenProject();
                m_Project = Project::GetActive();
            }
            if (ImGui::MenuItem("Save Project", NULL, false , m_Project != nullptr && !m_ScenePath.empty())) {
                SaveProject();
            }
            if (ImGui::MenuItem("Close Project")) {
                // 关闭当前项目
                if (Project::GetActive()) {
                    // 或者
                    m_Project = nullptr;
                    // 还需要重置 Scene
                    m_Scene = std::make_shared<Scene>();
                    m_Panel.SetContext(m_Scene);
                    
                    // 重置 Project 单例（根据你的 Project 类实现）
                    // 这一步很关键，否则下一帧还是会进入编辑器界面
                     Project::CloseActive();
                }
            }
                        
            if (ImGui::MenuItem("Exit Engine")) {
                Application::Get().OnClose();
            }
        ImGui::EndMenu();

        }
        if (ImGui::BeginMenu("Scene"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.

                if (ImGui::MenuItem("New", NULL, false , m_Project != nullptr)) {
                    m_Scene.reset();
                    m_Scene = std::make_shared<Scene>();
                    m_Panel.SetContext(m_Scene);
                }

                if (ImGui::MenuItem("Save", "Ctrl + S" , false , m_Project != nullptr)) {
                    SaveScene();
                }

                if (ImGui::MenuItem("Save As..", "Ctrl + S" , false, m_Project != nullptr)) {
                    SaveSceneAs();
                }

                if (ImGui::MenuItem("Load", "Ctrl + L" , false, m_Project != nullptr)) {
                    m_ScenePath = LoadScene();
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

        if (ImGui::Button("connect")) {

            /*NetWorkEngine::TestSend("127.0.0.1", 6000, "this is the first message");*/
            if (NetWorkEngine::Connect("127.0.0.1", 6000)) {
                TSO_INFO("connect sucussfully");
            }
        }
        if (ImGui::Button("disconnect")) {
            /*NetWorkEngine::TestSend("127.0.0.1", 6000, "this is the first message");*/
            if (NetWorkEngine::DisConnect()) {
                TSO_INFO("disconnect sucussfully");
            }
            else {
                TSO_ERROR("unable to disconnect");
            }
        }
        if (Project::GetActive() && ImGui::Button("reload")) {
            m_Scene->OnSceneStop();
            std::filesystem::path resourcePath = std::filesystem::path(Project::GetResourcePath());
            ScriptingEngine::LoadAllScripts((resourcePath / Project::GetActive()->GetConfig().ScriptModulePath).string() , false);
            m_Scene->OnScenePlay();
        }

        ImGui::End();



    ImGui::Begin("scene view");
           
           m_ViewportFocused = ImGui::IsWindowFocused();
           m_ViewportHovered = ImGui::IsWindowHovered();
           
           // 1. 获取当前这一帧的可用尺寸
           auto sceneViewContentSize = ImGui::GetContentRegionAvail();
           
           // 2. 判断尺寸是否发生变化
           bool sceneViewSizeChanged = (sceneViewContentSize.x > 0.0f && sceneViewContentSize.y > 0.0f &&
                                       (sceneViewContentSize.x != m_SceneVeiwSize.x || sceneViewContentSize.y != m_SceneVeiwSize.y));

           // 3. 如果尺寸变了，先更新一下成员变量记录
           if (sceneViewSizeChanged) {
               m_SceneVeiwSize = { sceneViewContentSize.x, sceneViewContentSize.y };
           }

           // 4. 处理焦点逻辑
            bool focusChanged = (m_Focus != FocusWindow::Sceneview);
            if (m_ViewportFocused) {
               // 判断是否是刚刚切换到这个窗口
               
               // 更新状态
               m_Focus = FocusWindow::Sceneview;
               m_Scene->SetUseSceneCamera(true);
            }
            if (sceneViewSizeChanged || focusChanged) {
                if(m_ViewportFocused){
                    TSO_CORE_INFO("SceneView Update: Resize to [{}, {}]", m_SceneVeiwSize.x, m_SceneVeiwSize.y);
                    // 1. Resize Framebuffer
                    m_FrameBuffer->Resize((uint32_t)m_SceneVeiwSize.x, (uint32_t)m_SceneVeiwSize.y);
                    // 3. Set GL Viewport
                    RenderCommand::SetViewPort(0, 0, (uint32_t)m_SceneVeiwSize.x, (uint32_t)m_SceneVeiwSize.y);
                    ViewportManager::SetViewportInfo({m_ViewportBounds[0].x , 0.f}, m_SceneVeiwSize);
                }
                // 2. Resize Editor Camera
                if(m_CameraEntity->HasComponent<CameraComponent>()){
                    m_CameraEntity->GetComponent<CameraComponent>().m_Camera.SetViewportSize((uint32_t)m_SceneVeiwSize.x, (uint32_t)m_SceneVeiwSize.y);
                }
            }
           
           // Input Bounds 处理 (保持原样)
           auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
           auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
           auto viewportOffset = ImGui::GetWindowPos();
           m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
           m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
           Input::SetViewportBound(m_ViewportBounds[0].x, m_ViewportBounds[0].y , m_ViewportBounds[1].x, m_ViewportBounds[1].y);
           
           Application::Get().GetGUILayer()->BlockEvents(!(m_GameViewFocused || m_ViewportFocused));

           uint32_t sceneTexId = m_FrameBuffer->GetColorAttachment(0);
           ImGui::Image((void*)(uintptr_t)sceneTexId, ImVec2{ m_SceneVeiwSize.x , m_SceneVeiwSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
           
           ImGui::End();


           // ==========================================================
           // Game View
           // ==========================================================
           ImGui::Begin("game view");

           m_GameViewFocused = ImGui::IsWindowFocused();
           m_GameViewHovered = ImGui::IsWindowHovered();
           
           // 1. 获取尺寸
           auto gameViewContentSize = ImGui::GetContentRegionAvail();
           
           // 2. 判断变化
           bool gameViewSizeChanged = (gameViewContentSize.x > 0.0f && gameViewContentSize.y > 0.0f &&
                                      (gameViewContentSize.x != m_GameViewSize.x || gameViewContentSize.y != m_GameViewSize.y));

           // 3. 更新记录
           if (gameViewSizeChanged) {
               m_GameViewSize = { gameViewContentSize.x, gameViewContentSize.y };
           }

           // 4. 处理焦点
            bool game_focusChanged = (m_Focus != FocusWindow::GameView);
            if (m_GameViewFocused) {
               m_Focus = FocusWindow::GameView;
               m_Scene->SetUseSceneCamera(false);
           }
            if (gameViewSizeChanged || game_focusChanged) {
                if(m_GameViewFocused){
                    TSO_CORE_INFO("GameView Update: Resize to [{}, {}]", m_GameViewSize.x, m_GameViewSize.y);
                    m_FrameBuffer->Resize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
                    RenderCommand::SetViewPort(0, 0, (uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
                    ViewportManager::SetViewportInfo({viewportMinRegion.x , viewportMinRegion.y}, m_GameViewSize); // 注意这里的 minRegion 变量名可能需要对应修改为 GameView 的
                }
                // 设置 Runtime Camera 的长宽比
                if (m_Scene->GetMainCamera()) {
                    m_Scene->GetMainCamera()->SetViewportSize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
                }
                
            }
           
           // Bounds 处理 (Game View 特有的)
           auto gameViewMinRegion = ImGui::GetWindowContentRegionMin();
           auto gameViewMaxRegion = ImGui::GetWindowContentRegionMax();
           auto gameViewOffset = ImGui::GetWindowPos();
           m_GameViewBounds[0] = { gameViewMinRegion.x + gameViewOffset.x, gameViewMinRegion.y + gameViewOffset.y };
           m_GameViewBounds[1] = { gameViewMaxRegion.x + gameViewOffset.x, gameViewMaxRegion.y + gameViewOffset.y };
           
           // 这里的 BlockEvents 可以去掉，因为上面 SceneView 已经做过一次了，或者保留做冗余检查
           // Application::Get().GetGUILayer()->BlockEvents(...);

           uint32_t gameTexId = m_FrameBuffer->GetColorAttachment(0);
           ImGui::Image((void*)(uintptr_t)gameTexId, ImVec2{ m_GameViewSize.x , m_GameViewSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
           
           ImGui::End();
}
    m_Panel.OnGuiRender();
    ImGui::End();
}
    



    void EditorLayer::OnUpdate(TimeStep ts)
    {

        Renderer2D::ResetStat();

        m_FrameBuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
        RenderCommand::Clear();
        m_FrameBuffer->ClearAttachment(1, -1);

        m_Scene->OnUpdate(ts);
        
        auto [mx , my] = ImGui::GetMousePos();
//        TSO_INFO("GUI pos [{},{}] , Input pos [{},{}]" , mx , my , Input::GetMouseX() , Input::GetMouseY());
//        TSO_INFO("viewportbound [{} , {}] , [{} , {}]" , m_ViewportBounds[0].x , m_ViewportBounds[0].y , m_ViewportBounds[1].x , m_ViewportBounds[1].y);

        mx -= m_ViewportBounds[0].x ;
        my -= m_ViewportBounds[0].y;
        
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
//        TSO_INFO("GUI pos [{},{}]" , mx , my);
        int mouseX = (int)mx;
        int mouseY = (int)my;
        if(m_ViewportFocused && mouseX > 0 && mouseY > 0 && m_Scene){
            int pixelData = m_FrameBuffer->ReadOnePixel(1, mouseX, mouseY);
//            TSO_CORE_INFO("read pixel = {}" , pixelData);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData , m_Scene.get());
        }

        m_FrameBuffer->UnBind();

        if (Input::IsKeyPressed(TSO_KEY_LEFT_CONTROL) && Input::IsKeyPressed(TSO_KEY_C)) {
            //copy entity
            if (m_HoveredEntity.HasComponent<IDComponent>()) {
                m_CopyEntity = m_HoveredEntity;
            }
        }

        if (Input::IsKeyPressed(TSO_KEY_LEFT_CONTROL) && Input::IsKeyPressed(TSO_KEY_V)) {
            //paste entity
            if (m_CopyEntity.HasComponent<IDComponent>()) {
                Entity newEntity = m_Scene->CopyEntity(m_CopyEntity);
                glm::vec3 posOffset = glm::vec3(0.2f, 0.2f, 0.0f);
                auto& comp = m_CopyEntity.GetComponent<TransformComponent>();
                auto& transComp = newEntity.GetComponent<TransformComponent>();
                transComp.m_Translation = comp.m_Translation + posOffset;
                m_CopyEntity = Entity();
                m_Panel.SetSelectedEntity(m_CopyEntity);
            }
        }

    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButton));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMove));

    }

    bool EditorLayer::OnMouseButton(MouseButtonPressedEvent& e) {

        if (e.GetButton() == TSO_MOUSE_BUTTON_1) {
            if(m_ViewportFocused && m_ViewportHovered){
                //pick entity
                m_Panel.SetSelectedEntity(m_HoveredEntity);
                return true;
            }
            
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
        return LoadScene(ScenePath);
    }

    std::string EditorLayer::LoadScene(const std::filesystem::path& scenePath)
    {
        if (!scenePath.empty()) {
            if (m_Scene != nullptr) {
                m_Scene.reset();
                m_Scene = std::make_shared<Scene>();
                
                m_Panel.SetContext(m_Scene);
                NetWorkEngine::SetContext(m_Scene);
            }
            Seriealizer seriealizer(m_Scene.get());
            seriealizer.DeseriealizeScene(scenePath.string());
            auto sceneCamera = m_Scene->GetSceneCamera();
            if(sceneCamera != nullptr){
                m_CameraEntity = sceneCamera;
            }
            else{
                m_CameraEntity = CreateRef<Entity>(m_Scene->CreateEntity("SceneCamera"));
                m_CameraEntity->AddComponent<CameraComponent>();
                m_Scene->SetSceneCamera(*m_CameraEntity);
            }
            
        }
        return scenePath.string();
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

    void EditorLayer::NewProject()
    {
        Project::New();
        SaveProject();
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        if (Project::LoadProject(path))
        {
            //ScriptingEngine::Init();

            std::filesystem::path startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().FirstScene);
            m_ScenePath = LoadScene(startScenePath);
            //m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
            std::filesystem::path resourcePath = std::filesystem::path(Project::GetResourcePath());
            ScriptingEngine::LoadAllScripts((resourcePath / Project::GetActive()->GetConfig().ScriptModulePath).string() , false);

        }
    }

    void EditorLayer::SaveProject()
    {
        auto& projPath = Project::GetActive()->GetProjectDirectory();
        if (projPath.empty()) {
            projPath = FileDialogs::SaveFile("Tso Project(*.tproj)\0 * .tproj\0");
        }
        auto projDir = projPath;
        if(m_ScenePath.empty()){
            auto rootPath = std::filesystem::path(projPath).parent_path();
            m_ScenePath = rootPath.string() + "/newScene.teScene";
        }
        SaveScene();
        Project::GetActive()->GetConfig().FirstScene = std::filesystem::path(m_ScenePath).lexically_relative(projDir);
        Project::SaveActive(projPath);
    }

    bool EditorLayer::OpenProject()
    {
        auto projPath = FileDialogs::OpenFile("Tso Project(*.tproj)\0 * .tproj\0");
        if (projPath.empty()) {
            return false;
        }
        OpenProject(projPath);
        return true;
    }

    bool EditorLayer::LoadProject(const std::filesystem::path& path)
    {
        std::string filepath = FileDialogs::OpenFile("Tso Project (*.tproj)\0*.tproj\0");
        if (filepath.empty())
            return false;

        OpenProject(filepath);
        return true;
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
