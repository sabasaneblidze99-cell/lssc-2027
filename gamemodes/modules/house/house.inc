new g_HPl[MAX_PLAYERS][E_HPLAYER];
new g_HSessionSeq;
new g_HsPickup[64];
new bool:g_HsFridgeBusy[64];
new Text3D:g_HsLabel[64];
new bool:g_HsBusy[64];
new bool:g_HsReady;
new g_HsTickTimer;
new g_HsSaveTimer;
new s_HsTierName[4][12] = {
	"Monolocale",
	"Bilocale",
	"Casa media",
	"Villa"
};
new s_HsFridgeCap[4] = {10, 16, 24, 40};
new s_HsTierLevel[4] = {1, 2, 5, 10};
new s_HsBillLuce[4] = {45, 70, 110, 220};
new s_HsBillGas[4] = {30, 45, 70, 140};
new s_HsIntId[8] = {1, 2, 2, 1, 5, 3, 8, 5};
new Float:s_HsIntPos[8][4] = {
	{244.41, 305.03, 999.15, 180.0},
	{267.23, 305.0, 999.15, 270.0},
	{2468.84, -1698.3, 1013.5, 90.0},
	{2525.0, -1679.1, 1015.5, 270.0},
	{2233.7, -1112.8, 1050.9},
	{2496.0, -1692.1, 1014.74, 180.0},
	{2365.1, -1135.6, 1050.9},
	{1299.1, -794.8, 1084.0}
};
new Float:s_HsShops[2][3] = {
	{1833.0, -1842.0, 13.6},
	{1004.0, -926.0, 42.3}
};

House_Find(dbId)
{
	for (new i = 0; i < g_HouseCount; i++)
	{
		if (g_HouseRows[i][hrId] == dbId)
		{
			return i;
		}
	}
	return -1;
}

public House_OnDBCompensated(houseId)
{
	new idx = House_Find(houseId);
	if (idx != -1)
	{
		g_HsBusy[idx] = 0;
	}
	return 1;
}

bool:House_PlayerOk(playerid)
{
	return Util_IsPlaying(playerid) && g_HPl[playerid][hpSession] != 0;
}

House_Tier(idx)
{
	return clamp(g_HouseRows[idx][hrTier], 0, 3);
}

House_IntIdx(idx)
{
	return clamp(g_HouseRows[idx][hrInteriorIdx], 0, 7);
}

House_FridgeCap(idx)
{
	return s_HsFridgeCap[House_Tier(idx)];
}

bool:House_IsGuest(idx, charId)
{
	if (charId <= 0)
	{
		return 0;
	}
	for (new k = 0; k < g_HouseRows[idx][hrKeyCount] && k < 3; k++)
	{
		if (g_HouseRows[idx][hrKeyChars][k] == charId)
		{
			return 1;
		}
	}
	return 0;
}

House_Role(idx, playerid)
{
	new owner = g_HouseRows[idx][hrOwnerId];
	if (owner <= 0)
	{
		return 0;
	}
	if (owner == gPlayer[playerid][pCharId])
	{
		return 1;
	}
	if (House_IsGuest(idx, gPlayer[playerid][pCharId]))
	{
		return 2;
	}
	return 3;
}

bool:House_HasAccess(idx, playerid)
{
	new role = House_Role(idx, playerid);
	return role == 1 || role == 2;
}

bool:House_CanEnterNow(idx, playerid)
{
	return g_HouseRows[idx][hrOwnerId] > 0 && (!g_HouseRows[idx][hrLocked] || House_HasAccess(idx, playerid));
}

bool:House_LightOn(idx)
{
	return g_HouseRows[idx][hrLuceOn] && !g_HouseRows[idx][hrLuceCut];
}

bool:House_GasOn(idx)
{
	return g_HouseRows[idx][hrGasOn] && !g_HouseRows[idx][hrGasCut];
}

House_UtilState(bool:on, bool:cut)
{
	if (cut)
	{
		return 2;
	}
	return on ? 1 : 0;
}

House_OwnedCount(charId)
{
	new n = 0;
	if (charId <= 0)
	{
		return 0;
	}
	for (new i = 0; i < g_HouseCount; i++)
	{
		if (g_HouseRows[i][hrOwnerId] == charId)
		{
			n++;
		}
	}
	return n;
}

House_MaxForLevel(level)
{
	if (level >= 25)
	{
		return 3;
	}
	if (level >= 10)
	{
		return 2;
	}
	return 1;
}

House_OwnerPlayer(idx)
{
	new owner = g_HouseRows[idx][hrOwnerId];
	if (owner <= 0)
	{
		return 65535;
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (IsPlayerConnected(p) && gPlayer[p][pLoggedIn] && gPlayer[p][pCharId] == owner)
		{
			return p;
		}
	}
	return 65535;
}

House_PlayerByChar(charId)
{
	if (charId <= 0)
	{
		return 65535;
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (IsPlayerConnected(p) && gPlayer[p][pLoggedIn] && gPlayer[p][pCharId] == charId)
		{
			return p;
		}
	}
	return 65535;
}

bool:House_PlayerActive(playerid, now)
{
	if (!Util_IsPlaying(playerid))
	{
		return 0;
	}
	return now - gPlayer[playerid][pLastActiveTick] <= 10000;
}

House_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	for (new i = 0; i < 56; i++)
	{
		g_HPl[playerid][i] = 0;
	}
}

bool:House_OpMatches(playerid, &houseId, result)
{
	if (g_HPl[playerid][hpOpHouseId] == houseId)
	{
		return 1;
	}
	if (result != -1)
	{
		return 0;
	}
	houseId = g_HPl[playerid][hpOpHouseId];
	return 1;
}

bool:House_CooldownBlocked(playerid)
{
	new now = GetTickCount();
	if (now - g_HPl[playerid][hpCmdTick] < 1500)
	{
		Util_Send(playerid, -10270721, "Attendi un attimo prima di usare un altro comando della casa.");
		return 1;
	}
	g_HPl[playerid][hpCmdTick] = now;
	return 0;
}

bool:House_CmdGuard(playerid, bool:needIdle)
{
	if (!House_PlayerOk(playerid))
	{
		return 0;
	}
	if (!g_HsReady)
	{
		Util_Send(playerid, -10270721, "Il sistema case non e' disponibile al momento.");
		return 0;
	}
	if (House_CooldownBlocked(playerid))
	{
		return 0;
	}
	if (needIdle && g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "Hai un'operazione della casa in corso: attendi.");
		return 0;
	}
	return 1;
}

House_ClearInside(playerid)
{
	g_HPl[playerid][hpInsideHouse] = 0;
	g_HPl[playerid][hpCooking] = 0;
	UI_HouseHudHide(playerid);
	UI_HouseDarkHide(playerid);
}

House_FindInside(playerid)
{
	new id = g_HPl[playerid][hpInsideHouse];
	if (id <= 0)
	{
		return -1;
	}
	new idx = House_Find(id);
	if (idx == -1 || GetPlayerVirtualWorld(playerid) != id + 1000)
	{
		House_ClearInside(playerid);
		return -1;
	}
	return idx;
}

House_FindNear(playerid)
{
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		return -1;
	}
	new best = -1, Float:bestDist = 2.0;
	for (new i = 0; i < g_HouseCount; i++)
	{
		new Float:d = GetPlayerDistanceFromPoint(playerid, g_HouseRows[i][hrExtX], g_HouseRows[i][hrExtY], g_HouseRows[i][hrExtZ]);
		if ((d <= bestDist))
		{
			bestDist = d;
			best = i;
		}
	}
	return best;
}

House_Context(playerid)
{
	new idx = House_FindInside(playerid);
	if (idx != -1)
	{
		return idx;
	}
	return House_FindNear(playerid);
}

bool:House_NearExit(playerid, idx)
{
	new k = House_IntIdx(idx);
	return (GetPlayerDistanceFromPoint(playerid, s_HsIntPos[k][0], s_HsIntPos[k][1], s_HsIntPos[k][2]) <= 2.5);
}

bool:House_NearShop(playerid)
{
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		return 0;
	}
	for (new i = 0; i < 2; i++)
	{
		if ((GetPlayerDistanceFromPoint(playerid, s_HsShops[i][0], s_HsShops[i][1], s_HsShops[i][2]) <= 3.0))
		{
			return 1;
		}
	}
	return 0;
}

House_RefreshWorld(idx)
{
	if (idx < 0 || idx >= g_HouseCount)
	{
		return 0;
	}
	new text[160], tier = House_Tier(idx), bool:free = g_HouseRows[idx][hrOwnerId] <= 0;
	if (free)
	{
		format(text, 160, "Casa %d - %s\nIn vendita: $%d (liv. %d)\nUsa /casa per info", g_HouseRows[idx][hrId], s_HsTierName[tier], g_HouseRows[idx][hrPrice], s_HsTierLevel[tier]);
	}
	else
	{
		format(text, 160, "Casa %d - %s\nProprieta' di %s\n%s", g_HouseRows[idx][hrId], s_HsTierName[tier], g_HouseRows[idx][hrOwnerName], g_HouseRows[idx][hrLocked] ? "Chiusa a chiave (/bussa)" : "Aperta (/entra)");
	}
	new color = free ? 0x66DD66FF : -2994689;
	if (g_HsLabel[idx] != 65535)
	{
		Lang_Update3DTextLabelText(g_HsLabel[idx], color, text);
	}
	else
	{
		g_HsLabel[idx] = Lang_Create3DTextLabel(text, color, g_HouseRows[idx][hrExtX], g_HouseRows[idx][hrExtY], (0.9 + g_HouseRows[idx][hrExtZ]), 12.0, 0, 0);
	}
	if (g_HsPickup[idx] >= 0)
	{
		DestroyPickup(g_HsPickup[idx]);
	}
	g_HsPickup[idx] = CreatePickup(free ? 1273 : 1272, 1, g_HouseRows[idx][hrExtX], g_HouseRows[idx][hrExtY], g_HouseRows[idx][hrExtZ], 0);
}

