#include "SandBox2D.h"
#include "Tso/Scene/Seriealizer.h"
#include "Tso/Scripting/ScriptingEngine.h"
#include "Tso/Renderer/ViewportManager.h"
#include "Tso/Renderer/RenderCommand.h"
#include "Tso/Renderer/Renderer2DMaterial.h"


namespace Tso{
void RuntimeLayer::OnAttach(){
    // 1. 尝试挂载资源包
    // 此时应该与 exe 同级目录
    VirtualFileSystem::Init("/Users/SuchanTso/project/Sandbox/bin/Game.pak");

    // 2. 加载项目配置
    // 我们约定打包时，原来的 .tproj 会被重命名并放在包的根目录，叫 "Game.config" 或类似名字
    // 这里假设我们保留了原始名字或者固定名字
    // 如果 Packer 没有改名，我们需要一种机制知道哪个是 Project 文件
    // 简单方案：约定打包后的配置文件名固定为 "App.tproj"
    m_Project = Project::LoadProject("App.tproj");
    
    if (!m_Project) {
        TSO_CORE_ERROR("Failed to load project config!");
        return;
    }
    Tso::Renderer2DMaterial::Init();


    // 3. 加载启动场景
    auto& config = m_Project->GetConfig();
    std::filesystem::path startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().FirstScene);
    m_Scene = Tso::CreateRef<Scene>();
    Seriealizer seriealizer(m_Scene.get());
    seriealizer.DeseriealizeScene(startScenePath.string());
    //m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
    std::filesystem::path resourcePath = std::filesystem::path(Project::GetResourcePath());
//    ScriptingEngine::LoadAllScripts((resourcePath / Project::GetActive()->GetConfig().ScriptModulePath).string() , false);

    if (m_Scene) {
        // 设置视口大小 (全屏)
        auto& window = Application::Get().GetWindow();
//        m_Scene->OnViewportResize(window.GetWidth(), window.GetHeight());
//        ViewportManager::SetViewportInfo({0.f , 0.f}, {window.GetWidth(), window.GetHeight()});

        // 启动运行时逻辑
        m_Scene->SetUseSceneCamera(false);
        m_Scene->OnScenePlay();
    }
}
void RuntimeLayer::OnUpdate(TimeStep ts) {
    Tso::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
    Tso::RenderCommand::Clear();
    Tso::Application& app = Tso::Application::Get();
    auto windowX = app.GetWindow().GetPosX();
    auto windowY = app.GetWindow().GetPosY();
    auto windowWidth = app.GetWindow().GetWidth();
    auto windowHeight = app.GetWindow().GetHeight();
    if (windowX != m_WindowPosX ||
        windowY != m_WindowPosY ||
        windowWidth != m_WindowWidth ||
        windowHeight != m_WindowHeight) {
        m_WindowPosX = windowX;
        m_WindowPosY = windowY;
        m_WindowWidth = windowWidth;
        m_WindowHeight = windowHeight;
        Tso::ViewportManager::SetViewportInfo({ m_WindowPosX , m_WindowPosY }, { m_WindowWidth , m_WindowHeight });
        Tso::Input::SetViewportBound(m_WindowPosX, m_WindowPosY, m_WindowPosX + m_WindowWidth, m_WindowPosY + m_WindowHeight);
        auto mainCamera = m_Scene->GetMainCamera();
        auto uiCamera = m_Scene->GetUICamera();
        if(mainCamera){
            mainCamera->SetViewportSize(windowWidth, windowHeight);
        }
        if(uiCamera){
            uiCamera->SetViewportSize(windowWidth, windowHeight);
        }
    }

    //TSO_INFO("window pos = {} , {}", app.GetWindow().GetPosX(), app.GetWindow().GetPosY());

    //TSO_INFO("Input pos [{},{}]" , Tso::Input::GetMouseX() , Tso::Input::GetMouseY());
    if (m_Scene) {
        m_Scene->OnUpdate(ts);
    }
//    if (m_Scene) {
//        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
//        RenderCommand::Clear();
//        m_Scene->OnUpdate(ts);
//    }
}

void RuntimeLayer::OnEvent(Event &e){
    Tso::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Tso::WindowResizeEvent>(BIND_EVENT_FN(RuntimeLayer::OnWindowResize));
}

bool RuntimeLayer::OnWindowResize(WindowResizeEvent &e){
    return false;
    if(m_Scene){
        auto mainCamera = m_Scene->GetMainCamera();
        auto uiCamera = m_Scene->GetUICamera();
        //        if(mainCamera){
        //            mainCamera->SetViewportSize(e.GetWidth(), e.GetHeight());
        //        }
        //        if(uiCamera){
        //            uiCamera->SetViewportSize(e.GetWidth(), e.GetHeight());
        //        }
        Tso::Application& app = Tso::Application::Get();
        auto windowX = app.GetWindow().GetPosX();
        auto windowY = app.GetWindow().GetPosY();
        auto windowWidth = app.GetWindow().GetWidth();
        auto windowHeight = app.GetWindow().GetHeight();
        ViewportManager::SetViewportInfo({windowX,windowY}, {windowWidth,windowHeight});
        Input::SetViewportBound(windowX, windowY, windowX + windowWidth, windowY + windowHeight);
    }
    return false;
}
}
