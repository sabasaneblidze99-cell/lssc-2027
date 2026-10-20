new g_FacMembers[100][E_FAC_MEMBER];
new g_FineRows[5][E_FINE_ROW];
new g_JFReq[MAX_PLAYERS][E_JF_REQ];
new g_FineInfl[32][E_FINE_INFL];
new g_FineToken;

bool:DB_JFSessionOk(playerid, const name[], charId)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return false;
	}
	return charId > 0 && gPlayer[playerid][pCharId] == charId;
}

DB_JFStaleRelease(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!IsPlayerConnected(playerid))
	{
		DB_PendingClear(playerid, op);
	}
	return 0;
}

bool:DB_JFCanStart(playerid, op)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid) && !(g_DBPending[playerid] & op);
}

DB_JFCompensate(kind, charId, id, amount)
{
	if (!DB_IsReady() || charId <= 0 || id <= 0 || (kind == 1 && amount <= 0))
	{
		Log_Write("DBJOB", "compensazione NON eseguita (db non pronto o id non valido): verificare a mano");
		return 0;
	}
	new query[512];
	if (kind == 1)
	{
		mysql_format(g_DB, query, 512, "UPDATE job_stats SET runs = IF(runs > 0, runs - 1, 0), earned = IF(earned >= %d, earned - %d, 0) WHERE character_id = %d AND job = %d", amount, amount, charId, id);
	}
	else
	{
		mysql_format(g_DB, query, 512, "DELETE FROM fines WHERE id = %d AND issuer_char = %d", id, charId);
	}
	mysql_tquery(g_DB, query, "DB_OnJFComp", "ddd", kind, charId, id);
	return 0;
}

public DB_OnJFComp(kind, charId, id)
{
	new msg[128];
	format(msg, 128, "%s (char %d, id %d) a sessione scaduta: righe=%d%s", kind == 1 ? "rollback payout lavoro" : "rollback multa", charId, id, cache_affected_rows(), cache_affected_rows() >= 1 ? "" : " ANOMALIA: verificare a mano");
	Log_Write("DBJOB", msg);
	return 1;
}

public DB_JobFacFailure(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	DB_PendingClear(playerid, op);
	switch (op)
	{
		case 262144:
		{
			if (funcidx("Job_OnDBPayout") != -1)
			{
				CallLocalFunction("Job_OnDBPayout", "dddd", playerid, g_JFReq[playerid][jqPayoutJobId], g_JFReq[playerid][jqPayoutAmount], -1);
			}
		}
		case 524288:
		{
			if (funcidx("Job_OnDBSet") != -1)
			{
				CallLocalFunction("Job_OnDBSet", "ddd", playerid, g_JFReq[playerid][jqSetJobId], -1);
			}
		}
		case 2097152:
		{
			if (funcidx("Fine_OnDBLoaded") != -1)
			{
				CallLocalFunction("Fine_OnDBLoaded", "dd", playerid, -1);
			}
		}
		case 4194304:
		{
			if (funcidx("Faction_OnDBChanged") != -1)
			{
				CallLocalFunction("Faction_OnDBChanged", "dddddd", playerid, g_JFReq[playerid][jqFacTargetChar], g_JFReq[playerid][jqFacCmd], g_JFReq[playerid][jqFacFaction], g_JFReq[playerid][jqFacNewRank], -1);
			}
		}
		case 8388608:
		{
			if (funcidx("Faction_OnDBMembers") != -1)
			{
				CallLocalFunction("Faction_OnDBMembers", "ddd", playerid, g_JFReq[playerid][jqMembersFaction], -1);
			}
		}
		case 16777216:
		{
			if (funcidx("Faction_OnDBFound") != -1)
			{
				CallLocalFunction("Faction_OnDBFound", "dddsddd", playerid, g_JFReq[playerid][jqFindTag], -1, "", 0, 0, 0);
			}
		}
	}
	return 1;
}

