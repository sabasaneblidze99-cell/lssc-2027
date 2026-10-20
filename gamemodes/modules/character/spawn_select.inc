// =============================================================================
// character/spawn_select.inc - scelta del punto di spawn dopo il login.
//
// Opzioni (solo quelle reali per il personaggio):
//   ultima posizione  - dove il personaggio e' uscito (con "uscito N ore fa")
//   casa              - ingresso della casa di cui e' proprietario
//   fazione           - quartier generale (LSPD / EMS)
//   lavoro            - deposito del lavoro attuale (bus, corriere, nettezza urbana)
//   citta'            - Unity Station (sempre disponibile)
// Regola combattimento: chi si scollega durante uno scontro con polizia/esercito/gang (danni dati o ricevuti
// negli ultimi 2 minuti, oppure ammanettato) rientra OBBLIGATORIAMENTE nell'ultima posizione se rientra entro
// 3 ore; passate 3 ore l'obbligo decade e la scelta torna libera.
// Il dato si salva in characters.combat_logout (migrazione sql/migrations/014_spawn_select.sql). Se la colonna
// manca la scelta funziona lo stesso (senza obbligo): la lettura e' separata e ha un tempo massimo.
// Interfaccia: pagina CEF (vista "spawn") oppure dialog a lista per chi non ha CEF.
// =============================================================================

#define SPAWN_DIALOG        (9810)
#define SPAWN_OPT_MAX       (5)
#define SPAWN_OPT_LAST      (1)
#define SPAWN_OPT_HOUSE     (2)
#define SPAWN_OPT_FACTION   (3)
#define SPAWN_OPT_JOB       (4)
#define SPAWN_OPT_CITY      (5)
#define SPAWN_COMBAT_MS     (120000)      // finestra "in combattimento"
#define SPAWN_FORCE_S       (3 * 3600)    // obbligo di ultima posizione: 3 ore
#define SPAWN_INFO_MS       (3000)        // attesa massima della lettura dal DB
#define SPAWN_CHOICE_MS     (90000)       // senza scelta: prima opzione
#define SPAWN_AUTH_STATE    (16)          // personaggio caricato, sta scegliendo lo spawn

new g_SsKind[MAX_PLAYERS][SPAWN_OPT_MAX];
new Float:g_SsPos[MAX_PLAYERS][SPAWN_OPT_MAX][4];
new g_SsInt[MAX_PLAYERS][SPAWN_OPT_MAX];
new g_SsWorld[MAX_PLAYERS][SPAWN_OPT_MAX];
new g_SsCount[MAX_PLAYERS];
new bool:g_SsForced[MAX_PLAYERS];          // obbligo di ultima posizione (letto dal DB)
new g_SsAwayS[MAX_PLAYERS];                // secondi dall'ultimo salvataggio (-1 = sconosciuto)
new g_SsTimer[MAX_PLAYERS];
new g_SsToken[MAX_PLAYERS];
new g_SsCombatTick[MAX_PLAYERS];           // ultimo scontro con una fazione armata (GetTickCount)
new bool:g_SsPendingForced[MAX_PLAYERS];   // obbligo letto ma non ancora "consumato" con lo spawn

// fazioni "armate" per la regola del combattimento: oggi LSPD (1); esercito e gang vanno aggiunte qui
stock bool:Spawn_IsCombatFaction(faction)
{
	return faction == 1;
}

SpawnSel_OnPlayerConnect(playerid)
{
	g_SsCount[playerid] = 0;
	g_SsForced[playerid] = false;
	g_SsPendingForced[playerid] = false;
	g_SsAwayS[playerid] = -1;
	g_SsCombatTick[playerid] = 0;
	g_SsToken[playerid]++;
	SpawnSel_StopTimer(playerid);
}

SpawnSel_StopTimer(playerid)
{
	if (g_SsTimer[playerid] != 0)
	{
		KillTimer(g_SsTimer[playerid]);
		g_SsTimer[playerid] = 0;
	}
}

