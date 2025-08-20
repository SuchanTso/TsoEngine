#include "TPch.h"

#include "ScriptingEngine.h"
#include "ScriptGlue.h"
#include <fstream>
#include "spdlog/fmt/ostr.h"
#include "Project/Project.h"

#include "Tso/Scene/Component.h"
#include "Tso/Protocol/LuaBridge.h"
//
//
//#include "mono/jit/jit.h"
//#include "mono/metadata/assembly.h"
//#include "mono/metadata/object.h"
//#include "mono/metadata/tabledefs.h"
//#include "mono/metadata/mono-debug.h"
//#include "mono/metadata/threads.h"
//#include "mono/metadata/image.h"
//



namespace Tso {
static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
{
	{ "System.Single", ScriptFieldType::Float },
	{ "System.Double", ScriptFieldType::Double },
	{ "System.Boolean", ScriptFieldType::Bool },
	{ "System.Char", ScriptFieldType::Char },
	{ "System.Int16", ScriptFieldType::Short },
	{ "System.Int32", ScriptFieldType::Int },
	{ "System.Int64", ScriptFieldType::Long },
	{ "System.Byte", ScriptFieldType::Byte },
	{ "System.UInt16", ScriptFieldType::UShort },
	{ "System.UInt32", ScriptFieldType::UInt },
	{ "System.UInt64", ScriptFieldType::ULong },

	{ "Tso.Vector2", ScriptFieldType::Vector2 },
	{ "Tso.Vector3", ScriptFieldType::Vector3 },
	{ "Tso.Vector4", ScriptFieldType::Vector4 },

	{ "Tso.Entity", ScriptFieldType::Entity },
	{ "Tso.SpriteAnimationIdle", ScriptFieldType::SpriteAnimationIdle },

};

namespace Utils {
	static char* ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			TSO_CORE_ERROR("Failed to open the file:" + filepath);
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = (uint32_t)(end - stream.tellg());

		if (!size)
		{
			TSO_CORE_ERROR("File is empty:" + filepath);
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}
}
//

    // 在 C++ 中定义 Lua 的"类"
    struct LuaClass {
        std::string Name;
        // 指向 Lua 注册表中代表这个类的 metatable
        int MetatableRef = LUA_NOREF;

        // 从脚本中解析出的可编辑字段 { "fieldName": FieldType }
        std::unordered_map<std::string, ScriptFieldType> Fields;
    };


    struct ScriptEngineData {
//        lua_State* L = nullptr; // 全局的 Lua 状态机
        sol::state L;

        // 管理所有从脚本文件加载的 "类"
        // "Player" -> LuaClass
        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;

        // 运行中的脚本实例
        // EntityID -> LuaInstance
        std::unordered_map<uint64_t, Ref<ScriptInstance>> EntityInstances;

        // 用于序列化和编辑器，存储每个实体脚本字段的默认值
        std::unordered_map<uint64_t, ScriptFieldMap> EntityScriptFields;

        Scene* SceneContext = nullptr;
        
        std::unordered_map<uint8_t, std::shared_ptr<LuaProtocolHandler>> LuaProtocolHandlers;

    };

    // 全局的 s_Data 指针保持不变
    static ScriptEngineData* s_Data = nullptr;
	 

//	void ScriptingEngine::Init()
//	{
//        s_Data = new ScriptEngineData;
//            
//        // 1. 初始化 Lua 状态机
////        s_Data->L = luaL_newstate();
//        s_Data->L.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
//
////        luaL_openlibs(s_Data->L); // 加载标准库
//
//        // 2. 注册 C++ 核心 API 给 Lua (ScriptGlue)
//        ScriptGlue::RegisterFunctions();
//
//        // 3. 加载并解析所有脚本文件
////        LoadAllScripts(Project::GetActive()->GetConfig().ScriptModulePath);
//        SetLuaPackagePath(Project::GetResourcePath() + "assets/scripts");
//	}
    void ScriptingEngine::Init() {
        s_Data = new ScriptEngineData();
        s_Data->L.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
        
        ScriptGlue::RegisterFunctions();
        
        SetLuaPackagePath(Project::GetResourcePath() + "assets/scripts");
        LoadAllScripts(Project::GetResourcePath() + "assets/scripts"); // [MODIFIED] 假设脚本路径在这里
    }

