// =============================================================================
// ui/cef_views.inc - viste CEF nuove (chars, spawn) con conferma dalla pagina.
//
// Se il giocatore ha una pagina CEF vecchia (cef/assets non aggiornati) la vista nuova non esiste:
// la pagina non conferma entro 3 s -> si chiude il CEF e si usano i dialog. Cosi' il login non si blocca mai.
// La pagina conferma con lssc:view_ok <nome> (chars.js, spawn.js).
// =============================================================================

#define CEFV_ACK_MS   (3000)
#define CEFV_CHARS    (1)
#define CEFV_SPAWN    (2)

new bool:g_CvOld[MAX_PLAYERS];      // pagina senza le viste nuove: solo dialog
new g_CvTimer[MAX_PLAYERS];
new g_CvWaiting[MAX_PLAYERS];       // vista in attesa di conferma (0 nessuna)

CefViews_OnPlayerConnect(playerid)
{
	g_CvOld[playerid] = false;
	g_CvWaiting[playerid] = 0;
	CefViews_StopTimer(playerid);
}

CefViews_StopTimer(playerid)
{
	if (g_CvTimer[playerid] != 0)
	{
		KillTimer(g_CvTimer[playerid]);
		g_CvTimer[playerid] = 0;
	}
}

// la vista nuova si puo' usare?
stock bool:CefViews_Use(playerid)
{
	return CefUI_Active(playerid) && !g_CvOld[playerid];
}

CefViews_Open(playerid, which, const view[])
{
	new v[16];
	strcat(v, view);
	CefUI_Open(playerid, v, "");
	g_CvWaiting[playerid] = which;
	CefViews_StopTimer(playerid);
	g_CvTimer[playerid] = SetTimerEx("CefViews_OnTimeout", CEFV_ACK_MS, false, "dd", playerid, which);
}

public Cef_OnViewOk(playerid, const arguments[])
{
	if (IsPlayerConnected(playerid))
	{
		g_CvWaiting[playerid] = 0;
		CefViews_StopTimer(playerid);
	}
	return 1;
}

public CefViews_OnTimeout(playerid, which)
{
	g_CvTimer[playerid] = 0;
	if (!IsPlayerConnected(playerid) || g_CvWaiting[playerid] != which)
	{
		return 1;
	}
	g_CvWaiting[playerid] = 0;
	g_CvOld[playerid] = true;
	printf("[CEF] giocatore %d: pagina CEF senza la vista %d (cef/assets da aggiornare): uso i dialog", playerid, which);
	cef_emit_event(playerid, "lssc:close", 0, "");     // chiude qualunque vista (anche il login rimasto a meta')
	CefUI_OnPageClosed(playerid);
	Util_Send(playerid, 0xFFB547FF, "CEF ინტერფეისი მოძველებულია: განაახლე cef/assets. ახლა გამოიყენება ფანჯრები.");
	if (which == CEFV_CHARS)
	{
		CharSel_OnCefFallback(playerid);
	}
	else if (which == CEFV_SPAWN)
	{
		SpawnSel_OnCefFallback(playerid);
	}
	return 1;
}