// danno tra due giocatori: se uno dei due e' di una fazione armata in servizio, entrambi sono "in combattimento"
SpawnSel_OnDamage(playerid, issuerid)
{
	if (issuerid == INVALID_PLAYER_ID || issuerid < 0 || issuerid >= MAX_PLAYERS || playerid == issuerid)
	{
		return 0;
	}
	if (!gPlayer[playerid][pSpawned] || !gPlayer[issuerid][pSpawned])
	{
		return 0;
	}
	new bool:a = Spawn_IsCombatFaction(gPlayer[playerid][pFaction]) && gPlayer[playerid][pOnDuty] != 0;
	new bool:b = Spawn_IsCombatFaction(gPlayer[issuerid][pFaction]) && gPlayer[issuerid][pOnDuty] != 0;
	if (!a && !b)
	{
		return 0;
	}
	new now = GetTickCount();
	if (!a)
	{
		g_SsCombatTick[playerid] = now;     // il civile colpito o che colpisce un agente
	}
	if (!b)
	{
		g_SsCombatTick[issuerid] = now;
	}
	return 1;
}

stock bool:SpawnSel_InCombat(playerid)
{
	if (gPlayer[playerid][pCuffed])
	{
		return true;
	}
	return g_SsCombatTick[playerid] != 0 && GetTickCount() - g_SsCombatTick[playerid] < SPAWN_COMBAT_MS;
}

// va chiamata PRIMA degli altri moduli (Faction_OnPlayerDisconnect azzera le manette)
SpawnSel_OnPlayerDisconnect(playerid)
{
	SpawnSel_StopTimer(playerid);
	g_SsToken[playerid]++;
	if (!gPlayer[playerid][pLoggedIn] || gPlayer[playerid][pCharId] <= 0 || !DB_IsReady())
	{
		return 0;
	}
	new combat;
	if (gPlayer[playerid][pSpawned])
	{
		combat = SpawnSel_InCombat(playerid) ? 1 : 0;
	}
	else
	{
		combat = g_SsPendingForced[playerid] ? 1 : 0;   // uscito mentre sceglieva: l'obbligo resta
	}
	new query[128];
	mysql_format(g_DB, query, sizeof query, "UPDATE characters SET combat_logout = %d WHERE id = %d", combat, gPlayer[playerid][pCharId]);
	mysql_tquery(g_DB, query);
	if (combat)
	{
		printf("[SPAWN] %d uscito in combattimento (char %d)", playerid, gPlayer[playerid][pCharId]);
	}
	return 1;
}

// al posto di Char_Spawn dopo il caricamento del personaggio
SpawnSel_Begin(playerid)
{
	gPlayer[playerid][pAuthState] = SPAWN_AUTH_STATE;
	g_SsForced[playerid] = false;
	g_SsAwayS[playerid] = -1;
	g_SsToken[playerid]++;
	SpawnSel_StopTimer(playerid);
	Char_SetLoginCamera(playerid);
	if (!DB_IsReady())
	{
		SpawnSel_Show(playerid);
		return 1;
	}
	new query[192];
	mysql_format(g_DB, query, sizeof query, "SELECT combat_logout, COALESCE(GREATEST(0, TIMESTAMPDIFF(SECOND, last_save, NOW())), -1) AS away FROM characters WHERE id = %d", gPlayer[playerid][pCharId]);
	mysql_tquery(g_DB, query, "SpawnSel_OnInfo", "ddd", playerid, gPlayer[playerid][pCharId], g_SsToken[playerid]);
	g_SsTimer[playerid] = SetTimerEx("SpawnSel_OnTimeout", SPAWN_INFO_MS, false, "dd", playerid, g_SsToken[playerid]);
	return 1;
}

