/***
 *                                                                   
 *             88                 ad88  88                           
 *             88                d8"    ""                           
 *             88                88                                  
 *     ,adPPYb,88   ,adPPYba,  MM88MMM  88  8b,dPPYba,    ,adPPYba,  
 *    a8"    `Y88  a8P_____88    88     88  88P'   `"8a  a8P_____88  
 *    8b       88  8PP"""""""    88     88  88       88  8PP"""""""  
 *    "8a,   ,d88  "8b,   ,aa    88     88  88       88  "8b,   ,aa  
 *     `"8bbdP"Y8   `"Ybbd8"'    88     88  88       88   `"Ybbd8"'  
 *                                                                   
 *                                                                   
 */




#define USE_EMAIL false

#define MIN_EMAIL_LENGTH            (7)
#define MAX_EMAIL_LENGTH            (13)
#define VERIFY_CODE_LENGTH          (6)

#define MAX_PASSWORD_ATTEMPT        (5)
#define MIN_PASSWORD_LENGTH         (4)
#define MAX_PASSWORD_LENGTH         (16)

#define INVALID_DATABESE_ID         (-1)

enum
{
    DIALOG_SERVER_RULE = 1,
    DIALOG_LOGIN,
    DIALOG_REGISTER,
    DIALOG_EMAIL_INPUT,
    DIALOG_WAIT_VERIFYCODE,
    DIALOG_VERIFY_CODE,
    DIALOG_VERIFY_LIST,
    DIALOG_VERIFY_OPTION
};

/***
 *                                                                                      
 *                                         88              88           88              
 *                                         ""              88           88              
 *                                                         88           88              
 *    8b       d8  ,adPPYYba,  8b,dPPYba,  88  ,adPPYYba,  88,dPPYba,   88   ,adPPYba,  
 *    `8b     d8'  ""     `Y8  88P'   "Y8  88  ""     `Y8  88P'    "8a  88  a8P_____88  
 *     `8b   d8'   ,adPPPPP88  88          88  ,adPPPPP88  88       d8  88  8PP"""""""  
 *      `8b,d8'    88,    ,88  88          88  88,    ,88  88b,   ,a8"  88  "8b,   ,aa  
 *        "8"      `"8bbdP"Y8  88          88  `"8bbdP"Y8  8Y"Ybbd8"'   88   `"Ybbd8"'  
 *                                                                                      
 *                                                                                      
 */

enum E_LOGIN_CAMERA_DATA
{
    Float:pos_x,
    Float:pos_y,
    Float:pos_z,
    Float:lookat_x,
    Float:lookat_y,
    Float:lookat_z
};

static const LoginCameraData[][E_LOGIN_CAMERA_DATA] =       //在下面添加登陆注册镜头的坐标和镜头看向的位置
{
    {1735.6965, -1328.4531, 687.8942, 1735.6871, -1329.4518, 677.9838}
};

static 
    pAccountID[MAX_PLAYERS] = {INVALID_DATABESE_ID, ...},
    pAccountName[MAX_PLAYERS][MAX_PLAYER_NAME + 1],
    pAccountPassword[MAX_PLAYERS][BCRYPT_HASH_LENGTH],
    pAccountRegisterDate[MAX_PLAYERS],
    pAccountLoginDate[MAX_PLAYERS],
    pAccountLeaveDate[MAX_PLAYERS];

static  
    pAccountEmail[MAX_PLAYERS][MAX_EMAIL_LENGTH],
    pAccountVerifyCode[MAX_PLAYERS],
    bool:pAccountActive[MAX_PLAYERS] = {false, ...};

static
    bool:pAccountLogged[MAX_PLAYERS] = {false, ...};

static
    pAccountLoginTimer[MAX_PLAYERS] = {-1, ...},
    pAccountTick[MAX_PLAYERS],
    pAccountLoginAttempt[MAX_PLAYERS] = {-1, ...};


/***
 *                                                           
 *                                                88         
 *                 ,d                             88         
 *                 88                             88         
 *    ,adPPYba,  MM88MMM  ,adPPYba,    ,adPPYba,  88   ,d8   
 *    I8[    ""    88    a8"     "8a  a8"     ""  88 ,a8"    
 *     `"Y8ba,     88    8b       d8  8b          8888[      
 *    aa    ]8I    88,   "8a,   ,a8"  "8a,   ,aa  88`"Yba,   
 *    `"YbbdP"'    "Y888  `"YbbdP"'    `"Ybbd8"'  88   `Y8a  
 *                                                           
 *                                                           
 */

static stock bool:IsValidPassword(const string[])
{
    if(strlen(string) < MIN_PASSWORD_LENGTH || strlen(string) > MAX_PASSWORD_LENGTH)
        return false;

    for(new i = 0; string[i] != EOS; ++i) {
        switch(string[i]) {
            case '0'..'9', 'A'..'Z', 'a'..'z': continue;
            default: return false;
        }
    }
    return true;
}

stock bool:IsValidPlayer(playerid)
{
    if(!IsPlayerConnected(playerid)) return false;
    if(IsPlayerNPC(playerid)) return false;

    if(pAccountLogged[playerid] == true && pAccountID[playerid] != INVALID_DATABESE_ID)
    {
        #if USE_EMAIL
            if(pAccountActive[playerid] == true) return true;
        #else
            return true;
        #endif
    }
    return false;
}

