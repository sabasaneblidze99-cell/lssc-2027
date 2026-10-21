new Float:g_VehStations[3][3] = {
	{1941.6, -1778.4, 13.6},
	{1004.0, -936.5, 42.3},
	{2114.0, -1893.0, 13.6}
};
new g_VehCatModel[13] = {462, 436, 410, 491, 466, 547, 529, 543, 461, 422, 549, 500, 405};
new g_VehCatPrice[13] = {ECON_VEH_FAGGIO, ECON_VEH_PREVION, ECON_VEH_MANANA, ECON_VEH_VIRGO, ECON_VEH_GLENDALE, ECON_VEH_PRIMO, ECON_VEH_WILLARD, ECON_VEH_SADLER, ECON_VEH_PCJ600, ECON_VEH_BOBCAT, ECON_VEH_TAMPA, ECON_VEH_MESA, ECON_VEH_SENTINEL};   // economy/econ_config.inc
new g_VehCatLevel[13] = {1, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5};
new g_VehCatName[13][12] = {
	"Faggio",
	"Previon",
	"Manana",
	"Virgo",
	"Glendale",
	"Primo",
	"Willard",
	"Sadler",
	"PCJ-600",
	"Bobcat",
	"Tampa",
	"Mesa",
	"Sentinel"
};
new g_VSlot[MAX_PLAYERS][4][E_VSLOT];
new g_VPl[MAX_PLAYERS][E_VPLAYER];
new g_VMapOwner[2001];
new g_VMapSlot[2001];
new g_VMapSession[2001];
new g_VPool;
new g_VSessionSeq;
new g_VFuelTimer;
new g_VHudTimer;
new g_VSaveTimer;

bool:Veh_PlayerReady(playerid)
{
	return Util_IsPlaying(playerid) && g_VPl[playerid][vpSession] != 0;
}

Veh_CatalogIndex(model)
{
	for (new i = 0; i < 13; i++)
	{
		if (g_VehCatModel[i] == model)
		{
			return i;
		}
	}
	return -1;
}

Veh_ModelName(model, out[], size)
{
	new idx = Veh_CatalogIndex(model);
	if (idx >= 0)
	{
		strmid(out, g_VehCatName[idx], 0, strlen(g_VehCatName[idx]), size);
	}
	else
	{
		strmid(out, "მანქანა", 0, 7, size);
	}
}

bool:Veh_IsBike(model)
{
	return model == 462 || model == 461;
}

Veh_CapacityCl(model)
{
	return (Veh_IsBike(model) ? 14 : 50) * 100;
}

Veh_ListPrice(model, paid)
{
	new idx = Veh_CatalogIndex(model);
	return idx >= 0 ? g_VehCatPrice[idx] : paid;
}

Veh_MaxForLevel(level)
{
	if (level >= 20)
	{
		return 4;
	}
	if (level >= 10)
	{
		return 3;
	}
	if (level >= 5)
	{
		return 2;
	}
	return 1;
}

Veh_UsedCount(playerid)
{
	new n = 0;
	for (new s = 0; s < 4; s++)
	{
		if (g_VSlot[playerid][s][vsUsed])
		{
			n++;
		}
	}
	return n;
}

Veh_FreeSlot(playerid)
{
	for (new s = 0; s < 4; s++)
	{
		if (!g_VSlot[playerid][s][vsUsed])
		{
			return s;
		}
	}
	return -1;
}

Veh_SpeedKmh(vehicleid)
{
	new Float:vx, Float:vy, Float:vz;
	GetVehicleVelocity(vehicleid, vx, vy, vz);
	return floatround((180.0 * floatsqroot((((vx * vx) + (vy * vy)) + (vz * vz)))), floatround_method:0);
}

Veh_FuelPct(playerid, slot)
{
	new cap = Veh_CapacityCl(g_VSlot[playerid][slot][vsModel]);
	if (cap <= 0)
	{
		return 0;
	}
	return clamp(g_VSlot[playerid][slot][vsFuelCl] * 100 / cap, 0, 100);
}

bool:Veh_NearDealer(playerid)
{
	if (GetPlayerInterior(playerid) != 0)
	{
		return false;
	}
	return (GetPlayerDistanceFromPoint(playerid, 1655.0, -1665.0, 22.5) <= 4.0);
}

bool:Veh_NearStation(playerid)
{
	for (new i = 0; i < 3; i++)
	{
		if ((GetPlayerDistanceFromPoint(playerid, g_VehStations[i][0], g_VehStations[i][1], g_VehStations[i][2]) <= 12.0))
		{
			return true;
		}
	}
	return false;
}

bool:Veh_Busy(playerid)
{
	return g_VPl[playerid][vpBuyPending] || g_VPl[playerid][vpSellPending];
}

bool:Veh_CooldownBlocked(playerid)
{
	new now = GetTickCount();
	if (now - g_VPl[playerid][vpCmdTick] < 2000)
	{
		Util_Send(playerid, -10270721, "მოიცადე, სანამ მანქანის სხვა ბრძანებას გამოიყენებ.");
		return true;
	}
	g_VPl[playerid][vpCmdTick] = now;
	return false;
}

Veh_SetParams(vehicleid, engine, lights, doors)
{
	new e = 0, l = 0, a = 0, d = 0, b = 0, bt = 0, o = 0;
	GetVehicleParamsEx(vehicleid, e, l, a, d, b, bt, o);
	if (engine != -1)
	{
		e = engine;
	}
	if (lights != -1)
	{
		l = lights;
	}
	if (doors != -1)
	{
		d = doors;
	}
	SetVehicleParamsEx(vehicleid, e, l, a, d, b, bt, o);
}

Veh_SyncTimers()
{
	new engines = 0, huds = 0;
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_VPl[p][vpSession] == 0)
		{
			continue;
		}
		if (g_VPl[p][vpLastVehicle] != 0)
		{
			huds++;
		}
		for (new s = 0; s < 4; s++)
		{
			if (g_VSlot[p][s][vsUsed] && g_VSlot[p][s][vsVehicleId] != 65535 && g_VSlot[p][s][vsEngineOn])
			{
				engines++;
			}
		}
	}
	if (engines > 0 && g_VFuelTimer == 0)
	{
		g_VFuelTimer = SetTimer("Veh_OnFuelTick", 10000, true);
	}
	else if (engines == 0 && g_VFuelTimer != 0)
	{
		KillTimer(g_VFuelTimer);
		g_VFuelTimer = 0;
	}
	if (huds > 0 && g_VHudTimer == 0)
	{
		g_VHudTimer = SetTimer("Veh_OnHudTick", 500, true);
	}
	else if (huds == 0 && g_VHudTimer != 0)
	{
		KillTimer(g_VHudTimer);
		g_VHudTimer = 0;
	}
}

bool:Veh_Lookup(vehicleid, &owner, &slot)
{
	owner = 65535;
	slot = -1;
	if (vehicleid <= 0 || vehicleid > 2000)
	{
		return false;
	}
	new o = g_VMapOwner[vehicleid], s = g_VMapSlot[vehicleid];
	if (o < 0 || o >= 1000 || !IsPlayerConnected(o))
	{
		return false;
	}
	if (g_VMapSession[vehicleid] != g_VPl[o][vpSession] || g_VPl[o][vpSession] == 0)
	{
		return false;
	}
	if (s < 0 || s >= 4)
	{
		return false;
	}
	if (!g_VSlot[o][s][vsUsed] || g_VSlot[o][s][vsVehicleId] != vehicleid)
	{
		return false;
	}
	owner = o;
	slot = s;
	return true;
}

Veh_HudStop(playerid)
{
	if (g_VPl[playerid][vpLastVehicle] == 0)
	{
		return 0;
	}
	g_VPl[playerid][vpLastVehicle] = 0;
	UI_VehicleHudHide(playerid);
	Veh_SyncTimers();
	return 0;
}

