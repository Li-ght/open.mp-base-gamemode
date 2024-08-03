#include <open.mp>

#undef MAX_PLAYERS
#define MAX_PLAYERS 200

#define YSI_NO_CACHE_MESSAGE
#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_NO_HEAP_MALLOC
#define YSI_NO_VERSION_CHECK
#define YSI_NO_MASTER // 如果你没有加载任何脚本(filterscripts) 请保留这行

#include <a_mysql>
#include <gvar>
#include <merrandom>
#include <samp_bcrypt>
#include <streamer>
#include <sscanf2>
#include <PawnPlus>
#include <Pawn.CMD>
#include <YSI_Data\y_iterate>

#include "utils/utils_str"
#include "utils/utils_fixes"
#include "utils/utils_helper"
#include "utils/utils_mysql"

#include "player/player_account"
#include "player/player_stats"

#define SERVER_NAME         "open.mp server"
#define SERVER_URL          "www.open.mp"
#define SERVER_VESION       "版本: 1.0.0.0"
#define SERVER_MAP          "San Andreas"
#define SERVER_LANGUAGE     "中文"
#define SERVER_RCON         "changeit"

main()
{
    SendRconCommand("name "SERVER_NAME"");
    SendRconCommand("website "SERVER_URL"");
    SendRconCommand("game.mode "SERVER_VESION"");
    SendRconCommand("game.map "SERVER_MAP"");
    SendRconCommand("language "SERVER_LANGUAGE"");
    SendRconCommand("rcon.password "SERVER_RCON"");
}

public OnGameModeInit()
{
    // 这可以让服务器支持中文昵称
    // 后续请等待open.mp内置关于非英文昵称的支持
    for(new i = 0; i <= 255; i++) 
    {
        if(!IsNickNameCharacterAllowed(i))
            AllowNickNameCharacter(i, true);
    }
    for (new i = 1; i <= 311; i++) {
        if (i == 74) continue;
        AddPlayerClass(i, 2495.3547, -1688.2319, 13.6774, 351.1646, WEAPON_M4, 500, WEAPON_KNIFE, 1, WEAPON_COLT45, 100);
    }
    AddStaticVehicle(522, 2493.7583, -1683.6482, 12.9099, 270.8069, -1, -1);
    return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerLoggedIn(playerid)
{
    SetPlayerPos(playerid, 217.8511, -98.4865, 1005.2578);
    SetPlayerFacingAngle(playerid, 113.8861);
    SetPlayerInterior(playerid, 15);
    SetPlayerCameraPos(playerid, 215.2182, -99.5546, 1006.4);
    SetPlayerCameraLookAt(playerid, 217.8511, -98.4865, 1005.2578);
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    if(result == -1)
    {
        SendClientMessage(playerid, 0xFFFFFFFF, "服务器: 无效指令.");
        return 0;
    }
    return 1;
}

public OnGameModeExit()
{
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    ApplyAnimation(playerid, "benchpress", "gym_bp_celebrate", 4.1, true, false, false, false, 0, SYNC_NONE);
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    return 1;
}

public OnPlayerSpawn(playerid)
{
    SetPlayerPos(playerid, 2495.3547, -1688.2319, 13.6774);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);
    return 1;
}

public OnPlayerConnect(playerid)
{
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    return 1;
}

public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
    return 1;
}

public OnPlayerText(playerid, text[])
{
    return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    return 1;
}

public OnPlayerWeaponShot(playerid, WEAPON:weaponid, BULLET_HIT_TYPE:hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
    return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, WEAPON:weaponid, bodypart)
{
    return 1;
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, WEAPON:weaponid, bodypart)
{
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
    return 1;
}

public OnPlayerLeaveCheckpoint(playerid)
{
    return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
    return 1;
}

public OnPlayerLeaveRaceCheckpoint(playerid)
{
    return 1;
}

public OnPlayerGiveDamageActor(playerid, damaged_actorid, Float:amount, WEAPON:weaponid, bodypart)
{
    return 1;
}

