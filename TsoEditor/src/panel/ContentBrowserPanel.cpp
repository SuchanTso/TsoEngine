//
//  ContentBrowserPanel.cpp
//  TsoEditor
//
//  Created by 左斯诚 on 2026/1/15.
//
#include "TPch.h"
#include "ContentBrowserPanel.h"
#include "Tso/Project/Project.h"
#include "Tso/Project/Resource.h"
#include "Tso/Utils/PlatformUtils.h"
#include "Tso/Renderer/Material.h"
#include "Tso/Scripting/ScriptingEngine.h"
#include "SceneHierarchyPanel.h"

namespace Utils{
    void StringReplace(std::string& str, const std::string& from, const std::string& to) {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
}

namespace Tso {

    extern const std::filesystem::path g_AssetPath = "Assets"; // 默认相对路径

    ContentBrowserPanel::ContentBrowserPanel()
    : m_BaseDirectory(Project::GetProjectDirectory() / g_AssetPath)
    {
        // 加载图标 (请确保你有这些 png)
        m_CurrentDirectory = m_BaseDirectory;
        auto resourceDir = Project::GetResourcePath();
        m_DirectoryIcon = Texture2D::Create(resourceDir+ "/assets/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create(resourceDir + "/assets/FileIcon.png");
    }

    void ContentBrowserPanel::SetSceneHierarchyPanel(SceneHierarchyPanel* panel){
        m_SceneHPanel = panel;
    }


    void ContentBrowserPanel::ImportAsset(const std::string& filter) {
            std::string srcPath = FileDialogs::OpenFile(filter.c_str());
            if (!srcPath.empty()) {
                std::filesystem::path src(srcPath);
                std::filesystem::path dest = m_CurrentDirectory / src.filename();
                
                try {
                    // 1. 复制文件
                    if (std::filesystem::exists(dest)) {
                        TSO_CORE_WARN("File already exists: {0}", dest.string());
                        // 这里可以做一个弹窗询问是否覆盖，为了简单先直接覆盖
                    }
                    std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
                    
                    // 2. 触发资源系统的导入 (生成 .meta, 分配 UUID)
                    // 这一步非常重要！否则新文件没有 UUID 无法被引用
                    // 假设 Resource 有这样一个接口，或者你依赖 FileWatcher 自动触发
                    Resource::ImportAsset(dest);
                    
                    TSO_CORE_INFO("Imported asset to: {0}", dest.string());
                } catch (std::filesystem::filesystem_error& e) {
                    TSO_CORE_ERROR("Import failed: {0}", e.what());
                }
            }
        }

void ContentBrowserPanel::DrawCreateScriptPopup(){
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create Script", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char nameBuf[64] = "NewScript";
        ImGui::InputText("Name", nameBuf, sizeof(nameBuf));
        
        ImGui::Text("This will create a basic script");
        ImGui::Separator();
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            std::string script = nameBuf;
            if (!script.empty()) {
                std::string basicScript = R"(
local $ClassName$ = {}
$ClassName$.__index = $ClassName$

-- 构造函数
function $ClassName$:new(uuid_str)
    local instance = setmetatable({}, $ClassName$)    
    return instance
end


function $ClassName$:OnCreate()

end

function $ClassName$:OnUpdate(ts)

end

function $ClassName$:OnDestroy()

end

return $ClassName$

)";
                Utils::StringReplace(basicScript, "$ClassName$", script);
                
                // 路径 B: (推荐) 保存到磁盘文件然后加载
                std::filesystem::path projDir = Project::GetProjectDirectory(); // 获取项目根目录
                std::string fileName = script + ".lua";
                std::filesystem::path filePath = m_CurrentDirectory / fileName;
                // 确保目录存在
                std::filesystem::create_directories(filePath.parent_path());
                std::ofstream out(filePath.string());
                if (out.is_open()) {
                    out << basicScript;
                    out.close();
                    //TODO: stop scene
                    ScriptingEngine::LoadAllScripts(m_CurrentDirectory.string(), false);
                    Resource::ImportAsset(filePath);
                } else {
                    TSO_CORE_ERROR("Failed to create script file: {0}", filePath.string());
                }

            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ContentBrowserPanel::DrawCreateShaderPopup() {
    // 设置弹窗居中
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Create Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        static char nameBuf[64] = "NewShader";
        ImGui::InputText("Name", nameBuf, sizeof(nameBuf));
        
        ImGui::Text("This will create a basic flat color shader.");
        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            std::string shaderName = nameBuf;
            if (!shaderName.empty()) {
                
                // --- 1. 定义默认的 Shader 模板代码 ---
                std::string vertexSrc = R"(
#version 330 core
   
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;
layout(location = 5) in vec4 a_CustomData;



uniform mat4 u_ProjViewMat;
//uniform mat4 u_Transform;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
flat out int v_EntityID;
out vec4 v_CustomData;

void main(){
v_Position = a_Position;
v_TexCoord = a_TexCoord;
v_Color = a_Color;
v_TexIndex = a_TexIndex;
v_EntityID = a_EntityID;
v_CustomData = a_CustomData;
gl_Position = u_ProjViewMat * vec4(a_Position , 1.0);
}
                    )";
                std::string fragmentSrc = R"(
#version 330 core
       
layout(location = 0) out vec4 color;
layout(location = 1) out int o_EntityID;
in vec3 v_Position;
in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;
flat in int v_EntityID;
in vec4 v_CustomData;



uniform sampler2D u_Textures[16];

void main(){
color =  texture(u_Textures[int(v_TexIndex)] , v_TexCoord) * v_Color;
o_EntityID = v_EntityID;
}
                    )";
                std::string fullSource = "#type vertex\n" + vertexSrc + "#type fragment\n" + fragmentSrc;

                // --- 2. 创建 Shader 对象 ---
                // 这里有两种路径：
                // 路径 A: 直接在内存创建 (如果有 Shader::CreateFromString 接口)
                 Ref<Shader> newShader = Shader::Create(shaderName, vertexSrc , fragmentSrc);
                
                // 路径 B: (推荐) 保存到磁盘文件然后加载
                std::string fileName = shaderName + ".glsl";
                std::filesystem::path filePath = m_CurrentDirectory / fileName;
                
                // 确保目录存在
                std::filesystem::create_directories(filePath.parent_path());
                //TODO: make new shader path file dialog seletion @Suchan
                // 写入文件
                
                std::ofstream out(filePath.string());
                if (out.is_open()) {
                    out << fullSource;
                    out.close();
                    
                    if (newShader) {
                        auto uuid = Resource::ImportAsset(filePath);
                        Resource::AddResource(shaderName , uuid,newShader);
                        newShader->SetPath(filePath.string());
                        TSO_CORE_INFO("Created new shader: {0}", filePath.string());
                    }
                } else {
                    TSO_CORE_ERROR("Failed to create shader file: {0}", filePath.string());
                }
                
            }
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ContentBrowserPanel::DrawCreateMaterialPopup() {
    // 居中显示
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Create Material", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char nameBuf[64] = "NewMaterial";
        ImGui::InputText("Name", nameBuf, sizeof(nameBuf));

        // 选择 Shader (可选)
        static std::string selectedShaderName = "None";
        static UUID selectedShaderUUID = 0;
        
        if (ImGui::BeginCombo("Shader", selectedShaderName.c_str())) {
            for (auto& [uuid, shader] : Resource::GetResourceMap<Shader>()) {
                bool isSelected = (selectedShaderUUID == uuid);
                if (ImGui::Selectable(shader->GetName().c_str(), isSelected)) {
                    selectedShaderUUID = uuid;
                    selectedShaderName = shader->GetName();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            std::string matName = nameBuf;
            if (!matName.empty()) {
                Ref<Shader> shader = Resource::GetResource<Shader>(selectedShaderUUID);
                Ref<Material> newMat = Material::Create(matName);
                if (shader) {
//                    newMat = Material::Create(shader, matName);
                    newMat->SetShader(shader);
                }
                
                std::filesystem::path projDir = Project::GetProjectDirectory(); // 获取项目根目录
                std::string fileName = matName + ".mat";
                std::filesystem::path filePath = m_CurrentDirectory / fileName;
                
                // 确保目录存在
                std::filesystem::create_directories(filePath.parent_path());
                newMat->SetPath(filePath.string());
                newMat->Serealize();
                auto uuid = Resource::ImportAsset(filePath.string());
                Resource::AddResource("" , uuid , newMat);

                    
            }
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

    void ContentBrowserPanel::OnImGuiRender() {
        ImGui::Begin("Content Browser");

        // 0. 更新 BaseDirectory
        if (Project::GetActive()) {
             m_BaseDirectory = Project::GetProjectDirectory() / g_AssetPath;
             // 如果当前目录无效（比如切项目了），重置回 Base
             if (m_CurrentDirectory.string().find(m_BaseDirectory.string()) == std::string::npos) {
                 m_CurrentDirectory = m_BaseDirectory;
             }
        }

        // 1. 顶部导航栏
        if (m_CurrentDirectory != m_BaseDirectory) {
            if (ImGui::Button("<-")) {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }
        ImGui::SameLine();
        // 显示相对路径而不是绝对路径
        std::string relPath = std::filesystem::relative(m_CurrentDirectory, Project::GetProjectDirectory()).string();
        ImGui::Text("Current: %s", relPath.c_str());

        ImGui::Separator();

        // 2. 布局设置
        static float padding = 16.0f;
        static float thumbnailSize = 80.0f;
        float cellSize = thumbnailSize + padding;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        // 3. 遍历目录
        if (std::filesystem::exists(m_CurrentDirectory)) {
            for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
                const auto& path = directoryEntry.path();
                std::string filenameString = path.filename().string();
                
                // 过滤
                if (filenameString[0] == '.') continue; // 隐藏文件
                if (path.extension() == ".meta") continue; // 不显示 meta 文件

                ImGui::PushID(filenameString.c_str());

                // 图标选择 (这里可以扩展：如果是 png 显示缩略图，如果是 .mat 显示材质球)
                Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
                
                // 按钮背景透明
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetTextureID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
                
                // --- 拖拽源 (Drag Source) ---
                if (ImGui::BeginDragDropSource()) {
                    // 核心逻辑：尝试获取 UUID
                    // 如果文件存在但没有 UUID (未导入)，此时应该尝试自动导入或者返回 0
                    UUID uuid = Resource::GetUUIDFromPath(path.string());
                    
                    if (uuid == 0) {
                        // 尝试即时导入 (Lazy Import)
                         uuid = Resource::ImportAsset(path);
                    }

                    if (uuid != 0) {
                        // 根据后缀分发不同类型的 Payload
                        // 这样接收端（Inspector）可以只接收特定类型
                        
                        if (path.extension() == ".anim") {
                            ImGui::SetDragDropPayload("RESOURCE_ANIM_CLIP", &uuid, sizeof(UUID));
                            ImGui::Text("Animation: %s", filenameString.c_str());
                        }
                        else if (path.extension() == ".png" || path.extension() == ".jpg") {
                            ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &uuid, sizeof(UUID));
                            ImGui::Text("Texture: %s", filenameString.c_str());
                        }
                        else if (path.extension() == ".mat") {
                            ImGui::SetDragDropPayload("RESOURCE_MATERIAL", &uuid, sizeof(UUID));
                            ImGui::Text("Material: %s", filenameString.c_str());
                        }
                        else if (path.extension() == ".lua") {
                            auto name = path.stem().string();
                            ImGui::SetDragDropPayload("RESOURCE_SCRIPT", &name, sizeof(name));
                            ImGui::Text("Script: %s", filenameString.c_str());
                        }
                        else if (path.extension() == ".controller") {
                            ImGui::SetDragDropPayload("RESOURCE_ANIM_CONTROLLER", &uuid, sizeof(UUID));
                            ImGui::Text("Controller: %s", filenameString.c_str());
                        }
                        else if(path.extension() == ".glsl"){
                            ImGui::SetDragDropPayload("RESOURCE_SHADER", &uuid, sizeof(UUID));
                            ImGui::Text("shader: %s", filenameString.c_str());
                        }
                        else {
                            // 通用文件 Payload
                            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &uuid, sizeof(UUID));
                            ImGui::Text("File: %s", filenameString.c_str());
                        }
                    } else {
                        ImGui::Text("Not Imported: %s", filenameString.c_str());
                    }
                    
                    ImGui::EndDragDropSource();
                }

                ImGui::PopStyleColor();
                
                if (ImGui::BeginPopupContextItem()) {
                    // 1. 加载文件到内存
                    if (ImGui::MenuItem("Load Asset")) {
                        UUID uuid = Resource::GetUUIDFromPath(path.string());
                        if(uuid == 0){
                            uuid = Resource::ImportAsset(path);
                        }
                        if (uuid != 0) {
                            if (path.extension() == ".anim") {
                                Resource::GetResource<AnimationClip>(uuid);
                            }
                            else if (path.extension() == ".png" || path.extension() == ".jpg") {
                                Resource::GetResource<Texture2D>(uuid);
                            }
                            else if (path.extension() == ".mat") {
                                Resource::GetResource<Material>(uuid);
                            }
                            else if (path.extension() == ".lua") {
                                
                            }
                            else if(path.extension() == ".glsl"){
                                Resource::GetResource<Shader>(uuid);
                            }
                            else {
                            }
                        } else {
                            TSO_CORE_WARN("Asset not imported: {0}", filenameString);
                        }
                    }

                    // 2. 重命名 (触发弹窗)
                    if (ImGui::MenuItem("Rename")) {
//                        m_PathToRename = path;
//                        m_OpenRenamePopup = true;
                    }

                    // 3. 删除文件
//                    if (ImGui::MenuItem("Delete")) {
//                        // 删除逻辑：不仅删文件，还要删 .meta
//                        try {
//                            // 删原始文件
//                            std::filesystem::remove(path);
//                            
//                            // 删 meta 文件
//                            std::filesystem::path metaPath = path.string() + ".meta";
//                            if (std::filesystem::exists(metaPath)) {
//                                std::filesystem::remove(metaPath);
//                            }
//                            
//                            // 通知资源系统卸载 (如果已加载)
//                            UUID uuid = Resource::GetUUIDFromPath(path); // 注意：这里可能已经读不到meta了，最好之前获取
//                            // Resource::UnloadAsset(uuid);
//
//                            TSO_CORE_INFO("Deleted asset: {0}", filenameString);
//                        } catch (std::filesystem::filesystem_error& e) {
//                            TSO_CORE_ERROR("Failed to delete file: {0}", e.what());
//                        }
//                    }

                    ImGui::EndPopup();
                }

                // 双击进入目录
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (directoryEntry.is_directory()) {
                        m_CurrentDirectory /= path.filename();
                    }
                }
                if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
                    if(directoryEntry.is_regular_file()){
                        UUID uuid = Resource::GetUUIDFromPath(path.string());
                        if(uuid != 0){
                            ResourceType type = ResourceType::None;
                            if (path.extension() == ".anim") {
                                
                            }
                            else if (path.extension() == ".png" || path.extension() == ".jpg") {
                                type = ResourceType::Texture;
                            }
                            else if (path.extension() == ".mat") {
                                type = ResourceType::Material;
                            }
                            else if (path.extension() == ".lua") {
                                
                            }
                            else if(path.extension() == ".glsl"){
                                type = ResourceType::Shader;
                            }
                            if(type !=  ResourceType::None){
                                m_SceneHPanel->SetSelectedResource(uuid, type);
                            }
                        }
                    }
                }

                ImGui::TextWrapped("%s", filenameString.c_str());
                ImGui::NextColumn();
                ImGui::PopID();
            }
        }

        ImGui::Columns(1);

        // 4. 右键上下文菜单
        // 在空白处右击
        if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            
            // 获取当前目录名
            std::string dirName = m_CurrentDirectory.filename().string();
            
            // 通用选项
            if (ImGui::MenuItem("Import Asset...")) {
                ImportAsset("All Files\0*.*\0");
            }
            ImGui::Separator();

            // 智能菜单
            if (dirName == "Shader") {
                if (ImGui::MenuItem("Create Shader")) {
                    m_NextAction = DeferredAction::CreateShader;
                }
                if (ImGui::MenuItem("Import GLSL")) { ImportAsset("Shader (*.glsl)\0*.glsl\0"); }
            }
            else if (dirName == "Material") {
                if (ImGui::MenuItem("Create Material")) {
                    m_NextAction = DeferredAction::CreateMaterial;
                }
            }
            else if (dirName == "Scripts") {
                if (ImGui::MenuItem("Create Lua Script")) {
                        m_NextAction = DeferredAction::CreateScript;
                }
                if (ImGui::MenuItem("Import Lua")) { ImportAsset("Lua (*.lua)\0*.lua\0"); }
            }
            else if (dirName == "Animations") {
                // 对于动画，通常是在 AnimationPanel 里创建，这里可以提供快捷入口
                if (ImGui::MenuItem("Create Animation")) { /* Open Animation Panel */ }
            }
            else if (dirName == "Sprites" || dirName == "Textures") {
                if (ImGui::MenuItem("Import Texture")) { ImportAsset("Image (*.png *.jpg)\0*.png;*.jpg\0"); }
            }

            ImGui::EndPopup();
        }
        if (m_NextAction != DeferredAction::None) {
            switch (m_NextAction) {
                case DeferredAction::CreateMaterial:
                    ImGui::OpenPopup("Create Material");
                    break;
                case DeferredAction::CreateShader:
                    ImGui::OpenPopup("Create Shader");
                    break;
                case DeferredAction::CreateScript:{
                    ImGui::OpenPopup("Create Script");
                    break;
                }
                    
                case DeferredAction::ImportTexture: {
                    // 原生对话框：阻塞操作
                    std::string filepath = FileDialogs::OpenFile("Image Files (*.png *.jpg)\0*.png;*.jpg\0");
                    std::filesystem::path p(filepath);
                    std::string filename = p.stem().string();
                    if (!filepath.empty()) {
                        auto tex = Texture2D::Create(filepath); // 加载纹理
                        Resource::AddResource(filename , UUID(), tex);
                        
                    }
                    break;
                }
                case DeferredAction::ImportShader: {
                    // 原生对话框：阻塞操作
                    std::string filepath = FileDialogs::OpenFile("Shader Files (*.glsl *.shader)\0*.glsl;*.shader\0");
                    if (!filepath.empty()) {
                        // Resource::LoadShader(filepath);
                        auto shader = Shader::Create(filepath);
                        std::filesystem::path p(filepath);
                        std::string shaderName = p.stem().string();
                        Resource::AddResource(shaderName, UUID(), shader);
                    }
                    break;
                }
                case DeferredAction::ImportFont : {
                    // 原生对话框：阻塞操作
                    std::string filepath = FileDialogs::OpenFile("ttf (*.ttf)\0 * .ttf\0");
                    if (!filepath.empty()) {
                        // Resource::LoadShader(filepath);
                        auto font = CreateRef<Font>(std::filesystem::path(filepath));
                        std::filesystem::path p(filepath);
                        std::string fontName = p.stem().string();
                        Resource::AddResource(fontName, UUID(), font);
                    }
                    break;
                }
                default: break;
            }
                
            // 动作执行完后重置
            m_NextAction = DeferredAction::None;
            }
        DrawCreateMaterialPopup();
        DrawCreateShaderPopup();
        DrawCreateScriptPopup();
        ImGui::End();
    }
}
