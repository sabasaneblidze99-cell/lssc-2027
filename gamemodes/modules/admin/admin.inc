new g_AdmPend[1001][E_ADM_PEND];
new g_RepId[10];
new g_RepAuthor[10][25];
new g_RepText[10][97];
new g_RepSeq;
new g_AdmHelpLevel[19] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3};
new g_AdmHelpText[19][32] = {
	"/a testo",
	"/ah",
	"/goto id",
	"/reports",
	"/rr id testo",
	"/gethere id",
	"/freeze id [motivo]",
	"/unfreeze id",
	"/sethp id hp",
	"/kick id motivo",
	"/mute id [min] motivo",
	"/unmute id",
	"/warn id motivo",
	"/setadmin id|nome 0-3",
	"/setlevel id livello",
	"/setmoney id importo",
	"/givemoney id importo",
	"/ban id min motivo",
	"/unban nome_account"
};

Admin_LevelName(level, name[], size)
{
	switch (level)
	{
		case 1:
		{
			strmid(name, "Helper", 0, strlen("Helper"), size);
		}
		case 2:
		{
			strmid(name, "Moderatore", 0, strlen("Moderatore"), size);
		}
		case 3:
		{
			strmid(name, "Admin", 0, strlen("Admin"), size);
		}
		default:
		{
			name[0] = 0;
		}
	}
}

bool:Admin_CanTarget(staffid, targetid, bool:allowSelf)
{
	if (staffid == targetid)
	{
		return allowSelf;
	}
	return Util_GetAdmin(targetid) < Util_GetAdmin(staffid);
}

Admin_ActorName(playerid, name[], size)
{
	if (playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid))
	{
		GetPlayerName(playerid, name, size);
	}
	else
	{
		strmid(name, "Console", 0, strlen("Console"), size);
	}
}

Admin_ActorAccount(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	return gPlayer[playerid][pAccountId];
}

Admin_Reply(playerid, color, text[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	Util_Send(playerid, color, text);
}

Admin_Notify(text[], color)
{
	new lines[3][128];
	new count = Util_SplitText(text, lines, 3);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i) || Util_GetAdmin(i) < 1)
		{
			continue;
		}
		for (new l = 0; l < count; l++)
		{
			Util_Send(i, color, lines[l]);
		}
	}
}

Admin_PublicNotice(text[])
{
	new lines[3][128];
	new count = Util_SplitText(text, lines, 3);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i) || !gPlayer[i][pLoggedIn] || Util_GetAdmin(i) >= 1)
		{
			continue;
		}
		for (new l = 0; l < count; l++)
		{
			Util_Send(i, -5963521, lines[l]);
		}
	}
}

Admin_AdmCmd(staffName[], action[], targetName[], reason[], participle[])
{
	new msg[256];
	if (reason[0] != 0)
	{
		format(msg, 256, "AdmCmd: Staff %s ha %s %s: %s", staffName, action, targetName, reason);
	}
	else
	{
		format(msg, 256, "AdmCmd: Staff %s ha %s %s", staffName, action, targetName);
	}
	Admin_Notify(msg, -5963521);
	if (participle[0] != 0)
	{
		if (reason[0] != 0)
		{
			format(msg, 256, "AdmCmd: %s e' stato %s da %s: %s", targetName, participle, staffName, reason);
		}
		else
		{
			format(msg, 256, "AdmCmd: %s e' stato %s da %s", targetName, participle, staffName);
		}
		Admin_PublicNotice(msg);
	}
}

Admin_LogRaw(staffAcc, staffName[], targetAcc, action[], reason[])
{
	new line[320];
	DB_AdminLog(staffAcc, targetAcc, action, reason);
	format(line, 320, "%s (acc %d) %s -> acc %d: %s", staffName, staffAcc, action, targetAcc, reason);
	Log_Write("ADMIN", line);
}

Admin_Log(staffid, targetAcc, action[], reason[])
{
	new name[25];
	Admin_ActorName(staffid, name, 25);
	Admin_LogRaw(Admin_ActorAccount(staffid), name, targetAcc, action, reason);
}

