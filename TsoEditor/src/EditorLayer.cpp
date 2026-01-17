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
#include "Tso/Project/Packer.h"
#include "imgui_internal.h"

namespace Utils{


std::vector<std::filesystem::path> GetAllExtensionFiles(const std::filesystem::path& dir , const std::string& ext){
    std::vector<std::filesystem::path> res;
    if (std::filesystem::exists(dir)) {
        for (auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ext) {
                res.push_back(entry.path());
            }
        }
    }
    return res;
}

}

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
        m_IconStop = Texture2D::Create(Project::GetResourcePath() + "assets/stopIcon.png");
        m_IconPlay = Texture2D::Create(Project::GetResourcePath() + "assets/startIcon.png");
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
                if (Application::Get().IsShowClosePrompt()) {
                    ImGui::OpenPopup("Save Project?");
                    Application::Get().SetShowClosePrompt(false); // 只触发一次 OpenPopup
                }
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::BeginPopupModal("Save Project?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Do you want to save the project before closing?");
                    ImGui::Separator();

                    // 1. Save & Quit
                    if (ImGui::Button("Save", ImVec2(120, 0))) {
                        SaveProject(); // 调用你的保存函数
                        SaveScene();
                        
                        Application::Get().ForceClose(); // 设置 m_BlockClose=true 并 m_Running=false
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();
                    
                    // 2. Don't Save & Quit
                    if (ImGui::Button("Don't Save", ImVec2(120, 0))) {
                        Application::Get().ForceClose();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();

                    // 3. Cancel
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                        // 什么都不做，继续运行
                    }

                    ImGui::EndPopup();
                }
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
            if (ImGui::MenuItem("Build & Export Game...")) {
                // 1. 让用户选择导出目录
                // 这里用 SaveDialog 模拟选择目录，或者实现一个 SelectFolder
                // 假设用户选择了 "D:/Builds/MyGame/MyGame.exe"
                std::string savePath = FileDialogs::SaveFile("Executable (*.exec)\0*.exec\0");
                
                if (!savePath.empty()) {
                    std::filesystem::path exePath(savePath);
                    std::filesystem::path buildDir = exePath.parent_path();
                    
                    // 2. 复制 Game Engine Runtime 可执行文件
                    // 你需要预先编译好一个 Release/Dist 版本的 exe 放在某个已知位置
                    // std::filesystem::copy_file("Bin/Dist/Runtime.exe", exePath);
                    
                    // 3. 执行打包：生成 Game.pak
                    std::filesystem::path projectRoot = Project::GetProjectDirectory();
                    std::filesystem::path pakPath = buildDir / "Game.pak";
                    
                    Packer::Pack(projectRoot, pakPath);
                    
                    // 4. 复制/生成配置 (Game.tproj)
                    // ...
                    
                    TSO_CORE_INFO("Export finished successfully to {0}", buildDir.string());
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
    auto stat = Renderer2DMaterial::GetStats();
    ImGui::Text("DrawCalls : %d ", stat.DrawCalls);
    ImGui::Text("QuadsCount : %d ", stat.QuadCount);
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    
    // 2. 设置按钮颜色：默认透明，悬停/按下时显示半透明高亮
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
    const auto& buttonActive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);
    // 3. 开始绘制窗口
    ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    

    // 4. 计算居中位置
    // 假设图标大小为窗口高度的大部分
    float size = ImGui::GetWindowHeight() - 4.0f;
    
    // 我们有 Play 按钮 (Icon) + Connect + Disconnect + Reload (Text)
    // 估算一下总宽度，以便居中 (图标宽度 + 3个文字按钮宽度 + 间距)
    // 这里为了简单，假设文字按钮宽 80，图标宽 size
    float textBtnWidth = 80.0f;
    int textBtnCount = 1;
    if (!Project::GetActive()) textBtnCount = 0; // 如果没项目，Reload不显示

    float totalWidth = size + (textBtnCount * textBtnWidth) + (textBtnCount * ImGui::GetStyle().ItemSpacing.x);
    
    // 设置光标 X 位置实现居中
    float cursorX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
    if (cursorX > 0.0f) ImGui::SetCursorPosX(cursorX + ImGui::GetCursorPosX());

    // 5. 绘制 Play/Stop 按钮 (ImageButton)
    {
        // 根据状态选择图标
        Ref<Texture2D> icon = m_StartScene ? m_IconStop : m_IconPlay;
        
        // 预防资源未加载时的空指针保护
        uint64_t texID = icon ? (uint64_t)icon->GetTextureID() : 0;
        
        // 绘制 ImageButton
        // 注意：UV 使用 0,1 -> 1,0 是为了适应 OpenGL 坐标系翻转，如果你的图标倒了，请改回 0,0 -> 1,1
        if (ImGui::ImageButton((ImTextureID)texID, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0))
        {
            m_StartScene = !m_StartScene;
            if (m_StartScene) m_Scene->OnScenePlay();
            else m_Scene->OnSceneStop();
        }
    }

    // 6. 绘制其他功能按钮 (保持文字风格，但享受透明背景样式)
    // 这里的文字按钮因为背景透明，看起来就像菜单项，很整洁
    
//    ImGui::SameLine();
//    if (ImGui::Button("Connect", ImVec2(textBtnWidth, size))) {
//        if (NetWorkEngine::Connect("127.0.0.1", 6000)) TSO_INFO("connect sucussfully");
//    }
//
//    ImGui::SameLine();
//    if (ImGui::Button("Disconnect", ImVec2(textBtnWidth, size))) {
//        if (NetWorkEngine::DisConnect()) TSO_INFO("disconnect sucussfully");
//        else TSO_ERROR("unable to disconnect");
//    }
    
    if (Project::GetActive()) {
        ImGui::SameLine();
        if (ImGui::Button("Reload", ImVec2(textBtnWidth, size))) {
            m_Scene->OnSceneStop();
            m_StartScene = false;
            std::filesystem::path resourcePath = std::filesystem::path(Project::GetProjectDirectory());
            ScriptingEngine::LoadAllScripts((resourcePath / Project::GetActive()->GetConfig().ScriptModulePath).string() , false);
        }
    }

    // 7. 恢复样式
    ImGui::PopStyleVar(2);   // Padding, Spacing
    ImGui::PopStyleColor(3); // Button, Hovered, Active
    ImGui::End();

//        ImGui::Begin("RenderInfo");
//
//        ImGui::Text("Render2DInfo");
//
//        auto stat = Renderer2DMaterial::GetStats();
//
//        ImGui::Text("DrawCalls : %d ", stat.DrawCalls);
//        ImGui::Text("QuadsCount : %d ", stat.QuadCount);
////        ImGui::Text("QuadVertices : %d", stat.GetTotalVertexCount());
////        ImGui::Text("QuadIndices : %d", stat.GetTotalIndexCount());
//
//        if (ImGui::Button("Play")) {
//            m_StartScene = !m_StartScene;
//            if (m_StartScene) {
//                m_Scene->OnScenePlay();
//            }
//            else {
//                m_Scene->OnSceneStop();
//            }
//        }
//        ImGui::SameLine();
//        ImGui::Text("%s", m_StartScene ? "play" : "stop");
//
//        if (ImGui::Button("connect")) {
//
//            /*NetWorkEngine::TestSend("127.0.0.1", 6000, "this is the first message");*/
//            if (NetWorkEngine::Connect("127.0.0.1", 6000)) {
//                TSO_INFO("connect sucussfully");
//            }
//        }
//        if (ImGui::Button("disconnect")) {
//            /*NetWorkEngine::TestSend("127.0.0.1", 6000, "this is the first message");*/
//            if (NetWorkEngine::DisConnect()) {
//                TSO_INFO("disconnect sucussfully");
//            }
//            else {
//                TSO_ERROR("unable to disconnect");
//            }
//        }
//        if (Project::GetActive() && ImGui::Button("reload")) {
//            m_Scene->OnSceneStop();
//            std::filesystem::path resourcePath = std::filesystem::path(Project::GetProjectDirectory());
//            ScriptingEngine::LoadAllScripts((resourcePath / Project::GetActive()->GetConfig().ScriptModulePath).string() , false);
//            m_Scene->OnScenePlay();
//        }
//
//        ImGui::End();



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
                ViewportManager::SetViewportInfo({m_ViewportBounds[0].x , 0.f}, m_SceneVeiwSize);
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
                ViewportManager::SetViewportInfo({viewportMinRegion.x , viewportMinRegion.y}, m_GameViewSize);
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

        Renderer2DMaterial::ResetStats();

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
            Project::SetProjectPath(path.string());
            auto scenes = Utils::GetAllExtensionFiles(Project::GetProjectDirectory(), ".teScene");
            Project::SetSceneAsset(scenes);
            
        }
    }


    void EditorLayer::SaveProject(){
        auto project = Project::GetActive();
        std::string projPathStr = project->GetProjectDirectory().string(); // 假设 GetProjectDirectory 返回 path
        bool newProj = false;
        // 1. 如果是新建项目（路径为空），弹出保存对话框
        if (projPathStr.empty()) {
            projPathStr = FileDialogs::SaveFile("Tso Project (*.tproj)\0*.tproj\0");
            if (projPathStr.empty()) return; // 用户取消
            newProj = true;
            // 确保后缀正确
            if (std::filesystem::path(projPathStr).extension() != ".tproj") {
                projPathStr += ".tproj";
            }
        }

        std::filesystem::path projFilePath(projPathStr);
        std::filesystem::path projRootDir = projFilePath;
        if(newProj){
            projRootDir = projRootDir.parent_path();
        }

        // =========================================================
        // 2. 创建目录结构 (核心修改)
        // =========================================================
        // std::filesystem::create_directories 会递归创建目录，且如果已存在不会报错
        // 跨平台兼容 (Windows/Mac/Linux)
        
        std::filesystem::create_directories(projRootDir / "Assets");
        std::filesystem::create_directories(projRootDir / "Assets" / "Shader");
        std::filesystem::create_directories(projRootDir / "Assets" / "Animations");
        std::filesystem::create_directories(projRootDir / "Assets" / "Sprites");
        std::filesystem::create_directories(projRootDir / "Assets" / "Scripts");
        std::filesystem::create_directories(projRootDir / "Assets" / "Material");

        // =========================================================

        // 3. 处理场景保存
        if (m_ScenePath.empty()) {
            // 如果是新项目，默认创建一个场景在 Assets/Scene 下 (如果你想规范化的话)
            // 或者直接在根目录
            // 建议放在 Assets/Scenes 下，或者根目录
            // 这里沿用你的逻辑：根目录下
            m_ScenePath = (projRootDir / "newScene.teScene").string();
        }
        
        SaveScene(); // 保存当前场景文件

        // 4. 更新 Project 配置
        // 计算 StartScene 相对于 Project 文件的相对路径
        std::filesystem::path sceneAbsPath(m_ScenePath);
        std::filesystem::path sceneRelPath = std::filesystem::relative(sceneAbsPath, projRootDir);
        
        project->GetConfig().FirstScene = sceneRelPath;
        
        // 5. 保存 Project 文件
        Project::SaveActive(projFilePath.string());
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
