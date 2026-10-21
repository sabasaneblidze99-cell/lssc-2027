
bool:Admin_ParseTarget(playerid, const token[], &targetid)
{
	if (!Util_ParsePlayerId(token, targetid) || !Util_IsPlaying(targetid))
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return false;
	}
	return true;
}

public Cmd_goto(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /goto id");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tok, targetid))
	{
		return 1;
	}
	if (targetid == playerid)
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return 1;
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(targetid, x, y, z);
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));
	SetPlayerPos(playerid, (1.0 + x), y, z);
	new an[75], tn[75], msg[384];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(playerid, -5963521, "გადაადგილდი %s-თან.", tn);
	format(msg, 384, "%s /goto %s", an, tn);
	Log_Write("ADMIN", msg);
	DB_AdminLog(gPlayer[playerid][pAccountId], gPlayer[targetid][pAccountId], "goto", "");
	Admin_AdmCmd(an, "raggiunto", tn, "", "");
	return 1;
}

public Cmd_setlevel(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[36], tlv[36], targetid = 0, level = 0;
	Util_NextToken(params, pos, tid, 36);
	Util_NextToken(params, pos, tlv, 36);
	if (tid[0] == 0 || tlv[0] == 0 || !Util_ParseInt(tlv, level))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /setlevel id დონე");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (level < 1 || level > 50)
	{
		SendClientMessage(playerid, -10270721, "არასწორი დონე (%d-%d).", 1, 50);
		return 1;
	}
	if (!Level_Set(targetid, level))
	{
		return 1;
	}
	Char_Save(targetid);
	new an[75], tn[75], msg[384];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(playerid, -5963521, "%s-ის დონე დაყენდა: %d.", tn, level);
	if (targetid != playerid)
	{
		SendClientMessage(targetid, -2994689, "ადმინისტრაციამ შენი დონე დააყენა: %d.", level);
	}
	format(msg, 384, "%s /setlevel %s %d", an, tn, level);
	Log_Write("ADMIN", msg);
	format(msg, 384, "დონე %d", level);
	DB_AdminLog(gPlayer[playerid][pAccountId], gPlayer[targetid][pAccountId], "setlevel", msg);
	Admin_AdmCmd(an, "შეცვალა დონე:", tn, msg, "");
	return 1;
}

public Cmd_setmoney(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[36], tam[36], targetid = 0, amount = 0;
	Util_NextToken(params, pos, tid, 36);
	Util_NextToken(params, pos, tam, 36);
	if (tid[0] == 0 || tam[0] == 0 || !Money_Parse(tam, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /setmoney id თანხა");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (!Money_Set(targetid, amount))
	{
		SendClientMessage(playerid, -10270721, "არასწორი თანხა (₾0-%s).", Money_Str(ECON_MONEY_MAX));
		return 1;
	}
	Char_Save(targetid);
	new an[75], tn[75], msg[384];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(playerid, -5963521, "%s-ის ნაღდი ფული დაყენდა: %s.", tn, Money_Str(amount));
	if (targetid != playerid)
	{
		SendClientMessage(targetid, 0x66DD66FF, "ადმინისტრაციამ შენი ნაღდი ფული დააყენა: %s.", Money_Str(amount));
	}
	format(msg, 384, "%s /setmoney %s %d", an, tn, amount);
	Log_Write("ADMIN", msg);
	format(msg, 384, "ნაღდი %d", amount);
	DB_AdminLog(gPlayer[playerid][pAccountId], gPlayer[targetid][pAccountId], "setmoney", msg);
	Admin_AdmCmd(an, "შეცვალა ნაღდი ფული:", tn, msg, "");
	return 1;
}

bool:Admin_ParseSanctionTarget(playerid, const token[], &targetid)
{
	if (!Admin_ParseTarget(playerid, token, targetid))
	{
		return false;
	}
	if (targetid == playerid)
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return false;
	}
	if (!Admin_CanTarget(playerid, targetid, false))
	{
		Util_Send(playerid, -10270721, "ამ მოთამაშეზე მოქმედება არ შეგიძლია.");
		return false;
	}
	return true;
}

public Cmd_ah(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	Admin_SendHelp(playerid);
	return 1;
}

public Cmd_a(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /a ტექსტი");
		return 1;
	}
	new name[75], lvl[72], msg[256];
	GetPlayerName(playerid, name, 75);
	Admin_LevelName(Util_GetAdmin(playerid), lvl, 72);
	format(msg, 256, "[STAFF] %s (%s): %s", name, lvl, text);
	Admin_Notify(msg, 0x99CCFFFF);
	return 1;
}