public OnActorStreamIn(actorid, forplayerid)
{
    return 1;
}

public OnActorStreamOut(actorid, forplayerid)
{
    return 1;
}

public OnPlayerEnterGangZone(playerid, zoneid)
{
    return 1;
}

public OnPlayerLeaveGangZone(playerid, zoneid)
{
    return 1;
}

public OnPlayerEnterPlayerGangZone(playerid, zoneid)
{
    return 1;
}

public OnPlayerLeavePlayerGangZone(playerid, zoneid)
{
    return 1;
}

public OnPlayerClickGangZone(playerid, zoneid)
{
    return 1;
}

public OnPlayerClickPlayerGangZone(playerid, zoneid)
{
    return 1;
}

public OnPlayerSelectedMenuRow(playerid, row)
{
    return 1;
}

public OnPlayerExitedMenu(playerid)
{
    return 1;
}

public OnClientCheckResponse(playerid, actionid, memaddr, retndata)
{
    return 1;
}

public OnRconLoginAttempt(ip[], password[], success)
{
    return 1;
}

public OnPlayerFinishedDownloading(playerid, virtualworld)
{
    return 1;
}

public OnPlayerRequestDownload(playerid, DOWNLOAD_REQUEST:type, crc)
{
    return 1;
}

public OnRconCommand(cmd[])
{
    return 0;
}

public OnPlayerSelectObject(playerid, SELECT_OBJECT:type, objectid, modelid, Float:fX, Float:fY, Float:fZ)
{
    return 1;
}

public OnPlayerEditObject(playerid, playerobject, objectid, EDIT_RESPONSE:response, Float:fX, Float:fY, Float:fZ, Float:fRotX, Float:fRotY, Float:fRotZ)
{
    return 1;
}

public OnPlayerEditAttachedObject(playerid, EDIT_RESPONSE:response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ)
{
    return 1;
}

public OnObjectMoved(objectid)
{
    return 1;
}

public OnPlayerObjectMoved(playerid, objectid)
{
    return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
    return 1;
}

public OnPlayerPickUpPlayerPickup(playerid, pickupid)
{
    return 1;
}

public OnPickupStreamIn(pickupid, playerid)
{
    return 1;
}

public OnPickupStreamOut(pickupid, playerid)
{
    return 1;
}

public OnPlayerPickupStreamIn(pickupid, playerid)
{
    return 1;
}

public OnPlayerPickupStreamOut(pickupid, playerid)
{
    return 1;
}

public OnPlayerStreamIn(playerid, forplayerid)
{
    return 1;
}

public OnPlayerStreamOut(playerid, forplayerid)
{
    return 1;
}

public OnPlayerClickPlayer(playerid, clickedplayerid, CLICK_SOURCE:source)
{
    return 1;
}

public OnScriptCash(playerid, amount, source)
{
    return 1;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    return 1;
}

public OnIncomingConnection(playerid, ip_address[], port)
{
    return 1;
}

public OnPlayerUpdate(playerid)
{
    return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
    return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
    return 1;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    return 1;
}

public GetPlayerLastSyncedVehicleID(playerid)
{
    return 1;
}

public GetPlayerLastSyncedTrailerID(playerid)
{
    return 1;
}

public OnTrailerUpdate(playerid, vehicleid)
{
    return 1;
}

public OnVehicleSirenStateChange(playerid, vehicleid, newstate)
{
    return 1;
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
    return 1;
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
    return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
    return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
    return 1;
}

public OnVehicleSpawn(vehicleid)
{
    return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
    return 1;
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
    return 1;
}

public OnEnterExitModShop(playerid, enterexit, interiorid)
{
    return 1;
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
    return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
    return 1;
}

public OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
    return 1;
}

/*
//该回调每秒对每辆空闲车辆的调用频率非常高。应避免在此回调中执行密集计算或密集文件写入/读取操作
public OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z)
{
    return 1;
}
*/