public DB_JobFacQueryError(const callback[])
{
	if (!strcmp(callback, "DB_OnJFPayout", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(262144);
	}
	else if (!strcmp(callback, "DB_OnJFJobSet", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(524288);
	}
	else if (!strcmp(callback, "DB_OnJFFinesLoaded", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(2097152);
	}
	else if (!strcmp(callback, "DB_OnJFFacChanged", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(4194304);
	}
	else if (!strcmp(callback, "DB_OnJFFacMembers", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(8388608);
	}
	else if (!strcmp(callback, "DB_OnJFFacFound", false, 0x7FFFFFFF))
	{
		DB_FailAllPending(0x01000000);
	}
	else if (!strcmp(callback, "DB_OnJFFineAdded", false, 0x7FFFFFFF))
	{
		DB_JFFailAllFines();
	}
	else if (!strcmp(callback, "DB_OnJFComp", false, 0x7FFFFFFF))
	{
		Log_Write("DBJOB", "compensazione FALLITA per errore DB: verificare a mano");
	}
	return 1;
}

DB_JobPayout(playerid, charId, jobId, amount)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new result = 0;
	if (!DB_IsReady())
	{
		result = -1;
	}
	else if (charId <= 0 || jobId < 1 || jobId > 3 || amount < 1 || amount > 100000 || !DB_JFCanStart(playerid, 262144))
	{
		result = 0;
	}
	else
	{
		new name[25], query[512];
		GetPlayerName(playerid, name, 25);
		g_JFReq[playerid][jqPayoutJobId] = jobId;
		g_JFReq[playerid][jqPayoutAmount] = amount;
		mysql_format(g_DB, query, 512, "INSERT INTO job_stats (character_id, job, runs, earned, last_run_at) SELECT id, %d, 1, %d, NOW() FROM characters WHERE id = %d ON DUPLICATE KEY UPDATE runs = runs + 1, earned = earned + %d, last_run_at = NOW()", jobId, amount, charId, amount);
		DB_PendingSet(playerid, 262144);
		mysql_tquery(g_DB, query, "DB_OnJFPayout", "dsddd", playerid, name, charId, jobId, amount);
		return 0;
	}
	if (funcidx("Job_OnDBPayout") != -1)
	{
		CallLocalFunction("Job_OnDBPayout", "dddd", playerid, jobId, amount, result);
	}
	return 0;
}

public DB_OnJFPayout(playerid, const name[], charId, jobId, amount)
{
	new affected = cache_affected_rows();
	if (!DB_JFSessionOk(playerid, name, charId))
	{
		if (affected > 0)
		{
			DB_JFCompensate(1, charId, jobId, amount);
		}
		DB_JFStaleRelease(playerid, 262144);
		return 1;
	}
	DB_PendingClear(playerid, 262144);
	if (funcidx("Job_OnDBPayout") != -1)
	{
		CallLocalFunction("Job_OnDBPayout", "dddd", playerid, jobId, amount, affected > 0 ? 1 : 0);
	}
	return 1;
}

DB_SetJob(playerid, charId, jobId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new result = 0;
	if (!DB_IsReady())
	{
		result = -1;
	}
	else if (charId <= 0 || jobId < 0 || jobId > 3 || !DB_JFCanStart(playerid, 524288))
	{
		result = 0;
	}
	else
	{
		new name[25], query[512];
		GetPlayerName(playerid, name, 25);
		g_JFReq[playerid][jqSetJobId] = jobId;
		mysql_format(g_DB, query, 512, "UPDATE characters SET job = %d WHERE id = %d AND job <> %d", jobId, charId, jobId);
		DB_PendingSet(playerid, 524288);
		mysql_tquery(g_DB, query, "DB_OnJFJobSet", "dsdd", playerid, name, charId, jobId);
		return 0;
	}
	if (funcidx("Job_OnDBSet") != -1)
	{
		CallLocalFunction("Job_OnDBSet", "ddd", playerid, jobId, result);
	}
	return 0;
}

public DB_OnJFJobSet(playerid, const name[], charId, jobId)
{
	if (!DB_JFSessionOk(playerid, name, charId))
	{
		DB_JFStaleRelease(playerid, 524288);
		return 1;
	}
	DB_PendingClear(playerid, 524288);
	if (funcidx("Job_OnDBSet") != -1)
	{
		CallLocalFunction("Job_OnDBSet", "ddd", playerid, jobId, cache_affected_rows() > 0 ? 1 : 0);
	}
	return 1;
}

DB_JFFineAlloc()
{
	for (new i = 0; i < 32; i++)
	{
		if (!g_FineInfl[i][fiActive])
		{
			return i;
		}
	}
	return -1;
}

DB_JFFineDeliver(idx, fineId)
{
	new target = g_FineInfl[idx][fiTargetChar], amount = g_FineInfl[idx][fiAmount], slot = g_FineInfl[idx][fiIssuerPlayerId], issuerChar = g_FineInfl[idx][fiIssuerChar];
	new bool:same = DB_JFSessionOk(slot, g_FineInfl[idx][fiIssuerName], issuerChar);
	new playerid = same ? slot : 65535;
	if (same)
	{
		DB_PendingClear(slot, 1048576);
	}
	else
	{
		DB_JFStaleRelease(slot, 1048576);
	}
	g_FineInfl[idx][fiActive] = 0;
	if (funcidx("Fine_OnDBAdded") != -1)
	{
		CallLocalFunction("Fine_OnDBAdded", "dddd", playerid, target, fineId, amount);
	}
}

DB_JFFailAllFines()
{
	for (new i = 0; i < 32; i++)
	{
		if (g_FineInfl[i][fiActive])
		{
			DB_JFFineDeliver(i, -1);
		}
	}
}

DB_AddFine(playerid, issuerChar, targetChar, faction, amount, const reason[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new result = 0, idx = -1;
	if (!DB_IsReady())
	{
		result = -1;
	}
	else if (issuerChar <= 0 || targetChar <= 0 || issuerChar == targetChar || gPlayer[playerid][pCharId] != issuerChar || faction < 1 || faction > 2 || amount < 1 || amount > 5000 || strlen(reason) < 3 || !DB_JFCanStart(playerid, 1048576))
	{
		result = 0;
	}
	else
	{
		idx = DB_JFFineAlloc();
		if (idx < 0)
		{
			result = 0;
		}
	}
	if (result != 0 || idx < 0)
	{
		if (funcidx("Fine_OnDBAdded") != -1)
		{
			CallLocalFunction("Fine_OnDBAdded", "dddd", playerid, targetChar, result, amount);
		}
		return 0;
	}
	new name[25], query[1024], rsn[129];
	GetPlayerName(playerid, name, 25);
	strmid(rsn, reason, 0, 128, 129);
	g_FineInfl[idx][fiActive] = 1;
	g_FineInfl[idx][fiToken] = ++g_FineToken;
	g_FineInfl[idx][fiIssuerPlayerId] = playerid;
	format(g_FineInfl[idx][fiIssuerName], 25, "%s", name);
	g_FineInfl[idx][fiIssuerChar] = issuerChar;
	g_FineInfl[idx][fiTargetChar] = targetChar;
	g_FineInfl[idx][fiAmount] = amount;
	mysql_format(g_DB, query, 1024, "INSERT IGNORE INTO fines (faction, issuer_char, target_char, amount, reason) SELECT i.faction, i.id, t.id, %d, LEFT('%e', %d) FROM characters i JOIN characters t ON t.id = %d WHERE i.id = %d AND i.faction = %d AND i.id <> t.id AND i.faction_rank >= %d AND %d <= CASE i.faction_rank WHEN 2 THEN %d WHEN 3 THEN %d WHEN 4 THEN %d ELSE %d END", amount, rsn, 64, targetChar, issuerChar, faction, 2, amount, 500, 1000, 2500, 5000);
	DB_PendingSet(playerid, 1048576);
	mysql_tquery(g_DB, query, "DB_OnJFFineAdded", "dd", idx, g_FineInfl[idx][fiToken]);
	return 0;
}

public DB_OnJFFineAdded(idx, token)
{
	new fineId = 0;
	if (cache_affected_rows() == 1)
	{
		fineId = cache_insert_id();
	}
	if (idx < 0 || idx >= 32 || !g_FineInfl[idx][fiActive] || g_FineInfl[idx][fiToken] != token)
	{
		if (fineId > 0 && DB_IsReady())
		{
			new query[512];
			mysql_format(g_DB, query, 512, "DELETE FROM fines WHERE id = %d", fineId);
			mysql_tquery(g_DB, query, "DB_OnJFComp", "ddd", 2, 0, fineId);
		}
		return 1;
	}
	DB_JFFineDeliver(idx, fineId);
	return 1;
}

DB_LoadFines(playerid, charId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || charId <= 0)
	{
		if (funcidx("Fine_OnDBLoaded") != -1)
		{
			CallLocalFunction("Fine_OnDBLoaded", "dd", playerid, -1);
		}
		return 0;
	}
	new name[25], query[1024];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 1024, "SELECT f.id, f.faction, f.amount, UNIX_TIMESTAMP(f.created_at) AS ts, f.reason, a.name AS issuer FROM fines f JOIN characters c ON c.id = f.issuer_char JOIN accounts a ON a.id = c.account_id WHERE f.target_char = %d ORDER BY f.id DESC LIMIT %d", charId, 5);
	DB_PendingSet(playerid, 2097152);
	mysql_tquery(g_DB, query, "DB_OnJFFinesLoaded", "dsd", playerid, name, charId);
	return 0;
}

public DB_OnJFFinesLoaded(playerid, const name[], charId)
{
	if (!DB_JFSessionOk(playerid, name, charId))
	{
		DB_JFStaleRelease(playerid, 2097152);
		return 1;
	}
	DB_PendingClear(playerid, 2097152);
	new rows = 0, n = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows && n < 5; i++)
	{
		cache_get_value_name_int(i, "id", g_FineRows[n][frId]);
		cache_get_value_name_int(i, "faction", g_FineRows[n][frFaction]);
		cache_get_value_name_int(i, "amount", g_FineRows[n][frAmount]);
		cache_get_value_name_int(i, "ts", g_FineRows[n][frTimestamp]);
		cache_get_value_name(i, "reason", g_FineRows[n][frReason], 65);
		cache_get_value_name(i, "issuer", g_FineRows[n][frIssuer], 25);
		n++;
	}
	if (funcidx("Fine_OnDBLoaded") != -1)
	{
		CallLocalFunction("Fine_OnDBLoaded", "dd", playerid, n);
	}
	return 1;
}

DB_JFFacExec(playerid, actorChar, targetChar, cmd, faction, newRank)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new result = 0;
	if (!DB_IsReady())
	{
		result = -1;
	}
	else if (actorChar <= 0 || targetChar <= 0 || gPlayer[playerid][pCharId] != actorChar || !DB_JFCanStart(playerid, 4194304))
	{
		result = 0;
	}
	else if (cmd != 6 && (faction < 1 || faction > 2))
	{
		result = 0;
	}
	else if (cmd == 6 && ((faction < 0 || faction > 2) || (faction == 0 && newRank != 0) || (faction > 0 && (newRank < 1 || newRank > 6))))
	{
		result = 0;
	}
	else if (cmd != 5 && cmd != 6 && actorChar == targetChar)
	{
		result = 0;
	}
	else if (cmd == 2 && (newRank < 2 || newRank > 6))
	{
		result = 0;
	}
	else if (cmd == 3 && (newRank < 1 || newRank >= 6))
	{
		result = 0;
	}
	else
	{
		new name[25], query[1024];
		GetPlayerName(playerid, name, 25);
		switch (cmd)
		{
			case 1:
			{
				mysql_format(g_DB, query, 1024, "UPDATE characters t JOIN characters a ON a.id = %d SET t.faction = a.faction, t.faction_rank = 1 WHERE t.id = %d AND t.id <> a.id AND t.faction = 0 AND t.level >= %d AND a.faction = %d AND a.faction_rank >= %d", actorChar, targetChar, 3, faction, 5);
			}
			case 2:
			{
				mysql_format(g_DB, query, 1024, "UPDATE characters t JOIN characters a ON a.id = %d SET t.faction_rank = %d WHERE t.id = %d AND t.id <> a.id AND t.faction = %d AND a.faction = %d AND a.faction_rank >= %d AND t.faction_rank = %d AND %d < a.faction_rank", actorChar, newRank, targetChar, faction, faction, 5, newRank - 1, newRank);
			}
			case 3:
			{
				mysql_format(g_DB, query, 1024, "UPDATE characters t JOIN characters a ON a.id = %d SET t.faction_rank = %d WHERE t.id = %d AND t.id <> a.id AND t.faction = %d AND a.faction = %d AND a.faction_rank >= %d AND t.faction_rank = %d AND t.faction_rank < a.faction_rank", actorChar, newRank, targetChar, faction, faction, 5, newRank + 1);
			}
			case 4:
			{
				mysql_format(g_DB, query, 1024, "UPDATE characters t JOIN characters a ON a.id = %d SET t.faction = 0, t.faction_rank = 0 WHERE t.id = %d AND t.id <> a.id AND t.faction = %d AND a.faction = %d AND a.faction_rank >= %d AND t.faction_rank < a.faction_rank", actorChar, targetChar, faction, faction, 5);
			}
			case 5:
			{
				mysql_format(g_DB, query, 1024, "UPDATE characters SET faction = 0, faction_rank = 0 WHERE id = %d AND faction = %d", targetChar, faction);
			}
			default:
			{
				mysql_format(g_DB, query, 1024, "UPDATE characters SET faction = %d, faction_rank = %d WHERE id = %d", faction, newRank, targetChar);
			}
		}
		g_JFReq[playerid][jqFacTargetChar] = targetChar;
		g_JFReq[playerid][jqFacCmd] = cmd;
		g_JFReq[playerid][jqFacFaction] = faction;
		g_JFReq[playerid][jqFacNewRank] = newRank;
		DB_PendingSet(playerid, 4194304);
		mysql_tquery(g_DB, query, "DB_OnJFFacChanged", "dsddddd", playerid, name, actorChar, targetChar, cmd, faction, newRank);
		return 0;
	}
	if (funcidx("Faction_OnDBChanged") != -1)
	{
		CallLocalFunction("Faction_OnDBChanged", "dddddd", playerid, targetChar, cmd, faction, newRank, result);
	}
	return 0;
}

public DB_OnJFFacChanged(playerid, const name[], actorChar, targetChar, cmd, faction, rank)
{
	new affected = cache_affected_rows();
	if (affected > 0)
	{
		new detail[65];
		new action[7][] = {"", "join", "promote", "demote", "fire", "leave", "set"};
		if (cmd == 4 || cmd == 5)
		{
			detail[0] = 0;
		}
		else
		{
			format(detail, 65, "grado %d", rank);
		}
		if (cmd >= 1 && cmd <= 6)
		{
			DB_FactionLogAdd(faction, actorChar, cmd == 5 ? actorChar : targetChar, action[cmd], detail);
		}
	}
	new bool:same = DB_JFSessionOk(playerid, name, actorChar);
	if (same)
	{
		DB_PendingClear(playerid, 4194304);
	}
	else
	{
		DB_JFStaleRelease(playerid, 4194304);
	}
	if (funcidx("Faction_OnDBChanged") != -1)
	{
		CallLocalFunction("Faction_OnDBChanged", "dddddd", same ? playerid : 65535, targetChar, cmd, faction, rank, affected > 0 ? 1 : 0);
	}
	return 1;
}

DB_FactionJoin(playerid, actorChar, targetChar, faction)
{
	DB_JFFacExec(playerid, actorChar, targetChar, 1, faction, 1);
}

DB_FactionPromote(playerid, actorChar, targetChar, faction, newRank)
{
	DB_JFFacExec(playerid, actorChar, targetChar, 2, faction, newRank);
}

DB_FactionDemote(playerid, actorChar, targetChar, faction, newRank)
{
	DB_JFFacExec(playerid, actorChar, targetChar, 3, faction, newRank);
}

DB_FactionFire(playerid, actorChar, targetChar, faction)
{
	DB_JFFacExec(playerid, actorChar, targetChar, 4, faction, 0);
}

DB_FactionLeave(playerid, charId, faction)
{
	DB_JFFacExec(playerid, charId, charId, 5, faction, 0);
}

DB_FactionSet(playerid, charId, faction, rank)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	DB_JFFacExec(playerid, gPlayer[playerid][pCharId], charId, 6, faction, rank);
	return 0;
}

