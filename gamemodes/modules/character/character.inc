new g_CharAutosaveTimer;

bool:Char_IsSkinAllowed(gender, skin)
{
	return Valid_SkinAllowed(gender, skin);
}

Char_SyncMoney(playerid)
{
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, gPlayer[playerid][PLAYER_DATA_88]);
}

Char_RefreshHud(playerid)
{
	if (playerid < 0 || playerid >= 1000 || !gPlayer[playerid][PLAYER_DATA_108])
	{
		return 0;
	}
	UI_HudUpdate(playerid, gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89], gPlayer[playerid][PLAYER_DATA_90], gPlayer[playerid][PLAYER_DATA_91]);
}

Char_RefreshState(playerid)
{
	if (!gPlayer[playerid][PLAYER_DATA_1] || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new Float:x, Float:y, Float:z, Float:a, Float:hp, Float:ar;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	GetPlayerHealth(playerid, hp);
	GetPlayerArmour(playerid, ar);
	gPlayer[playerid][PLAYER_DATA_94] = x;
	gPlayer[playerid][PLAYER_DATA_95] = y;
	gPlayer[playerid][PLAYER_DATA_96] = z;
	gPlayer[playerid][PLAYER_DATA_97] = a;
	gPlayer[playerid][PLAYER_DATA_92] = hp;
	gPlayer[playerid][PLAYER_DATA_93] = ar;
	gPlayer[playerid][PLAYER_DATA_98] = GetPlayerInterior(playerid);
	gPlayer[playerid][PLAYER_DATA_99] = GetPlayerVirtualWorld(playerid);
	gPlayer[playerid][PLAYER_DATA_100] = GetPlayerSkin(playerid);
}

Char_Save(playerid)
{
	if (playerid < 0 || playerid >= 1000)
	{
		return 0;
	}
	if (!gPlayer[playerid][PLAYER_DATA_0] || gPlayer[playerid][PLAYER_DATA_3] <= 0)
	{
		return 0;
	}
	Char_RefreshState(playerid);
	DB_SaveCharacter(playerid);
}

Char_SaveAll()
{
	for (new i = 0; i < 1000; i++)
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
	gPlayer[playerid][PLAYER_DATA_88] = clamp(gPlayer[playerid][PLAYER_DATA_88], 0, 0x3B9ACA00);
	gPlayer[playerid][PLAYER_DATA_89] = clamp(gPlayer[playerid][PLAYER_DATA_89], 0, 0x3B9ACA00);
	gPlayer[playerid][PLAYER_DATA_90] = clamp(gPlayer[playerid][PLAYER_DATA_90], 1, 50);
	if (gPlayer[playerid][PLAYER_DATA_91] < 0)
	{
		gPlayer[playerid][PLAYER_DATA_91] = 0;
	}
	gPlayer[playerid][PLAYER_DATA_90] = Level_FromExp(gPlayer[playerid][PLAYER_DATA_91]);
	if (gPlayer[playerid][PLAYER_DATA_100] < 0 || gPlayer[playerid][PLAYER_DATA_100] > 311)
	{
		gPlayer[playerid][PLAYER_DATA_100] = 7;
	}
	gPlayer[playerid][PLAYER_DATA_104] = gPlayer[playerid][PLAYER_DATA_100];
	gPlayer[playerid][PLAYER_DATA_101] = clamp(gPlayer[playerid][PLAYER_DATA_101], 0, 3);
	gPlayer[playerid][PLAYER_DATA_102] = clamp(gPlayer[playerid][PLAYER_DATA_102], 0, 2);
	gPlayer[playerid][PLAYER_DATA_103] = clamp(gPlayer[playerid][PLAYER_DATA_103], 0, 6);
	if (gPlayer[playerid][PLAYER_DATA_102] == 0)
	{
		gPlayer[playerid][PLAYER_DATA_103] = 0;
	}
	else
	{
		if (gPlayer[playerid][PLAYER_DATA_103] < 1)
		{
			gPlayer[playerid][PLAYER_DATA_103] = 1;
		}
	}
	gPlayer[playerid][PLAYER_DATA_121] = 0;
	gPlayer[playerid][PLAYER_DATA_122] = 0;
	gPlayer[playerid][PLAYER_DATA_123] = 0;
	if (gPlayer[playerid][PLAYER_DATA_98] < 0)
	{
		gPlayer[playerid][PLAYER_DATA_98] = 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_99] < 0)
	{
		gPlayer[playerid][PLAYER_DATA_99] = 0;
	}
	if ((Float:(0) == Float:(gPlayer[playerid][PLAYER_DATA_94])) && (Float:(0) == Float:(gPlayer[playerid][PLAYER_DATA_95])) && (Float:(0) == Float:(gPlayer[playerid][PLAYER_DATA_96])))
	{
		Char_PickSpawn(playerid);
	}
}

