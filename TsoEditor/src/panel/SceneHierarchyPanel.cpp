#include "SceneHierarchyPanel.h"
#include "imgui.h"
#include "Tso/Scene/Component.h"
#include "glm/gtc/type_ptr.hpp"
#include "Tso/Scene/SceneCamera.h"

namespace Tso {
	void SceneHierarchyPanel::OnGuiRender()
	{
		ImGui::Begin("SceneNode");

		m_Context->m_Registry.each([&](auto entity) {
			Entity e( entity , m_Context.get() );
			DrwaEntityNode(e);
			});
        
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
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;

            ImGui::EndPopup();
        }

		if (opened) {
			ImGui::TreePop();
		}
        if(entityDeleted){
            m_Context->DeleteEntity(entity);
            if (m_SelectedEntity == entity)
                m_SelectedEntity = Entity{entt::null , m_Context.get()};
        }
	}
	void SceneHierarchyPanel::DrawComponents(Entity& entity)
{
        
        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");
        
        if (ImGui::BeginPopup("AddComponent"))
        {
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<NativeScriptComponent>("NativeScript");

            
            ImGui::EndPopup();
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

		if (entity.HasComponent<Renderable>()) {
			if (ImGui::TreeNodeEx("Renderable", ImGuiTreeNodeFlags_OpenOnArrow)) {
				auto& comp = entity.GetComponent<Renderable>();
				ImGui::ColorEdit4("color", glm::value_ptr(comp.m_Color));
				ImGui::TreePop();
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

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}

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