    void ScriptingEngine::LoadAllScripts(const std::string& directory) {
        // 遍历指定目录下的所有 .lua 文件
        s_Data->EntityClasses.clear();
        if(std::filesystem::exists(directory)){
            for (auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.path().extension() == ".lua") {
                    LoadScriptClasses(entry.path());
                }
            }
        }
    }

//    void ScriptingEngine::SetLuaPackagePath(const std::string& rootPath){
//        lua_State* L = GetLuaState();
//
//            // 1. 定义我们的新搜索模板
//            // 我们希望 require("core.entity") 能找到 "rootPath/core/entity.lua"
//            // `?/init.lua` 是为了支持将模块作为一个目录，例如 require("my_module") 会寻找 my_module/init.lua
//            std::string new_search_path = rootPath + "/?.lua;" + rootPath + "/?/init.lua;";
//
//            // 2. 获取 Lua 全局的 package table
//            lua_getglobal(L, "package");
//            if (!lua_istable(L, -1)) {
//                TSO_CORE_ERROR("Could not get 'package' table from Lua");
//                lua_pop(L, 1); // 清理栈
//                return;
//            }
//
//            // 3. 获取当前的 package.path
//            lua_getfield(L, -1, "path");
//            if (!lua_isstring(L, -1)) {
//                TSO_CORE_ERROR("Could not get 'package.path' string from Lua");
//                lua_pop(L, 2); // 清理 'nil' 和 'package' table
//                return;
//            }
//            const char* current_path = lua_tostring(L, -1);
//
//            // 4. 构建新的 path 字符串 (我们的路径 + 现有路径)
//            new_search_path.append(current_path);
//
//            // 5. 将旧的 path string 从栈上弹出
//            lua_pop(L, 1);
//
//            // 6. 将新的 path 字符串压入栈
//            lua_pushstring(L, new_search_path.c_str());
//
//            // 7. 设置 package.path = new_path_string
//            // setfield 会自动弹出栈顶的值
//            // 此时栈是 [..., package_table, new_path_string]
//            lua_setfield(L, -2, "path");
//
//            // 8. 清理栈上的 package table
//            lua_pop(L, 1);
//
//            TSO_CORE_INFO("Lua package.path updated to: {}", new_search_path);
//    }
    void ScriptingEngine::SetLuaPackagePath(const std::string& rootPath) {
        // [MODIFIED] 使用 sol2 的方式修改 package.path，更简洁
        sol::table package = s_Data->L["package"];
        std::string current_path = package["path"];
        package["path"] = rootPath + "/?.lua;" + rootPath + "/?/init.lua;" + current_path;
        //TSO_CORE_INFO("Lua package.path updated to: {}", (std::string)package["path"]);
    }


    sol::state& ScriptingEngine::GetLuaState(){ // [MODIFIED] 返回 sol::state&
        TSO_CORE_ASSERT(s_Data, "ScriptingEngine not initialized!");
        return s_Data->L;
    }

//
    void ScriptingEngine::ShutDown()
    {
        delete s_Data;
        s_Data = nullptr;
    }

	void ScriptingEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptingEngine::EntityClassExists(sc.ClassName))
		{
			auto entityID = entity.GetUUID();

			Ref<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[entityID] = instance;

			// Copy field values
			if (s_Data->EntityScriptFields.find(entityID) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}

			instance->InvokeOnCreate();
		}
	}

	void ScriptingEngine::OnDeleteEntity(Entity& entity)
	{
		return;
		auto entityUUID = entity.GetUUID();
		if (auto it = s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end())
		{
			s_Data->EntityInstances.erase(it);
		}
		else
		{
			TSO_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
		}
	}

//	void ScriptingEngine::OnCollideEntity(Entity& thisEntity, Entity& otherEntity)
//	{
//		auto entityUUID = thisEntity.GetUUID();
//		if (auto it = s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end())
//		{
//			Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
//			instance->InvokeOnCollider(otherEntity.GetUUID());
//		}
//		else
//		{
//			TSO_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
//		}
//	}


	Scene* ScriptingEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	void ScriptingEngine::OnUpdateEntity(Entity entity, TimeStep ts)
	{
		auto entityUUID = entity.GetUUID();
		if (s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		}
		else
		{
			TSO_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
		}
	}