House_ResetRowState(idx)
{
	g_HouseRows[idx][hrLocked] = 1;
	g_HouseRows[idx][hrLuceOn] = 1;
	g_HouseRows[idx][hrGasOn] = 1;
	g_HouseRows[idx][hrLuceCut] = 0;
	g_HouseRows[idx][hrGasCut] = 0;
	g_HouseRows[idx][hrLuceMin] = 0;
	g_HouseRows[idx][hrGasMin] = 0;
	g_HouseRows[idx][hrCycleMin] = 0;
	g_HouseRows[idx][hrFridgeUnits] = 0;
	g_HouseRows[idx][hrFridgeOffMin] = 0;
	g_HouseRows[idx][hrKeyCount] = 0;
	g_HouseRows[idx][hrBills] = 0;
	for (new k = 0; k < 3; k++)
	{
		g_HouseRows[idx][hrKeyChars][k] = 0;
	}
}

House_SaveTick(idx)
{
	if (g_HouseRows[idx][hrOwnerId] <= 0)
	{
		return 0;
	}
	DB_SaveHouseTick(g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerId], g_HouseRows[idx][hrLuceMin], g_HouseRows[idx][hrGasMin], g_HouseRows[idx][hrCycleMin], g_HouseRows[idx][hrFridgeOffMin]);
}

House_ApplyInsideUi(playerid, idx)
{
	new bool:light = House_LightOn(idx);
	UI_HouseHudUpdate(playerid, light, House_GasOn(idx), g_HouseRows[idx][hrFridgeUnits], House_FridgeCap(idx));
	if (light)
	{
		UI_HouseDarkHide(playerid);
	}
	else
	{
		UI_HouseDarkShow(playerid);
	}
}

House_RefreshInside(idx)
{
	new id = g_HouseRows[idx][hrId];
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_HPl[p][hpInsideHouse] == id && Util_IsPlaying(p))
		{
			House_ApplyInsideUi(p, idx);
		}
	}
}

House_DoEnter(playerid, idx)
{
	new k = House_IntIdx(idx), id = g_HouseRows[idx][hrId];
	SetPlayerInterior(playerid, s_HsIntId[k]);
	SetPlayerVirtualWorld(playerid, id + 1000);
	SetPlayerPos(playerid, s_HsIntPos[k][0], s_HsIntPos[k][1], s_HsIntPos[k][2]);
	SetPlayerFacingAngle(playerid, s_HsIntPos[k][3]);
	SetCameraBehindPlayer(playerid);
	g_HPl[playerid][hpInsideHouse] = id;
	House_ApplyInsideUi(playerid, idx);
}

House_PlaceOutside(playerid, idx)
{
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerPos(playerid, g_HouseRows[idx][hrExtX], g_HouseRows[idx][hrExtY], g_HouseRows[idx][hrExtZ]);
	SetPlayerFacingAngle(playerid, g_HouseRows[idx][hrExtA]);
	SetCameraBehindPlayer(playerid);
	House_ClearInside(playerid);
}

House_EvictAll(idx, const reason[])
{
	new id = g_HouseRows[idx][hrId];
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_HPl[p][hpInsideHouse] != id || !IsPlayerConnected(p))
		{
			continue;
		}
		House_PlaceOutside(p, idx);
		Util_Send(p, -1, reason);
	}
}

House_HungerSet(playerid, value, bool:save)
{
	value = clamp(value, 0, 100);
	new old = g_HPl[playerid][hpHunger];
	g_HPl[playerid][hpHunger] = value;
	UI_HungerHudUpdate(playerid, value);
	if (value < old)
	{
		if (old > 10 && value <= 10)
		{
			Util_Send(playerid, -10270721, "Sei debole per la fame: mangia qualcosa (/mangia, /usa, /compra).");
		}
		else if (old > 20 && value <= 20)
		{
			Util_Send(playerid, -2994689, "Hai fame: pensa a mangiare qualcosa.");
		}
	}
	if (save && g_HPl[playerid][hpHungerState] == 2 && gPlayer[playerid][pCharId] > 0)
	{
		DB_SaveHunger(gPlayer[playerid][pCharId], value);
	}
}

House_CapHealthIfWeak(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 2 || g_HPl[playerid][hpHunger] > 10)
	{
		return 0;
	}
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	if ((hp > float(70)))
	{
		SetPlayerHealth(playerid, float(70));
	}
}

House_BeginFoodLoad(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 0 || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	g_HPl[playerid][hpHungerState] = 1;
	DB_LoadFood(playerid, gPlayer[playerid][pCharId]);
}

bool:House_FoodReady(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 2)
	{
		Util_Send(playerid, -10270721, "I tuoi dati sul cibo non sono ancora stati caricati: riprova tra poco.");
		return 0;
	}
	return 1;
}

House_NotifyBills(playerid)
{
	new charId = gPlayer[playerid][pCharId];
	for (new i = 0; i < g_HouseCount; i++)
	{
		if (g_HouseRows[i][hrOwnerId] != charId)
		{
			continue;
		}
		if (g_HouseRows[i][hrBills] > 0)
		{
			SendClientMessage(playerid, -10270721, "Casa %d: hai %d bollette non pagate (/bollette).", g_HouseRows[i][hrId], g_HouseRows[i][hrBills]);
		}
		if (g_HouseRows[i][hrLuceCut])
		{
			SendClientMessage(playerid, -10270721, "Casa %d: la luce e' stata staccata per morosita'.", g_HouseRows[i][hrId]);
		}
		if (g_HouseRows[i][hrGasCut])
		{
			SendClientMessage(playerid, -10270721, "Casa %d: il gas e' stato staccato per morosita'.", g_HouseRows[i][hrId]);
		}
	}
}

public House_OnDBFoodLoaded(playerid, hunger, groceries)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid) || g_HPl[playerid][hpHungerState] != 1)
	{
		return 1;
	}
	if (hunger == -1 || groceries == -1)
	{
		g_HPl[playerid][hpHungerState] = 0;
		return 1;
	}
	g_HPl[playerid][hpHunger] = clamp(hunger, 0, 100);
	g_HPl[playerid][hpHungerState] = 2;
	g_HPl[playerid][hpHungerAccumMs] = 0;
	if (Util_IsPlaying(playerid))
	{
		UI_HungerHudUpdate(playerid, g_HPl[playerid][hpHunger]);
		if (g_HPl[playerid][hpCapHealthOnLoad])
		{
			House_CapHealthIfWeak(playerid);
		}
		House_NotifyBills(playerid);
	}
	g_HPl[playerid][hpCapHealthOnLoad] = 0;
	return 1;
}

House_UtilText(bool:on, bool:cut, out[], size)
{
	if (cut)
	{
		format(out, size, "STACCATA");
	}
	else
	{
		format(out, size, "%s", on ? "ON" : "OFF");
	}
}

House_BillSum(playerid, kind, &count)
{
	new sum = 0;
	count = 0;
	for (new i = 0; i < g_HPl[playerid][hpBillCount] && i < 8; i++)
	{
		if (g_HouseBills[playerid][i][hbKind] != kind)
		{
			continue;
		}
		sum += g_HouseBills[playerid][i][hbAmount];
		count++;
	}
	return sum;
}

House_PushUi(playerid, idx)
{
	new id = g_HouseRows[idx][hrId];
	UI_HouseDataSet(playerid, id, House_Role(idx, playerid), House_Tier(idx), House_IntIdx(idx), g_HouseRows[idx][hrLocked], House_UtilState(g_HouseRows[idx][hrLuceOn], g_HouseRows[idx][hrLuceCut]), House_UtilState(g_HouseRows[idx][hrGasOn], g_HouseRows[idx][hrGasCut]), g_HouseRows[idx][hrFridgeUnits], Inv_Count(playerid, 1), g_HouseRows[idx][hrKeyCount], gPlayer[playerid][pLevel], gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
	new lightDue = 0, lightCnt = 0, gasDue = 0, gasCnt = 0;
	if (g_HPl[playerid][hpBillsHouseId] == id)
	{
		lightDue = House_BillSum(playerid, 0, lightCnt);
		gasDue = House_BillSum(playerid, 1, gasCnt);
	}
	UI_HouseBillsSet(playerid, lightDue, lightCnt, gasDue, gasCnt);
}

House_OpenMenu(playerid, idx)
{
	if (House_Role(idx, playerid) == 1)
	{
		g_HPl[playerid][hpPendingOp] = 9;
		g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
		g_HPl[playerid][hpBillIntent] = 1;
		DB_LoadBills(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId]);
		return 0;
	}
	g_HPl[playerid][hpBillsHouseId] = 0;
	House_PushUi(playerid, idx);
	UI_HouseMenuShow(playerid);
}

House_OpenBills(playerid, idx)
{
	g_HPl[playerid][hpPendingOp] = 9;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpBillIntent] = 2;
	DB_LoadBills(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId]);
}

public House_OnDBBillsLoaded(playerid, houseId, count)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 9 || !House_OpMatches(playerid, houseId, count))
	{
		return 1;
	}
	new intent = g_HPl[playerid][hpBillIntent];
	g_HPl[playerid][hpPendingOp] = 0;
	new idx = House_Find(houseId);
	if (count == -1 || idx == -1 || !House_PlayerOk(playerid))
	{
		g_HPl[playerid][hpBillsHouseId] = 0;
		Util_Send(playerid, -10270721, "Impossibile leggere le bollette (errore database). Riprova piu' tardi.");
		return 1;
	}
	g_HPl[playerid][hpBillsHouseId] = houseId;
	g_HPl[playerid][hpBillCount] = count;
	House_PushUi(playerid, idx);
	if (intent == 2)
	{
		UI_HouseBillsShow(playerid);
	}
	else
	{
		UI_HouseMenuShow(playerid);
	}
	return 1;
}

