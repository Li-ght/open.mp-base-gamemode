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
        case 1136: printf("[MySQL] ���� | ������ֵ������ => \"%s\"", query);
        case 1054: printf("[MySQL] ���� | ��Ч�ֶ��� => \"%s\"", query);
        case 1065: printf("[MySQL] ���� | ��ѯΪ�� (�����ߴ�̫С?) => \"%s\" from \"%s\"", query, callback);
        case 1058: printf("[MySQL] ���� | ������ֵ����ƥ�� => \"%s\" from \"%s\"", query, callback);
        case 1203: printf("[MySQL] ���� | �û���ӵ�г��� 'max_user_connections' �Ļ������ => \"%s\" from \"%s\"", query, callback);
        case ER_ACCESS_DENIED_ERROR: printf("[MySQL] ���� 1045 | �ܾ�����");
        case ER_SYNTAX_ERROR: printf("[MySQL] ���� | SQL����﷨���� \"%s\"",query);
        default : printf("[MySQL] ���� %d | \"%s\" (�ص�: \"%s\" ��ѯ: \"%s\")", errorid, error, callback, query);
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
        print("[MySQL] MySQL����ʧ��, ���������ر�...");
        SendRconCommand("exit");
        return 1;
    }
    else print("[MySQL] MySQL���ӳɹ�.");

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
        SendClientMessage(playerid, 0xE74C3CFF, "����: �����˺����Ĵ���, �����½�����Ϸ.");
        Kick(playerid);
        return false;
    }
    return true;
}

public OnMySQLConnected()
{
    return 1;
}