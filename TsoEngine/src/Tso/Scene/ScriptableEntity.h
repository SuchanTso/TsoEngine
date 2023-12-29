#pragma once
#include "Entity.h"
#include "Tso/Core/Log.h"
namespace Tso {

	class ScriptableEntity {
	public:
		ScriptableEntity() {}
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		virtual void OnCreate(){}

		virtual void OnUpdate(const TimeStep& ts){}

		virtual void OnDestroy(){}


	private:
		Entity m_Entity;
		friend class Scene;
	};


	class CircleBehavior : public ScriptableEntity {
	public:
		CircleBehavior() {}
		virtual ~CircleBehavior() {}
		virtual void OnCreate()override {
			TSO_CORE_INFO("On Create!");
		}
		virtual void OnUpdate(const TimeStep& ts)override;
		virtual void OnDestroy()override {}

	private:
		float m_Time = 0.f;
	};

}