House_DoBuy(playerid, idx, payWithBank)
{
	if (g_HPl[playerid][hpPendingOp] != 0 || g_HsBusy[idx])
	{
		Util_Send(playerid, -10270721, "Operazione in corso: attendi.");
		return 0;
	}
	if (House_FindNear(playerid) != idx || GetPlayerState(playerid) != 1)
	{
		Util_Send(playerid, -10270721, "Devi essere a piedi davanti alla porta della casa.");
		return 0;
	}
	if (g_HouseRows[idx][hrOwnerId] > 0)
	{
		Util_Send(playerid, -10270721, "Questa casa non e' in vendita.");
		return 0;
	}
	if (payWithBank != 0 && payWithBank != 1)
	{
		Util_Send(playerid, -10270721, "Metodo di pagamento non valido.");
		return 0;
	}
	new tier = House_Tier(idx), price = g_HouseRows[idx][hrPrice], level = gPlayer[playerid][pLevel], charId = gPlayer[playerid][pCharId];
	if (price <= 0 || charId <= 0)
	{
		Util_Send(playerid, -10270721, "Acquisto non disponibile.");
		return 0;
	}
	if (level < s_HsTierLevel[tier])
	{
		SendClientMessage(playerid, -10270721, "Questa casa richiede il livello %d.", s_HsTierLevel[tier]);
		return 0;
	}
	new maxHouses = House_MaxForLevel(level);
	if (House_OwnedCount(charId) >= maxHouses)
	{
		SendClientMessage(playerid, -10270721, "Hai raggiunto il limite di case per il tuo livello (%d).", maxHouses);
		return 0;
	}
	if (payWithBank == 1 ? !Money_CanBank(playerid, price) : !Money_Can(playerid, price))
	{
		Util_Send(playerid, -10270721, "Fondi insufficienti con questo metodo di pagamento.");
		return 0;
	}
	g_HsBusy[idx] = 1;
	g_HPl[playerid][hpPendingOp] = 1;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpPayWithBank] = payWithBank;
	g_HPl[playerid][hpOpAmount] = price;
	Util_Send(playerid, -1, "Pratica di acquisto in corso...");
	DB_BuyHouse(playerid, g_HouseRows[idx][hrId], charId, price, maxHouses);
}

public House_OnDBBought(playerid, houseId, affected)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new idx = -1;
	if (g_HPl[playerid][hpPendingOp] != 1 || !House_OpMatches(playerid, houseId, affected))
	{
		if (affected == 1)
		{
			Log_Write("HOUSE", "ANOMALIA: esito acquisto inatteso con riga assegnata: verificare a mano");
		}
		return 1;
	}
	idx = House_Find(houseId);
	new bank = g_HPl[playerid][hpPayWithBank], price = g_HPl[playerid][hpOpAmount], charId = gPlayer[playerid][pCharId];
	g_HPl[playerid][hpPendingOp] = 0;
	if (idx != -1)
	{
		g_HsBusy[idx] = 0;
	}
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "Acquisto non riuscito (errore database). Nessun addebito.");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "Acquisto non possibile: la casa non e' piu' disponibile o hai raggiunto il limite. Nessun addebito.");
		return 1;
	}
	new bool:ok = House_PlayerOk(playerid) && gPlayer[playerid][pLevel] >= s_HsTierLevel[House_Tier(idx)];
	if (ok)
	{
		ok = bank == 1 ? Money_TakeBank(playerid, price, "house", "") : Money_Take(playerid, price);
	}
	if (!ok)
	{
		g_HsBusy[idx] = 1;
		g_HPl[playerid][hpPendingOp] = 3;
		g_HPl[playerid][hpOpHouseId] = houseId;
		DB_SellHouse(playerid, houseId, charId);
		Util_Send(playerid, -10270721, "Acquisto annullato: requisiti o fondi non piu' validi. Nessun addebito.");
		return 1;
	}
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	g_HouseRows[idx][hrOwnerId] = charId;
	strmid(g_HouseRows[idx][hrOwnerName], name, 0, strlen(name), 25);
	g_HouseRows[idx][hrPricePaid] = price;
	House_ResetRowState(idx);
	House_RefreshWorld(idx);
	Char_Save(playerid);
	SendClientMessage(playerid, 0x66DD66FF, "Hai acquistato la casa %d per $%d (%s). Usa /entra per entrare, /casa per gestirla.", houseId, price, bank == 1 ? "banca" : "contanti");
	format(msg, 256, "ACQUISTO %s: casa %d $%d (%s)", name, houseId, price, bank == 1 ? "banca" : "contanti");
	Log_Write("HOUSE", msg);
	return 1;
}

House_SellRefund(idx)
{
	new base = g_HouseRows[idx][hrPricePaid] > 0 ? g_HouseRows[idx][hrPricePaid] : g_HouseRows[idx][hrPrice];
	return base * 70 / 100;
}

House_DoSell(playerid, idx, bool:confirmed)
{
	if (g_HPl[playerid][hpPendingOp] != 0 || g_HsBusy[idx])
	{
		Util_Send(playerid, -10270721, "Operazione in corso: attendi.");
		return 0;
	}
	if (g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "Solo il proprietario puo' vendere la casa.");
		return 0;
	}
	if (House_Context(playerid) != idx)
	{
		Util_Send(playerid, -10270721, "Devi essere dentro la casa o davanti alla sua porta.");
		return 0;
	}
	if (g_HouseRows[idx][hrBills] > 0)
	{
		Util_Send(playerid, -10270721, "Non puoi vendere la casa con bollette non pagate (/bollette).");
		return 0;
	}
	new refund = House_SellRefund(idx);
	if (refund <= 0 || refund > Money_BankRoom(playerid))
	{
		Util_Send(playerid, -10270721, "Il tuo conto bancario non puo' ricevere l'importo: vendita rifiutata.");
		return 0;
	}
	if (!confirmed)
	{
		SendClientMessage(playerid, -2994689, "Vendita casa %d: ricevi $%d (%d%%) in banca. Utenze, chiavi e frigo vengono azzerati.", g_HouseRows[idx][hrId], refund, 70);
		Util_Send(playerid, 0x99CCFFFF, "Per confermare: /casa vendi conferma");
		return 0;
	}
	g_HsBusy[idx] = 1;
	g_HPl[playerid][hpPendingOp] = 2;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpOpAmount] = refund;
	Util_Send(playerid, -1, "Vendita in corso...");
	DB_SellHouse(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId]);
}

public House_OnDBSold(playerid, houseId, affected)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new idx = -1;
	if (g_HPl[playerid][hpPendingOp] == 3 && House_OpMatches(playerid, houseId, affected))
	{
		idx = House_Find(houseId);
		g_HPl[playerid][hpPendingOp] = 0;
		if (idx != -1)
		{
			g_HsBusy[idx] = 0;
		}
		if (affected != 1)
		{
			Log_Write("HOUSE", "ANOMALIA: rollback acquisto non riuscito: casa da verificare a mano");
		}
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 2 || !House_OpMatches(playerid, houseId, affected))
	{
		if (affected == 1)
		{
			Log_Write("HOUSE", "ANOMALIA: esito vendita inatteso con casa liberata: verificare a mano");
		}
		return 1;
	}
	idx = House_Find(houseId);
	new refund = g_HPl[playerid][hpOpAmount], charId = gPlayer[playerid][pCharId];
	g_HPl[playerid][hpPendingOp] = 0;
	if (idx != -1)
	{
		g_HsBusy[idx] = 0;
	}
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "Vendita non riuscita (errore database). Nessun accredito.");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "Vendita non eseguita: la casa non risulta piu' di tua proprieta'. Nessun accredito.");
		return 1;
	}
	if (!Money_GiveBank(playerid, refund, "refund", ""))
	{
		DB_RestoreSoldHouse(houseId, charId, g_HouseRows[idx][hrPricePaid]);
		Util_Send(playerid, -10270721, "Vendita annullata: il conto bancario non puo' ricevere l'importo. Nessun accredito.");
		Log_Write("HOUSE", "vendita annullata: accredito impossibile, proprieta' ripristinata");
		return 1;
	}
	House_EvictAll(idx, "La casa e' stata venduta: sei stato portato fuori.");
	g_HouseRows[idx][hrOwnerId] = 0;
	g_HouseRows[idx][hrOwnerName][0] = 0;
	g_HouseRows[idx][hrPricePaid] = 0;
	House_ResetRowState(idx);
	House_RefreshWorld(idx);
	Char_Save(playerid);
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	SendClientMessage(playerid, 0x66DD66FF, "Hai venduto la casa %d: $%d accreditati in banca.", houseId, refund);
	format(msg, 256, "VENDITA %s: casa %d rimborso $%d", name, houseId, refund);
	Log_Write("HOUSE", msg);
	return 1;
}

House_ToggleLock(playerid, idx)
{
	if (g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "Solo il proprietario puo' usare la serratura.");
		return 0;
	}
	if (House_Context(playerid) != idx)
	{
		Util_Send(playerid, -10270721, "Devi essere dentro la casa o davanti alla sua porta.");
		return 0;
	}
	g_HouseRows[idx][hrLocked] = !g_HouseRows[idx][hrLocked];
	DB_SetHouseLock(g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerId], g_HouseRows[idx][hrLocked]);
	House_RefreshWorld(idx);
	SendClientMessage(playerid, 0x66DD66FF, "Porta della casa %d: %s.", g_HouseRows[idx][hrId], g_HouseRows[idx][hrLocked] ? "chiusa a chiave" : "aperta");
}

bool:House_SetUtility(playerid, idx, utility, bool:on)
{
	if (House_FindInside(playerid) != idx)
	{
		Util_Send(playerid, -10270721, "Devi essere dentro la casa.");
		return 0;
	}
	new role = House_Role(idx, playerid);
	if (role != 1 && !(utility == 0 && role == 2))
	{
		Util_Send(playerid, -10270721, utility == 1 ? "Solo il proprietario puo' gestire il gas." : "Non hai accesso alle utenze di questa casa.");
		return 0;
	}
	new bool:cut = utility == 1 ? g_HouseRows[idx][hrGasCut] : g_HouseRows[idx][hrLuceCut];
	if (cut)
	{
		Util_Send(playerid, -10270721, "Utenza staccata per morosita': paga le bollette (/bollette).");
		return 0;
	}
	if (utility == 1)
	{
		g_HouseRows[idx][hrGasOn] = on;
	}
	else
	{
		g_HouseRows[idx][hrLuceOn] = on;
	}
	DB_SetUtility(g_HouseRows[idx][hrId], gPlayer[playerid][pCharId], utility, on);
	House_RefreshInside(idx);
	SendClientMessage(playerid, 0x66DD66FF, "%s %s.", utility == 1 ? "Gas" : "Luce", utility == 1 ? (on ? "acceso" : "spento") : (on ? "accesa" : "spenta"));
	return 1;
}

