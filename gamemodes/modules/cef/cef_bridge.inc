new g_CefNonce[MAX_PLAYERS];

CefBridge_OnInit()
{
#if defined LSRP_CEF
	cef_subscribe("lssc:ready", "Cef_OnReady");
	cef_subscribe("lssc:close", "Cef_OnClose");
	cef_subscribe("lssc:accept", "Cef_OnAccept");
	cef_subscribe("lssc:abandon", "Cef_OnAbandon");
	cef_subscribe("lssc:board_refresh", "Cef_OnBoardRefresh");
	cef_subscribe("lssc:district", "Cef_OnDistrict");
	cef_subscribe("lssc:project_open", "Cef_OnProjectOpen");
	cef_subscribe("lssc:contribute", "Cef_OnContribute");
	cef_subscribe("lssc:bank_req", "Cef_OnBankReq");
	cef_subscribe("lssc:rep_req", "Cef_OnRepReq");
	cef_subscribe("lssc:job_waypoint", "Cef_OnJobWaypoint");
	cef_subscribe("lssc:login", "Cef_OnLogin");
	cef_subscribe("lssc:register", "Cef_OnRegister");
	cef_subscribe("lssc:gender", "Cef_OnGender");
	cef_subscribe("lssc:skin", "Cef_OnSkin");
	cef_subscribe("lssc:story", "Cef_OnStory");
	cef_subscribe("lssc:quit", "Cef_OnQuit");
	cef_subscribe("lssc:chat_send", "Cef_OnChatSend");
	cef_subscribe("lssc:quick", "Cef_OnQuick");
	cef_subscribe("lssc:quick_close", "Cef_OnQuickClose");
	cef_subscribe("lssc:spawn_pick", "Cef_OnSpawnPick");
	cef_subscribe("lssc:char_pick", "Cef_OnCharPick");
	cef_subscribe("lssc:view_ok", "Cef_OnViewOk");
	cef_subscribe("lssc:char_create", "Cef_OnCharCreate");
	cef_subscribe("lssc:char_back", "Cef_OnCharBack");
	SetTimer("CefBridge_ClockTick", 30000, true);
	Log_Write("CEF", "interfaccia CEF attiva: pagine da <GTA>/cef/assets/index.html");
#endif
}

CefBridge_OnPlayerConnect(playerid)
{
	CefUI_Reset(playerid);
#if defined LSRP_CEF
	g_CefNonce[playerid]++;
	SetTimerEx("CefBridge_DecideTimeout", 9000, false, "dd", playerid, g_CefNonce[playerid]);
#endif
}

CefBridge_Decided(playerid)
{
	if (CefUI_Decide(playerid))
	{
		Account_ShowPrompt(playerid);
	}
}

public CefBridge_DecideTimeout(playerid, nonce)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || g_CefNonce[playerid] != nonce || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (!CefUI_IsDecided(playerid))
	{
		CefBridge_Decided(playerid);
	}
	return 1;
}

CefBridge_OnPlayerDisconnect(playerid)
{
	CefUI_Reset(playerid);
#if defined LSRP_CEF
}

CefBridge_OnInitialize(playerid, success)
{
	if (!IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!success)
	{
		if (CefUI_State(playerid) != 2)
		{
			CefUI_SetState(playerid, 0);
		}
		CefBridge_Decided(playerid);
		return 0;
	}
	CefUI_SetState(playerid, 1);
	cef_create_browser(playerid, 1, "index.html", false, false);
#endif
	return 0;
}

CefBridge_Arg(const arguments[], idx, def)
{
	new pos = 0, tok[16];
	for (new i = 0; i <= idx; i++)
	{
		tok[0] = 0;
		Util_NextToken(arguments, pos, tok, 16);
		if (tok[0] == 0)
		{
			return def;
		}
	}
	return strval(tok);
}

bool:CefBridge_Ok(playerid)
{
	return IsPlayerConnected(playerid) && CefUI_Active(playerid) && Util_IsPlaying(playerid);
}

CefBridge_SendProfile(playerid)
{
	if (!CefBridge_Ok(playerid))
	{
		return 0;
	}
	new name[25], job[32];
	GetPlayerName(playerid, name, 25);
	Job_NameOf(gPlayer[playerid][pJob], job, 32);
	cef_emit_event(playerid, "lssc:profile", 0, name, 0, job);
	return 0;
}

