// LSSC test harness (filterscript). Drives an open.mp server-side NPC "Test_Bot"
// through the gamemode's dialogs by reading GetPlayerDialogID/GetPlayerDialogData
// and calling the gamemode's OnDialogResponse via CallRemoteFunction.
// NOTE: NPCs are not network clients: SendClientMessage output is not observable,
// and Util_IsPlaying() returns 0 for NPCs, so commands are also invoked directly (Cmd_*).
#define FILTERSCRIPT
#include <open.mp>

#define BOT_NAME  "Test_Bot"
#define BOT_PASS  "BotTest_2026"

new gBot = INVALID_PLAYER_ID, gTicks, gSame, gLastDlg = -1, gSpawnTries, gDone, gPhase, gPhaseTick;
new gTimer;

forward Bot_Create();
forward Bot_Tick();

public OnFilterScriptInit()
{
	print("[BOT] harness loaded");
	SetTimer("Bot_Create", 4000, false);
	return 1;
}

public Bot_Create()
{
	gBot = NPC_Create(BOT_NAME);
	printf("[BOT] NPC_Create(%s) -> %d  connected=%d npc=%d", BOT_NAME, gBot, IsPlayerConnected(gBot), IsPlayerNPC(gBot));
	gTimer = SetTimer("Bot_Tick", 1000, true);
	return 1;
}

public OnPlayerConnect(playerid)
{
	new n[25]; GetPlayerName(playerid, n, sizeof n);
	printf("[BOT] OnPlayerConnect id=%d name=%s npc=%d", playerid, n, IsPlayerNPC(playerid));
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	printf("[BOT] OnPlayerDisconnect id=%d reason=%d", playerid, reason);
	return 1;
}

