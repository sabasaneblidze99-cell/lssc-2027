new g_CharAutosaveTimer;

bool:Char_IsSkinAllowed(gender, skin)
{
	return Valid_SkinAllowed(gender, skin);
}

Char_SyncMoney(playerid)
{
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, gPlayer[playerid][pMoney]);
}

Char_RefreshHud(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayer[playerid][pHudCreated])
	{
		return 0;
	}
	UI_HudUpdate(playerid, gPlayer[playerid][pMoney], gPlayer[playerid][pBank], gPlayer[playerid][pLevel], gPlayer[playerid][pExp]);
	return 0;
}

Char_RefreshState(playerid)
{
	if (!gPlayer[playerid][pSpawned] || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new Float:x, Float:y, Float:z, Float:a, Float:hp, Float:ar;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	GetPlayerHealth(playerid, hp);
	GetPlayerArmour(playerid, ar);
	gPlayer[playerid][pPosX] = x;
	gPlayer[playerid][pPosY] = y;
	gPlayer[playerid][pPosZ] = z;
	gPlayer[playerid][pAngle] = a;
	gPlayer[playerid][pHealth] = hp;
	gPlayer[playerid][pArmor] = ar;
	gPlayer[playerid][pInterior] = GetPlayerInterior(playerid);
	gPlayer[playerid][pWorld] = GetPlayerVirtualWorld(playerid);
	gPlayer[playerid][pSkin] = GetPlayerSkin(playerid);
	return 0;
}

Char_Save(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!gPlayer[playerid][pLoggedIn] || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	Char_RefreshState(playerid);
	DB_SaveCharacter(playerid);
	return 0;
}

Char_SaveAll()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i))
		{
			Char_Save(i);
		}
	}
}

public Char_OnAutosave()
{
	Char_SaveAll();
	return 1;
}

Char_SanitizeLoaded(playerid)
{
	gPlayer[playerid][pMoney] = clamp(gPlayer[playerid][pMoney], 0, 0x3B9ACA00);
	gPlayer[playerid][pBank] = clamp(gPlayer[playerid][pBank], 0, 0x3B9ACA00);
	gPlayer[playerid][pLevel] = clamp(gPlayer[playerid][pLevel], 1, 50);
	if (gPlayer[playerid][pExp] < 0)
	{
		gPlayer[playerid][pExp] = 0;
	}
	gPlayer[playerid][pLevel] = Level_FromExp(gPlayer[playerid][pExp]);
	if (gPlayer[playerid][pSkin] < 0 || gPlayer[playerid][pSkin] > 311)
	{
		gPlayer[playerid][pSkin] = 7;
	}
	gPlayer[playerid][pCivilSkin] = gPlayer[playerid][pSkin];
	gPlayer[playerid][pJob] = clamp(gPlayer[playerid][pJob], 0, 3);
	gPlayer[playerid][pFaction] = clamp(gPlayer[playerid][pFaction], 0, 2);
	gPlayer[playerid][pFactionRank] = clamp(gPlayer[playerid][pFactionRank], 0, 6);
	if (gPlayer[playerid][pFaction] == 0)
	{
		gPlayer[playerid][pFactionRank] = 0;
	}
	else if (gPlayer[playerid][pFactionRank] < 1)
	{
		gPlayer[playerid][pFactionRank] = 1;
	}
	gPlayer[playerid][pOnDuty] = 0;
	gPlayer[playerid][pCuffed] = 0;
	gPlayer[playerid][pWounded] = 0;
	if (gPlayer[playerid][pInterior] < 0)
	{
		gPlayer[playerid][pInterior] = 0;
	}
	if (gPlayer[playerid][pWorld] < 0)
	{
		gPlayer[playerid][pWorld] = 0;
	}
	if ((0.0 == gPlayer[playerid][pPosX]) && (0.0 == gPlayer[playerid][pPosY]) && (0.0 == gPlayer[playerid][pPosZ]))
	{
		Char_PickSpawn(playerid);
	}
}

public Char_OnDBError(playerid)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 4)
	{
		return 1;
	}
	Account_Kick(playerid, "სერვისი დროებით მიუწვდომელია. სცადე მოგვიანებით.");
	return 1;
}

public Char_OnDBLoaded(playerid, bool:found)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 4)
	{
		return 1;
	}
	if (found)
	{
		Char_SanitizeLoaded(playerid);
		if (!CharSel_ApplyLoadedName(playerid))
		{
			Account_Kick(playerid, "პერსონაჟის სახელი ახლა დაკავებულია (სხვა მოთამაშე სერვერზეა). სცადე მოგვიანებით.");
			return 1;
		}
		Intro_OnCharLoaded(playerid);
		SpawnSel_Begin(playerid);      // scelta dello spawn (character/spawn_select.inc)
		return 1;
	}
	if (!DB_IsReady())
	{
		Account_Kick(playerid, "სერვისი დროებით მიუწვდომელია. სცადე მოგვიანებით.");
		return 1;
	}
	Intro_OnNoCharacter(playerid);
	gPlayer[playerid][pAuthState] = 5;
	Account_ShowPrompt(playerid);
	return 1;
}

public UI_OnGenderChosen(playerid, gender)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 5)
	{
		return 1;
	}
	if (gender != 0 && gender != 1)
	{
		UI_ShowGenderSelect(playerid);
		return 1;
	}
	gPlayer[playerid][pGender] = gender;
	gPlayer[playerid][pAuthState] = 6;
	UI_ShowSkinSelect(playerid, gender);
	return 1;
}

