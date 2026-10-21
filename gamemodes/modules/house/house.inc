new g_HPl[MAX_PLAYERS][E_HPLAYER];
new g_HSessionSeq;
new g_HsPickup[HOUSE_MAX];
new bool:g_HsFridgeBusy[HOUSE_MAX];
new Text3D:g_HsLabel[HOUSE_MAX];
new bool:g_HsBusy[HOUSE_MAX];
new bool:g_HsReady;
new g_HsTickTimer;
new g_HsSaveTimer;
new s_HsTierName[4][40] = {
	"ერთოთახიანი",
	"ოროთახიანი",
	"საშუალო სახლი",
	"ვილა"
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
		g_HsBusy[idx] = false;
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
		return false;
	}
	for (new k = 0; k < g_HouseRows[idx][hrKeyCount] && k < 3; k++)
	{
		if (g_HouseRows[idx][hrKeyChars][k] == charId)
		{
			return true;
		}
	}
	return false;
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
		return false;
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
		g_HPl[playerid][E_HPLAYER:i] = 0;
	}
	return 0;
}

bool:House_OpMatches(playerid, &houseId, result)
{
	if (g_HPl[playerid][hpOpHouseId] == houseId)
	{
		return true;
	}
	if (result != -1)
	{
		return false;
	}
	houseId = g_HPl[playerid][hpOpHouseId];
	return true;
}

bool:House_CooldownBlocked(playerid)
{
	new now = GetTickCount();
	if (now - g_HPl[playerid][hpCmdTick] < 1500)
	{
		Util_Send(playerid, -10270721, "დაელოდე ცოტა, სანამ სახლის სხვა ბრძანებას გამოიყენებ.");
		return true;
	}
	g_HPl[playerid][hpCmdTick] = now;
	return false;
}

bool:House_CmdGuard(playerid, bool:needIdle)
{
	if (!House_PlayerOk(playerid))
	{
		return false;
	}
	if (!g_HsReady)
	{
		Util_Send(playerid, -10270721, "სახლების სისტემა ახლა მიუწვდომელია.");
		return false;
	}
	if (House_CooldownBlocked(playerid))
	{
		return false;
	}
	if (needIdle && g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "სახლის ოპერაცია მიმდინარეობს: დაელოდე.");
		return false;
	}
	return true;
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
	if (g_HouseRows[idx][hrIntGame] >= 0)
	{
		return (GetPlayerDistanceFromPoint(playerid, g_HouseRows[idx][hrIntX], g_HouseRows[idx][hrIntY], g_HouseRows[idx][hrIntZ]) <= 2.5);
	}
	new k = House_IntIdx(idx);
	return (GetPlayerDistanceFromPoint(playerid, s_HsIntPos[k][0], s_HsIntPos[k][1], s_HsIntPos[k][2]) <= 2.5);
}

bool:House_NearShop(playerid)
{
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		return false;
	}
	for (new i = 0; i < 2; i++)
	{
		if ((GetPlayerDistanceFromPoint(playerid, s_HsShops[i][0], s_HsShops[i][1], s_HsShops[i][2]) <= 3.0))
		{
			return true;
		}
	}
	return false;
}

House_RefreshWorld(idx)
{
	if (idx < 0 || idx >= g_HouseCount)
	{
		return 0;
	}
	new text[480], tier = House_Tier(idx), bool:free = g_HouseRows[idx][hrOwnerId] <= 0;
	if (free)
	{
		format(text, 480, "სახლი %d - %s\nიყიდება: $%d (დონე %d)\nინფო: /casa", g_HouseRows[idx][hrId], s_HsTierName[tier], g_HouseRows[idx][hrPrice], s_HsTierLevel[tier]);
	}
	else
	{
		format(text, 480, "სახლი %d - %s\nმფლობელი: %s\n%s", g_HouseRows[idx][hrId], s_HsTierName[tier], g_HouseRows[idx][hrOwnerName], g_HouseRows[idx][hrLocked] ? "ჩაკეტილია (/bussa)" : "ღიაა (/entra)");
	}
	new color = free ? 0x66DD66FF : -2994689;
	if (g_HsLabel[idx] != INVALID_3DTEXT_ID)
	{
		Lang_Update3DTextLabelText(g_HsLabel[idx], color, text);
	}
	else
	{
		g_HsLabel[idx] = Lang_Create3DTextLabel(text, color, g_HouseRows[idx][hrExtX], g_HouseRows[idx][hrExtY], (0.9 + g_HouseRows[idx][hrExtZ]), 12.0, 0, false);
	}
	if (g_HsPickup[idx] >= 0)
	{
		DestroyPickup(g_HsPickup[idx]);
	}
	g_HsPickup[idx] = CreatePickup(free ? 1273 : 1272, 1, g_HouseRows[idx][hrExtX], g_HouseRows[idx][hrExtY], g_HouseRows[idx][hrExtZ], 0);
	return 0;
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
	return 0;
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
	SetPlayerVirtualWorld(playerid, id + 1000);
	if (g_HouseRows[idx][hrIntGame] >= 0)
	{
		// casa del mondo Arizona: interno GTA e punto proprio
		SetPlayerInterior(playerid, g_HouseRows[idx][hrIntGame]);
		SetPlayerPos(playerid, g_HouseRows[idx][hrIntX], g_HouseRows[idx][hrIntY], g_HouseRows[idx][hrIntZ]);
	}
	else
	{
		SetPlayerInterior(playerid, s_HsIntId[k]);
		SetPlayerPos(playerid, s_HsIntPos[k][0], s_HsIntPos[k][1], s_HsIntPos[k][2]);
		SetPlayerFacingAngle(playerid, s_HsIntPos[k][3]);
	}
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
			Util_Send(playerid, -10270721, "შიმშილისგან დასუსტდი: რამე ჭამე (/mangia, /usa, /compra).");
		}
		else if (old > 20 && value <= 20)
		{
			Util_Send(playerid, -2994689, "გშია: რამე ჭამე.");
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
	return 0;
}

House_BeginFoodLoad(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 0 || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	g_HPl[playerid][hpHungerState] = 1;
	DB_LoadFood(playerid, gPlayer[playerid][pCharId]);
	return 0;
}

