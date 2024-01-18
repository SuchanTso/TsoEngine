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

		template<typename T>
		bool HasComponent() {
			return m_Entity.HasComponent<T>();
		}

		virtual void OnCreate(){}

		virtual void OnUpdate(const TimeStep& ts){}

		virtual void OnDestroy(){}

		virtual void OnCollide() { TSO_CORE_INFO("entity get collide!!"); }


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

	class Controlable : public ScriptableEntity {
	public:
		Controlable() {}
		virtual ~Controlable() {}
		virtual void OnCreate()override {
			TSO_CORE_INFO("On Create!");
		}
		virtual void OnUpdate(const TimeStep& ts)override;
		virtual void OnDestroy()override {}

	private:
		float m_Time = 0.f;
		float m_MoveSpeed = 10.f;
	};

}