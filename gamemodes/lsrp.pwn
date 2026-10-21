// LSSC 2027 - Los Santos Street Crime's - gamemode (sorgente ricostruito dalla v8)
// UN SOLO gamemode (anche da Qawno): CEF incluso. La chat vocale e' nel filterscript lsrp_voce
// (filterscripts/lsrp_voce.pwn): se SampVoice manca, il gamemode parte lo stesso.
// Solo per il server di prova: LSRP_NO_CEF=1.
#if !defined LSRP_NO_CEF && !defined LSRP_CEF
	#define LSRP_CEF
#endif
#include <open.mp>
// Posti giocatore: open.mp prevede 1000, il server ne usa al massimo LSRP_MAX_PLAYERS (config.json "max_players" non deve
// superarlo: all'avvio il gamemode lo controlla). Array e cicli per giocatore 3 volte piu' piccoli: meno memoria e
// callback piu' brevi (crashdetect "Long callback execution").
#if !defined LSRP_MAX_PLAYERS
	#define LSRP_MAX_PLAYERS (300)
#endif
#undef MAX_PLAYERS
#define MAX_PLAYERS (LSRP_MAX_PLAYERS)
#include <a_mysql>
#include <samp_bcrypt>
#include <streamer>
#if defined LSRP_CEF
	#include <cef>
#endif

#pragma dynamic 32768

#include "modules/core/enums.inc"
#include "modules/core/forwards.inc"
#include "modules/cef/cef_safe.inc"
#include "modules/core/chat.inc"
#include "modules/core/vehicle_names.inc"
#include "modules/core/lang.inc"
#include "modules/core/player_data.inc"
#include "modules/core/utils.inc"
#include "modules/economy/currency.inc"
#include "modules/economy/econ_config.inc"
#include "modules/core/logger.inc"
#include "modules/ui/cef_ui.inc"
#include "modules/core/validation.inc"
#include "modules/ui/dialogs.inc"
#include "modules/ui/hud.inc"
#include "modules/ui/vehicle_ui.inc"
#include "modules/ui/house_ui.inc"
#include "modules/ui/job_ui.inc"
#include "modules/ui/faction_ui.inc"
#include "modules/ui/inventory_ui.inc"
#include "modules/ui/bank_ui.inc"
#include "modules/ui/intro_ui.inc"
#include "modules/ui/living_ui.inc"
#include "modules/database/db_core.inc"
#include "modules/database/db_account.inc"
#include "modules/database/db_character.inc"
#include "modules/database/db_admin.inc"
#include "modules/database/db_vehicle.inc"
#include "modules/database/db_house.inc"
#include "modules/database/db_jobfac.inc"
#include "modules/database/db_inventory.inc"
#include "modules/database/db_bank.inc"
#include "modules/database/db_story.inc"
#include "modules/database/db_living.inc"
#include "modules/account/account.inc"
#include "modules/character/spawn.inc"
#include "modules/character/character.inc"
#include "modules/economy/money.inc"
#include "modules/economy/level.inc"
#include "modules/economy/paycheck.inc"
#include "modules/admin/admin.inc"
#include "modules/vehicle/vehicle.inc"
#include "modules/inventory/inventory.inc"
#include "modules/bank/bank.inc"
#include "modules/intro/intro.inc"
#include "modules/house/house.inc"
#include "modules/job/job.inc"
#include "modules/faction/faction.inc"
#include "modules/inventory/inv_doc.inc"
#include "modules/living/living_calc.inc"
#include "modules/living/living_api.inc"
#include "modules/district/district.inc"
#include "modules/director/director.inc"
#include "modules/rental/street_rental.inc"
#include "modules/rental/rental.inc"
#include "modules/taxi/taxi.inc"
#include "modules/freight/freight.inc"
#include "modules/cityos/cityos.inc"
#include "modules/hub/hub.inc"
#include "modules/cef/cef_bridge.inc"
#include "modules/ui/hud_cef.inc"
#include "modules/voice/voice.inc"
#include "modules/world/az_map.inc"
#include "modules/world/az_data.inc"
#include "modules/world/az_world.inc"
#include "modules/world/gps.inc"
#include "modules/ui/cef_views.inc"
#include "modules/character/spawn_select.inc"
#include "modules/character/char_select.inc"
#include "modules/core/boot.inc"
#include "modules/economy/econ_admin.inc"
#include "modules/commands/admin_commands.inc"
#include "modules/commands/rp_commands.inc"


