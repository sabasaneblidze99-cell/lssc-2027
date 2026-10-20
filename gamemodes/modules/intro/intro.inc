new g_IntroSeq;
new Float:g_IntroCam[6][12] = {
	{0.0, ...},
	{-300.0, -1400.0, 25.0, -260.0, -1330.0, 22.0, -230.0, -1250.0, 18.0, -230.0, -1250.0, 18.0},
	{1180.0, -780.0, 110.0, 1260.0, -800.0, 108.0, 1500.0, -1100.0, 60.0, 1500.0, -1100.0, 60.0},
	{1380.0, -830.0, 112.0, 1330.0, -880.0, 100.0, 1500.0, -1500.0, 40.0, 1480.0, -1750.0, 40.0},
	{1752.0, -1880.0, 16.0, 1747.0, -1873.0, 15.0, 1758.0, -1888.0, 14.0, 1758.0, -1888.0, 14.0},
	{0.0, ...}
};

bool:Intro_ValidId(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid);
}

public Intro_TestSkip()
{
	return fexist("lsrp_intro_skip.flag") ? 1 : 0;
}

Intro_StopTimer(playerid)
{
	if (gPlayer[playerid][pIntroTimer] != 0)
	{
		KillTimer(gPlayer[playerid][pIntroTimer]);
		gPlayer[playerid][pIntroTimer] = 0;
	}
}

Intro_Schedule(playerid, ev, ms)
{
	Intro_StopTimer(playerid);
	if (ms < 1)
	{
		ms = 1;
	}
	if (gPlayer[playerid][pAuthState] == 14)
	{
		new remaining = 60000 - (GetTickCount() - gPlayer[playerid][pIntroStartTick]);
		if (remaining < 1)
		{
			remaining = 1;
		}
		if (ms > remaining)
		{
			ms = remaining;
		}
	}
	gPlayer[playerid][pIntroTimer] = SetTimerEx("Intro_OnTimer", ms, false, "dddd", playerid, ev, gPlayer[playerid][pIntroScene], gPlayer[playerid][pIntroSeq]);
}

Intro_SceneMs(scene, flags)
{
	new bool:shortV = (flags & 1) != 0;
	switch (scene)
	{
		case 1:
		{
			return 7000;
		}
		case 2:
		{
			return 7000;
		}
		case 3:
		{
			return shortV ? 6000 : 9000;
		}
		case 4:
		{
			return 9000;
		}
	}
	return shortV ? 4000 : 6000;
}

Intro_FirstScene(flags)
{
	return (flags & 1) != 0 ? 3 : 1;
}

Intro_NextScene(scene, flags)
{
	switch (scene)
	{
		case 1:
		{
			return 2;
		}
		case 2:
		{
			return 3;
		}
		case 3:
		{
			return (flags & 3) != 0 ? 5 : 4;
		}
		case 4:
		{
			return 5;
		}
	}
	return 0;
}

Intro_Camera(playerid, scene, flags)
{
	new row = scene, bool:hold = false;
	if (scene == 5)
	{
		row = (flags & 3) != 0 ? 3 : 4;
		hold = true;
	}
	if (row < 1 || row > 4)
	{
		return 0;
	}
	if (hold)
	{
		SetPlayerCameraPos(playerid, g_IntroCam[row][3], g_IntroCam[row][4], g_IntroCam[row][5]);
		SetPlayerCameraLookAt(playerid, g_IntroCam[row][9], g_IntroCam[row][10], g_IntroCam[row][11], CAMERA_CUT);
		return 0;
	}
	new ms = Intro_SceneMs(scene, flags);
	SetPlayerCameraPos(playerid, g_IntroCam[row][0], g_IntroCam[row][1], g_IntroCam[row][2]);
	SetPlayerCameraLookAt(playerid, g_IntroCam[row][6], g_IntroCam[row][7], g_IntroCam[row][8], CAMERA_CUT);
	InterpolateCameraPos(playerid, g_IntroCam[row][0], g_IntroCam[row][1], g_IntroCam[row][2], g_IntroCam[row][3], g_IntroCam[row][4], g_IntroCam[row][5], ms, CAMERA_MOVE);
	InterpolateCameraLookAt(playerid, g_IntroCam[row][6], g_IntroCam[row][7], g_IntroCam[row][8], g_IntroCam[row][9], g_IntroCam[row][10], g_IntroCam[row][11], ms, CAMERA_MOVE);
	return 0;
}

