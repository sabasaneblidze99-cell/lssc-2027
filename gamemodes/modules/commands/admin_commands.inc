
bool:Admin_ParseTarget(playerid, token[], &targetid)
{
	if (!Util_ParsePlayerId(token, targetid) || !Util_IsPlaying(targetid))
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 0;
	}
	return 1;
}

public Cmd_goto(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /goto id");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tok, targetid))
	{
		return 1;
	}
	if (targetid == playerid)
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 1;
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(targetid, x, y, z);
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));
	SetPlayerPos(playerid, (1.0 + x), y, z);
	new an[25], tn[25], msg[128];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(playerid, -5963521, "Teletrasportato presso %s.", tn);
	format(msg, 128, "%s /goto %s", an, tn);
	Log_Write("ADMIN", msg);
	DB_AdminLog(gPlayer[playerid][pAccountId], gPlayer[targetid][pAccountId], "goto", "");
	Admin_AdmCmd(an, "raggiunto", tn, "", "");
	return 1;
}

public Cmd_setlevel(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[12], tlv[12], targetid = 0, level = 0;
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, tlv, 12);
	if (tid[0] == 0 || tlv[0] == 0 || !Util_ParseInt(tlv, level))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /setlevel id livello");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (level < 1 || level > 50)
	{
		SendClientMessage(playerid, -10270721, "Livello non valido (%d-%d).", 1, 50);
		return 1;
	}
	if (!Level_Set(targetid, level))
	{
		return 1;
	}
	Char_Save(targetid);
	new an[25], tn[25], msg[128];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(playerid, -5963521, "Livello di %s impostato a %d.", tn, level);
	if (targetid != playerid)
	{
		SendClientMessage(targetid, -2994689, "Il tuo livello e' stato impostato a %d dallo staff.", level);
	}
	format(msg, 128, "%s /setlevel %s %d", an, tn, level);
	Log_Write("ADMIN", msg);
	format(msg, 128, "livello %d", level);
	DB_AdminLog(gPlayer[playerid][pAccountId], gPlayer[targetid][pAccountId], "setlevel", msg);
	Admin_AdmCmd(an, "impostato il livello di", tn, msg, "");
	return 1;
}

public Cmd_setmoney(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[12], tam[12], targetid = 0, amount = 0;
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, tam, 12);
	if (tid[0] == 0 || tam[0] == 0 || !Util_ParseInt(tam, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /setmoney id importo");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (!Money_Set(targetid, amount))
	{
		SendClientMessage(playerid, -10270721, "Importo non valido (0-%d).", 0x3B9ACA00);
		return 1;
	}
	Char_Save(targetid);
	new an[25], tn[25], msg[128];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(playerid, -5963521, "Contanti di %s impostati a $%d.", tn, amount);
	if (targetid != playerid)
	{
		SendClientMessage(targetid, 0x66DD66FF, "I tuoi contanti sono stati impostati a $%d dallo staff.", amount);
	}
	format(msg, 128, "%s /setmoney %s %d", an, tn, amount);
	Log_Write("ADMIN", msg);
	format(msg, 128, "contanti %d", amount);
	DB_AdminLog(gPlayer[playerid][pAccountId], gPlayer[targetid][pAccountId], "setmoney", msg);
	Admin_AdmCmd(an, "impostato i contanti di", tn, msg, "");
	return 1;
}

bool:Admin_ParseSanctionTarget(playerid, token[], &targetid)
{
	if (!Admin_ParseTarget(playerid, token, targetid))
	{
		return 0;
	}
	if (targetid == playerid)
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 0;
	}
	if (!Admin_CanTarget(playerid, targetid, 0))
	{
		Util_Send(playerid, -10270721, "Non puoi agire su questo giocatore.");
		return 0;
	}
	return 1;
}

public Cmd_ah(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	Admin_SendHelp(playerid);
	return 1;
}

public Cmd_a(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /a testo");
		return 1;
	}
	new name[25], lvl[24], msg[256];
	GetPlayerName(playerid, name, 25);
	Admin_LevelName(Util_GetAdmin(playerid), lvl, 24);
	format(msg, 256, "[STAFF] %s (%s): %s", name, lvl, text);
	Admin_Notify(msg, 0x99CCFFFF);
	return 1;
}

public Cmd_reports(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	Admin_ListReports(playerid);
	return 1;
}

public Cmd_rr(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	new pos = 0, tid[12], reportId = 0, text[129];
	Util_NextToken(params, pos, tid, 12);
	if (tid[0] == 0 || !Util_ParseInt(tid, reportId) || !Util_SanitizeText(params[pos], text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /rr id_report testo");
		return 1;
	}
	switch (Admin_ReplyReport(playerid, reportId, text))
	{
		case 0:
		{
			Util_Send(playerid, -10270721, "Report non trovato.");
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "L'autore del report non e' piu' online.");
		}
		default:
		{
			Util_Send(playerid, 0x66DD66FF, "Risposta inviata.");
		}
	}
	return 1;
}

