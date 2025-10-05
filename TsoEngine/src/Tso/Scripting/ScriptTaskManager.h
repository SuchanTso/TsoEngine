//
//  ScriptTaskManager.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/9/11.
//

#ifndef ScriptTaskManager_hpp
#define ScriptTaskManager_hpp
#include <sol/sol.hpp>

namespace Tso{

struct DelayedTask {
    sol::protected_function luaFunction; // 使用 sol::protected_function 安全地存储 Lua 函数
    float executionTime;                 // 任务应该在何时执行
};

class ScriptTaskManager{
public:
    static ScriptTaskManager& Get();

    // 由 Lua 绑定调用，现在参数类型更清晰
    void AddTask(sol::protected_function func, float delayInSeconds);

    // 在引擎的每一帧主循环中调用
    void OnUpdate(float totalTime);

private:
    ScriptTaskManager() = default;

    std::vector<DelayedTask> m_Tasks;
    float m_Time = 0.f;
};
}

#endif /* ScriptTaskManager_hpp */