Veh_HudRefresh(playerid)
{
	new vid = g_VPl[playerid][vpLastVehicle], owner = 65535, slot = -1;
	if (vid == 0)
	{
		return 0;
	}
	if (!Veh_Lookup(vid, owner, slot) || owner != playerid || GetPlayerVehicleID(playerid) != vid)
	{
		Veh_HudStop(playerid);
		return 0;
	}
	new cap = Veh_CapacityCl(g_VSlot[playerid][slot][vsModel]);
	UI_VehicleHudUpdate(playerid, (float(g_VSlot[playerid][slot][vsFuelCl]) / float(100)), (float(cap) / float(100)), Veh_SpeedKmh(vid), bool:g_VSlot[playerid][slot][vsEngineOn]);
	return 0;
}

Veh_HudStart(playerid, vehicleid)
{
	g_VPl[playerid][vpLastVehicle] = vehicleid;
	UI_VehicleHudShow(playerid);
	Veh_SyncTimers();
	Veh_HudRefresh(playerid);
}

Veh_HudDropVehicle(vehicleid)
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_VPl[p][vpLastVehicle] == vehicleid)
		{
			Veh_HudStop(p);
		}
	}
}

bool:Veh_Spawn(playerid, slot, Float:x, Float:y, Float:z, Float:a)
{
	if (g_VPool >= 300)
	{
		return false;
	}
	if (g_VSlot[playerid][slot][vsVehicleId] != 65535)
	{
		return false;
	}
	new vid = CreateVehicle(g_VSlot[playerid][slot][vsModel], x, y, z, a, g_VSlot[playerid][slot][vsColor1], g_VSlot[playerid][slot][vsColor2], -1, false);
	if (vid == 65535 || vid <= 0 || vid > 2000)
	{
		return false;
	}
	SetVehicleNumberPlate(vid, g_VSlot[playerid][slot][vsPlate]);
	new Float:hp = g_VSlot[playerid][slot][vsHealth];
	if ((hp < 350.0))
	{
		hp = 350.0;
	}
	SetVehicleHealth(vid, hp);
	g_VSlot[playerid][slot][vsHealth] = hp;
	g_VSlot[playerid][slot][vsEngineOn] = 0;
	g_VSlot[playerid][slot][vsLowFuelWarned] = 0;
	g_VSlot[playerid][slot][vsPosX] = x;
	g_VSlot[playerid][slot][vsPosY] = y;
	g_VSlot[playerid][slot][vsPosZ] = z;
	g_VSlot[playerid][slot][vsPosA] = a;
	Veh_SetParams(vid, 0, 0, g_VSlot[playerid][slot][vsLocked] ? 1 : 0);
	g_VMapOwner[vid] = playerid;
	g_VMapSlot[vid] = slot;
	g_VMapSession[vid] = g_VPl[playerid][vpSession];
	g_VSlot[playerid][slot][vsVehicleId] = vid;
	g_VPool++;
	return true;
}

Veh_Despawn(playerid, slot)
{
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (vid == 65535)
	{
		return 0;
	}
	Veh_HudDropVehicle(vid);
	if (vid > 0 && vid <= 2000)
	{
		g_VMapOwner[vid] = 65535;
		g_VMapSlot[vid] = -1;
		g_VMapSession[vid] = 0;
	}
	DestroyVehicle(vid);
	g_VSlot[playerid][slot][vsVehicleId] = 65535;
	g_VSlot[playerid][slot][vsEngineOn] = 0;
	if (g_VPool > 0)
	{
		g_VPool--;
	}
	Veh_SyncTimers();
	return 0;
}

Veh_SaveSlot(playerid, slot)
{
	if (!g_VSlot[playerid][slot][vsUsed] || g_VSlot[playerid][slot][vsDbId] <= 0)
	{
		return 0;
	}
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (vid != 65535)
	{
		new Float:x, Float:y, Float:z, Float:a, Float:hp;
		GetVehiclePos(vid, x, y, z);
		GetVehicleZAngle(vid, a);
		GetVehicleHealth(vid, hp);
		g_VSlot[playerid][slot][vsPosX] = x;
		g_VSlot[playerid][slot][vsPosY] = y;
		g_VSlot[playerid][slot][vsPosZ] = z;
		g_VSlot[playerid][slot][vsPosA] = a;
		if (!g_VSlot[playerid][slot][vsDestroyed])
		{
			g_VSlot[playerid][slot][vsHealth] = hp;
		}
	}
	else if (g_VSlot[playerid][slot][vsDestroyed])
	{
		return 0;
	}
	DB_SaveVehicle(g_VSlot[playerid][slot][vsDbId], g_VSlot[playerid][slot][vsPosX], g_VSlot[playerid][slot][vsPosY], g_VSlot[playerid][slot][vsPosZ], g_VSlot[playerid][slot][vsPosA], 0, 0, g_VSlot[playerid][slot][vsHealth], g_VSlot[playerid][slot][vsFuelCl], bool:g_VSlot[playerid][slot][vsLocked], bool:g_VSlot[playerid][slot][vsDestroyed]);
	return 0;
}

Veh_SaveAll()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_VPl[p][vpSession] == 0)
		{
			continue;
		}
		for (new s = 0; s < 4; s++)
		{
			if (g_VSlot[p][s][vsUsed] && g_VSlot[p][s][vsVehicleId] != 65535)
			{
				Veh_SaveSlot(p, s);
			}
		}
	}
}

Veh_SetEngine(playerid, slot, bool:on)
{
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (vid == 65535)
	{
		return 0;
	}
	g_VSlot[playerid][slot][vsEngineOn] = on;
	Veh_SetParams(vid, on ? 1 : 0, -1, -1);
	Veh_SyncTimers();
	return 0;
}

Veh_ResetPlayer(playerid)
{
	for (new s = 0; s < 4; s++)
	{
		for (new i = 0; i < 30; i++)
		{
			g_VSlot[playerid][s][E_VSLOT:i] = 0;
		}
		g_VSlot[playerid][s][vsVehicleId] = 65535;
	}
	for (new i = 0; i < 14; i++)
	{
		g_VPl[playerid][E_VPLAYER:i] = 0;
	}
}

public Veh_OnFuelTick()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_VPl[p][vpSession] == 0 || !IsPlayerConnected(p))
		{
			continue;
		}
		for (new s = 0; s < 4; s++)
		{
			new vid = g_VSlot[p][s][vsVehicleId];
			if (!g_VSlot[p][s][vsUsed] || vid == 65535 || !g_VSlot[p][s][vsEngineOn])
			{
				continue;
			}
			new use = Veh_SpeedKmh(vid) >= 5 ? 6 : 2;
			g_VSlot[p][s][vsFuelCl] = max(g_VSlot[p][s][vsFuelCl] - use, 0);
			if (g_VSlot[p][s][vsFuelCl] <= 0)
			{
				Veh_SetEngine(p, s, false);
				Veh_SaveSlot(p, s);
				new driver = GetVehicleDriver(vid);
				if (driver >= 0 && driver < 1000)
				{
					Util_Send(driver, -10270721, "ავზი ცარიელია: ძრავი გაითიშა. მოძებნე ბენზინგასამართი სადგური (/refuel).");
				}
				if (driver != p)
				{
					Util_Send(p, -10270721, "შენს ერთ-ერთ მანქანას საწვავი გაუთავდა და გაითიშა.");
				}
			}
			else if (!g_VSlot[p][s][vsLowFuelWarned] && Veh_FuelPct(p, s) <= 15)
			{
				g_VSlot[p][s][vsLowFuelWarned] = 1;
				Util_Send(p, -10270721, "საწვავი ცოტაა: მიდი ბენზინგასამართ სადგურზე.");
			}
		}
	}
}

public Veh_OnHudTick()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_VPl[p][vpLastVehicle] != 0)
		{
			Veh_HudRefresh(p);
		}
	}
}