public Cmd_reports(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	Admin_ListReports(playerid);
	return 1;
}

public Cmd_rr(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 1)
	{
		return 0;
	}
	new pos = 0, tid[36], reportId = 0, text[129];
	Util_NextToken(params, pos, tid, 36);
	if (tid[0] == 0 || !Util_ParseInt(tid, reportId) || !Util_SanitizeText(params[pos], text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /rr რეპორტის_id ტექსტი");
		return 1;
	}
	switch (Admin_ReplyReport(playerid, reportId, text))
	{
		case 0:
		{
			Util_Send(playerid, -10270721, "რეპორტი ვერ მოიძებნა.");
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "რეპორტის ავტორი აღარ არის ონლაინ.");
		}
		default:
		{
			Util_Send(playerid, 0x66DD66FF, "პასუხი გაიგზავნა.");
		}
	}
	return 1;
}

public Cmd_gethere(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /gethere id");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	new Float:x, Float:y, Float:z, an[75], tn[75];
	GetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));
	SetPlayerPos(targetid, (1.0 + x), y, z);
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(targetid, -5963521, "%s-მ გადაგაადგილა.", an);
	Admin_AdmCmd(an, "portato qui", tn, "", "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "gethere", "");
	return 1;
}

public Cmd_freeze(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0, reason[195];
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /freeze id [მიზეზი]");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	Admin_ParseReason(params, pos, reason, 195);
	if (strlen(reason) < 3)
	{
		reason[0] = 0;
	}
	gPlayer[targetid][pFrozen] = 1;
	TogglePlayerControllable(targetid, false);
	new an[75], tn[75];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(targetid, -10270721, "%s-მ გაგყინა%s%s.", an, reason[0] != 0 ? ": " : "", reason);
	Admin_AdmCmd(an, "congelato", tn, reason, "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "freeze", reason);
	return 1;
}

public Cmd_unfreeze(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /unfreeze id");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	gPlayer[targetid][pFrozen] = 0;
	Faction_UpdateControl(targetid);
	new an[75], tn[75];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(targetid, 0x66DD66FF, "%s-მ გაგყინვა მოგიხსნა.", an);
	Admin_AdmCmd(an, "scongelato", tn, "", "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "unfreeze", "");
	return 1;
}

public Cmd_sethp(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tid[36], thp[36], targetid = 0, hp = 0;
	Util_NextToken(params, pos, tid, 36);
	Util_NextToken(params, pos, thp, 36);
	if (tid[0] == 0 || thp[0] == 0 || !Util_ParseInt(thp, hp))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /sethp id %d-%d", 1, 100);
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (!Admin_CanTarget(playerid, targetid, true))
	{
		Util_Send(playerid, -10270721, "ამ მოთამაშეზე მოქმედება არ შეგიძლია.");
		return 1;
	}
	if (hp < 1 || hp > 100)
	{
		SendClientMessage(playerid, -10270721, "არასწორი სიცოცხლე (%d-%d).", 1, 100);
		return 1;
	}
	SetPlayerHealth(targetid, float(hp));
	new an[75], tn[75], msg[384];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	format(msg, 384, "სიცოცხლე %d", hp);
	Admin_AdmCmd(an, "შეცვალა სიცოცხლე:", tn, msg, "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "sethp", msg);
	return 1;
}

public Cmd_kick(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0, reason[195];
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0 || !Admin_ParseReason(params, pos, reason, 195))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /kick id მიზეზი (მიზეზი მინიმუმ %d სიმბოლო)", 3);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	new an[75], tn[75], msg[768];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	Admin_AdmCmd(an, "kickato", tn, reason, "kickato");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "kick", reason);
	format(msg, 768, "%s-მ სერვერიდან გაგაგდო: %s", an, reason);
	Account_Kick(targetid, msg);
	return 1;
}

public Cmd_mute(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tid[36], tmin[36], targetid = 0, minutes = 10, reason[195];
	Util_NextToken(params, pos, tid, 36);
	new afterId = pos;
	Util_NextToken(params, pos, tmin, 36);
	new value = 0;
	if (tmin[0] != 0 && Util_ParseInt(tmin, value))
	{
		minutes = value;
	}
	else
	{
		pos = afterId;
	}
	if (tid[0] == 0 || !Admin_ParseReason(params, pos, reason, 195))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /mute id [წუთი 1-%d] მიზეზი", 1440);
		return 1;
	}
	if (minutes < 1 || minutes > 1440)
	{
		SendClientMessage(playerid, -10270721, "არასწორი ხანგრძლივობა (1-%d წუთი).", 1440);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tid, targetid))
	{
		return 1;
	}
	gPlayer[targetid][pMutedUntil] = gettime() + minutes * 60;
	DB_SetMute(gPlayer[targetid][pAccountId], gPlayer[targetid][pMutedUntil]);
	new an[75], tn[75], msg[768];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	format(msg, 768, "%s (%d წთ)", reason, minutes);
	SendClientMessage(targetid, -10270721, "%s-მ მიუტი დაგადო %d წუთით: %s", an, minutes, reason);
	Admin_AdmCmd(an, "mutato", tn, msg, "mutato");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "mute", msg);
	return 1;
}