bool:House_FoodReady(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 2)
	{
		Util_Send(playerid, -10270721, "საკვების მონაცემები ჯერ არ ჩატვირთულა: სცადე ცოტა ხანში.");
		return false;
	}
	return true;
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
			SendClientMessage(playerid, -10270721, "სახლი %d: %d გადაუხდელი კომუნალური გადასახადი გაქვს (/bollette).", g_HouseRows[i][hrId], g_HouseRows[i][hrBills]);
		}
		if (g_HouseRows[i][hrLuceCut])
		{
			SendClientMessage(playerid, -10270721, "სახლი %d: ელექტროენერგია გადაუხდელობის გამო გაითიშა.", g_HouseRows[i][hrId]);
		}
		if (g_HouseRows[i][hrGasCut])
		{
			SendClientMessage(playerid, -10270721, "სახლი %d: გაზი გადაუხდელობის გამო გაითიშა.", g_HouseRows[i][hrId]);
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
		format(out, size, "გათიშულია");
	}
	else
	{
		format(out, size, "%s", on ? "ON" : "გამორთ.");
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
	UI_HouseDataSet(playerid, id, House_Role(idx, playerid), House_Tier(idx), House_IntIdx(idx), bool:g_HouseRows[idx][hrLocked], House_UtilState(bool:g_HouseRows[idx][hrLuceOn], bool:g_HouseRows[idx][hrLuceCut]), House_UtilState(bool:g_HouseRows[idx][hrGasOn], bool:g_HouseRows[idx][hrGasCut]), g_HouseRows[idx][hrFridgeUnits], Inv_Count(playerid, 1), g_HouseRows[idx][hrKeyCount], gPlayer[playerid][pLevel], gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
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
	return 0;
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
		Util_Send(playerid, -10270721, "კომუნალურების წაკითხვა ვერ მოხერხდა (ბაზის შეცდომა). სცადე მოგვიანებით.");
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
		Util_Send(playerid, -10270721, "ოპერაცია მიმდინარეობს: დაელოდე.");
		return 0;
	}
	if (House_FindNear(playerid) != idx || GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
	{
		Util_Send(playerid, -10270721, "ფეხით უნდა იდგე სახლის კართან.");
		return 0;
	}
	if (g_HouseRows[idx][hrOwnerId] > 0)
	{
		Util_Send(playerid, -10270721, "ეს სახლი არ იყიდება.");
		return 0;
	}
	if (payWithBank != 0 && payWithBank != 1)
	{
		Util_Send(playerid, -10270721, "გადახდის არასწორი მეთოდი.");
		return 0;
	}
	new tier = House_Tier(idx), price = g_HouseRows[idx][hrPrice], level = gPlayer[playerid][pLevel], charId = gPlayer[playerid][pCharId];
	if (price <= 0 || charId <= 0)
	{
		Util_Send(playerid, -10270721, "შეძენა მიუწვდომელია.");
		return 0;
	}
	if (level < s_HsTierLevel[tier])
	{
		SendClientMessage(playerid, -10270721, "ამ სახლისთვის საჭიროა %d დონე.", s_HsTierLevel[tier]);
		return 0;
	}
	new maxHouses = House_MaxForLevel(level);
	if (House_OwnedCount(charId) >= maxHouses)
	{
		SendClientMessage(playerid, -10270721, "შენი დონისთვის სახლების ლიმიტს მიაღწიე (%d).", maxHouses);
		return 0;
	}
	if (payWithBank == 1 ? !Money_CanBank(playerid, price) : !Money_Can(playerid, price))
	{
		Util_Send(playerid, -10270721, "ამ მეთოდით საკმარისი თანხა არ გაქვს.");
		return 0;
	}
	g_HsBusy[idx] = true;
	g_HPl[playerid][hpPendingOp] = 1;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpPayWithBank] = payWithBank;
	g_HPl[playerid][hpOpAmount] = price;
	Util_Send(playerid, -1, "შეძენა ფორმდება...");
	DB_BuyHouse(playerid, g_HouseRows[idx][hrId], charId, price, maxHouses);
	return 0;
}