public UI_OnSkinChosen(playerid, skin)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 6)
	{
		return 1;
	}
	if (!Char_IsSkinAllowed(gPlayer[playerid][pGender], skin))
	{
		skin = gPlayer[playerid][pGender] == 1 ? 12 : 7;
	}
	gPlayer[playerid][pMoney] = 2500;
	gPlayer[playerid][pBank] = 5000;
	gPlayer[playerid][pLevel] = 1;
	gPlayer[playerid][pExp] = 0;
	gPlayer[playerid][pHealth] = 100.0;
	gPlayer[playerid][pArmor] = 0.0;
	gPlayer[playerid][pSkin] = skin;
	Char_PickSpawn(playerid);
	gPlayer[playerid][pStoryDraftOrigin] = 0;
	gPlayer[playerid][pStoryDraftMotive] = 0;
	if (Intro_TestSkip())
	{
		Char_BeginCreate(playerid);
		return 1;
	}
	gPlayer[playerid][pAuthState] = 15;
	Intro_StoryBegin(playerid);
	return 1;
}

Char_BeginCreate(playerid)
{
	gPlayer[playerid][pAuthState] = 7;
	DB_CreateCharacter(playerid, gPlayer[playerid][pAccountId]);
}

public Char_OnDBCreated(playerid, charId)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 7)
	{
		return 1;
	}
	if (charId <= 0)
	{
		Log_Write("CHAR", "creazione personaggio fallita");
		Account_Kick(playerid, "პერსონაჟის შექმნა ვერ მოხერხდა. სცადე მოგვიანებით.");
		return 1;
	}
	new name[75], msg[384];
	GetPlayerName(playerid, name, 75);
	format(msg, 384, "nuovo personaggio %d (%s)", charId, name);
	Log_Write("CHAR", msg);
	Inv_IssueStartDoc(playerid);
	Intro_OnCharCreated(playerid, charId);
	return 1;
}

Char_OnInit()
{
	AddPlayerClass(7, 1481.0, -1749.5, 15.45, 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
	g_CharAutosaveTimer = SetTimer("Char_OnAutosave", 300000, true);
}

Char_OnExit()
{
	if (g_CharAutosaveTimer != 0)
	{
		KillTimer(g_CharAutosaveTimer);
	}
	g_CharAutosaveTimer = 0;
	Char_SaveAll();
}

Char_OnRequestClass(playerid)
{
	gPlayer[playerid][pClassRequested] = 1;
	if (SpawnSel_Active(playerid) || CharSel_Active(playerid))
	{
		Char_SetLoginCamera(playerid);
		return 1;
	}
	if (gPlayer[playerid][pAuthState] == 14)
	{
		return 1;
	}
	if (gPlayer[playerid][pAuthState] == 15)
	{
		Char_SetLoginCamera(playerid);
		Intro_StoryBegin(playerid);
		return 1;
	}
	if (gPlayer[playerid][pAuthState] == 9)
	{
		Char_Spawn(playerid);
		return 1;
	}
	Char_SetLoginCamera(playerid);
	Account_ShowPrompt(playerid);
	return 1;
}

Char_OnRequestSpawn(playerid)
{
	#pragma unused playerid
	return 0;
}

Char_OnSpawn(playerid)
{
	new authState = gPlayer[playerid][pAuthState];
	if (!gPlayer[playerid][pLoggedIn] || (authState != 8 && authState != 9))
	{
		return 1;
	}
	gPlayer[playerid][pAuthState] = 9;
	gPlayer[playerid][pSpawned] = 1;
	gPlayer[playerid][pLastActiveTick] = GetTickCount();
	gPlayer[playerid][pSkin] = gPlayer[playerid][pCivilSkin];
	SetPlayerSkin(playerid, gPlayer[playerid][pSkin]);
	SetPlayerInterior(playerid, gPlayer[playerid][pInterior]);
	SetPlayerVirtualWorld(playerid, gPlayer[playerid][pWorld]);
	SetPlayerHealth(playerid, 100.0);
	SetPlayerArmour(playerid, 0.0);
	SetCameraBehindPlayer(playerid);
	UI_CloseDialogs(playerid);
	Char_SyncMoney(playerid);
	if (gPlayer[playerid][pFrozen])
	{
		TogglePlayerControllable(playerid, false);
	}
	if (!gPlayer[playerid][pHudCreated])
	{
		UI_HudCreate(playerid);
		gPlayer[playerid][pHudCreated] = 1;
	}
	Char_RefreshHud(playerid);
	Intro_OnSpawn(playerid);
	if (gPlayer[playerid][pSpawnCount] == 0)
	{
		new name[75];
		GetPlayerName(playerid, name, 75);
		new msg[1024];
		format(msg, 1024, "კეთილი იყოს შენი მობრძანება ლოს-სანტოსში, %s. ბრძანებების სია: /help", name);
		Util_Send(playerid, -1, msg);
	}
	gPlayer[playerid][pSpawnCount]++;
	return 1;
}

Char_OnDeath(playerid)
{
	if (!gPlayer[playerid][pSpawned])
	{
		return 0;
	}
	Char_RefreshState(playerid);
	gPlayer[playerid][pSpawned] = 0;
	gPlayer[playerid][pHealth] = 100.0;
	gPlayer[playerid][pArmor] = 0.0;
	Char_ApplySpawnInfo(playerid);
	return 0;
}

Char_OnDisconnect(playerid)
{
	Char_Save(playerid);
	if (gPlayer[playerid][pHudCreated])
	{
		UI_HudDestroy(playerid);
	}
	gPlayer[playerid][pHudCreated] = 0;
}

Char_OnUpdate(playerid)
{
	gPlayer[playerid][pLastActiveTick] = GetTickCount();
	return 1;
}

Char_OnText(playerid)
{
	return gPlayer[playerid][pLoggedIn] && gPlayer[playerid][pSpawned] ? 1 : 0;
}