public SpawnSel_OnInfo(playerid, charId, token)
{
	if (!IsPlayerConnected(playerid) || g_SsToken[playerid] != token || gPlayer[playerid][pCharId] != charId ||
		gPlayer[playerid][pAuthState] != SPAWN_AUTH_STATE || g_SsCount[playerid] != 0)
	{
		return 1;
	}
	new rows = 0, combat = 0, away = -1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "combat_logout", combat);
		cache_get_value_name_int(0, "away", away);
	}
	g_SsAwayS[playerid] = away;
	g_SsForced[playerid] = combat != 0 && away >= 0 && away < SPAWN_FORCE_S;
	g_SsPendingForced[playerid] = g_SsForced[playerid];
	SpawnSel_StopTimer(playerid);
	SpawnSel_Show(playerid);
	return 1;
}

// lettura non arrivata (colonna mancante, DB lento) oppure scelta non fatta in tempo
public SpawnSel_OnTimeout(playerid, token)
{
	if (!IsPlayerConnected(playerid) || g_SsToken[playerid] != token || gPlayer[playerid][pAuthState] != SPAWN_AUTH_STATE)
	{
		return 1;
	}
	g_SsTimer[playerid] = 0;
	if (g_SsCount[playerid] == 0)
	{
		SpawnSel_Show(playerid);
		return 1;
	}
	SpawnSel_Pick(playerid, 0);
	return 1;
}

SpawnSel_Add(playerid, kind, Float:x, Float:y, Float:z, Float:a, interior, world)
{
	new n = g_SsCount[playerid];
	if (n >= SPAWN_OPT_MAX)
	{
		return 0;
	}
	g_SsKind[playerid][n] = kind;
	g_SsPos[playerid][n][0] = x;
	g_SsPos[playerid][n][1] = y;
	g_SsPos[playerid][n][2] = z;
	g_SsPos[playerid][n][3] = a;
	g_SsInt[playerid][n] = interior;
	g_SsWorld[playerid][n] = world;
	g_SsCount[playerid] = n + 1;
	return 1;
}

// nome del quartiere (Living Los Santos) oppure "Los Santos"
SpawnSel_Zone(Float:x, Float:y, out[], size)
{
	new d = District_At(x, y);
	out[0] = 0;
	if (d > 0)
	{
		LD_DistrictName(d, out, size);
	}
	else
	{
		strcat(out, "ლოს-სანტოსი", size);
	}
}

SpawnSel_HouseOf(playerid)
{
	for (new i = 0; i < g_HouseCount; i++)
	{
		if (g_HouseRows[i][hrOwnerId] > 0 && g_HouseRows[i][hrOwnerId] == gPlayer[playerid][pCharId])
		{
			return i;
		}
	}
	return -1;
}

SpawnSel_Build(playerid)
{
	g_SsCount[playerid] = 0;
	new bool:hasLast = !((0.0 == gPlayer[playerid][pPosX]) && (0.0 == gPlayer[playerid][pPosY]) && (0.0 == gPlayer[playerid][pPosZ]));
	if (hasLast)
	{
		SpawnSel_Add(playerid, SPAWN_OPT_LAST, gPlayer[playerid][pPosX], gPlayer[playerid][pPosY], gPlayer[playerid][pPosZ], gPlayer[playerid][pAngle],
			gPlayer[playerid][pInterior], gPlayer[playerid][pWorld]);
	}
	if (g_SsForced[playerid] && hasLast)
	{
		return 1;       // uscito in combattimento: solo l'ultima posizione
	}
	new h = SpawnSel_HouseOf(playerid);
	if (h >= 0)
	{
		new Float:a = g_HouseRows[h][hrExtA];
		SpawnSel_Add(playerid, SPAWN_OPT_HOUSE, g_HouseRows[h][hrExtX] + 2.0 * floatsin(-a, degrees), g_HouseRows[h][hrExtY] + 2.0 * floatcos(-a, degrees),
			g_HouseRows[h][hrExtZ], a, 0, 0);
	}
	new f = gPlayer[playerid][pFaction];
	if (f >= 1 && f < sizeof g_FacHq && gPlayer[playerid][pFactionRank] >= 1)
	{
		SpawnSel_Add(playerid, SPAWN_OPT_FACTION, g_FacHq[f][0], g_FacHq[f][1], g_FacHq[f][2], 0.0, 0, 0);
	}
	new j = gPlayer[playerid][pJob];
	if (j >= 1 && j <= 3)
	{
		new Float:x, Float:y, Float:z, Float:a;
		Job_SpawnPos(j, x, y, z, a);
		SpawnSel_Add(playerid, SPAWN_OPT_JOB, x - 3.0, y - 3.0, z, a, 0, 0);    // accanto al veicolo di servizio
	}
	new Float:cx, Float:cy, Float:cz, Float:ca;
	Char_GetSpawnPoint(0, cx, cy, cz, ca);
	SpawnSel_Add(playerid, SPAWN_OPT_CITY, cx, cy, cz, ca, 0, 0);
	return 1;
}