bool:Admin_ParseReason(params[], pos, reason[], size)
{
	while (params[pos] == 32)
	{
		pos++;
	}
	reason[0] = 0;
	if (!Util_SanitizeText(params[pos], reason, min(size - 1, 64)))
	{
		return 0;
	}
	for (new i = 0; reason[i] != 0; i++)
	{
		if (reason[i] == 37)
		{
			reason[i] = 35;
		}
	}
	return strlen(reason) >= 3;
}

Admin_FindByAccount(accountId)
{
	if (accountId <= 0)
	{
		return 65535;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && gPlayer[i][pLoggedIn] && gPlayer[i][pAccountId] == accountId)
		{
			return i;
		}
	}
	return 65535;
}

Admin_FindByName(name[])
{
	new cur[25];
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i))
		{
			continue;
		}
		GetPlayerName(i, cur, 25);
		if (!strcmp(cur, name, 1, 0x7FFFFFFF))
		{
			return i;
		}
	}
	return 65535;
}

Admin_FormatDuration(seconds, dest[], size)
{
	new minutes = (seconds + 60 - 1) / 60;
	if (minutes < 1)
	{
		minutes = 1;
	}
	if (minutes >= 1440)
	{
		format(dest, size, "%d giorni", (minutes + 1440 - 1) / 1440);
	}
	else if (minutes >= 60)
	{
		format(dest, size, "%d ore", (minutes + 60 - 1) / 60);
	}
	else
	{
		format(dest, size, "%d minuti", minutes);
	}
}

Admin_Notify_One(playerid, text[])
{
	new lines[3][128];
	new count = Util_SplitText(text, lines, 3);
	for (new l = 0; l < count; l++)
	{
		Util_Send(playerid, -5963521, lines[l]);
	}
}

Admin_SendHelp(playerid)
{
	new level = Util_GetAdmin(playerid);
	if (level < 1)
	{
		return 0;
	}
	SendClientMessage(playerid, -2994689, "--- Comandi staff ---");
	new line[256], name[25];
	for (new lv = 1; lv <= level && lv <= 3; lv++)
	{
		line[0] = 0;
		for (new i = 0; i < 19; i++)
		{
			if (g_AdmHelpLevel[i] != lv)
			{
				continue;
			}
			if (line[0] != 0)
			{
				strcat(line, " · ", 256);
			}
			strcat(line, g_AdmHelpText[i], 256);
		}
		if (line[0] == 0)
		{
			continue;
		}
		Admin_LevelName(lv, name, 25);
		new out[256];
		format(out, 256, "[%s] %s", name, line);
		Admin_Notify_One(playerid, out);
	}
}

bool:Admin_IsMuted(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	new until = gPlayer[playerid][pMutedUntil];
	if (until == 0)
	{
		return 0;
	}
	if (until > gettime())
	{
		return 1;
	}
	gPlayer[playerid][pMutedUntil] = 0;
	if (gPlayer[playerid][pAccountId] > 0)
	{
		DB_SetMute(gPlayer[playerid][pAccountId], 0);
	}
	return 0;
}

bool:Admin_CheckMuted(playerid)
{
	if (!Admin_IsMuted(playerid))
	{
		return 0;
	}
	new left[24];
	Admin_FormatDuration(gPlayer[playerid][pMutedUntil] - gettime(), left, 24);
	SendClientMessage(playerid, -10270721, "Sei mutato ancora %s.", left);
	return 1;
}

Admin_OnText(playerid)
{
	return Admin_CheckMuted(playerid) ? 0 : 1;
}

Admin_PendSet(requesterid, op, targetAcc, targetName[], val, reason[])
{
	new slot = DB_ReqSlot(requesterid);
	g_AdmPend[slot][apActive] = 1;
	g_AdmPend[slot][apOp] = op;
	g_AdmPend[slot][apTargetAcc] = targetAcc;
	g_AdmPend[slot][apStaffAcc] = Admin_ActorAccount(requesterid);
	g_AdmPend[slot][apValue] = val;
	strmid(g_AdmPend[slot][apTargetName], targetName, 0, strlen(targetName), 25);
	Admin_ActorName(requesterid, g_AdmPend[slot][apStaffName], 25);
	strmid(g_AdmPend[slot][apReason], reason, 0, strlen(reason), 65);
}