CefBridge_CmdTelefono(playerid)
{
	if (!CefUI_Active(playerid))
	{
		Util_Send(playerid, 0x99CCFFFF, "Il telefono CityOS richiede il client CEF. Comandi: /contratti · /contratto · /quartiere · /reputazione · /saldo");
		return 0;
	}
	CefBridge_SendProfile(playerid);
	CefUI_HudSend(playerid);
	CefUI_Open(playerid, "phone", "home");
	return 0;
}

public CefBridge_ClockTick()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && CefUI_Active(i) && CefUI_HudIsOn(i))
		{
			CefUI_HudSend(i);
		}
	}
	return 1;
}

public Cef_OnReady(playerid, const arguments[])
{
	if (!IsPlayerConnected(playerid) || CefUI_State(playerid) == 0)
	{
		return 1;
	}
	CefUI_SetState(playerid, 2);
	CefUI_Decide(playerid);
	switch (gPlayer[playerid][pAuthState])
	{
		case 1, 3, 5, 6:
		{
			HidePlayerDialog(playerid);
			Account_ShowPrompt(playerid);
		}
		case 15:
		{
			HidePlayerDialog(playerid);
			if (gPlayer[playerid][pClassRequested])
			{
				Intro_StoryBegin(playerid);
			}
		}
	}
	new msg[64];
	format(msg, 64, "pagina pronta per il giocatore %d", playerid);
	Log_Write("CEF", msg);
	if (UI_HudIsActive(playerid))
	{
		UI_HudDestroy(playerid);
		CefUI_HudShow(playerid, 1);
	}
	else
	{
		CefUI_HudShow(playerid, CefUI_HudIsOn(playerid));
	}
	CefBridge_SendProfile(playerid);
	return 1;
}

public Cef_OnClose(playerid, const arguments[])
{
	if (IsPlayerConnected(playerid))
	{
		CefUI_OnPageClosed(playerid);
	}
	return 1;
}

public Cef_OnAccept(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid))
	{
		return 1;
	}
	new id = CefBridge_Arg(arguments, 0, 0);
	if (id > 0)
	{
		UI_OnContractAccept(playerid, id);
	}
	return 1;
}

public Cef_OnAbandon(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid))
	{
		return 1;
	}
	new id = CefBridge_Arg(arguments, 0, 0);
	if (id > 0)
	{
		UI_OnContractAbandon(playerid, id);
	}
	return 1;
}

public Cef_OnBoardRefresh(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid))
	{
		return 1;
	}
	if (!LD_IsReady())
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	CityOS_CmdContratti(playerid, "");
	return 1;
}

public Cef_OnDistrict(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid) || !LD_IsReady())
	{
		return 1;
	}
	new d = CefBridge_Arg(arguments, 0, 0);
	if (d < 0 || d > 3)
	{
		d = 0;
	}
	if (d == 0)
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		d = GetPlayerInterior(playerid) == 0 ? District_At(x, y) : 0;
		if (d < 1 || d > 3)
		{
			d = 1;
		}
	}
	UI_OnDistrictPick(playerid, d);
	LD_OnboardAction(playerid, 2);
	return 1;
}

public Cef_OnProjectOpen(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid) || !LD_IsReady())
	{
		return 1;
	}
	new d = CefBridge_Arg(arguments, 0, 1);
	if (d < 1 || d > 3)
	{
		d = 1;
	}
	UI_OnProjectOpen(playerid, d);
	return 1;
}

public Cef_OnContribute(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid) || !LD_IsReady())
	{
		return 1;
	}
	new pid = CefBridge_Arg(arguments, 0, 0), amount = CefBridge_Arg(arguments, 1, 0);
	if (pid > 0 && amount > 0)
	{
		UI_OnProjectContribute(playerid, pid, amount);
	}
	return 1;
}

public Cef_OnBankReq(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid))
	{
		return 1;
	}
	CefUI_HudSend(playerid);
	cef_emit_event(playerid, "lssc:bank_begin");
	cef_emit_event(playerid, "lssc:bank_end");
	return 1;
}

public Cef_OnRepReq(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid) || !LD_IsReady())
	{
		return 1;
	}
	CefBridge_SendProfile(playerid);
	CityOS_CmdReputazione(playerid, "");
	LD_OnboardAction(playerid, 4);
	return 1;
}

