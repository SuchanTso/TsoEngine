#pragma once
#include "entt.hpp"

namespace Tso {
	class Entity;
	class Scene {
	public:
		Scene() = default;
		~Scene() {}

		Entity CreateEntity();

	private:
		entt::registry m_Registry;

	};



}