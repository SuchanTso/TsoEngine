//
//  LobbyProtocol.h
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/9.
//

#ifndef LobbyProtocol_h
#define LobbyProtocol_h
namespace Tso::Modules::Lobby {

    enum class GameType : uint8_t {
            TexasHoldem = 1,
            // Landlord = 2, // for future game
        };

constexpr uint8_t MODULE_ID = 2;

enum CommandID : uint8_t {
    // C2S
    C2S_CreateRoomReq = 1,
    C2S_JoinRoomReq = 2,
    C2S_ReadyReq = 3,
    C2S_GetRoomListReq = 4,

    // S2C
    S2C_CreateRoomRsp = 1,
    S2C_JoinRoomRsp = 2,
    S2C_RoomStateNtf = 3,     // 广播房间状态（玩家列表、准备状态等）
    S2C_GameStartNtf = 4,     // 通知客户端游戏开始，应切换到游戏场景
    S2C_RoomListRsp = 5,
};

inline uint16_t MakeProtocolID(CommandID cmd) {
    return (MODULE_ID << 8) | cmd;
}

// 房间内玩家信息
struct PlayerInfo {
    uint32_t clientId;
    std::string username;
    bool isReady;
};

// 房间信息
struct RoomInfo {
    uint32_t roomId;
    std::string roomName;
    std::vector<PlayerInfo> players;
    bool isGameInProgress = false;
};

#endif /* LobbyProtocol_h */
