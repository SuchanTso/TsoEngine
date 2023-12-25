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
		TransformComponent(const TransformComponent& transform);

		virtual void OnUpdate(TimeStep ts)override;
        
        static void AddComponent(const glm::vec3& pos);
	};
}
