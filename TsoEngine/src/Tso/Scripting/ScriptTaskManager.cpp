//
//  ScriptTaskManager.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/9/11.
//
#include "TPch.h"
#include "ScriptTaskManager.h"
namespace Tso{
ScriptTaskManager& ScriptTaskManager::Get() {
    static ScriptTaskManager instance;
    return instance;
}

void ScriptTaskManager::AddTask(sol::protected_function func, float delayInSeconds) {
    // 1. 确保函数是有效的
    if (!func.valid()) {
        TSO_CORE_ERROR("Attempted to add an invalid function to ScriptTaskManager.");
        return;
    }

    // 2. 计算执行时间点
    float currentTime = m_Time; // TODO: 替换为您引擎的真实总时间源，例如 Time::GetTotalTime()
    float executionTime = currentTime + delayInSeconds;

    // 3. 将任务添加到列表
    // sol::protected_function 可以被安全地移动和存储
    m_Tasks.push_back({ std::move(func), executionTime });
}

void ScriptTaskManager::OnUpdate(float totalTime) {
    m_Time = totalTime;
    if (m_Tasks.empty()) {
        return;
    }

    // 1. 遍历任务，找到所有到期的任务
    // 使用索引遍历，因为我们可能会在循环中删除元素
    for (int i = m_Tasks.size() - 1; i >= 0; i--) {
        auto& task = m_Tasks[i];

        if (totalTime >= task.executionTime) {
            // 执行 Lua 函数
            sol::protected_function_result result = task.luaFunction();

            if (!result.valid()) {
                // 如果执行出错，sol 会捕获错误
                sol::error err = result;
                // LogError("Error executing delayed task: %s", err.what());
            }

            // 从列表末尾快速删除已执行的任务
            std::swap(m_Tasks[i], m_Tasks.back());
            m_Tasks.pop_back();
        }
    }
}
}
