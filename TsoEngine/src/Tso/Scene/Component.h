#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Tso/Core/TimeStep.h"
#include "ScriptableEntity.h"
#include "SceneCamera.h"
#include "Tso/Renderer/Texture.h"
#include "Tso/Renderer/SubTexture2D.h"
#include "Tso/Renderer/Font.h"
#include "Tso/Core/UUID.h"
#include "Tso/Scene/Seriealizer.h"
#include "Tso/Network/ByteStream.h"
#include "sol/sol.hpp"
#include "Renderer/Material.h"

namespace Tso {

	enum class ComponentID: uint16_t {
		None = 0,
		TransformComponent = 1,
		Renderable = 2,
		TagComponent = 3,
		IDComponent = 4,
		NativeScriptComponent = 5,
		ScriptComponent = 6,
		CameraComponent = 7,
		Rigidbody2DComponent = 8, 
		BoxCollider2DComponent = 9,
		TextComponent = 10
	};

	struct TransformComponent {
        TransformComponent(){}
		TransformComponent(const glm::mat4& transform = glm::mat4(1.0));
		TransformComponent(const glm::vec3& pos = glm::vec3(1.0,1.0,1.0));
		TransformComponent(const TransformComponent& transform) = default;

        void SetPos(const glm::vec3& pos) { m_Translation = pos; }
		void SetRotate(const glm::vec3& rotate) { m_Rotation = rotate; }
		void SetScale(const glm::vec3& scale) { m_Scale = scale; }

        glm::vec3& GetPos(){return m_Translation;}
		glm::vec3& GetRotate() { return m_Rotation; }
		glm::vec3& GetScale() { return m_Scale; }
        
        glm::mat4& GetTransform(){
            
            m_Transform = glm::translate(glm::mat4(1.0f) , m_Translation) *
				  glm::rotate(glm::mat4(1.0f) , glm::radians(m_Rotation.x) , glm::vec3(1.0f , 0.0f , 0.0f)) *
                  glm::rotate(glm::mat4(1.0f) , glm::radians(m_Rotation.y) , glm::vec3(0.0f , 1.0f , 0.0f)) *
                  glm::rotate(glm::mat4(1.0f) , glm::radians(m_Rotation.z) , glm::vec3(0.0f , 0.0f , 1.0f)) *
                glm::scale(glm::mat4(1.0f) , m_Scale);
            
            return m_Transform;
        }
        
		glm::vec3 m_Translation = glm::vec3(0.0f, 0.0f, 0.0f);
        
        glm::vec3 m_Scale = glm::vec3(1.0f , 1.0f , 1.0f);
        
        glm::vec3 m_Rotation = glm::vec3(0.0f , 0.0f , 0.0f);
        
        glm::mat4 m_Transform = glm::mat4(1.0f);
	};


    enum RenderType {
        PureColor = 0,
        Texture = 1,
    };

	struct Renderable  {
        
		Renderable() = default;
		Renderable(const Renderable& other);

		Renderable(const glm::vec4& color);

		void Render(const glm::mat4& transform);

        RenderType type = PureColor;
        bool isSubtexture = false;
		glm::vec4 m_Color = glm::vec4(0.3 , 0.8 , 0.2 , 1.0);
        Ref<SubTexture2D> subTexture;
        Ref<Material> material = nullptr;
        Ref<Material> textMat = nullptr;
        
        glm::vec2 spriteSize = {1.0 , 1.0};
        glm::vec2 textureIndex = {0.0 , 0.0};
        glm::vec2 textureSize = {1.0 , 1.0};
        bool uiview = false;
	};

    struct MaterialInstanceComponent {
            // 存储实例数据：Key 是参数名，Value 是数值
            // 这里我们假设提供 4 个 float 的通用槽位（足够绝大多数 2D 效果使用）
            // 如果需要更多，可以用 glm::vec4 或者 std::vector
            std::unordered_map<std::string, float> FloatOverrides;
            // std::unordered_map<std::string, glm::vec3> Vec3Overrides; // 按需扩展
            