public Char_OnDBError(playerid)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_106] != 4)
	{
		return 1;
	}
	Account_Kick(playerid, "სერვისი დროებით მიუწვდომელია. სცადე მოგვიანებით.");
	return 1;
}

public Char_OnDBLoaded(playerid, bool:found)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_106] != 4)
	{
		return 1;
	}
	if (found)
	{
		Char_SanitizeLoaded(playerid);
		Intro_OnCharLoaded(playerid);
		Char_Spawn(playerid);
		return 1;
	}
	if (!DB_IsReady())
	{
		Account_Kick(playerid, "სერვისი დროებით მიუწვდომელია. სცადე მოგვიანებით.");
		return 1;
	}
	Intro_OnNoCharacter(playerid);
	gPlayer[playerid][PLAYER_DATA_106] = 5;
	Account_ShowPrompt(playerid);
	return 1;
}

public UI_OnGenderChosen(playerid, gender)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_106] != 5)
	{
		return 1;
	}
	if (gender != 0 && gender != 1)
	{
		UI_ShowGenderSelect(playerid);
		return 1;
	}
	gPlayer[playerid][PLAYER_DATA_105] = gender;
	gPlayer[playerid][PLAYER_DATA_106] = 6;
	UI_ShowSkinSelect(playerid, gender);
	return 1;
}

public UI_OnSkinChosen(playerid, skin)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_106] != 6)
	{
		return 1;
	}
	if (!Char_IsSkinAllowed(gPlayer[playerid][PLAYER_DATA_105], skin))
	{
		skin = gPlayer[playerid][PLAYER_DATA_105] == 1 ? 12 : 7;
	}
	gPlayer[playerid][PLAYER_DATA_88] = 2500;
	gPlayer[playerid][PLAYER_DATA_89] = 5000;
	gPlayer[playerid][PLAYER_DATA_90] = 1;
	gPlayer[playerid][PLAYER_DATA_91] = 0;
	gPlayer[playerid][PLAYER_DATA_92] = 0x42C80000;
	gPlayer[playerid][PLAYER_DATA_93] = 0;
	gPlayer[playerid][PLAYER_DATA_100] = skin;
	Char_PickSpawn(playerid);
	gPlayer[playerid][PLAYER_DATA_332] = 0;
	gPlayer[playerid][PLAYER_DATA_333] = 0;
	if (Intro_TestSkip())
	{
		Char_BeginCreate(playerid);
		return 1;
	}
	gPlayer[playerid][PLAYER_DATA_106] = 15;
	Intro_StoryBegin(playerid);
	return 1;
}

Char_BeginCreate(playerid)
{
	gPlayer[playerid][PLAYER_DATA_106] = 7;
	DB_CreateCharacter(playerid, gPlayer[playerid][PLAYER_DATA_2]);
}

