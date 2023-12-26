#pragma once
#include "glm/glm.hpp"
#include "Tso/Core/TimeStep.h"
namespace Tso {

	class Component {
	public:
		Component() = default;
		~Component() {}
		virtual void OnUpdate(TimeStep ts) = 0;
        
        template<typename T , typename Arg>
        static void AddComponent(const Arg&& arg...);
	};


	class TransformComponent : public Component {
	public:
		TransformComponent() = delete;
		TransformComponent(const glm::mat4& transform = glm::mat4(1.0));
		TransformComponent(const glm::vec3& pos = glm::vec3(1.0,1.0,1.0));
		TransformComponent(const TransformComponent& transform);

		glm::vec3& GetPos() { return m_Pos; }

        void SetPos(const glm::vec3& pos) { m_Pos = pos; }

		virtual void OnUpdate(TimeStep ts)override;
        
	private:
		glm::vec3 m_Pos = glm::vec3(0.0, 0.0, -0.5);
	};

	class Renderable : public Component {
	public:
		Renderable() = delete;
		Renderable(const glm::vec4& color);

		void Render(const glm::vec3& pos);

		virtual void OnUpdate(TimeStep ts)override;

	private:
		glm::vec4 m_Color = glm::vec4(0.3 , 0.8 , 0.2 , 1.0);
	};
}
