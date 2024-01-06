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

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			m_SelectedEntity.m_EntityID = entt::null;
		}

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
				if (ImGui::DragFloat3("pos:", glm::value_ptr(pos) , 0.1f)) {
					comp.SetPos(pos);
				}
				ImGui::TreePop();
			}
		}
	}
}
