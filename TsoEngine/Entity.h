#pragma once
#include "Scene.h"


namespace Tso {

	class Entity {
	public:
		Entity() = delete;
		Entity(const entt::entity& entityId ,  Scene* scene , const std::string& name = std::string());

		
	private:

		Scene* m_Scene = nullptr;
		entt::entity m_EntityID = entt::null ;

	};


}