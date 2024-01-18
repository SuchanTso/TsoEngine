#include "TPch.h"
#include "ScriptableEntity.h"
#include "Component.h"
#include "Tso/Core/Input.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
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
		if (HasComponent<Rigidbody2DComponent>()) {
			auto& rigidc = GetComponent<Rigidbody2DComponent>();
			if (rigidc.RuntimeBody) {
				b2Body* body = (b2Body*)rigidc.RuntimeBody;
				
				if (Input::IsKeyPressed(TSO_KEY_W)) {
					body->SetLinearVelocity({ 0.0f , 2.0f });
				}

				if (Input::IsKeyPressed(TSO_KEY_S)) {
					body->SetLinearVelocity({ 0.0f , -2.0f });
				}
				if (Input::IsKeyPressed(TSO_KEY_A)) {
					body->SetLinearVelocity({ -2.0f , 0.0f });
				}
				
				if (Input::IsKeyPressed(TSO_KEY_D)) {
					body->SetLinearVelocity({ 2.0f , 0.0f });
				}
				
			}
		}
		
		
	}

}