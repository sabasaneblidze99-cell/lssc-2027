new g_StoryReq[MAX_PLAYERS][E_STORY_REQ];
new g_StoryInfo[MAX_PLAYERS][E_STORY_INFO];
new g_StoryTokenSeq;
new g_StoryOpSeq;

DB_StoryMakeOpId(dest[], size, charId)
{
	g_StoryOpSeq++;
	format(dest, size, "%08x%08x%08x%08x", charId, g_StoryOpSeq, gettime(), GetTickCount());
}

DB_StoryLastOpId(playerid, dest[], size)
{
	dest[0] = 0;
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	strcat(dest, g_StoryReq[playerid][sqOpId], size);
	return 0;
}

DB_StoryCash(motive)
{
	switch (motive)
	{
		case 2:
		{
			return ECON_START_CASH_M2;
		}
		case 3:
		{
			return ECON_START_CASH_M3;
		}
		case 4:
		{
			return ECON_START_CASH_M4;
		}
	}
	return ECON_START_CASH;
}

DB_StoryBankStart(motive)
{
	return ECON_START_TOTAL - DB_StoryCash(motive);   // = SQL story_start_total()
}

bool:DB_StoryOpIdOk(const opId[])
{
	new len = strlen(opId);
	return len >= 1 && len <= 36;
}

DB_StoryApply(playerid, origin, motive)
{
	gPlayer[playerid][pStoryOrigin] = origin;
	gPlayer[playerid][pStoryMotive] = motive;
}

bool:DB_StoryInfoLive(playerid, const name[], token, kind)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return false;
	}
	if (!(g_DBPending[playerid] & 0x20000000))
	{
		return false;
	}
	return g_StoryInfo[playerid][siToken] == token && g_StoryInfo[playerid][siKind] == kind;
}

bool:DB_StoryOpLive(playerid, const name[], charId, token)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return false;
	}
	if (charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		return false;
	}
	if (!(g_DBPending[playerid] & 0x40000000))
	{
		return false;
	}
	return g_StoryReq[playerid][sqToken] == token && g_StoryReq[playerid][sqGen] == g_BankGen[playerid];
}

DB_StoryStaleRelease(playerid, bit)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!IsPlayerConnected(playerid))
	{
		DB_PendingClear(playerid, bit);
	}
	return 0;
}

DB_StoryDeliverLoad(playerid, result, introSeen, origin, motive, bool:hasStory)
{
	if (funcidx("Story_OnDBLoaded") != -1)
	{
		CallLocalFunction("Story_OnDBLoaded", "dddddd", playerid, result, introSeen, origin, motive, hasStory);
	}
}

DB_StoryDeliverRead(playerid, targetid, purpose, result, origin, motive, bool:hasStory)
{
	if (funcidx("Story_OnDBRead") != -1)
	{
		CallLocalFunction("Story_OnDBRead", "ddddddd", playerid, targetid, purpose, result, origin, motive, hasStory);
	}
}

DB_StoryDeliverSaved(playerid, result, origin, motive, moneyDelta)
{
	if (funcidx("Story_OnDBSaved") != -1)
	{
		CallLocalFunction("Story_OnDBSaved", "ddddd", playerid, result, origin, motive, moneyDelta);
	}
}

bool:DB_StoryInfoBegin(playerid, kind, bool:valid, accountId, charId, targetid, purpose)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return false;
	}
	new tchar = 0, tgen = 0;
	if (kind == 2 && targetid >= 0 && targetid < MAX_PLAYERS && IsPlayerConnected(targetid))
	{
		tchar = gPlayer[targetid][pCharId];
		tgen = g_BankGen[targetid];
	}
	if (g_DBPending[playerid] & 0x20000000)
	{
		if (kind == 1)
		{
			DB_StoryDeliverLoad(playerid, 5, 0, 0, 0, false);
		}
		else
		{
			DB_StoryDeliverRead(playerid, targetid, purpose, 5, 0, 0, false);
		}
		return false;
	}
	if (!valid)
	{
		if (kind == 1)
		{
			DB_StoryDeliverLoad(playerid, 0, 0, 0, 0, false);
		}
		else
		{
			DB_StoryDeliverRead(playerid, targetid, purpose, 0, 0, 0, false);
		}
		return false;
	}
	g_StoryInfo[playerid][siToken] = ++g_StoryTokenSeq;
	g_StoryInfo[playerid][siKind] = kind;
	g_StoryInfo[playerid][siAccountId] = accountId;
	g_StoryInfo[playerid][siCharId] = charId;
	g_StoryInfo[playerid][siTargetId] = targetid;
	g_StoryInfo[playerid][siTargetChar] = tchar;
	g_StoryInfo[playerid][siTargetGen] = tgen;
	g_StoryInfo[playerid][siPurpose] = purpose;
	if (!DB_IsReady())
	{
		if (kind == 1)
		{
			DB_StoryDeliverLoad(playerid, -1, 0, 0, 0, false);
		}
		else
		{
			DB_StoryDeliverRead(playerid, targetid, purpose, -1, 0, 0, false);
		}
		return false;
	}
	DB_PendingSet(playerid, 0x20000000);
	return true;
}