bool:Admin_Busy(requesterid)
{
	return g_DBAdminPending[DB_ReqSlot(requesterid)] != 0;
}

Admin_PendTake(op, accountKey, nameKey[])
{
	for (new i = 0; i <= 1000; i++)
	{
		if (!g_AdmPend[i][apActive] || g_AdmPend[i][apOp] != op)
		{
			continue;
		}
		if (accountKey > 0 && g_AdmPend[i][apTargetAcc] != accountKey)
		{
			continue;
		}
		if (accountKey <= 0 && nameKey[0] != 0 && strcmp(g_AdmPend[i][apTargetName], nameKey, 1, 0x7FFFFFFF) != 0)
		{
			continue;
		}
		g_AdmPend[i][apActive] = 0;
		return i;
	}
	return -1;
}

Admin_ApplyLevel(accountId, level)
{
	DB_SetAdminLevel(accountId, level);
	new online = Admin_FindByAccount(accountId);
	if (online == 65535)
	{
		for (new i = 0; i < MAX_PLAYERS; i++)
		{
			if (IsPlayerConnected(i) && gPlayer[i][pAccountId] == accountId)
			{
				gPlayer[i][pAdminLevel] = level;
			}
		}
		return 65535;
	}
	gPlayer[online][pAdminLevel] = level;
	return online;
}

public Admin_OnBanChecked(playerid, bool:banned, reason[], expiresAt)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][pAuthState] != 11)
	{
		return 1;
	}
	if (!banned)
	{
		Account_ContinueConnect(playerid);
		return 1;
	}
	new msg[144], name[25], when[40];
	GetPlayerName(playerid, name, 25);
	if (expiresAt > 0)
	{
		new left = expiresAt - gettime();
		new dur[24];
		Admin_FormatDuration(left, dur, 24);
		format(when, 40, "scade tra %s", dur);
	}
	else
	{
		strmid(when, "permanente", 0, 10, 40);
	}
	format(msg, 144, "Sei bannato (%s). Motivo: %s", when, reason);
	new line[320];
	format(line, 320, "connessione rifiutata (ban) %s", name);
	Log_Write("ADMIN", line);
	Account_Kick(playerid, msg);
	return 1;
}

public Admin_OnWarnAdded(requesterid, targetAccountId, activeCount)
{
	new e = Admin_PendTake(1, targetAccountId, "");
	if (e < 0)
	{
		return 1;
	}
	if (activeCount < 0)
	{
		Admin_Reply(requesterid, -10270721, "Warn non registrato (errore database).");
		return 1;
	}
	new reason[65], tname[25], sname[25], msg[256];
	strmid(reason, g_AdmPend[e][apReason], 0, strlen(g_AdmPend[e][apReason]), 65);
	strmid(tname, g_AdmPend[e][apTargetName], 0, strlen(g_AdmPend[e][apTargetName]), 25);
	strmid(sname, g_AdmPend[e][apStaffName], 0, strlen(g_AdmPend[e][apStaffName]), 25);
	new staffAcc = g_AdmPend[e][apStaffAcc];
	format(msg, 256, "%s (warn attivi %d/%d)", reason, activeCount, 3);
	Admin_AdmCmd(sname, "warnato", tname, msg, "warnato");
	format(msg, 256, "%s (%d/%d)", reason, activeCount, 3);
	Admin_LogRaw(staffAcc, sname, targetAccountId, "warn", msg);
	new target = Admin_FindByAccount(targetAccountId);
	if (target == INVALID_PLAYER_ID)
	{
		return 1;
	}
	SendClientMessage(target, -10270721, "Hai ricevuto un warn da %s: %s (%d/%d).", sname, reason, activeCount, 3);
	if (activeCount >= 3)
	{
		format(msg, 256, "%d warn attivi: kick automatico", activeCount);
		Admin_AdmCmd(sname, "kickato", tname, msg, "kickato");
		Admin_LogRaw(staffAcc, sname, targetAccountId, "kick", msg);
		Account_Kick(target, "Hai raggiunto il numero massimo di warn: sei stato espulso.");
	}
	return 1;
}

