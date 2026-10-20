// =============================================================================
// LSRP — Los Santos Roleplay · entry point
// Solo #include dei moduli e callback open.mp che delegano ai moduli (CLAUDE.md §2).
// Nessuna logica di gioco in questo file.
// =============================================================================
#include <open.mp>
#include <a_mysql>
#include <streamer>
#include <samp_bcrypt>

// Ordine: core -> ui -> database -> account -> character -> economy -> admin -> vehicle -> house -> job -> faction -> command
#include "modules/core/config.inc"
#include "modules/core/player_data.inc"
#include "modules/core/utils.inc"
#include "modules/core/logger.inc"

#include "modules/ui/dialogs.inc"
#include "modules/ui/hud.inc"
#include "modules/ui/vehicle_ui.inc"
#include "modules/ui/house_ui.inc"
#include "modules/ui/job_ui.inc"
#include "modules/ui/faction_ui.inc"

#include "modules/database/db_core.inc"
#include "modules/database/db_account.inc"
#include "modules/database/db_character.inc"
#include "modules/database/db_admin.inc"
#include "modules/database/db_vehicle.inc"
#include "modules/database/db_house.inc"
#include "modules/database/db_jobfac.inc"

#include "modules/account/account.inc"
#include "modules/character/character.inc"

#include "modules/economy/money.inc"
#include "modules/economy/level.inc"
#include "modules/economy/paycheck.inc"

#include "modules/admin/admin.inc"

#include "modules/vehicle/vehicle.inc"

#include "modules/house/house.inc"

#include "modules/job/job.inc"

#include "modules/faction/faction.inc"

#include "modules/commands/rp_commands.inc"

main()
{
	printf("LSRP %s", LSRP_VERSION);
}

public OnGameModeInit()
{
	SetGameModeText("LSRP " LSRP_VERSION);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Player_Reset(i);
	}
	DB_Connect();
	Char_OnInit();
	Economy_OnInit();
	Veh_OnInit();
	House_OnInit();         // carica case (DB_BootHouses), pickup, timer utenze/fame
	Job_OnInit();           // Job Center, timer dei turni
	Faction_OnInit();       // timer di servizio, stipendio, ferito
	return 1;
}

public OnGameModeExit()
{
	Economy_OnExit();
	Job_OnExit();           // distrugge i veicoli di lavoro
	Faction_OnExit();
	Veh_OnExit();           // salva i veicoli prima di chiudere il DB
	House_OnExit();         // salva utenze e fame prima di chiudere il DB
	Char_OnExit();          // salva tutti i personaggi prima di chiudere il DB
	DB_Shutdown();
	return 1;
}

public OnPlayerConnect(playerid)
{
	Player_Reset(playerid);
	Veh_OnPlayerConnect(playerid);
	House_OnPlayerConnect(playerid);
	Job_OnPlayerConnect(playerid);
	Faction_OnPlayerConnect(playerid);
	Account_OnConnect(playerid);
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	Job_OnPlayerDisconnect(playerid);    // annulla il turno (nessun pagamento) e distrugge il veicolo di lavoro
	Veh_OnPlayerDisconnect(playerid);    // salva e rimuove i veicoli del giocatore
	UI_VehicleUiDestroy(playerid);
	House_OnPlayerDisconnect(playerid);  // salva utenze/fame (serve ancora pCharId)
	UI_HouseUiDestroy(playerid);
	UI_JobUiDestroy(playerid);
	UI_FactionUiDestroy(playerid);
	Faction_OnPlayerDisconnect(playerid); // servizio/manette/ferito: prima del salvataggio (skin civile, posizione di ricovero)
	Char_OnDisconnect(playerid);     // salva prima del reset
	Account_OnDisconnect(playerid);
	Player_Reset(playerid);
	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
	#pragma unused classid
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
	Veh_OnPlayerSpawn(playerid);     // carica i veicoli dopo lo spawn del personaggio
	House_OnPlayerSpawn(playerid);   // ripristina l'istanza casa e la fame
	Faction_OnPlayerSpawn(playerid); // stato ferito / ricovero dopo la sadasdasdasd
	return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
	#pragma unused killerid, reason
	Job_OnPlayerDeath(playerid);         // turno di lavoro perso
	Faction_OnPlayerDeath(playerid);     // stato ferito, fuori servizio, manette: PRIMA di Char_OnDeath (serve pSpawned)
	Char_OnDeath(playerid);
	return 1;
}

public OnPlayerUpdate(playerid)
{
	return Char_OnUpdate(playerid);
}

public OnPlayerText(playerid, text[])
{
	#pragma unused text
	if (!Char_OnText(playerid)) return 0;     // chat bloccata prima dello spawn
	return Admin_OnText(playerid);            // 0 = giocatore mutato
}

public OnRconCommand(cmd[])
{
	return Admin_OnRconCommand(cmd);          // bootstrap primo admin: "setadmin nome_account livello"
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return Cmd_Dispatch(playerid, cmdtext);
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (UI_OnDialogResponse(playerid, dialogid, response, listitem, inputtext)) return 1;
	if (UI_VehicleOnDialogResponse(playerid, dialogid, response, listitem)) return 1;
	if (UI_HouseOnDialogResponse(playerid, dialogid, response, listitem, inputtext)) return 1;
	if (UI_JobOnDialogResponse(playerid, dialogid, response, listitem, inputtext)) return 1;
	return UI_FactionOnDialogResponse(playerid, dialogid, response, listitem, inputtext) ? 1 : 0;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	Veh_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	Job_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	Veh_OnPlayerExitVehicle(playerid, vehicleid);
	Job_OnPlayerExitVehicle(playerid, vehicleid);
	return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	Veh_OnPlayerStateChange(playerid, newstate, oldstate);
	Job_OnPlayerStateChange(playerid, newstate);
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
	Veh_OnVehicleDeath(vehicleid, killerid);
	Job_OnVehicleDeath(vehicleid);
	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	House_OnPlayerPickUpPickup(playerid, pickupid);
	return 1;
}