public Veh_OnSaveTick()
{
	Veh_SaveAll();
}

public Veh_OnFindExpire(playerid, session)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || g_VPl[playerid][vpSession] != session)
	{
		return 0;
	}
	g_VPl[playerid][vpFindTimer] = 0;
	DisablePlayerCheckpoint(playerid);
	return 0;
}

public Veh_OnWreckRemove(vehicleid, owner, session)
{
	new o = 65535, s = -1;
	if (!Veh_Lookup(vehicleid, o, s) || o != owner || g_VMapSession[vehicleid] != session)
	{
		return 0;
	}
	if (!g_VSlot[o][s][vsDestroyed])
	{
		return 0;
	}
	Veh_Despawn(o, s);
	return 0;
}

Veh_BeginLoad(playerid)
{
	g_VPl[playerid][vpLoadState] = 1;
	DB_LoadVehicles(playerid, gPlayer[playerid][pCharId]);
}

public Veh_OnDBLoaded(playerid, count)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_VPl[playerid][vpLoadState] != 1)
	{
		return 1;
	}
	if (count < 0)
	{
		g_VPl[playerid][vpLoadState] = 3;
		Util_Send(playerid, -10270721, "შენი მანქანების ჩატვირთვა ვერ მოხერხდა (მონაცემთა ბაზის შეცდომა). ყიდვა შეჩერებულია შემდეგ შესვლამდე.");
		return 1;
	}
	new needed = 0;
	for (new i = 0; i < count && i < 4; i++)
	{
		for (new f = 0; f < 30; f++)
		{
			g_VSlot[playerid][i][E_VSLOT:f] = 0;
		}
		g_VSlot[playerid][i][vsVehicleId] = 65535;
		g_VSlot[playerid][i][vsUsed] = 1;
		g_VSlot[playerid][i][vsDbId] = g_VehRows[playerid][i][vrId];
		g_VSlot[playerid][i][vsModel] = g_VehRows[playerid][i][vrModel];
		strmid(g_VSlot[playerid][i][vsPlate], g_VehRows[playerid][i][vrPlate], 0, strlen(g_VehRows[playerid][i][vrPlate]), 8);
		g_VSlot[playerid][i][vsColor1] = g_VehRows[playerid][i][vrColor1];
		g_VSlot[playerid][i][vsColor2] = g_VehRows[playerid][i][vrColor2];
		g_VSlot[playerid][i][vsPricePaid] = g_VehRows[playerid][i][vrPricePaid];
		g_VSlot[playerid][i][vsPosX] = g_VehRows[playerid][i][vrPosX];
		g_VSlot[playerid][i][vsPosY] = g_VehRows[playerid][i][vrPosY];
		g_VSlot[playerid][i][vsPosZ] = g_VehRows[playerid][i][vrPosZ];
		g_VSlot[playerid][i][vsPosA] = g_VehRows[playerid][i][vrPosA];
		g_VSlot[playerid][i][vsParkX] = g_VehRows[playerid][i][vrParkX];
		g_VSlot[playerid][i][vsParkY] = g_VehRows[playerid][i][vrParkY];
		g_VSlot[playerid][i][vsParkZ] = g_VehRows[playerid][i][vrParkZ];
		g_VSlot[playerid][i][vsParkA] = g_VehRows[playerid][i][vrParkA];
		g_VSlot[playerid][i][vsHealth] = g_VehRows[playerid][i][vrHealth];
		g_VSlot[playerid][i][vsFuelCl] = g_VehRows[playerid][i][vrFuelCl];
		g_VSlot[playerid][i][vsLocked] = g_VehRows[playerid][i][vrLocked];
		g_VSlot[playerid][i][vsDestroyed] = g_VehRows[playerid][i][vrDestroyed];
		if (!g_VSlot[playerid][i][vsDestroyed])
		{
			needed++;
		}
	}
	g_VPl[playerid][vpLoadState] = 2;
	if (count <= 0)
	{
		return 1;
	}
	if (g_VPool + needed > 300)
	{
		new name[75], msg[768];
		GetPlayerName(playerid, name, 75);
		format(msg, 768, "[მანქანები] მიღწეულია %d პირადი მანქანის ლიმიტი: %s-ის [%d] მანქანები არ შეიქმნა.", 300, name, playerid);
		Admin_Notify(msg, -5963521);
		Log_Write("VEH", msg);
		Util_Send(playerid, -10270721, "სერვერმა პირადი მანქანების ლიმიტს მიაღწია: შენი მანქანები ამჟამად მიუწვდომელია.");
		return 1;
	}
	new spawned = 0;
	for (new i = 0; i < count && i < 4; i++)
	{
		if (g_VSlot[playerid][i][vsDestroyed])
		{
			continue;
		}
		if (Veh_Spawn(playerid, i, g_VSlot[playerid][i][vsPosX], g_VSlot[playerid][i][vsPosY], g_VSlot[playerid][i][vsPosZ], g_VSlot[playerid][i][vsPosA]))
		{
			spawned++;
		}
	}
	SendClientMessage(playerid, -1, "გაქვს %d მანქანა (%d გზაზე). დეტალებისთვის გამოიყენე /v list.", count, spawned);
	return 1;
}

Veh_CmdDealer(playerid)
{
	if (!Veh_PlayerReady(playerid))
	{
		return 0;
	}
	if (Veh_CooldownBlocked(playerid))
	{
		return 0;
	}
	if (!Veh_NearDealer(playerid))
	{
		Util_Send(playerid, -10270721, "ლოს-სანტოსის ავტოსალონში უნდა იყო.");
		return 0;
	}
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
	{
		Util_Send(playerid, -10270721, "ავტოსალონით სარგებლობისთვის მანქანიდან გადმოდი.");
		return 0;
	}
	if (g_VPl[playerid][vpLoadState] != 2)
	{
		Util_Send(playerid, -10270721, "შენი მანქანები ჯერ არ ჩატვირთულა: ცოტა ხანში სცადე.");
		return 0;
	}
	if (Veh_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "მანქანასთან დაკავშირებული ოპერაცია მიმდინარეობს: მოიცადე.");
		return 0;
	}
	UI_VehicleShopShow(playerid, gPlayer[playerid][pLevel], gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
	return 0;
}

public UI_OnVehicleBuyConfirm(playerid, catalogIndex, payWithBank)
{
	if (!Veh_PlayerReady(playerid))
	{
		return 1;
	}
	if (!Veh_NearDealer(playerid))
	{
		Util_Send(playerid, -10270721, "ლოს-სანტოსის ავტოსალონში უნდა იყო.");
		return 1;
	}
	if (Veh_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "მანქანასთან დაკავშირებული ოპერაცია მიმდინარეობს: მოიცადე.");
		return 1;
	}
	if (g_VPl[playerid][vpLoadState] != 2)
	{
		Util_Send(playerid, -10270721, "შენი მანქანები მიუწვდომელია (ჩატვირთვა ვერ მოხერხდა ან მიმდინარეობს).");
		return 1;
	}
	if (catalogIndex < 0 || catalogIndex >= 13 || (payWithBank != 0 && payWithBank != 1))
	{
		Util_Send(playerid, -10270721, "ყიდვის არასწორი მოთხოვნა.");
		return 1;
	}
	if (gPlayer[playerid][pLevel] < g_VehCatLevel[catalogIndex])
	{
		SendClientMessage(playerid, -10270721, "ეს მოდელი მოითხოვს დონე %d-ს.", g_VehCatLevel[catalogIndex]);
		return 1;
	}
	if (Veh_UsedCount(playerid) >= Veh_MaxForLevel(gPlayer[playerid][pLevel]) || Veh_FreeSlot(playerid) == -1)
	{
		SendClientMessage(playerid, -10270721, "შენი დონისთვის მანქანების ლიმიტს მიაღწიე (%d).", Veh_MaxForLevel(gPlayer[playerid][pLevel]));
		return 1;
	}
	if (g_VPool >= 300)
	{
		Util_Send(playerid, -10270721, "ავტოსალონს ამჟამად მეტი მანქანის მიწოდება არ შეუძლია.");
		return 1;
	}
	new price = g_VehCatPrice[catalogIndex];
	if (payWithBank == 1 ? !Money_CanBank(playerid, price) : !Money_Can(playerid, price))
	{
		Util_Send(playerid, -10270721, "ამ გადახდის მეთოდით საკმარისი თანხა არ გაქვს.");
		return 1;
	}
	g_VPl[playerid][vpBuyPending] = 1;
	g_VPl[playerid][vpBuyCatalogIdx] = catalogIndex;
	g_VPl[playerid][vpBuyWithBank] = payWithBank;
	g_VPl[playerid][vpBuyPrice] = price;
	Util_Send(playerid, -1, "ყიდვა ფორმდება...");
	DB_CreateVehicle(playerid, gPlayer[playerid][pCharId], g_VehCatModel[catalogIndex], 1, 1, price, 1655.0, -1690.0, 22.5, 270.0, Veh_CapacityCl(g_VehCatModel[catalogIndex]) * 50 / 100);
	return 1;
}