//void ScriptingEngine::LoadScriptClasses(const std::filesystem::path& path)
//	{
//    // 执行 Lua 脚本文件，它会返回一个 table 在栈顶
//        if (luaL_dofile(s_Data->L, path.string().c_str()) != LUA_OK) {
//            // 错误处理...
//            TSO_CORE_ERROR("Unable to load file{} :[{}]" , path.string().c_str() , lua_tostring(s_Data->L, -1));
//            return;
//        }
//
//        if (!lua_istable(s_Data->L, -1)) {
//            // 脚本没有返回一个 table，格式错误
//            lua_pop(s_Data->L, 1);
//            return;
//        }
//
//        // 从返回的 table 中提取类名 (从文件名获取)
//        std::string className = path.stem().string();
//        
//        Ref<ScriptClass> scriptClass = std::make_shared<ScriptClass>(className , luaL_ref(s_Data->L, LUA_REGISTRYINDEX));
//        
//        // --- 解析公共字段 (Fields) ---
//        // 再次把 metatable 推到栈上
//        lua_rawgeti(s_Data->L, LUA_REGISTRYINDEX, scriptClass->m_MetatableRef);
//        lua_getfield(s_Data->L, -1, "Fields");
//        if (lua_istable(s_Data->L, -1)) {
//            lua_pushnil(s_Data->L); // 开始遍历
//            while (lua_next(s_Data->L, -2) != 0) {
//                // key at -2, value at -1
//                const char* fieldName = lua_tostring(s_Data->L, -2);
//                const char* fieldTypeStr = lua_tostring(s_Data->L, -1);
//                scriptClass->m_Fields[fieldName] = {s_ScriptFieldTypeMap[fieldTypeStr] , fieldName}; // 使用你现有的 map
//                TSO_CORE_INFO("Scipt class{}:{}",fieldName , fieldTypeStr);
//                lua_pop(s_Data->L, 1); // 弹出 value，保留 key 给下一次迭代
//            }
//        }
//        lua_pop(s_Data->L, 1); // 弹出 Fields table
//
//        
//        lua_pop(s_Data->L, 1); // 弹出 metatable
//
//        s_Data->EntityClasses[className] = scriptClass;
//        TSO_CORE_INFO("Loaded Lua class: {}", className);
//		
//	}
    void ScriptingEngine::LoadScriptClasses(const std::filesystem::path& path) {
        // [MODIFIED] 使用 sol2 加载和解析脚本
        try {
            // sol::dofile 会执行脚本并返回其返回值
            sol::table luaClassTable = s_Data->L.script_file(path.string());

            if (!luaClassTable.valid()) {
                TSO_CORE_WARN("Script '{}' did not return a table.", path.string());
                return;
            }

            std::string className = path.stem().string();
            Ref<ScriptClass> scriptClass = std::make_shared<ScriptClass>(className, luaClassTable);
            
            // 解析 Fields
            sol::optional<sol::table> fields = luaClassTable["Fields"];
            if (fields) {
                for (const auto& kvp : fields.value()) {
                    std::string fieldName = kvp.first.as<std::string>();
                    std::string fieldTypeStr = kvp.second.as<std::string>();
                    if (s_ScriptFieldTypeMap.count(fieldTypeStr)) {
                        scriptClass->m_Fields[fieldName] = {s_ScriptFieldTypeMap.at(fieldTypeStr), fieldName};
                    }
                }
            }
            
            s_Data->EntityClasses[className] = scriptClass;
            TSO_CORE_INFO("Loaded Lua class: {}", className);

        } catch (const sol::error& e) {
            TSO_CORE_ERROR("Failed to load script '{}': {}", path.string(), e.what());
        }
    }

	bool ScriptingEngine::EntityClassExists(const std::string& className)
	{
		return s_Data->EntityClasses.find(className) != s_Data->EntityClasses.end();
	}


    bool ScriptingEngine::EntityInstanceExists(const UUID& uuid){
        return s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end();
    }


	void ScriptingEngine::OnScenePlay(Scene* context)
	{
		s_Data->SceneContext = context;
	}

	void ScriptingEngine::OnSceneStop()
	{
		s_Data->SceneContext = nullptr;
	}

std::unordered_map<uint8_t, Ref<LuaProtocolHandler>>& ScriptingEngine::GetLuaProtocolHandlers(){
    return s_Data->LuaProtocolHandlers;
}

Ref<ScriptInstance> ScriptingEngine::GetScriptInstance(const UUID& uuid){
    if(s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end()){
        return s_Data->EntityInstances[uuid];
    }
    return nullptr;
}



	ScriptClass::ScriptClass(const std::string& className, sol::table luaClassTable)
        :m_ClassName(className) , m_LuaClassTable(luaClassTable)
	{
		
	}
