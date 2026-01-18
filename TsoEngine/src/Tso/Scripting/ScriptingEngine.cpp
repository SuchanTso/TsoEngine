#include "TPch.h"

#include "ScriptingEngine.h"
#include "ScriptGlue.h"
#include <fstream>
#include "spdlog/fmt/ostr.h"
#include "Project/Project.h"

#include "Tso/Scene/Component.h"
#include "Tso/Protocol/LuaBridge.h"
#include "Tso/Core/VirtualFileSystem.h"
#include "Tso/Scene/Prefab.h"
#include "Tso/Renderer/Material.h"



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
    { "Tso.Prefab", ScriptFieldType::Prefab },
    { "Tso.Material", ScriptFieldType::Material },
	{ "Tso.Animation", ScriptFieldType::Animation },

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

    int LuaVFSLoader(lua_State* L) {
        // 1. 获取模块名 (require 的参数)
        const char* moduleName = lua_tostring(L, 1);
        std::string path(moduleName);

        // 2. 路径转换: "a.b" -> "a/b"
        std::replace(path.begin(), path.end(), '.', '/');

        // 3. 定义搜索策略
        // 你可能需要搜索几个默认路径，或者要求 require 写全路径
        // 假设我们在 PAK 里的结构是 "Assets/Scripts/..."
        std::vector<std::string> searchPaths = {
            path + ".lua",                    // 绝对匹配
            "Assets/Scripts/" + path + ".lua", // 常用前缀
            "Assets/" + path + ".lua"          // 备用前缀
        };

        for (const auto& tryPath : searchPaths) {
            // 4. 从 VFS 读取
            if (VirtualFileSystem::Exists(tryPath)) {
                Buffer buffer = VirtualFileSystem::ReadFile(tryPath);
                if (buffer.IsValid()) {
                    // 5. 加载缓冲区 (编译代码)
                    // 最后一个参数是 debug 名字，加 @ 表示文件名
                    std::string chunkName = "@" + tryPath;
                    if (luaL_loadbuffer(L, buffer.DataPtr(), buffer.Size(), chunkName.c_str()) != LUA_OK) {
                        // 如果编译失败 (语法错误)，luaL_loadbuffer 会把错误信息压栈
                        // 我们直接报错返回
                        return lua_error(L);
                    }
                    
                    // 成功：栈顶现在是编译好的函数
                    return 1;
                }
            }
        }

        // 6. 没找到：返回错误信息 (Lua 会把所有 searcher 的错误拼起来显示)
        lua_pushstring(L, ("\n\t[VFS] Cannot find module '" + path + "'").c_str());
        return 1;
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
        s_Data->L.add_package_loader(Utils::LuaVFSLoader);
        SetLuaPackagePath(Project::GetResourcePath() + "assets/scripts");
        LoadAllScripts(Project::GetResourcePath() + "assets/scripts"); // [MODIFIED] 假设脚本路径在这里
    }

    void ScriptingEngine::LoadAllScripts(const std::string& directory , bool reset) {
        // 遍历指定目录下的所有 .lua 文件
        if(reset){
            s_Data->EntityClasses.clear();
        }
        auto files = VirtualFileSystem::GetFiles(directory);

        for (const auto& filePath : files) {
            if (std::filesystem::path(filePath).extension() == ".lua") {
                LoadScriptClasses(filePath);
            }
        }
    }


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
    std::unordered_map<std::string, Ref<ScriptClass>> ScriptingEngine::GetScriptClasses(){
        TSO_CORE_ASSERT(s_Data, "ScriptingEngine not initialized!");
        return s_Data->EntityClasses;
    }