Veh_RollbackCreate(playerid, vehDbId)
{
	g_VPl[playerid][vpSellPending] = 1;
	g_VPl[playerid][vpSellRollback] = 1;
	g_VPl[playerid][vpSellSlot] = -1;
	g_VPl[playerid][vpSellDbId] = vehDbId;
	g_VPl[playerid][vpSellRefund] = 0;
	DB_DeleteVehicle(playerid, gPlayer[playerid][pCharId], vehDbId);
}

public Veh_OnDBCreated(playerid, vehDbId, const plate[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (!g_VPl[playerid][vpBuyPending])
	{
		if (vehDbId > 0 && !g_VPl[playerid][vpSellPending])
		{
			Veh_RollbackCreate(playerid, vehDbId);
		}
		return 1;
	}
	g_VPl[playerid][vpBuyPending] = 0;
	if (vehDbId <= 0)
	{
		Util_Send(playerid, -10270721, "ყიდვა ვერ მოხერხდა (ავტოსალონის შეცდომა). თანხა არ ჩამოგეჭრა.");
		return 1;
	}
	new idx = g_VPl[playerid][vpBuyCatalogIdx], price = g_VPl[playerid][vpBuyPrice], bool:bank = g_VPl[playerid][vpBuyWithBank] == 1;
	new slot = Veh_FreeSlot(playerid);
	new bool:ok = slot != -1 && Veh_PlayerReady(playerid) && g_VPl[playerid][vpLoadState] == 2 && gPlayer[playerid][pLevel] >= g_VehCatLevel[idx] && Veh_UsedCount(playerid) < Veh_MaxForLevel(gPlayer[playerid][pLevel]) && (bank ? Money_CanBank(playerid, price) : Money_Can(playerid, price));
	if (!ok)
	{
		Veh_RollbackCreate(playerid, vehDbId);
		Util_Send(playerid, -10270721, "ყიდვა გაუქმდა: მოთხოვნები ან თანხა აღარ არის საკმარისი. თანხა არ ჩამოგეჭრა.");
		return 1;
	}
	for (new f = 0; f < 30; f++)
	{
		g_VSlot[playerid][slot][E_VSLOT:f] = 0;
	}
	g_VSlot[playerid][slot][vsVehicleId] = 65535;
	g_VSlot[playerid][slot][vsUsed] = 1;
	g_VSlot[playerid][slot][vsDbId] = vehDbId;
	g_VSlot[playerid][slot][vsModel] = g_VehCatModel[idx];
	strmid(g_VSlot[playerid][slot][vsPlate], plate, 0, strlen(plate), 8);
	g_VSlot[playerid][slot][vsColor1] = 1;
	g_VSlot[playerid][slot][vsColor2] = 1;
	g_VSlot[playerid][slot][vsPricePaid] = price;
	g_VSlot[playerid][slot][vsPosX] = 1655.0;
	g_VSlot[playerid][slot][vsPosY] = -1690.0;
	g_VSlot[playerid][slot][vsPosZ] = 22.5;
	g_VSlot[playerid][slot][vsPosA] = 270.0;
	g_VSlot[playerid][slot][vsParkX] = 1655.0;
	g_VSlot[playerid][slot][vsParkY] = -1690.0;
	g_VSlot[playerid][slot][vsParkZ] = 22.5;
	g_VSlot[playerid][slot][vsParkA] = 270.0;
	g_VSlot[playerid][slot][vsHealth] = 1000.0;
	g_VSlot[playerid][slot][vsFuelCl] = Veh_CapacityCl(g_VehCatModel[idx]) * 50 / 100;
	g_VSlot[playerid][slot][vsLocked] = 1;
	if (!Veh_Spawn(playerid, slot, 1655.0, -1690.0, 22.5, 270.0))
	{
		g_VSlot[playerid][slot][vsUsed] = 0;
		Veh_RollbackCreate(playerid, vehDbId);
		Util_Send(playerid, -10270721, "მიწოდება ვერ მოხერხდა (სერვერის მანქანების ლიმიტი). თანხა არ ჩამოგეჭრა.");
		return 1;
	}
	if (!(bank ? Money_TakeBank(playerid, price, "vehicle", "") : Money_Take(playerid, price)))
	{
		Veh_Despawn(playerid, slot);
		g_VSlot[playerid][slot][vsUsed] = 0;
		Veh_RollbackCreate(playerid, vehDbId);
		Util_Send(playerid, -10270721, "გადახდა ვერ მოხერხდა. ყიდვა გაუქმდა.");
		return 1;
	}
	Char_Save(playerid);
	new mname[36], pname[75], msg[256];
	Veh_ModelName(g_VehCatModel[idx], mname, 36);
	GetPlayerName(playerid, pname, 75);
	SendClientMessage(playerid, 0x66DD66FF, "შენ იყიდე %s (ნომერი %s) %s-ად (%s). ავტოსალონის გასასვლელთან გელოდება.", mname, plate, Money_Str(price), bank ? "ბანკიდან" : "ნაღდით");
	format(msg, 256, "ACQUISTO %s: %s targa %s %s (%s) dbId %d", pname, mname, plate, Money_Str(price), bank ? "banca" : "contanti", vehDbId);
	Log_Write("VEH", msg);
	return 1;
}

Veh_DoSell(playerid, slot, bool:confirmed)
{
	if (slot < 0 || slot >= 4 || !g_VSlot[playerid][slot][vsUsed])
	{
		Util_Send(playerid, -10270721, "მანქანის არასწორი სლოტი.");
		return 0;
	}
	if (Veh_Busy(playerid) || g_VSlot[playerid][slot][vsSelling])
	{
		Util_Send(playerid, -10270721, "მანქანასთან დაკავშირებული ოპერაცია მიმდინარეობს: მოიცადე.");
		return 0;
	}
	if (!Veh_NearDealer(playerid))
	{
		Util_Send(playerid, -10270721, "გაყიდვა მხოლოდ ლოს-სანტოსის ავტოსალონში შეგიძლია.");
		return 0;
	}
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (vid != 65535 && GetPlayerVehicleID(playerid) != vid)
	{
		new Float:px, Float:py, Float:pz;
		GetPlayerPos(playerid, px, py, pz);
		if ((GetVehicleDistanceFromPoint(vid, px, py, pz) > 5.0))
		{
			Util_Send(playerid, -10270721, "მანქანა შენთან ერთად ავტოსალონში უნდა იყოს (ან საჭესთან უნდა იჯდე).");
			return 0;
		}
	}
	new refund = Veh_ListPrice(g_VSlot[playerid][slot][vsModel], g_VSlot[playerid][slot][vsPricePaid]) * ECON_VEH_RESALE_PCT / 100;
	if (refund <= 0 || refund > Money_BankRoom(playerid))
	{
		Util_Send(playerid, -10270721, "შენს საბანკო ანგარიშს თანხის მიღება არ შეუძლია: გაყიდვა უარყოფილია.");
		return 0;
	}
	new mname[36];
	Veh_ModelName(g_VSlot[playerid][slot][vsModel], mname, 36);
	if (!confirmed)
	{
		SendClientMessage(playerid, -2994689, "გაყიდვა: %s (ნომერი %s): ბანკში მიიღებ %s-ს.", mname, g_VSlot[playerid][slot][vsPlate], Money_Str(refund));
		SendClientMessage(playerid, 0x99CCFFFF, "დასადასტურებლად: /v sell %d conferma", slot + 1);
		return 0;
	}
	g_VPl[playerid][vpSellPending] = 1;
	g_VPl[playerid][vpSellRollback] = 0;
	g_VPl[playerid][vpSellSlot] = slot;
	g_VPl[playerid][vpSellDbId] = g_VSlot[playerid][slot][vsDbId];
	g_VPl[playerid][vpSellRefund] = refund;
	g_VSlot[playerid][slot][vsSelling] = 1;
	Util_Send(playerid, -1, "მიმდინარეობს გაყიდვა...");
	DB_DeleteVehicle(playerid, gPlayer[playerid][pCharId], g_VSlot[playerid][slot][vsDbId]);
	return 0;
}

public Veh_OnDBDeleted(playerid, vehDbId, affected)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (!g_VPl[playerid][vpSellPending])
	{
		return 1;
	}
	new slot = g_VPl[playerid][vpSellSlot], bool:rollback = bool:g_VPl[playerid][vpSellRollback];
	new dbId = g_VPl[playerid][vpSellDbId], refund = g_VPl[playerid][vpSellRefund];
	g_VPl[playerid][vpSellPending] = 0;
	g_VPl[playerid][vpSellRollback] = 0;
	g_VPl[playerid][vpSellSlot] = -1;
	g_VPl[playerid][vpSellDbId] = 0;
	g_VPl[playerid][vpSellRefund] = 0;
	if (rollback)
	{
		if (affected != 1)
		{
			Log_Write("VEH", "Rollback acquisto non riuscito: riga veicolo da verificare a mano");
		}
		return 1;
	}
	if (slot >= 0 && slot < 4)
	{
		g_VSlot[playerid][slot][vsSelling] = 0;
	}
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "გაყიდვა ვერ მოხერხდა (მონაცემთა ბაზის შეცდომა). თანხა არ ჩაირიცხა.");
		return 1;
	}
	if (affected != 1 || vehDbId != dbId || slot < 0 || slot >= 4 || g_VSlot[playerid][slot][vsDbId] != dbId)
	{
		Util_Send(playerid, -10270721, "გაყიდვა არ შესრულდა: მანქანა აღარ არის შენს საკუთრებაში. თანხა არ ჩაირიცხა.");
		return 1;
	}
	new mname[36], plate[24], pname[75], msg[256];
	Veh_ModelName(g_VSlot[playerid][slot][vsModel], mname, 36);
	strmid(plate, g_VSlot[playerid][slot][vsPlate], 0, strlen(g_VSlot[playerid][slot][vsPlate]), 24);
	Veh_Despawn(playerid, slot);
	g_VSlot[playerid][slot][vsUsed] = 0;
	new paid = min(refund, Money_BankRoom(playerid));
	if (paid > 0)
	{
		Money_GiveBank(playerid, paid, "refund", "");
	}
	Char_Save(playerid);
	SendClientMessage(playerid, 0x66DD66FF, "შენ გაყიდე %s (ნომერი %s): ბანკში ჩაგერიცხა %s.", mname, plate, Money_Str(paid));
	if (paid < refund)
	{
		Util_Send(playerid, -10270721, "ანგარიშმა ლიმიტს მიაღწია: თანხის ნაწილი არ ჩაირიცხა.");
	}
	GetPlayerName(playerid, pname, 75);
	format(msg, 256, "VENDITA %s: %s targa %s dbId %d accredito %s", pname, mname, plate, dbId, Money_Str(paid));
	Log_Write("VEH", msg);
	return 1;
}

