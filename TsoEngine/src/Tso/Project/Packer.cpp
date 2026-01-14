//
//  ProjectExport.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/13.
//
#include "TPch.h"
#include "Packer.h"
#include "Tso/Core/PakFile.h"
#include "Resource.h"

namespace Tso {

    bool Packer::Pack(const std::filesystem::path& rootDir, const std::filesystem::path& outputPak){
        std::ofstream out(outputPak, std::ios::binary);
        if (!out) {
            TSO_CORE_ERROR("Failed to create PAK file: {0}", outputPak.string());
            return false;
        }
        
        TSO_CORE_INFO("Starting Build: {0}", outputPak.string());
        
        // 1. 扫描文件
        std::vector<std::filesystem::path> filesToPack = Resource::GetAllResourceToExport();
        auto projePath = Project::GetProjectPath();
        filesToPack.push_back(projePath);
        auto scenes = Project::GetSceneAsset();
        filesToPack.insert(filesToPack.end(), scenes.begin() , scenes.end());
        
        // 2. 准备 Header
        PakHeader header;
        header.FileCount = (uint32_t)filesToPack.size();
        
        // 写入 Header 占位
        out.write((char*)&header, sizeof(PakHeader));
        
        // 3. 准备 Index Table
        std::vector<PakEntry> entries(header.FileCount);
        
        // 计算数据区起始位置 = Header + IndexTable
        uint64_t currentOffset = sizeof(PakHeader) + (sizeof(PakEntry) * header.FileCount);
        
        // 写入空的 Index Table 占位
        uint64_t indexTableStartPos = out.tellp(); // 记录位置以便回头重写
        out.write((char*)entries.data(), sizeof(PakEntry) * header.FileCount);
        
        // 4. 写入文件数据并填充索引
        for (size_t i = 0; i < filesToPack.size(); ++i) {
            const auto& filePath = filesToPack[i];
            std::vector<char> buffer; // 用于存储文件内容（无论是源码还是字节码）
            PakEntry& entry = entries[i];
            std::filesystem::path relPath = std::filesystem::relative(filePath, rootDir);
            std::string relPathStr = relPath.generic_string();
            
            strncpy(entry.Path, relPathStr.c_str(), sizeof(entry.Path) - 1);

            if (filePath.extension() == ".lua") {
                TSO_CORE_INFO("Compiling Script: {0}", filePath.string());
                if (!CompileLua(filePath, buffer)) {
                    TSO_CORE_WARN("Falling back to raw source for: {0}", filePath.string());
                    continue;
                }
            }
            else {
                if(filePath.extension() == ".tproj"){
                    strncpy(entry.Path, "App.tproj", sizeof(entry.Path) - 1);
                }
                std::ifstream input(filePath, std::ios::binary | std::ios::ate);
                if (!input) continue;
                uint64_t fileSize = input.tellg();
                input.seekg(0);
                buffer.resize(fileSize);
                input.read(buffer.data(), fileSize);
                input.close();
            }
            
            uint64_t dataSize = buffer.size();
            
            // 写入 PAK
            out.write(buffer.data(), dataSize);
            
            
            entry.Offset = currentOffset;
            entry.Size = dataSize;
            entry.CompressedSize = 0;
            entry.Flags = (filePath.extension() == ".lua") ? 1 : 0; // 可以标记一下这是脚本
            
            currentOffset += dataSize;
        }
            
        out.seekp(indexTableStartPos);
        out.write((char*)entries.data(), sizeof(PakEntry) * header.FileCount);
        
        out.close();
        TSO_CORE_INFO("Build Complete! Total files: {0}", header.FileCount);
        return true;
    }
    

bool Packer::CompileLua(const std::filesystem::path& inputPath, std::vector<char>& outputBuffer) {
    // 临时文件路径
    std::string tempOut = "temp_script.out";
    
    // 构造编译命令
    // 假设 luac.exe 在系统 PATH 中，或者你可以写绝对路径 "Vendor\\Bin\\luac.exe"
    // -o: 输出文件
    // -s: 剥离调试信息 (Strip debug info)，让文件更小且难以反编译
#ifdef TSO_PLATFORM_MACOSX
    std::string command = "assets/luac -s -o " + tempOut + " \"" + inputPath.string() + "\"";
#elif TSO_PLATFORM_WINDOWS
    std::string command = " \"\"assets/luac54.exe\" -s -o " + tempOut + " \"" + inputPath.string() + "\" \"";
    //TSO_CORE_INFO("Compile cmd :[{}]", command);

#endif
    // 如果是 LuaJIT，命令通常是: "luajit -b \"" + inputPath.string() + "\" " + tempOut;

    // 执行系统命令
    int result = std::system(command.c_str());
    
    if (result != 0) {
        TSO_CORE_ERROR("Lua compilation failed for: {0}", inputPath.string());
        return false;
    }

    // 读取编译后的临时文件
    std::ifstream input(tempOut, std::ios::binary | std::ios::ate);
    if (input) {
        uint64_t fileSize = input.tellg();
        input.seekg(0);
        outputBuffer.resize(fileSize);
        input.read(outputBuffer.data(), fileSize);
        input.close();
        
        // 删除临时文件
        std::filesystem::remove(tempOut);
        return true;
    }
    
    return false;
}
}
