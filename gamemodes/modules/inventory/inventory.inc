new s_InvName[12][88] = {
	{0},
	"საკვების ულუფა",
	"მზა კერძი",
	"წყლის ბოთლი",
	"სნექი",
	"პირველადი დახმარების ნაკრები",
	"ბინტი",
	"სიგარეტი",
	"სანთებელა",
	"ტელეფონი",
	"დოკუმენტი",
	"ზურგჩანთა"
};
new s_InvPrice[12] = {0, 9, 14, 3, 4, 60, 12, 8, 5, 250, 0, 350};
new s_InvCooldown[11] = {0, 0, 10000, 5000, 5000, 45000, 15000, 20000};
new s_InvShopKey[12][24] = {
	{0},
	"cibo",
	"pasto",
	"acqua",
	"snack",
	"kit",
	"benda",
	"sigarette",
	"accendino",
	"telefono",
	{0},
	"zaino"
};
new s_InvAliasName[16][24] = {
	"cibo",
	"porzione",
	"pasto",
	"acqua",
	"bottiglia",
	"snack",
	"kit",
	"benda",
	"bende",
	"sigarette",
	"sigaretta",
	"accendino",
	"telefono",
	"cellulare",
	"documento",
	"zaino"
};
new s_InvAliasId[16] = {1, 1, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 9, 9, 10, 11};
new g_InvGround[40][E_INV_GROUND];
new g_InvTickTimer;
new g_InvGiveSeq;

bool:Inv_ValidPlayer(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid);
}

Inv_Count(playerid, itemId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || itemId < 1 || itemId >= 11)
	{
		return 0;
	}
	new total = 0;
	for (new s = 0; s < 20; s++)
	{
		if (gPlayer[playerid][pInvItem][s] == itemId)
		{
			total += gPlayer[playerid][pInvQty][s];
		}
	}
	return total;
}

Inv_Capacity(playerid)
{
	return DB_InvCapacity(gPlayer[playerid][pInvHasBag] != 0);
}

bool:Inv_Ready(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && gPlayer[playerid][pInvState] == 2;
}

Inv_ParseItem(const token[])
{
	new value = 0;
	if (token[0] == 0)
	{
		return 0;
	}
	if (Util_ParseInt(token, value))
	{
		return value >= 1 && value <= 10 ? value : 0;
	}
	for (new i = 0; i < 16; i++)
	{
		if (!strcmp(token, s_InvAliasName[i], true, 0x7FFFFFFF))
		{
			return s_InvAliasId[i];
		}
	}
	return 0;
}

Inv_Say(playerid, color, const text[])
{
	Util_Send(playerid, color, text);
}

Inv_PushUi(playerid)
{
	new cap = Inv_Capacity(playerid);
	UI_InvClear(playerid);
	for (new s = 0; s < cap && s < 20; s++)
	{
		UI_InvSet(playerid, s, gPlayer[playerid][pInvItem][s], gPlayer[playerid][pInvQty][s]);
	}
	UI_InvMeta(playerid, gPlayer[playerid][pInvCount], cap, gPlayer[playerid][pInvHasBag] != 0);
}

Inv_ClearCache(playerid)
{
	for (new s = 0; s < 20; s++)
	{
		gPlayer[playerid][pInvItem][s] = 0;
		gPlayer[playerid][pInvQty][s] = 0;
	}
	gPlayer[playerid][pInvCount] = 0;
}

