//
//  VirtualFileSystem.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/13.
//

#include "TPch.h"
#include "VirtualFileSystem.h"

namespace Tso {


#if defined(TSO_PLATFORM_WINDOWS)
    #include <windows.h>
#elif defined(TSO_PLATFORM_MACOSX)
    #include <mach-o/dyld.h>
    #include <unistd.h>
#elif defined(TSO_PLATFORM_LINUX)
    #include <unistd.h>
    #include <limits.h>
#endif

namespace Utils {

    void FixWorkingDirectory() {
        std::filesystem::path exePath;

#if defined(TSO_PLATFORM_WINDOWS)
        // Windows 获取 exe 路径
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        exePath = std::filesystem::path(path);

#elif defined(TSO_PLATFORM_MACOSX)
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) {
            // 此时 exePath 是 ".../Game.app/Contents/MacOS/Game"
            exePath = std::filesystem::path(path);
            
            // 1. 获取 MacOS 目录
            std::filesystem::path macOSDir = exePath.parent_path();
            
            // 2. 尝试寻找兄弟目录 Resources
            // 逻辑结构: Game.app -> Contents -> Resources
            std::filesystem::path contentDir = macOSDir.parent_path();
            std::filesystem::path resourcesDir = contentDir / "Resources";
            
            if (std::filesystem::exists(resourcesDir)) {
                // >>> 情况 A: 在 App Bundle 内 <<<
                // 将工作目录设为 Resources，这样 "./Game.pak" 就能被找到
                std::filesystem::current_path(resourcesDir);
            } else {
                // >>> 情况 B: 裸奔的可执行文件 (开发模式) <<<
                // 设为可执行文件所在的目录
                std::filesystem::current_path(macOSDir);
            }
        }

#elif defined(TSO_PLATFORM_LINUX)
        // Linux 获取 exe 路径
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count != -1) {
            exePath = std::filesystem::path(std::string(path, count));
        }
#endif

        // 获取父目录 (即 exe 所在的文件夹)
        if (!exePath.empty()) {
            std::filesystem::path exeDir = exePath.parent_path();
            
            // 设置当前工作目录
            std::filesystem::current_path(exeDir);
            
            // Debug 输出验证一下
            // printf("Working Directory set to: %s\n", exeDir.string().c_str());
        }
    }
}

    std::ifstream VirtualFileSystem::s_PakStream;
    std::unordered_map<std::string, PakEntry> VirtualFileSystem::s_IndexTable;
    bool VirtualFileSystem::s_IsMounted = false;
    std::string VirtualFileSystem::s_MountPath;

    // 辅助：统一路径分隔符 (将 '\\' 替换为 '/')
    static std::string NormalizePath(const std::string& path) {
        std::string result = path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

    void VirtualFileSystem::Init(const std::string& pakPath) {
        Utils::FixWorkingDirectory();
        if (std::filesystem::exists(pakPath)) {
            Mount(pakPath);
        } else {
            TSO_CORE_WARN("VFS: No PAK file found at {0}. Running in Disk Mode.", pakPath);
        }
    }

    void VirtualFileSystem::Shutdown() {
        if (s_PakStream.is_open()) {
            s_PakStream.close();
        }
        s_IndexTable.clear();
        s_IsMounted = false;
    }

    bool VirtualFileSystem::Mount(const std::string& pakPath) {
        s_PakStream.open(pakPath, std::ios::binary);
        if (!s_PakStream) return false;

        // 1. 读取 Header
        PakHeader header;
        s_PakStream.read((char*)&header, sizeof(PakHeader));

        // 简单校验 Magic
        if (strncmp(header.Magic, "TSOP", 4) != 0) {
            TSO_CORE_ERROR("VFS: Invalid PAK format!");
            s_PakStream.close();
            return false;
        }

        // 2. 读取索引表
        std::vector<PakEntry> entries(header.FileCount);
        s_PakStream.read((char*)entries.data(), sizeof(PakEntry) * header.FileCount);

        // 3. 构建哈希映射
        for (const auto& entry : entries) {
            // 注意：我们用标准化的路径作为 Key
            std::string pathKey = NormalizePath(entry.Path);
            s_IndexTable[pathKey] = entry;
        }

        s_IsMounted = true;
        s_MountPath = pakPath;
        TSO_CORE_INFO("VFS: Mounted {0} with {1} files.", pakPath, header.FileCount);
        TSO_CORE_INFO("=== VFS Index Table Dump ===");
        for (const auto& [key, entry] : s_IndexTable) {
            TSO_CORE_INFO("Key: '{0}' | Size: {1}", key, entry.Size);
        }
        TSO_CORE_INFO("============================");
        return true;
    }

    Buffer VirtualFileSystem::ReadFile(const std::string& filepath) {
        Buffer buffer;
        std::string normalizedPath = NormalizePath(filepath);
        if(Project::GetActive()){
            std::filesystem::path rootDir = Project::GetProjectDirectory();
            std::filesystem::path relPath = std::filesystem::relative(normalizedPath, rootDir);
            normalizedPath = relPath.string();
        }
        normalizedPath = NormalizePath(normalizedPath);


        // 场景 A: 从 PAK 读取
        if (s_IsMounted) {
//            normalizedPath = "assets/8bitoperator.ttf";
            auto it = s_IndexTable.find(normalizedPath);
            if (it != s_IndexTable.end()) {
                const PakEntry& entry = it->second;
                
                buffer.Data.resize(entry.Size);
                
                // 线程安全注意：如果多线程加载，这里需要加锁 (s_PakStream 是共享的)
                // 或者每个线程 open 一个单独的 ifstream
                s_PakStream.seekg(entry.Offset);
                s_PakStream.read(buffer.Data.data(), entry.Size);
                
                return buffer;
            }
            // 如果 PAK 里没有，可能会 Fallback 到磁盘，也可能直接报错
            // 取决于你想不想允许 "Patch" (磁盘文件覆盖 PAK)
        }

        // 场景 B: 从磁盘直接读取 (开发模式 或 PAK 中未找到)
        std::ifstream input(filepath, std::ios::binary | std::ios::ate);
        if (input) {
            size_t fileSize = input.tellg();
            input.seekg(0);
            buffer.Data.resize(fileSize);
            input.read(buffer.Data.data(), fileSize);
            input.close();
        } else {
            TSO_CORE_ERROR("VFS: Failed to read file: {0}", filepath);
        }

        return buffer;
    }

// VirtualFileSystem.cpp

    std::vector<std::string> VirtualFileSystem::GetFiles(const std::string& directory) {
        std::vector<std::string> files;
        std::filesystem::path rootPath(directory);
        std::string rootStr = rootPath.generic_string();

        // 模式 A: PAK (内存索引)
        if (s_IsMounted) {
            for (const auto& [path, entry] : s_IndexTable) {
                // 检查前缀匹配 (简单的 startswith)
                if (path.find(rootStr) == 0) {
                    files.push_back(path);
                }
            }
        }
        // 模式 B: Disk (物理磁盘)
        else if (std::filesystem::exists(rootPath)) {
            for (auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().generic_string());
                }
            }
        }

        return files;
    }

    bool VirtualFileSystem::Exists(const std::string& filepath) {
        std::string normalizedPath = NormalizePath(filepath);
        if (s_IsMounted && s_IndexTable.find(normalizedPath) != s_IndexTable.end()) {
            return true;
        }
        return std::filesystem::exists(filepath);
    }
}
