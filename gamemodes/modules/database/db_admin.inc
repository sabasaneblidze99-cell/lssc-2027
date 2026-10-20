
DB_ReqSlot(requesterid)
{
	return requesterid >= 0 && requesterid < 1000 ? requesterid : 1000;
}

DB_ReqName(requesterid, name[], size)
{
	name[0] = 0;
	if (requesterid >= 0 && requesterid < 1000 && IsPlayerConnected(requesterid))
	{
		GetPlayerName(requesterid, name, size);
	}
}

DB_ReqResolve(requesterid, reqName[])
{
	if (requesterid < 0 || requesterid >= 1000 || reqName[0] == 0)
	{
		return 65535;
	}
	if (!DB_PlayerMatches(requesterid, reqName))
	{
		return 65535;
	}
	return requesterid;
}

DB_ReqBegin(requesterid, op)
{
	g_DBAdminPending[DB_ReqSlot(requesterid)] |= op;
}

DB_ReqEnd(requesterid, op)
{
	g_DBAdminPending[DB_ReqSlot(requesterid)] &= ~op;
}

DB_AdminLog(staffAccountId, targetAccountId, action[], reason[])
{
	if (!DB_IsReady() || action[0] == 0)
	{
		return 0;
	}
	new query[1024], target[16];
	if (targetAccountId > 0)
	{
		format(target, 16, "%d", targetAccountId);
	}
	else
	{
		strcat(target, "NULL", 16);
	}
	mysql_format(g_DB, query, 1024, "INSERT INTO admin_log (staff_account_id, target_account_id, action, reason) VALUES (%d, %s, LEFT('%e', %d), LEFT('%e', %d))", max(staffAccountId, 0), target, action, 24, reason, 128);
	mysql_tquery(g_DB, query, "", "");
}

DB_SetMute(accountId, untilTimestamp)
{
	if (!DB_IsReady() || accountId <= 0)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE accounts SET muted_until = %d WHERE id = %d", max(untilTimestamp, 0), accountId);
	mysql_tquery(g_DB, query, "", "");
}

DB_SetAdminLevel(accountId, level)
{
	if (!DB_IsReady() || accountId <= 0 || level < 0 || level > 255)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE accounts SET admin_level = %d WHERE id = %d", level, accountId);
	mysql_tquery(g_DB, query, "", "");
}

DB_AddWarn(requesterid, targetAccountId, staffAccountId, reason[])
{
	if (!DB_IsReady() || targetAccountId <= 0)
	{
		DB_ReqBegin(requesterid, 1);
		DB_DeliverAdminFailure(DB_ReqSlot(requesterid), 1);
		return 0;
	}
	new query[1024], reqName[25];
	DB_ReqName(requesterid, reqName, 25);
	mysql_format(g_DB, query, 1024, "INSERT INTO warns (account_id, staff_account_id, reason) VALUES (%d, %d, LEFT('%e', %d))", targetAccountId, max(staffAccountId, 0), reason, 64);
	DB_ReqBegin(requesterid, 1);
	mysql_tquery(g_DB, query, "DB_OnWarnInserted", "dsd", requesterid, reqName, targetAccountId);
}

public DB_OnWarnInserted(requesterid, reqName[], targetAccountId)
{
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT COUNT(*) AS cnt FROM warns WHERE account_id = %d AND active = 1", targetAccountId);
	mysql_tquery(g_DB, query, "DB_OnWarnCounted", "dsd", requesterid, reqName, targetAccountId);
	return 1;
}

public DB_OnWarnCounted(requesterid, reqName[], targetAccountId)
{
	DB_ReqEnd(requesterid, 1);
	new cnt = 0;
	cache_get_value_name_int(0, "cnt", cnt);
	if (funcidx("Admin_OnWarnAdded") != -1)
	{
		CallLocalFunction("Admin_OnWarnAdded", "ddd", DB_ReqResolve(requesterid, reqName), targetAccountId, cnt);
	}
	return 1;
}

DB_AddBan(requesterid, accountId, name[], ip[], staffAccountId, reason[], minutes)
{
	if (!DB_IsReady() || name[0] == 0)
	{
		DB_ReqBegin(requesterid, 2);
		DB_DeliverAdminFailure(DB_ReqSlot(requesterid), 2);
		return 0;
	}
	new query[1024], reqName[25], accStr[16], expStr[48];
	DB_ReqName(requesterid, reqName, 25);
	if (accountId > 0)
	{
		format(accStr, 16, "%d", accountId);
	}
	else
	{
		strcat(accStr, "NULL", 16);
	}
	if (minutes > 0)
	{
		format(expStr, 48, "UNIX_TIMESTAMP() + %d", min(minutes, 5256000) * 60);
	}
	else
	{
		strcat(expStr, "NULL", 48);
	}
	mysql_format(g_DB, query, 1024, "INSERT INTO bans (account_id, account_name, ip, staff_account_id, reason, expires_at) VALUES (%s, LEFT('%e', 24), LEFT('%e', 15), %d, LEFT('%e', %d), %s)", accStr, name, ip, max(staffAccountId, 0), reason, 64, expStr);
	DB_ReqBegin(requesterid, 2);
	mysql_tquery(g_DB, query, "DB_OnBanInserted", "dss", requesterid, reqName, name);
}

