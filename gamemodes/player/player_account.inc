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

static const LoginCameraData[][E_LOGIN_CAMERA_DATA] =       //���������ӵ�½ע�᾵ͷ������;�ͷ�����λ��
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
        SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: ����60����δ��¼��ϵͳ�߳��˷�����.");
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

    ShowPlayerDialog(playerid, DIALOG_SERVER_RULE, DIALOG_STYLE_MSGBOX, "{d2d2d2}����", 
        "{FFFFFF}1. �ǳƹ淶{AAAAAA}\n�ǳƲ����漰�������С�����������ʵ�\n�������ġ���������ơ�С������Ӣ��\n��ֹЯ����������\n��ֹ��ð�����ǳ�\n\n\
        {FFFFFF}2. ��������{AAAAAA}\n��ֹ����������ࡢ��ս������.\n��ֹ������桢Υ������\n��ֹ�����������λ��������\n\n\
        {FFFFFF}3. �Ͻ�����{AAAAAA}\n��ֹ��װΪ���ṩ����ƽ���Ƶĵ������޸ĺ�mod\n��ֹ����������ϷBUG���ĳЩ����", 
        "ͬ��", 
        "�ܾ�"
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
                SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: ���ѱ��߳�������, ԭ��: δ��¼�˺�.");
                return Kick(playerid);
            }
        #else
            if(pAccountLogged[playerid] == false || pAccountID[playerid] == INVALID_DATABESE_ID)
            {
                SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: ���ѱ��߳�������, ԭ��: δ��¼�˺�.");
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
                SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: ��ȷ����������û��ʹ���κν��õ��ַ�(��ʹ�� 0-9��a-z), �������볤���� %d - %d ���ַ���Χ.", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);

                pAccountLoginAttempt[playerid] += 1;
                if(pAccountLoginAttempt[playerid] >= MAX_PASSWORD_ATTEMPT)
                {
                    SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: �����¼ʧ�ܴ������౻ϵͳ�߳��˷�����.");
                    Kick(playerid);
                    return 1;
                }
                ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "��½", 
                    "{FFFFFF}����, ��ӭ����, �����·��������������¼��Ϸ\n\
                    ��ܰ��ʾ: ����60��֮�ڵ�¼�����˺�.\n\n\
                    {FF6347}ʣ�ೢ�Դ���: %d", 
                    "��¼", "�˳�", 
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
                SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: ��ȷ����������û��ʹ���κν��õ��ַ�(��ʹ�� 0-9��a-z), �������볤���� %d - %d ���ַ���Χ.", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);

                ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "ע��", 
                    "{FFFFFF}����, �����·��������봴��һ���˺�(ʹ�� %d - %d ���ַ�).\n\
                    ��ܰ��ʾ: ע��������Ʊ��ܺ���������.", 
                    "ע��", "�˳�",
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

            ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "��½", 
                "{FFFFFF}����, ��ӭ����, �����·��������������¼��Ϸ\n\
                ��ܰ��ʾ: ����60��֮�ڵ�¼�����˺�.", "��¼", "�˳�"
            );
        }
        else ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "ע��", 
            "{FFFFFF}����, �����·��������봴��һ���˺�(ʹ��4-16���ַ�).\n\
            ��ܰ��ʾ: ע��������Ʊ��ܺ���������.", "ע��", "�˳�"
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
                SendClientMessage(playerid, 0xE74C3CFF, "��ʾ: �����¼ʧ�ܴ������౻ϵͳ�߳��˷�����.");
                Kick(playerid);
                return 1;
            }
            ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "��½", 
                "{FFFFFF}����, ��ӭ����, �����·��������������¼��Ϸ\n\
                ��ܰ��ʾ: ����60���ڵ�¼�����˺�.\n\n\
                {FF6347}ʣ�ೢ�Դ���: %d", 
                "��¼", "�˳�", 
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
                    SendClientMessage(playerid, 0xFF6347FF, "��QQ����δͨ������Ա��֤[ԭ��: С��], �����������QQ����.");
                }
                ShowPlayerDialog(playerid, DIALOG_EMAIL_INPUT, DIALOG_STYLE_INPUT, "{FF6347}��QQ", "{FFFFFF}���û�����δ����, {FF6347}��ֹС�ŷ���, Ӱ����Ϸ����\n{FFFFFF}�����·�������Ҫ�󶨵�QQ�����Խ���{FF6347}��֤��", "����", "�˳�");
                return 1;
            }
            else
            {
                if(pAccountVerifyCode[playerid] == 0)
                {
                    ShowPlayerDialog(playerid, DIALOG_WAIT_VERIFYCODE, DIALOG_STYLE_INPUT, "{FF6347}��֤�����", "{FFFFFF}��������Ϊ: %s@qq.com, �����ĵȴ�{FF6347}��֤���\n{FFFFFF}��������ߵȴ���֤������{FF6347}changemail{FFFFFF}��������", "ˢ��", "���ߵȴ�", pAccountEmail[playerid]);
                    return 1;
                }
                else if(pAccountVerifyCode[playerid] == 88)
                {
                    if(Account_SetUnactivated(playerid))
                    {
                        SendClientMessage(playerid, 0xFF6347FF, "��QQ����δͨ������Ա��֤[ԭ��: С��], �����������QQ����.");
                        ShowPlayerDialog(playerid, DIALOG_EMAIL_INPUT, DIALOG_STYLE_INPUT, "{FF6347}��QQ", "{FFFFFF}���û�����δ����, {FF6347}��ֹС�ŷ���, Ӱ����Ϸ����\n{FFFFFF}�����·�������Ҫ�󶨵�QQ�����Խ���{FF6347}��֤��", "����", "�˳�");
                    }
                    return 1;
                }
                ShowPlayerDialog(playerid, DIALOG_VERIFY_CODE, DIALOG_STYLE_INPUT, "{FF6347}��֤��", "{FFFFFF}��֤���ѷ�������������: %s@qq.com\n{FF6347}��δ�յ�, �����ʼ��Ƿ�����������", "����", "�˳�", pAccountEmail[playerid]);
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
                SendClientMessage(playerid, 0xFF6347FF, "�������QQ��������, ��������ȷ��QQ����.");
                ShowPlayerDialog(playerid, DIALOG_EMAIL_INPUT, DIALOG_STYLE_INPUT, "{FF6347}��QQ", "{FFFFFF}���û�����δ����, {FF6347}��ֹС�ŷ���, Ӱ����Ϸ����\n{FFFFFF}�����·�������Ҫ�󶨵�QQ�����Խ���{FF6347}��֤��", "����", "�˳�");
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
                ShowPlayerDialog(playerid, DIALOG_WAIT_VERIFYCODE, DIALOG_STYLE_INPUT, "{FF6347}��֤�����", 
                    "{FFFFFF}��������Ϊ: %s@qq.com, �����ĵȴ�{FF6347}��֤���\n\
                    {FFFFFF}��������ߵȴ���֤������{FF6347}changemail{FFFFFF}��������\n\n\
                    {FF6347}��ȴ�%d�����ˢ��", 
                    "ˢ��", "���ߵȴ�",
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
                SendClientMessage(playerid, 0xFF6347FF, "���������֤������, �������������.");
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
        SendClientMessage(playerid, 0xFF6347FF, "���������֤������, �������������.");
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
            SendClientMessage(playerid, 0xFF6347FF, "��QQ���뱻ռ��, �����QQ����, ���ж���ռ��, ����ϵ����Ա����.");
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
            SendClientMessage(playerid, 0xFF6347FF, "��QQ���뱻������������С���嵥, �����������QQ����.");
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
        strcat(buf, "ID\t����ǳ�\tQQ����\n");
        for(new i, j = cache_num_rows(); i < j; i++)
        {
            cache_get_value_int(i, "uid", targetID);
            cache_get_value(i, "name", targetName, MAX_PLAYER_NAME + 1);
            cache_get_value(i, "email", targetEmail, MAX_EMAIL_LENGTH);
            format(string, sizeof(string), "%i\t%s\t%s\n", targetID, targetName, targetEmail);
            strcat(buf, string);
        }
        ShowPlayerDialog(playerid, DIALOG_VERIFY_LIST, DIALOG_STYLE_TABLIST_HEADERS, "��������", buf, "ѡ��", "�ر�");
        return 1;
    }
    SendClientMessage(playerid, 0xAAC4E5FF, "[����Ա���ϵͳ] ��ǰû�������Ҫ���.");
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
                    �����������ʼ����͹���, �������� number ��Ϊ��֤�뷢�����������, ����������˲��ṩ���֧�ֺͰ���
                    ����������Ѱ�������������ķ���
                */
                new number = MRandRange(100000, 999999);
                new query[90 + MAX_PLAYER_NAME + 1];
                mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `verifycode` = '%d', `active` = '0' WHERE `uid` = '%d' LIMIT 1", number, databeseID);
                mysql_tquery(MySQL_GetHandle(), query);
                SendClientMessage(playerid, 0xAAC4E5FF, "���ѳɹ�ͨ������ҵ����, ϵͳ���Զ����ʼ�����ʽ֪ͨ�����, �����ַ: %s@qq.com, ��֤��: %d", email, number);
            }
            else
            {
                new query[90 + MAX_PLAYER_NAME + 1];
                mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT INTO `email_blacklist` (`email`) VALUES ('%s')", email);
                mysql_tquery(MySQL_GetHandle(), query);
                mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE `account` SET `verifycode` = '88', `active` = '0' WHERE `uid` = '%d' LIMIT 1", databeseID);
                mysql_tquery(MySQL_GetHandle(), query);
                SendClientMessage(playerid, 0xAAC4E5FF, "���Ѿܾ�����ҵ����, ϵͳ�ѽ�����ҵ������ַ[%s@qq.com]���������������", email);
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
            ShowPlayerDialog(playerid, DIALOG_VERIFY_OPTION, DIALOG_STYLE_LIST, "��˽��", "ͨ�����\n{a93634}�ܾ�ͨ��", "ȷ��", "ȡ��");
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