Inv_Load(playerid)
{
	if (!Inv_ValidPlayer(playerid) || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	if (gPlayer[playerid][pInvState] == 1)
	{
		return 0;
	}
	gPlayer[playerid][pInvState] = 1;
	DB_InvLoad(playerid, gPlayer[playerid][pCharId]);
	return 0;
}

Inv_Reload(playerid)
{
	if (!Inv_ValidPlayer(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pInvState] == 1)
	{
		gPlayer[playerid][pInvReloadQueued] = 1;
		return 0;
	}
	Inv_Load(playerid);
	return 0;
}

public Inv_OnDBLoaded(playerid, count)
{
	if (!Inv_ValidPlayer(playerid))
	{
		return 1;
	}
	if (count == -1 || count < 0)
	{
		gPlayer[playerid][pInvState] = 3;
		gPlayer[playerid][pInvReloadQueued] = 0;
		if (gPlayer[playerid][pFridgeHouse] > 0)
		{
			House_InvFridgeResult(playerid, gPlayer[playerid][pFridgeHouse], gPlayer[playerid][pFridgeDir], 0, -1);
			Log_Write("INV", "esito frigo incerto non riconciliato (reload fallito): verificare house_utilities.fridge_units");
			gPlayer[playerid][pFridgeHouse] = 0;
		}
		if (gPlayer[playerid][pSpawned])
		{
			Inv_Say(playerid, -10270721, "ინვენტარი მიუწვდომელია (ბაზის შეცდომა): სცადე /inventario-ით.");
		}
		return 1;
	}
	Inv_ClearCache(playerid);
	gPlayer[playerid][pInvHasBag] = g_InvBag[playerid] != 0 ? 1 : 0;
	new cap = Inv_Capacity(playerid);
	for (new r = 0; r < count && r < 20; r++)
	{
		new slot = g_InvRows[playerid][r][irSlot], item = g_InvRows[playerid][r][irItem], qty = g_InvRows[playerid][r][irQty];
		if (slot < 0 || slot >= cap || !DB_InvItemOk(item) || qty < 1 || qty > DB_InvStackMax(item) || gPlayer[playerid][pInvItem][slot] != 0)
		{
			Log_Write("INV", "riga inventario anomala scartata dalla cache");
			continue;
		}
		gPlayer[playerid][pInvItem][slot] = item;
		gPlayer[playerid][pInvQty][slot] = qty;
		gPlayer[playerid][pInvCount]++;
	}
	gPlayer[playerid][pInvState] = 2;
	if (gPlayer[playerid][pFridgeHouse] > 0)
	{
		new before = gPlayer[playerid][pFridgeFoodBefore], after = Inv_Count(playerid, 1);
		new moved = gPlayer[playerid][pFridgeDir] == 1 ? before - after : after - before;
		new house = gPlayer[playerid][pFridgeHouse], dir = gPlayer[playerid][pFridgeDir];
		gPlayer[playerid][pFridgeHouse] = 0;
		if (moved > 0)
		{
			House_InvFridgeResult(playerid, house, dir, moved, 1);
		}
		else
		{
			House_InvFridgeResult(playerid, house, dir, 0, 0);
		}
	}
	if (gPlayer[playerid][pInvReloadQueued])
	{
		gPlayer[playerid][pInvReloadQueued] = 0;
		Inv_Load(playerid);
		return 1;
	}
	if (count == 0 && Inv_Count(playerid, 10) == 0 && !gPlayer[playerid][pInvOpBusy])
	{
		Inv_StartDoc(playerid, true);
	}
	Inv_PushUi(playerid);
	return 1;
}

Inv_BeginOp(playerid, op, itemId, qty, aux)
{
	gPlayer[playerid][pInvOpBusy] = 1;
	gPlayer[playerid][pInvOp] = op;
	gPlayer[playerid][pInvOpItem] = itemId;
	gPlayer[playerid][pInvOpQty] = qty;
	gPlayer[playerid][pInvOpAux] = aux;
	gPlayer[playerid][pInvOpPeer] = 0;
	DB_InvMakeOpId(gPlayer[playerid][pInvOpId], 33, gPlayer[playerid][pCharId]);
}

Inv_EndOp(playerid)
{
	gPlayer[playerid][pInvOpBusy] = 0;
	gPlayer[playerid][pInvOp] = 0;
	gPlayer[playerid][pInvOpItem] = 0;
	gPlayer[playerid][pInvOpQty] = 0;
	gPlayer[playerid][pInvOpAux] = 0;
	gPlayer[playerid][pInvOpPeer] = 0;
}

bool:Inv_OpIs(playerid, op)
{
	return Inv_ValidPlayer(playerid) && gPlayer[playerid][pInvOpBusy] != 0 && gPlayer[playerid][pInvOp] == op;
}

bool:Inv_StateOk(playerid)
{
	if (!Util_IsPlaying(playerid))
	{
		return false;
	}
	return gPlayer[playerid][pInvState] == 2 && !gPlayer[playerid][pInvOpBusy] && !gPlayer[playerid][pWounded] && !gPlayer[playerid][pCuffed];
}

bool:Inv_Guard(playerid)
{
	if (!Util_IsPlaying(playerid))
	{
		return false;
	}
	switch (gPlayer[playerid][pInvState])
	{
		case 2:
		{
		}
		case 1:
		{
			Inv_Say(playerid, -10270721, "ინვენტარი იტვირთება: ერთ წამში სცადე.");
			return false;
		}
		default:
		{
			Inv_Say(playerid, -10270721, "ინვენტარი მიუწვდომელია: ვცდილობ ხელახლა ჩატვირთვას, ცოტა ხანში სცადე.");
			Inv_Load(playerid);
			return false;
		}
	}
	if (gPlayer[playerid][pInvOpBusy])
	{
		Inv_Say(playerid, -10270721, "ინვენტარის ოპერაცია მიმდინარეობს: დაელოდე.");
		return false;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Inv_Say(playerid, -10270721, "დაჭრილს ან ბორკილდადებულს ეს არ შეგიძლია.");
		return false;
	}
	new now = GetTickCount();
	if (gPlayer[playerid][pInvLastActionTick] != 0 && now - gPlayer[playerid][pInvLastActionTick] < 1500)
	{
		Inv_Say(playerid, -10270721, "ინვენტარით შემდეგ მოქმედებამდე ცოტა დაელოდე.");
		return false;
	}
	gPlayer[playerid][pInvLastActionTick] = now != 0 ? now : 1;
	return true;
}

Inv_CmdInventario(playerid)
{
	if (gPlayer[playerid][pInvState] == 1)
	{
		Inv_Say(playerid, -10270721, "ინვენტარი იტვირთება: ერთ წამში სცადე.");
		return 0;
	}
	if (gPlayer[playerid][pInvState] != 2)
	{
		Inv_Say(playerid, -10270721, "ინვენტარი მიუწვდომელია: ვცდილობ ხელახლა ჩატვირთვას, ცოტა ხანში სცადე.");
		Inv_Load(playerid);
		return 0;
	}
	Inv_PushUi(playerid);
	UI_InvShow(playerid);
	return 0;
}

Inv_CdLeftSec(playerid, itemId)
{
	new until = gPlayer[playerid][pInvItemCdUntil][itemId];
	if (until == 0)
	{
		return 0;
	}
	new left = until - GetTickCount();
	if (left <= 0)
	{
		return 0;
	}
	return (left + 1000 - 1) / 1000;
}

Inv_UseStart(playerid, itemId)
{
	if (!Inv_Guard(playerid))
	{
		return 0;
	}
	if (itemId < 1 || itemId > 10)
	{
		Inv_Say(playerid, -10270721, "უცნობი ნივთი.");
		return 0;
	}
	if (Inv_Count(playerid, itemId) < 1)
	{
		SendClientMessage(playerid, -10270721, "არ გაქვს: %s.", s_InvName[itemId]);
		return 0;
	}
	switch (itemId)
	{
		case 1:
		{
			Inv_Say(playerid, -10270721, "საკვების ულუფა პირდაპირ არ გამოიყენება: ჩადე მაცივარში (/frigo metti) და მოამზადე ან ცივად ჭამე.");
			return 0;
		}
		case 8:
		{
			Inv_Say(playerid, -10270721, "სანთებელა მოსაწევადაა: გამოიყენე /usa sigarette.");
			return 0;
		}
		case 9:
		{
			SendClientMessage(playerid, -1, "ტელეფონი: შენი ნომერია %d.", gPlayer[playerid][pCharId] + 555000);
			return 0;
		}
		case 10:
		{
			Inv_ShowDocument(playerid, playerid);
			return 0;
		}
	}
	new left = Inv_CdLeftSec(playerid, itemId);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "ხელახლა გამოსაყენებლად კიდევ %d წამი დაელოდე: %s.", left, s_InvName[itemId]);
		return 0;
	}
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	switch (itemId)
	{
		case 2 .. 4:
		{
			new hunger = House_InvHunger(playerid);
			if (hunger < 0)
			{
				Inv_Say(playerid, -10270721, "საკვების მონაცემები ჯერ არ ჩატვირთულა: სცადე ცოტა ხანში.");
				return 0;
			}
			if (hunger >= 100)
			{
				Inv_Say(playerid, -10270721, "არ გშია.");
				return 0;
			}
		}
		case 5:
		{
			if ((hp >= float(90)))
			{
				Inv_Say(playerid, -10270721, "პირველადი დახმარების ნაკრები არ გჭირდება: ჯანმრთელად ხარ.");
				return 0;
			}
		}
		case 6:
		{
			if ((hp >= float(95)))
			{
				Inv_Say(playerid, -10270721, "ბინტის საჭირო ჭრილობა არ გაქვს.");
				return 0;
			}
		}
		case 7:
		{
			if (Inv_Count(playerid, 8) < 1)
			{
				Inv_Say(playerid, -10270721, "მოსაწევად სანთებელა გჭირდება.");
				return 0;
			}
		}
	}
	Inv_BeginOp(playerid, 1, itemId, 1, 0);
	DB_InvUse(playerid, gPlayer[playerid][pCharId], itemId, gPlayer[playerid][pInvOpId]);
	return 0;
}