//
//	MonoObject* ScriptClass::Instantiate()
//	{
//		return ScriptingEngine::InstantiateClass(m_MonoClass);
//	}
//
//	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
//	{
//		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
//	}
//
//	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr)
//	{
//		MonoObject* exception = nullptr;
//		return mono_runtime_invoke(method, instance, params, &exception);
//	}
//s
//
    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass) {
        
        // [MODIFIED] 使用 sol2 创建实例，不再需要手动调用构造函数
        sol::table luaClass = m_ScriptClass->GetLuaClassTable();
        
        // 调用 Lua 脚本中的 "new" 方法来创建实例
        // sol::function newFunc = luaClass["new"]; // 如果 new 是类的成员
        // 如果 new 是一个全局函数或者在类的元表里，调用方式可能不同
        // 假设 new 是 class table 的一个字段
        if (luaClass["new"].is<sol::function>()) {
            sol::protected_function newFunc = luaClass["new"];
            auto result = newFunc(luaClass, std::to_string(entity.GetUUID()).c_str()); // 调用 MyClass:new(entityId)
            if (result.valid()) {
                m_LuaInstance = result; // 获取返回的 instance table
            } else {
                sol::error err = result;
                TSO_CORE_ERROR("Lua error in constructor for {}: {}", m_ScriptClass->GetClassName(), err.what());
                m_LuaInstance = sol::make_object(s_Data->L, sol::lua_nil); // 创建一个空的 table 作为兜底
            }
        } else {
            TSO_CORE_ERROR("Class '{}' does not have a 'new' function.", m_ScriptClass->GetClassName());
            m_LuaInstance = s_Data->L.create_table();
        }
    }

//	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
//		: m_ScriptClass(scriptClass)
//	{
////		m_Instance = scriptClass->Instantiate();
//        lua_rawgeti(s_Data->L , LUA_REGISTRYINDEX , scriptClass->GetMetatableRef());
//
//        //constructor
//        auto entityID = entity.GetUUID();
//
//        lua_getfield(s_Data->L, -1, "new");
//        if (lua_isfunction(s_Data->L, -1)) {
//            m_Constructor = luaL_ref(s_Data->L, LUA_REGISTRYINDEX);
//        }
//        else{
//            lua_pop(s_Data->L,1);
//            TSO_CORE_ERROR("Unable to fetch Constructor function for entity {}" , entityID);
//        }
//        lua_rawgeti(s_Data->L , LUA_REGISTRYINDEX , m_Constructor);
//        
////        lua_pushnumber(s_Data->L, entityID);
//        lua_pushstring(s_Data->L, std::to_string(entityID).c_str());
//        if(lua_pcall(s_Data->L, 1, 1, 0) != LUA_OK){
//            TSO_CORE_ERROR("Unable to call constructor when instantiating entity:{}",entityID);
//            TSO_CORE_ERROR("Error:[{}]" , lua_tostring(s_Data->L, -1));
//        }
//        m_InstanceTableRef = luaL_ref(s_Data->L , LUA_REGISTRYINDEX);
//
////        lua_rawgeti(s_Data->L , LUA_REGISTRYINDEX , m_InstanceTableRef);
//        lua_getfield(s_Data->L, -1, "OnCreate");
//        if(lua_isfunction(s_Data->L, -1)){
//            m_OnCreateRef = luaL_ref(s_Data->L, LUA_REGISTRYINDEX);
//        }
//        else {
//            TSO_CORE_ERROR("Unable to fetch OnCreate() function for entity {}" , entityID);
//            lua_pop(s_Data->L, 1); // 弹出 nil
//        }
//        
//        lua_getfield(s_Data->L, -1, "OnUpdate");
//        if(lua_isfunction(s_Data->L, -1)){
//            m_OnUpdateRef = luaL_ref(s_Data->L, LUA_REGISTRYINDEX);
//        }
//        else{
//            lua_pop(s_Data->L, 1);
//        }
//        
//        lua_pop(s_Data->L, 1); // 弹出 instance table
//	}
//
//	void ScriptInstance::InvokeOnCreate()
//	{
//        if (m_OnCreateRef != LUA_NOREF) {
//            lua_rawgeti(s_Data->L, LUA_REGISTRYINDEX, m_OnCreateRef);
//            // OnCreate 的第一个参数是 self (instance table)
//            lua_rawgeti(s_Data->L, LUA_REGISTRYINDEX, m_InstanceTableRef);
//            lua_pcall(s_Data->L, 1, 0, 0); // 1个参数(self), 0个返回值
//        }
//	}
    void ScriptInstance::InvokeOnCreate() {
        if (!m_LuaInstance.valid()) return;

        // [MODIFIED] 直接尝试调用，让 sol2 去处理 __index
        // sol::protected_function 会安全地执行查找和调用
        sol::protected_function onCreate = m_LuaInstance["OnCreate"];

        if (onCreate.valid()) {
            auto result = onCreate(m_LuaInstance); // 调用 self:OnCreate()
            if (!result.valid()) {
                sol::error err = result;
                TSO_CORE_ERROR("Lua error in OnCreate for {}: {}", m_ScriptClass->GetClassName(), err.what());
            }
        }
        // 如果 OnCreate 不存在（onCreate.valid() 为 false），则什么也不做
    }

