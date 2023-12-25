#pragma once
#include "Scene.h"


namespace Tso {

	class Entity {
	public:
		Entity() = delete;
		Entity(const entt::entity& entityId , Scene* scene , const std::string& name = std::string());

		template<typename T , typename... Arg>
        void AddComponent(Arg&& ...arg){
//            AddComponent<T>(std::forward<Arg>(arg)...);
        }
	private:

		Scene* m_Scene = nullptr;
		entt::entity m_EntityID = entt::null ;
        std::string m_Name = "";

	};


}
