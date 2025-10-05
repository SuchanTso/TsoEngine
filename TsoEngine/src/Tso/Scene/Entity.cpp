#include "TPch.h"
#include "Entity.h"
#include "Component.h"

namespace Tso {




Entity::Entity(const entt::entity& entityId, Scene* scene, const std::string& name)
:m_Scene(scene) , m_EntityID(entityId) , m_Name(name)
	{
	}

Entity::~Entity(){
    
//    TSO_CORE_INFO("entity {} has destroyed" , uint32_t(m_EntityID));
    
}

glm::mat4 Entity::GetWorldTransform()
{
	TSO_CORE_ASSERT(HasComponent<TransformComponent>(), "an entity without transformcomponent is not allowed");
	auto& transComp = GetComponent<TransformComponent>();
	if (GetParent() == nullptr) {
		return transComp.GetTransform();
	}
	else {
		return GetParent()->GetWorldTransform() * transComp.GetTransform();
	}
	return glm::mat4(1.0f);
}

glm::mat4 Entity::GetParentTransform()
{
	TSO_CORE_ASSERT(HasComponent<TransformComponent>(), "an entity without transformcomponent is not allowed");
	auto& transComp = GetComponent<TransformComponent>();
	if (GetParent() == nullptr) {
		return glm::mat4(1.0f);
	}
	else {
		return GetParent()->GetWorldTransform() * glm::mat4(1.0f);
	}
	return glm::mat4(1.0f);
}

void Entity::AddChild(Entity& child)
{
	m_Scene->SetEntityParent(*this, child);
}

void Entity::SetParent(Entity& parent)
{
	m_Scene->SetEntityParent(parent, *this);
}


void Entity::RemoveChild(Entity& child)
{
	m_Scene->RemoveChild(*this, child);
}

uint64_t Entity::GetUUID(){
    return GetComponent<IDComponent>().ID;
}

Entity::operator bool(){
        // 一个实体有效的条件是它有一个有效的句柄（不是null）并且它属于一个场景
        return m_EntityID != entt::null && m_Scene != nullptr;
}


}
