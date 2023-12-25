#pragma once
#include "entt.hpp"

namespace Tso {
	class Entity;
	class Scene {
		friend class Entity;
	public:
		Scene() = default;
		~Scene() {}

		Entity CreateEntity();

		void OnUpdate(TimeStep ts);

	private:
		entt::registry m_Registry;

		float m_Time = 0.f;

	};



}