public House_OnDBBought(playerid, houseId, affected)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new idx;
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
		g_HsBusy[idx] = false;
	}
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "შეძენა ვერ მოხერხდა (ბაზის შეცდომა). თანხა არ ჩამოიჭრა.");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "შეძენა შეუძლებელია: სახლი აღარ არის ხელმისაწვდომი ან ლიმიტს მიაღწიე. თანხა არ ჩამოიჭრა.");
		return 1;
	}
	new bool:ok = House_PlayerOk(playerid) && gPlayer[playerid][pLevel] >= s_HsTierLevel[House_Tier(idx)];
	if (ok)
	{
		ok = bank == 1 ? Money_TakeBank(playerid, price, "house", "") : Money_Take(playerid, price);
	}
	if (!ok)
	{
		g_HsBusy[idx] = true;
		g_HPl[playerid][hpPendingOp] = 3;
		g_HPl[playerid][hpOpHouseId] = houseId;
		DB_SellHouse(playerid, houseId, charId);
		Util_Send(playerid, -10270721, "შეძენა გაუქმდა: მოთხოვნები ან თანხა აღარ შეესაბამება. თანხა არ ჩამოიჭრა.");
		return 1;
	}
	new name[75], msg[256];
	GetPlayerName(playerid, name, 75);
	g_HouseRows[idx][hrOwnerId] = charId;
	strmid(g_HouseRows[idx][hrOwnerName], name, 0, strlen(name), 25);
	g_HouseRows[idx][hrPricePaid] = price;
	House_ResetRowState(idx);
	House_RefreshWorld(idx);
	Char_Save(playerid);
	SendClientMessage(playerid, 0x66DD66FF, "იყიდე სახლი %d $%d-ად (%s). შესასვლელად /entra, სამართავად /casa.", houseId, price, bank == 1 ? "ბანკიდან" : "ნაღდით");
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
		Util_Send(playerid, -10270721, "ოპერაცია მიმდინარეობს: დაელოდე.");
		return 0;
	}
	if (g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "სახლის გაყიდვა მხოლოდ მფლობელს შეუძლია.");
		return 0;
	}
	if (House_Context(playerid) != idx)
	{
		Util_Send(playerid, -10270721, "სახლში ან მის კართან უნდა იყო.");
		return 0;
	}
	if (g_HouseRows[idx][hrBills] > 0)
	{
		Util_Send(playerid, -10270721, "გადაუხდელი კომუნალურებით სახლს ვერ გაყიდი (/bollette).");
		return 0;
	}
	new refund = House_SellRefund(idx);
	if (refund <= 0 || refund > Money_BankRoom(playerid))
	{
		Util_Send(playerid, -10270721, "შენი საბანკო ანგარიში თანხას ვერ მიიღებს: გაყიდვა უარყოფილია.");
		return 0;
	}
	if (!confirmed)
	{
		SendClientMessage(playerid, -2994689, "სახლის %d გაყიდვა: ბანკში მიიღებ $%d-ს (%d%%). კომუნალურები, გასაღებები და მაცივარი განულდება.", g_HouseRows[idx][hrId], refund, 70);
		Util_Send(playerid, 0x99CCFFFF, "დასადასტურებლად: /casa vendi conferma");
		return 0;
	}
	g_HsBusy[idx] = true;
	g_HPl[playerid][hpPendingOp] = 2;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpOpAmount] = refund;
	Util_Send(playerid, -1, "მიმდინარეობს გაყიდვა...");
	DB_SellHouse(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId]);
	return 0;
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
			g_HsBusy[idx] = false;
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
		g_HsBusy[idx] = false;
	}
	if (affected == -1)
	{
		Util_Send(playerid, -10270721, "გაყიდვა ვერ მოხერხდა (ბაზის შეცდომა). თანხა არ ჩაირიცხა.");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "გაყიდვა არ შესრულდა: სახლი აღარ არის შენი. თანხა არ ჩაირიცხა.");
		return 1;
	}
	if (!Money_GiveBank(playerid, refund, "refund", ""))
	{
		DB_RestoreSoldHouse(houseId, charId, g_HouseRows[idx][hrPricePaid]);
		Util_Send(playerid, -10270721, "გაყიდვა გაუქმდა: საბანკო ანგარიში თანხას ვერ მიიღებს. თანხა არ ჩაირიცხა.");
		Log_Write("HOUSE", "vendita annullata: accredito impossibile, proprieta' ripristinata");
		return 1;
	}
	House_EvictAll(idx, "სახლი გაიყიდა: გარეთ გაგიყვანეს.");
	g_HouseRows[idx][hrOwnerId] = 0;
	g_HouseRows[idx][hrOwnerName][0] = 0;
	g_HouseRows[idx][hrPricePaid] = 0;
	House_ResetRowState(idx);
	House_RefreshWorld(idx);
	Char_Save(playerid);
	new name[75], msg[256];
	GetPlayerName(playerid, name, 75);
	SendClientMessage(playerid, 0x66DD66FF, "გაყიდე სახლი %d: $%d ჩაირიცხა ბანკში.", houseId, refund);
	format(msg, 256, "VENDITA %s: casa %d rimborso $%d", name, houseId, refund);
	Log_Write("HOUSE", msg);
	return 1;
}

House_ToggleLock(playerid, idx)
{
	if (g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "საკეტის გამოყენება მხოლოდ მფლობელს შეუძლია.");
		return 0;
	}
	if (House_Context(playerid) != idx)
	{
		Util_Send(playerid, -10270721, "სახლში ან მის კართან უნდა იყო.");
		return 0;
	}
	g_HouseRows[idx][hrLocked] = !g_HouseRows[idx][hrLocked];
	DB_SetHouseLock(g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerId], bool:g_HouseRows[idx][hrLocked]);
	House_RefreshWorld(idx);
	SendClientMessage(playerid, 0x66DD66FF, "სახლის %d კარი: %s.", g_HouseRows[idx][hrId], g_HouseRows[idx][hrLocked] ? "ჩაკეტილია" : "ღიაა");
	return 0;
}

