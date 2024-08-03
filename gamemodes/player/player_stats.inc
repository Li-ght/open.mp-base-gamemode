/***
 *                                       
 *           db         88888888ba   88  
 *          d88b        88      "8b  88  
 *         d8'`8b       88      ,8P  88  
 *        d8'  `8b      88aaaaaa8P'  88  
 *       d8YaaaaY8b     88""""""'    88  
 *      d8""""""""8b    88           88  
 *     d8'        `8b   88           88  
 *    d8'          `8b  88           88  
 *                                       
 *                                   

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
*/

#include <YSI_Coding\y_hooks>

static
    PlayerKill[MAX_PLAYERS],
    PlayerDeath[MAX_PLAYERS],
    PlayerScore[MAX_PLAYERS],
    PlayerMoney[MAX_PLAYERS],
    PlayerBank[MAX_PLAYERS];


hook OnMySQLConnected()
{
    mysql_tquery(MySQL_GetHandle(), "CREATE TABLE IF NOT EXISTS `player_stats` (\
    `uid` mediumint(8),\
    `kills` int(11) DEFAULT '0',\
    `deaths` int(11) DEFAULT '0',\
    `score` int(11) DEFAULT '0',\
    `money` int(11) DEFAULT '0',\
    `bank` int(11) DEFAULT '0',\
    PRIMARY KEY(`uid`),\
    FOREIGN KEY(`uid`) REFERENCES `account`(`uid`) ON DELETE CASCADE ON UPDATE NO ACTION\
    ) ENGINE = InnoDB DEFAULT CHARSET=utf8");
    return 1;
}

hook OnPlayerLoggedIn(playerid)
{
    if(!IsValidPlayer(playerid)) return false;
    new query[64];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT * FROM `player_stats` WHERE `uid` = '%d'", GetPlayerID(playerid));
    mysql_tquery(MySQL_GetHandle(), query, "PlayerStats_Load", "ii", playerid, GetPlayerMysqlRace(playerid));
    return 1;
}

hook OnPlayerConnect(playerid)
{
    PlayerKill[playerid] = 
    PlayerDeath[playerid] = 
    PlayerScore[playerid] = 
    PlayerMoney[playerid] = 
    PlayerBank[playerid] = 0;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if(!IsValidPlayer(playerid)) return false;

    new query[512];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `player_stats` SET `kills` = '%d', `deaths` = '%d', `score` = '%d', `money` = '%d', `bank` = '%d' WHERE `uid` = '%d' LIMIT 1", 
        PlayerKill[playerid],
        PlayerDeath[playerid],
        PlayerScore[playerid],
        PlayerMoney[playerid],
        PlayerBank[playerid],
        GetPlayerID(playerid)
    );
    mysql_tquery(MySQL_GetHandle(), query);
    return 1;
}

forward PlayerStats_Load(playerid, race_check);
public PlayerStats_Load(playerid, race_check)
{
    if(MysqlRaceCheck(playerid, race_check))
    {
        if(cache_num_rows())
        {
            cache_get_value_int(0, "kills", PlayerKill[playerid]);
            cache_get_value_int(0, "deaths", PlayerDeath[playerid]);
            cache_get_value_int(0, "score", PlayerScore[playerid]);
            cache_get_value_int(0, "money", PlayerMoney[playerid]);
            cache_get_value_int(0, "bank", PlayerBank[playerid]);
        }
        else
        {
            new query[128];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT INTO `player_stats` (`uid`) VALUES ('%d')", GetPlayerID(playerid));
            mysql_tquery(MySQL_GetHandle(), query, "PlayerStats_Create", "ii", playerid, GetPlayerMysqlRace(playerid));
        }
    }
    return 1;
}

forward PlayerStats_Create(playerid, race_check);
public PlayerStats_Create(playerid, race_check)
{
    if(MysqlRaceCheck(playerid, race_check))
    {
        PlayerKill[playerid] = 
        PlayerDeath[playerid] = 
        PlayerScore[playerid] = 0;
        PlayerMoney[playerid] = 350;
        PlayerBank[playerid] = 0;
    }
    return 1;
}

hook OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    if(killerid != INVALID_PLAYER_ID)
    {
        PlayerKill[killerid] += 1;
        PlayerDeath[playerid] += 1;
    }
    return 1;
}

stock orig_GivePlayerMoney(playerid, money) return GivePlayerMoney(playerid, money);
stock stats_GivePlayerMoney(playerid, money)
{
    if(GivePlayerMoney(playerid, money))
    {
        PlayerMoney[playerid] += money;
        return true;
    }   
    return false;
}
#if defined _ALS_GivePlayerMoney
    #undef GivePlayerMoney
#else
    #define _ALS_GivePlayerMoney
#endif
#define GivePlayerMoney stats_GivePlayerMoney

stock orig_GetPlayerMoney(playerid) return GetPlayerMoney(playerid);
stock stats_GetPlayerMoney(playerid)
{
    return PlayerMoney[playerid];
}
#if defined _ALS_GetPlayerMoney
    #undef GetPlayerMoney
#else
    #define _ALS_GetPlayerMoney
#endif
#define GetPlayerMoney stats_GetPlayerMoney

stock GivePlayerScore(playerid, score)
{
    PlayerScore[playerid] += score;
    SetPlayerScore(playerid, PlayerScore[playerid]);
    return;
}

stock orig_SetPlayerScore(playerid,score) return SetPlayerScore(playerid,score);
stock stats_SetPlayerScore(playerid, score)
{
    PlayerScore[playerid] = score;
    SetPlayerScore(playerid, PlayerScore[playerid]);
    return;
}
#if defined _ALS_SetPlayerScore
    #undef SetPlayerScore
#else
    #define _ALS_SetPlayerScore
#endif
#define SetPlayerScore stats_SetPlayerScore

stock orig_GetPlayerScore(playerid) return GetPlayerScore(playerid);
stock stats_GetPlayerScore(playerid)
{
    return PlayerScore[playerid];
}
#if defined _ALS_GetPlayerScore
    #undef GetPlayerScore
#else
    #define _ALS_GetPlayerScore
#endif
#define GetPlayerScore stats_GetPlayerScore

stock Player_GetDeposit(playerid)
{
    return PlayerBank[playerid];
}

stock Player_GiveDeposit(playerid, money)
{
    PlayerBank[playerid] += money;
    return;
}

stock Player_GetKill(playerid)
{
    return PlayerKill[playerid];
}

stock Player_GiveKill(playerid, kill)
{
    PlayerKill[playerid] += kill;
    return;
}

stock Player_SetKill(playerid, kill)
{
    PlayerKill[playerid] = kill;
    return;
}

stock Player_GetDeath(playerid)
{
    return PlayerDeaths[playerid];
}

stock Player_GiveDeath(playerid, death)
{
    PlayerDeath[playerid] += death;
    return;
}

hook OnPlayerUpdate(playerid)
{
	if(orig_GetPlayerMoney(playerid) != PlayerMoney[playerid])
	{
	    ResetPlayerMoney(playerid);
		orig_GivePlayerMoney(playerid, PlayerMoney[playerid]);
	}
	if(orig_GetPlayerScore(playerid) != PlayerScore[playerid]) orig_SetPlayerScore(playerid, PlayerScore[playerid]);
	return 1;
}