stock GetPlayerID(playerid)
{
    if(!IsPlayerConnected(playerid)) return INVALID_DATABESE_ID;
    if(IsPlayerNPC(playerid)) return INVALID_DATABESE_ID;

    if(pAccountLogged[playerid] == true && pAccountID[playerid] != INVALID_DATABESE_ID)
    {
        #if USE_EMAIL
            if(pAccountActive[playerid] == true) return pAccountID[playerid];
        #else
            return pAccountID[playerid];
        #endif
    }
    return INVALID_DATABESE_ID;
}

/***
 *                                                                                       
 *      ,ad8888ba,               88  88  88888888ba                           88         
 *     d8"'    `"8b              88  88  88      "8b                          88         
 *    d8'                        88  88  88      ,8P                          88         
 *    88             ,adPPYYba,  88  88  88aaaaaa8P'  ,adPPYYba,   ,adPPYba,  88   ,d8   
 *    88             ""     `Y8  88  88  88""""""8b,  ""     `Y8  a8"     ""  88 ,a8"    
 *    Y8,            ,adPPPPP88  88  88  88      `8b  ,adPPPPP88  8b          8888[      
 *     Y8a.    .a8P  88,    ,88  88  88  88      a8P  88,    ,88  "8a,   ,aa  88`"Yba,   
 *      `"Y8888Y"'   `"8bbdP"Y8  88  88  88888888P"   `"8bbdP"Y8   `"Ybbd8"'  88   `Y8a  
 *                                                                                       
 *                                                                                       
 */

forward KickWithoutLogin(playerid);
public KickWithoutLogin(playerid)
{
    if(pAccountLoginTimer[playerid] != -1)
    {
        KillTimer(pAccountLoginTimer[playerid]);
        pAccountLoginTimer[playerid] = -1;
    }
    if(pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID)
    {
        SendClientMessage(playerid, 0xE74C3CFF, "提示: 您因60秒内未登录被系统踢出了服务器.");
        Kick(playerid);
    }
    return 1;
}

forward SetupLoginCamera(playerid);
public SetupLoginCamera(playerid)
{
    pAccountLoginAttempt[playerid] = 0;
    if(pAccountLoginTimer[playerid] != -1)
    {
        KillTimer(pAccountLoginTimer[playerid]);
    }
    pAccountLoginTimer[playerid] = SetTimerEx("KickWithoutLogin", 65*1000, false, "i", playerid);

    ShowPlayerDialog(playerid, DIALOG_SERVER_RULE, DIALOG_STYLE_MSGBOX, "{d2d2d2}规则", 
        "{FFFFFF}1. 昵称规范{AAAAAA}\n昵称不得涉及政治敏感、辱骂、擦边性质等\n允许中文、化名、简称、小名或是英文\n禁止携带特殊文字\n禁止仿冒他人昵称\n\n\
        {FFFFFF}2. 不良内容{AAAAAA}\n禁止恶意辱骂、喷脏、引战、滋事.\n禁止发布广告、违规宣传\n禁止讨论敏感政治或恶劣内容\n\n\
        {FFFFFF}3. 严禁作弊{AAAAAA}\n禁止安装为您提供不公平优势的第三方修改和mod\n禁止恶意利用游戏BUG获得某些便利", 
        "同意", 
        "拒绝"
    );

    new tvar = MRandom(sizeof(LoginCameraData));
    SetPlayerCameraLookAt(playerid, LoginCameraData[tvar][lookat_x], LoginCameraData[tvar][lookat_y], LoginCameraData[tvar][lookat_z], CAMERA_CUT);
    SetPlayerCameraPos(playerid, LoginCameraData[tvar][pos_x], LoginCameraData[tvar][pos_y], LoginCameraData[tvar][pos_z]);
    return 1;
}

#include <YSI_Coding\y_hooks>

/***
 *                                                                                                    
 *    88888888ba,                                     88888888ba                                      
 *    88      `"8b                 ,d                 88      "8b                                     
 *    88        `8b                88                 88      ,8P                                     
 *    88         88  ,adPPYYba,  MM88MMM  ,adPPYYba,  88aaaaaa8P'  ,adPPYYba,  ,adPPYba,   ,adPPYba,  
 *    88         88  ""     `Y8    88     ""     `Y8  88""""""8b,  ""     `Y8  I8[    ""  a8P_____88  
 *    88         8P  ,adPPPPP88    88     ,adPPPPP88  88      `8b  ,adPPPPP88   `"Y8ba,   8PP"""""""  
 *    88      .a8P   88,    ,88    88,    88,    ,88  88      a8P  88,    ,88  aa    ]8I  "8b,   ,aa  
 *    88888888Y"'    `"8bbdP"Y8    "Y888  `"8bbdP"Y8  88888888P"   `"8bbdP"Y8  `"YbbdP"'   `"Ybbd8"'  
 *                                                                                                    
 *                                                                                                    
 */

