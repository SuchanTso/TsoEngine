#pragma once
#include <filesystem>
#include "Tso/Scene/Entity.h"
#include "Tso/Core/TimeStep.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoString MonoString;
}
namespace Tso {
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity,
		SpriteAnimationIdle
	};

	struct ScriptField
	{
		ScriptFieldType Type = ScriptFieldType::None;
		std::string Name = "";
		MonoClassField* ClassField = nullptr;
	};

	// ScriptField + data storage
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class ScriptingEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptingEngine {
	public :
		ScriptingEngine() = default;
		~ScriptingEngine() = default;
		static void Init();
		static void ShutDown();
		static bool EntityClassExists(const std::string& className);
		static void OnScenePlay(Scene* context);
		static void OnSceneStop();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, TimeStep ts);
		static Scene* GetSceneContext();



	private:
		static void InitMono();
		static void ShutdownMono();
		static bool LoadAssembly(const std::filesystem::path& filepath);
		static void LoadAssemblyClasses();
		static MonoObject* InstantiateClass(MonoClass* monoClass);


		friend class ScriptClass;

	};

	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore);
		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params);
		const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }
	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;

		std::unordered_map<std::string, ScriptField> m_Fields;


		friend class ScriptingEngine;
	};


	class ScriptInstance {
	public:
		ScriptInstance() = default;
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);


	private:
		Ref<ScriptClass> m_ScriptClass = nullptr;

		MonoObject* m_Instance = nullptr;

		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		std::unordered_map<std::string, ScriptField> m_Fields;


		friend class ScriptingEngine;
	};

}