public UI_OnVehicleListAction(playerid, slot, action)
{
	if (!Veh_PlayerReady(playerid))
	{
		return 1;
	}
	if (slot < 1 || slot > 4)
	{
		return 1;
	}
	switch (action)
	{
		case 0:
		{
			Veh_DoFind(playerid, slot - 1);
		}
		case 1:
		{
			Veh_DoSell(playerid, slot - 1, true);
		}
	}
	return 1;
}

Veh_NearestOwnSlot(playerid)
{
	new cur = GetPlayerVehicleID(playerid), o = 65535, s = -1;
	if (cur != 0 && Veh_Lookup(cur, o, s) && o == playerid)
	{
		return s;
	}
	new Float:px, Float:py, Float:pz, Float:best = (5.0 + 1.0), found = -1;
	GetPlayerPos(playerid, px, py, pz);
	for (new i = 0; i < 4; i++)
	{
		new vid = g_VSlot[playerid][i][vsVehicleId];
		if (!g_VSlot[playerid][i][vsUsed] || vid == 65535)
		{
			continue;
		}
		new Float:d = GetVehicleDistanceFromPoint(vid, px, py, pz);
		if ((d <= 5.0) && (d < best))
		{
			best = d;
			found = i;
		}
	}
	return found;
}

Veh_DrivenSlot(playerid)
{
	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	{
		return -1;
	}
	new o = 65535, s = -1;
	if (!Veh_Lookup(GetPlayerVehicleID(playerid), o, s) || o != playerid)
	{
		return -1;
	}
	return s;
}

bool:Veh_ParseSlot(const token[], &slot)
{
	new v = 0;
	if (!Util_ParseInt(token, v) || v < 1 || v > 4)
	{
		return false;
	}
	slot = v - 1;
	return true;
}

Veh_Emote(playerid, const action[])
{
	new name[75], msg[256];
	GetPlayerName(playerid, name, 75);
	format(msg, 256, "* %s %s", name, action);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
}

Veh_DoLock(playerid)
{
	new slot = Veh_NearestOwnSlot(playerid);
	if (slot == -1)
	{
		SendClientMessage(playerid, -10270721, "%.0f მეტრის რადიუსში შენი მანქანა არ არის.", 5.0);
		return 0;
	}
	if (g_VSlot[playerid][slot][vsSelling])
	{
		Util_Send(playerid, -10270721, "მანქანა მიუწვდომელია: მიმდინარეობს ოპერაცია.");
		return 0;
	}
	new bool:lock = !g_VSlot[playerid][slot][vsLocked];
	g_VSlot[playerid][slot][vsLocked] = lock;
	Veh_SetParams(g_VSlot[playerid][slot][vsVehicleId], -1, -1, lock ? 1 : 0);
	Veh_Emote(playerid, lock ? "კეტავს თავისი მანქანის კარებს." : "აღებს თავისი მანქანის კარებს.");
	Veh_SaveSlot(playerid, slot);
	return 0;
}

// motore dei veicoli che non sono personali: noleggio, auto di strada, lavoro, merci
Veh_DoEngineOther(playerid)
{
	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	{
		Util_Send(playerid, -10270721, "ძრავის ჩასართავად საჭესთან უნდა იჯდე.");
		return 0;
	}
	new veh = GetPlayerVehicleID(playerid);
	if (StreetRental_SpotOf(veh) >= 0)
	{
		StreetRental_OnStateChange(playerid, PLAYER_STATE_DRIVER);   // auto di strada non ancora noleggiata: offerta
		return 0;
	}
	new bool:mine = (gPlayer[playerid][pRentalVehicle] != 0 && gPlayer[playerid][pRentalVehicle] == veh)
		|| LD_VehOwner(veh) == playerid
		|| AzWorld_CanDrive(playerid, veh) == 1
		|| Job_VehicleOwner(veh) == playerid;
	if (!mine)
	{
		Util_Send(playerid, -10270721, "ამ მანქანის გასაღები არ გაქვს.");
		return 0;
	}
	new e, l, a, d, b, bt, o;
	GetVehicleParamsEx(veh, e, l, a, d, b, bt, o);
	SetVehicleParamsEx(veh, e == 1 ? 0 : 1, l, a, d, b, bt, o);
	Veh_Emote(playerid, e == 1 ? "თიშავს ძრავს." : "ქოქავს ძრავს.");
	return 1;
}

