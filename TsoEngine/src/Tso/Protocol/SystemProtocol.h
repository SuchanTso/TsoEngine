//
//  SystemProtocol.h
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/9.
//

#ifndef SystemProtocol_h
#define SystemProtocol_h
namespace Tso::Modules::System {

    constexpr uint8_t MODULE_ID = 1;

    enum CommandID : uint8_t {
        C2S_HeartBeat = 1,
        S2C_HeartBeat = 2,
        C2S_LoginReq = 3,
        S2C_LoginRsp = 4,
    };

    inline uint16_t MakeProtocolID(CommandID cmd) {
        return (MODULE_ID << 8) | cmd;
    }

}

#endif /* SystemProtocol_h */