hook OnMySQLConnected()
{
    mysql_tquery(MySQL_GetHandle(), "CREATE TABLE IF NOT EXISTS `account` (\
    `uid` mediumint(8) NOT NULL AUTO_INCREMENT,\
    `name` varchar(25) CHARACTER SET utf8 DEFAULT NULL,\
    `password` varchar(65) DEFAULT NULL,\
    `registerdate` int(10) unsigned DEFAULT '0',\
    `logindate` int(10) unsigned DEFAULT '0',\
    `leavedate` int(10) unsigned DEFAULT '0',\
    `email` varchar(13) DEFAULT '0',\
    `verifycode` mediumint(8) unsigned DEFAULT '0',\
    `active` tinyint(1) DEFAULT '0',\
    PRIMARY KEY (`uid`),\
    UNIQUE KEY `name` (`name`)) \
    ENGINE=InnoDB DEFAULT CHARSET=utf8;");
    
    mysql_tquery(MySQL_GetHandle(), "CREATE TABLE IF NOT EXISTS `email_blacklist` (`email` varchar(13) DEFAULT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
    return 1;
}


/***
 *                                                      
 *    88                                     88         
 *    88                                     88         
 *    88                                     88         
 *    88,dPPYba,    ,adPPYba,    ,adPPYba,   88   ,d8   
 *    88P'    "8a  a8"     "8a  a8"     "8a  88 ,a8"    
 *    88       88  8b       d8  8b       d8  8888[      
 *    88       88  "8a,   ,a8"  "8a,   ,a8"  88`"Yba,   
 *    88       88   `"YbbdP"'    `"YbbdP"'   88   `Y8a  
 *                                                      
 *                                                      
 */

hook OnPlayerConnect(playerid)
{
    if(IsPlayerNPC(playerid)) return 1;
    
    pAccountID[playerid] = INVALID_DATABESE_ID;
    pAccountName[playerid][0] = '\0';
    pAccountPassword[playerid][0] = '\0';
    pAccountRegisterDate[playerid] = 
    pAccountLoginDate[playerid] = 
    pAccountLeaveDate[playerid] = 0;
    pAccountEmail[playerid][0] = '\0';
    pAccountVerifyCode[playerid] = 0;
    pAccountActive[playerid] = false;

    pAccountLogged[playerid] = false;

    if(pAccountLoginTimer[playerid] != -1)
    {
        KillTimer(pAccountLoginTimer[playerid]);
        pAccountLoginTimer[playerid] = -1;
    }
    pAccountTick[playerid] = 0;
    pAccountLoginAttempt[playerid] = -1;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if(pAccountLoginTimer[playerid] != -1)
    {
        KillTimer(pAccountLoginTimer[playerid]);
        pAccountLoginTimer[playerid] = -1;
    }
    pAccountTick[playerid] = 0;
    pAccountLoginAttempt[playerid] = -1;

    if(pAccountLogged[playerid] == true && pAccountID[playerid] != INVALID_DATABESE_ID)
    {
        new query[128];
        mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `leavedate` = '%d', `logindate` = '%d' WHERE `uid` = '%d' LIMIT 1", gettime(), pAccountLoginDate[playerid], pAccountID[playerid]);
        mysql_tquery(MySQL_GetHandle(), query);
    }
    return 1;
}

hook OnPlayerRequestClass(playerid, classid)
{
    if(IsPlayerNPC(playerid)) return 1;
    if(pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID)
    {
        if(pAccountLoginAttempt[playerid] == -1)
        {
            if(pAccountLoginTimer[playerid] != -1)
            {
                KillTimer(pAccountLoginTimer[playerid]);
            }
            pAccountLoginTimer[playerid] = SetTimerEx("SetupLoginCamera", 250, false, "i", playerid);
        }
    }
    return 1;
}

hook OnPlayerRequestSpawn(playerid)
{
    #if USE_EMAIL
        if(pAccountActive[playerid] == false || pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID) return Y_HOOKS_BREAK_RETURN_0;
    #else
        if(pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID) return Y_HOOKS_BREAK_RETURN_0;
    #endif
    return 1;
}

hook OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
    if(IsPlayerNPC(playerid)) return 1;

    if(IsPlayerSpawned(playerid))
    {
        #if USE_EMAIL
            if(pAccountActive[playerid] == false || pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID)
            {
                SendClientMessage(playerid, 0xE74C3CFF, "提示: 您已被踢出服务器, 原因: 未登录账号.");
                return Kick(playerid);
            }
        #else
            if(pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID)
            {
                SendClientMessage(playerid, 0xE74C3CFF, "提示: 您已被踢出服务器, 原因: 未登录账号.");
                return Kick(playerid);
            }
        #endif
    }
    return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_SERVER_RULE:
        {
            if(!response) return Kick(playerid), Y_HOOKS_BREAK_RETURN_0;
            
            GetPlayerName(playerid, pAccountName[playerid], MAX_PLAYER_NAME + 1);
            for(new i; pAccountName[playerid][i] != 0; i ++)
            {
                if(pAccountName[playerid][i] < 0)
                    pAccountName[playerid][i] += 256;
            }
            new query[62 + MAX_PLAYER_NAME + 1];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT `uid`, `password` FROM `account` WHERE `name` = '%e' LIMIT 1", pAccountName[playerid]);
            mysql_tquery(MySQL_GetHandle(), query, "Account_CheckUser", "ii", playerid, GetPlayerMysqlRace(playerid));
            return Y_HOOKS_BREAK_RETURN_0;
        }
        case DIALOG_LOGIN:
        {
            if(!response) return Kick(playerid), Y_HOOKS_BREAK_RETURN_0;
            if(!IsValidPassword(inputtext))
            {
                SendClientMessage(playerid, 0xE74C3CFF, "提示: 请确保您的密码没有使用任何禁用的字符(仅使用 0-9、a-z), 并且密码长度在 %d - %d 个字符范围.", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);

                pAccountLoginAttempt[playerid] += 1;
                if(pAccountLoginAttempt[playerid] >= MAX_PASSWORD_ATTEMPT)
                {
                    SendClientMessage(playerid, 0xE74C3CFF, "提示: 您因登录失败次数过多被系统踢出了服务器.");
                    Kick(playerid);
                    return 1;
                }
                ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "登陆", 
                    "{FFFFFF}您好, 欢迎回来, 请在下方输入您的密码登录游戏\n\
                    温馨提示: 请在60秒之内登录您的账号.\n\n\
                    {FF6347}剩余尝试次数: %d", 
                    "登录", "退出", 
                    MAX_PASSWORD_ATTEMPT - pAccountLoginAttempt[playerid]);
                
                return Y_HOOKS_BREAK_RETURN_0;
            }
            else bcrypt_verify(playerid, "Account_HashCheck", inputtext, pAccountPassword[playerid], "i", GetPlayerMysqlRace(playerid));
            return Y_HOOKS_BREAK_RETURN_0;
        }
        case DIALOG_REGISTER:
        {
            if(!response) return Kick(playerid), Y_HOOKS_BREAK_RETURN_0;
            if(!IsValidPassword(inputtext))
            {
                SendClientMessage(playerid, 0xE74C3CFF, "提示: 请确保您的密码没有使用任何禁用的字符(仅使用 0-9、a-z), 并且密码长度在 %d - %d 个字符范围.", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);

                ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "注册", 
                    "{FFFFFF}您好, 请在下方输入密码创建一个账号(使用 %d - %d 个字符).\n\
                    温馨提示: 注册后请妥善保管好您的密码.", 
                    "注册", "退出",
                    MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH
                );
                return Y_HOOKS_BREAK_RETURN_0;
            }
            else bcrypt_hash(playerid, "Account_HashPassword", inputtext, BCRYPT_COST, "ii", playerid, GetPlayerMysqlRace(playerid));
            return Y_HOOKS_BREAK_RETURN_0;
        }
    }
    return 1;
}


