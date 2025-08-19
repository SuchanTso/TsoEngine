//
//  LuaBridge.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/15.
//

#include "TPch.h"
#include "LuaBridge.h"

namespace Tso{
void LuaProtocolHandler::HandleNetworkMessage(uint8_t commandId, ByteStream& stream) {
    if (m_LuaFunction.valid()) {
        try {
            auto result = m_LuaFunction(commandId, stream);
            if (!result.valid()) {
                sol::error err = result;
                TSO_CORE_ERROR("[LUA ERROR] in network callback: {}", err.what());
            }
        } catch (const sol::error& e) {
            TSO_CORE_ERROR("[LUA ERROR] exception in network callback: {}", e.what());
        }
    }
}
}
