//
//  LuaBridge.h
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/15.
//

#ifndef LuaBridge_h
#define LuaBridge_h
#pragma once
#include "TPch.h"
#include <sol/sol.hpp>
#include "Tso/Network/IProtocol.h"
#include "Scripting/ScriptingEngine.h"
namespace Tso{
class LuaProtocolHandler : public IProtocolHandler {
    public:
        LuaProtocolHandler(sol::state_view L, sol::function func) : m_LuaFunction(func) {
            // sol::function 对象会自动处理其在Lua注册表中的引用，我们不需要手动管理
        }
        
        // 我们不需要手动析构，sol::function 会处理好
        ~LuaProtocolHandler() override = default;
        
    void HandleNetworkMessage(uint8_t commandId, ByteStream& stream) override;
        
    private:
        // 直接持有 sol::function，它是一个轻量级的引用对象
        sol::function m_LuaFunction;
    };

    // 全局实例
}
#endif /* LuaBridge_h */