/***
 *                                                                           
 *             88                                                        88  
 *      ,d     88                                                        88  
 *      88     88                                                        88  
 *    MM88MMM  88,dPPYba,   8b,dPPYba,   ,adPPYba,  ,adPPYYba,   ,adPPYb,88  
 *      88     88P'    "8a  88P'   "Y8  a8P_____88  ""     `Y8  a8"    `Y88  
 *      88     88       88  88          8PP"""""""  ,adPPPPP88  8b       88  
 *      88,    88       88  88          "8b,   ,aa  88,    ,88  "8a,   ,d88  
 *      "Y888  88       88  88           `"Ybbd8"'  `"8bbdP"Y8   `"8bbdP"Y8  
 *                                                                           
 *                                                                           
 */

forward Account_CheckUser(playerid, raceCheck);
public Account_CheckUser(playerid, raceCheck)
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(cache_num_rows())
        {
            cache_get_value_int(0, "uid", pAccountID[playerid]);
            cache_get_value(0, "password", pAccountPassword[playerid], BCRYPT_HASH_LENGTH);

            ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "登陆", 
                "{FFFFFF}您好, 欢迎回来, 请在下方输入您的密码登录游戏\n\
                温馨提示: 请在60秒之内登录您的账号.", "登录", "退出"
            );
        }
        else ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "注册", 
            "{FFFFFF}您好, 请在下方输入密码创建一个账号(使用4-16个字符).\n\
            温馨提示: 注册后请妥善保管好您的密码.", "注册", "退出"
        );
    }
    return;
}

forward Account_LoginSuccessful(playerid, raceCheck);
public Account_LoginSuccessful(playerid, raceCheck) 
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        pAccountLogged[playerid] = true;

        cache_get_value_int(0, "registerdate", pAccountRegisterDate[playerid]);
        cache_get_value_int(0, "logindate", pAccountLoginDate[playerid]);
        cache_get_value_int(0, "leavedate", pAccountLeaveDate[playerid]);
        cache_get_value(0, "email", pAccountEmail[playerid], MAX_EMAIL_LENGTH);
        cache_get_value_int(0, "verifycode", pAccountVerifyCode[playerid]);
        cache_get_value_bool(0, "active", pAccountActive[playerid]);

        pAccountLoginDate[playerid] = gettime();
        
        Account_Validation(playerid);
    }
    return 1;
}

forward Account_RegisterSuccessful(playerid, raceCheck);
public Account_RegisterSuccessful(playerid, raceCheck) 
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        pAccountLogged[playerid] = true;

        new curTime = gettime();
        
        pAccountID[playerid] = cache_insert_id();
        pAccountRegisterDate[playerid] = curTime;
        pAccountLoginDate[playerid] = curTime;
        pAccountLeaveDate[playerid] = 0;
        pAccountEmail[playerid][0] = '\0';
        pAccountVerifyCode[playerid] = 0;
        pAccountActive[playerid] = false;

        Account_Validation(playerid);
        
    }
    return 1;
}