Inv_RpUse(playerid, const action[])
{
	new name[75], msg[256];
	GetPlayerName(playerid, name, 75);
	format(msg, 256, "* %s %s", name, action);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
}

Inv_ApplyUse(playerid, itemId)
{
	new Float:hp;
	switch (itemId)
	{
		case 2 .. 4:
		{
			new gain = itemId == 2 ? 30 : (itemId == 3 ? 8 : 10);
			new value = House_InvHungerAdd(playerid, gain);
			Inv_RpUse(playerid, itemId == 2 ? "მზა კერძს მიირთმევს." : (itemId == 3 ? "წყალს სვამს." : "სნექს ჭამს."));
			if (value >= 0)
			{
				SendClientMessage(playerid, 0x66DD66FF, "სიმაძღრე: %d%%.", value);
			}
		}
		case 5, 6:
		{
			GetPlayerHealth(playerid, hp);
			hp += float(itemId == 5 ? 25 : 8);
			if ((hp > 100.0))
			{
				hp = 100.0;
			}
			SetPlayerHealth(playerid, hp);
			Inv_RpUse(playerid, itemId == 5 ? "პირველადი დახმარების ნაკრებს იყენებს." : "ჭრილობას ბინტით იხვევს.");
			SendClientMessage(playerid, 0x66DD66FF, "ჯანმრთელობა: %d.", floatround(hp, floatround_method:0));
		}
		case 7:
		{
			Inv_RpUse(playerid, "სიგარეტს უკიდებს.");
		}
	}
	new cd = s_InvCooldown[itemId];
	if (cd > 0)
	{
		new until = GetTickCount() + cd;
		gPlayer[playerid][pInvItemCdUntil][itemId] = until != 0 ? until : 1;
	}
}

public Inv_OnDBUsed(playerid, itemId, result)
{
	if (!Inv_OpIs(playerid, 1))
	{
		return 1;
	}
	Inv_EndOp(playerid);
	switch (result)
	{
		case 1:
		{
			Inv_Reload(playerid);
			UI_InvToast(playerid, itemId, -1);
			if (Util_IsPlaying(playerid) && !gPlayer[playerid][pWounded])
			{
				Inv_ApplyUse(playerid, itemId);
			}
			else
			{
				Inv_Say(playerid, -10270721, "ნივთი დაკარგე: მისი გამოყენების მდგომარეობაში არ იყავი.");
			}
		}
		case 3:
		{
			Inv_Say(playerid, -10270721, "ეს ნივთი აღარ გაქვს.");
			Inv_Reload(playerid);
		}
		case -1:
		{
			Inv_Say(playerid, -10270721, "ბაზის შეცდომა: ნივთი არ გამოყენებულა. შეამოწმე ინვენტარი და სცადე თავიდან.");
			Inv_Reload(playerid);
		}
		default:
		{
			Inv_Say(playerid, -10270721, "ნივთის გამოყენება ვერ მოხერხდა.");
		}
	}
	return 1;
}

public UI_OnInvUse(playerid, slot)
{
	if (!Inv_ValidPlayer(playerid) || slot < 0 || slot >= Inv_Capacity(playerid))
	{
		return 1;
	}
	new itemId = gPlayer[playerid][pInvItem][slot];
	if (itemId > 0)
	{
		Inv_UseStart(playerid, itemId);
	}
	return 1;
}

Inv_GroundCount(charId)
{
	new n = 0;
	for (new i = 0; i < 40; i++)
	{
		if (g_InvGround[i][igState] == 0)
		{
			continue;
		}
		if (charId == 0 || g_InvGround[i][igDropperChar] == charId)
		{
			n++;
		}
	}
	return n;
}

Inv_GroundFree(idx)
{
	if (idx < 0 || idx >= 40)
	{
		return 0;
	}
	if (g_InvGround[idx][igObject] != 65535)
	{
		DestroyObject(g_InvGround[idx][igObject]);
	}
	g_InvGround[idx][igState] = 0;
	g_InvGround[idx][igItemId] = 0;
	g_InvGround[idx][igQty] = 0;
	g_InvGround[idx][igObject] = 65535;
	g_InvGround[idx][igExpireTick] = 0;
	g_InvGround[idx][igDropper] = 0;
	g_InvGround[idx][igDropperChar] = 0;
	g_InvGround[idx][igPicker] = 0;
	return 0;
}

Inv_GroundSlot()
{
	for (new i = 0; i < 40; i++)
	{
		if (g_InvGround[i][igState] == 0)
		{
			return i;
		}
	}
	return -1;
}

Inv_GroundRelease(playerid)
{
	for (new i = 0; i < 40; i++)
	{
		if (g_InvGround[i][igState] == 1 && g_InvGround[i][igDropper] == playerid + 1)
		{
			Inv_GroundFree(i);
		}
		else if (g_InvGround[i][igPicker] == playerid + 1)
		{
			g_InvGround[i][igPicker] = 0;
		}
	}
}

Inv_CmdGetta(playerid, const params[])
{
	new pos = 0, tok[72], itemId = 0, qty = 1;
	Util_NextToken(params, pos, tok, 72);
	itemId = Inv_ParseItem(tok);
	if (itemId < 1)
	{
		Inv_Say(playerid, 0x99CCFFFF, "გამოყენება: /getta [ნივთი] [რაოდენობა] (cibo, pasto, acqua, snack, kit, benda, sigarette, accendino)");
		return 0;
	}
	Util_NextToken(params, pos, tok, 72);
	if (tok[0] != 0 && (!Util_ParseInt(tok, qty) || qty < 1 || qty > 1000))
	{
		Inv_Say(playerid, -10270721, "არასწორი რაოდენობა.");
		return 0;
	}
	Inv_DropStart(playerid, itemId, qty);
	return 0;
}