public Cmd_unmute(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /unmute id");
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	if (!Admin_IsMuted(targetid))
	{
		Util_Send(playerid, -10270721, "მოთამაშეს მიუტი არ აქვს.");
		return 1;
	}
	gPlayer[targetid][pMutedUntil] = 0;
	DB_SetMute(gPlayer[targetid][pAccountId], 0);
	new an[75], tn[75];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(targetid, 0x66DD66FF, "%s-მ მიუტი მოგიხსნა.", an);
	Admin_AdmCmd(an, "smutato", tn, "", "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "unmute", "");
	return 1;
}

public Cmd_warn(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	new pos = 0, tok[36], targetid = 0, reason[195];
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0 || !Admin_ParseReason(params, pos, reason, 195))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /warn id მიზეზი (მიზეზი მინიმუმ %d სიმბოლო)", 3);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tok, targetid))
	{
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "წინა ოპერაცია ჯერ კიდევ მიმდინარეობს, მოიცადე.");
		return 1;
	}
	new tn[75];
	GetPlayerName(targetid, tn, 75);
	Admin_PendSet(playerid, 1, gPlayer[targetid][pAccountId], tn, 0, reason);
	DB_AddWarn(playerid, gPlayer[targetid][pAccountId], gPlayer[playerid][pAccountId], reason);
	return 1;
}

public Cmd_setadmin(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, who[75], tlv[36], level = 0;
	Util_NextToken(params, pos, who, 75);
	Util_NextToken(params, pos, tlv, 36);
	if (who[0] == 0 || tlv[0] == 0 || !Util_ParseInt(tlv, level))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /setadmin id|ანგარიშის_სახელი %d-%d", 0, 3);
		return 1;
	}
	if (level < 0 || level > 3)
	{
		SendClientMessage(playerid, -10270721, "არასწორი დონე (%d-%d).", 0, 3);
		return 1;
	}
	new targetid = 65535, an[75], tn[75], msg[384];
	GetPlayerName(playerid, an, 75);
	if (Util_ParsePlayerId(who, targetid))
	{
		if (!Util_IsPlaying(targetid))
		{
			Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
			return 1;
		}
	}
	else
	{
		targetid = Admin_FindByName(who);
	}
	if (targetid != INVALID_PLAYER_ID && gPlayer[targetid][pLoggedIn])
	{
		if (!Admin_CanTarget(playerid, targetid, false))
		{
			Util_Send(playerid, -10270721, "ამ მოთამაშეზე მოქმედება არ შეგიძლია.");
			return 1;
		}
		GetPlayerName(targetid, tn, 75);
		gPlayer[targetid][pAdminLevel] = level;
		DB_SetAdminLevel(gPlayer[targetid][pAccountId], level);
		SendClientMessage(targetid, -5963521, "შენი ადმინ-დონე დაყენდა: %d (%s).", level, an);
		format(msg, 384, "დონე %d", level);
		Admin_AdmCmd(an, "შეცვალა ადმინ-დონე:", tn, msg, "");
		Admin_Log(playerid, gPlayer[targetid][pAccountId], "setadmin", msg);
		return 1;
	}
	if (strlen(who) < 2 || (who[0] >= 48 && who[0] <= 57))
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "წინა ოპერაცია ჯერ კიდევ მიმდინარეობს, მოიცადე.");
		return 1;
	}
	Admin_PendSet(playerid, 4, 0, who, level, "");
	DB_FindAccountByName(playerid, who, 4);
	return 1;
}