//	void ScriptInstance::InvokeOnUpdate(float ts)
//	{
//        if (m_OnUpdateRef != LUA_NOREF){
//            // 推入 OnUpdate 函数
//            lua_rawgeti(s_Data->L, LUA_REGISTRYINDEX, m_OnUpdateRef);
//            
//            // 推入参数1：self (instance table)
//            lua_rawgeti(s_Data->L, LUA_REGISTRYINDEX, m_InstanceTableRef);
//            
//            // 推入参数2：ts (timestep)
//            lua_pushnumber(s_Data->L, (float)ts);
//            
//            // 调用：2个参数，0个返回值
//            if (lua_pcall(s_Data->L, 2, 0, 0) != LUA_OK) {
//                TSO_CORE_ERROR("Unable to update entity");
//            }
//        }
//	}
    void ScriptInstance::InvokeOnUpdate(float ts) {
        if (m_LuaInstance.valid() && m_LuaInstance["OnUpdate"].is<sol::function>()) {
            sol::protected_function onUpdate = m_LuaInstance["OnUpdate"];
            auto result = onUpdate(m_LuaInstance, ts); // 调用 self:OnUpdate(ts)
            if (!result.valid()) {
                sol::error err = result;
                TSO_CORE_ERROR("Lua error in OnUpdate for {}: {}", m_ScriptClass->GetClassName(), err.what());
            }
        }
    }