bool:House_SetUtility(playerid, idx, utility, bool:on)
{
	if (House_FindInside(playerid) != idx)
	{
		Util_Send(playerid, -10270721, "სახლში უნდა იყო.");
		return false;
	}
	new role = House_Role(idx, playerid);
	if (role != 1 && !(utility == 0 && role == 2))
	{
		Util_Send(playerid, -10270721, utility == 1 ? "გაზს მხოლოდ მფლობელი მართავს." : "ამ სახლის კომუნალურებზე წვდომა არ გაქვს.");
		return false;
	}
	new bool:cut = bool:(utility == 1 ? g_HouseRows[idx][hrGasCut] : g_HouseRows[idx][hrLuceCut]);
	if (cut)
	{
		Util_Send(playerid, -10270721, "გადაუხდელობის გამო გათიშულია: გადაიხადე კომუნალური (/bollette).");
		return false;
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
	SendClientMessage(playerid, 0x66DD66FF, "%s %s.", utility == 1 ? "გაზი" : "შუქი", on ? "ჩართულია" : "გამორთულია");
	return true;
}

House_CmdToggleUtility(playerid, utility)
{
	if (!House_CmdGuard(playerid, true))
	{
		return 0;
	}
	new idx = House_FindInside(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "სახლში უნდა იყო.");
		return 0;
	}
	new bool:cur = bool:(utility == 1 ? g_HouseRows[idx][hrGasOn] : g_HouseRows[idx][hrLuceOn]);
	House_SetUtility(playerid, idx, utility, !cur);
	return 0;
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
	return 0;
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
		Util_Send(playerid, -10270721, "შენს სახლში უნდა იყო (ან მეგობრისაში, რომელმაც გასაღები მოგცა).");
		return -1;
	}
	if (!House_HasAccess(idx, playerid))
	{
		Util_Send(playerid, -10270721, "ამ სახლის მაცივარს ვერ გამოიყენებ.");
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
		Util_Send(playerid, -10270721, "მაცივარს ვიღაც იყენებს: ცოტა დაელოდე.");
		return 0;
	}
	if (g_HouseRows[idx][hrFridgeUnits] <= 0)
	{
		Util_Send(playerid, -10270721, "მაცივარი ცარიელია.");
		return 0;
	}
	if (g_HPl[playerid][hpHunger] >= 100)
	{
		Util_Send(playerid, -10270721, "არ გშია.");
		return 0;
	}
	g_HouseRows[idx][hrFridgeUnits]--;
	House_FridgeChanged(playerid, idx, -1);
	House_HungerSet(playerid, g_HPl[playerid][hpHunger] + 25, true);
	SendClientMessage(playerid, 0x66DD66FF, "მაცივრიდან რაღაც ცივს ჭამ. სიმაძღრე: %d%%.", g_HPl[playerid][hpHunger]);
	return 0;
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
		Util_Send(playerid, -10270721, "უკვე ამზადებ საჭმელს.");
		return 0;
	}
	if (g_HPl[playerid][hpCookCooldown] != 0 && g_HPl[playerid][hpCookCooldown] - GetTickCount() > 0)
	{
		Util_Send(playerid, -10270721, "ცხელი კერძი უკვე მზადაა: გამოიყენე /mangia.");
		return 0;
	}
	if (!House_GasOn(idx))
	{
		Util_Send(playerid, -10270721, "გაზი არ მუშაობს: საჭმელს ვერ მოამზადებ.");
		return 0;
	}
	if (g_HouseRows[idx][hrFridgeUnits] <= 0)
	{
		Util_Send(playerid, -10270721, "მაცივარი ცარიელია.");
		return 0;
	}
	g_HPl[playerid][hpCooking] = 1;
	g_HPl[playerid][hpCookHouseId] = g_HouseRows[idx][hrId];
	SetTimerEx("House_OnCookDone", 6000, false, "dd", playerid, g_HPl[playerid][hpSession]);
	Util_Send(playerid, -1, "საჭმლის მომზადებას იწყებ...");
	return 0;
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
		Util_Send(playerid, -10270721, "მომზადება გაუქმდა.");
		return 1;
	}
	g_HouseRows[idx][hrFridgeUnits]--;
	House_FridgeChanged(playerid, idx, -1);
	g_HPl[playerid][hpCookCooldown] = GetTickCount() + 300000;
	if (g_HPl[playerid][hpCookCooldown] == 0)
	{
		g_HPl[playerid][hpCookCooldown] = 1;
	}
	SendClientMessage(playerid, 0x66DD66FF, "კერძი მზადაა: გამოიყენე /mangia %d წუთში.", 5);
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
		Util_Send(playerid, -10270721, "ცხელი კერძი არ გაქვს. გამოიყენე /cucina სახლში ან /mangia freddo.");
		return 0;
	}
	if (g_HPl[playerid][hpHunger] >= 100)
	{
		Util_Send(playerid, -10270721, "არ გშია.");
		return 0;
	}
	g_HPl[playerid][hpCookCooldown] = 0;
	House_HungerSet(playerid, g_HPl[playerid][hpHunger] + 45, true);
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	hp += float(5);
	if ((hp > 100.0))
	{
		hp = 100.0;
	}
	SetPlayerHealth(playerid, hp);
	SendClientMessage(playerid, 0x66DD66FF, "ცხელ კერძს ჭამ. სიმაძღრე: %d%%.", g_HPl[playerid][hpHunger]);
	return 0;
}

House_Store(playerid, n)
{
	new idx = House_FindInside(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "პროდუქტების შენახვა მხოლოდ შენი სახლის მაცივარში შეგიძლია.");
		return 0;
	}
	if (g_HsFridgeBusy[idx])
	{
		Util_Send(playerid, -10270721, "მაცივარს ვიღაც იყენებს: ცოტა დაელოდე.");
		return 0;
	}
	if (n < 1 || n > Inv_Count(playerid, 1))
	{
		Util_Send(playerid, -10270721, "ამდენი პროდუქტი თან არ გაქვს.");
		return 0;
	}
	new cap = House_FridgeCap(idx);
	if (g_HouseRows[idx][hrFridgeUnits] + n > cap)
	{
		SendClientMessage(playerid, -10270721, "მაცივარში კიდევ მხოლოდ %d ულუფის ადგილია.", cap - g_HouseRows[idx][hrFridgeUnits]);
		return 0;
	}
	g_HsFridgeBusy[idx] = true;
	if (!Inv_FridgeMove(playerid, g_HouseRows[idx][hrId], 1, n, cap))
	{
		g_HsFridgeBusy[idx] = false;
	}
	return 0;
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
		Util_Send(playerid, -10270721, "მაცივარს ვიღაც იყენებს: ცოტა დაელოდე.");
		return 0;
	}
	if (n < 1 || n > g_HouseRows[idx][hrFridgeUnits])
	{
		Util_Send(playerid, -10270721, "მაცივარში ამდენი ულუფა არ არის.");
		return 0;
	}
	if (Inv_Count(playerid, 1) + n > 12)
	{
		SendClientMessage(playerid, -10270721, "თან მაქსიმუმ %d ულუფის ტარება შეგიძლია.", 12);
		return 0;
	}
	g_HsFridgeBusy[idx] = true;
	if (!Inv_FridgeMove(playerid, g_HouseRows[idx][hrId], 2, n, House_FridgeCap(idx)))
	{
		g_HsFridgeBusy[idx] = false;
	}
	return 0;
}