main()
{
	print("LS Crime's RP - Los Santos Street Crime's 2027");
}

public OnGameModeInit()
{
	SetGameModeText("LS Crime's RP");
	// (niente reset dei 300 posti qui: gPlayer parte gia' a zero e ogni giocatore viene azzerato in OnPlayerConnect)
	if (GetMaxPlayers() > MAX_PLAYERS)
	{
		printf("[AVVIO] ERRORE: config.json \"max_players\" = %d, ma il gamemode gestisce al massimo %d giocatori (LSRP_MAX_PLAYERS). Abbassa max_players.", GetMaxPlayers(), MAX_PLAYERS);
		SendRconCommand("exit");
		return 1;
	}
	// database, moduli e mondo: a tappe (core/boot.inc), una per tick, per non bloccare il server
	Boot_Start();
	return 1;
}

public OnGameModeExit()
{
	Economy_OnExit();
	Inv_OnExit();
	Bank_OnExit();
	Intro_OnExit();
	Job_OnExit();
	HudCef_OnExit();
	StreetRental_OnExit();
	AzWorld_OnExit();
	Voice_OnExit();
	Living_OnExit();
	Faction_OnExit();
	Veh_OnExit();
	House_OnExit();
	Char_OnExit();
	DB_Shutdown();
	return 1;
}

public OnPlayerConnect(playerid)
{
	Player_Reset(playerid);
	CefBridge_OnPlayerConnect(playerid);
	Bank_OnPlayerConnect(playerid);
	Intro_OnPlayerConnect(playerid);
	Veh_OnPlayerConnect(playerid);
	House_OnPlayerConnect(playerid);
	Job_OnPlayerConnect(playerid);
	Faction_OnPlayerConnect(playerid);
	Living_OnPlayerConnect(playerid);
	HudCef_OnPlayerConnect(playerid);
	StreetRental_OnPlayerConnect(playerid);
	SpawnSel_OnPlayerConnect(playerid);
	CharSel_OnPlayerConnect(playerid);
	CefViews_OnPlayerConnect(playerid);
	AzWorld_OnPlayerConnect(playerid);
	if (!Boot_DeferLogin(playerid))
	{
		Account_OnConnect(playerid);
	}
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	SpawnSel_OnPlayerDisconnect(playerid);   // prima di Faction (manette)
	CharSel_OnPlayerDisconnect(playerid);
	Bank_OnPlayerDisconnect(playerid);
	Inv_OnPlayerDisconnect(playerid);
	Job_OnPlayerDisconnect(playerid);
	Living_OnPlayerDisconnect(playerid, reason);
	Voice_OnPlayerDisconnect(playerid);
	HudCef_OnPlayerDisconnect(playerid);
	StreetRental_OnPlayerDisconnect(playerid);
	Gps_OnPlayerDisconnect(playerid);
	Boot_OnPlayerDisconnect(playerid);
	Veh_OnPlayerDisconnect(playerid);
	UI_VehicleUiDestroy(playerid);
	House_OnPlayerDisconnect(playerid);
	UI_HouseUiDestroy(playerid);
	UI_JobUiDestroy(playerid);
	UI_FactionUiDestroy(playerid);
	UI_InvUiDestroy(playerid);
	UI_BankUiDestroy(playerid);
	UI_LivingUiDestroy(playerid);
	Intro_OnPlayerDisconnect(playerid);
	Faction_OnPlayerDisconnect(playerid);
	Char_OnDisconnect(playerid);
	Account_OnDisconnect(playerid);
	CefBridge_OnPlayerDisconnect(playerid);
	Player_Reset(playerid);
	return 1;
}

#if defined LSRP_CEF
public OnCefInitialize(player_id, success)
{
	CefBridge_OnInitialize(player_id, success);
	return 1;
}