//
//	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
//	{
//		const auto& fields = m_ScriptClass->GetFields();
//		auto it = fields.find(name);
//		if (it == fields.end())
//			return false;
//
//		const ScriptField& field = it->second;
//		mono_field_get_value(m_Instance, field.ClassField, buffer);
//		return true;
//	}
//
//	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
//	{
//        lua_rawgeti(s_Data->L , LUA_REGISTRYINDEX , m_InstanceTableRef);
//        if(!lua_istable(s_Data->L, -1)){
//            lua_pop(s_Data->L, 1); // 弹出 nil 或非 table 值
//            TSO_CORE_ERROR("ScriptInstance's table is invalid!");
//            return false;
//        }
//		const auto& fields = m_ScriptClass->GetFields();
//		auto it = fields.find(name);
//        if (it == fields.end()){
//            lua_pop(s_Data->L, 1); // 弹出 instance table
//            TSO_CORE_WARN("Trying to set non-existent or non-public field '{}'", name);
//            return false;
//        }
//		const ScriptField& field = it->second;
//        lua_pushstring(s_Data->L, name.c_str());
//        switch (field.Type)
//            {
//                case ScriptFieldType::Float:   lua_pushnumber(s_Data->L, *(const float*)value); break;
//                case ScriptFieldType::Double:  lua_pushnumber(s_Data->L, *(const double*)value); break;
//                case ScriptFieldType::Bool:    lua_pushboolean(s_Data->L, *(const bool*)value); break;
//                case ScriptFieldType::Char:    { char str[2] = { *(const char*)value, '\0' }; lua_pushstring(s_Data->L, str); break; }
//                case ScriptFieldType::Short:   lua_pushinteger(s_Data->L, *(const int16_t*)value); break;
//                case ScriptFieldType::Int:     lua_pushinteger(s_Data->L, *(const int32_t*)value); break;
//                case ScriptFieldType::Long:    lua_pushinteger(s_Data->L, *(const int64_t*)value); break;
//                case ScriptFieldType::Byte:    lua_pushinteger(s_Data->L, *(const uint8_t*)value); break;
//                case ScriptFieldType::UShort:  lua_pushinteger(s_Data->L, *(const uint16_t*)value); break;
//                case ScriptFieldType::UInt:    lua_pushinteger(s_Data->L, *(const uint32_t*)value); break;
//                case ScriptFieldType::ULong:   lua_pushinteger(s_Data->L, *(const uint64_t*)value); break;
//
//                // 对于向量类型，我们需要创建一个 table
//                case ScriptFieldType::Vector2:
//                {
//                    const glm::vec2& vec = *(const glm::vec2*)value;
//                    lua_newtable(s_Data->L);
//                    lua_pushnumber(s_Data->L, vec.x); lua_setfield(s_Data->L, -2, "x");
//                    lua_pushnumber(s_Data->L, vec.y); lua_setfield(s_Data->L, -2, "y");
//                    // 你也可以在这里关联一个 Vector2 的 metatable，让它在 Lua 中有自己的方法
//                    break;
//                }
//                case ScriptFieldType::Vector3:
//                {
//                    const glm::vec3& vec = *(const glm::vec3*)value;
//                    lua_newtable(s_Data->L);
//                    lua_pushnumber(s_Data->L, vec.x); lua_setfield(s_Data->L, -2, "x");
//                    lua_pushnumber(s_Data->L, vec.y); lua_setfield(s_Data->L, -2, "y");
//                    lua_pushnumber(s_Data->L, vec.z); lua_setfield(s_Data->L, -2, "z");
//                    break;
//                }
//                case ScriptFieldType::Vector4:
//                {
//                    const glm::vec4& vec = *(const glm::vec4*)value;
//                    lua_newtable(s_Data->L);
//                    lua_pushnumber(s_Data->L, vec.x); lua_setfield(s_Data->L, -2, "x");
//                    lua_pushnumber(s_Data->L, vec.y); lua_setfield(s_Data->L, -2, "y");
//                    lua_pushnumber(s_Data->L, vec.z); lua_setfield(s_Data->L, -2, "z");
//                    lua_pushnumber(s_Data->L, vec.w); lua_setfield(s_Data->L, -2, "w");
//                    break;
//                }
//
//                // 对于实体，我们存储它的 UUID
//                case ScriptFieldType::Entity:
//                {
//                    const uint64_t& uuid = *(const uint64_t*)value;
//                    lua_pushnumber(s_Data->L, uuid); // 在 Lua 端存储为数字 ID
//                    break;
//                }
//                    
//                default:
//                    TSO_CORE_ERROR("Unknown script field type!");
//                    lua_pop(s_Data->L, 2); // 弹出 instance table 和 key
//                    return false;
//            }
//        
//        
//        lua_settable(s_Data->L, -3);
//        // 6. 弹出 instance table
//        lua_pop(s_Data->L, 1);
//
//		return true;
//	}
    bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value) {
        if (!m_LuaInstance.valid()) return false;

        const auto& fields = m_ScriptClass->GetFields();
        if (fields.find(name) == fields.end()) {
            TSO_CORE_WARN("Trying to set non-existent or non-public field '{}'", name);
            return false;
        }
        const ScriptField& field = fields.at(name);

        // [MODIFIED] 使用 sol2 的方式设置 table 字段，类型安全
        switch (field.Type) {
            case ScriptFieldType::Float:   m_LuaInstance[name] = *(const float*)value; break;
            case ScriptFieldType::Double:  m_LuaInstance[name] = *(const double*)value; break;
            case ScriptFieldType::Bool:    m_LuaInstance[name] = *(const bool*)value; break;
            case ScriptFieldType::Int:     m_LuaInstance[name] = *(const int32_t*)value; break;
            case ScriptFieldType::UInt:    m_LuaInstance[name] = *(const uint32_t*)value; break;
            case ScriptFieldType::Vector2: m_LuaInstance[name] = *(const glm::vec2*)value; break;
            case ScriptFieldType::Vector3: m_LuaInstance[name] = *(const glm::vec3*)value; break;
            case ScriptFieldType::Vector4: m_LuaInstance[name] = *(const glm::vec4*)value; break;
            case ScriptFieldType::Entity:  m_LuaInstance[name] = *(const uint64_t*)value; break;
            default: TSO_CORE_ERROR("Unknown script field type!"); return false;
        }
        return true;
    }

    ScriptInstance::~ScriptInstance(){
        //TODO: garbage collection @Suchan
    }
//
//
}