public Cmd_gethere(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /gethere id");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	new Float:x, Float:y, Float:z, an[25], tn[25];
	GetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));
	SetPlayerPos(targetid, (1.0 + x), y, z);
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(targetid, -5963521, "Sei stato teletrasportato da %s.", an);
	Admin_AdmCmd(an, "portato qui", tn, "", "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "gethere", "");
	return 1;
}

public Cmd_freeze(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0, reason[65];
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /freeze id [motivo]");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	Admin_ParseReason(params, pos, reason, 65);
	if (strlen(reason) < 3)
	{
		reason[0] = 0;
	}
	gPlayer[targetid][pFrozen] = 1;
	TogglePlayerControllable(targetid, 0);
	new an[25], tn[25];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(targetid, -10270721, "Sei stato congelato da %s%s%s.", an, reason[0] != 0 ? ": " : "", reason);
	Admin_AdmCmd(an, "congelato", tn, reason, "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "freeze", reason);
	return 1;
}

public Cmd_unfreeze(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /unfreeze id");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	gPlayer[targetid][pFrozen] = 0;
	Faction_UpdateControl(targetid);
	new an[25], tn[25];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(targetid, 0x66DD66FF, "Sei stato scongelato da %s.", an);
	Admin_AdmCmd(an, "scongelato", tn, "", "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "unfreeze", "");
	return 1;
}

public Cmd_sethp(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tid[12], thp[12], targetid = 0, hp = 0;
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, thp, 12);
	if (tid[0] == 0 || thp[0] == 0 || !Util_ParseInt(thp, hp))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /sethp id %d-%d", 1, 100);
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (!Admin_CanTarget(playerid, targetid, 1))
	{
		Util_Send(playerid, -10270721, "Non puoi agire su questo giocatore.");
		return 1;
	}
	if (hp < 1 || hp > 100)
	{
		SendClientMessage(playerid, -10270721, "Vita non valida (%d-%d).", 1, 100);
		return 1;
	}
	SetPlayerHealth(targetid, float(hp));
	new an[25], tn[25], msg[128];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	format(msg, 128, "vita %d", hp);
	Admin_AdmCmd(an, "impostato la vita di", tn, msg, "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "sethp", msg);
	return 1;
}

public Cmd_kick(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0, reason[65];
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0 || !Admin_ParseReason(params, pos, reason, 65))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /kick id motivo (motivo di almeno %d caratteri)", 3);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	new an[25], tn[25], msg[256];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	Admin_AdmCmd(an, "kickato", tn, reason, "kickato");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "kick", reason);
	format(msg, 256, "Sei stato espulso da %s: %s", an, reason);
	Account_Kick(targetid, msg);
	return 1;
}

public Cmd_mute(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tid[12], tmin[12], targetid = 0, minutes = 10, reason[65];
	Util_NextToken(params, pos, tid, 12);
	new afterId = pos;
	Util_NextToken(params, pos, tmin, 12);
	new value = 0;
	if (tmin[0] != 0 && Util_ParseInt(tmin, value))
	{
		minutes = value;
	}
	else
	{
		pos = afterId;
	}
	if (tid[0] == 0 || !Admin_ParseReason(params, pos, reason, 65))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /mute id [minuti 1-%d] motivo", 1440);
		return 1;
	}
	if (minutes < 1 || minutes > 1440)
	{
		SendClientMessage(playerid, -10270721, "Durata non valida (1-%d minuti).", 1440);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tid, targetid))
	{
		return 1;
	}
	gPlayer[targetid][pMutedUntil] = gettime() + minutes * 60;
	DB_SetMute(gPlayer[targetid][pAccountId], gPlayer[targetid][pMutedUntil]);
	new an[25], tn[25], msg[256];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	format(msg, 256, "%s (%d min)", reason, minutes);
	SendClientMessage(targetid, -10270721, "Sei stato mutato da %s per %d minuti: %s", an, minutes, reason);
	Admin_AdmCmd(an, "mutato", tn, msg, "mutato");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "mute", msg);
	return 1;
}

public Cmd_unmute(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /unmute id");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	if (!Admin_IsMuted(targetid))
	{
		Util_Send(playerid, -10270721, "Il giocatore non e' mutato.");
		return 1;
	}
	gPlayer[targetid][pMutedUntil] = 0;
	DB_SetMute(gPlayer[targetid][pAccountId], 0);
	new an[25], tn[25];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(targetid, 0x66DD66FF, "Il tuo mute e' stato rimosso da %s.", an);
	Admin_AdmCmd(an, "smutato", tn, "", "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "unmute", "");
	return 1;
}

public Cmd_warn(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[12], targetid = 0, reason[65];
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0 || !Admin_ParseReason(params, pos, reason, 65))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /warn id motivo (motivo di almeno %d caratteri)", 3);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "Operazione precedente ancora in corso, attendi.");
		return 1;
	}
	new tn[25];
	GetPlayerName(targetid, tn, 25);
	Admin_PendSet(playerid, 1, gPlayer[targetid][pAccountId], tn, 0, reason);
	DB_AddWarn(playerid, gPlayer[targetid][pAccountId], gPlayer[playerid][pAccountId], reason);
	return 1;
}

