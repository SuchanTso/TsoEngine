#include "SceneHierarchyPanel.h"
#include "imgui.h"
#include "Tso/Scene/Component.h"
#include "glm/gtc/type_ptr.hpp"
#include "Tso/Scene/SceneCamera.h"
#include "Tso/Utils/PlatformUtils.h"
#include "Tso/Renderer/Font.h"
#include "Tso/Scene/ScriptableEntity.h"
#include "Tso/Scripting/ScriptingEngine.h"
#include "Tso/Project/Project.h"
#include "Tso/Project/Resource.h"
#include "Tso/Core/UUID.h"
#include <fstream>

namespace Tso {
template<> void SceneHierarchyPanel::DisplayAddComponentEntry<ButtonComponent>(const std::string &entryName){
    if (!m_SelectedEntity.HasComponent<ButtonComponent>())
    {
        if (ImGui::MenuItem(entryName.c_str()))
        {
            m_SelectedEntity.AddComponent<ButtonComponent>();
            m_SelectedEntity.AddComponent<UITransformComponent>();
            auto& textc = m_SelectedEntity.AddComponent<TextComponent>();
            textc.isUI = true;
            ImGui::CloseCurrentPopup();
        }
    }
}

template<> void SceneHierarchyPanel::DisplayAddComponentEntry<InputFieldComponent>(const std::string &entryName){
    if (!m_SelectedEntity.HasComponent<InputFieldComponent>())
    {
        if (ImGui::MenuItem(entryName.c_str()))
        {
            m_SelectedEntity.AddComponent<InputFieldComponent>();
            m_SelectedEntity.AddComponent<UITransformComponent>();
            auto& textc = m_SelectedEntity.AddComponent<TextComponent>();
            textc.isUI = true;
            ImGui::CloseCurrentPopup();
        }
    }
}

template<> void SceneHierarchyPanel::DisplayAddComponentEntry<TextComponent>(const std::string &entryName){
    if (!m_SelectedEntity.HasComponent<InputFieldComponent>())
    {
        if (ImGui::MenuItem(entryName.c_str()))
        {
            m_SelectedEntity.AddComponent<TextComponent>();
            ImGui::CloseCurrentPopup();
        }
    }
}

static const char* s_UniformTypes[] = {
        "Float", "Int", "Vec2", "Vec3", "Vec4", "Color (Vec3)", "Color (Vec4)" , "Texture"
    };


// 在类成员中添加状态变量
// bool m_ShowCreateMaterialPopup = false;

    void SceneHierarchyPanel::CreateMaterialPrompt() {
        ImGui::OpenPopup("Create Material");
    }

    void SceneHierarchyPanel::CreateShaderPrompt() {
        // 触发 ImGui 的弹窗打开状态
        ImGui::OpenPopup("Create New Shader");
    }