Veh_DoEngine(playerid)
{
	new slot = Veh_DrivenSlot(playerid);
	if (slot == -1)
	{
		return Veh_DoEngineOther(playerid);
	}
	if (g_VSlot[playerid][slot][vsSelling])
	{
		Util_Send(playerid, -10270721, "მანქანა მიუწვდომელია: მიმდინარეობს ოპერაცია.");
		return 0;
	}
	if (g_VSlot[playerid][slot][vsEngineOn])
	{
		Veh_SetEngine(playerid, slot, false);
		Veh_Emote(playerid, "თიშავს ძრავს.");
		return 0;
	}
	if (g_VSlot[playerid][slot][vsDestroyed])
	{
		Util_Send(playerid, -10270721, "მანქანა განადგურებულია.");
		return 0;
	}
	if (g_VSlot[playerid][slot][vsFuelCl] <= 0)
	{
		Util_Send(playerid, -10270721, "ავზი ცარიელია.");
		return 0;
	}
	Veh_SetEngine(playerid, slot, true);
	Veh_Emote(playerid, "ქოქავს ძრავს.");
	return 0;
}

Veh_DoLights(playerid)
{
	new slot = Veh_DrivenSlot(playerid);
	if (slot == -1)
	{
		Util_Send(playerid, -10270721, "შენი მანქანის მძღოლის ადგილზე უნდა იჯდე.");
		return 0;
	}
	new vid = g_VSlot[playerid][slot][vsVehicleId], e = 0, l = 0, a = 0, d = 0, b = 0, bt = 0, o = 0;
	GetVehicleParamsEx(vid, e, l, a, d, b, bt, o);
	Veh_SetParams(vid, -1, l == 1 ? 0 : 1, -1);
	Util_Send(playerid, -1, l == 1 ? "ფარები ჩაქრა." : "ფარები აინთო.");
	return 0;
}

Veh_DoPark(playerid)
{
	new slot = Veh_DrivenSlot(playerid);
	if (slot == -1)
	{
		Util_Send(playerid, -10270721, "შენი მანქანის საჭესთან უნდა იჯდე.");
		return 0;
	}
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (g_VSlot[playerid][slot][vsSelling] || g_VSlot[playerid][slot][vsDestroyed])
	{
		Util_Send(playerid, -10270721, "მანქანა მიუწვდომელია.");
		return 0;
	}
	if (Veh_SpeedKmh(vid) >= 5)
	{
		Util_Send(playerid, -10270721, "დასაპარკინგებლად უნდა გაჩერდე.");
		return 0;
	}
	new Float:x, Float:y, Float:z, Float:a;
	GetVehiclePos(vid, x, y, z);
	GetVehicleZAngle(vid, a);
	g_VSlot[playerid][slot][vsParkX] = x;
	g_VSlot[playerid][slot][vsParkY] = y;
	g_VSlot[playerid][slot][vsParkZ] = z;
	g_VSlot[playerid][slot][vsParkA] = a;
	DB_SetVehiclePark(g_VSlot[playerid][slot][vsDbId], gPlayer[playerid][pCharId], x, y, z, a);
	Veh_SaveSlot(playerid, slot);
	Util_Send(playerid, 0x66DD66FF, "პარკინგის ადგილი შენახულია.");
	return 0;
}

Veh_DoFind(playerid, slot)
{
	if (slot < 0 || slot >= 4 || !g_VSlot[playerid][slot][vsUsed])
	{
		Util_Send(playerid, -10270721, "მანქანის არასწორი სლოტი.");
		return 0;
	}
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (vid == 65535)
	{
		Util_Send(playerid, -10270721, g_VSlot[playerid][slot][vsDestroyed] ? "მანქანა განადგურებულია: გამოიყენე /v recover." : "მანქანა ამჟამად გზაზე არ არის.");
		return 0;
	}
	new Float:x, Float:y, Float:z, Float:px, Float:py, Float:pz;
	GetVehiclePos(vid, x, y, z);
	GetPlayerPos(playerid, px, py, pz);
	if (g_VPl[playerid][vpFindTimer] != 0)
	{
		KillTimer(g_VPl[playerid][vpFindTimer]);
	}
	SetPlayerCheckpoint(playerid, x, y, z, 4.0);
	g_VPl[playerid][vpFindTimer] = SetTimerEx("Veh_OnFindExpire", 60000, false, "dd", playerid, g_VPl[playerid][vpSession]);
	SendClientMessage(playerid, -1, "მანქანა %d რუკაზე მონიშნულია %d წამით (მანძილი: %.0f მ).", slot + 1, 60, GetVehicleDistanceFromPoint(vid, px, py, pz));
	return 0;
}

Veh_DoList(playerid)
{
	if (g_VPl[playerid][vpLoadState] != 2)
	{
		Util_Send(playerid, -10270721, "შენი მანქანები მიუწვდომელია (ჩატვირთვა ვერ მოხერხდა ან მიმდინარეობს).");
		return 0;
	}
	UI_VehicleListClear(playerid);
	for (new s = 0; s < 4; s++)
	{
		if (!g_VSlot[playerid][s][vsUsed])
		{
			continue;
		}
		new st = 0, vid = g_VSlot[playerid][s][vsVehicleId];
		if (g_VSlot[playerid][s][vsDestroyed])
		{
			st = 2;
		}
		else if (vid == 65535)
		{
			st = 1;
		}
		else if (GetVehicleDriver(vid) == 65535 && (GetVehicleDistanceFromPoint(vid, g_VSlot[playerid][s][vsParkX], g_VSlot[playerid][s][vsParkY], g_VSlot[playerid][s][vsParkZ]) <= 6.0))
		{
			st = 1;
		}
		UI_VehicleListAdd(playerid, s + 1, g_VSlot[playerid][s][vsModel], g_VSlot[playerid][s][vsPlate], Veh_FuelPct(playerid, s), st);
	}
	UI_VehicleListShow(playerid);
	return 0;
}

