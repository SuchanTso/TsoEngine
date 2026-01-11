#pragma once
#include"Tso/Scene/Scene.h"
#include "Tso/Core/Core.h"
#include "Tso/Scene/Entity.h"

namespace Tso {
	class Entity;
    class Material;
    class Texture2D;
    class Shader;
    struct ShaderEditorSession {
        UUID ShaderUUID = 0;
        // 缓存每个阶段的源码: Stage Type -> Source Code Buffer
        // 注意：InputText 需要可变长的 char buffer，这里用 std::string 简化管理，但在 ImGui 调用时需注意扩容
        std::unordered_map<unsigned int, std::string> SourceBuffers;
        bool IsDirty = false;
    };
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel(){}

		void SetContext(Ref<Scene> context) { m_Context = context; }
        
        template<typename T>
        void DisplayAddComponentEntry(const std::string& entryName);

		void OnGuiRender();
        void SetSelectedEntity(const Entity& entity){m_SelectedEntity = entity;}
        
        void DrawResourceList();      // 绘制左侧列表
        void DrawResourceInspector(); // 绘制右侧详细信息
        
    private:

        void DrawMaterialNode(Ref<Material> material);
        void DrawTextureNode(Ref<Texture2D> texture);
        void DrawShaderNode(Ref<Shader> shader);
        void SyncShaderToCache(Ref<Shader> shader);
        void CreateMaterialPrompt();

        void DrawCreateMaterialPopup();
        
        void CreateShaderPrompt();

        void DrawCreateShaderPopup();

        
        
		void DrwaEntityNode(Entity& entity);
		void DrawComponents(Entity& entity);
        void DrawResources();
	private:
		friend class Scene;
		Ref<Scene> m_Context = nullptr;
		Entity m_SelectedEntity{entt::null , nullptr};
		Entity m_DeletedEntity{ entt::null , nullptr };
        enum class ResourceType { None, Texture, Shader, Material , Font };
        UUID m_SelectedResourceUUID = 0;
        ResourceType m_SelectedType = ResourceType::None;
        ShaderEditorSession m_ShaderCache;
        
        enum class DeferredAction {
                None,
                CreateMaterial,
                CreateShader,
                ImportTexture,
                ImportShader,
                ImportFont,
            };
            
        DeferredAction m_NextAction = DeferredAction::None;

        bool m_RequestSelectTexture = false;
        std::string m_TextureSlotToChange;
	};

}