Inv_DropStart(playerid, itemId, qty)
{
	if (!Inv_Guard(playerid))
	{
		return 0;
	}
	if (!DB_InvItemMovable(itemId))
	{
		Inv_Say(playerid, -10270721, "ამ ნივთის გადაგდება არ შეიძლება.");
		return 0;
	}
	if (qty < 1 || Inv_Count(playerid, itemId) < qty)
	{
		SendClientMessage(playerid, -10270721, "საკმარისად არ გაქვს: %s.", s_InvName[itemId]);
		return 0;
	}
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Inv_Say(playerid, -10270721, "ნივთების გადაგდება მხოლოდ ფეხით, გარეთ შეგიძლია.");
		return 0;
	}
	new charId = gPlayer[playerid][pCharId];
	if (Inv_GroundCount(0) >= 40)
	{
		Inv_Say(playerid, -10270721, "გარშემო ძალიან ბევრი ნივთი ყრია მიწაზე: მოგვიანებით სცადე.");
		return 0;
	}
	if (Inv_GroundCount(charId) >= 3)
	{
		SendClientMessage(playerid, -10270721, "მიწაზე უკვე %d ნივთი გიყრია: აკრიფე ან დაელოდე, სანამ გაქრება.", 3);
		return 0;
	}
	new idx = Inv_GroundSlot();
	if (idx == -1)
	{
		Inv_Say(playerid, -10270721, "გარშემო ძალიან ბევრი ნივთი ყრია მიწაზე: მოგვიანებით სცადე.");
		return 0;
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	g_InvGround[idx][igState] = 1;
	g_InvGround[idx][igItemId] = itemId;
	g_InvGround[idx][igQty] = qty;
	g_InvGround[idx][igObject] = 65535;
	g_InvGround[idx][igPosX] = x;
	g_InvGround[idx][igPosY] = y;
	g_InvGround[idx][igPosZ] = (z - 0.9);
	g_InvGround[idx][igDropper] = playerid + 1;
	g_InvGround[idx][igDropperChar] = charId;
	g_InvGround[idx][igPicker] = 0;
	Inv_BeginOp(playerid, 3, itemId, qty, idx);
	DB_InvDrop(playerid, charId, itemId, qty, gPlayer[playerid][pInvOpId]);
	return 0;
}

public Inv_OnDBDropped(playerid, itemId, qty, result)
{
	if (!Inv_OpIs(playerid, 3))
	{
		return 1;
	}
	new idx = gPlayer[playerid][pInvOpAux];
	new opId[99];
	strcat(opId, gPlayer[playerid][pInvOpId], 99);
	Inv_EndOp(playerid);
	new bool:mine = idx >= 0 && idx < 40 && g_InvGround[idx][igState] == 1 && g_InvGround[idx][igDropper] == playerid + 1;
	if (result != 1)
	{
		if (mine)
		{
			Inv_GroundFree(idx);
		}
		switch (result)
		{
			case 3:
			{
				Inv_Say(playerid, -10270721, "ეს ნივთი აღარ გაქვს.");
				Inv_Reload(playerid);
			}
			case 4:
			{
				Inv_Say(playerid, -10270721, "ამ ნივთის გადაგდება არ შეიძლება.");
			}
			case -1:
			{
				Inv_Say(playerid, -10270721, "ბაზის შეცდომა: ნივთი არ გადაგდებულა.");
				Inv_Reload(playerid);
			}
			default:
			{
				Inv_Say(playerid, -10270721, "ნივთის გადაგდება ვერ მოხერხდა.");
			}
		}
		return 1;
	}
	if (!mine)
	{
		Log_Write("INV", "drop riuscito senza riserva a terra: annullo");
		DB_InvUndo(opId);
		Inv_Say(playerid, -10270721, "ნივთის გადაგდება ვერ მოხერხდა.");
		Inv_Reload(playerid);
		return 1;
	}
	new model = itemId == 5 ? 11736 : 1264;
	g_InvGround[idx][igObject] = CreateObject(model, g_InvGround[idx][igPosX], g_InvGround[idx][igPosY], g_InvGround[idx][igPosZ], 0.0, 0.0, 0.0, 40.0);
	g_InvGround[idx][igState] = 2;
	g_InvGround[idx][igExpireTick] = GetTickCount() + 600000;
	g_InvGround[idx][igDropper] = 0;
	Inv_Reload(playerid);
	UI_InvToast(playerid, itemId, -qty);
	new text[432];
	format(text, 432, "მიწაზე აგდებს: %s x%d.", s_InvName[itemId], qty);
	Inv_RpUse(playerid, text);
	return 1;
}

public UI_OnInvDrop(playerid, slot, qty)
{
	if (!Inv_ValidPlayer(playerid) || slot < 0 || slot >= Inv_Capacity(playerid))
	{
		return 1;
	}
	new itemId = gPlayer[playerid][pInvItem][slot];
	if (itemId > 0)
	{
		Inv_DropStart(playerid, itemId, qty);
	}
	return 1;
}

Inv_CmdPrendi(playerid)
{
	if (!Inv_Guard(playerid))
	{
		return 0;
	}
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Inv_Say(playerid, -10270721, "ნივთების აკრეფა მხოლოდ ფეხით, გარეთ შეგიძლია.");
		return 0;
	}
	new best = -1, Float:bestDist = (2.0 + 1.0);
	for (new i = 0; i < 40; i++)
	{
		if (g_InvGround[i][igState] != 2 || g_InvGround[i][igPicker] != 0)
		{
			continue;
		}
		new Float:d = GetPlayerDistanceFromPoint(playerid, g_InvGround[i][igPosX], g_InvGround[i][igPosY], (0.9 + g_InvGround[i][igPosZ]));
		if ((d <= 2.0) && (d < bestDist))
		{
			best = i;
			bestDist = d;
		}
	}
	if (best == -1)
	{
		Inv_Say(playerid, -10270721, "ახლოს მიწაზე ნივთი არ არის.");
		return 0;
	}
	g_InvGround[best][igPicker] = playerid + 1;
	Inv_BeginOp(playerid, 4, g_InvGround[best][igItemId], g_InvGround[best][igQty], best);
	DB_InvAdd(playerid, gPlayer[playerid][pCharId], g_InvGround[best][igItemId], g_InvGround[best][igQty], gPlayer[playerid][pInvOpId], "pickup", 0);
	return 0;
}