public House_InvFridgeResult(playerid, houseId, direction, qty, result)
{
	new idx = House_Find(houseId);
	if (idx == -1)
	{
		return 1;
	}
	g_HsFridgeBusy[idx] = false;
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
				SendClientMessage(playerid, 0x66DD66FF, "მაცივარში ჩადე %d ულუფა (%d/%d).", qty, g_HouseRows[idx][hrFridgeUnits], House_FridgeCap(idx));
			}
			else
			{
				SendClientMessage(playerid, 0x66DD66FF, "მაცივრიდან აიღე %d ულუფა (თან მაქს. %d).", qty, 12);
			}
			if (House_FindInside(playerid) == idx)
			{
				House_ReopenFridge(playerid);
			}
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "ინვენტარში ადგილი არ გაქვს (ან ულუფების მაქსიმუმი უკვე გაქვს).");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, direction == 1 ? "ამდენი პროდუქტი თან არ გაქვს." : "მაცივარში ამდენი ულუფა არ არის.");
		}
		case 6:
		{
			Util_Send(playerid, -10270721, "მაცივარში საკმარისი ადგილი არ არის.");
		}
		case 4:
		{
			Util_Send(playerid, -10270721, "ამ სახლის მაცივარს ვერ გამოიყენებ.");
		}
		case -1:
		{
			Util_Send(playerid, -10270721, "ბაზის შეცდომა: მაცივრის მდგომარეობა ვერ შემოწმდა, სცადე მოგვიანებით.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "ულუფების გადატანა ვერ მოხერხდა.");
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
	g_HsFridgeBusy[idx] = true;
	SetTimerEx("House_FridgeReloadLater", 1500, false, "d", houseId);
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
	g_HsFridgeBusy[idx] = false;
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
	House_HungerSet(playerid, g_HPl[playerid][hpHunger] + amount, true);
	return g_HPl[playerid][hpHunger];
}

House_CmdFrigo(playerid, const params[])
{
	if (!House_CmdGuard(playerid, false))
	{
		return 0;
	}
	new idx = House_FoodContext(playerid);
	if (idx == -1)
	{
		return 0;
	}
	new pos = 0, sub[96], arg[96], n = 0;
	Util_NextToken(params, pos, sub, 96);
	if (sub[0] == 0)
	{
		SendClientMessage(playerid, -1, "მაცივარი: %d/%d ულუფა (თან: %d/%d).", g_HouseRows[idx][hrFridgeUnits], House_FridgeCap(idx), Inv_Count(playerid, 1), 12);
		if (!House_LightOn(idx))
		{
			Util_Send(playerid, -10270721, "შუქი გამორთულია: მაცივარი არ აცივებს და მარაგი ფუჭდება.");
		}
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /frigo metti [n] (მხოლოდ მფლობელი) · /frigo prendi [n] (მფლობელი ან სტუმარი)");
		return 0;
	}
	new bool:store = !strcmp(sub, "metti", true, 0x7FFFFFFF);
	if (!store && strcmp(sub, "prendi", true, 0x7FFFFFFF))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /frigo metti [n] · /frigo prendi [n]");
		return 0;
	}
	Util_NextToken(params, pos, arg, 96);
	n = 1;
	if (arg[0] != 0 && (!Util_ParseInt(arg, n) || n < 1 || n > 40))
	{
		Util_Send(playerid, -10270721, "არასწორი რაოდენობა.");
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
	return 0;
}

House_CmdMangia(playerid, const params[])
{
	if (!House_CmdGuard(playerid, false))
	{
		return 0;
	}
	new pos = 0, sub[96];
	Util_NextToken(params, pos, sub, 96);
	if (sub[0] == 0)
	{
		House_EatPlate(playerid);
		return 0;
	}
	if (!strcmp(sub, "freddo", true, 0x7FFFFFFF))
	{
		House_EatCold(playerid);
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /mangia (ცხელი კერძი /cucina-ს შემდეგ) · /mangia freddo");
	return 0;
}

House_CmdCucina(playerid)
{
	if (!House_CmdGuard(playerid, false))
	{
		return 0;
	}
	House_StartCook(playerid);
	return 0;
}

House_CmdCompra(playerid, const params[])
{
	if (!House_CmdGuard(playerid, false))
	{
		return 0;
	}
	if (!House_NearShop(playerid))
	{
		Util_Send(playerid, -10270721, "სუპერმარკეტში უნდა იყო.");
		return 0;
	}
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
	{
		Util_Send(playerid, -10270721, "ჩამოდი მანქანიდან.");
		return 0;
	}
	new pos = 0, sub[96], arg[96], n = 1;
	Util_NextToken(params, pos, sub, 96);
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
	Util_NextToken(params, pos, arg, 96);
	if (arg[0] != 0 && (!Util_ParseInt(arg, n) || n < 1 || n > 12))
	{
		Util_Send(playerid, -10270721, "არასწორი რაოდენობა.");
		return 0;
	}
	Inv_Buy(playerid, itemId, n);
	return 0;
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
	return 0;
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
		SendClientMessage(owner, -2994689, "სახლი %d: ახალი გადასახადი %s, $%d (/bollette).", houseId, kind == 1 ? "გაზის" : "შუქის", amount);
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
		SendClientMessage(owner, -10270721, "სახლი %d: %s გაითიშა%s გადაუხდელი გადასახადების გამო. გადაიხადე /bollette (+$%d აღდგენა).", id, kind == 1 ? "გაზი" : "შუქი", "", 40);
	}
	new msg[256];
	format(msg, 256, "DISTACCO casa %d: %s", id, kind == 1 ? "gas" : "luce");
	Log_Write("HOUSE", msg);
	return 0;
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
	new bool:cut = bool:(kind == 1 ? g_HouseRows[idx][hrGasCut] : g_HouseRows[idx][hrLuceCut]);
	g_HPl[playerid][hpBillKind] = kind;
	DB_PayBills(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId], kind, sum, cut);
	return 0;
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
		Util_Send(playerid, -10270721, "კომუნალურის გადახდა მხოლოდ მფლობელს შეუძლია.");
		return 1;
	}
	if (g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "ოპერაცია მიმდინარეობს: დაელოდე.");
		return 1;
	}
	if (g_HPl[playerid][hpBillsHouseId] != houseId)
	{
		Util_Send(playerid, -10270721, "სიის გასაახლებლად ხელახლა გახსენი /bollette.");
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
		new bool:cut = bool:(kind == 1 ? g_HouseRows[idx][hrGasCut] : g_HouseRows[idx][hrLuceCut]);
		if (sum <= 0 && !cut)
		{
			continue;
		}
		mask |= 1 << kind;
		total += sum + (cut ? 40 : 0);
	}
	if (mask == 0 || total <= 0)
	{
		Util_Send(playerid, 0x66DD66FF, "გადასახდელი არაფერია.");
		return 1;
	}
	if (payWithBank == 1 ? !Money_CanBank(playerid, total) : !Money_Can(playerid, total))
	{
		Util_Send(playerid, -10270721, "ამ მეთოდით საკმარისი თანხა არ გაქვს.");
		return 1;
	}
	g_HPl[playerid][hpPendingOp] = 4;
	g_HPl[playerid][hpPayWithBank] = payWithBank;
	g_HPl[playerid][hpBillKindMask] = mask;
	g_HPl[playerid][hpBillPaidTotal] = 0;
	Util_Send(playerid, -1, "მიმდინარეობს გადახდა...");
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
		Util_Send(playerid, -10270721, "გადახდა ვერ მოხერხდა (ბაზის შეცდომა). შეამოწმე /bollette-ით.");
		Log_Write("HOUSE", "pagamento bollette: errore database (verificare lo stato delle bollette della casa)");
		return 1;
	}
	if (paidAmount == 0 && restored == 0)
	{
		g_HPl[playerid][hpPendingOp] = 0;
		g_HPl[playerid][hpBillsHouseId] = 0;
		Util_Send(playerid, -10270721, "გადასახადები შეიცვალა ან აღარ არის გადასახდელი: ხელახლა გახსენი /bollette. ამ კომუნალურზე თანხა არ ჩამოიჭრა.");
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
		Util_Send(playerid, -10270721, "თანხა არ კმარა: ამ კომუნალურის გადახდა გაუქმდა.");
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
	new msg[256], name[75];
	GetPlayerName(playerid, name, 75);
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
	SendClientMessage(playerid, 0x66DD66FF, "გადასახადები გადახდილია: ჩამოიჭრა $%d (%s).", g_HPl[playerid][hpBillPaidTotal], bank == 1 ? "ბანკიდან" : "ნაღდით");
	return 1;
}