DB_StoryLoad(playerid, accountId, charId)
{
	new bool:ok = false;
	if (playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid))
	{
		ok = accountId > 0 && gPlayer[playerid][pAccountId] == accountId && charId >= 0 && gPlayer[playerid][pCharId] == charId;
	}
	if (!DB_StoryInfoBegin(playerid, 1, ok, accountId, charId, 65535, 0))
	{
		return 0;
	}
	new query[512], name[75];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 512, "SELECT a.intro_seen AS intro_seen, COALESCE(s.origin, 0) AS origin, COALESCE(s.motive, 0) AS motive, (s.character_id IS NOT NULL) AS has_story FROM accounts a LEFT JOIN character_story s ON s.character_id = %d WHERE a.id = %d", charId, accountId);
	mysql_tquery(g_DB, query, "DB_OnStoryLoaded", "dsddd", playerid, name, accountId, charId, g_StoryInfo[playerid][siToken]);
	return 0;
}

public DB_OnStoryLoaded(playerid, const name[], accountId, charId, token)
{
	if (!DB_StoryInfoLive(playerid, name, token, 1))
	{
		DB_StoryStaleRelease(playerid, 0x20000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x20000000);
	if (gPlayer[playerid][pAccountId] != accountId || gPlayer[playerid][pCharId] != charId)
	{
		return 1;
	}
	new rows = 0, intro = 0, origin = 0, motive = 0, has = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		DB_StoryDeliverLoad(playerid, 0, 0, 0, 0, false);
		return 1;
	}
	cache_get_value_name_int(0, "intro_seen", intro);
	cache_get_value_name_int(0, "origin", origin);
	cache_get_value_name_int(0, "motive", motive);
	cache_get_value_name_int(0, "has_story", has);
	if (intro != 0)
	{
		intro = 1;
	}
	gPlayer[playerid][pIntroSeen] = intro;
	DB_StoryApply(playerid, origin, motive);
	DB_StoryDeliverLoad(playerid, 1, intro, origin, motive, has != 0);
	return 1;
}

DB_StoryRead(playerid, targetid, purpose)
{
	new bool:ok = false;
	if (playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid) && targetid >= 0 && targetid < MAX_PLAYERS && IsPlayerConnected(targetid))
	{
		ok = gPlayer[targetid][pCharId] > 0 && gPlayer[playerid][pCharId] > 0;
	}
	if (!DB_StoryInfoBegin(playerid, 2, ok, 0, 0, targetid, purpose))
	{
		return 0;
	}
	new query[512], name[75];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 512, "SELECT COALESCE(s.origin, 0) AS origin, COALESCE(s.motive, 0) AS motive, (s.character_id IS NOT NULL) AS has_story FROM characters c LEFT JOIN character_story s ON s.character_id = c.id WHERE c.id = %d", g_StoryInfo[playerid][siTargetChar]);
	mysql_tquery(g_DB, query, "DB_OnStoryRead", "dsd", playerid, name, g_StoryInfo[playerid][siToken]);
	return 0;
}

public DB_OnStoryRead(playerid, const name[], token)
{
	if (!DB_StoryInfoLive(playerid, name, token, 2))
	{
		DB_StoryStaleRelease(playerid, 0x20000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x20000000);
	new target = g_StoryInfo[playerid][siTargetId], purpose = g_StoryInfo[playerid][siPurpose];
	if (target < 0 || target >= MAX_PLAYERS || !IsPlayerConnected(target))
	{
		return 1;
	}
	if (gPlayer[target][pCharId] != g_StoryInfo[playerid][siTargetChar] || g_BankGen[target] != g_StoryInfo[playerid][siTargetGen])
	{
		return 1;
	}
	new rows = 0, origin = 0, motive = 0, has = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		DB_StoryDeliverRead(playerid, target, purpose, 0, 0, 0, false);
		return 1;
	}
	cache_get_value_name_int(0, "origin", origin);
	cache_get_value_name_int(0, "motive", motive);
	cache_get_value_name_int(0, "has_story", has);
	DB_StoryDeliverRead(playerid, target, purpose, 1, origin, motive, has != 0);
	return 1;
}

