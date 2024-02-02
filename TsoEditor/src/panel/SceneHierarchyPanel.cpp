#include "SceneHierarchyPanel.h"
#include "imgui.h"
#include "Tso/Scene/Component.h"
#include "glm/gtc/type_ptr.hpp"
#include "Tso/Scene/SceneCamera.h"
#include "Tso/Utils/PlatformUtils.h"
#include "Tso/Renderer/Font.h"
#include "Tso/Scene/ScriptableEntity.h"

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
            DisplayAddComponentEntry<Renderable>("Renderable");
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<NativeScriptComponent>("NativeScript");
            DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody2DComponent");
            DisplayAddComponentEntry<BoxCollider2DComponent>("BoxCollider2DComponent");
            DisplayAddComponentEntry<TextComponent>("TextComponent");
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

                    if(ImGui::Button("browse:")){
                        auto fontPath = FileDialogs::OpenFile("ttf (*.ttf)\0 * .ttf\0");
                        if(!fontPath.empty()){
                            comp.FontPath = fontPath;
                            comp.TextFont.reset();
                            comp.TextFont = std::make_shared<Font>(fontPath);
                        }
                    }
                    ImGui::SameLine();
                    if(!comp.FontPath.empty()){
                        ImGui::Text("%s", comp.FontPath.c_str());
                    }

                }

                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<Renderable>()) {
            auto& comp = entity.GetComponent<Renderable>();
            
            bool open = ImGui::TreeNodeEx("Renderable", ImGuiTreeNodeFlags_OpenOnArrow);
            std::string QuadRenderType[2] = { "PureColor" , "Texture" };
            std::string currentRenderType = QuadRenderType[(int)comp.type];
            if(open){
                if (ImGui::BeginCombo("RenderType", currentRenderType.c_str())) {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentRenderType == QuadRenderType[i];
                        if (ImGui::Selectable(QuadRenderType[i].c_str(), isSelected))
                        {
                            currentRenderType = QuadRenderType[i];
                            comp.type = (RenderType)i;
                        }
                        
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    
                    ImGui::EndCombo();
                }
                bool removeComponent = false;
                
                if(comp.type == RenderType::PureColor){
                    ImGui::ColorEdit4("color", glm::value_ptr(comp.m_Color));
                }
                else if(comp.type == RenderType::Texture){
                    ImGui::Checkbox("isSubtexture", &comp.isSubtexture);
                    
                    ImGui::Text("Path:%s",comp.subTexture ? comp.subTexture->GetTexture()->GetPath().c_str() : "");
                    ImGui::SameLine();
                    if (ImGui::Button("browse")) {
                        auto TexturePath = FileDialogs::OpenFile("png (*.png)\0 * .png\0");
                        if (!TexturePath.empty()) {
                            auto texture = Texture2D::Create(TexturePath);
                            comp.subTexture.reset();
                            comp.subTexture = SubTexture2D::CreateByCoord(texture, comp.spriteSize, comp.textureIndex, comp.textureSize);
                        }
                    }
                    if(comp.isSubtexture){
                        bool spriteSizeDirty = ImGui::DragFloat2("SpriteSize" , glm::value_ptr(comp.spriteSize) , 1.0f);
                        bool spriteIndexDirty = ImGui::DragFloat2("SpriteIndex" , glm::value_ptr(comp.textureIndex) , 1.0f);
                        bool textureSizeDirty = ImGui::DragFloat2("TextureSize" , glm::value_ptr(comp.textureSize) , 1.0f);
                        if(spriteSizeDirty || spriteIndexDirty || textureSizeDirty){
                            comp.subTexture->RecalculateCoords(comp.spriteSize, comp.textureIndex, comp.textureSize);
                        }
                    }
                }
                if (ImGui::MenuItem("Remove component"))
                    removeComponent = true;
                ImGui::TreePop();
                
                if(removeComponent){
                    entity.RemoveComponent<Renderable>();
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

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
                    ImGui::Checkbox("Primary", &component.m_Pramiary);

				}

				ImGui::TreePop();
			}
		}
        
        if (entity.HasComponent<NativeScriptComponent>()) {
            bool open = ImGui::TreeNodeEx("NativeScript", ImGuiTreeNodeFlags_OpenOnArrow);
            bool removeComponent = false;
            
            auto& nsc = entity.GetComponent<NativeScriptComponent>();
            if(nsc.hasBind && nsc.Instance){
                nsc.Instance->OnGUI();
            }
            
            if(open){
                if(!nsc.hasBind){
                    if (ImGui::Button("Add Behavior"))
                        ImGui::OpenPopup("AddBehavior");
                    
                    if (ImGui::BeginPopup("AddBehavior"))
                    {
                        BindNativeScriptBehavior<Controlable>("Controlable");
                        BindNativeScriptBehavior<CircleBehavior>("CircleBehavior");
                        
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