Intro_ObjectCount()
{
	new count = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && gPlayer[i][pIntroVehicle] != 0)
		{
			count++;
		}
	}
	return count;
}

Intro_DestroyObjects(playerid)
{
	if (gPlayer[playerid][pIntroVehicle] != 0)
	{
		DestroyVehicle(gPlayer[playerid][pIntroVehicle]);
		gPlayer[playerid][pIntroVehicle] = 0;
	}
	if (gPlayer[playerid][pIntroActor] != 0)
	{
		DestroyActor(gPlayer[playerid][pIntroActor] - 1);
		gPlayer[playerid][pIntroActor] = 0;
	}
}

bool:Intro_CreateObjects(playerid)
{
	new world = playerid + 20000;
	new veh = CreateVehicle(437, 1758.0, -1888.0, 13.8, 0.0, 1, 1, -1, false);
	if (veh == 65535)
	{
		return false;
	}
	SetVehicleVirtualWorld(veh, world);
	new actor = CreateActor(gPlayer[playerid][pSkin], 1753.5, -1884.5, 13.55, 180.0);
	if (actor == 65535)
	{
		DestroyVehicle(veh);
		return false;
	}
	SetActorVirtualWorld(actor, world);
	SetActorFacingAngle(actor, 180.0);
	ApplyActorAnimation(actor, "PED", "IDLE_stance", 4.1, true, false, false, false, 0);
	gPlayer[playerid][pIntroVehicle] = veh;
	gPlayer[playerid][pIntroActor] = actor + 1;
	return true;
}

Intro_AudioStart(playerid)
{
	new url[1];
	if (url[0] != 0)
	{
		PlayAudioStreamForPlayer(playerid, url, 0.0, 0.0, 0.0, 50.0, false);
	}
}

Intro_AudioStop(playerid)
{
	new url[1];
	if (url[0] != 0)
	{
		StopAudioStreamForPlayer(playerid);
	}
}

Intro_BeginScene(playerid, scene)
{
	new flags = gPlayer[playerid][pIntroFlags];
	gPlayer[playerid][pIntroScene] = scene;
	Intro_Camera(playerid, scene, flags);
	if (scene == 5)
	{
		UI_IntroSubtitle(playerid, "");
		UI_IntroTitle(playerid, true);
	}
	else
	{
		UI_IntroTitle(playerid, false);
		UI_IntroNarrate(playerid, scene);
	}
	UI_IntroFadeTo(playerid, 0, 8, 60);
	Intro_Schedule(playerid, 1, Intro_SceneMs(scene, flags) - 540);
}

public Intro_Begin(playerid)
{
	if (!Intro_ValidId(playerid) || gPlayer[playerid][pAuthState] != 7)
	{
		return 0;
	}
	if (Intro_TestSkip())
	{
		Char_Spawn(playerid);
		return 1;
	}
	new flags = 0;
	if (gPlayer[playerid][pIntroSeen] != 0)
	{
		flags |= 1;
	}
	else
	{
		flags |= 4;
	}
	if (Intro_ObjectCount() >= 8)
	{
		flags |= 2;
	}
	if ((flags & 3) == 0 && !Intro_CreateObjects(playerid))
	{
		flags |= 2;
	}
	g_IntroSeq++;
	if (g_IntroSeq <= 0)
	{
		g_IntroSeq = 1;
	}
	gPlayer[playerid][pIntroSeq] = g_IntroSeq;
	gPlayer[playerid][pIntroFlags] = flags;
	gPlayer[playerid][pIntroStartTick] = GetTickCount();
	gPlayer[playerid][pIntroScene] = 0;
	gPlayer[playerid][pAuthState] = 14;
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, playerid + 20000);
	UI_IntroCreate(playerid);
	UI_IntroFade(playerid, 255);
	UI_IntroSkipHint(playerid, (flags & 4) != 0 ? 8 : 0);
	Intro_AudioStart(playerid);
	Intro_BeginScene(playerid, Intro_FirstScene(flags));
	return 1;
}