public DB_OnBanInserted(requesterid, reqName[], name[])
{
	DB_ReqEnd(requesterid, 2);
	new banId = 0;
	if (cache_affected_rows() == 1)
	{
		banId = cache_insert_id();
	}
	if (funcidx("Admin_OnBanAdded") != -1)
	{
		CallLocalFunction("Admin_OnBanAdded", "dsd", DB_ReqResolve(requesterid, reqName), name, banId);
	}
	return 1;
}

DB_Unban(requesterid, accountName[])
{
	if (!DB_IsReady() || accountName[0] == 0)
	{
		DB_ReqBegin(requesterid, 4);
		DB_DeliverAdminFailure(DB_ReqSlot(requesterid), 4);
		return 0;
	}
	new query[512], reqName[25];
	DB_ReqName(requesterid, reqName, 25);
	mysql_format(g_DB, query, 512, "UPDATE bans SET active = 0 WHERE active = 1 AND account_name = '%e'", accountName);
	DB_ReqBegin(requesterid, 4);
	mysql_tquery(g_DB, query, "DB_OnUnbanDone", "dss", requesterid, reqName, accountName);
}

public DB_OnUnbanDone(requesterid, reqName[], name[])
{
	DB_ReqEnd(requesterid, 4);
	if (funcidx("Admin_OnUnbanned") != -1)
	{
		CallLocalFunction("Admin_OnUnbanned", "dsd", DB_ReqResolve(requesterid, reqName), name, cache_affected_rows());
	}
	return 1;
}

DB_CheckBan(playerid, name[], ip[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady())
	{
		DB_DeliverFailure(playerid, 16);
		return 0;
	}
	new query[512], pname[25];
	GetPlayerName(playerid, pname, 25);
	mysql_format(g_DB, query, 512, "UPDATE bans SET active = 0 WHERE active = 1 AND expires_at IS NOT NULL AND expires_at <= UNIX_TIMESTAMP() AND account_name = '%e'", name);
	mysql_tquery(g_DB, query, "", "");
	mysql_format(g_DB, query, 512, "SELECT reason, IFNULL(expires_at, 0) AS expires_at FROM bans WHERE active = 1 AND (expires_at IS NULL OR expires_at > UNIX_TIMESTAMP()) AND account_name = '%e' ORDER BY (expires_at IS NULL) DESC, expires_at DESC LIMIT 1", name);
	DB_PendingSet(playerid, 16);
	mysql_tquery(g_DB, query, "DB_OnBanChecked", "ds", playerid, pname);
}

public DB_OnBanChecked(playerid, name[])
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	DB_PendingClear(playerid, 16);
	new rows = 0, reason[65], expiresAt = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name(0, "reason", reason, 65);
		cache_get_value_name_int(0, "expires_at", expiresAt);
	}
	if (funcidx("Admin_OnBanChecked") != -1)
	{
		CallLocalFunction("Admin_OnBanChecked", "ddsd", playerid, rows > 0 ? 1 : 0, reason, expiresAt);
	}
	return 1;
}

DB_FindAccountByName(requesterid, name[], tag)
{
	new slot = DB_ReqSlot(requesterid);
	g_DBFindTag[slot] = tag;
	if (!DB_IsReady() || name[0] == 0)
	{
		DB_ReqBegin(requesterid, 8);
		DB_DeliverAdminFailure(slot, 8);
		return 0;
	}
	new query[512], reqName[25];
	DB_ReqName(requesterid, reqName, 25);
	mysql_format(g_DB, query, 512, "SELECT id, name, admin_level, IFNULL(last_ip, '') AS last_ip FROM accounts WHERE name = '%e' LIMIT 1", name);
	DB_ReqBegin(requesterid, 8);
	mysql_tquery(g_DB, query, "DB_OnAccountFound", "dsds", requesterid, reqName, tag, name);
}

public DB_OnAccountFound(requesterid, reqName[], tag, name[])
{
	DB_ReqEnd(requesterid, 8);
	new rows = 0, accountId = 0, adminLevel = 0, canon[25], lastIp[16];
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "id", accountId);
		cache_get_value_name(0, "name", canon, 25);
		cache_get_value_name_int(0, "admin_level", adminLevel);
		cache_get_value_name(0, "last_ip", lastIp, 16);
	}
	else
	{
		strcat(canon, name, 25);
	}
	if (funcidx("Admin_OnAccountFound") != -1)
	{
		CallLocalFunction("Admin_OnAccountFound", "dddsds", DB_ReqResolve(requesterid, reqName), tag, accountId, canon, adminLevel, lastIp);
	}
	return 1;
}
