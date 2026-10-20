// =============================================================================
// world/az_world.inc - mondo Arizona (solo Los Santos) dentro LSSC.
//
// Dati generati: world/az_map.inc (mapping) e world/az_data.inc (porte, business, veicoli).
//  - porte degli edifici pubblici: si entra/esce passando sulla freccia (pickup 1318)
//  - business: ingresso (pickup "i") e uscita dentro l'interno; per ora solo visitabili (proprieta' da decidere)
//  - veicoli: fazione (LSPD -> LSPD, ambulanze -> EMS, le altre fazioni non esistono ancora: chiusi),
//    lavoro (nettezza urbana -> lavoro 3, pizza -> corriere 2, gli altri lavori non esistono ancora: chiusi),
//    motorini liberi, auto parcheggiate chiuse.
// Le case sono nel DB (sql/migrations/015_arizona_houses.sql) e usano il sistema case di LSSC.
// =============================================================================

#define AZ_TP_COOLDOWN_MS  (3000)
#define AZ_BIZ_WORLD_BASE  (5000)
#define AZ_TEAM_LSPD       (1)
#define AZ_TEAM_MEDICS     (5)
#define AZ_JOB_PIZZA       (12)
#define AZ_JOB_MUSOR       (14)

new g_AzDoorPick[AZ_DOORS];
new g_AzBizPickIn[AZ_BIZ];
new g_AzBizPickOut[AZ_BIZ];
new g_AzVeh[AZ_VEH];
new g_AzVehOf[2001];                 // vehicleid -> indice + 1 (0 = non e' un veicolo Arizona)
new g_AzTpTick[MAX_PLAYERS];

AzWorld_OnInit()
{
	AzMap_Create();
	new label[160];
	for (new i = 0; i < AZ_DOORS; i++)
	{
		g_AzDoorPick[i] = CreatePickup(1318, 1, s_AzDoorPos[i][0], s_AzDoorPos[i][1], s_AzDoorPos[i][2], s_AzDoorWI[i][0]);
		format(label, sizeof label, "{FFFFFF}%s", s_AzDoorName[i]);
		Lang_Create3DTextLabel(label, 0xFFFFFFFF, s_AzDoorPos[i][0], s_AzDoorPos[i][1], s_AzDoorPos[i][2] + 0.6, 12.0, s_AzDoorWI[i][0], false);
	}
	for (new i = 0; i < AZ_BIZ; i++)
	{
		g_AzBizPickIn[i] = CreatePickup(1239, 1, s_AzBizPos[i][0], s_AzBizPos[i][1], s_AzBizPos[i][2], 0);
		g_AzBizPickOut[i] = CreatePickup(1318, 1, s_AzBizPos[i][3], s_AzBizPos[i][4], s_AzBizPos[i][5], AZ_BIZ_WORLD_BASE + i);
		format(label, sizeof label, "{FFB547}%s", s_AzBizName[i]);
		Lang_Create3DTextLabel(label, 0xFFFFFFFF, s_AzBizPos[i][0], s_AzBizPos[i][1], s_AzBizPos[i][2] + 0.7, 15.0, 0, false);
	}
	for (new i = 0; i < AZ_VEH; i++)
	{
		new veh = CreateVehicle(s_AzVehInfo[i][0], s_AzVehPos[i][0], s_AzVehPos[i][1], s_AzVehPos[i][2], s_AzVehPos[i][3],
			s_AzVehInfo[i][1], s_AzVehInfo[i][2], 300, false);
		g_AzVeh[i] = veh;
		if (veh > 0 && veh <= 2000)
		{
			g_AzVehOf[veh] = i + 1;
			// parcheggiate: porte chiuse; le altre: motore spento finche' non sale chi puo' guidarle
			SetVehicleParamsEx(veh, false, false, false, s_AzVehInfo[i][3] == 4, false, false, false);
		}
	}
	printf("[MONDO] Arizona Los Santos: %d porte, %d business, %d veicoli", AZ_DOORS, AZ_BIZ, AZ_VEH);
}

AzWorld_OnExit()
{
	for (new i = 0; i < AZ_VEH; i++)
	{
		if (g_AzVeh[i] > 0)
		{
			DestroyVehicle(g_AzVeh[i]);
			g_AzVeh[i] = 0;
		}
	}
}

AzWorld_OnPlayerConnect(playerid)
{
	g_AzTpTick[playerid] = 0;
	AzMap_RemoveBuildings(playerid);
}

static AzWorld_Teleport(playerid, Float:x, Float:y, Float:z, Float:a, interior, world)
{
	g_AzTpTick[playerid] = GetTickCount();
	SetPlayerInterior(playerid, interior);
	SetPlayerVirtualWorld(playerid, world);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerFacingAngle(playerid, a);
	SetCameraBehindPlayer(playerid);
}

