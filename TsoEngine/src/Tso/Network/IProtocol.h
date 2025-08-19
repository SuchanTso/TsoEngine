//
//  IProtocol.h
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/9.
//

#ifndef IProtocol_h
#define IProtocol_h
#pragma once
#include "ByteStream.h" // 假设路径

// 一个简单的接口，让UI或逻辑类可以处理特定模块的网络消息
namespace Tso{
    class IProtocolHandler {
    public:
        virtual ~IProtocolHandler() = default;
        // 当收到该模块的消息时，此函数被调用
        virtual void HandleNetworkMessage(uint8_t commandId, ByteStream& stream) = 0;
    };
}
#endif /* IProtocol_h */
