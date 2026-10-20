new bool:g_DistBooted;
new bool:g_DistBootInFlight;
new bool:g_DistCycle;
new g_DistTimer;
new g_DistAct[4];
new g_DistSec[4];
new g_DistCrime[4];
new g_DistQType[16];
new g_DistQDist[16];
new g_DistQChar[16];
new g_DistQValue[16];
new g_DistQOp[16][41];
new g_DistQHead;
new g_DistQCount;
new g_DistSeen[32][41];
new g_DistSeenPos;
new g_DistHookHour[MAX_PLAYERS];
new g_DistHookCount[MAX_PLAYERS][4];
new g_DistLoadTry[MAX_PLAYERS];
new bool:g_DistQuietLoad[MAX_PLAYERS];
new g_DistAdminOp[MAX_PLAYERS][41];

bool:District_Valid(districtId)
{
	return districtId >= 1 && districtId <= 3;
}

District_CopyCache()
{
	for (new d = 1; d <= 3; d++)
	{
		if (!DB_LivingDistrictGet(d, 0))
		{
			continue;
		}
		new act = DB_LivingDistrictGet(d, 1);
		new bool:changed = g_DistBooted && act != g_DistAct[d];
		g_DistAct[d] = act;
		g_DistSec[d] = DB_LivingDistrictGet(d, 2);
		g_DistCrime[d] = DB_LivingDistrictGet(d, 3);
		if (changed)
		{
			LD_Emit(105, d, 0, act, "-");
		}
	}
}

bool:District_SeenOp(const opId[])
{
	if (opId[0] == 0 || !strcmp(opId, "-", 0, 0x7FFFFFFF))
	{
		return 0;
	}
	for (new i = 0; i < 32; i++)
	{
		if (g_DistSeen[i][0] != 0 && !strcmp(g_DistSeen[i], opId, 0, 0x7FFFFFFF))
		{
			return 1;
		}
	}
	g_DistSeen[g_DistSeenPos][0] = 0;
	strcat(g_DistSeen[g_DistSeenPos], opId, 41);
	g_DistSeenPos = (g_DistSeenPos + 1) % 32;
	return 0;
}

bool:District_HookAllowed(districtId, charId)
{
	if (charId <= 0)
	{
		return 1;
	}
	new p = DB_LdCharOnline(charId);
	if (p == INVALID_PLAYER_ID)
	{
		return 1;
	}
	new hour = gettime() / 3600;
	if (g_DistHookHour[p] != hour)
	{
		g_DistHookHour[p] = hour;
		for (new d = 0; d <= 3; d++)
		{
			g_DistHookCount[p][d] = 0;
		}
	}
	if (g_DistHookCount[p][districtId] >= 3)
	{
		return 0;
	}
	g_DistHookCount[p][districtId]++;
	return 1;
}

District_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_DistHookHour[playerid] = 0;
	for (new d = 0; d <= 3; d++)
	{
		g_DistHookCount[playerid][d] = 0;
	}
	g_DistLoadTry[playerid] = 0;
	g_DistQuietLoad[playerid] = 0;
	g_DistAdminOp[playerid][0] = 0;
	DB_LivingPlayerReset(playerid);
	return 0;
}

District_StartBoot()
{
	if (g_DistBooted || g_DistBootInFlight)
	{
		return 0;
	}
	g_DistBootInFlight = 1;
	DB_LivingPushConfig();
	DB_LivingBoot();
	return 0;
}

District_OnInit()
{
	g_DistBooted = 0;
	g_DistBootInFlight = 0;
	g_DistCycle = 0;
	g_DistQHead = 0;
	g_DistQCount = 0;
	g_DistSeenPos = 0;
	for (new i = 0; i < 32; i++)
	{
		g_DistSeen[i][0] = 0;
	}
	for (new d = 0; d <= 3; d++)
	{
		g_DistAct[d] = 0;
		g_DistSec[d] = 0;
		g_DistCrime[d] = 0;
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		District_ResetPlayer(p);
	}
	if (g_DistTimer != 0)
	{
		KillTimer(g_DistTimer);
	}
	g_DistTimer = SetTimer("District_OnTick", 30000, 1);
	District_StartBoot();
}

District_OnExit()
{
	if (g_DistTimer != 0)
	{
		KillTimer(g_DistTimer);
	}
	g_DistTimer = 0;
	g_DistBooted = 0;
	g_DistBootInFlight = 0;
	g_DistQCount = 0;
}

District_OnPlayerConnect(playerid)
{
	District_ResetPlayer(playerid);
}

District_OnCharReady(playerid)
{
	g_DistLoadTry[playerid] = 0;
	g_DistQuietLoad[playerid] = 0;
	DB_LivingLoadPlayer(playerid, 1);
}