House_CmdBollette(playerid)
{
	if (!House_CmdGuard(playerid, true))
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
			Util_Send(playerid, -10270721, "სახლი არ გაქვს.");
			return 0;
		}
		if (n > 1)
		{
			Util_Send(playerid, -10270721, "რამდენიმე სახლი გაქვს: შედი სასურველში ან მის კართან მიდი.");
			return 0;
		}
		idx = found;
	}
	House_OpenBills(playerid, idx);
	return 0;
}

House_KeyContext(playerid)
{
	new idx = House_Context(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] != gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "უნდა იყო მფლობელი და იმყოფებოდე სახლში ან მის კართან.");
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
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return 0;
	}
	if (!Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "მოთამაშე ძალიან შორსაა.");
		return 0;
	}
	if (House_IsGuest(idx, gPlayer[target][pCharId]))
	{
		Util_Send(playerid, -10270721, "ამ მოთამაშეს უკვე აქვს გასაღები.");
		return 0;
	}
	if (g_HouseRows[idx][hrKeyCount] >= 3)
	{
		SendClientMessage(playerid, -10270721, "გასაღებების მაქსიმუმი უკვე გაეცი (%d).", 3);
		return 0;
	}
	g_HPl[playerid][hpPendingOp] = 7;
	g_HPl[playerid][hpOpHouseId] = g_HouseRows[idx][hrId];
	g_HPl[playerid][hpKeyTargetChar] = gPlayer[target][pCharId];
	DB_AddKey(playerid, g_HouseRows[idx][hrId], gPlayer[playerid][pCharId], gPlayer[target][pCharId]);
	return 0;
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
		Util_Send(playerid, -10270721, "გასაღები ვერ გადაეცა (ბაზის შეცდომა).");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "გასაღები ვერ გადაეცა: ლიმიტი ამოწურულია ან უკვე აქვს.");
		return 1;
	}
	if (g_HouseRows[idx][hrKeyCount] < 3)
	{
		g_HouseRows[idx][hrKeyChars][g_HouseRows[idx][hrKeyCount]] = targetId;
		g_HouseRows[idx][hrKeyCount]++;
	}
	new target = House_PlayerByChar(targetId), tname[75] = "il giocatore";
	if (target != INVALID_PLAYER_ID)
	{
		GetPlayerName(target, tname, 75);
		SendClientMessage(target, 0x66DD66FF, "მიიღე სახლის %d გასაღები (%s).", houseId, g_HouseRows[idx][hrOwnerName]);
	}
	SendClientMessage(playerid, 0x66DD66FF, "სახლის %d გასაღები გადაეცი %s-ს (%d/%d).", houseId, tname, g_HouseRows[idx][hrKeyCount], 3);
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
	return 0;
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
		Util_Send(playerid, -10270721, "გასაღებების წაკითხვა ვერ მოხერხდა (ბაზის შეცდომა).");
		return 1;
	}
	if (intent == 3)
	{
		SendClientMessage(playerid, -2994689, "--- სახლის %d გასაღებები (%d/%d) ---", houseId, count, 3);
		for (new i = 0; i < count; i++)
		{
			new online = House_PlayerByChar(g_HouseKeyList[playerid][i][hkCharId]);
			if (online != 65535)
			{
				SendClientMessage(playerid, -1, "%d) %s (ონლაინ, id %d)", i + 1, g_HouseKeyList[playerid][i][hkName], online);
			}
			else
			{
				SendClientMessage(playerid, -1, "%d) %s", i + 1, g_HouseKeyList[playerid][i][hkName]);
			}
		}
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /casa chiave [id] · /casa revoca [id | სახელი_გვარი | ნომერი]");
		return 1;
	}
	new target = 0, num = 0;
	for (new i = 0; i < count && target == 0; i++)
	{
		if (!strcmp(g_HouseKeyList[playerid][i][hkName], g_HPl[playerid][hpKeyToken], true, 0x7FFFFFFF))
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
		Util_Send(playerid, -10270721, "ასეთი გასაღები ვერ მოიძებნა: სია ნახე /casa chiavi-ით.");
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
		Util_Send(playerid, -10270721, "გასაღები ვერ გაუქმდა (ბაზის შეცდომა).");
		return 1;
	}
	if (affected != 1 || idx == -1)
	{
		Util_Send(playerid, -10270721, "გასაღები ვერ გაუქმდა: აღარ არსებობს.");
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
		SendClientMessage(target, -10270721, "სახლის %d შენი გასაღები გაუქმდა.", houseId);
		if (g_HPl[target][hpInsideHouse] == houseId && g_HouseRows[idx][hrLocked])
		{
			House_PlaceOutside(target, idx);
		}
	}
	SendClientMessage(playerid, 0x66DD66FF, "გასაღები გაუქმდა (%d/%d).", g_HouseRows[idx][hrKeyCount], 3);
	return 1;
}