House_CmdToggleUtility(playerid, utility)
{
	if (!House_CmdGuard(playerid, 1))
	{
		return 0;
	}
	new idx = House_FindInside(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "Devi essere dentro una casa.");
		return 0;
	}
	new bool:cur = utility == 1 ? g_HouseRows[idx][hrGasOn] : g_HouseRows[idx][hrLuceOn];
	House_SetUtility(playerid, idx, utility, !cur);
}

House_ReopenFridge(playerid)
{
	new idx = House_FindInside(playerid);
	if (idx == -1)
	{
		return 0;
	}
	House_PushUi(playerid, idx);
	UI_HouseFridgeShow(playerid);
}

House_FridgeChanged(playerid, idx, delta)
{
	DB_FridgeAdd(g_HouseRows[idx][hrId], gPlayer[playerid][pCharId], delta);
	House_RefreshInside(idx);
}

House_FoodContext(playerid)
{
	new idx = House_FindInside(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "Devi essere dentro la tua casa (o di un amico che ti ha dato la chiave).");
		return -1;
	}
	if (!House_HasAccess(idx, playerid))
	{
		Util_Send(playerid, -10270721, "Non puoi usare il frigo di questa casa.");
		return -1;
	}
	return idx;
}

House_EatCold(playerid)
{
	new idx = House_FoodContext(playerid);
	if (idx == -1 || !House_FoodReady(playerid))
	{
		return 0;
	}
	if (g_HsFridgeBusy[idx])
	{
		Util_Send(playerid, -10270721, "Qualcuno sta usando il frigo: attendi un attimo.");
		return 0;
	}
	if (g_HouseRows[idx][hrFridgeUnits] <= 0)
	{
		Util_Send(playerid, -10270721, "Il frigo e' vuoto.");
		return 0;
	}
	if (g_HPl[playerid][hpHunger] >= 100)
	{
		Util_Send(playerid, -10270721, "Non hai fame.");
		return 0;
	}
	g_HouseRows[idx][hrFridgeUnits]--;
	House_FridgeChanged(playerid, idx, -1);
	House_HungerSet(playerid, g_HPl[playerid][hpHunger] + 25, 1);
	SendClientMessage(playerid, 0x66DD66FF, "Mangi qualcosa di freddo dal frigo. Sazieta': %d%%.", g_HPl[playerid][hpHunger]);
}

House_StartCook(playerid)
{
	new idx = House_FoodContext(playerid);
	if (idx == -1 || !House_FoodReady(playerid))
	{
		return 0;
	}
	if (g_HPl[playerid][hpCooking])
	{
		Util_Send(playerid, -10270721, "Stai gia' cucinando.");
		return 0;
	}
	if (g_HPl[playerid][hpCookCooldown] != 0 && g_HPl[playerid][hpCookCooldown] - GetTickCount() > 0)
	{
		Util_Send(playerid, -10270721, "Hai gia' un piatto caldo pronto: usa /mangia.");
		return 0;
	}
	if (!House_GasOn(idx))
	{
		Util_Send(playerid, -10270721, "Il gas non e' attivo: non puoi cucinare.");
		return 0;
	}
	if (g_HouseRows[idx][hrFridgeUnits] <= 0)
	{
		Util_Send(playerid, -10270721, "Il frigo e' vuoto.");
		return 0;
	}
	g_HPl[playerid][hpCooking] = 1;
	g_HPl[playerid][hpCookHouseId] = g_HouseRows[idx][hrId];
	SetTimerEx("House_OnCookDone", 6000, 0, "dd", playerid, g_HPl[playerid][hpSession]);
	Util_Send(playerid, -1, "Inizi a cucinare...");
}

public House_OnCookDone(playerid, session)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_HPl[playerid][hpSession] != session || session == 0 || !g_HPl[playerid][hpCooking])
	{
		return 1;
	}
	g_HPl[playerid][hpCooking] = 0;
	new idx = House_FindInside(playerid);
	if (idx == -1 || g_HouseRows[idx][hrId] != g_HPl[playerid][hpCookHouseId] || !House_PlayerOk(playerid) || !House_GasOn(idx) || g_HouseRows[idx][hrFridgeUnits] <= 0 || g_HsFridgeBusy[idx])
	{
		Util_Send(playerid, -10270721, "Cottura annullata.");
		return 1;
	}
	g_HouseRows[idx][hrFridgeUnits]--;
	House_FridgeChanged(playerid, idx, -1);
	g_HPl[playerid][hpCookCooldown] = GetTickCount() + 300000;
	if (g_HPl[playerid][hpCookCooldown] == 0)
	{
		g_HPl[playerid][hpCookCooldown] = 1;
	}
	SendClientMessage(playerid, 0x66DD66FF, "Il piatto e' pronto: usa /mangia entro %d minuti.", 5);
	return 1;
}

House_EatPlate(playerid)
{
	if (!House_FoodReady(playerid))
	{
		return 0;
	}
	if (g_HPl[playerid][hpCookCooldown] == 0 || g_HPl[playerid][hpCookCooldown] - GetTickCount() <= 0)
	{
		g_HPl[playerid][hpCookCooldown] = 0;
		Util_Send(playerid, -10270721, "Non hai un piatto caldo pronto. Usa /cucina in casa oppure /mangia freddo.");
		return 0;
	}
	if (g_HPl[playerid][hpHunger] >= 100)
	{
		Util_Send(playerid, -10270721, "Non hai fame.");
		return 0;
	}
	g_HPl[playerid][hpCookCooldown] = 0;
	House_HungerSet(playerid, g_HPl[playerid][hpHunger] + 45, 1);
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	hp += float(5);
	if ((hp > 100.0))
	{
		hp = 100.0;
	}
	SetPlayerHealth(playerid, hp);
	SendClientMessage(playerid, 0x66DD66FF, "Mangi il piatto caldo. Sazieta': %d%%.", g_HPl[playerid][hpHunger]);
}

House_Store(playerid, n)
{
	new idx = House_FindInside(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "Puoi riporre la spesa solo nel frigo di casa tua.");
		return 0;
	}
	if (g_HsFridgeBusy[idx])
	{
		Util_Send(playerid, -10270721, "Qualcuno sta usando il frigo: attendi un attimo.");
		return 0;
	}
	if (n < 1 || n > Inv_Count(playerid, 1))
	{
		Util_Send(playerid, -10270721, "Non hai tutta quella spesa con te.");
		return 0;
	}
	new cap = House_FridgeCap(idx);
	if (g_HouseRows[idx][hrFridgeUnits] + n > cap)
	{
		SendClientMessage(playerid, -10270721, "Nel frigo c'e' posto solo per altre %d porzioni.", cap - g_HouseRows[idx][hrFridgeUnits]);
		return 0;
	}
	g_HsFridgeBusy[idx] = 1;
	if (!Inv_FridgeMove(playerid, g_HouseRows[idx][hrId], 1, n, cap))
	{
		g_HsFridgeBusy[idx] = 0;
	}
}

House_Take(playerid, n)
{
	new idx = House_FoodContext(playerid);
	if (idx == -1)
	{
		return 0;
	}
	if (g_HsFridgeBusy[idx])
	{
		Util_Send(playerid, -10270721, "Qualcuno sta usando il frigo: attendi un attimo.");
		return 0;
	}
	if (n < 1 || n > g_HouseRows[idx][hrFridgeUnits])
	{
		Util_Send(playerid, -10270721, "Non ci sono tante porzioni nel frigo.");
		return 0;
	}
	if (Inv_Count(playerid, 1) + n > 12)
	{
		SendClientMessage(playerid, -10270721, "Puoi portare al massimo %d porzioni.", 12);
		return 0;
	}
	g_HsFridgeBusy[idx] = 1;
	if (!Inv_FridgeMove(playerid, g_HouseRows[idx][hrId], 2, n, House_FridgeCap(idx)))
	{
		g_HsFridgeBusy[idx] = 0;
	}
}

public House_InvFridgeResult(playerid, houseId, direction, qty, result)
{
	new idx = House_Find(houseId);
	if (idx == -1)
	{
		return 1;
	}
	g_HsFridgeBusy[idx] = 0;
	if (!Inv_ValidPlayer(playerid))
	{
		return 1;
	}
	switch (result)
	{
		case 1:
		{
			if (direction == 1)
			{
				g_HouseRows[idx][hrFridgeUnits] = min(g_HouseRows[idx][hrFridgeUnits] + qty, House_FridgeCap(idx));
			}
			else
			{
				g_HouseRows[idx][hrFridgeUnits] = max(g_HouseRows[idx][hrFridgeUnits] - qty, 0);
			}
			House_RefreshInside(idx);
			if (direction == 1)
			{
				SendClientMessage(playerid, 0x66DD66FF, "Hai riposto %d porzioni nel frigo (%d/%d).", qty, g_HouseRows[idx][hrFridgeUnits], House_FridgeCap(idx));
			}
			else
			{
				SendClientMessage(playerid, 0x66DD66FF, "Hai preso %d porzioni dal frigo (max %d con te).", qty, 12);
			}
			if (House_FindInside(playerid) == idx)
			{
				House_ReopenFridge(playerid);
			}
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "Non hai abbastanza spazio nell'inventario (o hai gia' il massimo di porzioni).");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, direction == 1 ? "Non hai tutta quella spesa con te." : "Non ci sono tante porzioni nel frigo.");
		}
		case 6:
		{
			Util_Send(playerid, -10270721, "Il frigo non ha abbastanza posto.");
		}
		case 4:
		{
			Util_Send(playerid, -10270721, "Non puoi usare il frigo di questa casa.");
		}
		case -1:
		{
			Util_Send(playerid, -10270721, "Errore database: stato del frigo non verificabile, riprova piu' tardi.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "Impossibile spostare le porzioni.");
		}
	}
	return 1;
}

public House_InvFridgeAbort(houseId)
{
	new idx = House_Find(houseId);
	if (idx == -1)
	{
		return 1;
	}
	g_HsFridgeBusy[idx] = 1;
	SetTimerEx("House_FridgeReloadLater", 1500, 0, "d", houseId);
	return 1;
}

public House_FridgeReloadLater(houseId)
{
	DB_ReloadFridge(houseId);
	return 1;
}

public House_OnDBFridgeReloaded(houseId, units)
{
	new idx = House_Find(houseId);
	if (idx == -1)
	{
		return 1;
	}
	if (units != -1 && units >= 0)
	{
		g_HouseRows[idx][hrFridgeUnits] = units;
		House_RefreshInside(idx);
	}
	g_HsFridgeBusy[idx] = 0;
	return 1;
}