SpawnSel_Away(playerid, out[], size)
{
	new s = g_SsAwayS[playerid];
	out[0] = 0;
	if (s < 0)
	{
		return 0;
	}
	if (s < 3600)
	{
		format(out, size, "გამოხვედი %d წუთის წინ", s / 60 > 0 ? s / 60 : 1);
	}
	else if (s < 86400)
	{
		format(out, size, "გამოხვედი %d საათის წინ", s / 3600);
	}
	else
	{
		format(out, size, "გამოხვედი %d დღის წინ", s / 86400);
	}
	return 1;
}

SpawnSel_Texts(playerid, idx, icon[], isize, title[], tsize, sub[], ssize)
{
	icon[0] = 0;
	title[0] = 0;
	sub[0] = 0;
	switch (g_SsKind[playerid][idx])
	{
		case SPAWN_OPT_LAST:
		{
			strcat(icon, "pin", isize);
			strcat(title, "ბოლო ადგილი", tsize);
			new away[96], zone[64];
			SpawnSel_Away(playerid, away, sizeof away);
			SpawnSel_Zone(g_SsPos[playerid][idx][0], g_SsPos[playerid][idx][1], zone, sizeof zone);
			if (g_SsInt[playerid][idx] != 0)
			{
				format(sub, ssize, "შენობაში%s%s", away[0] ? " - " : "", away);
			}
			else
			{
				format(sub, ssize, "%s%s%s", zone, (zone[0] && away[0]) ? " - " : "", away);
			}
		}
		case SPAWN_OPT_HOUSE:
		{
			strcat(icon, "home", isize);
			strcat(title, "სახლი", tsize);
			SpawnSel_Zone(g_SsPos[playerid][idx][0], g_SsPos[playerid][idx][1], sub, ssize);
		}
		case SPAWN_OPT_FACTION:
		{
			new f = gPlayer[playerid][pFaction];
			strcat(icon, f == 2 ? "medic" : "badge", isize);
			strcat(title, f == 2 ? "საავადმყოფო (EMS)" : "პოლიციის განყოფილება (LSPD)", tsize);
			strcat(sub, "ფრაქციის შტაბი", ssize);
		}
		case SPAWN_OPT_JOB:
		{
			strcat(icon, "briefcase", isize);
			strcat(title, "სამსახური", tsize);
			Job_NameOf(gPlayer[playerid][pJob], sub, ssize);
		}
		default:
		{
			strcat(icon, "city", isize);
			strcat(title, "ქალაქი", tsize);
			strcat(sub, "Unity Station", ssize);
		}
	}
}