public Intro_Finish(playerid, reason)
{
	if (!Intro_ValidId(playerid) || gPlayer[playerid][pAuthState] != 14)
	{
		return 0;
	}
	Intro_StopTimer(playerid);
	Intro_DestroyObjects(playerid);
	Intro_AudioStop(playerid);
	UI_IntroTitle(playerid, false);
	UI_IntroSubtitle(playerid, "");
	UI_IntroSkipHintHide(playerid);
	UI_IntroLetterbox(playerid, false);
	UI_IntroFade(playerid, 255);
	gPlayer[playerid][pIntroScene] = 0;
	gPlayer[playerid][pIntroFlags] |= 8;
	if (reason != 0)
	{
		new name[25], msg[128];
		GetPlayerName(playerid, name, 25);
		format(msg, 128, "intro terminata (%s) per %s", reason == 1 ? "skip" : "watchdog", name);
		Log_Write("INTRO", msg);
	}
	if (gPlayer[playerid][pAccountId] > 0)
	{
		DB_StoryIntroSeen(gPlayer[playerid][pAccountId], playerid);
	}
	new Float:x, Float:y, Float:z, Float:a;
	Char_GetSpawnPoint(0, x, y, z, a);
	gPlayer[playerid][pPosX] = x;
	gPlayer[playerid][pPosY] = y;
	gPlayer[playerid][pPosZ] = z;
	gPlayer[playerid][pAngle] = a;
	gPlayer[playerid][pInterior] = 0;
	gPlayer[playerid][pWorld] = 0;
	Char_Spawn(playerid);
	if (gPlayer[playerid][pIntroTimer] == 0)
	{
		Intro_Schedule(playerid, 3, 4000);
	}
	return 1;
}

public Intro_OnSpawn(playerid)
{
	if (!Intro_ValidId(playerid) || (gPlayer[playerid][pIntroFlags] & 8) == 0)
	{
		return 0;
	}
	UI_IntroFadeTo(playerid, 0, 8, 60);
	Intro_Schedule(playerid, 3, 540);
	return 1;
}

Intro_EndOutro(playerid)
{
	UI_IntroUiDestroy(playerid);
	gPlayer[playerid][pIntroFlags] = 0;
}

public Intro_OnTimer(playerid, ev, scene, seq)
{
	if (!Intro_ValidId(playerid) || seq == 0 || gPlayer[playerid][pIntroSeq] != seq)
	{
		return 1;
	}
	gPlayer[playerid][pIntroTimer] = 0;
	if (ev == 3)
	{
		Intro_EndOutro(playerid);
		return 1;
	}
	if (gPlayer[playerid][pAuthState] != 14 || gPlayer[playerid][pIntroScene] != scene)
	{
		return 1;
	}
	if (GetTickCount() - gPlayer[playerid][pIntroStartTick] >= 59950)
	{
		Intro_Finish(playerid, 2);
		return 1;
	}
	switch (ev)
	{
		case 1:
		{
			UI_IntroFadeTo(playerid, 255, 8, 60);
			Intro_Schedule(playerid, 2, 540);
		}
		case 2:
		{
			new next = Intro_NextScene(scene, gPlayer[playerid][pIntroFlags]);
			if (next == 0)
			{
				Intro_Finish(playerid, 0);
			}
			else
			{
				Intro_BeginScene(playerid, next);
			}
		}
	}
	return 1;
}

