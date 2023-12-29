#pragma once
#include "glm/glm.hpp"
#include "Tso/Core/TimeStep.h"
#include "ScriptableEntity.h"

namespace Tso {

	class Component {
	public:
		Component() = default;
		~Component() {}
        
	};


	struct TransformComponent {
	public:
		TransformComponent() = delete;
		TransformComponent(const glm::mat4& transform = glm::mat4(1.0));
		TransformComponent(const glm::vec3& pos = glm::vec3(1.0,1.0,1.0));
		TransformComponent(const TransformComponent& transform);

		glm::vec3& GetPos() { return m_Pos; }

		void SetRand(const float& rand) { m_Rand = rand; }

		float GetRand() { return m_Rand; }

        void SetPos(const glm::vec3& pos) { m_Pos = pos; }

		glm::vec3 m_Pos = glm::vec3(0.0, 0.0, -0.5);

		float m_Rand = -1.f;
	};

	class Renderable : public Component {
	public:
		Renderable() = delete;
		Renderable(const glm::vec4& color);

		void Render(const glm::vec3& pos);


	private:
		glm::vec4 m_Color = glm::vec4(0.3 , 0.8 , 0.2 , 1.0);
	};

	class TagComponent : public Component {
	public:
		TagComponent() = delete;
		TagComponent(const std::string& name = "blankNameEntity");

		std::string GetTagName() { return m_Name; }

		void SetTagName(const std::string& name) { m_Name = name; }
	private:
		std::string m_Name = "";
	};



	class ScriptableEntity;

	struct NativeScriptComponent {

		ScriptableEntity* Instance = nullptr;
		std::string test = "null";

		ScriptableEntity* (*InstantiateScript)();
		//std::function<ScriptableEntity* ()>InstantiateScript;
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			auto lambda = []()->ScriptableEntity* {
				TSO_INFO("On Bind!"); return static_cast<ScriptableEntity*>(new T());
				};
			InstantiateScript = lambda;
			if (InstantiateScript) {
				TSO_CORE_INFO("BIiiid not null");
				test = "not null";
			}
			else {
				TSO_CORE_INFO("BIiiid but null");
			}
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}

	};

	

	

}