// passaggio sulla freccia/icona: entra o esce (pausa di 3 s per non rimbalzare tra le due porte)
bool:AzWorld_OnPickUp(playerid, pickupid)
{
	if (!Util_IsPlaying(playerid) || GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
	{
		return false;
	}
	if (g_AzTpTick[playerid] != 0 && GetTickCount() - g_AzTpTick[playerid] < AZ_TP_COOLDOWN_MS)
	{
		return false;
	}
	for (new i = 0; i < AZ_DOORS; i++)
	{
		if (g_AzDoorPick[i] == pickupid)
		{
			AzWorld_Teleport(playerid, s_AzDoorPos[i][3], s_AzDoorPos[i][4], s_AzDoorPos[i][5], s_AzDoorPos[i][6], s_AzDoorWI[i][2], s_AzDoorWI[i][3]);
			return true;
		}
	}
	for (new i = 0; i < AZ_BIZ; i++)
	{
		if (g_AzBizPickIn[i] == pickupid)
		{
			AzWorld_Teleport(playerid, s_AzBizPos[i][3], s_AzBizPos[i][4], s_AzBizPos[i][5], 0.0, s_AzBizInt[i], AZ_BIZ_WORLD_BASE + i);
			SendClientMessage(playerid, 0xFFB547FF, "%s: გამოსასვლელად ისევ დადექი ისარზე.", s_AzBizName[i]);
			return true;
		}
		if (g_AzBizPickOut[i] == pickupid)
		{
			AzWorld_Teleport(playerid, s_AzBizPos[i][0], s_AzBizPos[i][1] - 1.5, s_AzBizPos[i][2], 180.0, 0, 0);
			return true;
		}
	}
	return false;
}

// -1 = non e' un veicolo Arizona; 0 = non puo' guidarlo; 1 = puo'
AzWorld_CanDrive(playerid, vehicleid)
{
	if (vehicleid <= 0 || vehicleid > 2000 || g_AzVehOf[vehicleid] == 0)
	{
		return -1;
	}
	if (gPlayer[playerid][pAdminLevel] > 0)
	{
		return 1;
	}
	new i = g_AzVehOf[vehicleid] - 1, grp = s_AzVehInfo[i][4];
	switch (s_AzVehInfo[i][3])
	{
		case 1:
		{
			if (grp == AZ_TEAM_LSPD)
			{
				return gPlayer[playerid][pFaction] == 1 && gPlayer[playerid][pOnDuty] ? 1 : 0;
			}
			if (grp == AZ_TEAM_MEDICS)
			{
				return gPlayer[playerid][pFaction] == 2 && gPlayer[playerid][pOnDuty] ? 1 : 0;
			}
			return 0;
		}
		case 2:
		{
			if (grp == AZ_JOB_MUSOR)
			{
				return gPlayer[playerid][pJob] == 3 ? 1 : 0;
			}
			if (grp == AZ_JOB_PIZZA)
			{
				return gPlayer[playerid][pJob] == 2 ? 1 : 0;
			}
			return 0;
		}
		case 3:
		{
			return 1;
		}
	}
	return 0;
}

// al volante di un veicolo Arizona: chi non puo' guidarlo scende, chi puo' trova il motore acceso
AzWorld_OnStateChange(playerid, PLAYER_STATE:newstate)
{
	if (newstate != PLAYER_STATE_DRIVER)
	{
		return 0;
	}
	new veh = GetPlayerVehicleID(playerid), ok = AzWorld_CanDrive(playerid, veh);
	if (ok == -1)
	{
		return 0;
	}
	if (ok == 0)
	{
		new i = g_AzVehOf[veh] - 1;
		switch (s_AzVehInfo[i][3])
		{
			case 1: Util_Send(playerid, 0xFF6347FF, "ეს ფრაქციის მანქანაა: მხოლოდ სამსახურში მყოფ წევრებს შეუძლიათ მისი მართვა.");
			case 2: Util_Send(playerid, 0xFF6347FF, "ეს სამსახურეობრივი მანქანაა: საჭიროა შესაბამისი სამსახური.");
			default: Util_Send(playerid, 0xFF6347FF, "ეს მანქანა დაკეტილია.");
		}
		RemovePlayerFromVehicle(playerid);
		return 1;
	}
	new e, l, a, d, b, bt, o;
	GetVehicleParamsEx(veh, e, l, a, d, b, bt, o);
	#pragma unused e
	SetVehicleParamsEx(veh, true, bool:l, bool:a, bool:d, bool:b, bool:bt, bool:o);
	return 1;
}