public Cmd_setadmin(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, who[25], tlv[12], level = 0;
	Util_NextToken(params, pos, who, 25);
	Util_NextToken(params, pos, tlv, 12);
	if (who[0] == 0 || tlv[0] == 0 || !Util_ParseInt(tlv, level))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /setadmin id|nome_account %d-%d", 0, 3);
		return 1;
	}
	if (level < 0 || level > 3)
	{
		SendClientMessage(playerid, -10270721, "Livello non valido (%d-%d).", 0, 3);
		return 1;
	}
	new targetid = 65535, an[25], tn[25], msg[128];
	GetPlayerName(playerid, an, 25);
	if (Util_ParsePlayerId(who, targetid))
	{
		if (!Util_IsPlaying(targetid))
		{
			Util_Send(playerid, -10270721, "Giocatore non valido.");
			return 1;
		}
	}
	else
	{
		targetid = Admin_FindByName(who);
	}
	if (targetid != INVALID_PLAYER_ID && gPlayer[targetid][pLoggedIn])
	{
		if (!Admin_CanTarget(playerid, targetid, 0))
		{
			Util_Send(playerid, -10270721, "Non puoi agire su questo giocatore.");
			return 1;
		}
		GetPlayerName(targetid, tn, 25);
		gPlayer[targetid][pAdminLevel] = level;
		DB_SetAdminLevel(gPlayer[targetid][pAccountId], level);
		SendClientMessage(targetid, -5963521, "Il tuo livello staff e' stato impostato a %d da %s.", level, an);
		format(msg, 128, "livello %d", level);
		Admin_AdmCmd(an, "impostato il livello staff di", tn, msg, "");
		Admin_Log(playerid, gPlayer[targetid][pAccountId], "setadmin", msg);
		return 1;
	}
	if (strlen(who) < 2 || (who[0] >= 48 && who[0] <= 57))
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "Operazione precedente ancora in corso, attendi.");
		return 1;
	}
	Admin_PendSet(playerid, 4, 0, who, level, "");
	DB_FindAccountByName(playerid, who, 4);
	return 1;
}

public Cmd_givemoney(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[12], tam[12], targetid = 0, amount = 0;
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, tam, 12);
	if (tid[0] == 0 || tam[0] == 0 || !Util_ParseInt(tam, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /givemoney id importo");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (amount < 1 || amount > 100000)
	{
		SendClientMessage(playerid, -10270721, "Importo non valido (1-%d).", 100000);
		return 1;
	}
	new now = GetTickCount(), elapsed = now - gPlayer[playerid][pLastGiveMoneyTick];
	if (elapsed < 10000)
	{
		SendClientMessage(playerid, -10270721, "Attendi ancora %d secondi.", (10000 - elapsed + 1000 - 1) / 1000);
		return 1;
	}
	if (!Money_Give(targetid, amount))
	{
		Util_Send(playerid, -10270721, "Il destinatario non puo' ricevere questa somma.");
		return 1;
	}
	gPlayer[playerid][pLastGiveMoneyTick] = now;
	Char_Save(targetid);
	new an[25], tn[25], msg[128];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(targetid, 0x66DD66FF, "Hai ricevuto $%d dallo staff.", amount);
	format(msg, 128, "$%d", amount);
	Admin_AdmCmd(an, "dato contanti a", tn, msg, "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "givemoney", msg);
	return 1;
}

public Cmd_ban(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[12], tmin[12], targetid = 0, minutes = 0, reason[65];
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, tmin, 12);
	if (tid[0] == 0 || tmin[0] == 0 || !Util_ParseInt(tmin, minutes) || !Admin_ParseReason(params, pos, reason, 65))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /ban id minuti(0=permanente, max %d) motivo", 525600);
		return 1;
	}
	if (minutes < 0 || minutes > 525600)
	{
		SendClientMessage(playerid, -10270721, "Durata non valida (0-%d minuti).", 525600);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "Operazione precedente ancora in corso, attendi.");
		return 1;
	}
	new tn[25], ip[16];
	GetPlayerName(targetid, tn, 25);
	GetPlayerIp(targetid, ip, 16);
	Admin_PendSet(playerid, 2, gPlayer[targetid][pAccountId], tn, minutes, reason);
	DB_AddBan(playerid, gPlayer[targetid][pAccountId], tn, ip, gPlayer[playerid][pAccountId], reason, minutes);
	return 1;
}

public Cmd_unban(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, name[25];
	Util_NextToken(params, pos, name, 25);
	if (name[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /unban nome_account");
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "Operazione precedente ancora in corso, attendi.");
		return 1;
	}
	Admin_PendSet(playerid, 6, 0, name, 0, "");
	DB_FindAccountByName(playerid, name, 6);
	return 1;
}