DB_StorySave(playerid, charId, origin, motive, const opId[], bool:creation)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_DBPending[playerid] & 0x40000000)
	{
		DB_StoryDeliverSaved(playerid, 5, 0, 0, 0);
		return 0;
	}
	new bool:ok = charId > 0 && gPlayer[playerid][pCharId] == charId && DB_StoryOpIdOk(opId) && origin >= 0 && origin <= 5 && motive >= 0 && motive <= 4 && (creation || origin != 0 || motive != 0);
	if (!ok)
	{
		DB_StoryDeliverSaved(playerid, 0, 0, 0, 0);
		return 0;
	}
	g_StoryReq[playerid][sqToken] = ++g_StoryTokenSeq;
	g_StoryReq[playerid][sqGen] = g_BankGen[playerid];
	g_StoryReq[playerid][sqCreation] = creation;
	g_StoryReq[playerid][sqOpId][0] = 0;
	strcat(g_StoryReq[playerid][sqOpId], opId, 37);
	if (!DB_IsReady())
	{
		DB_StoryDeliverSaved(playerid, -1, 0, 0, 0);
		return 0;
	}
	DB_PendingSet(playerid, _:2.0);
	new query[512], name[75];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 512, "CALL story_save('%e', %d, %d, %d, %d)", opId, charId, origin, motive, creation ? 0 : 1);
	mysql_tquery(g_DB, query, "DB_OnStoryCalled", "dsdds", playerid, name, charId, g_StoryReq[playerid][sqToken], opId);
	return 0;
}

public DB_OnStoryCalled(playerid, const name[], charId, token, const opId[])
{
	if (!DB_StoryOpLive(playerid, name, charId, token))
	{
		DB_StoryStaleRelease(playerid, _:2.0);
		return 1;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT o.result AS result, o.money_delta AS money_delta, COALESCE(s.origin, 0) AS origin, COALESCE(s.motive, 0) AS motive FROM story_ops o LEFT JOIN character_story s ON s.character_id = o.character_id WHERE o.op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnStoryResult", "dsdds", playerid, name, charId, token, opId);
	return 1;
}

public DB_OnStoryResult(playerid, const name[], charId, token, const opId[])
{
	new rows = 0, result = -1, delta = 0, origin = 0, motive = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
		cache_get_value_name_int(0, "money_delta", delta);
		cache_get_value_name_int(0, "origin", origin);
		cache_get_value_name_int(0, "motive", motive);
	}
	if (!DB_StoryOpLive(playerid, name, charId, token))
	{
		if (result == 3)
		{
			Log_Write("DBSTORY", "sessione scaduta dopo una ripartizione dei soldi iniziali: verificare i saldi del personaggio (story_ops)");
		}
		DB_StoryStaleRelease(playerid, _:2.0);
		return 1;
	}
	DB_PendingClear(playerid, _:2.0);
	if (result == 1 || result == 3 || result == 4 || result == 2)
	{
		DB_StoryApply(playerid, origin, motive);
		if (result == 3 && delta != 0)
		{
			gPlayer[playerid][pMoney] += delta;
			gPlayer[playerid][pBank] -= delta;
			g_BankDb[playerid] -= delta;
		}
		else
		{
			delta = 0;
		}
	}
	else
	{
		delta = 0;
	}
	DB_StoryDeliverSaved(playerid, result, origin, motive, delta);
	return 1;
}

DB_StoryIntroSeen(accountId, playerid)
{
	if (accountId <= 0)
	{
		return 0;
	}
	if (playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid) && gPlayer[playerid][pAccountId] == accountId)
	{
		gPlayer[playerid][pIntroSeen] = 1;
	}
	if (!DB_IsReady())
	{
		Log_Write("DBSTORY", "intro_seen non salvato (db non pronto)");
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE accounts SET intro_seen = 1 WHERE id = %d", accountId);
	mysql_tquery(g_DB, query, "DB_OnStoryIntroSet", "d", accountId);
	return 0;
}

public DB_OnStoryIntroSet(accountId)
{
	return 1;
}

public DB_StoryFailure(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	DB_PendingClear(playerid, op);
	if (op == 0x20000000)
	{
		if (g_StoryInfo[playerid][siKind] == 1)
		{
			DB_StoryDeliverLoad(playerid, -1, 0, 0, 0, false);
		}
		else
		{
			DB_StoryDeliverRead(playerid, g_StoryInfo[playerid][siTargetId], g_StoryInfo[playerid][siPurpose], -1, 0, 0, false);
		}
		return 1;
	}
	DB_StoryDeliverSaved(playerid, -1, 0, 0, 0);
	return 1;
}

public DB_StoryQueryError(const callback[])
{
	if (!strcmp(callback, "DB_OnStoryLoaded", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnStoryRead", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(0x20000000);
	}
	else if (!strcmp(callback, "DB_OnStoryIntroSet", false, 0x7FFFFFFF))
	{
		Log_Write("DBSTORY", "errore query: intro_seen non salvato");
	}
	else
	{
		DB_FailAllPending(_:2.0);
	}
	return 1;
}