public House_InvHunger(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || g_HPl[playerid][hpHungerState] != 2)
	{
		return -1;
	}
	return g_HPl[playerid][hpHunger];
}

public House_InvHungerAdd(playerid, amount)
{
	if (House_InvHunger(playerid) < 0)
	{
		return -1;
	}
	House_HungerSet(playerid, g_HPl[playerid][hpHunger] + amount, 1);
	return g_HPl[playerid][hpHunger];
}

House_CmdFrigo(playerid, const params[])
{
	if (!House_CmdGuard(playerid, 0))
	{
		return 0;
	}
	new idx = House_FoodContext(playerid);
	if (idx == -1)
	{
		return 0;
	}
	new pos = 0, sub[32], arg[32], n = 0;
	Util_NextToken(params, pos, sub, 32);
	if (sub[0] == 0)
	{
		SendClientMessage(playerid, -1, "Frigo: %d/%d porzioni (con te: %d/%d).", g_HouseRows[idx][hrFridgeUnits], House_FridgeCap(idx), Inv_Count(playerid, 1), 12);
		if (!House_LightOn(idx))
		{
			Util_Send(playerid, -10270721, "Luce spenta: il frigo non raffredda e le scorte si deteriorano.");
		}
		Util_Send(playerid, 0x99CCFFFF, "Uso: /frigo metti [n] (solo proprietario) · /frigo prendi [n] (proprietario o ospite)");
		return 0;
	}
	new bool:store = !strcmp(sub, "metti", 1, 0x7FFFFFFF);
	if (!store && strcmp(sub, "prendi", 1, 0x7FFFFFFF))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /frigo metti [n] · /frigo prendi [n]");
		return 0;
	}
	Util_NextToken(params, pos, arg, 32);
	n = 1;
	if (arg[0] != 0 && (!Util_ParseInt(arg, n) || n < 1 || n > 40))
	{
		Util_Send(playerid, -10270721, "Quantita' non valida.");
		return 0;
	}
	if (store)
	{
		House_Store(playerid, n);
	}
	else
	{
		House_Take(playerid, n);
	}
}

House_CmdMangia(playerid, const params[])
{
	if (!House_CmdGuard(playerid, 0))
	{
		return 0;
	}
	new pos = 0, sub[32];
	Util_NextToken(params, pos, sub, 32);
	if (sub[0] == 0)
	{
		House_EatPlate(playerid);
		return 0;
	}
	if (!strcmp(sub, "freddo", 1, 0x7FFFFFFF))
	{
		House_EatCold(playerid);
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "Uso: /mangia (piatto caldo dopo /cucina) · /mangia freddo");
}

House_CmdCucina(playerid)
{
	if (!House_CmdGuard(playerid, 0))
	{
		return 0;
	}
	House_StartCook(playerid);
}

House_CmdCompra(playerid, const params[])
{
	if (!House_CmdGuard(playerid, 0))
	{
		return 0;
	}
	if (!House_NearShop(playerid))
	{
		Util_Send(playerid, -10270721, "Devi essere in un supermercato.");
		return 0;
	}
	if (GetPlayerState(playerid) != 1)
	{
		Util_Send(playerid, -10270721, "Scendi dal veicolo.");
		return 0;
	}
	new pos = 0, sub[32], arg[32], n = 1;
	Util_NextToken(params, pos, sub, 32);
	if (sub[0] == 0)
	{
		Inv_ShopList(playerid);
		return 0;
	}
	new itemId = Inv_ParseItem(sub);
	if (itemId == 10)
	{
		itemId = 0;
	}
	if (itemId < 1)
	{
		Inv_ShopList(playerid);
		return 0;
	}
	Util_NextToken(params, pos, arg, 32);
	if (arg[0] != 0 && (!Util_ParseInt(arg, n) || n < 1 || n > 12))
	{
		Util_Send(playerid, -10270721, "Quantita' non valida.");
		return 0;
	}
	Inv_Buy(playerid, itemId, n);
}

House_BillAmount(tariff, minutesOn)
{
	new minutes = clamp(minutesOn, 0, 180);
	new amount = tariff * minutes / 180;
	new floorAmount = tariff * 25 / 100;
	if (amount < floorAmount)
	{
		amount = floorAmount;
	}
	return amount < 1 ? 1 : amount;
}

House_CloseCycle(idx)
{
	if (!DB_IsReady())
	{
		return 0;
	}
	new id = g_HouseRows[idx][hrId], tier = House_Tier(idx);
	new luceAmt = House_BillAmount(s_HsBillLuce[tier], g_HouseRows[idx][hrLuceMin]);
	new gasAmt = House_BillAmount(s_HsBillGas[tier], g_HouseRows[idx][hrGasMin]);
	g_HouseRows[idx][hrLuceMin] = 0;
	g_HouseRows[idx][hrGasMin] = 0;
	g_HouseRows[idx][hrCycleMin] = 0;
	House_SaveTick(idx);
	DB_AgeBills(id, 2);
	DB_AddBill(id, 0, luceAmt, 4);
	DB_AddBill(id, 1, gasAmt, 4);
}

public House_OnDBBillAdded(houseId, kind, billId, amount)
{
	new idx = House_Find(houseId);
	if (billId == -1 || idx == -1)
	{
		Log_Write("HOUSE", "emissione bolletta fallita (errore database)");
		return 1;
	}
	if (billId <= 0)
	{
		return 1;
	}
	g_HouseRows[idx][hrBills]++;
	new owner = House_OwnerPlayer(idx);
	if (owner != 65535 && Util_IsPlaying(owner))
	{
		SendClientMessage(owner, -2994689, "Casa %d: nuova bolletta %s di $%d (/bollette).", houseId, kind == 1 ? "del gas" : "della luce", amount);
	}
	return 1;
}

House_CutUtil(idx, kind)
{
	new id = g_HouseRows[idx][hrId];
	if (kind == 1)
	{
		if (g_HouseRows[idx][hrGasCut])
		{
			return 0;
		}
		g_HouseRows[idx][hrGasCut] = 1;
		g_HouseRows[idx][hrGasOn] = 0;
	}
	else
	{
		if (g_HouseRows[idx][hrLuceCut])
		{
			return 0;
		}
		g_HouseRows[idx][hrLuceCut] = 1;
		g_HouseRows[idx][hrLuceOn] = 0;
	}
	DB_CutUtility(id, kind);
	House_RefreshInside(idx);
	new owner = House_OwnerPlayer(idx);
	if (owner != 65535)
	{
		SendClientMessage(owner, -10270721, "Casa %d: %s staccat%s per bollette non pagate. Paga con /bollette (+$%d di riattivazione).", id, kind == 1 ? "il gas" : "la luce", kind == 1 ? "o" : "a", 40);
	}
	new msg[256];
	format(msg, 256, "DISTACCO casa %d: %s", id, kind == 1 ? "gas" : "luce");
	Log_Write("HOUSE", msg);
}

public House_OnDBBillsAged(houseId, overdueMask)
{
	if (overdueMask == -1 || houseId <= 0)
	{
		Log_Write("HOUSE", "invecchiamento bollette fallito (errore database)");
		return 1;
	}
	new idx = House_Find(houseId);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] <= 0)
	{
		return 1;
	}
	for (new kind = 0; kind < 2; kind++)
	{
		if (overdueMask & 1 << kind)
		{
			House_CutUtil(idx, kind);
		}
	}
	return 1;
}

House_PayNext(playerid)
{
	new idx = House_Find(g_HPl[playerid][hpOpHouseId]);
	new mask = g_HPl[playerid][hpBillKindMask];
	if (idx == -1 || mask == 0)
	{
		g_HPl[playerid][hpPendingOp] = 0;
		return 0;
	}
	new kind = mask & 1 ? 0 : 1, cnt = 0;
	new sum = House_BillSum(playerid, kind, cnt);
	new bool:cut = kind == 1 ? g_HouseRows[idx][hrGasCut] : g_HouseRows[idx][hrLuceCut];
	g_HPl[playerid][hpBillKind] = kind;
	DB_PayBills(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId], kind, sum, cut);
}

public UI_OnHouseBillsPayAll(playerid, houseId, payWithBank)
{
	if (!House_PlayerOk(playerid) || !g_HsReady)
	{
		return 1;
	}
	new idx = House_Find(houseId);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "Solo il proprietario puo' pagare le bollette.");
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "Operazione in corso: attendi.");
		return 1;
	}
	if (g_HPl[playerid][hpBillsHouseId] != houseId)
	{
		Util_Send(playerid, -10270721, "Riapri /bollette per aggiornare l'elenco.");
		return 1;
	}
	if (payWithBank != 0 && payWithBank != 1)
	{
		return 1;
	}
	new total = 0, mask = 0;
	for (new kind = 0; kind < 2; kind++)
	{
		new cnt = 0, sum = House_BillSum(playerid, kind, cnt);
		new bool:cut = kind == 1 ? g_HouseRows[idx][hrGasCut] : g_HouseRows[idx][hrLuceCut];
		if (sum <= 0 && !cut)
		{
			continue;
		}
		mask |= 1 << kind;
		total += sum + (cut ? 40 : 0);
	}
	if (mask == 0 || total <= 0)
	{
		Util_Send(playerid, 0x66DD66FF, "Nessuna bolletta da pagare.");
		return 1;
	}
	if (payWithBank == 1 ? !Money_CanBank(playerid, total) : !Money_Can(playerid, total))
	{
		Util_Send(playerid, -10270721, "Fondi insufficienti con questo metodo di pagamento.");
		return 1;
	}
	g_HPl[playerid][hpPendingOp] = 4;
	g_HPl[playerid][hpPayWithBank] = payWithBank;
	g_HPl[playerid][hpBillKindMask] = mask;
	g_HPl[playerid][hpBillPaidTotal] = 0;
	Util_Send(playerid, -1, "Pagamento in corso...");
	House_PayNext(playerid);
	return 1;
}