public Cef_OnJobWaypoint(playerid, const arguments[])
{
	if (!CefBridge_Ok(playerid) || !LD_IsReady())
	{
		return 1;
	}
	Hub_CmdAiuto(playerid, "lavoro");
	CefUI_Toast(playerid, "Job Center segnato sulla mappa", 1);
	return 1;
}

bool:CefBridge_HexDecode(const hex[], dest[], size)
{
	dest[0] = 0;
	new len = strlen(hex);
	if (len == 0 || len % 2 != 0 || len / 2 >= size)
	{
		return false;
	}
	for (new i = 0, o = 0; i < len; i += 2, o++)
	{
		new hi = CefBridge_HexVal(hex[i]), lo = CefBridge_HexVal(hex[i + 1]);
		if (hi < 0 || lo < 0 || (hi == 0 && lo == 0))
		{
			dest[0] = 0;
			return false;
		}
		dest[o] = hi * 16 + lo;
		dest[o + 1] = 0;
	}
	return true;
}

CefBridge_HexVal(c)
{
	if (c >= 48 && c <= 57)
	{
		return c - 48;
	}
	if (c >= 97 && c <= 102)
	{
		return c - 97 + 10;
	}
	if (c >= 65 && c <= 70)
	{
		return c - 65 + 10;
	}
	return -1;
}

bool:CefBridge_AuthOk(playerid)
{
	return IsPlayerConnected(playerid) && CefUI_Active(playerid) && !gPlayer[playerid][pLoggedIn];
}

public Cef_OnLogin(playerid, const arguments[])
{
	if (!CefBridge_AuthOk(playerid) || gPlayer[playerid][pAuthState] != 3)
	{
		return 1;
	}
	new hex[68], pw[34], pos = 0;
	Util_NextToken(arguments, pos, hex, 68);
	if (!CefBridge_HexDecode(hex, pw, 34))
	{
		pw[0] = 0;
	}
	UI_CefLoginSubmit(playerid, pw);
	for (new i = 0; i < 34; i++)
	{
		pw[i] = 0;
	}
	for (new i = 0; i < 68; i++)
	{
		hex[i] = 0;
	}
	return 1;
}

public Cef_OnRegister(playerid, const arguments[])
{
	if (!CefBridge_AuthOk(playerid) || gPlayer[playerid][pAuthState] != 1)
	{
		return 1;
	}
	new hex[68], pw[34], pos = 0;
	Util_NextToken(arguments, pos, hex, 68);
	if (!CefBridge_HexDecode(hex, pw, 34))
	{
		pw[0] = 0;
	}
	new email[255];
	Util_NextToken(arguments, pos, email, 255);
	Account_SetPendingEmail(playerid, email[0] == 45 && email[1] == 0 ? "" : email);
	UI_CefRegisterSubmit(playerid, pw);
	for (new i = 0; i < 34; i++)
	{
		pw[i] = 0;
	}
	for (new i = 0; i < 68; i++)
	{
		hex[i] = 0;
	}
	return 1;
}

public Cef_OnGender(playerid, const arguments[])
{
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid) || gPlayer[playerid][pAuthState] != 5)
	{
		return 1;
	}
	UI_OnGenderChosen(playerid, CefBridge_Arg(arguments, 0, -1));
	return 1;
}

public Cef_OnSkin(playerid, const arguments[])
{
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid) || gPlayer[playerid][pAuthState] != 6)
	{
		return 1;
	}
	UI_CefSkinPick(playerid, CefBridge_Arg(arguments, 0, -1));
	return 1;
}

public Cef_OnStory(playerid, const arguments[])
{
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid) || gPlayer[playerid][pAuthState] != 15)
	{
		return 1;
	}
	UI_OnStoryChosen(playerid, CefBridge_Arg(arguments, 0, 0), CefBridge_Arg(arguments, 1, 0));
	return 1;
}

public Cef_OnQuit(playerid, const arguments[])
{
	// "გასვლა": in qualunque momento prima dello spawn (login, registrazione, personaggio, storia)
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid) || gPlayer[playerid][pSpawned])
	{
		return 1;
	}
	cef_emit_event(playerid, "lssc:close", 0, "");   // via la pagina: si vede la disconnessione
	CefUI_OnPageClosed(playerid);
	Account_Kick(playerid, "ნახვამდის! თამაშიდან გასასვლელად დაწერე /q");
	return 1;
}
