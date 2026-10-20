#if !defined LSRP_CEF

cef_emit_event(player_id, const event[], {Float,_}:...)
{
	return 0;
}

cef_focus_browser(player_id, browser_id, bool:focused)
{
	return 0;
}
#endif
new g_CefState[MAX_PLAYERS];
new bool:g_CefFocus[MAX_PLAYERS];
new bool:g_CefHudOn[MAX_PLAYERS];
new g_CefHud[MAX_PLAYERS][5];
new bool:g_CefHudHas[MAX_PLAYERS];
new g_CefView[MAX_PLAYERS];
new bool:g_CefDecided[MAX_PLAYERS];
new bool:g_CefPromptWait[MAX_PLAYERS];

CefUI_Reset(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_CefState[playerid] = 0;
	g_CefFocus[playerid] = false;
	g_CefHudOn[playerid] = false;
	g_CefHudHas[playerid] = false;
	g_CefView[playerid] = 0;
#if defined LSRP_CEF
	g_CefDecided[playerid] = false;
#else
	g_CefDecided[playerid] = 1;
#endif
	g_CefPromptWait[playerid] = false;
	for (new i = 0; i < 5; i++)
	{
		g_CefHud[playerid][i] = 0;
	}
	return 0;
}

CefUI_Active(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	return g_CefState[playerid] == 2 ? 1 : 0;
}

CefUI_State(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	return g_CefState[playerid];
}

CefUI_SetState(playerid, st)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_CefState[playerid] = st;
	return 0;
}

CefUI_Open(playerid, const view[], const arg[])
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	new v = !strcmp(view, "phone", false, 0x7FFFFFFF) ? 2 : (!strcmp(view, "auth", false, 0x7FFFFFFF) ? 3 : 1);
	if (v == 2 && g_CefView[playerid] == 2)
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:open", 0, view, 0, arg);
	cef_focus_browser(playerid, 1, true);
	g_CefFocus[playerid] = true;
	g_CefView[playerid] = v;
	return 0;
}

CefUI_OnPageClosed(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_CefView[playerid] = 0;
	CefUI_ReleaseFocus(playerid);
	return 0;
}

CefUI_Close(playerid, const view[])
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:close", 0, view);
	g_CefView[playerid] = 0;
	CefUI_ReleaseFocus(playerid);
	return 0;
}

CefUI_ReleaseFocus(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !g_CefFocus[playerid])
	{
		return 0;
	}
	cef_focus_browser(playerid, 1, false);
	g_CefFocus[playerid] = false;
	return 0;
}

CefUI_Toast(playerid, const text[], kind)
{
	if (!CefUI_Active(playerid) || text[0] == 0)
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:toast", 0, text, 1, kind);
	return 0;
}

CefUI_Reject(playerid, const text[])
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:reject", 0, text);
	return 0;
}

CefUI_HudData(playerid, money, bank, level, expIn, expStep)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_CefHud[playerid][0] = money;
	g_CefHud[playerid][1] = bank;
	g_CefHud[playerid][2] = level;
	g_CefHud[playerid][3] = expIn;
	g_CefHud[playerid][4] = expStep;
	g_CefHudHas[playerid] = true;
	HudCef_OnHudData(playerid, money, bank, level);
	if (CefUI_Active(playerid) && g_CefHudOn[playerid])
	{
		CefUI_HudSend(playerid);
	}
	return 0;
}

CefUI_HudSend(playerid)
{
	if (!CefUI_Active(playerid) || !g_CefHudHas[playerid])
	{
		return 0;
	}
	new hour = 0, minute = 0;
	gettime(hour, minute);
	cef_emit_event(playerid, "lssc:hud", 1, g_CefHud[playerid], 1, g_CefHud[playerid][1], 1, g_CefHud[playerid][2], 1, g_CefHud[playerid][3], 1, g_CefHud[playerid][4], 1, hour, 1, minute);
	return 0;
}

CefUI_HudShow(playerid, bool:show)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_CefHudOn[playerid] = show;
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:hud_visible", 1, show ? 1 : 0);
	if (show)
	{
		CefUI_AuthClose(playerid);
		CefUI_HudSend(playerid);
	}
	HudCef_OnVisible(playerid, show);
	return 0;
}

bool:CefUI_HudIsOn(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return false;
	}
	return g_CefHudOn[playerid];
}

CefUI_AuthGate(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	if (g_CefDecided[playerid])
	{
		return 1;
	}
	if (!g_CefPromptWait[playerid])
	{
		SendClientMessage(playerid, 0xAAAAAAFF, "Caricamento dell'interfaccia...");
	}
	g_CefPromptWait[playerid] = true;
	return 0;
}

bool:CefUI_Decide(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return false;
	}
	g_CefDecided[playerid] = true;
	new bool:wait = g_CefPromptWait[playerid];
	g_CefPromptWait[playerid] = false;
	return wait;
}

bool:CefUI_IsDecided(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return true;
	}
	return g_CefDecided[playerid];
}

CefUI_AuthStep(playerid, const step[], const err[], n)
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	new name[25];
	GetPlayerName(playerid, name, 25);
	cef_emit_event(playerid, "lssc:auth", 0, step, 0, name, 0, err, 1, n, 1, 6, 1, 32);
	CefUI_Open(playerid, "auth", step);
	return 0;
}

CefUI_AuthSkin(playerid, idx, skin)
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:auth_skin", 1, idx, 1, skin);
	return 0;
}

CefUI_AuthStoryOpt(playerid, kind, idx, const shortText[], const longText[])
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:auth_story", 1, kind, 1, idx, 0, shortText, 0, longText);
	return 0;
}

CefUI_AuthClose(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || g_CefView[playerid] != 3)
	{
		return 0;
	}
	CefUI_Close(playerid, "auth");
	return 0;
}
