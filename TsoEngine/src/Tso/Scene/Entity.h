#pragma once
#include "Scene.h"
#include <utility>


namespace Tso {

	class Entity {
	public:
		Entity() = delete;
		Entity(const entt::entity& entityId , Scene* scene , const std::string& name = std::string());

		template<typename T , typename... Arg>
        T AddComponent(Arg&& ...arg){
			return m_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Arg>(arg)...);
        }
		template<typename T>
		T GetComponent() {
			TSO_CORE_ASSERT(m_Scene->m_Registry.has<T>(m_EntityID));
			return m_Scene->m_Registry.get<T>(m_EntityID);
		}
	private:

		Scene* m_Scene = nullptr;
		entt::entity m_EntityID = entt::null ;
        std::string m_Name = "";

	};


}
