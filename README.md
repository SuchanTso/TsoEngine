# TsoEngine

# 配置git
``` http
https://blog.csdn.net/weixin_44842112/article/details/125696979
```
* ps:不想下载gitbash也可以，直接下载vs，使用【工具】-【命令行】-【开发者powershell】也能使用指令

# 下载vs2022
``` http
https://visualstudio.microsoft.com/zh-hans/free-developer-offers/
```
* 最好下载vs2022，因为如果其他较低版本，配置环境时需要修改配置脚本

# 获取项目
* 1、进入到你想要存放项目的目录，运行指令
``` bash
git clone git@github.com:Zediakense/TsoEngine.git
```

* 2、进入buildsystem分支，运行指令：
``` bash
git checkout buildsystem
git pull
```
* ps: git checkout 指的是切换分支，开发途中每个人在自己的开发分支上完成功能，完成后统一合回develop分支
* 如何创建自己的分支将在后文描述

* 初始化子模块
``` bash
git submodule update --init --recursive
```

该指令是为了更新子模块，第一次获取项目时需要运行，其余时候如果仅仅只是有头文件找不到，也许是有人新加了子模块，需要运行

``` bash
git submodule update 
```

# 配置环境
* 获取到的工程文件，可以直接双击make.bat文件一键生成TsoEngine.sln
* 至此打开TsoEngine.sln应该可以编译运行



# 常见问题
* 1、报错：无法解析的外部符号：__imp__strncpy
	* 解决：查看premake5.lua文件下，搜索buildoption，将注释去掉（ps：lua脚本的注释符号是“--”）
* 2、报错：找不到<unistd.h>
	* 解决：全局搜unistd.h（其实在sandbox.cpp里），把#include <unistd.h>注释掉	
* 3、点击运行弹窗：xxx不是有效的程序
	* 解决：找到右方的文件管理，右键点击Sandbox选择【设为启动项目】

