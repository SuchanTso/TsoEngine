#pragma once
#include"Tso/Scene/Scene.h"
#include "Tso/Core/Core.h"
#include "Tso/Scene/Entity.h"

namespace Tso {
	class Entity;
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel(){}

		void SetContext(Ref<Scene> context) { m_Context = context; }
        
        template<typename T>
        void DisplayAddComponentEntry(const std::string& entryName);

		void OnGuiRender();
        void SetSelectedEntity(const Entity& entity){m_SelectedEntity = entity;}
	private:
		void DrwaEntityNode(Entity& entity);
		void DrawComponents(Entity& entity);
	private:
		friend class Scene;
		Ref<Scene> m_Context = nullptr;
		Entity m_SelectedEntity{entt::null , nullptr};

	};

}