District_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	District_ResetPlayer(playerid);
}

District_ReloadPlayer(playerid)
{
	if (!Util_IsPlaying(playerid) || gPlayer[playerid][pLivingReady] == 0)
	{
		return 0;
	}
	g_DistQuietLoad[playerid] = 1;
	DB_LivingLoadPlayer(playerid, 0);
	return 0;
}

public District_OnTick()
{
	if (!g_DistBooted)
	{
		District_StartBoot();
		return 1;
	}
	g_DistCycle = 1;
	DB_LivingTick();
	return 1;
}

public Living_OnDBBoot(result)
{
	if (result != 1)
	{
		g_DistBootInFlight = 0;
		Log_Write("DISTRICT", "boot living fallito (DB): nuovo tentativo al prossimo tick");
	}
	return 1;
}

public Living_OnDBDistricts(result, count)
{
	if (result != 1 || count <= 0)
	{
		if (!g_DistBooted && g_DistBootInFlight)
		{
			g_DistBootInFlight = 0;
			Log_Write("DISTRICT", "lettura quartieri fallita durante il boot: nuovo tentativo al prossimo tick");
		}
		g_DistCycle = 0;
		return 1;
	}
	District_CopyCache();
	if (!g_DistBooted)
	{
		if (!g_DistBootInFlight)
		{
			return 1;
		}
		g_DistBooted = 1;
		g_DistBootInFlight = 0;
		g_DistCycle = 1;
		new msg[160];
		format(msg, 160, "boot living completato: %d quartieri, %d contratti per bucket", count, DB_LivingBucketSlots());
		Log_Write("DISTRICT", msg);
		LD_SetReady(1);
	}
	if (g_DistCycle)
	{
		g_DistCycle = 0;
		if (funcidx("Director_OnCycle") != -1)
		{
			CallLocalFunction("Director_OnCycle", "");
		}
	}
	return 1;
}

District_GetActivity(districtId)
{
	return District_Valid(districtId) ? g_DistAct[districtId] : 0;
}

District_GetSecurity(districtId)
{
	return District_Valid(districtId) ? g_DistSec[districtId] : 0;
}

District_GetCrime(districtId)
{
	return District_Valid(districtId) ? g_DistCrime[districtId] : 0;
}

District_SetCache(districtId, activity, security, crime)
{
	if (!District_Valid(districtId))
	{
		return 0;
	}
	if (activity >= 0)
	{
		g_DistAct[districtId] = LD_Clamp(activity, 0, 1000);
	}
	if (security >= 0)
	{
		g_DistSec[districtId] = LD_Clamp(security, 0, 100);
	}
	if (crime >= 0)
	{
		g_DistCrime[districtId] = LD_Clamp(crime, 0, 100);
	}
	return 1;
}

District_Emit(type, districtId, charId, value, const opId[])
{
	if (type >= 100)
	{
		return LD_Emit(type, districtId, charId, value, opId) >= 0 ? 1 : 0;
	}
	if (type < 1 || type > 9 || !District_Valid(districtId))
	{
		return 0;
	}
	if (type == 6 || type == 7)
	{
		if (!g_DistBooted || g_DistQCount >= 16)
		{
			return 0;
		}
		if (!District_HookAllowed(districtId, charId))
		{
			return 0;
		}
		new op[41];
		if (opId[0] == 0 || !strcmp(opId, "-", 0, 0x7FFFFFFF))
		{
			DB_LivingMakeOpId(op, 41, "le", districtId, charId > 0 ? charId : 0);
		}
		else
		{
			strcat(op, opId, 41);
		}
		new slot = (g_DistQHead + g_DistQCount) % 16;
		g_DistQType[slot] = type;
		g_DistQDist[slot] = districtId;
		g_DistQChar[slot] = charId;
		g_DistQValue[slot] = value;
		g_DistQOp[slot][0] = 0;
		strcat(g_DistQOp[slot], op, 41);
		g_DistQCount++;
		DB_LivingEmit(op, districtId, type, charId > 0 ? charId : 0, value);
		return 1;
	}
	if (District_SeenOp(opId))
	{
		return 0;
	}
	LD_Emit(type, districtId, charId, value, opId);
	if (g_DistBooted)
	{
		DB_LivingLoadDistricts();
	}
	return 1;
}

