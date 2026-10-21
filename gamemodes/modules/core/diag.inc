// =============================================================================
// core/diag.inc - diagnostica dei crash del client.
//
// Il client GTA che crasha non manda niente al server: il server vede solo un timeout (reason 0) qualche secondo
// dopo. Qui ogni passo importante del login/spawn/intro viene ricordato; al timeout il log del server scrive
// l'ultimo passo, da quanto tempo, la posizione e quanti oggetti dello streamer il giocatore aveva intorno.
// =============================================================================

new g_DiagStep[MAX_PLAYERS][32];
new g_DiagTick[MAX_PLAYERS];

stock Diag_Step(playerid, const step[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return;
	}
	g_DiagStep[playerid][0] = '\0';
	strcat(g_DiagStep[playerid], step, sizeof g_DiagStep[]);
	g_DiagTick[playerid] = GetTickCount();
}

Diag_OnPlayerConnect(playerid)
{
	Diag_Step(playerid, "connessione");
}

Diag_OnPlayerDisconnect(playerid, reason)
{
	if (reason == 0)
	{
		new name[MAX_PLAYER_NAME + 1], Float:x, Float:y, Float:z;
		GetPlayerName(playerid, name, sizeof name);
		GetPlayerPos(playerid, x, y, z);
		printf("[CRASH?] %s (id %d) timeout: ultimo passo '%s' %d ms prima, pos %.1f %.1f %.1f int %d vw %d, oggetti visibili %d",
			name, playerid, g_DiagStep[playerid], GetTickCount() - g_DiagTick[playerid], x, y, z,
			GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid),
			Streamer_CountVisibleItems(playerid, STREAMER_TYPE_OBJECT));
	}
	g_DiagStep[playerid][0] = '\0';
}