    void SceneHierarchyPanel::DrawCreateShaderPopup() {
        // 设置弹窗居中
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Create New Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            
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
                    std::filesystem::path projDir = Project::GetProjectDirectory(); // 获取项目根目录
                    std::string fileName = shaderName + ".glsl";
                    std::filesystem::path filePath = projDir / "assets" / "shader" / fileName;
                    
                    // 确保目录存在
                    std::filesystem::create_directories(filePath.parent_path());
                    
                    // 写入文件
                    
                    std::ofstream out(filePath.string());
                    if (out.is_open()) {
                        out << fullSource;
                        out.close();
                        
                        if (newShader) {
                            Resource::AddResource(shaderName , UUID(),newShader);
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

    void SceneHierarchyPanel::DrawCreateMaterialPopup() {
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
                    
                    // 如果没选 Shader，可能需要由 Material::Create 处理默认情况
                    if (!shader) {
                        // shader = Resource::GetDefaultShader(); // 建议有个默认紫色 Shader
                    }
                    
                    if (shader) {
                        Ref<Material> newMat = Material::Create(shader, matName);
                        // 重要：这里需要将新材质注册到全局资源管理器中
                        Resource::AddResource("" , UUID() , newMat);
                        std::filesystem::path projDir = Project::GetProjectDirectory(); // 获取项目根目录
                        std::string fileName = matName + ".mat";
                        std::filesystem::path filePath = projDir / "assets" / "material" / fileName;
                        
                        // 确保目录存在
                        std::filesystem::create_directories(filePath.parent_path());
                        newMat->SetPath(filePath.string());
                        newMat->Serealize();
                        
                    
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

    // --- 1. 资源列表窗口 ---
    void SceneHierarchyPanel::DrawResourceList() {
        ImGui::Begin("Resources");

        // 1. Shaders
        if (ImGui::CollapsingHeader("Shaders", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& shaders = Resource::GetResourceMap<Shader>();
            for (auto& [uuid, shader] : shaders) {
                bool isSelected = (m_SelectedResourceUUID == uuid);
                if (ImGui::Selectable(shader->GetName().c_str(), isSelected)) {
                    m_SelectedResourceUUID = uuid;
                    m_SelectedType = ResourceType::Shader;
                }
            }
        }

        // 2. Textures
        if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& textures = Resource::GetResourceMap<Texture2D>();
            for (auto& [uuid, tex] : textures) {
                bool isSelected = (m_SelectedResourceUUID == uuid);
                
                // 显示名称
                if (ImGui::Selectable(tex->GetName().c_str(), isSelected)) {
                    m_SelectedResourceUUID = uuid;
                    m_SelectedType = ResourceType::Texture;
                }

                // --- 拖拽源 (Drag Source) ---
                // 允许将这个 Texture 拖动到 Material 的插槽中
                if (ImGui::BeginDragDropSource()) {
                    // 传递 UUID 数据
                    ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &uuid, sizeof(UUID));
                    ImGui::Text("%s", tex->GetName().c_str()); // 拖动时显示的预览图
                    ImGui::EndDragDropSource();
                }
            }
        }
        
        if (ImGui::CollapsingHeader("Fonts", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& fonts = Resource::GetResourceMap<Font>();
            for (auto& [uuid, font] : fonts) {
                bool isSelected = (m_SelectedResourceUUID == uuid);
                if (ImGui::Selectable(font->GetName().c_str(), isSelected)) {
                    m_SelectedResourceUUID = uuid;
                    m_SelectedType = ResourceType::Font;
                }
                if (ImGui::BeginDragDropSource()) {
                    // 参数1: 标签 (必须与 Target 端一致)
                    // 参数2: 数据指针 (这里传 UUID 的地址)
                    // 参数3: 数据大小
                    ImGui::SetDragDropPayload("RESOURCE_FONT", &uuid, sizeof(UUID));

                    // 3. 设置拖拽时的预览图/文字 (跟在鼠标旁边的那个提示)
                    ImGui::Text("font: %s", font->GetName().c_str());
                    // 如果你想做得更花哨，可以在这里绘制材质的缩略图
                    
                    ImGui::EndDragDropSource();
                }
            }
        }

        // 3. Materials
        if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& materials = Resource::GetResourceMap<Material>();
            for (auto& [uuid, mat] : materials) {
                bool isSelected = (m_SelectedResourceUUID == uuid);
                if (ImGui::Selectable(mat->GetName().c_str(), isSelected)) {
                    m_SelectedResourceUUID = uuid;
                    m_SelectedType = ResourceType::Material;
                }
                if (ImGui::BeginDragDropSource()) {
                    // 参数1: 标签 (必须与 Target 端一致)
                    // 参数2: 数据指针 (这里传 UUID 的地址)
                    // 参数3: 数据大小
                    ImGui::SetDragDropPayload("RESOURCE_MATERIAL", &uuid, sizeof(UUID));

                    // 3. 设置拖拽时的预览图/文字 (跟在鼠标旁边的那个提示)
                    ImGui::Text("Material: %s", mat->GetName().c_str());
                    // 如果你想做得更花哨，可以在这里绘制材质的缩略图
                    
                    ImGui::EndDragDropSource();
                }
            }
        }
        
        if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                
            if (ImGui::MenuItem("Create Material")) {
                m_NextAction = DeferredAction::CreateMaterial;
            }

            if (ImGui::MenuItem("Import Texture...")) {
                m_NextAction = DeferredAction::ImportTexture;
            }

            if (ImGui::MenuItem("Import Shader...")) {
                m_NextAction = DeferredAction::ImportShader;
            }
            
            if(ImGui::MenuItem("Import Font...")) {
                m_NextAction = DeferredAction::ImportFont;
            }
            
            if (ImGui::MenuItem("Create Empty Shader")) {
                 m_NextAction = DeferredAction::CreateShader;
            }

            ImGui::EndPopup();
        }

            // --- 2. 在右键菜单作用域之外，处理延迟的动作 ---
            // 这样做可以确保 ContextWindow 已经关闭，ImGui 栈是干净的，
            // 且原生文件对话框不会阻塞 ContextWindow 的渲染提交。
            
        if (m_NextAction != DeferredAction::None) {
            switch (m_NextAction) {
                case DeferredAction::CreateMaterial:
                    CreateMaterialPrompt(); // 打开 ImGui 弹窗
                    break;
                case DeferredAction::CreateShader:
                    CreateShaderPrompt();   // 打开 ImGui 弹窗
                    break;
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

            // --- 3. 绘制 ImGui 模态弹窗 ---
            // 这些函数内部有 BeginPopupModal，必须放在根层级调用
            DrawCreateMaterialPopup();
            DrawCreateShaderPopup();

        ImGui::End();
    }

    // --- 2. 资源属性面板窗口 ---
    void SceneHierarchyPanel::DrawResourceInspector() {
        ImGui::Begin("Resource Inspector");

        if (m_SelectedResourceUUID == 0 || m_SelectedType == ResourceType::None) {
            ImGui::Text("No resource selected.");
            ImGui::End();
            return;
        }

        switch (m_SelectedType) {
            case ResourceType::Texture: {
                // 获取资源 (需自行实现 Resource::GetTexture(uuid))
                Ref<Texture2D> tex = Resource::GetResource<Texture2D>(m_SelectedResourceUUID);
                if (tex) DrawTextureNode(tex);
                break;
            }
            case ResourceType::Material: {
                Ref<Material> mat = Resource::GetResource<Material>(m_SelectedResourceUUID);
                if (mat) DrawMaterialNode(mat);
//                mat->ClearInstanceSlots();
                break;
            }
            case ResourceType::Shader: {
                Ref<Shader> shader = Resource::GetResource<Shader>(m_SelectedResourceUUID);
                if (shader) DrawShaderNode(shader);
                break;
            }
            default: break;
        }
        
        if (m_RequestSelectTexture) {
            // 1. 打开弹窗选择 (纯 ImGui)
            ImGui::OpenPopup("SelectTexturePopup");
            // 2. 或者打开文件对话框 (Native)
            // FileDialogs::OpenFile(...)
            
            m_RequestSelectTexture = false; // 消费掉事件
        }


        ImGui::End();
    }

    // --- 辅助函数具体实现 ---

    void SceneHierarchyPanel::DrawTextureNode(Ref<Texture2D> texture) {
        ImGui::Text("Name: %s", texture->GetName().c_str());
        ImGui::Text("Path: %s", texture->GetPath().c_str());
        ImGui::Text("Size: %d x %d", texture->GetWidth(), texture->GetHeight());
        ImGui::Separator();

        // 计算缩略图大小（保持宽高比）
        float regionWidth = ImGui::GetContentRegionAvail().x;
        float aspect = (float)texture->GetHeight() / (float)texture->GetWidth();
        float imgHeight = regionWidth * aspect;

        // ImGui::Image 需要传入纹理 ID (void*)
        // 注意：OpenGL ID 需要强转为 void*
        // ImVec2(0, 1), ImVec2(1, 0) 是为了翻转 UV，因为 OpenGL 和 ImGui 坐标系 Y 轴相反
        ImGui::Image((void*)(uintptr_t)texture->GetTextureID(),
                     ImVec2(regionWidth, imgHeight),
                     ImVec2(0, 1), ImVec2(1, 0));
    }

    void SceneHierarchyPanel::SyncShaderToCache(Ref<Shader> shader) {
        // 只有当 ID 变了，或者缓存是空的，才从 Shader 实例拉取数据
        // 假设 Shader 有一个 GetUUID() 或者我们用 m_SelectedResourceUUID
        if (m_ShaderCache.ShaderUUID != m_SelectedResourceUUID) {
            m_ShaderCache.ShaderUUID = m_SelectedResourceUUID;
            m_ShaderCache.SourceBuffers.clear();
            m_ShaderCache.IsDirty = false;

            auto& sources = shader->GetSource(); // 这里获取的是原始数据的引用或拷贝
            for (auto& [stage, code] : sources) {
                m_ShaderCache.SourceBuffers[stage] = code;
            }
        }
    }

void SceneHierarchyPanel::DrawShaderNode(Ref<Shader> shader) {
    // 1. 同步状态：确保 m_ShaderCache 里的内容是当前 Shader 的代码
    SyncShaderToCache(shader);
    
    ImGui::Text("Shader Name: %s", shader->GetName().c_str());
    
    // 2. 编译按钮
    // 如果代码被修改过（Dirty），按钮变色提醒用户保存
    if (m_ShaderCache.IsDirty) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.1f, 1.0f)); // 橙色
        if (ImGui::Button("Compile & Save Changes")) {
            // --- 核心操作：调用 Shader 的重编译接口 ---
            // 这里我们需要修改 Shader 基类，添加 Recompile(map) 接口
            bool success = shader->Recompile(m_ShaderCache.SourceBuffers);
            
            if (success) {
                m_ShaderCache.IsDirty = false; // 重置 Dirty 标记
                TSO_CORE_INFO("Shader recompiled successfully!");
            } else {
                TSO_CORE_ERROR("Shader compilation failed!");
            }
        }
        ImGui::PopStyleColor();
    } else {
        if (ImGui::Button("Recompile")) {
            // 即使没修改，也允许强制重编译（可能为了测试随机性或外部因素）
            shader->Recompile(m_ShaderCache.SourceBuffers);
        }
    }
    
    ImGui::SameLine();
    
    // 还原按钮：放弃修改，重新从 Shader 对象拉取代码
    if (ImGui::Button("Reset")) {
        m_ShaderCache.ShaderUUID = 0; // 强制 SyncShaderToCache 下次执行
        SyncShaderToCache(shader);
    }
    
    ImGui::Separator();
    
    // 3. 多标签页编辑器
    if (ImGui::BeginTabBar("ShaderStages")) {
        
        for (auto& [stage, buffer] : m_ShaderCache.SourceBuffers) {
            if (ImGui::BeginTabItem(shader->GetShaderStageName(stage))) {
                
                // ImGui::InputTextMultiline 需要 char*
                // 为了支持动态扩容，ImGui 提供了 Callback 机制，但这里我们用简单的 resize 策略
                // 预留一些 buffer 空间，防止用户打几个字就溢出
                if (buffer.capacity() < buffer.size() + 1024) {
                    buffer.resize(buffer.size() + 1024); // 简单粗暴的扩容
                }
                
                // 计算高度：填满窗口，留出底部一点空隙
                ImVec2 contentRegion = ImGui::GetContentRegionAvail();
                contentRegion.y -= 10.0f;
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); // 浅灰色代码
                
                // 这里的 buffer.data() 是可读写的
                // 注意：InputTextMultiline 返回 true 表示内容被修改了
                if (ImGui::InputTextMultiline(("##src" + std::to_string(stage)).c_str(),
                                              buffer.data(),
                                              buffer.capacity(),
                                              contentRegion,
                                              ImGuiInputTextFlags_AllowTabInput)) {
                    
                    // 重新计算实际 string 长度 (InputText 会写入 \0)
                    // 必须这一步，否则 string::size 还是原来扩容后的大小
                    buffer.resize(strlen(buffer.data()));
                    
                    m_ShaderCache.IsDirty = true;
                }
                
                ImGui::PopStyleColor();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

template<typename T, typename UIFunction>
    static bool DrawMaterialPropertyRow(const std::string& name, T& value, UIFunction uiFunction) {
        bool deleted = false;
        
        ImGui::TableNextRow();
        
        // 第一列：名称 (Name)
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding(); // 让文字垂直居中
        ImGui::Text("%s", name.c_str());

        // 第二列：数值控件 (Value)
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1); // 填满列宽
        
        //为了避免 ID 冲突，这里必须 PushID
        ImGui::PushID(name.c_str());
        uiFunction(value); // 调用具体的绘制回调
        ImGui::PopID();
        
        ImGui::PopItemWidth();

        // 第三列：删除按钮 (Delete)
        ImGui::TableSetColumnIndex(2);
        ImGui::PushID(("del_" + name).c_str());
        if (ImGui::Button("X")) {
            deleted = true;
        }
        ImGui::PopID();

        return deleted;
    }

    void SceneHierarchyPanel::DrawMaterialNode(Ref<Material> material) {
        ImGui::Text("Material: %s", material->GetName().c_str());
        std::string shaderName = material->GetShader() ? material->GetShader()->GetName() : "None";
        ImGui::Text("Shader: %s", shaderName.c_str());
        ImGui::Separator();

        ImGui::Text("Properties:");

        // 用于记录当前正在点击哪个 Texture Slot 进行编辑
        static std::string s_TextureSlotToEdit = "";

        static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg;
        
        if (ImGui::BeginTable("MaterialPropsTable", 3, table_flags)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Del", ImGuiTableColumnFlags_WidthFixed, 24.0f);

            // --- 1. Float Params (保持不变) ---
            auto& floatParams = material->GetFloatParams();
            for (auto it = floatParams.begin(); it != floatParams.end(); ) {
                bool deleted = DrawMaterialPropertyRow(it->first, it->second, [](float& val){
                    ImGui::DragFloat("##v", &val, 0.1f);
                });
                if (deleted) it = floatParams.erase(it); else ++it;
            }

            // --- 2. Vec3 / Color Params (保持不变) ---
            auto& vec3Params = material->GetVec3Params();
            for (auto it = vec3Params.begin(); it != vec3Params.end(); ) {
                bool deleted = DrawMaterialPropertyRow(it->first, it->second, [](glm::vec3& val){
                    ImGui::ColorEdit3("##v", &val.x);
                });
                if (deleted) it = vec3Params.erase(it); else ++it;
            }

            // --- 3. Texture Params (修改部分) ---
            auto& texParams = material->GetTextureParams();
            for (auto it = texParams.begin(); it != texParams.end(); ) {
                bool deleted = false;
                ImGui::TableNextRow();
                
                // Col 0: Name
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", it->first.c_str());

                // Col 1: Value (Image Button)
                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(it->first.c_str());
                
                Ref<Texture2D> tex = it->second;
                // 注意：GetTextureID() 还是 GetRendererID() 取决于你的 API 封装，这里沿用你之前的写法
                ImTextureID texID = tex ? (void*)(uintptr_t)tex->GetTextureID() : 0;
                
                // >>> 修改点 2: 点击图片按钮打开选择弹窗 <<<
                if (ImGui::ImageButton(texID, ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0))) {
                    m_RequestSelectTexture = true;
                    m_TextureSlotToChange = it->first;
                }
                // 鼠标悬停显示 Tooltip 提示可以点击
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Click to select texture");
                }
                
                // 接收拖拽 (保持不变)
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE")) {
                        UUID texUUID = *(const UUID*)payload->Data;
                        it->second = Resource::GetResource<Texture2D>(texUUID);
                    }
                    ImGui::EndDragDropTarget();
                }
                
                ImGui::SameLine();
                ImGui::Text(tex ? tex->GetName().c_str() : "Empty"); // 显示纹理名字
                ImGui::PopID();

                // Col 2: Delete
                ImGui::TableSetColumnIndex(2);
                ImGui::PushID(("del_" + it->first).c_str());
                if (ImGui::Button("X")) deleted = true;
                ImGui::PopID();

                if (deleted) it = texParams.erase(it);
                else ++it;
            }
            
            

            ImGui::EndTable();
            
            ImGui::Separator();
            ImGui::Text("Instance Parameter Definition (Vertex Attributes)");

            // 显示当前的映射
            auto& instanceParams = material->GetInstanceParams();
            for (auto& [name, slot] : instanceParams) {
                ImGui::Text("%s -> Slot [%d] (a_CustomData.%c)", name.c_str(), slot, "xyzw"[slot]);
            }

            // 添加新映射的 UI
            static char s_ParamNameBuf[64] = "CustomData";
            static int s_SlotIdx = 0;
            ImGui::InputText("Param Name", s_ParamNameBuf, sizeof(s_ParamNameBuf));
            ImGui::Combo("Slot", &s_SlotIdx, "Slot X (0)\0Slot Y (1)\0Slot Z (2)\0Slot W (3)\0");

            if (ImGui::Button("Define Instance Param")) {
                material->DefineInstanceParam(s_ParamNameBuf, s_SlotIdx);
            }
        }

        // >>> 修改点 2 (续): 纹理选择弹窗实现 <<<
        if (ImGui::BeginPopup("SelectTexturePopup")) {
            ImGui::Text("Select Texture for '%s'", m_TextureSlotToChange.c_str());
            ImGui::Separator();

            // 提供一个 "None" 选项来清空纹理
            if (ImGui::Selectable("None")) {
                material->SetTexture(m_TextureSlotToChange, nullptr);
                ImGui::CloseCurrentPopup();
            }

            // 遍历所有资源中的纹理
            auto& allTextures = Resource::GetResourceMap<Texture2D>();
            for (auto& [uuid, tex] : allTextures) {
                // 可选：显示小图标
                ImGui::Image((void*)(uintptr_t)tex->GetTextureID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0));
                ImGui::SameLine();
                
                if (ImGui::Selectable(tex->GetName().c_str())) {
                    // 选中后赋值
                    material->SetTexture(m_TextureSlotToChange, tex);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        // --- 添加 Uniform 的逻辑 ---

        if (ImGui::Button("Add Uniform...")) {
            ImGui::OpenPopup("AddUniformPopup");
        }

        static char s_NewNameBuffer[64] = "u_NewProp";
        static int s_SelectedTypeIndex = 0;
        static bool s_ShouldFocusInput = true;

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("AddUniformPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Add new property to Material");
            ImGui::Separator();

            if (s_ShouldFocusInput) {
                ImGui::SetKeyboardFocusHere();
                s_ShouldFocusInput = false;
            }
            ImGui::InputText("Name", s_NewNameBuffer, sizeof(s_NewNameBuffer));
            
            // 类型选择
            ImGui::Combo("Type", &s_SelectedTypeIndex, s_UniformTypes, IM_ARRAYSIZE(s_UniformTypes));

            ImGui::Separator();

            if (ImGui::Button("Add", ImVec2(120, 0))) {
                std::string newName = std::string(s_NewNameBuffer);
                
                if (!newName.empty()) {
                    // >>> 修改点 1: 处理 Texture 类型添加 <<<
                    // 确保 switch case 的索引与 s_UniformTypes 数组对应
                    switch (s_SelectedTypeIndex) {
                        case 0: material->SetFloat(newName, 0.0f); break;
                        case 1: material->SetInt(newName, 0); break;
                        case 2: material->SetVec2(newName, glm::vec2(0.0f)); break;
                        case 3: // Vec3
                        case 5: // Color3
                                material->SetVec3(newName, glm::vec3(1.0f)); break;
                        case 4: // Vec4
                        case 6: // Color4
                                material->SetVec4(newName, glm::vec4(1.0f)); break;
                        case 7: // Texture (Index 7)
                                // 初始化为空纹理
                                material->SetTexture(newName, nullptr);
                                break;
                    }
                    
                    memset(s_NewNameBuffer, 0, sizeof(s_NewNameBuffer));
                    s_ShouldFocusInput = true;
                }
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Close", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                strcpy(s_NewNameBuffer, "u_NewProp");
                s_ShouldFocusInput = true;
            }

            ImGui::EndPopup();
        }
    }




	void SceneHierarchyPanel::OnGuiRender()
	{
		ImGui::Begin("SceneNode");

		m_Context->m_Registry.each([&](auto entity) {
			Entity e( entity , m_Context.get() );
            if (e.GetParent() == nullptr) {
                DrwaEntityNode(e);
            }
			});

        if (m_DeletedEntity.m_EntityID != entt::null) {
            //delete entity
            auto parent = m_DeletedEntity.GetParent();
            if (parent) {
                parent->RemoveChild(m_DeletedEntity);
            }
            for (auto& child : m_DeletedEntity.GetChildren()) {
                m_DeletedEntity.RemoveChild(*child.second.get());
                m_Context->DeleteEntity(*child.second.get());
            }
            m_Context->DeleteEntity(m_DeletedEntity);
            m_DeletedEntity = { entt::null , nullptr };
        }
        
        
        if(m_SelectedEntity.m_EntityID == entt::null){
            if (ImGui::BeginPopupContextWindow(0, 1))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                    m_Context->CreateEntity("Empty Entity");
                
                ImGui::EndPopup();
            }
        }
		ImGui::End();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			m_SelectedEntity.m_EntityID = entt::null;
		}