public House_OnDBBillsPaid(playerid, houseId, kind, paidAmount, restored)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 4 || !House_OpMatches(playerid, houseId, paidAmount))
	{
		return 1;
	}
	kind = g_HPl[playerid][hpBillKind];
	new idx = House_Find(houseId), charId = gPlayer[playerid][pCharId], bank = g_HPl[playerid][hpPayWithBank];
	if (paidAmount == -1 || idx == -1)
	{
		g_HPl[playerid][hpPendingOp] = 0;
		g_HPl[playerid][hpBillsHouseId] = 0;
		Util_Send(playerid, -10270721, "Pagamento non riuscito (errore database). Verifica lo stato con /bollette.");
		Log_Write("HOUSE", "pagamento bollette: errore database (verificare lo stato delle bollette della casa)");
		return 1;
	}
	if (paidAmount == 0 && restored == 0)
	{
		g_HPl[playerid][hpPendingOp] = 0;
		g_HPl[playerid][hpBillsHouseId] = 0;
		Util_Send(playerid, -10270721, "Le bollette sono cambiate o non risultano piu' dovute: riapri /bollette. Nessun addebito per questa utenza.");
		return 1;
	}
	new fee = restored != 0 ? 40 : 0, amount = paidAmount + fee, cnt = 0;
	House_BillSum(playerid, kind, cnt);
	if (amount <= 0 || !(bank == 1 ? Money_TakeBank(playerid, amount, "bill", "") : Money_Take(playerid, amount)))
	{
		if (paidAmount > 0)
		{
			DB_RevertBillPayment(houseId, charId, kind, restored != 0);
		}
		else if (restored != 0)
		{
			DB_CutUtility(houseId, kind);
		}
		g_HPl[playerid][hpPendingOp] = 0;
		g_HPl[playerid][hpBillsHouseId] = 0;
		Util_Send(playerid, -10270721, "Fondi insufficienti: pagamento annullato per questa utenza.");
		return 1;
	}
	g_HouseRows[idx][hrBills] = max(g_HouseRows[idx][hrBills] - cnt, 0);
	if (restored != 0)
	{
		if (kind == 1)
		{
			g_HouseRows[idx][hrGasCut] = 0;
			g_HouseRows[idx][hrGasOn] = 1;
		}
		else
		{
			g_HouseRows[idx][hrLuceCut] = 0;
			g_HouseRows[idx][hrLuceOn] = 1;
		}
		House_RefreshInside(idx);
	}
	Char_Save(playerid);
	g_HPl[playerid][hpBillPaidTotal] += amount;
	new msg[256], name[25];
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "BOLLETTE %s: casa %d %s $%d%s (%s)", name, houseId, kind == 1 ? "gas" : "luce", amount, restored != 0 ? " con riattivazione" : "", bank == 1 ? "banca" : "contanti");
	Log_Write("HOUSE", msg);
	g_HPl[playerid][hpBillKindMask] &= ~(1 << kind);
	if (g_HPl[playerid][hpBillKindMask] != 0)
	{
		House_PayNext(playerid);
		return 1;
	}
	g_HPl[playerid][hpPendingOp] = 0;
	g_HPl[playerid][hpBillsHouseId] = 0;
	SendClientMessage(playerid, 0x66DD66FF, "Bollette pagate: $%d addebitati (%s).", g_HPl[playerid][hpBillPaidTotal], bank == 1 ? "banca" : "contanti");
	return 1;
}

House_CmdBollette(playerid)
{
	if (!House_CmdGuard(playerid, 1))
	{
		return 0;
	}
	new idx = House_Context(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		new found = -1, n = 0;
		for (new i = 0; i < g_HouseCount; i++)
		{
			if (g_HouseRows[i][hrOwnerId] == gPlayer[playerid][pCharId])
			{
				found = i;
				n++;
			}
		}
		if (n == 0)
		{
			Util_Send(playerid, -10270721, "Non possiedi nessuna casa.");
			return 0;
		}
		if (n > 1)
		{
			Util_Send(playerid, -10270721, "Possiedi piu' case: entra o avvicinati alla porta di quella interessata.");
			return 0;
		}
		idx = found;
	}
	House_OpenBills(playerid, idx);
}

House_KeyContext(playerid)
{
	new idx = House_Context(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "Devi essere il proprietario, dentro la casa o davanti alla sua porta.");
		return -1;
	}
	return idx;
}

House_GiveKey(playerid, const token[])
{
	new idx = House_KeyContext(playerid), target = 0;
	if (idx == -1)
	{
		return 0;
	}
	if (!Util_ParsePlayerId(token, target) || !Util_IsPlaying(target) || target == playerid || gPlayer[target][pCharId] <= 0)
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 0;
	}
	if (!Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "Il giocatore e' troppo lontano.");
		return 0;
	}
	if (House_IsGuest(idx, gPlayer[target][pCharId]))
	{
		Util_Send(playerid, -10270721, "Quel giocatore ha gia' una chiave.");
		return 0;
	}
	if (g_HouseRows[idx][hrKeyCount] >= 3)
	{
		SendClientMessage(playerid, -10270721, "Hai gia' consegnato il massimo di chiavi (%d).", 3);
		return 0;
	}
	g_HPl[playerid][hpPendingOp] = 7;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpKeyTargetChar] = gPlayer[target][pCharId];
	DB_AddKey(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId], gPlayer[target][pCharId]);
}

public House_OnDBKeyAdded(playerid, houseId, targetId, affected)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 7 || !House_OpMatches(playerid, houseId, affected))
	{
		return 1;
	}
	targetId = g_HPl[playerid][hpKeyTargetChar];
	g_HPl[playerid][hpPendingOp] = 0;
	new idx = House_Find(houseId);
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "Chiave non consegnata (errore database).");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "Chiave non consegnata: limite raggiunto o gia' presente.");
		return 1;
	}
	if (g_HouseRows[idx][hrKeyCount] < 3)
	{
		g_HouseRows[idx][hrKeyChars][g_HouseRows[idx][hrKeyCount]] = targetId;
		g_HouseRows[idx][hrKeyCount]++;
	}
	new target = House_PlayerByChar(targetId), tname[25] = "il giocatore";
	if (target != INVALID_PLAYER_ID)
	{
		GetPlayerName(target, tname, 25);
		SendClientMessage(target, 0x66DD66FF, "Hai ricevuto la chiave della casa %d (%s).", houseId, g_HouseRows[idx][hrOwnerName]);
	}
	SendClientMessage(playerid, 0x66DD66FF, "Hai consegnato una chiave della casa %d a %s (%d/%d).", houseId, tname, g_HouseRows[idx][hrKeyCount], 3);
	return 1;
}

House_LoadKeys(playerid, intent, const token[])
{
	new idx = House_KeyContext(playerid);
	if (idx == -1)
	{
		return 0;
	}
	g_HPl[playerid][hpPendingOp] = 6;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpBillIntent] = intent;
	strmid(g_HPl[playerid][hpKeyToken], token, 0, strlen(token), 32);
	DB_LoadKeys(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId]);
}

public House_OnDBKeysLoaded(playerid, houseId, count)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 6 || !House_OpMatches(playerid, houseId, count))
	{
		return 1;
	}
	new intent = g_HPl[playerid][hpBillIntent];
	g_HPl[playerid][hpPendingOp] = 0;
	if (count == -1)
	{
		Util_Send(playerid, -10270721, "Impossibile leggere le chiavi (errore database).");
		return 1;
	}
	if (intent == 3)
	{
		SendClientMessage(playerid, -2994689, "--- Chiavi casa %d (%d/%d) ---", houseId, count, 3);
		for (new i = 0; i < count; i++)
		{
			new online = House_PlayerByChar(g_HouseKeyList[playerid][i][hkCharId]);
			if (online != 65535)
			{
				SendClientMessage(playerid, -1, "%d) %s (online, id %d)", i + 1, g_HouseKeyList[playerid][i][hkName], online);
			}
			else
			{
				SendClientMessage(playerid, -1, "%d) %s", i + 1, g_HouseKeyList[playerid][i][hkName]);
			}
		}
		Util_Send(playerid, 0x99CCFFFF, "Uso: /casa chiave [id] · /casa revoca [id | Nome_Cognome | numero]");
		return 1;
	}
	new target = 0, num = 0;
	for (new i = 0; i < count && target == 0; i++)
	{
		if (!strcmp(g_HouseKeyList[playerid][i][hkName], g_HPl[playerid][hpKeyToken], 1, 0x7FFFFFFF))
		{
			target = g_HouseKeyList[playerid][i][hkCharId];
		}
	}
	if (target == 0 && Util_ParseInt(g_HPl[playerid][hpKeyToken], num))
	{
		if (num >= 0 && num < 1000 && IsPlayerConnected(num))
		{
			for (new i = 0; i < count; i++)
			{
				if (g_HouseKeyList[playerid][i][hkCharId] == gPlayer[num][pCharId])
				{
					target = gPlayer[num][pCharId];
				}
			}
		}
		if (target == 0 && num >= 1 && num <= count)
		{
			target = g_HouseKeyList[playerid][num - 1][hkCharId];
		}
	}
	if (target <= 0)
	{
		Util_Send(playerid, -10270721, "Nessuna chiave corrisponde: usa /casa chiavi per l'elenco.");
		return 1;
	}
	g_HPl[playerid][hpPendingOp] = 8;
	g_HPl[playerid][hpOpHouseId] = houseId;
	g_HPl[playerid][hpKeyTargetChar] = target;
	DB_RemoveKey(playerid, houseId, gPlayer[playerid][pCharId], target);
	return 1;
}

public House_OnDBKeyRemoved(playerid, houseId, targetId, affected)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 8 || !House_OpMatches(playerid, houseId, affected))
	{
		return 1;
	}
	targetId = g_HPl[playerid][hpKeyTargetChar];
	g_HPl[playerid][hpPendingOp] = 0;
	new idx = House_Find(houseId);
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "Chiave non revocata (errore database).");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "Chiave non revocata: non risulta piu' presente.");
		return 1;
	}
	new w = 0;
	for (new k = 0; k < g_HouseRows[idx][hrKeyCount] && k < 3; k++)
	{
		if (g_HouseRows[idx][hrKeyChars][k] == targetId)
		{
			continue;
		}
		g_HouseRows[idx][hrKeyChars][w++] = g_HouseRows[idx][hrKeyChars][k];
	}
	for (new k = w; k < 3; k++)
	{
		g_HouseRows[idx][hrKeyChars][k] = 0;
	}
	g_HouseRows[idx][hrKeyCount] = w;
	new target = House_PlayerByChar(targetId);
	if (target != INVALID_PLAYER_ID)
	{
		SendClientMessage(target, -10270721, "La tua chiave della casa %d e' stata revocata.", houseId);
		if (g_HPl[target][hpInsideHouse] == houseId && g_HouseRows[idx][hrLocked])
		{
			House_PlaceOutside(target, idx);
		}
	}
	SendClientMessage(playerid, 0x66DD66FF, "Chiave revocata (%d/%d).", g_HouseRows[idx][hrKeyCount], 3);
	return 1;
}