public Inv_OnDBAdded(playerid, itemId, qty, result)
{
	if (Inv_OpIs(playerid, 4))
	{
		new idx = gPlayer[playerid][pInvOpAux];
		Inv_EndOp(playerid);
		new bool:mine = idx >= 0 && idx < 40 && g_InvGround[idx][igPicker] == playerid + 1 && g_InvGround[idx][igState] == 2;
		if (result == 1)
		{
			if (mine)
			{
				Inv_GroundFree(idx);
			}
			Inv_Reload(playerid);
			UI_InvToast(playerid, itemId, qty);
			SendClientMessage(playerid, 0x66DD66FF, "აკრიფე: %s x%d.", s_InvName[itemId], qty);
			return 1;
		}
		if (mine)
		{
			g_InvGround[idx][igPicker] = 0;
		}
		switch (result)
		{
			case 2:
			{
				Inv_Say(playerid, -10270721, "ინვენტარში საკმარისი ადგილი არ გაქვს.");
			}
			case -1:
			{
				Inv_Say(playerid, -10270721, "ბაზის შეცდომა: ნივთი ვერ აიკრიფა.");
			}
			default:
			{
				Inv_Say(playerid, -10270721, "ნივთის აკრეფა ვერ მოხერხდა.");
			}
		}
		return 1;
	}
	if (Inv_OpIs(playerid, 8))
	{
		new bool:starter = gPlayer[playerid][pInvOpAux] == 1;
		Inv_EndOp(playerid);
		if (starter)
		{
			if (result == 1)
			{
				Inv_Reload(playerid);
				UI_InvToast(playerid, itemId, qty);
			}
			else
			{
				Log_Write("INV", "documento iniziale non consegnato (si riprova al login)");
			}
			return 1;
		}
		if (result == 1)
		{
			Inv_Reload(playerid);
			UI_InvToast(playerid, itemId, qty);
			Inv_Say(playerid, 0x66DD66FF, "ახალი პირადობის მოწმობა მოითხოვე.");
		}
		else
		{
			gPlayer[playerid][pDocRequestTick] = 0;
			Inv_Say(playerid, -10270721, result == 2 ? "დოკუმენტისთვის ინვენტარში ადგილი არ გაქვს." : "დოკუმენტის გაცემა ვერ მოხერხდა.");
		}
	}
	return 1;
}

public Inv_IssueStartDoc(playerid)
{
	return Inv_StartDoc(playerid, false);
}

Inv_StartDoc(playerid, bool:retry)
{
	if (!Inv_ValidPlayer(playerid) || gPlayer[playerid][pCharId] <= 0 || gPlayer[playerid][pInvOpBusy])
	{
		return 0;
	}
	Inv_BeginOp(playerid, 8, 10, 1, 1);
	if (!retry)
	{
		format(gPlayer[playerid][pInvOpId], 33, "startdoc%d", gPlayer[playerid][pCharId]);
	}
	DB_InvAdd(playerid, gPlayer[playerid][pCharId], 10, 1, gPlayer[playerid][pInvOpId], "add", 1);
	return 1;
}

Inv_DocRequest(playerid)
{
	if (!Inv_Guard(playerid))
	{
		return 0;
	}
	if (Inv_Count(playerid, 10) > 0)
	{
		Inv_Say(playerid, -10270721, "პირადობის მოწმობა უკვე გაქვს.");
		return 0;
	}
	new now = GetTickCount();
	if (gPlayer[playerid][pDocRequestTick] != 0 && now - gPlayer[playerid][pDocRequestTick] < 3600000)
	{
		Inv_Say(playerid, -10270721, "დოკუმენტი უკვე მოითხოვე ახლახან: მოიცადე.");
		return 0;
	}
	gPlayer[playerid][pDocRequestTick] = now != 0 ? now : 1;
	Inv_BeginOp(playerid, 8, 10, 1, 0);
	DB_InvAdd(playerid, gPlayer[playerid][pCharId], 10, 1, gPlayer[playerid][pInvOpId], "add", 1);
	return 0;
}

Inv_GiveCancel(giver, const reason[])
{
	if (!Inv_ValidPlayer(giver) || gPlayer[giver][pInvGiveTo] == 0)
	{
		return 0;
	}
	new recv = gPlayer[giver][pInvGiveTo] - 1;
	gPlayer[giver][pInvGiveTo] = 0;
	gPlayer[giver][pInvGiveItem] = 0;
	gPlayer[giver][pInvGiveQty] = 0;
	Inv_Say(giver, -10270721, reason);
	if (Inv_ValidPlayer(recv) && gPlayer[recv][pInvGiveFrom] == giver + 1)
	{
		gPlayer[recv][pInvGiveFrom] = 0;
		UI_InvGiveConfirmClose(recv);
		Inv_Say(recv, -10270721, reason);
	}
	return 0;
}

bool:Inv_GiveRateOk(playerid)
{
	new now = GetTickCount();
	if (gPlayer[playerid][pInvGiveHourStart] == 0 || now - gPlayer[playerid][pInvGiveHourStart] >= 3600000)
	{
		gPlayer[playerid][pInvGiveHourStart] = now != 0 ? now : 1;
		gPlayer[playerid][pInvGiveHourCount] = 0;
	}
	return gPlayer[playerid][pInvGiveHourCount] < 20;
}

Inv_CmdDai(playerid, const params[])
{
	new pos = 0, tok[72], target = 0, itemId = 0, qty = 1;
	Util_NextToken(params, pos, tok, 72);
	if (!Util_ParsePlayerId(tok, target))
	{
		Inv_Say(playerid, 0x99CCFFFF, "გამოყენება: /dai [მოთამაშის id] [ნივთი] [რაოდენობა]");
		return 0;
	}
	Util_NextToken(params, pos, tok, 72);
	itemId = Inv_ParseItem(tok);
	if (itemId < 1)
	{
		Inv_Say(playerid, 0x99CCFFFF, "გამოყენება: /dai [მოთამაშის id] [ნივთი] [რაოდენობა] (cibo, pasto, acqua, snack, kit, benda, sigarette, accendino)");
		return 0;
	}
	Util_NextToken(params, pos, tok, 72);
	if (tok[0] != 0 && (!Util_ParseInt(tok, qty) || qty < 1 || qty > 1000))
	{
		Inv_Say(playerid, -10270721, "არასწორი რაოდენობა.");
		return 0;
	}
	Inv_GiveStart(playerid, target, itemId, qty);
	return 0;
}

