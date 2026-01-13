//
//  ProjectExport.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/13.
//
#pragma once
#ifndef Packer_h
#define Packer_h

namespace Tso{

    class Packer {
        public:
            // rootDir: 项目根目录 (用于计算相对路径)
            // outputPak: 输出的 .pak 文件路径
            static bool Pack(const std::filesystem::path& rootDir, const std::filesystem::path& outputPak);
        static bool CompileLua(const std::filesystem::path& inputPath, std::vector<char>& outputBuffer);
        };


}

#endif // !Packer_h
