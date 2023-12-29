#include "TPch.h"
#include "ScriptableEntity.h"
#include "Component.h"
namespace Tso {




	void CircleBehavior::OnUpdate(const TimeStep& ts)
	{
		TSO_CORE_INFO("On Update!");
		TransformComponent& comp = GetComponent<TransformComponent>();
		m_Time += ts;
		auto& pos = comp.GetPos();
		pos.x = cos(m_Time);
		pos.y = sin(m_Time);
	}

}