public OnPlayerSpawn(playerid)
{
	new Float:x, Float:y, Float:z; GetPlayerPos(playerid, x, y, z);
	printf("[BOT] OnPlayerSpawn id=%d pos=%.1f,%.1f,%.1f skin=%d vw=%d int=%d", playerid, x, y, z, GetPlayerSkin(playerid), GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
	return 1;
}

public OnNPCSpawn(npcid)
{
	printf("[BOT] OnNPCSpawn npcid=%d", npcid);
	return 1;
}

Bot_Answer(dlg, DIALOG_STYLE:style, &resp, &li, input[], size)
{
	resp = 1; li = 0; input[0] = 0;
	switch (dlg)
	{
		case 9001, 9002: format(input, size, "%s", BOT_PASS);
		case 9003, 9004, 9005: li = 0;
		case 9006: format(input, size, "%s", BOT_NAME);
		case 9007: format(input, size, "14/05/1998");
		case 9810: li = 0;
		default:
		{
			if (style == DIALOG_STYLE_MSGBOX) li = -1;
		}
	}
}

Bot_Cmd(const cmd[])
{
	new r = CallRemoteFunction("OnPlayerCommandText", "is", gBot, cmd);
	printf("[BOT] OnPlayerCommandText(\"%s\") -> %d", cmd, r);
}

Bot_Direct(const fn[], const params[])
{
	new r = CallRemoteFunction(fn, "ds", gBot, params);
	printf("[BOT] %s(\"%s\") direct -> %d", fn, params, r);
}

public Bot_Tick()
{
	gTicks++;
	if (gBot == INVALID_PLAYER_ID || !IsPlayerConnected(gBot))
	{
		printf("[BOT] bot not connected at tick %d (kicked?) - stopping", gTicks);
		KillTimer(gTimer);
		return 1;
	}
	if (gTicks > 150)
	{
		print("[BOT] TIMEOUT 150s - destroying NPC");
		NPC_Destroy(gBot);
		KillTimer(gTimer);
		return 1;
	}
	if (gTicks == 2 || (gLastDlg == -1 && gTicks % 15 == 0))
	{
		// a real 0.3.7 client sends RequestClass right after connecting; NPCs do not
		new r = CallRemoteFunction("OnPlayerRequestClass", "ii", gBot, 0);
		printf("[BOT] simulated OnPlayerRequestClass -> %d", r);
	}
	new dlg = GetPlayerDialogID(gBot);
	if (dlg != -1 && dlg != 65535)
	{
		new DIALOG_STYLE:style, title[128], body[1024], b1[64], b2[64];
		GetPlayerDialogData(gBot, style, title, sizeof title, body, sizeof body, b1, sizeof b1, b2, sizeof b2);
		for (new i = 0; body[i]; i++) if (body[i] == '\n' || body[i] == '\t') body[i] = '|';
		if (strlen(body) > 200) body[200] = 0;
		printf("[BOT] dialog id=%d style=%d title=\"%s\" body=\"%s\"", dlg, _:style, title, body);
		if (dlg == gLastDlg) gSame++; else gSame = 0;
		gLastDlg = dlg;
		if (gSame >= 3)
		{
			printf("[BOT] FAIL: dialog %d shown again %d times after answering - giving up", dlg, gSame);
			NPC_Destroy(gBot);
			KillTimer(gTimer);
			return 1;
		}
		new resp, li, input[64];
		Bot_Answer(dlg, style, resp, li, input, sizeof input);
		HidePlayerDialog(gBot);
		new r = CallRemoteFunction("OnDialogResponse", "iiiis", gBot, dlg, resp, li, input);
		printf("[BOT] -> OnDialogResponse(%d, resp=%d, li=%d, input=\"%s\") returned %d", dlg, resp, li, (dlg == 9001 || dlg == 9002) ? "***" : input, r);
		gPhaseTick = gTicks;
		return 1;
	}
	// no dialog pending
	if (gLastDlg == 9602 && gPhase == 0)
	{
		// intro cinematic running (no skip flag). It runs in a private virtual world;
		// wait for the gamemode to put the player back in world 0, trying /salta every 10 s.
		static seenIntroVw;
		new vw = GetPlayerVirtualWorld(gBot);
		if (vw != 0) seenIntroVw = 1;
		if (seenIntroVw && vw == 0)
		{
			printf("[BOT] intro finished after %d s (world back to 0)", gTicks - gPhaseTick);
			gLastDlg = 9004; gPhaseTick = gTicks;
			return 1;
		}
		if ((gTicks - gPhaseTick) % 10 == 5) Bot_Cmd("/salta");
		if (gTicks - gPhaseTick > 90) { print("[BOT] intro did not end within 90 s"); gLastDlg = 9004; gPhaseTick = gTicks; }
		return 1;
	}
	if ((gLastDlg == 9810 || gLastDlg == 9004) && gPhase == 0 && gTicks - gPhaseTick >= 2)
	{
		if (GetPlayerState(gBot) == PLAYER_STATE_SPAWNED || GetPlayerState(gBot) == PLAYER_STATE_ONFOOT || NPC_IsSpawned(gBot))
		{
			printf("[BOT] bot spawned (state %d)", _:GetPlayerState(gBot));
			gPhase = 1; gPhaseTick = gTicks;
		}
		else if (gTicks - gPhaseTick >= 3 && gSpawnTries < 2)
		{
			gSpawnTries++;
			printf("[BOT] not spawned after spawn-select (state %d): calling NPC_Spawn", _:GetPlayerState(gBot));
			NPC_Spawn(gBot);
		}
		else if (gSpawnTries >= 2 && gTicks - gPhaseTick >= 6)
		{
			print("[BOT] still not spawned - running commands anyway");
			gPhase = 1; gPhaseTick = gTicks;
		}
		return 1;
	}
	if (gPhase == 1 && gTicks - gPhaseTick >= 3)
	{
		Bot_Cmd("/help");
		Bot_Cmd("/stats");
		Bot_Cmd("/me saluta tutti");
		Bot_Cmd("/comandoinesistente");
		Bot_Direct("Cmd_stats", "");
		Bot_Direct("Cmd_help", "");
		Bot_Direct("Cmd_me", "saluta tutti");
		Bot_Direct("Cmd_saldo", "");
		Bot_Direct("Cmd_inventario", "");
		gPhase = 2; gPhaseTick = gTicks;
		return 1;
	}
	if (gPhase == 2 && gTicks - gPhaseTick >= 5)
	{
		print("[BOT] done - destroying NPC (disconnect)");
		gDone = 1;
		NPC_Destroy(gBot);
		KillTimer(gTimer);
		return 1;
	}
	if (gPhase == 0 && gTicks % 10 == 0)
	{
		printf("[BOT] waiting... tick %d, no dialog, state=%d lastDlg=%d", gTicks, _:GetPlayerState(gBot), gLastDlg);
	}
	return 1;
}
