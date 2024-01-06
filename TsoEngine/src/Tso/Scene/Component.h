#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Tso/Core/TimeStep.h"
#include "ScriptableEntity.h"
#include "SceneCamera.h"

namespace Tso {



	struct TransformComponent {
		TransformComponent() = delete;
		TransformComponent(const glm::mat4& transform = glm::mat4(1.0));
		TransformComponent(const glm::vec3& pos = glm::vec3(1.0,1.0,1.0));
		TransformComponent(const TransformComponent& transform);

        void SetPos(const glm::vec3& pos) { m_Translation = pos; }
        glm::vec3& GetPos(){return m_Translation;}
        
        glm::mat4 GetTransform(){
            glm::mat4 res = glm::mat4(1.0f);
            
            res = glm::translate(glm::mat4(1.0f) , m_Translation) * glm::scale(glm::mat4(1.0f) , m_Scale) *                              glm::rotate(glm::mat4(1.0f) , m_Rotation.x , glm::vec3(1.0f , 0.0f , 0.0f)) *
                  glm::rotate(glm::mat4(1.0f) , m_Rotation.y , glm::vec3(0.0f , 1.0f , 0.0f)) *
                  glm::rotate(glm::mat4(1.0f) , m_Rotation.z , glm::vec3(0.0f , 0.0f , 1.0f));
            
            return res;
        }
        
        glm::vec3 m_Translation = glm::vec3(0.0f , 0.0f , 0.f);
        
        glm::vec3 m_Scale = glm::vec3(1.0f , 1.0f , 1.0f);
        
        glm::vec3 m_Rotation = glm::vec3(0.0f , 0.0f , 0.0f);
	};

	struct Renderable  {
		Renderable() = delete;
		Renderable(const glm::vec4& color);

		void Render(const glm::mat4& transform);

		glm::vec4 m_Color = glm::vec4(0.3 , 0.8 , 0.2 , 1.0);
	};

	struct TagComponent  {
		TagComponent() = delete;
		TagComponent(const std::string& name = "blankNameEntity");

		std::string& GetTagName() { return m_Name; }

		void SetTagName(const std::string& name) { m_Name = name; }
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


struct CameraComponent{
    CameraComponent() = delete;
    CameraComponent(const std::string& name = "blankNameEntity");
    
    SceneCamera m_Camera;
    
    
};

	

	

}
