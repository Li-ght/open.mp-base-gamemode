# open.mp基础服务端

请阅读完此文档！！本文档有关于你需要知道的帮助信息，如果还重复提问，纯属浪费彼此时间精力，我会生气的！

本人原创自用的端, 经过很多修改调整, 让它更适合小白人群, 并作为open.mp基础服务端开发包提供给大家

本人开发此基础端的用意是为了让小白在开发 open.mp/sa-mp 初期更加轻松

能暂时先越过在入门阶段可能比较困难的玩家登陆注册等数据储存系统，更快接触到其它更直接的开发内容

通过在基础端添加新的功能，让此基础端往你们希望的方向发展

基础端内的代码你可以尝试进行学习理解或者复刻，如果你遇到跟你一样可能在开发服务器方面有需要帮助的小白，你可以分享此链接给他

关于其它教程和帮助请前往[open.mp/samp相关教程](https://github.com/Li-ght/open.mp-samp-tutorials)

# 模块编程

本人把基础端的模块化估计做得没这么规范，正因为这是面向小白的，我不希望分太多的模块让他们过于迷茫

因此把关于玩家数据、登录注册、数据库等几类做了简单的分模块处理，尽可能让小白接触到对他们而言太陌生繁琐的开发环境

# 如何开启服务器
1. 下载 [USBWebserver](https://www.usbwebserver.net/webserver/)，或者你习惯用的建站软件
2. 把 USBWebserver 解压至电脑某个地方, 如 C 盘
3. 打开 USBWebserver，并在设置里把 MySQL端口改成 3306
4. 在 USBWebserver 常规里点击 PHPMyAdmin 进入网页
5. 创建名为 `openmp` 的数据库
6. 打开服务端内的 `omp-server.exe` 启动服务器

# 选项
1. MySQL数据库名称/用户名/密码 可自行在`gamemodes\utils\utils_mysql.inc`修改
2. 如果你想开启邮箱注册功能, 可在`gamemodes\player\player_account.inc`里，把 USE_EMAIL 设置为 true, 并在 `883` 行处添加发送邮箱功能，本基础服务端不提供相关支持和帮助, 自行在网上寻找邮箱服务器搭建的方法
```pawn
#define USE_EMAIL false
```
3. 服务器信息可在`gamemodes\main.pwn`处修改

# 相关说明
- 许多函数、变量等借鉴了一些其它地方的命名（部分未完成），建议在开发过程中保持格式/规范的统一
- 玩家如果未登录/注册 系统会禁止他们出生 如果玩家使用作弊强制出生，他们会被踢出服务器
- 不对 NPC 进行登录注册
- 登录注册的镜头目前只设了一个，可自行在`gamemodes\player\player_account.inc`添加或修改
- 玩家基础数据包含：注册时间、上次登录时间、上次离线时间、QQ 、金钱、分数、杀敌、死亡、银行存款，自行拓展更多
- 已加入对中文昵称的支持，后续请等待open.mp内置关于非英文昵称的支持
- 已在`main.pwn`加入open.mp所有的回调


# 服务器使用的插件介绍
- [MySQL](https://github.com/pBlueG/SA-MP-MySQL) 最好最著名的SA:MP MySQL插件, 此插件允许您在 PAWN 中使用 MySQL
- [YSI](https://github.com/pawn-lang/YSI-Includes)，是 SA:MP/Pawn 历史最悠久、规模最大、经过大量测试且支持性最好的库，提供了大量的新游戏和语言功能
- [Pawn.CMD](https://github.com/katursis/Pawn.CMD/releases) 最快的指令处理插件
- [samp-bcrypt](https://github.com/Sreyas-Sreelal/samp-bcrypt/) sa-mp最好的加密插件
- MerRandom 更好的随机数获取插件, SA-MP自带的random据说挺糟糕的

# 服务器已内置但未使用的插件
- [sscanf](https://github.com/Y-Less/sscanf/releases) 将字符串转换为多个值、整数、浮点数、玩家等
- [streamer](https://github.com/samp-incognito/samp-streamer-plugin) 绕过许多SA：MP限制
- [PawnPlus](https://github.com/IS4Code/PawnPlus) 通过新的构造、数据类型和编程技术扩展了 Pawn 脚本语言的可能性
- [gvar](https://github.com/samp-incognito/samp-gvar-plugin) 变量系统的扩展，执行时间比属性快得多
  
# API
`utils_mysql`
```pawn
forward OnMySQLConnected();

MySQL_GetHandle()
GetPlayerMysqlRace(playerid)
MysqlRaceCheck(playerid, race)
```
`gamemodes\player\player_account.inc`
```pawn
forward OnPlayerLoggedIn(playerid); // 当玩家登录/注册成功的回调

IsValidPlayer(playerid)             // 是否是有效的玩家
GetPlayerID(playerid)               // 获取玩家数据库ID

RCON 管理员指令:
/apps 查看/管理待审核的玩家 (需要自行开启邮箱验证功能)
```
`gamemodes\player\player_stats.inc`
```pawn
GivePlayerMoney(playerid, money)        // 给予玩家金钱
GetPlayerMoney(playerid)                // 获取玩家金钱
GivePlayerScore(playerid, score)        // 给予玩家分数
SetPlayerScore(playerid, score)         // 设置玩家分数
GetPlayerScore(playerid)                // 获取玩家分数
Player_GetDeposit(playerid)             // 获取玩家存款
Player_GiveDeposit(playerid, money)     // 给予玩家存款
Player_GetKill(playerid)                // 获取玩家杀敌数
Player_GiveKill(playerid, kill)         // 给予玩家杀敌数
Player_SetKill(playerid, kill)          // 设置玩家杀敌数
Player_GetDeath(playerid)               // 获取玩家死亡次数
Player_GiveDeath(playerid, death)       // 给予玩家死亡次数
```