Veh_DoRecover(playerid, slot)
{
	if (slot < 0 || slot >= 4 || !g_VSlot[playerid][slot][vsUsed])
	{
		Util_Send(playerid, -10270721, "მანქანის არასწორი სლოტი.");
		return 0;
	}
	if (Veh_Busy(playerid) || g_VSlot[playerid][slot][vsSelling])
	{
		Util_Send(playerid, -10270721, "მანქანასთან დაკავშირებული ოპერაცია მიმდინარეობს: მოიცადე.");
		return 0;
	}
	if (!g_VSlot[playerid][slot][vsDestroyed])
	{
		Util_Send(playerid, -10270721, "ეს მანქანა განადგურებული არ არის.");
		return 0;
	}
	if (g_VPool >= 300)
	{
		Util_Send(playerid, -10270721, "სერვერის მანქანების ლიმიტი ამოწურულია: მოგვიანებით სცადე.");
		return 0;
	}
	new listPrice = Veh_ListPrice(g_VSlot[playerid][slot][vsModel], g_VSlot[playerid][slot][vsPricePaid]);
	new cost = max(listPrice * ECON_VEH_RECOVER_PCT / 100, ECON_VEH_RECOVER_MIN);
	new bool:useCash = Money_Can(playerid, cost);
	if (!useCash && !Money_CanBank(playerid, cost))
	{
		SendClientMessage(playerid, -10270721, "დაზღვევა %s: თანხა არ გყოფნის (ნაღდი ფული ან ბანკი).", Money_Str(cost));
		return 0;
	}
	new vid = g_VSlot[playerid][slot][vsVehicleId];
	if (vid != 65535)
	{
		Veh_Despawn(playerid, slot);
	}
	new minFuel = Veh_CapacityCl(g_VSlot[playerid][slot][vsModel]) * 10 / 100;
	g_VSlot[playerid][slot][vsDestroyed] = 0;
	g_VSlot[playerid][slot][vsHealth] = 350.0;
	g_VSlot[playerid][slot][vsFuelCl] = max(g_VSlot[playerid][slot][vsFuelCl], minFuel);
	g_VSlot[playerid][slot][vsLocked] = 1;
	if (!Veh_Spawn(playerid, slot, g_VSlot[playerid][slot][vsParkX], g_VSlot[playerid][slot][vsParkY], g_VSlot[playerid][slot][vsParkZ], g_VSlot[playerid][slot][vsParkA]))
	{
		g_VSlot[playerid][slot][vsDestroyed] = 1;
		Util_Send(playerid, -10270721, "აღდგენა ვერ მოხერხდა (სერვერის მანქანების ლიმიტი). თანხა არ ჩამოგეჭრა.");
		return 0;
	}
	if (!(useCash ? Money_Take(playerid, cost) : Money_TakeBank(playerid, cost, "bill", "")))
	{
		Veh_Despawn(playerid, slot);
		g_VSlot[playerid][slot][vsDestroyed] = 1;
		Util_Send(playerid, -10270721, "გადახდა ვერ მოხერხდა. აღდგენა გაუქმდა.");
		return 0;
	}
	Char_Save(playerid);
	DB_RecoverVehicle(g_VSlot[playerid][slot][vsDbId], gPlayer[playerid][pCharId], g_VSlot[playerid][slot][vsHealth], g_VSlot[playerid][slot][vsFuelCl]);
	new mname[36], pname[75], msg[256];
	Veh_ModelName(g_VSlot[playerid][slot][vsModel], mname, 36);
	SendClientMessage(playerid, 0x66DD66FF, "დაზღვევა გადახდილია (%s, %s): შენი %s პარკინგზე გელოდება.", Money_Str(cost), useCash ? "ნაღდით" : "ბანკიდან", mname);
	GetPlayerName(playerid, pname, 75);
	format(msg, 256, "RECUPERO %s: %s dbId %d costo %s", pname, mname, g_VSlot[playerid][slot][vsDbId], Money_Str(cost));
	Log_Write("VEH", msg);
	return 0;
}

Veh_CmdV(playerid, const params[])
{
	if (!Veh_PlayerReady(playerid))
	{
		return 0;
	}
	new pos = 0, sub[48], a1[48], a2[48];
	Util_NextToken(params, pos, sub, 48);
	Util_NextToken(params, pos, a1, 48);
	Util_NextToken(params, pos, a2, 48);
	for (new i = 0; sub[i] != 0; i++)
	{
		sub[i] = tolower(sub[i]);
	}
	if (sub[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /v lock | engine | lights | park | find [slot] | list | sell [slot] | recover [slot]");
		return 0;
	}
	if (Veh_CooldownBlocked(playerid))
	{
		return 0;
	}
	if (!strcmp(sub, "lock", false, 0x7FFFFFFF))
	{
		Veh_DoLock(playerid);
		return 0;
	}
	if (!strcmp(sub, "engine", false, 0x7FFFFFFF))
	{
		Veh_DoEngine(playerid);
		return 0;
	}
	if (!strcmp(sub, "lights", false, 0x7FFFFFFF))
	{
		Veh_DoLights(playerid);
		return 0;
	}
	if (!strcmp(sub, "park", false, 0x7FFFFFFF))
	{
		Veh_DoPark(playerid);
		return 0;
	}
	if (!strcmp(sub, "list", false, 0x7FFFFFFF))
	{
		Veh_DoList(playerid);
		return 0;
	}
	if (!strcmp(sub, "tow", false, 0x7FFFFFFF))
	{
		Util_Send(playerid, -10270721, "ევაკუატორი ჯერ მიუწვდომელია.");
		return 0;
	}
	new slot = -1;
	if (!strcmp(sub, "find", false, 0x7FFFFFFF) || !strcmp(sub, "sell", false, 0x7FFFFFFF) || !strcmp(sub, "recover", false, 0x7FFFFFFF))
	{
		new bool:confirm = false;
		if (!strcmp(sub, "sell", false, 0x7FFFFFFF) && !strcmp(a1, "conferma", false, 0x7FFFFFFF))
		{
			confirm = true;
			a1[0] = 0;
		}
		else if (!strcmp(sub, "sell", false, 0x7FFFFFFF) && !strcmp(a2, "conferma", false, 0x7FFFFFFF))
		{
			confirm = true;
		}
		if (a1[0] != 0)
		{
			if (!Veh_ParseSlot(a1, slot))
			{
				SendClientMessage(playerid, -10270721, "არასწორი სლოტი (1-%d).", 4);
				return 0;
			}
		}
		else if (!strcmp(sub, "recover", false, 0x7FFFFFFF))
		{
			new destroyed = 0;
			for (new s = 0; s < 4; s++)
			{
				if (g_VSlot[playerid][s][vsUsed] && g_VSlot[playerid][s][vsDestroyed])
				{
					slot = s;
					destroyed++;
				}
			}
			if (destroyed != 1)
			{
				Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /v recover [slot] (/v list - ნახე, რომელია განადგურებული)");
				return 0;
			}
		}
		else
		{
			slot = Veh_NearestOwnSlot(playerid);
			if (slot == -1 && Veh_UsedCount(playerid) == 1)
			{
				for (new s = 0; s < 4; s++)
				{
					if (g_VSlot[playerid][s][vsUsed])
					{
						slot = s;
					}
				}
			}
			if (slot == -1)
			{
				Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /v find|sell [slot] (ახლოს შენი მანქანა არ არის)");
				return 0;
			}
		}
		if (!strcmp(sub, "find", false, 0x7FFFFFFF))
		{
			Veh_DoFind(playerid, slot);
		}
		else if (!strcmp(sub, "recover", false, 0x7FFFFFFF))
		{
			Veh_DoRecover(playerid, slot);
		}
		else
		{
			Veh_DoSell(playerid, slot, confirm);
		}
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /v lock | engine | lights | park | find [slot] | list | sell [slot] | recover [slot]");
	return 0;
}

Veh_CmdRefuel(playerid, const params[])
{
	if (!Veh_PlayerReady(playerid))
	{
		return 0;
	}
	if (Veh_CooldownBlocked(playerid))
	{
		return 0;
	}
	new slot = Veh_DrivenSlot(playerid);
	if (slot == -1)
	{
		Util_Send(playerid, -10270721, "შენი მანქანის საჭესთან უნდა იჯდე.");
		return 0;
	}
	if (!Veh_NearStation(playerid))
	{
		Util_Send(playerid, -10270721, "ბენზინგასამართ სადგურთან არ ხარ.");
		return 0;
	}
	if (Veh_Busy(playerid) || g_VSlot[playerid][slot][vsSelling])
	{
		Util_Send(playerid, -10270721, "მანქანა მიუწვდომელია: მიმდინარეობს ოპერაცია.");
		return 0;
	}
	if (g_VSlot[playerid][slot][vsEngineOn])
	{
		Util_Send(playerid, -10270721, "საწვავის ჩასხმამდე ძრავი გამორთე (/v engine).");
		return 0;
	}
	new cap = Veh_CapacityCl(g_VSlot[playerid][slot][vsModel]);
	new need = cap - g_VSlot[playerid][slot][vsFuelCl];
	if (need <= 0)
	{
		Util_Send(playerid, -10270721, "ავზი უკვე სავსეა.");
		return 0;
	}
	new want = need, pos = 0, tok[36];
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] != 0)
	{
		new liters = 0;
		if (!Util_ParseInt(tok, liters) || liters < 1 || liters > cap / 100)
		{
			SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /refuel [ლიტრი 1-%d] (არგუმენტის გარეშე: სავსე ავზი)", cap / 100);
			return 0;
		}
		want = min(liters * 100, need);
	}
	new cash = min(gPlayer[playerid][pMoney], cap);
	new affordable = cash * 100 / 3;
	new add = min(want, affordable);
	if (add <= 0)
	{
		Util_Send(playerid, -10270721, "საწვავისთვის საკმარისი ნაღდი ფული არ გაქვს.");
		return 0;
	}
	new cost = (add * ECON_FUEL_PER_LITER + 100 - 1) / 100;   // add in centilitri, costo in tetri
	if (!Money_Take(playerid, cost))
	{
		Util_Send(playerid, -10270721, "საწვავისთვის საკმარისი ნაღდი ფული არ გაქვს.");
		return 0;
	}
	g_VSlot[playerid][slot][vsFuelCl] += add;
	if (Veh_FuelPct(playerid, slot) > 15)
	{
		g_VSlot[playerid][slot][vsLowFuelWarned] = 0;
	}
	Char_Save(playerid);
	Veh_SaveSlot(playerid, slot);
	Veh_HudRefresh(playerid);
	SendClientMessage(playerid, 0x66DD66FF, "ჩასხმულია: +%.1f ლ %s-ად. ავზი: %.1f/%.0f ლ.", (float(add) / float(100)), Money_Str(cost), (float(g_VSlot[playerid][slot][vsFuelCl]) / float(100)), (float(cap) / float(100)));
	return 0;
}