public Intro_Skip(playerid)
{
	if (!Intro_ValidId(playerid) || gPlayer[playerid][pAuthState] != 14)
	{
		return -1;
	}
	if ((gPlayer[playerid][pIntroFlags] & 4) != 0)
	{
		new elapsed = GetTickCount() - gPlayer[playerid][pIntroStartTick];
		if (elapsed < 8000)
		{
			return (8000 - elapsed + 1000 - 1) / 1000;
		}
	}
	Intro_Finish(playerid, 1);
	return 0;
}

public UI_OnIntroFadeDone(playerid, alpha)
{
	return 1;
}

Intro_OnKeyStateChange(playerid, t_KEY:newkeys)
{
	if (!Intro_ValidId(playerid) || gPlayer[playerid][pAuthState] != 14)
	{
		return 0;
	}
	if ((newkeys & KEY_SECONDARY_ATTACK) != KEY:0)
	{
		Intro_Skip(playerid);
	}
	return 0;
}

public Intro_OnPlayerConnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	UI_IntroUiDestroy(playerid);
	return 1;
}

public Intro_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	Intro_StopTimer(playerid);
	Intro_DestroyObjects(playerid);
	if (gPlayer[playerid][pAuthState] == 14)
	{
		Intro_AudioStop(playerid);
	}
	UI_IntroUiDestroy(playerid);
	return 1;
}

Intro_OnExit()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Intro_DestroyObjects(i);
	}
}

public Intro_OnNoCharacter(playerid)
{
	if (!Intro_ValidId(playerid))
	{
		return 0;
	}
	if (Intro_TestSkip())
	{
		return 0;
	}
	DB_StoryLoad(playerid, gPlayer[playerid][pAccountId], 0);
	return 1;
}

public Intro_OnCharLoaded(playerid)
{
	if (!Intro_ValidId(playerid))
	{
		return 0;
	}
	DB_StoryLoad(playerid, gPlayer[playerid][pAccountId], gPlayer[playerid][pCharId]);
	return 1;
}

public Intro_StoryBegin(playerid)
{
	if (!Intro_ValidId(playerid))
	{
		return 0;
	}
	UI_StoryBegin(playerid);
	return 1;
}

Intro_ApplyStartMoney(playerid, motive)
{
	if (motive >= 1 && motive <= 4)
	{
		gPlayer[playerid][pMoney] = DB_StoryCash(motive);
		gPlayer[playerid][pBank] = DB_StoryBankStart(motive);
	}
	else
	{
		gPlayer[playerid][pMoney] = 2500;
		gPlayer[playerid][pBank] = 5000;
	}
}

Intro_SaveStory(playerid, bool:creation)
{
	new opId[37];
	DB_StoryMakeOpId(opId, 37, gPlayer[playerid][pCharId]);
	gPlayer[playerid][pStorySaveRetries] = 0;
	DB_StorySave(playerid, gPlayer[playerid][pCharId], gPlayer[playerid][pStoryDraftOrigin], gPlayer[playerid][pStoryDraftMotive], opId, creation);
}

public UI_OnStoryChosen(playerid, origin, reason)
{
	if (!Intro_ValidId(playerid))
	{
		return 1;
	}
	origin = clamp(origin, 0, 5);
	reason = clamp(reason, 0, 4);
	if (origin == 0)
	{
		reason = 0;
	}
	if (gPlayer[playerid][pAuthState] == 15)
	{
		gPlayer[playerid][pStoryDraftOrigin] = origin;
		gPlayer[playerid][pStoryDraftMotive] = reason;
		Intro_ApplyStartMoney(playerid, reason);
		Char_BeginCreate(playerid);
		return 1;
	}
	if (gPlayer[playerid][pStorySetMode] != 0 && Util_IsPlaying(playerid))
	{
		if (gPlayer[playerid][pStoryOrigin] != 0 || gPlayer[playerid][pStoryMotive] != 0)
		{
			gPlayer[playerid][pStorySetMode] = 0;
			Util_Send(playerid, -10270721, "უკვე გაქვს ისტორია: მისი შეცვლა შეუძლებელია.");
			return 1;
		}
		if (origin == 0)
		{
			gPlayer[playerid][pStorySetMode] = 0;
			Util_Send(playerid, -1, "ისტორია არ არის არჩეული: შეგიძლია მოგვიანებით აირჩიო - /storia imposta.");
			return 1;
		}
		gPlayer[playerid][pStoryDraftOrigin] = origin;
		gPlayer[playerid][pStoryDraftMotive] = reason;
		Intro_SaveStory(playerid, false);
	}
	return 1;
}