DB_FactionLogAdd(faction, actorChar, targetChar, const action[], const detail[])
{
	if (!DB_IsReady() || action[0] == 0)
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "INSERT INTO faction_log (faction, actor_char, target_char, action, detail) VALUES (%d, (SELECT id FROM characters WHERE id = %d), (SELECT id FROM characters WHERE id = %d), LEFT('%e', %d), LEFT('%e', %d))", clamp(faction, 0, 255), max(actorChar, 0), max(targetChar, 0), action, 24, detail, 64);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_FactionMembers(playerid, faction)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || gPlayer[playerid][pCharId] <= 0)
	{
		if (funcidx("Faction_OnDBMembers") != -1)
		{
			CallLocalFunction("Faction_OnDBMembers", "ddd", playerid, faction, -1);
		}
		return 0;
	}
	if (faction < 1 || faction > 2)
	{
		if (funcidx("Faction_OnDBMembers") != -1)
		{
			CallLocalFunction("Faction_OnDBMembers", "ddd", playerid, faction, 0);
		}
		return 0;
	}
	new name[25], query[1024];
	GetPlayerName(playerid, name, 25);
	g_JFReq[playerid][jqMembersFaction] = faction;
	mysql_format(g_DB, query, 1024, "SELECT c.id, a.name, c.faction_rank, c.level, COALESCE(UNIX_TIMESTAMP(a.last_login), 0) AS last_login FROM characters c JOIN accounts a ON a.id = c.account_id WHERE c.faction = %d ORDER BY c.faction_rank DESC, a.name LIMIT %d", faction, 100);
	DB_PendingSet(playerid, 8388608);
	mysql_tquery(g_DB, query, "DB_OnJFFacMembers", "dsdd", playerid, name, gPlayer[playerid][pCharId], faction);
	return 0;
}