Inv_GiveStart(playerid, target, itemId, qty)
{
	if (!Inv_Guard(playerid))
	{
		return 0;
	}
	if (!DB_InvItemMovable(itemId))
	{
		Inv_Say(playerid, -10270721, "ამ ნივთის გადაცემა შეუძლებელია.");
		return 0;
	}
	if (target == playerid)
	{
		Inv_Say(playerid, -10270721, "საკუთარ თავს ნივთს ვერ გადასცემ.");
		return 0;
	}
	if (!Util_IsPlaying(target) || gPlayer[target][pInvState] != 2)
	{
		Inv_Say(playerid, -10270721, "მოთამაშე არასწორია ან ჯერ მზად არ არის.");
		return 0;
	}
	if (!Util_InRange(playerid, target, 3.0))
	{
		Inv_Say(playerid, -10270721, "მოთამაშე ძალიან შორსაა.");
		return 0;
	}
	if (qty < 1 || Inv_Count(playerid, itemId) < qty)
	{
		SendClientMessage(playerid, -10270721, "საკმარისი არ გაქვს: %s.", s_InvName[itemId]);
		return 0;
	}
	if (gPlayer[playerid][pInvGiveTo] != 0 || gPlayer[playerid][pInvGiveFrom] != 0)
	{
		Inv_Say(playerid, -10270721, "უკვე გაქვს მოლოდინში მყოფი გაცვლა.");
		return 0;
	}
	if (gPlayer[target][pInvGiveTo] != 0 || gPlayer[target][pInvGiveFrom] != 0)
	{
		Inv_Say(playerid, -10270721, "ამ მოთამაშეს უკვე აქვს მოლოდინში მყოფი გაცვლა.");
		return 0;
	}
	new now = GetTickCount();
	if (gPlayer[playerid][pInvLastGiveTick] != 0 && now - gPlayer[playerid][pInvLastGiveTick] < 5000)
	{
		Inv_Say(playerid, -10270721, "ახალ გაცვლამდე რამდენიმე წამი მოიცადე.");
		return 0;
	}
	if (!Inv_GiveRateOk(playerid))
	{
		SendClientMessage(playerid, -10270721, "მიაღწიე საათში %d გაცვლის ლიმიტს.", 20);
		return 0;
	}
	g_InvGiveSeq++;
	gPlayer[playerid][pInvLastGiveTick] = now != 0 ? now : 1;
	gPlayer[playerid][pInvGiveStartTick] = now;
	gPlayer[playerid][pInvGiveSeq] = g_InvGiveSeq;
	gPlayer[playerid][pInvGiveTo] = target + 1;
	gPlayer[playerid][pInvGiveItem] = itemId;
	gPlayer[playerid][pInvGiveQty] = qty;
	gPlayer[target][pInvGiveFrom] = playerid + 1;
	gPlayer[target][pInvGiveSeq] = g_InvGiveSeq;
	gPlayer[target][pInvGiveStartTick] = now;
	new name[75], tname[75];
	GetPlayerName(playerid, name, 75);
	GetPlayerName(target, tname, 75);
	UI_InvGiveConfirmShow(target, name, itemId, qty, 15);
	SendClientMessage(playerid, -1, "შესთავაზე %s x%d მოთამაშეს %s: დაელოდე პასუხს (%d წამი).", s_InvName[itemId], qty, tname, 15);
	return 0;
}

public UI_OnInvGive(playerid, slot, targetId, qty)
{
	if (!Inv_ValidPlayer(playerid) || slot < 0 || slot >= Inv_Capacity(playerid))
	{
		return 1;
	}
	new itemId = gPlayer[playerid][pInvItem][slot];
	if (itemId > 0)
	{
		Inv_GiveStart(playerid, targetId, itemId, qty);
	}
	return 1;
}

public UI_OnInvGiveReply(playerid, accept)
{
	return Inv_GiveReply(playerid, accept, false);
}

public UI_OnInvGiveExpired(playerid)
{
	return Inv_GiveReply(playerid, 0, true);
}

Inv_GiveReply(playerid, accept, bool:expired)
{
	if (!Inv_ValidPlayer(playerid) || gPlayer[playerid][pInvGiveFrom] == 0)
	{
		return 1;
	}
	new giver = gPlayer[playerid][pInvGiveFrom] - 1, seq = gPlayer[playerid][pInvGiveSeq], start = gPlayer[playerid][pInvGiveStartTick];
	gPlayer[playerid][pInvGiveFrom] = 0;
	UI_InvGiveConfirmClose(playerid);
	if (!Inv_ValidPlayer(giver) || gPlayer[giver][pInvGiveTo] != playerid + 1 || gPlayer[giver][pInvGiveSeq] != seq || gPlayer[giver][pInvOp] == 2)
	{
		Inv_Say(playerid, -10270721, "გაცვლა აღარ არის მოქმედი.");
		return 1;
	}
	if (!accept)
	{
		Inv_GiveCancel(giver, expired ? "გაცვლის დრო ამოიწურა." : "გაცვლა უარყოფილია.");
		return 1;
	}
	new itemId = gPlayer[giver][pInvGiveItem], qty = gPlayer[giver][pInvGiveQty];
	if (GetTickCount() - start > 15000)
	{
		Inv_GiveCancel(giver, "გაცვლის დრო ამოიწურა.");
		Inv_Say(playerid, -10270721, "გაცვლა არ დასრულდა.");
		return 1;
	}
	if (!Inv_StateOk(giver) || !Inv_StateOk(playerid))
	{
		Inv_GiveCancel(giver, "გაცვლა გაუქმდა: ერთ-ერთს ახლა ეს არ შეუძლია.");
		Inv_Say(playerid, -10270721, "გაცვლა არ დასრულდა.");
		return 1;
	}
	if (!Util_InRange(giver, playerid, 3.0))
	{
		Inv_GiveCancel(giver, "გაცვლა გაუქმდა: ძალიან შორს ხართ.");
		Inv_Say(playerid, -10270721, "გაცვლა არ დასრულდა.");
		return 1;
	}
	if (Inv_Count(giver, itemId) < qty)
	{
		Inv_GiveCancel(giver, "გაცვლა გაუქმდა: ნივთი აღარ არის ხელმისაწვდომი.");
		Inv_Say(playerid, -10270721, "გაცვლა არ დასრულდა.");
		return 1;
	}
	Inv_BeginOp(giver, 2, itemId, qty, 0);
	gPlayer[giver][pInvOpPeer] = playerid + 1;
	DB_InvTransfer(giver, playerid, itemId, qty, gPlayer[giver][pInvOpId]);
	return 1;
}

