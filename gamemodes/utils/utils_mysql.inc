#include <YSI_Coding\y_hooks>

#define MYSQL_HOST          "localhost"
#define MYSQL_USER          "root"
#define MYSQL_PASS          "usbw"
#define MYSQL_DATABASE      "openmp"

static MySQL:gMySQLHandle;
static pMySQLPlayerRace[MAX_PLAYERS];

forward OnMySQLConnected();
forward OnMySQLDisconnect();

stock MySQL:MySQL_GetHandle() 
{
    return gMySQLHandle;
}

stock GetPlayerMysqlRace(playerid)
{
    return pMySQLPlayerRace[playerid];
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
    switch(errorid) 
    {
        case 1136: printf("[MySQL] 错误 | 列数与值数不符 => \"%s\"", query);
        case 1054: printf("[MySQL] 错误 | 无效字段名 => \"%s\"", query);
        case 1065: printf("[MySQL] 错误 | 查询为空 (变量尺寸太小?) => \"%s\" from \"%s\"", query, callback);
        case 1058: printf("[MySQL] 错误 | 列数与值数不匹配 => \"%s\" from \"%s\"", query, callback);
        case 1203: printf("[MySQL] 错误 | 用户已拥有超过 'max_user_connections' 的活动连接数 => \"%s\" from \"%s\"", query, callback);
        case ER_ACCESS_DENIED_ERROR: printf("[MySQL] 错误 1045 | 拒绝访问");
        case ER_SYNTAX_ERROR: printf("[MySQL] 错误 | SQL语句语法错误 \"%s\"",query);
        default : printf("[MySQL] 错误 %d | \"%s\" (回调: \"%s\" 查询: \"%s\")", errorid, error, callback, query);
    }
    return 1;
}

hook OnGameModeInit() 
{
    new MySQLOpt:option_id = mysql_init_options();
    mysql_set_option(option_id, AUTO_RECONNECT, true);
    gMySQLHandle = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DATABASE, option_id);
    mysql_set_charset("gbk", gMySQLHandle);

    if(gMySQLHandle == MYSQL_INVALID_HANDLE || mysql_errno(gMySQLHandle) != 0) 
    {
        print("[MySQL] MySQL连接失败, 服务器将关闭...");
        SendRconCommand("exit");
        return 1;
    }
    else print("[MySQL] MySQL连接成功.");

    CallLocalFunction("OnMySQLConnected");
    return 1;
}

hook OnGameModeExit()
{
    mysql_close(gMySQLHandle);
    return 1;
}

hook OnPlayerConnect(playerid)
{
    pMySQLPlayerRace[playerid]++;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    pMySQLPlayerRace[playerid]++;
    return 1;
}

stock bool:MysqlRaceCheck(playerid, race)
{
    if(race != pMySQLPlayerRace[playerid])
    {
        SendClientMessage(playerid, 0xE74C3CFF, "错误: 出现了罕见的错误, 请重新进入游戏.");
        Kick(playerid);
        return false;
    }
    return true;
}

public OnMySQLConnected()
{
    return 1;
}