public Cmd_givemoney(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[36], tam[36], targetid = 0, amount = 0;
	Util_NextToken(params, pos, tid, 36);
	Util_NextToken(params, pos, tam, 36);
	if (tid[0] == 0 || tam[0] == 0 || !Money_Parse(tam, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /givemoney id თანხა");
		return 1;
	}
	if (!Admin_ParseTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (amount < 1 || amount > ECON_ADMIN_GIVE_MAX)
	{
		SendClientMessage(playerid, -10270721, "არასწორი თანხა (%s-%s).", Money_Str(1), Money_Str(ECON_ADMIN_GIVE_MAX));
		return 1;
	}
	new now = GetTickCount(), elapsed = now - gPlayer[playerid][pLastGiveMoneyTick];
	if (elapsed < 10000)
	{
		SendClientMessage(playerid, -10270721, "მოიცადე კიდევ %d წამი.", (10000 - elapsed + 1000 - 1) / 1000);
		return 1;
	}
	if (!Money_Give(targetid, amount))
	{
		Util_Send(playerid, -10270721, "მიმღები ამ თანხას ვერ მიიღებს.");
		return 1;
	}
	gPlayer[playerid][pLastGiveMoneyTick] = now;
	Char_Save(targetid);
	new an[75], tn[75], msg[384];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(targetid, tn, 75);
	SendClientMessage(targetid, 0x66DD66FF, "ადმინისტრაციისგან მიიღე %s.", Money_Str(amount));
	format(msg, 384, "%s", Money_Str(amount));
	Admin_AdmCmd(an, "ნაღდი ფული მისცა:", tn, msg, "");
	Admin_Log(playerid, gPlayer[targetid][pAccountId], "givemoney", msg);
	return 1;
}

public Cmd_ban(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, tid[36], tmin[36], targetid = 0, minutes = 0, reason[195];
	Util_NextToken(params, pos, tid, 36);
	Util_NextToken(params, pos, tmin, 36);
	if (tid[0] == 0 || tmin[0] == 0 || !Util_ParseInt(tmin, minutes) || !Admin_ParseReason(params, pos, reason, 195))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /ban id წუთი(0=სამუდამოდ, მაქს. %d) მიზეზი", 525600);
		return 1;
	}
	if (minutes < 0 || minutes > 525600)
	{
		SendClientMessage(playerid, -10270721, "არასწორი ხანგრძლივობა (0-%d წუთი).", 525600);
		return 1;
	}
	if (!Admin_ParseSanctionTarget(playerid, tid, targetid))
	{
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "წინა ოპერაცია ჯერ კიდევ მიმდინარეობს, მოიცადე.");
		return 1;
	}
	new tn[75], ip[48];
	GetPlayerName(targetid, tn, 75);
	GetPlayerIp(targetid, ip, 48);
	Admin_PendSet(playerid, 2, gPlayer[targetid][pAccountId], tn, minutes, reason);
	DB_AddBan(playerid, gPlayer[targetid][pAccountId], tn, ip, gPlayer[playerid][pAccountId], reason, minutes);
	return 1;
}

public Cmd_unban(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, name[75];
	Util_NextToken(params, pos, name, 75);
	if (name[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /unban ანგარიშის_სახელი");
		return 1;
	}
	if (Admin_Busy(playerid))
	{
		Util_Send(playerid, -10270721, "წინა ოპერაცია ჯერ კიდევ მიმდინარეობს, მოიცადე.");
		return 1;
	}
	Admin_PendSet(playerid, 6, 0, name, 0, "");
	DB_FindAccountByName(playerid, name, 6);
	return 1;
}