public Inv_OnDBTransferred(giverid, receiverid, itemId, qty, result)
{
	if (Inv_OpIs(giverid, 2))
	{
		Inv_EndOp(giverid);
		gPlayer[giverid][pInvGiveTo] = 0;
		gPlayer[giverid][pInvGiveItem] = 0;
		gPlayer[giverid][pInvGiveQty] = 0;
		switch (result)
		{
			case 1:
			{
				gPlayer[giverid][pInvGiveHourCount]++;
				Inv_Reload(giverid);
				UI_InvToast(giverid, itemId, -qty);
				SendClientMessage(giverid, 0x66DD66FF, "გადაეცი %s x%d.", s_InvName[itemId], qty);
			}
			case 2:
			{
				Inv_Say(giverid, -10270721, "გაცვლა ვერ მოხერხდა: მიმღებს ადგილი არ აქვს.");
			}
			case 3:
			{
				Inv_Say(giverid, -10270721, "გაცვლა ვერ მოხერხდა: საკმარისი ნივთი აღარ გაქვს.");
				Inv_Reload(giverid);
			}
			case 4:
			{
				Inv_Say(giverid, -10270721, "ამ ნივთის გადაცემა შეუძლებელია.");
			}
			case -1:
			{
				Inv_Say(giverid, -10270721, "ბაზის შეცდომა გაცვლისას: გადაამოწმე ინვენტარი.");
				Inv_Reload(giverid);
				if (Inv_ValidPlayer(receiverid))
				{
					Inv_Reload(receiverid);
				}
			}
			default:
			{
				Inv_Say(giverid, -10270721, "გაცვლა ვერ მოხერხდა.");
			}
		}
	}
	if (result == 1 && Inv_ValidPlayer(receiverid))
	{
		Inv_Reload(receiverid);
		UI_InvToast(receiverid, itemId, qty);
		SendClientMessage(receiverid, 0x66DD66FF, "მიიღე %s x%d.", s_InvName[itemId], qty);
	}
	return 1;
}

Inv_ShopList(playerid)
{
	SendClientMessage(playerid, -2994689, "--- სუპერმარკეტი: /compra [პროდუქტი] [რაოდენობა] ---");
	new line[256], part[192];
	line[0] = 0;
	for (new i = 1; i <= 11; i++)
	{
		if (s_InvPrice[i] <= 0)
		{
			continue;
		}
		format(part, 192, "%s %s  ", s_InvShopKey[i], Money_Str(s_InvPrice[i]));
		strcat(line, part, 256);
	}
	Util_Send(playerid, 0x99CCFFFF, line);
	Util_Send(playerid, 0x99CCFFFF, "საკვები იყიდება პორციებად (მაქს. 12 თან) და ინახება მაცივარში; კერძი მიირთვი /usa pasto-თი.");
}

Inv_Buy(playerid, itemId, qty)
{
	if (!Inv_Guard(playerid))
	{
		return 0;
	}
	if (itemId == 11)
	{
		Inv_BuyBag(playerid);
		return 0;
	}
	if (itemId < 1 || itemId > 10 || s_InvPrice[itemId] <= 0)
	{
		Inv_Say(playerid, -10270721, "ეს პროდუქტი არ იყიდება.");
		return 0;
	}
	new stack = DB_InvStackMax(itemId);
	if (qty < 1 || qty > stack)
	{
		SendClientMessage(playerid, -10270721, "არასწორი რაოდენობა (1-%d).", stack);
		return 0;
	}
	new maxTotal = 0;
	if (itemId == 1)
	{
		maxTotal = 12;
	}
	else if (itemId == 9)
	{
		maxTotal = 1;
	}
	if (maxTotal > 0 && Inv_Count(playerid, itemId) + qty > maxTotal)
	{
		SendClientMessage(playerid, -10270721, "თან მაქსიმუმ %d შეიძლება გქონდეს.", maxTotal);
		return 0;
	}
	new cost = s_InvPrice[itemId] * qty;
	if (!Money_Can(playerid, cost))
	{
		SendClientMessage(playerid, -10270721, "საჭიროა %s ნაღდი ფული.", Money_Str(cost));
		return 0;
	}
	Inv_BeginOp(playerid, 5, itemId, qty, cost);
	DB_InvBuy(playerid, gPlayer[playerid][pCharId], itemId, qty, gPlayer[playerid][pInvOpId], maxTotal);
	return 0;
}

Inv_BuyBag(playerid)
{
	if (gPlayer[playerid][pInvHasBag] != 0)
	{
		Inv_Say(playerid, -10270721, "ზურგჩანთა უკვე გაქვს.");
		return 0;
	}
	if (!Money_Can(playerid, 350))
	{
		SendClientMessage(playerid, -10270721, "საჭიროა %s ნაღდი ფული.", Money_Str(350));
		return 0;
	}
	Inv_BeginOp(playerid, 6, 11, 1, 350);
	DB_InvSetBag(playerid, gPlayer[playerid][pCharId], gPlayer[playerid][pInvOpId]);
	return 0;
}

Inv_UndoPurchase(playerid)
{
	gPlayer[playerid][pInvUndoOpId][0] = 0;
	strcat(gPlayer[playerid][pInvUndoOpId], gPlayer[playerid][pInvOpId], 33);
	Inv_Say(playerid, -10270721, "ნაღდი ფული არ გყოფნის: ყიდვა გაუქმდა.");
	DB_InvUndo(gPlayer[playerid][pInvUndoOpId]);
}

public Inv_OnDBBought(playerid, itemId, qty, result)
{
	if (!Inv_OpIs(playerid, 5))
	{
		return 1;
	}
	new cost = gPlayer[playerid][pInvOpAux];
	if (result == 1)
	{
		if (!Money_Take(playerid, cost))
		{
			Inv_UndoPurchase(playerid);
			return 1;
		}
		Char_Save(playerid);
		Inv_EndOp(playerid);
		Inv_Reload(playerid);
		UI_InvToast(playerid, itemId, qty);
		SendClientMessage(playerid, 0x66DD66FF, "იყიდე %s x%d %s-ად.", s_InvName[itemId], qty, Money_Str(cost));
		return 1;
	}
	Inv_EndOp(playerid);
	switch (result)
	{
		case 2:
		{
			Inv_Say(playerid, -10270721, "ინვენტარში ადგილი არ გაქვს (ან უკვე მაქსიმუმი გაქვს).");
		}
		case -1:
		{
			Inv_Say(playerid, -10270721, "ბაზის შეცდომა: თანხა არ ჩამოგეჭრა, სცადე ხელახლა.");
		}
		default:
		{
			Inv_Say(playerid, -10270721, "ყიდვა ვერ მოხერხდა.");
		}
	}
	return 1;
}

