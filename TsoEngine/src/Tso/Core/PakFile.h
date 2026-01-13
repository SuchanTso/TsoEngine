//
//  PakFile.h
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/13.
//
#pragma once
#ifndef PakFile_h
#define PakFile_h
#include <cstdint>

namespace Tso {

    // 1. 文件头 (Header)
    struct PakHeader {
        char Magic[4] = {'T', 'S', 'O', 'P'}; // 标识符 "TSOP"
        uint32_t Version = 1;                 // 版本号
        uint32_t FileCount = 0;               // 包含的文件数量
    };

    // 2. 文件索引项 (Directory Entry)
    struct PakEntry {
        char Path[128];      // 相对路径 (例如 "Assets/Textures/Player.png")
        uint64_t Offset;     // 数据在 PAK 文件中的起始偏移量 (字节)
        uint64_t Size;       // 文件原始大小 (字节)
        uint64_t CompressedSize; // 压缩后大小 (暂不使用压缩填 0)
        uint8_t Flags;       // 预留标志位 (0:无, 1:加密, 2:压缩)
    };

}


#endif /* PakFile_h */