public DB_OnJFFacMembers(playerid, const name[], charId, faction)
{
	if (!DB_JFSessionOk(playerid, name, charId))
	{
		DB_JFStaleRelease(playerid, 8388608);
		return 1;
	}
	DB_PendingClear(playerid, 8388608);
	new rows = 0, n = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows && n < 100; i++)
	{
		cache_get_value_name_int(i, "id", g_FacMembers[n][fmCharId]);
		cache_get_value_name(i, "name", g_FacMembers[n][fmName], 25);
		cache_get_value_name_int(i, "faction_rank", g_FacMembers[n][fmRank]);
		cache_get_value_name_int(i, "level", g_FacMembers[n][fmLevel]);
		cache_get_value_name_int(i, "last_login", g_FacMembers[n][fmLastLogin]);
		n++;
	}
	if (funcidx("Faction_OnDBMembers") != -1)
	{
		CallLocalFunction("Faction_OnDBMembers", "ddd", playerid, faction, n);
	}
	return 1;
}

DB_FactionFindByName(playerid, const name[], tag)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || gPlayer[playerid][pCharId] <= 0)
	{
		if (funcidx("Faction_OnDBFound") != -1)
		{
			CallLocalFunction("Faction_OnDBFound", "dddsddd", playerid, tag, -1, "", 0, 0, 0);
		}
		return 0;
	}
	if (name[0] == 0 || strlen(name) > 24)
	{
		if (funcidx("Faction_OnDBFound") != -1)
		{
			CallLocalFunction("Faction_OnDBFound", "dddsddd", playerid, tag, 0, "", 0, 0, 0);
		}
		return 0;
	}
	new me[25], query[1024];
	GetPlayerName(playerid, me, 25);
	g_JFReq[playerid][jqFindTag] = tag;
	mysql_format(g_DB, query, 1024, "SELECT c.id, a.name, c.faction, c.faction_rank, c.level FROM characters c JOIN accounts a ON a.id = c.account_id WHERE a.name = '%e' AND c.slot = %d LIMIT 1", name, 0);
	DB_PendingSet(playerid, 0x01000000);
	mysql_tquery(g_DB, query, "DB_OnJFFacFound", "dsdd", playerid, me, gPlayer[playerid][pCharId], tag);
	return 0;
}

public DB_OnJFFacFound(playerid, const name[], charId, tag)
{
	if (!DB_JFSessionOk(playerid, name, charId))
	{
		DB_JFStaleRelease(playerid, 0x01000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x01000000);
	new rows = 0, id = 0, faction = 0, rank = 0, level = 0, found[25];
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "id", id);
		cache_get_value_name(0, "name", found, 25);
		cache_get_value_name_int(0, "faction", faction);
		cache_get_value_name_int(0, "faction_rank", rank);
		cache_get_value_name_int(0, "level", level);
	}
	if (funcidx("Faction_OnDBFound") != -1)
	{
		CallLocalFunction("Faction_OnDBFound", "dddsddd", playerid, tag, id, found, faction, rank, level);
	}
	return 1;
}