public Admin_OnBanAdded(requesterid, name[], banId)
{
	new e = Admin_PendTake(2, 0, name);
	if (e < 0)
	{
		return 1;
	}
	if (banId <= 0)
	{
		Admin_Reply(requesterid, -10270721, "Ban non registrato (errore database).");
		return 1;
	}
	new reason[65], tname[25], sname[25], msg[256], dur[24];
	strmid(reason, g_AdmPend[e][apReason], 0, strlen(g_AdmPend[e][apReason]), 65);
	strmid(tname, g_AdmPend[e][apTargetName], 0, strlen(g_AdmPend[e][apTargetName]), 25);
	strmid(sname, g_AdmPend[e][apStaffName], 0, strlen(g_AdmPend[e][apStaffName]), 25);
	new minutes = g_AdmPend[e][apValue];
	if (minutes > 0)
	{
		format(dur, 24, "%d min", minutes);
	}
	else
	{
		strmid(dur, "permanente", 0, 10, 24);
	}
	format(msg, 256, "%s (%s)", reason, dur);
	Admin_AdmCmd(sname, "bannato", tname, msg, "bannato");
	Admin_LogRaw(g_AdmPend[e][apStaffAcc], sname, g_AdmPend[e][apTargetAcc], "ban", msg);
	new target = Admin_FindByName(tname);
	if (target != INVALID_PLAYER_ID)
	{
		format(msg, 256, "Sei stato bannato da %s (%s). Motivo: %s", sname, dur, reason);
		Account_Kick(target, msg);
	}
	return 1;
}

public Admin_OnUnbanned(requesterid, name[], affected)
{
	new e = Admin_PendTake(3, 0, name);
	if (e < 0)
	{
		return 1;
	}
	if (affected < 0)
	{
		Admin_Reply(requesterid, -10270721, "Unban non riuscito (errore database).");
		return 1;
	}
	if (affected == 0)
	{
		Admin_Reply(requesterid, -10270721, "Nessun ban attivo per questo account.");
		return 1;
	}
	new tname[25], sname[25];
	strmid(tname, g_AdmPend[e][apTargetName], 0, strlen(g_AdmPend[e][apTargetName]), 25);
	strmid(sname, g_AdmPend[e][apStaffName], 0, strlen(g_AdmPend[e][apStaffName]), 25);
	Admin_AdmCmd(sname, "sbannato", tname, "", "");
	Admin_LogRaw(g_AdmPend[e][apStaffAcc], sname, g_AdmPend[e][apTargetAcc], "unban", tname);
	return 1;
}

public Admin_OnAccountFound(requesterid, tag, accountId, name[], adminLevel, lastIp[])
{
	new e = Admin_PendTake(tag, 0, name);
	if (e < 0)
	{
		return 1;
	}
	new tname[25], sname[25], msg[256];
	strmid(sname, g_AdmPend[e][apStaffName], 0, strlen(g_AdmPend[e][apStaffName]), 25);
	new level = g_AdmPend[e][apValue], staffAcc = g_AdmPend[e][apStaffAcc];
	if (accountId > 0)
	{
		strmid(tname, name, 0, strlen(name), 25);
	}
	else
	{
		strmid(tname, g_AdmPend[e][apTargetName], 0, strlen(g_AdmPend[e][apTargetName]), 25);
	}
	if (tag == 5)
	{
		if (accountId == -1)
		{
			print("[ADMIN] setadmin: errore database");
			return 1;
		}
		if (accountId <= 0)
		{
			printf("[ADMIN] setadmin: account '%s' non trovato (deve essere registrato)", tname);
			return 1;
		}
		Admin_ApplyLevel(accountId, level);
		format(msg, 256, "rcon: livello %d", level);
		Admin_LogRaw(0, "Console", accountId, "setadmin", msg);
		printf("[ADMIN] %s impostato a livello staff %d", tname, level);
		new online = Admin_FindByAccount(accountId);
		if (online != 65535)
		{
			SendClientMessage(online, -5963521, "Il tuo livello staff e' stato impostato a %d dalla console.", level);
		}
		return 1;
	}
	if (requesterid == 65535 || !IsPlayerConnected(requesterid) || gPlayer[requesterid][pAccountId] != staffAcc)
	{
		return 1;
	}
	if (accountId == -1)
	{
		Admin_Reply(requesterid, -10270721, "Errore database, riprova.");
		return 1;
	}
	if (accountId <= 0)
	{
		Admin_Reply(requesterid, -10270721, "Account non trovato.");
		return 1;
	}
	new myLevel = Util_GetAdmin(requesterid);
	if (adminLevel >= myLevel || accountId == staffAcc)
	{
		Admin_Reply(requesterid, -10270721, "Non puoi agire su questo giocatore.");
		return 1;
	}
	if (tag == 4)
	{
		if (myLevel < 3)
		{
			return 1;
		}
		Admin_ApplyLevel(accountId, level);
		new online = Admin_FindByAccount(accountId);
		if (online != 65535)
		{
			SendClientMessage(online, -5963521, "Il tuo livello staff e' stato impostato a %d da %s.", level, sname);
		}
		format(msg, 256, "livello %d", level);
		Admin_AdmCmd(sname, "impostato il livello staff di", tname, msg, "");
		Admin_LogRaw(staffAcc, sname, accountId, "setadmin", msg);
	}
	else if (tag == 6)
	{
		if (myLevel < 3)
		{
			return 1;
		}
		Admin_PendSet(requesterid, 3, accountId, tname, 0, "");
		DB_Unban(requesterid, tname);
	}
	return 1;
}

