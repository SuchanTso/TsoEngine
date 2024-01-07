#include "TPch.h"
#include "ScriptableEntity.h"
#include "Component.h"
#include "Tso/Core/Input.h"
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

	void Controlable::OnUpdate(const TimeStep& ts)
	{
		auto& transform = GetComponent<TransformComponent>();
		auto& pos = transform.GetPos();
		if (Input::IsKeyPressed(TSO_KEY_W)) {
			pos.y += m_MoveSpeed * ts;
		}
		if (Input::IsKeyPressed(TSO_KEY_S)) {
			pos.y -= m_MoveSpeed * ts;
		}
		if (Input::IsKeyPressed(TSO_KEY_A)) {
			pos.x -= m_MoveSpeed * ts;
		}
		if (Input::IsKeyPressed(TSO_KEY_D)) {
			pos.x += m_MoveSpeed * ts;
		}
	}

}