forward Account_HashPassword(playerid, hashid, raceCheck);
public Account_HashPassword(playerid, hashid, raceCheck) 
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(pAccountLoginTimer[playerid] != -1)
        {
            KillTimer(pAccountLoginTimer[playerid]);
            pAccountLoginTimer[playerid] = -1;
        }

        bcrypt_get_hash(pAccountPassword[playerid]);

        new query[512];
        mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT INTO `account` (`name`, `password`, `registerdate`, `logindate`) VALUES ('%e', '%e', '%d', '%d')", pAccountName[playerid], pAccountPassword[playerid], gettime(), gettime());
        mysql_tquery(MySQL_GetHandle(), query, "Account_RegisterSuccessful", "ii", playerid, GetPlayerMysqlRace(playerid));  
    }
    return 1;
}

forward Account_HashCheck(playerid, bool:success, raceCheck);
public Account_HashCheck(playerid, bool:success, raceCheck)
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(success == true)
        {   
            if(pAccountLoginTimer[playerid] != -1)
            {
                KillTimer(pAccountLoginTimer[playerid]);
                pAccountLoginTimer[playerid] = -1;
            } 
            new query[130 + MAX_PLAYER_NAME + 1];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT * FROM `account` WHERE `uid` = '%d' LIMIT 1", pAccountID[playerid]);
            mysql_tquery(MySQL_GetHandle(), query, "Account_LoginSuccessful", "ii", playerid, GetPlayerMysqlRace(playerid));
        }
        else
        {
            pAccountLoginAttempt[playerid] += 1;
            if(pAccountLoginAttempt[playerid] >= MAX_PASSWORD_ATTEMPT)
            {
                SendClientMessage(playerid, 0xE74C3CFF, "提示: 您因登录失败次数过多被系统踢出了服务器.");
                Kick(playerid);
                return 1;
            }
            ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "登陆", 
                "{FFFFFF}您好, 欢迎回来, 请在下方输入您的密码登录游戏\n\
                温馨提示: 请在60秒内登录您的账号.\n\n\
                {FF6347}剩余尝试次数: %d", 
                "登录", "退出", 
                MAX_PASSWORD_ATTEMPT - pAccountLoginAttempt[playerid]);
        }
    }
    return 1;
}

/***
 *                                                                                                                                         
 *                                                88  88        ad88                                                                       
 *                                                ""  88       d8"                             ,d                                          
 *                                                    88       88                              88                                          
 *     ,adPPYba,  88,dPYba,,adPYba,   ,adPPYYba,  88  88     MM88MMM  ,adPPYba,  ,adPPYYba,  MM88MMM  88       88  8b,dPPYba,   ,adPPYba,  
 *    a8P_____88  88P'   "88"    "8a  ""     `Y8  88  88       88    a8P_____88  ""     `Y8    88     88       88  88P'   "Y8  a8P_____88  
 *    8PP"""""""  88      88      88  ,adPPPPP88  88  88       88    8PP"""""""  ,adPPPPP88    88     88       88  88          8PP"""""""  
 *    "8b,   ,aa  88      88      88  88,    ,88  88  88       88    "8b,   ,aa  88,    ,88    88,    "8a,   ,a88  88          "8b,   ,aa  
 *     `"Ybbd8"'  88      88      88  `"8bbdP"Y8  88  88       88     `"Ybbd8"'  `"8bbdP"Y8    "Y888   `"YbbdP'Y8  88           `"Ybbd8"'  
 *                                                                                                                                         
 *                                                                                                                                         
 */

static stock bool:IsValidEmail(const string[])
{
    new len;
    if(!(len = strlen(string))) return false;
    for(new i; i < len; i++)
    {
        if (string[i] < '0' || string[i] > '9') return false;
    }
    if(len < MIN_EMAIL_LENGTH || len >= MAX_EMAIL_LENGTH) return false;
    return true;
}

static stock bool:IsVaildVerifyCode(const code[])
{
    new len = strlen(code);
    if(len != VERIFY_CODE_LENGTH) return false;
    for(new i; i < len; i++)
    {
        if (code[i] < '0' || code[i] > '9') return false;
    }
    return true;
}

static stock bool:Account_SetUnactivated(playerid)
{
    if(pAccountLogged[playerid] == true && pAccountID[playerid] != INVALID_DATABESE_ID)
    {
        pAccountVerifyCode[playerid] = 0;
        pAccountEmail[playerid][0] = '\0';
        pAccountActive[playerid] = false;

        new query[95 + 10];
        mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `email` = '0', `verifycode` = '0', `active` = '0' WHERE `uid` = '%d' LIMIT 1", pAccountID[playerid]);
        mysql_tquery(MySQL_GetHandle(), query);
        return true;
    }
    return false;
}