Admin_StoreReport(playerid, text[])
{
	g_RepSeq++;
	new slot = g_RepSeq % 10;
	g_RepId[slot] = g_RepSeq;
	GetPlayerName(playerid, g_RepAuthor[slot], 25);
	strmid(g_RepText[slot], text, 0, strlen(text), 97);
}

Admin_ListReports(playerid)
{
	new shown = 0, msg[256];
	SendClientMessage(playerid, -2994689, "--- Ultimi report ---");
	for (new k = 0; k < 10; k++)
	{
		new id = g_RepSeq - k;
		if (id < 1)
		{
			break;
		}
		new slot = id % 10;
		if (g_RepId[slot] != id)
		{
			continue;
		}
		format(msg, 256, "[#%d] %s: %s", id, g_RepAuthor[slot], g_RepText[slot]);
		Admin_Notify_One(playerid, msg);
		shown++;
	}
	if (shown == 0)
	{
		Util_Send(playerid, -1, "Nessun report.");
	}
}

Admin_ReplyReport(playerid, reportId, text[])
{
	if (reportId < 1 || reportId > g_RepSeq)
	{
		return 0;
	}
	new slot = reportId % 10;
	if (g_RepId[slot] != reportId)
	{
		return 0;
	}
	new target = Admin_FindByName(g_RepAuthor[slot]);
	if (target == INVALID_PLAYER_ID)
	{
		return 2;
	}
	new name[25];
	GetPlayerName(playerid, name, 25);
	SendClientMessage(target, -5963521, "[STAFF] %s: %s", name, text);
	return 1;
}

Admin_OnRconCommand(cmd[])
{
	new pos = 0, word[16], name[25], tlv[12], level = 0;
	Util_NextToken(cmd, pos, word, 16);
	if (strcmp(word, "setadmin", 1, 0x7FFFFFFF) != 0)
	{
		return 0;
	}
	Util_NextToken(cmd, pos, name, 25);
	Util_NextToken(cmd, pos, tlv, 12);
	for (new i = 0; tlv[i] != 0; i++)
	{
		if (tlv[i] < 48 || tlv[i] > 57)
		{
			tlv[i] = 0;
			break;
		}
	}
	if (name[0] == 0 || tlv[0] == 0 || !Util_ParseInt(tlv, level) || level < 0 || level > 3)
	{
		printf("Uso: %s nome_account 0-%d", "setadmin", 3);
		return 1;
	}
	if (!DB_IsReady())
	{
		print("[ADMIN] database non disponibile");
		return 1;
	}
	if (Admin_Busy(65535))
	{
		print("[ADMIN] operazione precedente ancora in corso, riprova");
		return 1;
	}
	Admin_PendSet(65535, 5, 0, name, level, "");
	DB_FindAccountByName(65535, name, 5);
	return 1;
}