            MaterialInstanceComponent() = default;
            MaterialInstanceComponent(const MaterialInstanceComponent&) = default;
        };

	struct TagComponent  {
		TagComponent() = default;
		TagComponent(const std::string& name = "blankNameEntity");

		std::string& GetTagName() { return m_Name; }

		void SetTagName(const std::string& name) { m_Name = name; }
		std::string m_Name = "";
	};

    struct IDComponent{
        IDComponent() = default;
        IDComponent(const UUID& uuid):ID(uuid){}
        IDComponent(const IDComponent& other) = default;
        UUID ID;
    };



	class ScriptableEntity;

	struct NativeScriptComponent {

		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
        bool hasBind = false;
		//std::function<ScriptableEntity* ()>InstantiateScript;
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			auto lambda = []()->ScriptableEntity* {
				 return static_cast<ScriptableEntity*>(new T());
				};
			InstantiateScript = lambda;
            DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
            hasBind = true;
        }
        
        void UnBind(){
            DestroyScript(this);
            hasBind = false;
        }

	};

	struct ScriptComponent {
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent& sc) = default;

		std::string ClassName = "";
	};


	struct CameraComponent{
		CameraComponent(){}
    
		bool m_Pramiary = false;
		bool FixedAspectRatio = false;

		SceneCamera m_Camera;
    
	};


	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};



    struct TextParam{
        float CharacterSpacing = 0.0;
        float LineSpacing = 0.0;
        glm::vec2 scale = glm::vec2(1.f);
        glm::vec2 offset = glm::vec2(0.f);
    };

    struct TextComponent{
        TextComponent();
        TextComponent(const std::string& fontPath);
        std::string Text = "";
        std::string FontPath = "";
        Ref<Font> TextFont = nullptr;
        TextParam textParam;
        bool isUI = false;

    };

	struct ActiveComponent {
		ActiveComponent() = default;
		ActiveComponent(const bool& active) :Active(active) {

		}
		bool Active = true;
	};

	struct NetworkComponent
	{
		// we don't set network id additionally, UUID can handle this
		bool authoritative = false;
		ByteStream byte;// implemented as vector , so it is okay write nothing for initial
		uint8_t protocol = 0; // see more information in networkEngine.h
		
	};

    struct ButtonComponent {
        // 状态
        bool IsHovered = false;
        bool IsPressed = false;
        
        // 视觉属性
        glm::vec4 NormalColor = {1.f, 1.f, 1.f, 1.0f};
        glm::vec4 HoverColor = {0.9f, 0.9f, 0.9f, 1.0f};
        glm::vec4 PressedColor = {0.7f, 0.7f, 0.7f, 1.0f};
        
        // 回调
        sol::function OnClick;
        
        // 构造函数，以便在AddComponent时使用
        ButtonComponent() = default;
        ButtonComponent(const ButtonComponent&) = default;
    };

    struct InputFieldComponent {
        std::string Text;
        std::string PlaceholderText = "Enter text...";
        bool IsFocused = false;
        
        // 视觉属性 (例如，聚焦时的背景色)
        glm::vec4 FocusedColor = {1.0f, 1.0f, 1.0f, 1.0f};
        //tem UItransform, separate to a new component later
        
        // 回调
        sol::function OnValueChanged;
        sol::function OnSubmit; // 例如按回车时
        
        InputFieldComponent() = default;
        InputFieldComponent(const InputFieldComponent&) = default;
    };

struct UITransformComponent{
   //all values here are based on virtual resolution canvas.
    //(0,0) for left bottom and (UISystem::VirutalResolutionX , UISystem::VirutalResolutionX) for right top.
    glm::vec2 UIpos = {1000.f , 1000.f};
    glm::vec2 UISize = {100.f , 100.f};
    float Rotation_Z = 0.f;
};


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent , Renderable,
		CameraComponent,ScriptComponent,
		 Rigidbody2DComponent, BoxCollider2DComponent,
		TextComponent>;

	

	

}
