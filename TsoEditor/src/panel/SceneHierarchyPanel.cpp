#include "SceneHierarchyPanel.h"
#include "imgui.h"
#include "Tso/Scene/Component.h"
#include "glm/gtc/type_ptr.hpp"

namespace Tso {
	void SceneHierarchyPanel::OnGuiRender()
	{
		ImGui::Begin("SceneNode");

		m_Context->m_Registry.each([&](auto entity) {
			Entity e( entity , m_Context.get() );
			DrwaEntityNode(e);
			});

		ImGui::End();


		ImGui::Begin("Components");
		if (m_SelectedEntity.m_EntityID != entt::null) {
			DrawComponents(m_SelectedEntity);
		}
		ImGui::End();
	}
	void SceneHierarchyPanel::DrwaEntityNode(Entity& entity)
	{

		auto& tag = entity.GetComponent<TagComponent>().GetTagName();
		ImGuiTreeNodeFlags flag = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flag, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
		}

		if (opened) {
			ImGui::TreePop();
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity& entity)
	{
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().GetTagName();

			ImGui::Text("%s", tag.c_str());
		}
		if (entity.HasComponent<TransformComponent>()) {
			auto& comp = entity.GetComponent<TransformComponent>();
			auto& pos = comp.GetPos();
			if (ImGui::DragFloat3("pos:", glm::value_ptr(pos))) {
				comp.SetPos(pos);
			}
		}
	}
}