static stock Account_Validation(playerid)
{
    #if USE_EMAIL
        if(pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID)
        {
            Kick(playerid);
            return 1;
        }
        if(pAccountActive[playerid] == false)
        {
            if(IsValidEmail(pAccountEmail[playerid]) == false)
            {
                if(pAccountVerifyCode[playerid] == 88)
                {
                    Account_SetUnactivated(playerid);
                    SendClientMessage(playerid, 0xFF6347FF, "此QQ号码未通过管理员验证[原因: 小号], 请更换其他的QQ号码.");
                }
                ShowPlayerDialog(playerid, DIALOG_EMAIL_INPUT, DIALOG_STYLE_INPUT, "{FF6347}绑定QQ", "{FFFFFF}该用户名尚未激活, {FF6347}防止小号泛滥, 影响游戏体验\n{FFFFFF}请在下方输入您要绑定的QQ号码以接收{FF6347}验证码", "输入", "退出");
                return 1;
            }
            else
            {
                if(pAccountVerifyCode[playerid] == 0)
                {
                    ShowPlayerDialog(playerid, DIALOG_WAIT_VERIFYCODE, DIALOG_STYLE_INPUT, "{FF6347}验证审核中", "{FFFFFF}您的邮箱为: %s@qq.com, 请耐心等待{FF6347}验证审核\n{FFFFFF}你可以下线等待验证或输入{FF6347}changemail{FFFFFF}更换邮箱", "刷新", "下线等待", pAccountEmail[playerid]);
                    return 1;
                }
                else if(pAccountVerifyCode[playerid] == 88)
                {
                    if(Account_SetUnactivated(playerid))
                    {
                        SendClientMessage(playerid, 0xFF6347FF, "此QQ号码未通过管理员验证[原因: 小号], 请更换其他的QQ号码.");
                        ShowPlayerDialog(playerid, DIALOG_EMAIL_INPUT, DIALOG_STYLE_INPUT, "{FF6347}绑定QQ", "{FFFFFF}该用户名尚未激活, {FF6347}防止小号泛滥, 影响游戏体验\n{FFFFFF}请在下方输入您要绑定的QQ号码以接收{FF6347}验证码", "输入", "退出");
                    }
                    return 1;
                }
                ShowPlayerDialog(playerid, DIALOG_VERIFY_CODE, DIALOG_STYLE_INPUT, "{FF6347}验证码", "{FFFFFF}验证码已发送至您的邮箱: %s@qq.com\n{FF6347}如未收到, 请检查邮件是否纳入垃圾箱", "输入", "退出", pAccountEmail[playerid]);
            }
            return 1;
        }
        else CallLocalFunction("OnPlayerLoggedIn", "i", playerid);
    #else
        CallLocalFunction("OnPlayerLoggedIn", "i", playerid);
    #endif
    return 1;
}

#include <YSI_Coding\y_hooks>

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_EMAIL_INPUT:
        {
            if(!response) return Kick(playerid), Y_HOOKS_BREAK_RETURN_0;
            if(!IsValidEmail(inputtext))
            {
                SendClientMessage(playerid, 0xFF6347FF, "您输入的QQ号码有误, 请输入正确的QQ号码.");
                ShowPlayerDialog(playerid, DIALOG_EMAIL_INPUT, DIALOG_STYLE_INPUT, "{FF6347}绑定QQ", "{FFFFFF}该用户名尚未激活, {FF6347}防止小号泛滥, 影响游戏体验\n{FFFFFF}请在下方输入您要绑定的QQ号码以接收{FF6347}验证码", "输入", "退出");
                return Y_HOOKS_BREAK_RETURN_0;
            }
            pAccountEmail[playerid][0] = '\0';
            strcat(pAccountEmail[playerid], inputtext, MAX_EMAIL_LENGTH);
            new query[64 + MAX_EMAIL_LENGTH];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT * FROM `account` WHERE `email` = '%s' LIMIT 1", pAccountEmail[playerid]);
            mysql_tquery(MySQL_GetHandle(), query, "Account_CheckEmail", "ii", playerid, GetPlayerMysqlRace(playerid));
            return Y_HOOKS_BREAK_RETURN_0;
        }
        case DIALOG_WAIT_VERIFYCODE:
        {
            if(!response) return Kick(playerid), Y_HOOKS_BREAK_RETURN_0;
            if((GetTickCount() - pAccountTick[playerid]) < 5000)
            {
                ShowPlayerDialog(playerid, DIALOG_WAIT_VERIFYCODE, DIALOG_STYLE_INPUT, "{FF6347}验证审核中", 
                    "{FFFFFF}您的邮箱为: %s@qq.com, 请耐心等待{FF6347}验证审核\n\
                    {FFFFFF}你可以下线等待验证或输入{FF6347}changemail{FFFFFF}更换邮箱\n\n\
                    {FF6347}请等待%d秒后再刷新", 
                    "刷新", "下线等待",
                    pAccountEmail[playerid], 5 - ((GetTickCount() - pAccountTick[playerid])/1000)
                );
                return Y_HOOKS_BREAK_RETURN_0; 
            }
            pAccountTick[playerid] = GetTickCount();

            if(!strcmp("changemail", inputtext) && strlen(inputtext) != 0)
            {
                if(Account_SetUnactivated(playerid))
                    Account_Validation(playerid);

                return Y_HOOKS_BREAK_RETURN_0;
            }
            new query[49 + 10];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT * FROM `account` WHERE `uid` = '%d' LIMIT 1", pAccountID[playerid]);
            mysql_tquery(MySQL_GetHandle(), query, "Account_ReflashVerifyCode", "ii", playerid, GetPlayerMysqlRace(playerid));//
            return Y_HOOKS_BREAK_RETURN_0;
        }
        case DIALOG_VERIFY_CODE:
        {
            if(!response) return Kick(playerid), Y_HOOKS_BREAK_RETURN_0;
            if(!IsVaildVerifyCode(inputtext))
            {
                SendClientMessage(playerid, 0xFF6347FF, "您输入的验证码有误, 请检查后重新输入.");
                Account_Validation(playerid);
                return Y_HOOKS_BREAK_RETURN_0;
            }
            new query[128];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT * FROM `account` WHERE `verifycode` = '%d' AND `uid` = '%d' LIMIT 1", strval(inputtext), pAccountID[playerid]);
            mysql_tquery(MySQL_GetHandle(), query, "Account_CheckVerifyCode", "ii", playerid, GetPlayerMysqlRace(playerid));
            return Y_HOOKS_BREAK_RETURN_0;
        }
    }
    return 1;
}