Veh_OnInit()
{
	for (new v = 0; v < 2001; v++)
	{
		g_VMapOwner[v] = 65535;
		g_VMapSlot[v] = -1;
		g_VMapSession[v] = 0;
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		Veh_ResetPlayer(p);
	}
	g_VPool = 0;
	g_VSaveTimer = SetTimer("Veh_OnSaveTick", 120000, true);
	CreatePickup(1239, 1, 1655.0, -1665.0, 22.5, 0);
	Lang_Create3DTextLabel("ლოს-სანტოსის ავტოსალონი\nგამოიყენე /concessionario", -2994689, 1655.0, -1665.0, (22.5 + 1.0), 15.0, 0, false);
	for (new i = 0; i < 3; i++)
	{
		Lang_Create3DTextLabel("ბენზინგასამართი სადგური\nგამოიყენე /refuel [ლიტრი]", -2994689, g_VehStations[i][0], g_VehStations[i][1], (1.0 + g_VehStations[i][2]), 15.0, 0, false);
	}
}

Veh_OnExit()
{
	Veh_SaveAll();
	if (g_VSaveTimer != 0)
	{
		KillTimer(g_VSaveTimer);
	}
	if (g_VFuelTimer != 0)
	{
		KillTimer(g_VFuelTimer);
	}
	if (g_VHudTimer != 0)
	{
		KillTimer(g_VHudTimer);
	}
	g_VSaveTimer = 0;
	g_VFuelTimer = 0;
	g_VHudTimer = 0;
}

Veh_OnPlayerConnect(playerid)
{
	Veh_ResetPlayer(playerid);
	g_VSessionSeq++;
	g_VPl[playerid][vpSession] = g_VSessionSeq;
	g_VPl[playerid][vpCmdTick] = GetTickCount() - 2000;
}

Veh_OnPlayerSpawn(playerid)
{
	if (!Veh_PlayerReady(playerid))
	{
		return 0;
	}
	if (g_VPl[playerid][vpLoadState] == 0 || g_VPl[playerid][vpLoadState] == 3)
	{
		Veh_BeginLoad(playerid);
	}
	return 0;
}

Veh_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (g_VPl[playerid][vpFindTimer] != 0)
	{
		KillTimer(g_VPl[playerid][vpFindTimer]);
	}
	Veh_HudStop(playerid);
	for (new s = 0; s < 4; s++)
	{
		if (!g_VSlot[playerid][s][vsUsed])
		{
			continue;
		}
		if (g_VSlot[playerid][s][vsVehicleId] != 65535)
		{
			if (g_VSlot[playerid][s][vsEngineOn])
			{
				g_VSlot[playerid][s][vsEngineOn] = 0;
			}
			Veh_SaveSlot(playerid, s);
			Veh_Despawn(playerid, s);
		}
	}
	Veh_ResetPlayer(playerid);
	Veh_SyncTimers();
	return 0;
}

Veh_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	new o = 65535, s = -1;
	if (!Veh_Lookup(vehicleid, o, s) || o == playerid)
	{
		return 0;
	}
	if (ispassenger == 0)
	{
		ClearAnimations(playerid, SYNC_NONE);
		Util_Send(playerid, -10270721, "ამ მანქანის გასაღები არ გაქვს.");
	}
	return 0;
}

Veh_OnPlayerExitVehicle(playerid, vehicleid)
{
	#pragma unused playerid
	new o = 65535, s = -1;
	if (Veh_Lookup(vehicleid, o, s))
	{
		Veh_SaveSlot(o, s);
	}
}

Veh_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate, t_PLAYER_STATE:oldstate)
{
	if (oldstate == PLAYER_STATE_DRIVER && g_VPl[playerid][vpLastVehicle] != 0)
	{
		new vid = g_VPl[playerid][vpLastVehicle], o = 65535, s = -1;
		Veh_HudStop(playerid);
		if (Veh_Lookup(vid, o, s) && o == playerid)
		{
			if (g_VSlot[o][s][vsEngineOn])
			{
				Veh_SetEngine(o, s, false);
			}
			Veh_SaveSlot(o, s);
		}
	}
	if (newstate == PLAYER_STATE_DRIVER)
	{
		new vid = GetPlayerVehicleID(playerid), o = 65535, s = -1;
		if (!Veh_Lookup(vid, o, s))
		{
			return 0;
		}
		if (o != playerid)
		{
			Util_Send(playerid, -10270721, "ამ მანქანის გასაღები არ გაქვს.");
			RemovePlayerFromVehicle(playerid, false);
			return 0;
		}
		Veh_SetParams(vid, g_VSlot[o][s][vsEngineOn] ? 1 : 0, -1, -1);
		Veh_HudStart(playerid, vid);
		if (!g_VSlot[o][s][vsEngineOn])
		{
			Util_Send(playerid, 0x99CCFFFF, "ძრავი გამორთულია: დააჭირე N ან დაწერე /v engine.");
		}
	}
	return 0;
}

Veh_OnVehicleDeath(vehicleid, killerid)
{
	#pragma unused killerid
	new o = 65535, s = -1;
	if (!Veh_Lookup(vehicleid, o, s) || g_VSlot[o][s][vsDestroyed])
	{
		return 0;
	}
	if (g_VSlot[o][s][vsEngineOn])
	{
		Veh_SetEngine(o, s, false);
	}
	g_VSlot[o][s][vsDestroyed] = 1;
	g_VSlot[o][s][vsHealth] = 350.0;
	Veh_SaveSlot(o, s);
	SetTimerEx("Veh_OnWreckRemove", 10000, false, "ddd", vehicleid, o, g_VMapSession[vehicleid]);
	new mname[36], pname[75], msg[256];
	Veh_ModelName(g_VSlot[o][s][vsModel], mname, 36);
	SendClientMessage(o, -10270721, "შენი %s (სლოტი %d) განადგურდა. აღადგინე /v recover %d-ით.", mname, s + 1, s + 1);
	GetPlayerName(o, pname, 75);
	format(msg, 256, "DISTRUTTO %s: %s dbId %d", pname, mname, g_VSlot[o][s][vsDbId]);
	Log_Write("VEH", msg);
	return 0;
}