House_Info(playerid, idx)
{
	new tier = House_Tier(idx), role = House_Role(idx, playerid), st1[12], st2[12];
	SendClientMessage(playerid, -2994689, "--- Casa %d - %s ---", g_HouseRows[idx][hrId], s_HsTierName[tier]);
	if (role == 0)
	{
		SendClientMessage(playerid, -1, "In vendita: $%d, livello minimo %d, frigo %d porzioni. Limite case per il tuo livello: %d.", g_HouseRows[idx][hrPrice], s_HsTierLevel[tier], s_HsFridgeCap[tier], House_MaxForLevel(gPlayer[playerid][pLevel]));
		Util_Send(playerid, 0x99CCFFFF, "Acquisto: /casa compra (menu) oppure /casa compra contanti|banca");
		return 0;
	}
	SendClientMessage(playerid, -1, "Proprietario: %s - porta %s.", g_HouseRows[idx][hrOwnerName], g_HouseRows[idx][hrLocked] ? "chiusa a chiave" : "aperta");
	if (role == 3)
	{
		return 0;
	}
	House_UtilText(g_HouseRows[idx][hrLuceOn], g_HouseRows[idx][hrLuceCut], st1, 12);
	House_UtilText(g_HouseRows[idx][hrGasOn], g_HouseRows[idx][hrGasCut], st2, 12);
	SendClientMessage(playerid, -1, "Luce: %s - Gas: %s - Frigo: %d/%d", st1, st2, g_HouseRows[idx][hrFridgeUnits], s_HsFridgeCap[tier]);
	if (role == 1)
	{
		SendClientMessage(playerid, -1, "Bollette non pagate: %d - Chiavi consegnate: %d/%d", g_HouseRows[idx][hrBills], g_HouseRows[idx][hrKeyCount], 3);
	}
}

House_CmdCasa(playerid, const params[])
{
	if (!House_CmdGuard(playerid, 0))
	{
		return 0;
	}
	new pos = 0, sub[32], arg[32];
	Util_NextToken(params, pos, sub, 32);
	new idx = House_Context(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "Devi essere dentro una casa o davanti alla sua porta.");
		return 0;
	}
	if (g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "Hai un'operazione della casa in corso: attendi.");
		return 0;
	}
	if (sub[0] == 0)
	{
		House_OpenMenu(playerid, idx);
		return 0;
	}
	if (!strcmp(sub, "info", 1, 0x7FFFFFFF))
	{
		House_Info(playerid, idx);
		return 0;
	}
	if (!strcmp(sub, "serratura", 1, 0x7FFFFFFF))
	{
		House_ToggleLock(playerid, idx);
		return 0;
	}
	if (!strcmp(sub, "chiavi", 1, 0x7FFFFFFF))
	{
		House_LoadKeys(playerid, 3, "");
		return 0;
	}
	if (!strcmp(sub, "chiave", 1, 0x7FFFFFFF))
	{
		Util_NextToken(params, pos, arg, 32);
		if (arg[0] == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "Uso: /casa chiave [id giocatore]");
			return 0;
		}
		House_GiveKey(playerid, arg);
		return 0;
	}
	if (!strcmp(sub, "revoca", 1, 0x7FFFFFFF))
	{
		Util_NextToken(params, pos, arg, 32);
		if (arg[0] == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "Uso: /casa revoca [id giocatore | Nome_Cognome | numero da /casa chiavi]");
			return 0;
		}
		House_LoadKeys(playerid, 4, arg);
		return 0;
	}
	if (!strcmp(sub, "vendi", 1, 0x7FFFFFFF))
	{
		Util_NextToken(params, pos, arg, 32);
		House_DoSell(playerid, idx, !strcmp(arg, "conferma", 1, 0x7FFFFFFF));
		return 0;
	}
	if (!strcmp(sub, "compra", 1, 0x7FFFFFFF))
	{
		if (g_HouseRows[idx][hrOwnerId] > 0 || House_FindNear(playerid) != idx)
		{
			Util_Send(playerid, -10270721, "Devi essere davanti alla porta di una casa in vendita.");
			return 0;
		}
		Util_NextToken(params, pos, arg, 32);
		if (arg[0] == 0)
		{
			House_OpenMenu(playerid, idx);
			return 0;
		}
		if (!strcmp(arg, "contanti", 1, 0x7FFFFFFF))
		{
			House_DoBuy(playerid, idx, 0);
			return 0;
		}
		if (!strcmp(arg, "banca", 1, 0x7FFFFFFF))
		{
			House_DoBuy(playerid, idx, 1);
			return 0;
		}
		Util_Send(playerid, 0x99CCFFFF, "Uso: /casa compra [contanti|banca]");
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "Uso: /casa [info|compra|vendi|serratura|chiave|chiavi|revoca]");
}

House_CmdEntra(playerid)
{
	if (!House_CmdGuard(playerid, 1))
	{
		return 0;
	}
	if (House_FindInside(playerid) != -1)
	{
		Util_Send(playerid, -10270721, "Sei gia' dentro una casa.");
		return 0;
	}
	new idx = House_FindNear(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "Non sei davanti alla porta di una casa.");
		return 0;
	}
	if (GetPlayerState(playerid) != 1)
	{
		Util_Send(playerid, -10270721, "Scendi dal veicolo per entrare.");
		return 0;
	}
	if (g_HouseRows[idx][hrOwnerId] <= 0)
	{
		Util_Send(playerid, -10270721, "La casa e' in vendita e non e' visitabile. Usa /casa per le informazioni.");
		return 0;
	}
	if (!House_CanEnterNow(idx, playerid))
	{
		Util_Send(playerid, -10270721, "La porta e' chiusa a chiave. Prova con /bussa.");
		return 0;
	}
	House_DoEnter(playerid, idx);
	SendClientMessage(playerid, -1, "Sei entrato nella casa %d. Usa /esci per uscire.", g_HouseRows[idx][hrId]);
}

House_CmdEsci(playerid)
{
	if (!House_CmdGuard(playerid, 1))
	{
		return 0;
	}
	new idx = House_FindInside(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "Non sei dentro una casa.");
		return 0;
	}
	if (!House_NearExit(playerid, idx))
	{
		Util_Send(playerid, -10270721, "Avvicinati alla porta per uscire.");
		return 0;
	}
	House_PlaceOutside(playerid, idx);
}

House_CmdBussa(playerid)
{
	if (!House_CmdGuard(playerid, 1))
	{
		return 0;
	}
	new idx = House_FindNear(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] <= 0)
	{
		Util_Send(playerid, -10270721, "Non sei davanti alla porta di una casa abitata.");
		return 0;
	}
	if (!g_HouseRows[idx][hrLocked])
	{
		Util_Send(playerid, -1, "La porta e' aperta: usa /entra.");
		return 0;
	}
	new now = GetTickCount();
	if (now - g_HPl[playerid][hpKnockTick] < 5000)
	{
		Util_Send(playerid, -10270721, "Hai bussato da poco: attendi.");
		return 0;
	}
	g_HPl[playerid][hpKnockTick] = now;
	new id = g_HouseRows[idx][hrId], name[25], inside = 0;
	GetPlayerName(playerid, name, 25);
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_HPl[p][hpInsideHouse] != id || !Util_IsPlaying(p))
		{
			continue;
		}
		SendClientMessage(p, 0xC2A2DAFF, "* Qualcuno bussa alla porta (%s).", name);
		inside++;
	}
	Util_Send(playerid, 0xC2A2DAFF, "* Bussi alla porta.");
	if (inside == 0)
	{
		Util_Send(playerid, -1, "Non sembra esserci nessuno in casa.");
	}
}

House_UiGuard(playerid, houseId)
{
	if (!House_PlayerOk(playerid) || !g_HsReady)
	{
		return -1;
	}
	new idx = House_Find(houseId);
	if (idx == -1 || House_Context(playerid) != idx)
	{
		if (idx != -1 && g_HPl[playerid][hpBillsHouseId] == houseId && House_Role(idx, playerid) == 1)
		{
			return idx;
		}
		Util_Send(playerid, -10270721, "Non sei piu' vicino a quella casa.");
		return -1;
	}
	return idx;
}

public UI_OnHouseAction(playerid, houseId, action)
{
	new idx = House_UiGuard(playerid, houseId);
	if (idx == -1)
	{
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "Operazione in corso: attendi.");
		return 1;
	}
	switch (action)
	{
		case 1:
		{
			House_ToggleLock(playerid, idx);
			House_OpenMenu(playerid, idx);
		}
		case 2:
		{
			if (!House_HasAccess(idx, playerid))
			{
				Util_Send(playerid, -10270721, "Non hai accesso alle utenze.");
				return 1;
			}
			House_PushUi(playerid, idx);
			UI_HouseUtilityShow(playerid);
		}
		case 3:
		{
			if (House_FindInside(playerid) != idx || !House_HasAccess(idx, playerid))
			{
				Util_Send(playerid, -10270721, "Devi essere dentro la casa per usare il frigo.");
				return 1;
			}
			House_PushUi(playerid, idx);
			UI_HouseFridgeShow(playerid);
		}
		case 4:
		{
			if (House_Role(idx, playerid) != 1)
			{
				Util_Send(playerid, -10270721, "Solo il proprietario gestisce le bollette.");
				return 1;
			}
			House_OpenBills(playerid, idx);
		}
		case 5:
		{
			if (House_Role(idx, playerid) != 1)
			{
				Util_Send(playerid, -10270721, "Solo il proprietario gestisce le chiavi.");
				return 1;
			}
			House_LoadKeys(playerid, 3, "");
		}
		case 6:
		{
			House_DoSell(playerid, idx, 1);
		}
	}
	return 1;
}

