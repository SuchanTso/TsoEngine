#include "TPch.h"
#include "Entity.h"
#include "Component.h"

namespace Tso {




Entity::Entity(const entt::entity& entityId, Scene* scene, const std::string& name)
:m_Scene(scene) , m_EntityID(entityId) , m_Name(name)
	{
//        Component::AddComponent<TransformComponent>({1.0f , 1.0f , 1.0f});
        
	}

}