public Intro_OnCharCreated(playerid, charId)
{
	if (!Intro_ValidId(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pStoryDraftOrigin] != 0 || gPlayer[playerid][pStoryDraftMotive] != 0)
	{
		gPlayer[playerid][pStoryOrigin] = gPlayer[playerid][pStoryDraftOrigin];
		gPlayer[playerid][pStoryMotive] = gPlayer[playerid][pStoryDraftMotive];
		if (charId > 0)
		{
			Intro_SaveStory(playerid, true);
		}
	}
	return Intro_Begin(playerid);
}

public Story_OnDBLoaded(playerid, result, introSeen, origin, motive, bool:hasStory)
{
	if (result == -1 && Intro_ValidId(playerid))
	{
		Log_Write("STORY", "lettura storia/intro_seen fallita: si procede con i valori di base");
	}
	return 1;
}

public Story_OnDBSaved(playerid, result, origin, motive, moneyDelta)
{
	if (!Intro_ValidId(playerid))
	{
		return 1;
	}
	new bool:setMode = gPlayer[playerid][pStorySetMode] != 0;
	if (result == -1 && gPlayer[playerid][pStorySaveRetries] < 1)
	{
		new opId[37];
		gPlayer[playerid][pStorySaveRetries]++;
		DB_StoryLastOpId(playerid, opId, 37);
		DB_StorySave(playerid, gPlayer[playerid][pCharId], gPlayer[playerid][pStoryDraftOrigin], gPlayer[playerid][pStoryDraftMotive], opId, !setMode);
		return 1;
	}
	if (moneyDelta != 0 && gPlayer[playerid][pSpawned])
	{
		Char_SyncMoney(playerid);
		Char_RefreshHud(playerid);
	}
	if (result != 1 && result != 3 && result != 4)
	{
		new msg[128];
		format(msg, 128, "salvataggio storia non riuscito (esito %d, set %d)", result, setMode);
		Log_Write("STORY", msg);
	}
	if (setMode)
	{
		gPlayer[playerid][pStorySetMode] = 0;
		if (result == 1 || result == 4 || result == 3)
		{
			Util_Send(playerid, 0x66DD66FF, "შენი ისტორია შენახულია (/storia - ხელახლა წასაკითხად).");
		}
		else if (result == 2)
		{
			Util_Send(playerid, -10270721, "უკვე გაქვს ისტორია: მისი შეცვლა შეუძლებელია.");
		}
		else
		{
			Util_Send(playerid, -10270721, "ისტორიის შენახვა ახლა ვერ ხერხდება. სცადე მოგვიანებით - /storia imposta.");
		}
	}
	return 1;
}

Intro_CmdSalta(playerid)
{
	new left = Intro_Skip(playerid);
	if (left < 0)
	{
		Util_Send(playerid, -10270721, "ამჟამად არცერთ ინტროს არ უყურებ.");
	}
	else if (left > 0)
	{
		new msg[512];
		format(msg, 512, "ინტროს გამოტოვება შეგეძლება %d წამში.", left);
		Util_Send(playerid, -10270721, msg);
	}
}