/***
 *                                                                           
 *             88                                                        88  
 *      ,d     88                                                        88  
 *      88     88                                                        88  
 *    MM88MMM  88,dPPYba,   8b,dPPYba,   ,adPPYba,  ,adPPYYba,   ,adPPYb,88  
 *      88     88P'    "8a  88P'   "Y8  a8P_____88  ""     `Y8  a8"    `Y88  
 *      88     88       88  88          8PP"""""""  ,adPPPPP88  8b       88  
 *      88,    88       88  88          "8b,   ,aa  88,    ,88  "8a,   ,d88  
 *      "Y888  88       88  88           `"Ybbd8"'  `"8bbdP"Y8   `"8bbdP"Y8  
 *                                                                           
 *                                                                           
 */

forward Account_CheckVerifyCode(playerid, raceCheck);
public Account_CheckVerifyCode(playerid, raceCheck) 
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(cache_num_rows())
        {
            new query[58 + 10];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `active` = 1 WHERE `uid` = '%d' LIMIT 1", pAccountID[playerid]);
            mysql_tquery(MySQL_GetHandle(), query);
            pAccountActive[playerid] = true;
            CallLocalFunction("OnPlayerLoggedIn", "i", playerid);
            return 1;
        }
        SendClientMessage(playerid, 0xFF6347FF, "您输入的验证码有误, 请检查后重新输入.");
        Account_Validation(playerid);
    }
    return 1;
}

forward Account_ReflashVerifyCode(playerid, raceCheck);
public Account_ReflashVerifyCode(playerid, raceCheck) 
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(cache_num_rows())
            cache_get_value_int(0, "verifycode", pAccountVerifyCode[playerid]);

        Account_Validation(playerid);
    }
    return 1;
}

forward Account_CheckEmail(playerid, raceCheck);
public Account_CheckEmail(playerid, raceCheck)
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(cache_num_rows())
        {
            pAccountEmail[playerid][0] = '\0';
            SendClientMessage(playerid, 0xFF6347FF, "此QQ号码被占用, 请更换QQ号码, 如有恶意占用, 请联系管理员处理.");
            Account_Validation(playerid);
            return 1;
        }
        new query[60 + MAX_EMAIL_LENGTH];
        mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT * FROM `email_blacklist` WHERE `email` = '%s' LIMIT 1", pAccountEmail[playerid]);
        mysql_tquery(MySQL_GetHandle(), query, "Account_CheckEmailEx", "ii", playerid, GetPlayerMysqlRace(playerid));
    }
    return 1;
}

forward Account_CheckEmailEx(playerid, raceCheck);
public Account_CheckEmailEx(playerid, raceCheck)
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        if(cache_num_rows())
        {
            pAccountEmail[playerid][0] = '\0';
            SendClientMessage(playerid, 0xFF6347FF, "此QQ号码被服务器列入了小号清单, 请更换其他的QQ号码.");
            Account_Validation(playerid);
        }
        else
        {
            new query[61 + MAX_EMAIL_LENGTH + 10];
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `email` = '%s' WHERE `uid` = '%d' LIMIT 1", pAccountEmail[playerid], pAccountID[playerid]);
            mysql_tquery(MySQL_GetHandle(), query, "Account_EmailCheckFinish", "ii", playerid, GetPlayerMysqlRace(playerid));
        }
    }
    return 1;
}

forward Account_EmailCheckFinish(playerid, raceCheck);
public Account_EmailCheckFinish(playerid, raceCheck)
{
    if(MysqlRaceCheck(playerid, raceCheck))
    {
        Account_Validation(playerid);
    }
    return 1;
}

/***
 *                                                                  
 *                         88                      88               
 *                         88                      ""               
 *                         88                                       
 *    ,adPPYYba,   ,adPPYb,88  88,dPYba,,adPYba,   88  8b,dPPYba,   
 *    ""     `Y8  a8"    `Y88  88P'   "88"    "8a  88  88P'   `"8a  
 *    ,adPPPPP88  8b       88  88      88      88  88  88       88  
 *    88,    ,88  "8a,   ,d88  88      88      88  88  88       88  
 *    `"8bbdP"Y8   `"8bbdP"Y8  88      88      88  88  88       88  
 *                                                                  
 *                                                                  
 */
#include <YSI_Coding\y_hooks>