public OnCefBrowserCreated(player_id, browser_id, status_code)
{
	if (status_code != 200)
	{
		new msg[96];
		format(msg, 96, "browser %d del giocatore %d non creato (codice %d)", browser_id, player_id, status_code);
		Log_Write("CEF", msg);
	}
	return 1;
}

#endif
public OnPlayerRequestClass(playerid, classid)
{
	Char_OnRequestClass(playerid);
	return 1;
}

public OnPlayerRequestSpawn(playerid)
{
	return Char_OnRequestSpawn(playerid);
}

public OnPlayerSpawn(playerid)
{
	Char_OnSpawn(playerid);
	Veh_OnPlayerSpawn(playerid);
	House_OnPlayerSpawn(playerid);
	Faction_OnPlayerSpawn(playerid);
	Voice_OnPlayerSpawn(playerid);
	Inv_OnPlayerSpawn(playerid);
	Living_OnPlayerSpawn(playerid);
	return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, WEAPON:weaponid, bodypart)
{
	SpawnSel_OnDamage(playerid, issuerid);
	return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
	Job_OnPlayerDeath(playerid);
	Living_OnPlayerDeath(playerid);
	Faction_OnPlayerDeath(playerid);
	Char_OnDeath(playerid);
	return 1;
}

public OnPlayerUpdate(playerid)
{
	return Char_OnUpdate(playerid);
}

public OnPlayerText(playerid, text[])
{
	if (!Char_OnText(playerid) || !Admin_OnText(playerid))
	{
		return 0;
	}
	// stessa chat di SA-MP ("Nome: testo" a tutti) ma attraverso l'adapter: chi ha la chat CEF la riceve li'
	Chat_Broadcast(playerid, text);
	return 0;
}

public OnRconCommand(cmd[])
{
	return Admin_OnRconCommand(cmd);
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return Cmd_Dispatch(playerid, cmdtext);
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (CharSel_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (SpawnSel_OnDialogResponse(playerid, dialogid, listitem))
	{
		return 1;
	}
	if (StreetRental_OnDialogResponse(playerid, dialogid, response))
	{
		return 1;
	}
	if (Gps_OnDialogResponse(playerid, dialogid, response, listitem))
	{
		return 1;
	}
	if (UI_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_VehicleOnDialogResponse(playerid, dialogid, response, listitem))
	{
		return 1;
	}
	if (UI_HouseOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_JobOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_FactionOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_InvOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_BankOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_IntroOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	if (UI_LivingOnDialogResponse(playerid, dialogid, response, listitem, inputtext))
	{
		return 1;
	}
	return 0;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	Veh_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	Job_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	Living_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	Veh_OnPlayerExitVehicle(playerid, vehicleid);
	Job_OnPlayerExitVehicle(playerid, vehicleid);
	Living_OnPlayerExitVehicle(playerid, vehicleid);
	return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	Veh_OnPlayerStateChange(playerid, newstate, oldstate);
	Job_OnPlayerStateChange(playerid, newstate);
	Living_OnPlayerStateChange(playerid, newstate, oldstate);
	StreetRental_OnStateChange(playerid, newstate);
	AzWorld_OnStateChange(playerid, newstate);
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
	Veh_OnVehicleDeath(vehicleid, killerid);
	Job_OnVehicleDeath(vehicleid);
	Living_OnVehicleDeath(vehicleid, killerid);
	StreetRental_OnVehicleDeath(vehicleid);
	return 1;
}

public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
	Intro_OnKeyStateChange(playerid, newkeys);
	HudCef_OnKeyStateChange(playerid, newkeys, oldkeys);
	if ((newkeys & KEY_NO) && !(oldkeys & KEY_NO) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER && Util_IsPlaying(playerid))
	{
		Veh_DoEngine(playerid);   // N al volante: motore acceso/spento
	}
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	Gps_OnEnterRaceCheckpoint(playerid);
	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	if (AzWorld_OnPickUp(playerid, pickupid))
	{
		return 1;
	}
	House_OnPlayerPickUpPickup(playerid, pickupid);
	return 1;
}
