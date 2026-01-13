//
//  VirtualFileSystem.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/13.
//
#pragma once
#ifndef VirtualFileSystem_hpp
#define VirtualFileSystem_hpp

#include "Tso/Core/PakFile.h"

namespace Tso {

    // 内存文件结构 (代表一个加载到内存中的文件)
    struct Buffer {
        std::vector<char> Data;
        
        bool IsValid() const { return !Data.empty(); }
        char* DataPtr() { return Data.data(); }
        size_t Size() const { return Data.size(); }
    };

    class VirtualFileSystem {
    public:
        // 初始化：尝试加载 .pak 文件
        static void Init(const std::string& pakPath);
        static void Shutdown();

        // 核心接口：读取文件内容
        // 优先从 PAK 读取，如果没挂载 PAK 则从磁盘读取
        static Buffer ReadFile(const std::string& filepath);
        
        // 检查文件是否存在
        static bool Exists(const std::string& filepath);
        
        static std::vector<std::string> GetFiles(const std::string& directory);


    private:
        // 挂载 PAK
        static bool Mount(const std::string& pakPath);

    private:
        static std::ifstream s_PakStream; // 保持文件句柄打开，避免频繁开关
        static std::unordered_map<std::string, PakEntry> s_IndexTable;
        static bool s_IsMounted;
        static std::string s_MountPath;
    };
}

#endif /* VirtualFileSystem_hpp */