CMD:apps(playerid, params[])
{
    if(!IsPlayerAdmin(playerid)) 
        return -1;

    DeletePVar(playerid, "ShenHeTargetID");
    mysql_tquery(MySQL_GetHandle(), "SELECT `name`, `email`, `uid` FROM `account` WHERE `verifycode` = '0' AND `active` = '0' AND `email` != '0' LIMIT 15", "Account_ShowVerifyList", "i", playerid);
	return 1;
}

forward Account_ShowVerifyList(playerid);
public Account_ShowVerifyList(playerid)
{
    if(cache_num_rows())
    {
        new targetID, targetName[MAX_PLAYER_NAME+1], targetEmail[32], string[128], buf[512];
        strcat(buf, "ID\t玩家昵称\tQQ号码\n");
        for(new i, j = cache_num_rows(); i < j; i++)
        {
            cache_get_value_int(i, "uid", targetID);
            cache_get_value(i, "name", targetName, MAX_PLAYER_NAME + 1);
            cache_get_value(i, "email", targetEmail, MAX_EMAIL_LENGTH);
            format(string, sizeof(string), "%i\t%s\t%s\n", targetID, targetName, targetEmail);
            strcat(buf, string);
        }
        ShowPlayerDialog(playerid, DIALOG_VERIFY_LIST, DIALOG_STYLE_TABLIST_HEADERS, "待审核玩家", buf, "选择", "关闭");
        return 1;
    }
    SendClientMessage(playerid, 0xAAC4E5FF, "[管理员审核系统] 当前没有玩家需要审核.");
    return 1;
}

forward Account_SendVerifyCode(playerid, databeseID, pass);
public Account_SendVerifyCode(playerid, databeseID, pass)
{
    if(cache_num_rows())
    {
        new tvar1, tvar2;
        cache_get_value_int(0, "verifycode", tvar1);
        cache_get_value_int(0, "active", tvar2);

        if(tvar2 == 0 && tvar2 == 0)
        {
            new email[MAX_EMAIL_LENGTH];
            cache_get_value(0, "email", email, MAX_EMAIL_LENGTH);

            if(pass == 1)
            {
                /*
                    在这里添加邮件发送功能, 并把下面 number 作为验证码发送至玩家邮箱, 本基础服务端不提供相关支持和帮助
                    自行在网上寻找邮箱服务器搭建的方法
                */
                new number = MRandRange(100000, 999999);
                new query[90 + MAX_PLAYER_NAME + 1];
                mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `verifycode` = '%d', `active` = '0' WHERE `uid` = '%d' LIMIT 1", number, databeseID);
                mysql_tquery(MySQL_GetHandle(), query);
                SendClientMessage(playerid, 0xAAC4E5FF, "您已成功通过该玩家的审核, 系统将自动以邮件的形式通知该玩家, 邮箱地址: %s@qq.com, 验证码: %d", email, number);
            }
            else
            {
                new query[90 + MAX_PLAYER_NAME + 1];
                mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT INTO `email_blacklist` (`email`) VALUES ('%s')", email);
                mysql_tquery(MySQL_GetHandle(), query);
                mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `verifycode` = '88', `active` = '0' WHERE `uid` = '%d' LIMIT 1", databeseID);
                mysql_tquery(MySQL_GetHandle(), query);
                SendClientMessage(playerid, 0xAAC4E5FF, "您已拒绝该玩家的审核, 系统已将该玩家的邮箱地址[%s@qq.com]列入服务器黑名单", email);
            }
        }
    }
    mysql_tquery(MySQL_GetHandle(), "SELECT `name`, `email`, `uid` FROM `account` WHERE `verifycode` = '0' AND `active` = '0' AND `email` != '0' LIMIT 15", "Account_ShowVerifyList", "i", playerid);
    return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if(dialogid == DIALOG_VERIFY_LIST)
    {
        if(response)
        {
            SetPVarInt(playerid, "ShenHeTargetID", strval(inputtext));
            ShowPlayerDialog(playerid, DIALOG_VERIFY_OPTION, DIALOG_STYLE_LIST, "审核结果", "通过审核\n{a93634}拒绝通过", "确定", "取消");
        }
        return Y_HOOKS_BREAK_RETURN_0;
    }
    else if(dialogid == DIALOG_VERIFY_OPTION)
    {
        if(response)
        {
            new query[128];
            new databeseID = GetPVarInt(playerid, "ShenHeTargetID");
            mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT `email`, `verifycode`, `active` FROM `account` WHERE `uid` = '%d' LIMIT 1", databeseID);
            mysql_tquery(MySQL_GetHandle(), query, "Account_SendVerifyCode", "iii", playerid, databeseID, ((listitem = 0)?(1):(0)));
            return Y_HOOKS_BREAK_RETURN_0;
        }
        mysql_tquery(MySQL_GetHandle(), "SELECT `name`, `email`, `uid` FROM `account` WHERE `verifycode` = '0' AND `active` = '0' AND `email` != '0' LIMIT 15", "Account_ShowVerifyList", "i", playerid);
        return Y_HOOKS_BREAK_RETURN_0;
    }
    return 1;
}

forward OnPlayerLoggedIn(playerid);
public OnPlayerLoggedIn(playerid)
{
    return 1;
}