SpawnSel_Show(playerid)
{
	SpawnSel_Build(playerid);
	if (g_SsCount[playerid] <= 1 && !g_SsForced[playerid])
	{
		SpawnSel_Pick(playerid, 0);      // personaggio nuovo: niente da scegliere
		return 1;
	}
	new icon[16], title[96], sub[192];
	if (CefUI_Active(playerid))
	{
		cef_emit_event(playerid, "lssc:spawn_begin", 1, g_SsForced[playerid] ? 1 : 0, 1, SPAWN_CHOICE_MS / 1000);
		for (new i = 0; i < g_SsCount[playerid]; i++)
		{
			SpawnSel_Texts(playerid, i, icon, sizeof icon, title, sizeof title, sub, sizeof sub);
			cef_emit_event(playerid, "lssc:spawn_add", 1, i, 0, icon, 0, title, 0, sub);
		}
		CefUI_Open(playerid, "spawn", "");
	}
	else
	{
		new body[1400], line[320];
		for (new i = 0; i < g_SsCount[playerid]; i++)
		{
			SpawnSel_Texts(playerid, i, icon, sizeof icon, title, sizeof title, sub, sizeof sub);
			format(line, sizeof line, "%s{FFFFFF}%s\t{AAAAAA}%s", i ? "\n" : "", title, sub);
			strcat(body, line);
		}
		Lang_ShowPlayerDialog(playerid, SPAWN_DIALOG, DIALOG_STYLE_TABLIST,
			g_SsForced[playerid] ? "ბრძოლის დროს გახვედი: ბრუნდები იქ, სადაც იყავი" : "სად გინდა გაჩნდე?", body, "არჩევა", "");
	}
	if (g_SsForced[playerid])
	{
		Util_Send(playerid, 0xFF914DFF, "ბრძოლის დროს გახვედი: 3 საათის განმავლობაში ბრუნდები ბოლო ადგილზე.");
	}
	SpawnSel_StopTimer(playerid);
	g_SsTimer[playerid] = SetTimerEx("SpawnSel_OnTimeout", SPAWN_CHOICE_MS, false, "dd", playerid, g_SsToken[playerid]);
	return 1;
}

SpawnSel_Pick(playerid, idx)
{
	if (gPlayer[playerid][pAuthState] != SPAWN_AUTH_STATE)
	{
		return 0;
	}
	SpawnSel_StopTimer(playerid);
	g_SsToken[playerid]++;
	if (g_SsCount[playerid] == 0)
	{
		SpawnSel_Build(playerid);
	}
	if (idx < 0 || idx >= g_SsCount[playerid])
	{
		idx = 0;
	}
	gPlayer[playerid][pPosX] = g_SsPos[playerid][idx][0];
	gPlayer[playerid][pPosY] = g_SsPos[playerid][idx][1];
	gPlayer[playerid][pPosZ] = g_SsPos[playerid][idx][2];
	gPlayer[playerid][pAngle] = g_SsPos[playerid][idx][3];
	gPlayer[playerid][pInterior] = g_SsInt[playerid][idx];
	gPlayer[playerid][pWorld] = g_SsWorld[playerid][idx];
	g_SsPendingForced[playerid] = false;
	g_SsForced[playerid] = false;
	g_SsCount[playerid] = 0;
	if (CefUI_Active(playerid))
	{
		CefUI_Close(playerid, "spawn");
	}
	if (DB_IsReady() && gPlayer[playerid][pCharId] > 0)
	{
		new query[96];
		mysql_format(g_DB, query, sizeof query, "UPDATE characters SET combat_logout = 0 WHERE id = %d", gPlayer[playerid][pCharId]);
		mysql_tquery(g_DB, query);
	}
	Char_Spawn(playerid);
	return 1;
}

bool:SpawnSel_OnDialogResponse(playerid, dialogid, listitem)
{
	if (dialogid != SPAWN_DIALOG)
	{
		return false;
	}
	if (gPlayer[playerid][pAuthState] == SPAWN_AUTH_STATE)
	{
		SpawnSel_Pick(playerid, listitem);
	}
	return true;
}

public Cef_OnSpawnPick(playerid, arguments[])
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != SPAWN_AUTH_STATE || g_SsCount[playerid] == 0)
	{
		return 1;
	}
	SpawnSel_Pick(playerid, CefBridge_Arg(arguments, 0, 0));
	return 1;
}

// durante la scelta la selezione di classe non deve far nulla (Char_OnRequestClass)
stock bool:SpawnSel_Active(playerid)
{
	return gPlayer[playerid][pAuthState] == SPAWN_AUTH_STATE;
}