		ImGui::Begin("Components");
		if (m_SelectedEntity.m_EntityID != entt::null) {
			DrawComponents(m_SelectedEntity);
		}
		ImGui::End();
        
        DrawResourceList();
        DrawResourceInspector();
        
	}
	void SceneHierarchyPanel::DrwaEntityNode(Entity& entity)
	{

		auto& tag = entity.GetComponent<TagComponent>().GetTagName();
		ImGuiTreeNodeFlags flag = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flag, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
		}
        
        
        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            
            if (ImGui::MenuItem("Add Child Entity")) {
                auto newEntity = m_Context->CreateEntity("Empty Entity");
                entity.AddChild(newEntity);
                
            }
                
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;

            ImGui::EndPopup();
        }

		if (opened) {
            for (auto& child : entity.GetChildren()) {
                DrwaEntityNode(*child.second);
            }
			ImGui::TreePop();
		}
        if(entityDeleted){
            m_DeletedEntity = entity;
            /*auto parent = entity.GetParent();
            if (parent) {
                parent->RemoveChild(entity);
            }
            m_Context->DeleteEntity(entity);
            if (m_SelectedEntity == entity)
                m_SelectedEntity = Entity{entt::null , m_Context.get()};*/
        }
	}

void SceneHierarchyPanel::DrawResources(){
    auto& shaders = Resource::GetResourceMap<Shader>();
    for(auto& [uuid , shader] : shaders){
        ImGui::Text("%s",shader->GetName().c_str());
    }
    auto& textures = Resource::GetResourceMap<Texture2D>();
    for(auto& [uuid , tex] : textures){
        ImGui::Text("%s",tex->GetName().c_str());
    }
    
    auto& materials = Resource::GetResourceMap<Material>();
    for(auto& [uuid , mat] : materials){
        ImGui::Text("%s",mat->GetName().c_str());
    }
    
    auto& fonts = Resource::GetResourceMap<Font>();
    for(auto& [uuid , font] : fonts){
        ImGui::Text("%s",font->GetName().c_str());
    }
    
}


	void SceneHierarchyPanel::DrawComponents(Entity& entity)
{

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");
        
        if (ImGui::BeginPopup("AddComponent"))
        {
            DisplayAddComponentEntry<Renderable>("Renderable");
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<ScriptComponent>("Script");
            DisplayAddComponentEntry<MaterialInstanceComponent>("MaterialInstance");
            DisplayAddComponentEntry<NativeScriptComponent>("NativeScript");
            DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody2DComponent");
            DisplayAddComponentEntry<BoxCollider2DComponent>("BoxCollider2DComponent");
            DisplayAddComponentEntry<TextComponent>("TextComponent");
            DisplayAddComponentEntry<NetworkComponent>("NetworkComponent");
            DisplayAddComponentEntry<ButtonComponent>("ButtonComponent");
            DisplayAddComponentEntry<InputFieldComponent>("InputField");

            ImGui::EndPopup();
        }
        
        if(entity.HasComponent<IDComponent>()){
            auto comp = entity.GetComponent<IDComponent>();
            ImGui::Text("ID:%llu" , (uint64_t)comp.ID);
        }
    
		if (entity.HasComponent<TagComponent>()) {

			auto& comp = entity.GetComponent<TagComponent>();

			auto& tag = comp.GetTagName();
			char buff[256];
			
			strcpy(buff, tag.c_str());

			if (ImGui::InputText("tagName", buff, sizeof(buff))) {
				comp.SetTagName(buff);
			}
		}
		if (entity.HasComponent<TransformComponent>()) {
			if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_OpenOnArrow)) {
				auto& comp = entity.GetComponent<TransformComponent>();
				auto& pos = comp.GetPos();
				auto& rotate = comp.GetRotate();
				auto& scale = comp.GetScale();
				if (ImGui::DragFloat3("pos:", glm::value_ptr(pos) , 0.1f)) {
					comp.SetPos(pos);
				}
				if (ImGui::DragFloat3("rotate:", glm::value_ptr(rotate), 0.1f)) {
					comp.SetRotate(rotate);
				}
				if (ImGui::DragFloat3("scale:", glm::value_ptr(scale), 0.1f)) {
					comp.SetScale(scale);
				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<Rigidbody2DComponent>()) {
			if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_OpenOnArrow)) {
				auto& comp = entity.GetComponent<Rigidbody2DComponent>();
                std::string RigidBodyType[2] = { "Static" , "Dynamic" };
                std::string currentRigidBodyType = RigidBodyType[(int)comp.Type];
                if (open) {
                    if (ImGui::BeginCombo("RigidBodyType", currentRigidBodyType.c_str())) {
                        for (int i = 0; i < 2; i++)
                        {
                            bool isSelected = currentRigidBodyType == RigidBodyType[i];
                            if (ImGui::Selectable(RigidBodyType[i].c_str(), isSelected))
                            {
                                currentRigidBodyType = RigidBodyType[i];
                                comp.Type = (Rigidbody2DComponent::BodyType)i;
                            }

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }

                        ImGui::EndCombo();
                    }
                    ImGui::Checkbox("FixRotation" , &comp.FixedRotation);
                }

				ImGui::TreePop();
			}
		}

        if (entity.HasComponent<BoxCollider2DComponent>()) {
            if (ImGui::TreeNodeEx("BoxCollider2D", ImGuiTreeNodeFlags_OpenOnArrow)) {
                auto& comp = entity.GetComponent<BoxCollider2DComponent>();
                if (open) {
                    ImGui::DragFloat2("Size", glm::value_ptr(comp.Size));
                    ImGui::DragFloat2("Offset", glm::value_ptr(comp.Offset));

                    ImGui::DragFloat("Density" , &comp.Density);
                    ImGui::DragFloat("Friction", &comp.Friction);
                    ImGui::DragFloat("Restitution", &comp.Restitution);
                    ImGui::DragFloat("RestitutionThreshold", &comp.RestitutionThreshold);
                }

                ImGui::TreePop();
            }
        }
        
        
        if(entity.HasComponent<TextComponent>()){
            if (ImGui::TreeNodeEx("Text", ImGuiTreeNodeFlags_OpenOnArrow)) {
                auto& comp = entity.GetComponent<TextComponent>();
                if (open) {
                    char buff[256];
                    
                    strcpy(buff, comp.Text.c_str());
                    ImGui::LogText("%s", comp.Text.c_str());
                    if(ImGui::InputTextMultiline("TextContent", buff, sizeof(buff))){
                        comp.Text = buff;
                    }
                    ImGui::DragFloat("linaSpacing", &comp.textParam.LineSpacing , 0.1f , -5.0f ,100.0f);
                    ImGui::DragFloat("CharaterSpacing", &comp.textParam.CharacterSpacing , 0.1f , -5.0f ,100.0f);
                    ImGui::DragFloat2("Offset", glm::value_ptr(comp.textParam.offset));
                    ImGui::DragFloat2("Scale", glm::value_ptr(comp.textParam.scale));


//                    if(ImGui::Button("browse:")){
//                        auto fontPath = FileDialogs::OpenFile("ttf (*.ttf)\0 * .ttf\0");
//                        if(!fontPath.empty()){
//                            comp.FontPath = std::filesystem::relative(fontPath, Project::GetProjectDirectory()).generic_string();
//                            comp.TextFont.reset();
//                            comp.TextFont = std::make_shared<Font>(fontPath);
//                        }
//                    }
                    if (ImGui::BeginCombo("Font:", comp.TextFont ? comp.TextFont->GetName().c_str() : "None")) {
                        // 1. 提供一个清空选项
                        bool isNoneSelected = (comp.TextFont == nullptr);
                        if (ImGui::Selectable("None", isNoneSelected)) {
                            comp.TextFont = nullptr;
                        }

                        auto& fonts = Resource::GetResourceMap<Font>();
                        for (auto& [uuid, font] : fonts) {
                            bool isSelected = comp.TextFont && (comp.TextFont->GetUUID() == uuid);
                            if (ImGui::Selectable(font->GetName().c_str(), isSelected)) {
                                comp.TextFont = font;
                            }

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    if (ImGui::BeginDragDropTarget()) {
                        // 1. 接收 Payload
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_FONT")) {
                            
                            // 2. 获取数据：payload->Data 是 void*，指向之前 SetPayload 传入的 UUID 内存地址
                            // 我们将其强转为 UUID* 并解引用，拿到真正的 UUID 数值
                            UUID fontUUID = *(const UUID*)payload->Data;

                            // 3. 核心逻辑：使用 UUID 去资源管理器查找真正的 Ref<Material> 对象
                            // 假设你有这样一个全局静态方法
                            Ref<Font> newFont = Resource::GetResource<Font>(fontUUID);
                            
                            // 4. 赋值
                            if (newFont) {
                                comp.TextFont = newFont;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::SameLine();
                    if(!comp.FontPath.empty()){
                        ImGui::Text("%s", comp.FontPath.c_str());
                    }
                    comp.isUI = entity.HasComponent<UITransformComponent>();
                    ImGui::Checkbox("IsUI", &comp.isUI);
                    if(comp.isUI){
                        if(!entity.HasComponent<UITransformComponent>()){
                            entity.AddComponent<UITransformComponent>();
                        }
                        auto& uitransformc = entity.GetComponent<UITransformComponent>();
                        ImGui::DragFloat2("pos:", glm::value_ptr(uitransformc.UIpos) , 0.1f);
                        ImGui::DragFloat3("size:", glm::value_ptr(uitransformc.UISize), 0.1f);
                    }
                    else{
                        if(entity.HasComponent<UITransformComponent>()){
                            entity.RemoveComponent<UITransformComponent>();
                        }
                    }
                    
                    

                }

                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<Renderable>()) {
            auto& comp = entity.GetComponent<Renderable>();
            
            // 增加 Flags 使得点击整行都能展开，更符合习惯
            bool open = ImGui::TreeNodeEx("Renderable", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
            
            // 右键菜单移除组件的逻辑通常放在 TreeNode 这一行
            // (代码略，为了保持和你原有结构一致，放在最后处理)

            if (open) {
                // --- 1. Material Selector (新增部分) ---
                {
                    // 获取当前材质名称
                    // 假设 Resource::GetMaterial(uuid) 返回 Ref<Material>
                    Ref<Material> currentMaterial = comp.material;
                    std::string materialName = currentMaterial ? currentMaterial->GetName() : "None (Default)";

                    if (ImGui::BeginCombo("Material", materialName.c_str())) {
                        // 1. 提供一个清空选项
                        bool isNoneSelected = (comp.material == nullptr);
                        if (ImGui::Selectable("None", isNoneSelected)) {
                            comp.material = nullptr;
                        }

                        // 2. 遍历所有材质
                        // 假设 Resource::GetAllMaterials() 返回 std::unordered_map<UUID, Ref<Material>>
                        auto& materials = Resource::GetResourceMap<Material>();
                        for (auto& [uuid, mat] : materials) {
                            bool isSelected = comp.material && (comp.material->GetRendererID() == uuid);
                            if (ImGui::Selectable(mat->GetName().c_str(), isSelected)) {
                                comp.material = mat;
                            }

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    // --- 拖拽接收 (Drag & Drop Target) ---
                    // 允许从左侧资源面板拖动材质到这个 Combo 框上
                    if (ImGui::BeginDragDropTarget()) {
                        // 1. 接收 Payload
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_MATERIAL")) {
                            
                            // 2. 获取数据：payload->Data 是 void*，指向之前 SetPayload 传入的 UUID 内存地址
                            // 我们将其强转为 UUID* 并解引用，拿到真正的 UUID 数值
                            UUID materialUUID = *(const UUID*)payload->Data;

                            // 3. 核心逻辑：使用 UUID 去资源管理器查找真正的 Ref<Material> 对象
                            // 假设你有这样一个全局静态方法
                            Ref<Material> newMaterial = Resource::GetResource<Material>(materialUUID);
                            
                            // 4. 赋值
                            if (newMaterial) {
                                comp.material = newMaterial;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                {
                    // 获取当前 Text 材质名称
                    Ref<Material> currentTextMat = comp.textMat;
                    std::string textMatName = currentTextMat ? currentTextMat->GetName() : "None (Default Text Mat)";

                    if (ImGui::BeginCombo("Text Material", textMatName.c_str())) {
                        // 1. 提供一个清空选项
                        bool isNoneSelected = (comp.textMat == nullptr);
                        if (ImGui::Selectable("None", isNoneSelected)) {
                            comp.textMat = nullptr;
                        }

                        // 2. 遍历所有材质
                        auto& materials = Resource::GetResourceMap<Material>();
                        for (auto& [uuid, mat] : materials) {
                            // 这里可以过滤一下，只显示适合做文字渲染的材质（如果你的 Material 类有相关标记的话）
                            bool isSelected = comp.textMat && (comp.textMat->GetRendererID() == uuid);
                            if (ImGui::Selectable(mat->GetName().c_str(), isSelected)) {
                                comp.textMat = mat;
                            }

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    // --- 拖拽接收 (Drag & Drop Target) ---
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_MATERIAL")) {
                            
                            UUID materialUUID = *(const UUID*)payload->Data;
                            Ref<Material> newMaterial = Resource::GetResource<Material>(materialUUID);
                            
                            if (newMaterial) {
                                comp.textMat = newMaterial;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                
                ImGui::Separator();

                // --- 2. 原有的 RenderType 逻辑 ---
                // (即使选了材质，可能还需要保留这个逻辑来决定基础几何体的顶点属性生成方式，或者作为材质的参数输入)
                std::string QuadRenderType[2] = { "PureColor" , "Texture" };
                std::string currentRenderType = QuadRenderType[(int)comp.type];
                
                if (ImGui::BeginCombo("RenderType", currentRenderType.c_str())) {
                    for (int i = 0; i < 2; i++) {
                        bool isSelected = currentRenderType == QuadRenderType[i];
                        if (ImGui::Selectable(QuadRenderType[i].c_str(), isSelected)) {
                            currentRenderType = QuadRenderType[i];
                            comp.type = (RenderType)i;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                // ... 原有的属性面板逻辑 ...
                if (comp.type == RenderType::PureColor) {
                    ImGui::ColorEdit4("Color", glm::value_ptr(comp.m_Color));
                }
                else if (comp.type == RenderType::Texture) {
                    ImGui::Checkbox("Is Subtexture", &comp.isSubtexture);
                    
                    // 显示当前纹理路径（如果太长可以截断显示）
//                    std::string pathText = comp.subTexture ? comp.subTexture->GetTexture()->GetPath() : "None";
//                    ImGui::Text("UUID: %s", pathText.c_str());
                    
//                    ImGui::SameLine();
                    
                    
                    // 同样加上纹理的拖拽接收
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE")) {
                            UUID texUUID = *(const UUID*)payload->Data;
                            Ref<Texture2D> texture = Resource::GetResource<Texture2D>(texUUID);
                            if (texture) {
                                comp.subTexture = SubTexture2D::CreateByCoord(texture, comp.spriteSize, comp.textureIndex, comp.textureSize);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    if(comp.isSubtexture && comp.subTexture == nullptr){
                        TSO_CORE_ASSERT(comp.material != nullprt , "Entity cannot be subtexture without a material!");
                    }

                    if (comp.isSubtexture && comp.subTexture) {
                        bool dirty = false;
                        dirty |= ImGui::DragFloat2("Sprite Size", glm::value_ptr(comp.spriteSize), 1.0f);
                        dirty |= ImGui::DragFloat2("Sprite Index", glm::value_ptr(comp.textureIndex), 1.0f);
                        dirty |= ImGui::DragFloat2("Texture Size", glm::value_ptr(comp.textureSize), 1.0f);
                        
                        if (dirty) {
                            comp.subTexture->RecalculateCoords(comp.spriteSize, comp.textureIndex, comp.textureSize);
                        }
                    }
                }

                // --- UIView Logic ---
                bool& uiview = comp.uiview;
//                ImGui::Checkbox("UI View", &uiview);
                
                // 自动添加/移除组件的逻辑
//                if (uiview && !entity.HasComponent<UITransformComponent>()) {
//                    entity.AddComponent<UITransformComponent>();
//                }
//                if (!uiview && entity.HasComponent<UITransformComponent>()) {
//                    entity.RemoveComponent<UITransformComponent>();
//                }

                if (entity.HasComponent<UITransformComponent>()) {
                    auto& uit = entity.GetComponent<UITransformComponent>();
                    ImGui::DragFloat2("UI Pos", glm::value_ptr(uit.UIpos), 0.1f);
                    ImGui::DragFloat3("UI Size", glm::value_ptr(uit.UISize), 0.1f);
                }

                // --- Remove Component Logic ---
                // 这种在 Tree 内部的 remove 方式需要注意不要在 draw 过程中使得 entity 迭代器失效
                // 通常在 Inspector 中没问题，因为我们只操作单体
                if (ImGui::Button("Remove Component") ) {
                     entity.RemoveComponent<Renderable>();
                }

                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<MaterialInstanceComponent>()) {
            auto& comp = entity.GetComponent<MaterialInstanceComponent>();
            
            // 我们需要获取 Renderable 里的 Material 来知道要显示什么名字
            if (entity.HasComponent<Renderable>()) {
                auto& renderable = entity.GetComponent<Renderable>();
                Ref<Material> material = renderable.material;
                
                if (material) {
                    ImGui::Text("Instance Overrides:");
                    // 遍历材质定义的参数
                    for (auto& [name, slot] : material->GetInstanceParams()) {
                        
                        // 确保组件里有这个值，没有就初始化为 0
                        if (comp.FloatOverrides.find(name) == comp.FloatOverrides.end()) {
                            comp.FloatOverrides[name] = 0.0f;
                        }
                        
                        // 绘制滑块
                        ImGui::DragFloat(name.c_str(), &comp.FloatOverrides[name], 0.01f, 0.0f, 1.0f);
                    }
                }
            }
        }
		if (entity.HasComponent<CameraComponent>()) {
			if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_OpenOnArrow)) {
				std::string projectionTypeStrings[2] = { "Projection" , "Orthographic" };
				auto& component = entity.GetComponent<CameraComponent>();
				auto& camera = component.m_Camera;
				std::string currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("projectionType", currentProjectionTypeString.c_str())) {
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i].c_str(), isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Projection)
				{
					float perspectiveVerticalFov = glm::degrees(camera.GetProjectionFov());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
						camera.SetProjectionFov(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetProjectionNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetProjectionNearClip(perspectiveNear);

					float perspectiveFar = camera.GetProjectionFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetProjectionFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

                    if(ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio)){
                        component.m_Camera.SetFixAspectRatio(component.FixedAspectRatio);
                    }
                    ImGui::Checkbox("Primary", &component.m_Pramiary);

				}

				ImGui::TreePop();
			}
		}
        
        if (entity.HasComponent<NativeScriptComponent>()) {
            bool open = ImGui::TreeNodeEx("NativeScript", ImGuiTreeNodeFlags_OpenOnArrow);
            bool removeComponent = false;
            
            auto& nsc = entity.GetComponent<NativeScriptComponent>();
            //if(nsc.hasBind && nsc.Instance){
                //nsc.Instance->OnGUI();
            //}
            
            if(open){
                if(!nsc.hasBind){
                    if (ImGui::Button("Add Behavior"))
                        ImGui::OpenPopup("AddBehavior");
                    
                    if (ImGui::BeginPopup("AddBehavior"))
                    {
                       // BindNativeScriptBehavior<Controlable>("Controlable");
                       // BindNativeScriptBehavior<CircleBehavior>("CircleBehavior");
                        nsc.Bind<CircleBehavior>();
                        TSO_CORE_INFO("add circle");
                        ImGui::EndPopup();
                    }
                    
                }
                
                if (ImGui::MenuItem("Remove component"))
                    removeComponent = true;
                ImGui::TreePop();
            }
            if(removeComponent){
                entity.RemoveComponent<NativeScriptComponent>();
            }
            
        }

        if (entity.HasComponent<ScriptComponent>()) {

            auto& comp = entity.GetComponent<ScriptComponent>();

            auto& className = comp.ClassName;
            char buff[256];

            strcpy(buff, className.c_str());


            bool classExist = ScriptingEngine::EntityClassExists(comp.ClassName);
            if (!classExist) {
                ImGui::PushStyleColor(0, { 0.8 , 0.3 , 0.2 , 1.0 });
            }

            if (ImGui::InputText("ScriptClass", buff, sizeof(buff))) {
                comp.ClassName = buff;
            }
            if (!classExist) {
                ImGui::PopStyleColor();
            }

            
        }

        if (entity.HasComponent<NetworkComponent>()) {
            auto& comp = entity.GetComponent<NetworkComponent>();
            auto protocol = std::to_string(comp.protocol);
            char buff[256];

            strcpy(buff, protocol.c_str());

            if (ImGui::InputText("protocol", buff, sizeof(buff))) {
                comp.protocol = std::stoi(buff);
            }
        }
        
        if(entity.HasComponent<ButtonComponent>()){
            TSO_ASSERT(entity.HasComponent<UITransformComponent>() , "UI lacks virtual Transform component!");
            if (ImGui::TreeNodeEx("Button", ImGuiTreeNodeFlags_OpenOnArrow)) {
                auto& uitransformc = entity.GetComponent<UITransformComponent>();
                auto& comp = entity.GetComponent<ButtonComponent>();
                auto& pos = uitransformc.UIpos;
                auto& size = uitransformc.UISize;
                if (ImGui::DragFloat2("pos:", glm::value_ptr(pos) , 0.1f)) {
                    uitransformc.UIpos = pos;
                }
                if (ImGui::DragFloat3("size:", glm::value_ptr(size), 0.1f)) {
                    uitransformc.UISize = size;
                }
                ImGui::ColorEdit4("Hover Tint", glm::value_ptr(comp.HoverColor));
                ImGui::ColorEdit4("Click Tint", glm::value_ptr(comp.PressedColor));

                ImGui::TreePop();
            }
        }
        
        if(entity.HasComponent<InputFieldComponent>()){
            TSO_ASSERT(entity.HasComponent<UITransformComponent>() , "UI lacks virtual Transform component!");
            if (ImGui::TreeNodeEx("Input", ImGuiTreeNodeFlags_OpenOnArrow)) {
                auto& uitransformc = entity.GetComponent<UITransformComponent>();
                auto& comp = entity.GetComponent<InputFieldComponent>();
                auto& pos = uitransformc.UIpos;
                auto& size = uitransformc.UISize;
                if (ImGui::DragFloat2("pos:", glm::value_ptr(pos) , 0.1f)) {
                    uitransformc.UIpos = pos;
                }
                if (ImGui::DragFloat3("size:", glm::value_ptr(size), 0.1f)) {
                    uitransformc.UISize = size;
                }
                ImGui::ColorEdit4("color", glm::value_ptr(comp.FocusedColor));

                ImGui::TreePop();
            }
        }
        
        
	}


template<typename T>
    void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
        if (!m_SelectedEntity.HasComponent<T>())
        {
            if (ImGui::MenuItem(entryName.c_str()))
            {
                m_SelectedEntity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }




}