public Char_OnDBCreated(playerid, charId)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_106] != 7)
	{
		return 1;
	}
	if (charId <= 0)
	{
		Log_Write("CHAR", "creazione personaggio fallita");
		Account_Kick(playerid, "პერსონაჟის შექმნა ვერ მოხერხდა. სცადე მოგვიანებით.");
		return 1;
	}
	new name[25], msg[128];
	GetPlayerName(playerid, name, 25);
	format(msg, 128, "nuovo personaggio %d (%s)", charId, name);
	Log_Write("CHAR", msg);
	Inv_IssueStartDoc(playerid);
	Intro_OnCharCreated(playerid, charId);
	return 1;
}

Char_OnInit()
{
	AddPlayerClass(7, 1481.0, -1749.5, 15.45, 0.0, 0, 0, 0, 0, 0, 0);
	g_CharAutosaveTimer = SetTimer("Char_OnAutosave", 300000, 1);
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
	gPlayer[playerid][PLAYER_DATA_107] = 1;
	if (gPlayer[playerid][PLAYER_DATA_106] == 14)
	{
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_106] == 15)
	{
		Char_SetLoginCamera(playerid);
		Intro_StoryBegin(playerid);
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_106] == 9)
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
	return 0;
}

Char_OnSpawn(playerid)
{
	new authState = gPlayer[playerid][PLAYER_DATA_106];
	if (!gPlayer[playerid][PLAYER_DATA_0] || (authState != 8 && authState != 9))
	{
		return 1;
	}
	gPlayer[playerid][PLAYER_DATA_106] = 9;
	gPlayer[playerid][PLAYER_DATA_1] = 1;
	gPlayer[playerid][PLAYER_DATA_114] = GetTickCount();
	gPlayer[playerid][PLAYER_DATA_100] = gPlayer[playerid][PLAYER_DATA_104];
	SetPlayerSkin(playerid, gPlayer[playerid][PLAYER_DATA_100]);
	SetPlayerInterior(playerid, gPlayer[playerid][PLAYER_DATA_98]);
	SetPlayerVirtualWorld(playerid, gPlayer[playerid][PLAYER_DATA_99]);
	SetPlayerHealth(playerid, Float:0x42C80000);
	SetPlayerArmour(playerid, 0.0);
	SetCameraBehindPlayer(playerid);
	UI_CloseDialogs(playerid);
	Char_SyncMoney(playerid);
	if (gPlayer[playerid][PLAYER_DATA_119])
	{
		TogglePlayerControllable(playerid, 0);
	}
	if (!gPlayer[playerid][PLAYER_DATA_108])
	{
		UI_HudCreate(playerid);
		gPlayer[playerid][PLAYER_DATA_108] = 1;
	}
	Char_RefreshHud(playerid);
	Intro_OnSpawn(playerid);
	if (gPlayer[playerid][PLAYER_DATA_109] == 0)
	{
		new name[25];
		GetPlayerName(playerid, name, 25);
		new msg[512];
		format(msg, 512, "კეთილი იყოს შენი მობრძანება ლოს-სანტოსში, %s. ბრძანებების სია: /help", name);
		Util_Send(playerid, -1, msg);
	}
	gPlayer[playerid][PLAYER_DATA_109]++;
	return 1;
}

Char_OnDeath(playerid)
{
	if (!gPlayer[playerid][PLAYER_DATA_1])
	{
		return 0;
	}
	Char_RefreshState(playerid);
	gPlayer[playerid][PLAYER_DATA_1] = 0;
	gPlayer[playerid][PLAYER_DATA_92] = 0x42C80000;
	gPlayer[playerid][PLAYER_DATA_93] = 0;
	Char_ApplySpawnInfo(playerid);
}

Char_OnDisconnect(playerid)
{
	Char_Save(playerid);
	if (gPlayer[playerid][PLAYER_DATA_108])
	{
		UI_HudDestroy(playerid);
	}
	gPlayer[playerid][PLAYER_DATA_108] = 0;
}

Char_OnUpdate(playerid)
{
	gPlayer[playerid][PLAYER_DATA_114] = GetTickCount();
	return 1;
}

Char_OnText(playerid)
{
	return gPlayer[playerid][PLAYER_DATA_0] && gPlayer[playerid][PLAYER_DATA_1] ? 1 : 0;
}