//
    void ScriptingEngine::ShutDown()
    {
        delete s_Data;
        s_Data = nullptr;
    }

	void ScriptingEngine::OnCreateEntity(Entity entity)
	{
        if(!s_Data->SceneContext || !s_Data->SceneContext->IsScenePlay()){
            return;
        }
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptingEngine::EntityClassExists(sc.ClassName))
		{
			auto entityID = entity.GetUUID();

			Ref<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[entityID] = instance;

			// Copy field values
            auto scriptC = entity.GetComponent<ScriptComponent>();
            
//				const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(entityID);
            for (const auto& [name, fieldInstance] : scriptC.FieldInstances)
                instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);

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

    void ScriptingEngine::LoadScriptClasses(const std::filesystem::path& path) {
        // 路径标准化：确保路径分隔符一致，方便 VFS 查找
        std::string pathStr = path.string();
        // std::replace(pathStr.begin(), pathStr.end(), '\\', '/'); // 如果需要手动替换

        try {
            // [MODIFIED] 使用 VFS 读取文件内容 (可能是源码，也可能是字节码)
            Buffer fileBuffer = VirtualFileSystem::ReadFile(pathStr);
            
            if (!fileBuffer.IsValid()) {
                TSO_CORE_ERROR("ScriptingEngine: Failed to read file '{0}' from VFS", pathStr);
                return;
            }

            // 使用 sol::state::script 执行内存数据
            // 注意：必须显式传入 string_view 的长度，因为二进制字节码中间可能包含 \0
            std::string_view scriptData(fileBuffer.DataPtr(), fileBuffer.Size());
            
            // chunkname (第二个参数) 用于在报错时显示文件名，非常有帮助
            sol::table luaClassTable = s_Data->L.script(scriptData, ("@" + pathStr));

            if (!luaClassTable.valid()) {
                TSO_CORE_WARN("Script '{}' did not return a table.", pathStr);
                return;
            }

            std::string className = path.stem().string();
            Ref<ScriptClass> scriptClass = std::make_shared<ScriptClass>(className, luaClassTable);
            
            // 解析 Fields (保持原有逻辑不变)
            sol::optional<sol::table> properties = luaClassTable["Properties"];
            if (properties) {
               for (const auto& kvp : properties.value()) {
                   std::string fieldName = kvp.first.as<std::string>();
                   
                   // kvp.second 是一个 sol::object，我们需要把它当作 table 处理
                   if (!kvp.second.is<sol::table>()) continue;
                   sol::table propData = kvp.second.as<sol::table>();
                   
                   // [修正] 使用 .get<>() 获取 Type 字符串
                   // 检查是否存在 "Type" 键
                   sol::optional<std::string> typeStrOpt = propData["Type"];
                   if (!typeStrOpt) {
                       TSO_CORE_WARN("Property '{}' in script '{}' missing 'Type' field.", fieldName, className);
                       continue;
                   }
                   
                   std::string fieldTypeStr = typeStrOpt.value();
                   
                   // 查找映射
                   if (s_ScriptFieldTypeMap.count(fieldTypeStr)) {
                       ScriptFieldType type = s_ScriptFieldTypeMap.at(fieldTypeStr);
                       scriptClass->m_Fields[fieldName] = { type, fieldName };
                   } else {
                       TSO_CORE_WARN("Unknown field type '{0}' in script '{1}'", fieldTypeStr, className);
                   }
               }
           }
            
            s_Data->EntityClasses[className] = scriptClass;
            scriptClass->SetPath(path.string());
            TSO_CORE_INFO("Loaded Lua class: {}", className);

        } catch (const sol::error& e) {
            TSO_CORE_ERROR("Failed to load script '{}': {}", pathStr, e.what());
        }
    }

	bool ScriptingEngine::EntityClassExists(const std::string& className)
	{
		return s_Data->EntityClasses.find(className) != s_Data->EntityClasses.end();
	}

    Ref<ScriptClass> ScriptingEngine::GetScriptClass(const std::string& className){
        if(EntityClassExists(className)){
            return s_Data->EntityClasses[className];
        }
        return nullptr;
    }



    bool ScriptingEngine::EntityInstanceExists(const UUID& uuid){
        return s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end();
    }

    Ref<ScriptInstance> ScriptingEngine::GetEntityScriptInstance(UUID entityID) {
        auto it = s_Data->EntityInstances.find(entityID);
        if (it != s_Data->EntityInstances.end())
            return it->second;
        return nullptr;
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
            case ScriptFieldType::Prefab: {
                uint64_t uuid = *(const uint64_t*)value;
                // 尝试加载资源对象并传给 Lua
                Ref<Prefab> prefab = Resource::GetResource<Prefab>(uuid);
                // 注意：你需要先在 Sol2 中注册好 Prefab 类，否则这里会报错
                if (prefab) m_LuaInstance[name] = prefab;
                else m_LuaInstance[name] = sol::lua_nil;
                break;
            }
            case ScriptFieldType::Material: {
                uint64_t uuid = *(const uint64_t*)value;
                Ref<Material> mat = Resource::GetResource<Material>(uuid);
                if (mat) m_LuaInstance[name] = mat;
                else m_LuaInstance[name] = sol::lua_nil;
                break;
            }
            case ScriptFieldType::Animation: {
                // Animation 可能对应 AnimationClip
                uint64_t uuid = *(const uint64_t*)value;
                Ref<AnimationClip> clip = Resource::GetResource<AnimationClip>(uuid);
                if (clip) m_LuaInstance[name] = clip;
                else m_LuaInstance[name] = sol::lua_nil;
                break;
            }
            case ScriptFieldType::Entity: {
                uint64_t uuid = *(const uint64_t*)value;
                // 需要获取当前场景才能 Find Entity
                // 假设 ScriptInstance 持有 entity 引用，可以通过 entity.GetScene() 获取
                // 或者你需要通过全局 Application::GetActiveScene() 获取
                Entity target = ScriptingEngine::GetSceneContext()->GetEntityByUUID(uuid);
                if (target) m_LuaInstance[name] = target;
                else m_LuaInstance[name] = sol::lua_nil;
                break;
            }
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
