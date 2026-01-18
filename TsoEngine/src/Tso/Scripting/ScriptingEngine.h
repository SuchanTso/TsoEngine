#pragma once
#include <filesystem>
#include "Tso/Scene/Entity.h"
#include "Tso/Core/TimeStep.h"
#include "sol/sol.hpp"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace Tso {
    class LuaProtocolHandler;
    class ScriptInstance;
    class ScriptClass;
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity,Prefab,Material,
        Texture,
        Animation
	};

	struct ScriptField
	{
		ScriptFieldType Type = ScriptFieldType::None;
		std::string Name = "";
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
		T GetValue()const
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
        static void LoadAllScripts(const std::string& directory , bool reset = true);
		static void ShutDown();
		static bool EntityClassExists(const std::string& className);
        static Ref<ScriptClass> GetScriptClass(const std::string& className);
        static bool EntityInstanceExists(const UUID& uuid);
		static void OnScenePlay(Scene* context);
		static void OnSceneStop();
		static void OnCreateEntity(Entity entity);
		static void OnDeleteEntity(Entity& entity);
		static void OnUpdateEntity(Entity entity, TimeStep ts);
        static Ref<ScriptInstance> GetScriptInstance(const UUID& uuid);
//		static void OnCollideEntity(Entity& thisEntity , Entity& otherEntity);
//        static lua_State* GetLuaState();
		static Scene* GetSceneContext();
        static std::unordered_map<uint8_t, Ref<LuaProtocolHandler>>& GetLuaProtocolHandlers();

        
        static void SetLuaPackagePath(const std::string& rootPath);
        static sol::state& GetLuaState(); // 返回 sol::state 的引用
        static std::unordered_map<std::string, Ref<ScriptClass>> GetScriptClasses();





	private:
		static void InitMono();
		static void ShutdownMono();
		static bool LoadAssembly(const std::filesystem::path& filepath);
        static void LoadScriptClasses(const std::filesystem::path& path);
//		static MonoObject* InstantiateClass(MonoClass* monoClass);



		friend class ScriptClass;

	};

	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& className, sol::table luaClassTable);
		const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }
//        int GetMetatableRef(){return m_MetatableRef;}
        sol::table GetLuaClassTable()&{return m_LuaClassTable;}
        std::string GetClassName()&{return m_ClassName;}
        void SetPath(const std::string& path){m_Path = path;}
        std::string& GetPath(){return m_Path;}
	private:
        
        // 指向 Lua 注册表中代表这个类的 metatable
//        int m_MetatableRef = LUA_NOREF;
        sol::table m_LuaClassTable; // [MODIFIED] 直接持有 sol::table

        // 从脚本中解析出的可编辑字段 { "fieldName": FieldType }
		std::string m_ClassName;

		std::unordered_map<std::string, ScriptField> m_Fields;
        std::string m_Path;


		friend class ScriptingEngine;
	};


	class ScriptInstance {
	public:
		ScriptInstance() = default;
        ~ScriptInstance();
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
        sol::table GetLuaInstance(){return m_LuaInstance;}

//		void InvokeOnCollider(UUID uuid);


	private:
		Ref<ScriptClass> m_ScriptClass = nullptr;
        
        // 指向 Lua 注册表中代表这个实例的 table
//        int m_InstanceTableRef = LUA_NOREF;
        sol::table m_LuaInstance; // [MODIFIED] 直接持有代表实例的 table

        // 缓存生命周期函数的引用，避免每次都查找
//        int m_Constructor = LUA_NOREF;
//        int m_OnCreateRef = LUA_NOREF;
//        int m_OnUpdateRef = LUA_NOREF;

		std::unordered_map<std::string, ScriptField> m_Fields;


		friend class ScriptingEngine;
	};

}