Intro_CmdStoria(playerid, const params[])
{
	new pos = 0, tok[16], name[25], target = INVALID_PLAYER_ID;
	Util_NextToken(params, pos, tok, 16);
	if (tok[0] == 0)
	{
		GetPlayerName(playerid, name, 25);
		UI_StoryShowOwn(playerid, name, gPlayer[playerid][pStoryOrigin], gPlayer[playerid][pStoryMotive]);
		return 0;
	}
	if (!strcmp(tok, "imposta", true, 0x7FFFFFFF))
	{
		if (gPlayer[playerid][pStoryOrigin] != 0 || gPlayer[playerid][pStoryMotive] != 0)
		{
			Util_Send(playerid, -10270721, "უკვე გაქვს ისტორია: მისი შეცვლა შეუძლებელია.");
			return 0;
		}
		gPlayer[playerid][pStorySetMode] = 1;
		UI_StoryBegin(playerid);
		return 0;
	}
	if (Util_GetAdmin(playerid) >= 2 && Util_ParsePlayerId(tok, target) && Util_IsPlaying(target))
	{
		DB_StoryRead(playerid, target, 2);
		return 0;
	}
	if (Util_GetAdmin(playerid) >= 2)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /storia · /storia imposta · /storia [id giocatore in gioco] (staff)");
	}
	else
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /storia · /storia imposta (una sola volta, solo se non hai una storia)");
	}
	return 0;
}

Intro_CmdMostra(playerid, const params[])
{
	new pos = 0, tok[16], target = INVALID_PLAYER_ID;
	Util_NextToken(params, pos, tok, 16);
	if (!Util_ParsePlayerId(tok, target) || target == playerid || !Util_IsPlaying(target))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /mostrastoria [id] (a un giocatore vicino a te)");
		return 0;
	}
	if (!Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "Il giocatore e' troppo lontano.");
		return 0;
	}
	new now = GetTickCount(), last = gPlayer[playerid][pStoryLastShowTick];
	if (gPlayer[playerid][pStoryShowTarget] != 0 || (last != 0 && now - last < 10000))
	{
		Util_Send(playerid, -10270721, "Attendi qualche secondo prima di mostrare di nuovo la tua storia.");
		return 0;
	}
	gPlayer[playerid][pStoryLastShowTick] = now != 0 ? now : 1;
	gPlayer[playerid][pStoryShowTarget] = target + 1;
	DB_StoryRead(playerid, playerid, 1);
	return 0;
}

public Story_OnDBRead(playerid, targetid, purpose, result, origin, motive, bool:hasStory)
{
	if (!Intro_ValidId(playerid))
	{
		return 1;
	}
	if (purpose == 1)
	{
		new to = gPlayer[playerid][pStoryShowTarget] - 1;
		gPlayer[playerid][pStoryShowTarget] = 0;
		if (!Util_IsPlaying(playerid))
		{
			return 1;
		}
		if (result != 1)
		{
			Util_Send(playerid, -10270721, "Impossibile leggere la tua storia ora. Riprova tra poco.");
			return 1;
		}
		if (!hasStory || (origin == 0 && motive == 0))
		{
			Util_Send(playerid, -10270721, "Non hai una storia da mostrare (/storia imposta).");
			return 1;
		}
		if (to < 0 || to >= 1000 || !Util_IsPlaying(to) || !Util_InRange(playerid, to, 3.0))
		{
			Util_Send(playerid, -10270721, "Il giocatore non e' piu' abbastanza vicino.");
			return 1;
		}
		new name[25], tname[25];
		GetPlayerName(playerid, name, 25);
		GetPlayerName(to, tname, 25);
		UI_StoryShowOther(to, name, origin, motive);
		SendClientMessage(to, -1, "%s ti mostra la propria storia.", name);
		SendClientMessage(playerid, 0x66DD66FF, "Hai mostrato la tua storia a %s.", tname);
		return 1;
	}
	if (purpose == 2)
	{
		if (Util_GetAdmin(playerid) < 2)
		{
			return 1;
		}
		if (result != 1)
		{
			Util_Send(playerid, -10270721, "Impossibile leggere la storia del giocatore.");
			return 1;
		}
		if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
		{
			return 1;
		}
		new tname[25], reason[64];
		GetPlayerName(targetid, tname, 25);
		format(reason, 64, "letta la storia di %s (pg %d)", tname, gPlayer[targetid][pCharId]);
		Admin_Log(playerid, gPlayer[targetid][pAccountId], "storia", reason);
		UI_StoryShowOther(playerid, tname, origin, motive);
	}
	return 1;
}
