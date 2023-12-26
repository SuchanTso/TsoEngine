#pragma once
#include "entt.hpp"

namespace Tso {
	class Entity;
	class Scene {
		friend class Entity;
	public:
		Scene() = default;
		~Scene() {}

		Entity CreateEntity(const std::string& name = "");

		void OnUpdate(TimeStep ts);

		std::vector<std::string> GetSceneEntityNames();

	private:
		entt::registry m_Registry;

		float m_Time = 0.f;

		uint32_t m_EntityCount = 0;

	};



}