public UI_OnHouseBuyConfirm(playerid, houseId, payWithBank)
{
	if (!House_PlayerOk(playerid) || !g_HsReady)
	{
		return 1;
	}
	new idx = House_Find(houseId);
	if (idx == -1)
	{
		return 1;
	}
	House_DoBuy(playerid, idx, payWithBank);
	return 1;
}

public UI_OnHouseUtilityToggle(playerid, houseId, utility, on)
{
	new idx = House_UiGuard(playerid, houseId);
	if (idx == -1)
	{
		return 1;
	}
	new kind = utility == 1 ? 1 : 0;
	House_SetUtility(playerid, idx, kind, on != 0);
	House_PushUi(playerid, idx);
	UI_HouseUtilityShow(playerid);
	return 1;
}

public UI_OnHouseFridgeAction(playerid, houseId, action, amount)
{
	new idx = House_UiGuard(playerid, houseId);
	if (idx == -1)
	{
		return 1;
	}
	switch (action)
	{
		case 1:
		{
			House_EatCold(playerid);
			House_ReopenFridge(playerid);
		}
		case 2:
		{
			House_StartCook(playerid);
			House_ReopenFridge(playerid);
		}
		case 3:
		{
			House_Store(playerid, amount);
		}
		case 4:
		{
			House_Take(playerid, amount);
			House_ReopenFridge(playerid);
		}
	}
	return 1;
}

House_TickRow(idx)
{
	g_HouseRows[idx][hrCycleMin] += 1;
	if (House_LightOn(idx))
	{
		g_HouseRows[idx][hrLuceMin] += 1;
	}
	if (House_GasOn(idx))
	{
		g_HouseRows[idx][hrGasMin] += 1;
	}
	if (!House_LightOn(idx))
	{
		g_HouseRows[idx][hrFridgeOffMin] += 1;
		if (g_HouseRows[idx][hrFridgeOffMin] >= 180 && !g_HsFridgeBusy[idx])
		{
			g_HouseRows[idx][hrFridgeOffMin] = 0;
			new units = g_HouseRows[idx][hrFridgeUnits], lost = (units * 50 + 100 - 1) / 100;
			if (lost > 0)
			{
				g_HouseRows[idx][hrFridgeUnits] = max(units - lost, 0);
				DB_FridgeAddSys(g_HouseRows[idx][hrId], -(units - g_HouseRows[idx][hrFridgeUnits]));
				House_RefreshInside(idx);
				new owner = House_OwnerPlayer(idx);
				if (owner != 65535)
				{
					SendClientMessage(owner, -10270721, "Casa %d: senza corrente il frigo ha perso %d porzioni.", g_HouseRows[idx][hrId], lost);
				}
			}
			House_SaveTick(idx);
		}
	}
	if (g_HouseRows[idx][hrCycleMin] >= 180)
	{
		House_CloseCycle(idx);
	}
}

public House_OnTick()
{
	new now = GetTickCount();
	if (g_HsReady)
	{
		for (new idx = 0; idx < g_HouseCount; idx++)
		{
			if (g_HouseRows[idx][hrOwnerId] <= 0 || g_HsBusy[idx])
			{
				continue;
			}
			new owner = House_OwnerPlayer(idx);
			if (owner == 65535 || !House_PlayerActive(owner, now))
			{
				continue;
			}
			House_TickRow(idx);
		}
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || !Util_IsPlaying(p) || g_HPl[p][hpSession] == 0)
		{
			continue;
		}
		if (g_HPl[p][hpInsideHouse] > 0)
		{
			House_FindInside(p);
		}
		if (g_HPl[p][hpHungerState] == 0)
		{
			House_BeginFoodLoad(p);
		}
		if (g_HPl[p][hpHungerState] != 2 || !House_PlayerActive(p, now))
		{
			continue;
		}
		g_HPl[p][hpHungerAccumMs] += 60000;
		if (g_HPl[p][hpHungerAccumMs] >= 360000)
		{
			g_HPl[p][hpHungerAccumMs] -= 360000;
			if (g_HPl[p][hpHunger] > 0)
			{
				House_HungerSet(p, g_HPl[p][hpHunger] - 1, 1);
			}
		}
	}
	return 1;
}

public House_OnSaveTick()
{
	for (new idx = 0; idx < g_HouseCount; idx++)
	{
		if (g_HouseRows[idx][hrOwnerId] <= 0 || g_HsBusy[idx])
		{
			continue;
		}
		if (House_OwnerPlayer(idx) == 65535)
		{
			continue;
		}
		House_SaveTick(idx);
	}
	return 1;
}

public House_OnDBSeeded(ok)
{
	if (ok != 1)
	{
		Log_Write("HOUSE", "seed case fallito (errore database)");
	}
	return 1;
}

public House_OnDBLoaded(count)
{
	if (count == -1)
	{
		g_HsReady = 0;
		Log_Write("HOUSE", "caricamento case fallito: sistema case disabilitato");
		return 1;
	}
	for (new i = 0; i < count && i < 64; i++)
	{
		g_HsBusy[i] = 0;
		House_RefreshWorld(i);
	}
	g_HsReady = 1;
	new msg[128];
	format(msg, 128, "caricate %d case", count);
	Log_Write("HOUSE", msg);
	return 1;
}

House_OnInit()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		House_ResetPlayer(p);
	}
	for (new i = 0; i < 64; i++)
	{
		g_HsPickup[i] = -1;
		g_HsLabel[i] = 65535;
		g_HsBusy[i] = 0;
		g_HsFridgeBusy[i] = 0;
	}
	g_HsReady = 0;
	for (new s = 0; s < 2; s++)
	{
		Lang_Create3DTextLabel("Supermercato\n/compra - cibo, pasto, acqua, snack, kit, benda, zaino...", 0x66DD66FF, s_HsShops[s][0], s_HsShops[s][1], (0.9 + s_HsShops[s][2]), 12.0, 0, 0);
	}
	g_HsTickTimer = SetTimer("House_OnTick", 60000, 1);
	g_HsSaveTimer = SetTimer("House_OnSaveTick", 300000, 1);
	DB_BootHouses();
}

House_SavePlayerFood(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 2 || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	DB_SaveHunger(gPlayer[playerid][pCharId], g_HPl[playerid][hpHunger]);
}

House_OnExit()
{
	if (g_HsTickTimer != 0)
	{
		KillTimer(g_HsTickTimer);
	}
	if (g_HsSaveTimer != 0)
	{
		KillTimer(g_HsSaveTimer);
	}
	g_HsTickTimer = 0;
	g_HsSaveTimer = 0;
	for (new idx = 0; idx < g_HouseCount; idx++)
	{
		if (g_HouseRows[idx][hrOwnerId] > 0 && House_OwnerPlayer(idx) != 65535)
		{
			House_SaveTick(idx);
		}
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (IsPlayerConnected(p))
		{
			House_SavePlayerFood(p);
		}
	}
}

House_OnPlayerConnect(playerid)
{
	House_ResetPlayer(playerid);
	g_HSessionSeq++;
	g_HPl[playerid][hpSession] = g_HSessionSeq;
	g_HPl[playerid][hpCmdTick] = GetTickCount() - 1500;
}

House_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	for (new idx = 0; idx < g_HouseCount; idx++)
	{
		if (g_HouseRows[idx][hrOwnerId] > 0 && g_HouseRows[idx][hrOwnerId] == gPlayer[playerid][pCharId])
		{
			House_SaveTick(idx);
		}
	}
	House_SavePlayerFood(playerid);
	House_ResetPlayer(playerid);
}

House_OnPlayerSpawn(playerid)
{
	if (!House_PlayerOk(playerid))
	{
		return 0;
	}
	g_HPl[playerid][hpCooking] = 0;
	new world = GetPlayerVirtualWorld(playerid);
	if (g_HsReady && world > 1000)
	{
		new idx = House_Find(world - 1000);
		if (idx != -1 && g_HouseRows[idx][hrOwnerId] > 0 && (!g_HouseRows[idx][hrLocked] || House_HasAccess(idx, playerid)))
		{
			g_HPl[playerid][hpInsideHouse] = g_HouseRows[idx][hrId];
			House_ApplyInsideUi(playerid, idx);
		}
		else if (idx != -1)
		{
			House_PlaceOutside(playerid, idx);
		}
		else
		{
			new Float:x, Float:y, Float:z, Float:a;
			Char_GetSpawnPoint(0, x, y, z, a);
			SetPlayerInterior(playerid, 0);
			SetPlayerVirtualWorld(playerid, 0);
			SetPlayerPos(playerid, x, y, z);
			House_ClearInside(playerid);
		}
	}
	else
	{
		House_ClearInside(playerid);
	}
	if (g_HPl[playerid][hpHungerState] == 2)
	{
		UI_HungerHudUpdate(playerid, g_HPl[playerid][hpHunger]);
		House_CapHealthIfWeak(playerid);
	}
	else
	{
		g_HPl[playerid][hpCapHealthOnLoad] = 1;
		House_BeginFoodLoad(playerid);
	}
}

House_OnPlayerPickUpPickup(playerid, pickupid)
{
	if (!House_PlayerOk(playerid) || !g_HsReady)
	{
		return 0;
	}
	new idx = -1;
	for (new i = 0; i < g_HouseCount; i++)
	{
		if (g_HsPickup[i] == pickupid)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
	{
		return 0;
	}
	new now = GetTickCount();
	if (now - g_HPl[playerid][hpInfoMsgTick] < 3000)
	{
		return 0;
	}
	g_HPl[playerid][hpInfoMsgTick] = now;
	new tier = House_Tier(idx);
	if (g_HouseRows[idx][hrOwnerId] <= 0)
	{
		SendClientMessage(playerid, -1, "Casa %d (%s) in vendita: $%d, livello minimo %d. Usa /casa per le info e l'acquisto.", g_HouseRows[idx][hrId], s_HsTierName[tier], g_HouseRows[idx][hrPrice], s_HsTierLevel[tier]);
	}
	else if (House_CanEnterNow(idx, playerid))
	{
		SendClientMessage(playerid, -1, "Casa %d di %s. Usa /entra per entrare.", g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerName]);
	}
	else
	{
		SendClientMessage(playerid, -1, "Casa %d di %s: porta chiusa a chiave. Usa /bussa.", g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerName]);
	}
}