public Living_OnDBEmit(result, districtId, activity, security, crime)
{
	if (g_DistQCount <= 0)
	{
		return 1;
	}
	new slot = g_DistQHead;
	g_DistQHead = (g_DistQHead + 1) % 16;
	g_DistQCount--;
	if (result == 1 || result == 2)
	{
		new d = District_Valid(districtId) ? districtId : g_DistQDist[slot];
		District_SetCache(d, activity, security, crime);
		if (!District_SeenOp(g_DistQOp[slot]))
		{
			LD_Emit(g_DistQType[slot], d, g_DistQChar[slot], g_DistQValue[slot], g_DistQOp[slot]);
		}
		return 1;
	}
	new msg[160];
	format(msg, 160, "evento %d quartiere %d non registrato (esito %d, op %s)", g_DistQType[slot], g_DistQDist[slot], result, g_DistQOp[slot]);
	Log_Write("DISTRICT", msg);
	return 1;
}

public Living_OnDBPlayer(playerid, result, refund)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new bool:quiet = g_DistQuietLoad[playerid];
	if (result == 1)
	{
		g_DistLoadTry[playerid] = 0;
		g_DistQuietLoad[playerid] = 0;
		if (refund > 0)
		{
			new msg[160];
			format(msg, 160, "Un noleggio rimasto aperto e' stato chiuso: $%d di cauzione sono tornati sul tuo conto.", refund);
			Util_Send(playerid, 0x66DD66FF, msg);
		}
		LD_Emit(108, 0, gPlayer[playerid][pCharId], refund, "-");
		return 1;
	}
	if (quiet && result != -1 && result != 10)
	{
		g_DistQuietLoad[playerid] = 0;
		return 1;
	}
	if (g_DistLoadTry[playerid] >= 3)
	{
		g_DistQuietLoad[playerid] = 0;
		if (!quiet)
		{
			Util_Send(playerid, -10270721, "Non e' stato possibile caricare la tua reputazione: riprova a rientrare piu' tardi.");
		}
		Log_Write("DISTRICT", "caricamento dati living del personaggio fallito dopo i tentativi");
		return 1;
	}
	g_DistLoadTry[playerid]++;
	SetTimerEx("District_OnLoadRetry", 3000, 0, "dd", playerid, gPlayer[playerid][pCharId]);
	return 1;
}

public District_OnLoadRetry(playerid, charId)
{
	if (!IsPlayerConnected(playerid) || charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		return 1;
	}
	DB_LivingLoadPlayer(playerid, !g_DistQuietLoad[playerid]);
	return 1;
}

bool:District_ParseOpt(const tok[], &value)
{
	value = -1;
	if (tok[0] == 0 || !strcmp(tok, "-", 0, 0x7FFFFFFF))
	{
		return 1;
	}
	if (!Util_ParseInt(tok, value))
	{
		return 0;
	}
	return value >= -1;
}

bool:District_CmdLdset(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	if (!g_DistBooted)
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tName[16], tAct[16], tSec[16], tCrime[16];
	Util_NextToken(params, pos, tName, 16);
	Util_NextToken(params, pos, tAct, 16);
	Util_NextToken(params, pos, tSec, 16);
	Util_NextToken(params, pos, tCrime, 16);
	new d = LD_ParseDistrict(tName), act = 0, sec = 0, crime = 0;
	if (d == 0 || tAct[0] == 0 || !District_ParseOpt(tAct, act) || !District_ParseOpt(tSec, sec) || !District_ParseOpt(tCrime, crime))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /ldset <idlewood|ganton|jefferson> <attivita' 0-1000> [sicurezza 0-100] [pressione 0-100]  (- = invariato)");
		return 1;
	}
	if (act > 1000 || sec > 100 || crime > 100)
	{
		Util_Send(playerid, -10270721, "Valori fuori range: attivita' 0-1000, sicurezza e pressione 0-100.");
		return 1;
	}
	DB_LivingMakeOpId(g_DistAdminOp[playerid], 41, "la", d, gPlayer[playerid][pCharId]);
	DB_LivingAdminSet(playerid, g_DistAdminOp[playerid], d, act, sec, crime);
	return 1;
}

public Living_OnDBAdminSet(playerid, result, districtId, activity, security, crime)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new msg[160], name[16];
	if (result != 1 && result != 2)
	{
		format(msg, 160, "/ldset non applicato (esito %d).", result);
		Util_Send(playerid, -10270721, msg);
		return 1;
	}
	District_SetCache(districtId, activity, security, crime);
	LD_DistrictName(districtId, name, 16);
	format(msg, 160, "%s: attivita' %d, sicurezza %d, pressione %d", name, activity, security, crime);
	DB_AdminLog(gPlayer[playerid][pAccountId], 0, "ldset", msg);
	Util_Send(playerid, -5963521, msg);
	if (!District_SeenOp(g_DistAdminOp[playerid]))
	{
		LD_Emit(9, districtId, gPlayer[playerid][pCharId], activity, g_DistAdminOp[playerid]);
	}
	g_DistCycle = 1;
	DB_LivingLoadDistricts();
	return 1;
}