public Inv_OnDBBag(playerid, result)
{
	if (!Inv_OpIs(playerid, 6))
	{
		return 1;
	}
	if (result == 1)
	{
		if (!Money_Take(playerid, 350))
		{
			Inv_UndoPurchase(playerid);
			return 1;
		}
		Char_Save(playerid);
		Inv_EndOp(playerid);
		Inv_Reload(playerid);
		SendClientMessage(playerid, 0x66DD66FF, "იყიდე ზურგჩანთა %s-ად: ტევადობა %d სლოტი.", Money_Str(350), 20);
		return 1;
	}
	Inv_EndOp(playerid);
	switch (result)
	{
		case 5:
		{
			Inv_Say(playerid, -10270721, "ზურგჩანთა უკვე გაქვს.");
			Inv_Reload(playerid);
		}
		case -1:
		{
			Inv_Say(playerid, -10270721, "ბაზის შეცდომა: თანხა არ ჩამოგეჭრა, სცადე ხელახლა.");
		}
		default:
		{
			Inv_Say(playerid, -10270721, "ყიდვა ვერ მოხერხდა.");
		}
	}
	return 1;
}

public Inv_OnDBUndone(const opId[], result)
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || gPlayer[p][pInvUndoOpId][0] == 0 || strcmp(gPlayer[p][pInvUndoOpId], opId, false, 0x7FFFFFFF))
		{
			continue;
		}
		gPlayer[p][pInvUndoOpId][0] = 0;
		if (gPlayer[p][pInvOpBusy])
		{
			Inv_EndOp(p);
		}
		if (result != 1)
		{
			Inv_Say(p, -10270721, "ყიდვის გაუქმება ვერ მოხერხდა: აცნობე ადმინისტრატორს.");
		}
		Inv_Reload(p);
	}
	return 1;
}

bool:Inv_FridgeMove(playerid, houseId, direction, qty, fridgeCap)
{
	if (!Inv_Guard(playerid))
	{
		return false;
	}
	if (qty < 1 || qty > 40)
	{
		Inv_Say(playerid, -10270721, "არასწორი რაოდენობა.");
		return false;
	}
	if (direction == 1 && Inv_Count(playerid, 1) < qty)
	{
		Inv_Say(playerid, -10270721, "ამდენი პროდუქტი თან არ გაქვს.");
		return false;
	}
	if (direction == 2 && Inv_Count(playerid, 1) + qty > 12)
	{
		SendClientMessage(playerid, -10270721, "მაქსიმუმ %d პორციის ტარება შეგიძლია.", 12);
		return false;
	}
	Inv_BeginOp(playerid, 7, 1, qty, houseId);
	DB_FridgeMove(playerid, gPlayer[playerid][pCharId], houseId, 1, qty, direction, gPlayer[playerid][pInvOpId], fridgeCap, direction == 2 ? 12 : 0);
	return true;
}

public Inv_OnDBFridge(playerid, houseId, direction, qty, result)
{
	if (!Inv_OpIs(playerid, 7))
	{
		return 1;
	}
	Inv_EndOp(playerid);
	if (result == -1)
	{
		gPlayer[playerid][pFridgeHouse] = houseId;
		gPlayer[playerid][pFridgeDir] = direction;
		gPlayer[playerid][pFridgeFoodBefore] = Inv_Count(playerid, 1);
		Inv_Say(playerid, -10270721, "ბაზის შეცდომა: ვამოწმებ მაცივრის მდგომარეობას...");
		Inv_Reload(playerid);
		return 1;
	}
	if (result == 1)
	{
		Inv_Reload(playerid);
		UI_InvToast(playerid, 1, direction == 1 ? -qty : qty);
	}
	House_InvFridgeResult(playerid, houseId, direction, qty, result);
	return 1;
}

public Inv_OnTick()
{
	new now = GetTickCount();
	for (new i = 0; i < 40; i++)
	{
		if (g_InvGround[i][igState] != 2 || g_InvGround[i][igPicker] != 0)
		{
			continue;
		}
		if (now - g_InvGround[i][igExpireTick] >= 0)
		{
			Inv_GroundFree(i);
		}
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || gPlayer[p][pInvGiveTo] == 0 || gPlayer[p][pInvOp] == 2)
		{
			continue;
		}
		if (now - gPlayer[p][pInvGiveStartTick] >= 15000)
		{
			Inv_GiveCancel(p, "გაცვლის დრო ამოიწურა.");
		}
	}
	return 1;
}

Inv_OnInit()
{
	for (new i = 0; i < 40; i++)
	{
		g_InvGround[i][igObject] = 65535;
		g_InvGround[i][igState] = 0;
	}
	g_InvTickTimer = SetTimer("Inv_OnTick", 1000, true);
}

Inv_OnExit()
{
	if (g_InvTickTimer != 0)
	{
		KillTimer(g_InvTickTimer);
	}
	g_InvTickTimer = 0;
	for (new i = 0; i < 40; i++)
	{
		Inv_GroundFree(i);
	}
}

Inv_OnPlayerSpawn(playerid)
{
	if (!Inv_ValidPlayer(playerid) || !gPlayer[playerid][pLoggedIn] || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	if (gPlayer[playerid][pInvState] == 0 || gPlayer[playerid][pInvState] == 3)
	{
		Inv_Load(playerid);
	}
	return 0;
}

Inv_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (gPlayer[playerid][pInvGiveTo] != 0 && gPlayer[playerid][pInvOp] != 2)
	{
		Inv_GiveCancel(playerid, "გაცვლა გაუქმდა: მოთამაშე გავიდა სერვერიდან.");
	}
	else if (gPlayer[playerid][pInvGiveTo] != 0)
	{
		gPlayer[playerid][pInvGiveTo] = 0;
	}
	if (gPlayer[playerid][pInvGiveFrom] != 0)
	{
		new giver = gPlayer[playerid][pInvGiveFrom] - 1;
		gPlayer[playerid][pInvGiveFrom] = 0;
		if (Inv_ValidPlayer(giver) && gPlayer[giver][pInvGiveTo] == playerid + 1 && gPlayer[giver][pInvOp] != 2)
		{
			Inv_GiveCancel(giver, "გაცვლა გაუქმდა: მოთამაშე გავიდა სერვერიდან.");
		}
	}
	if (gPlayer[playerid][pInvOpBusy] && gPlayer[playerid][pInvOp] == 7)
	{
		House_InvFridgeAbort(gPlayer[playerid][pInvOpAux]);
	}
	if (gPlayer[playerid][pFridgeHouse] > 0)
	{
		House_InvFridgeAbort(gPlayer[playerid][pFridgeHouse]);
	}
	Inv_GroundRelease(playerid);
	return 0;
}