House_Info(playerid, idx)
{
	new tier = House_Tier(idx), role = House_Role(idx, playerid), st1[36], st2[36];
	SendClientMessage(playerid, -2994689, "--- სახლი %d - %s ---", g_HouseRows[idx][hrId], s_HsTierName[tier]);
	if (role == 0)
	{
		SendClientMessage(playerid, -1, "იყიდება: $%d, მინ. დონე %d, მაცივარი %d ულუფა. სახლების ლიმიტი შენი დონისთვის: %d.", g_HouseRows[idx][hrPrice], s_HsTierLevel[tier], s_HsFridgeCap[tier], House_MaxForLevel(gPlayer[playerid][pLevel]));
		Util_Send(playerid, 0x99CCFFFF, "შეძენა: /casa compra (მენიუ) ან /casa compra contanti|banca");
		return 0;
	}
	SendClientMessage(playerid, -1, "მფლობელი: %s - კარი %s.", g_HouseRows[idx][hrOwnerName], g_HouseRows[idx][hrLocked] ? "ჩაკეტილია" : "ღიაა");
	if (role == 3)
	{
		return 0;
	}
	House_UtilText(bool:g_HouseRows[idx][hrLuceOn], bool:g_HouseRows[idx][hrLuceCut], st1, 36);
	House_UtilText(bool:g_HouseRows[idx][hrGasOn], bool:g_HouseRows[idx][hrGasCut], st2, 36);
	SendClientMessage(playerid, -1, "შუქი: %s - გაზი: %s - მაცივარი: %d/%d", st1, st2, g_HouseRows[idx][hrFridgeUnits], s_HsFridgeCap[tier]);
	if (role == 1)
	{
		SendClientMessage(playerid, -1, "გადაუხდელი გადასახადები: %d - გაცემული გასაღებები: %d/%d", g_HouseRows[idx][hrBills], g_HouseRows[idx][hrKeyCount], 3);
	}
	return 0;
}

House_CmdCasa(playerid, const params[])
{
	if (!House_CmdGuard(playerid, false))
	{
		return 0;
	}
	new pos = 0, sub[96], arg[96];
	Util_NextToken(params, pos, sub, 96);
	new idx = House_Context(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "სახლში ან მის კართან უნდა იყო.");
		return 0;
	}
	if (g_HPl[playerid][hpPendingOp] != 0)
	{
		Util_Send(playerid, -10270721, "სახლის ოპერაცია მიმდინარეობს: დაელოდე.");
		return 0;
	}
	if (sub[0] == 0)
	{
		House_OpenMenu(playerid, idx);
		return 0;
	}
	if (!strcmp(sub, "info", true, 0x7FFFFFFF))
	{
		House_Info(playerid, idx);
		return 0;
	}
	if (!strcmp(sub, "serratura", true, 0x7FFFFFFF))
	{
		House_ToggleLock(playerid, idx);
		return 0;
	}
	if (!strcmp(sub, "chiavi", true, 0x7FFFFFFF))
	{
		House_LoadKeys(playerid, 3, "");
		return 0;
	}
	if (!strcmp(sub, "chiave", true, 0x7FFFFFFF))
	{
		Util_NextToken(params, pos, arg, 96);
		if (arg[0] == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /casa chiave [მოთამაშის id]");
			return 0;
		}
		House_GiveKey(playerid, arg);
		return 0;
	}
	if (!strcmp(sub, "revoca", true, 0x7FFFFFFF))
	{
		Util_NextToken(params, pos, arg, 96);
		if (arg[0] == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /casa revoca [მოთამაშის id | სახელი_გვარი | ნომერი /casa chiavi-დან]");
			return 0;
		}
		House_LoadKeys(playerid, 4, arg);
		return 0;
	}
	if (!strcmp(sub, "vendi", true, 0x7FFFFFFF))
	{
		Util_NextToken(params, pos, arg, 96);
		House_DoSell(playerid, idx, !strcmp(arg, "conferma", true, 0x7FFFFFFF));
		return 0;
	}
	if (!strcmp(sub, "compra", true, 0x7FFFFFFF))
	{
		if (g_HouseRows[idx][hrOwnerId] > 0 || House_FindNear(playerid) != idx)
		{
			Util_Send(playerid, -10270721, "გასაყიდი სახლის კართან უნდა იყო.");
			return 0;
		}
		Util_NextToken(params, pos, arg, 96);
		if (arg[0] == 0)
		{
			House_OpenMenu(playerid, idx);
			return 0;
		}
		if (!strcmp(arg, "contanti", true, 0x7FFFFFFF))
		{
			House_DoBuy(playerid, idx, 0);
			return 0;
		}
		if (!strcmp(arg, "banca", true, 0x7FFFFFFF))
		{
			House_DoBuy(playerid, idx, 1);
			return 0;
		}
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /casa compra [contanti|banca]");
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /casa [info|compra|vendi|serratura|chiave|chiavi|revoca]");
	return 0;
}

House_CmdEntra(playerid)
{
	if (!House_CmdGuard(playerid, true))
	{
		return 0;
	}
	if (House_FindInside(playerid) != -1)
	{
		Util_Send(playerid, -10270721, "უკვე სახლში ხარ.");
		return 0;
	}
	new idx = House_FindNear(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "სახლის კართან არ ხარ.");
		return 0;
	}
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
	{
		Util_Send(playerid, -10270721, "შესასვლელად ჩამოდი მანქანიდან.");
		return 0;
	}
	if (g_HouseRows[idx][hrOwnerId] <= 0)
	{
		Util_Send(playerid, -10270721, "სახლი იყიდება და დათვალიერება არ შეიძლება. ინფორმაცია: /casa.");
		return 0;
	}
	if (!House_CanEnterNow(idx, playerid))
	{
		Util_Send(playerid, -10270721, "კარი ჩაკეტილია. სცადე /bussa.");
		return 0;
	}
	House_DoEnter(playerid, idx);
	SendClientMessage(playerid, -1, "შეხვედი სახლში %d. გასასვლელად: /esci.", g_HouseRows[idx][hrId]);
	return 0;
}

House_CmdEsci(playerid)
{
	if (!House_CmdGuard(playerid, true))
	{
		return 0;
	}
	new idx = House_FindInside(playerid);
	if (idx == -1)
	{
		Util_Send(playerid, -10270721, "სახლში არ ხარ.");
		return 0;
	}
	if (!House_NearExit(playerid, idx))
	{
		Util_Send(playerid, -10270721, "გასასვლელად კართან მიდი.");
		return 0;
	}
	House_PlaceOutside(playerid, idx);
	return 0;
}

House_CmdBussa(playerid)
{
	if (!House_CmdGuard(playerid, true))
	{
		return 0;
	}
	new idx = House_FindNear(playerid);
	if (idx == -1 || g_HouseRows[idx][hrOwnerId] <= 0)
	{
		Util_Send(playerid, -10270721, "დასახლებული სახლის კართან არ ხარ.");
		return 0;
	}
	if (!g_HouseRows[idx][hrLocked])
	{
		Util_Send(playerid, -1, "კარი ღიაა: გამოიყენე /entra.");
		return 0;
	}
	new now = GetTickCount();
	if (now - g_HPl[playerid][hpKnockTick] < 5000)
	{
		Util_Send(playerid, -10270721, "ახლახან დააკაკუნე: დაელოდე.");
		return 0;
	}
	g_HPl[playerid][hpKnockTick] = now;
	new id = g_HouseRows[idx][hrId], name[75], inside = 0;
	GetPlayerName(playerid, name, 75);
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_HPl[p][hpInsideHouse] != id || !Util_IsPlaying(p))
		{
			continue;
		}
		SendClientMessage(p, 0xC2A2DAFF, "* ვიღაც კარზე აკაკუნებს (%s).", name);
		inside++;
	}
	Util_Send(playerid, 0xC2A2DAFF, "* კარზე აკაკუნებ.");
	if (inside == 0)
	{
		Util_Send(playerid, -1, "სახლში არავინ ჩანს.");
	}
	return 0;
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
		Util_Send(playerid, -10270721, "ამ სახლთან ახლოს აღარ ხარ.");
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
		Util_Send(playerid, -10270721, "ოპერაცია მიმდინარეობს: დაელოდე.");
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
				Util_Send(playerid, -10270721, "კომუნალურებზე წვდომა არ გაქვს.");
				return 1;
			}
			House_PushUi(playerid, idx);
			UI_HouseUtilityShow(playerid);
		}
		case 3:
		{
			if (House_FindInside(playerid) != idx || !House_HasAccess(idx, playerid))
			{
				Util_Send(playerid, -10270721, "მაცივრის გამოსაყენებლად სახლში უნდა იყო.");
				return 1;
			}
			House_PushUi(playerid, idx);
			UI_HouseFridgeShow(playerid);
		}
		case 4:
		{
			if (House_Role(idx, playerid) != 1)
			{
				Util_Send(playerid, -10270721, "გადასახადებს მხოლოდ მფლობელი მართავს.");
				return 1;
			}
			House_OpenBills(playerid, idx);
		}
		case 5:
		{
			if (House_Role(idx, playerid) != 1)
			{
				Util_Send(playerid, -10270721, "გასაღებებს მხოლოდ მფლობელი მართავს.");
				return 1;
			}
			House_LoadKeys(playerid, 3, "");
		}
		case 6:
		{
			House_DoSell(playerid, idx, true);
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
					SendClientMessage(owner, -10270721, "სახლი %d: შუქის გარეშე მაცივარმა %d ულუფა დაკარგა.", g_HouseRows[idx][hrId], lost);
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
				House_HungerSet(p, g_HPl[p][hpHunger] - 1, true);
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
		g_HsReady = false;
		Log_Write("HOUSE", "caricamento case fallito: sistema case disabilitato");
		return 1;
	}
	for (new i = 0; i < count && i < HOUSE_MAX; i++)
	{
		g_HsBusy[i] = false;
		House_RefreshWorld(i);
	}
	g_HsReady = true;
	new msg[384];
	format(msg, 384, "caricate %d case", count);
	Log_Write("HOUSE", msg);
	return 1;
}

House_OnInit()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		House_ResetPlayer(p);
	}
	for (new i = 0; i < HOUSE_MAX; i++)
	{
		g_HsPickup[i] = -1;
		g_HsLabel[i] = Text3D:65535;
		g_HsBusy[i] = false;
		g_HsFridgeBusy[i] = false;
	}
	g_HsReady = false;
	for (new s = 0; s < 2; s++)
	{
		Lang_Create3DTextLabel("სუპერმარკეტი\n/compra - cibo, pasto, acqua, snack, kit, benda, zaino...", 0x66DD66FF, s_HsShops[s][0], s_HsShops[s][1], (0.9 + s_HsShops[s][2]), 12.0, 0, false);
	}
	g_HsTickTimer = SetTimer("House_OnTick", 60000, true);
	g_HsSaveTimer = SetTimer("House_OnSaveTick", 300000, true);
	DB_BootHouses();
}

House_SavePlayerFood(playerid)
{
	if (g_HPl[playerid][hpHungerState] != 2 || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	DB_SaveHunger(gPlayer[playerid][pCharId], g_HPl[playerid][hpHunger]);
	return 0;
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
	return 0;
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
	return 0;
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
		SendClientMessage(playerid, -1, "სახლი %d (%s) იყიდება: $%d, მინ. დონე %d. ინფო და შეძენა: /casa.", g_HouseRows[idx][hrId], s_HsTierName[tier], g_HouseRows[idx][hrPrice], s_HsTierLevel[tier]);
	}
	else if (House_CanEnterNow(idx, playerid))
	{
		SendClientMessage(playerid, -1, "სახლი %d, მფლობელი %s. შესასვლელად: /entra.", g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerName]);
	}
	else
	{
		SendClientMessage(playerid, -1, "სახლი %d, მფლობელი %s: კარი ჩაკეტილია. გამოიყენე /bussa.", g_HouseRows[idx][hrId], g_HouseRows[idx][hrOwnerName]);
	}
	return 0;
}
