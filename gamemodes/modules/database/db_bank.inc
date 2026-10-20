new g_BankStmt[15][E_BANK_STMT];
new g_BankReq[MAX_PLAYERS][E_BANK_REQ];
new g_BankInfo[MAX_PLAYERS][E_BANK_INFO];
new g_BankTokenSeq;
new g_BankOpSeq;
new g_BankUndoNotify;

DB_BankMakeOpId(dest[], size, charId)
{
	g_BankOpSeq++;
	format(dest, size, "%08x%08x%08x%08x", charId, g_BankOpSeq, gettime(), GetTickCount());
}

DB_BankLastOpId(playerid, dest[], size)
{
	dest[0] = 0;
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	strcat(dest, g_BankReq[playerid][bqOpId], size);
	return 0;
}

DB_BankXferFee(amount)
{
	if (amount < 1)
	{
		return 0;
	}
	if (amount > 0x3B9ACA00)
	{
		amount = 0x3B9ACA00;
	}
	new fee = (amount * 2 + 99) / 100;
	return fee < 5 ? 5 : fee;
}

bool:DB_BankOpIdOk(const opId[])
{
	new len = strlen(opId);
	return len >= 1 && len <= 36;
}

DB_BankMirror(playerid, delta)
{
	gPlayer[playerid][pBank] += delta;
	g_BankDb[playerid] += delta;
}

bool:DB_BankLive(playerid, const name[], charId, token, op)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 0;
	}
	if (charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		return 0;
	}
	if (!(g_DBPending[playerid] & 0x08000000))
	{
		return 0;
	}
	return g_BankReq[playerid][bqToken] == token && g_BankReq[playerid][bqOp] == op && g_BankReq[playerid][bqGen] == g_BankGen[playerid];
}

bool:DB_BankInfoLive(playerid, const name[], charId, token, infoOp)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 0;
	}
	if (charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		return 0;
	}
	if (!(g_DBPending[playerid] & 0x10000000))
	{
		return 0;
	}
	return g_BankInfo[playerid][biToken] == token && g_BankInfo[playerid][biInfoOp] == infoOp;
}

DB_BankStaleRelease(playerid, bit)
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

DB_BankDeliver(playerid, op, result, amount, fee, balance, recv)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		playerid = INVALID_PLAYER_ID;
	}
	switch (op)
	{
		case 1:
		{
			if (funcidx("Bank_OnDBDeposit") != -1)
			{
				CallLocalFunction("Bank_OnDBDeposit", "dddd", playerid, amount, result, balance);
			}
		}
		case 2:
		{
			if (funcidx("Bank_OnDBWithdraw") != -1)
			{
				CallLocalFunction("Bank_OnDBWithdraw", "dddd", playerid, amount, result, balance);
			}
		}
		case 3:
		{
			if (funcidx("Bank_OnDBTransfer") != -1)
			{
				CallLocalFunction("Bank_OnDBTransfer", "dddddd", playerid, recv, amount, fee, result, balance);
			}
		}
	}
}

DB_BankFlush(playerid)
{
	if (gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	new d = DB_BankDeltaTake(playerid);
	if (d == 0)
	{
		return 0;
	}
	new query[512], name[25];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "UPDATE characters SET bank = GREATEST(0, LEAST(%d, CAST(bank AS SIGNED) + (%d))) WHERE id = %d", 0x3B9ACA00, d, gPlayer[playerid][pCharId]);
	mysql_tquery(g_DB, query, "DB_OnBankDeltaSaved", "dsddd", playerid, name, gPlayer[playerid][pCharId], g_BankGen[playerid], d);
	return 0;
}

bool:DB_BankBegin(playerid, op, bool:valid, charId, amount, const opId[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_DBPending[playerid] & 0x08000000)
	{
		DB_BankDeliver(playerid, op, 7, amount, 0, 0, 65535);
		return 0;
	}
	if (!valid || charId <= 0 || gPlayer[playerid][pCharId] != charId || !DB_BankOpIdOk(opId))
	{
		DB_BankDeliver(playerid, op, 0, amount, 0, 0, 65535);
		return 0;
	}
	g_BankReq[playerid][bqToken] = ++g_BankTokenSeq;
	g_BankReq[playerid][bqOp] = op;
	g_BankReq[playerid][bqAmount] = amount;
	g_BankReq[playerid][bqGen] = g_BankGen[playerid];
	g_BankReq[playerid][bqXferPending] = 0;
	g_BankReq[playerid][bqOpId][0] = 0;
	strcat(g_BankReq[playerid][bqOpId], opId, 37);
	if (!DB_IsReady())
	{
		DB_BankDeliver(playerid, op, -1, amount, 0, 0, 65535);
		return 0;
	}
	DB_PendingSet(playerid, 0x08000000);
	return 1;
}

DB_BankCompensate(const opId[], op)
{
	if (op != 1 && op != 2)
	{
		return 0;
	}
	if (!DB_IsReady() || !DB_BankOpIdOk(opId))
	{
		Log_Write("DBBANK", "compensazione NON eseguita (db non pronto o op_id non valido): verificare a mano");
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "CALL bank_undo('%e', %d)", opId, 0x3B9ACA00);
	mysql_tquery(g_DB, query, "DB_OnBankUndoCalled", "sd", opId, 0);
	return 0;
}

DB_BankInfoDeliver(playerid, infoOp, result, page, a, b, c, d, bool:more)
{
	switch (infoOp)
	{
		case 1:
		{
			if (funcidx("Bank_OnDBLimits") != -1)
			{
				CallLocalFunction("Bank_OnDBLimits", "dddddd", playerid, result, a, b, c, d);
			}
		}
		case 2:
		{
			if (funcidx("Bank_OnDBStatement") != -1)
			{
				CallLocalFunction("Bank_OnDBStatement", "ddddd", playerid, result, page, a, more);
			}
		}
		case 3:
		{
			if (funcidx("Bank_OnDBSynced") != -1)
			{
				CallLocalFunction("Bank_OnDBSynced", "ddd", playerid, result, a);
			}
		}
	}
}

bool:DB_BankInfoBegin(playerid, infoOp, charId, page)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_DBPending[playerid] & 0x10000000)
	{
		DB_BankInfoDeliver(playerid, infoOp, 7, page, 0, 0, 0, -1, 0);
		return 0;
	}
	if (charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		DB_BankInfoDeliver(playerid, infoOp, 0, page, 0, 0, 0, -1, 0);
		return 0;
	}
	g_BankInfo[playerid][biToken] = ++g_BankTokenSeq;
	g_BankInfo[playerid][biInfoOp] = infoOp;
	g_BankInfo[playerid][biPage] = page;
	if (!DB_IsReady())
	{
		DB_BankInfoDeliver(playerid, infoOp, -1, page, 0, 0, 0, -1, 0);
		return 0;
	}
	DB_PendingSet(playerid, 0x10000000);
	return 1;
}

DB_BankLimits(playerid, charId)
{
	if (!DB_BankInfoBegin(playerid, 1, charId, 0))
	{
		return 0;
	}
	new query[512], name[25];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "SELECT LEAST(bank_day_withdrawn(c.id, %d), 2147483647) AS wd, LEAST(bank_day_xfer_out(c.id, %d), 2147483647) AS xo, LEAST(bank_day_xfer_in(c.id, %d), 2147483647) AS xi, COALESCE(TIMESTAMPDIFF(SECOND, (SELECT MAX(ts) FROM bank_ledger WHERE character_id = c.id AND kind = 'xfer_out'), NOW()), -1) AS since FROM characters c WHERE c.id = %d", 24, 24, 24, charId);
	mysql_tquery(g_DB, query, "DB_OnBankLimits", "dsdd", playerid, name, charId, g_BankInfo[playerid][biToken]);
	return 0;
}

public DB_OnBankLimits(playerid, const name[], charId, token)
{
	if (!DB_BankInfoLive(playerid, name, charId, token, 1))
	{
		DB_BankStaleRelease(playerid, 0x10000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x10000000);
	new rows = 0, wd = 0, xo = 0, xi = 0, since = -1, result = -1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		result = 1;
		cache_get_value_name_int(0, "wd", wd);
		cache_get_value_name_int(0, "xo", xo);
		cache_get_value_name_int(0, "xi", xi);
		cache_get_value_name_int(0, "since", since);
	}
	DB_BankInfoDeliver(playerid, 1, result, 0, wd, xo, xi, since, 0);
	return 1;
}

DB_BankStatement(playerid, charId, page)
{
	if (page < 0 || page >= 5)
	{
		if (playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid))
		{
			DB_BankInfoDeliver(playerid, 2, 0, page, 0, 0, 0, 0, 0);
		}
		return 0;
	}
	if (!DB_BankInfoBegin(playerid, 2, charId, page))
	{
		return 0;
	}
	new query[512], name[25];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "SELECT UNIX_TIMESTAMP(l.ts) AS ts, l.kind AS kind, l.delta AS delta, l.balance_after AS balance, COALESCE(a.name, '') AS other, COALESCE(l.ref, '') AS ref FROM bank_ledger l LEFT JOIN characters oc ON oc.id = l.other_character_id LEFT JOIN accounts a ON a.id = oc.account_id WHERE l.character_id = %d ORDER BY l.id DESC LIMIT %d OFFSET %d", charId, 16, page * 15);
	mysql_tquery(g_DB, query, "DB_OnBankStatement", "dsdd", playerid, name, charId, g_BankInfo[playerid][biToken]);
	return 0;
}

public DB_OnBankStatement(playerid, const name[], charId, token)
{
	if (!DB_BankInfoLive(playerid, name, charId, token, 2))
	{
		DB_BankStaleRelease(playerid, 0x10000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x10000000);
	new rows = 0, count = 0;
	cache_get_row_count(rows);
	for (new r = 0; r < rows && count < 15; r++)
	{
		g_BankStmt[count][bsTimestamp] = 0;
		g_BankStmt[count][bsDelta] = 0;
		g_BankStmt[count][bsBalance] = 0;
		cache_get_value_name_int(r, "ts", g_BankStmt[count][bsTimestamp]);
		cache_get_value_name(r, "kind", g_BankStmt[count][bsKind], 13);
		cache_get_value_name_int(r, "delta", g_BankStmt[count][bsDelta]);
		cache_get_value_name_int(r, "balance", g_BankStmt[count][bsBalance]);
		cache_get_value_name(r, "other", g_BankStmt[count][bsOther], 25);
		cache_get_value_name(r, "ref", g_BankStmt[count][bsRef], 33);
		count++;
	}
	DB_BankInfoDeliver(playerid, 2, 1, g_BankInfo[playerid][biPage], count, 0, 0, 0, rows > 15);
	return 1;
}

DB_BankResync(playerid, charId)
{
	if (!DB_BankInfoBegin(playerid, 3, charId, 0))
	{
		return 0;
	}
	DB_BankFlush(playerid);
	new query[512], name[25];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "SELECT bank FROM characters WHERE id = %d", charId);
	mysql_tquery(g_DB, query, "DB_OnBankSynced", "dsdd", playerid, name, charId, g_BankInfo[playerid][biToken]);
	return 0;
}

public DB_OnBankSynced(playerid, const name[], charId, token)
{
	if (!DB_BankInfoLive(playerid, name, charId, token, 3))
	{
		DB_BankStaleRelease(playerid, 0x10000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x10000000);
	new rows = 0, bank = 0, result = -1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		result = 1;
		cache_get_value_name_int(0, "bank", bank);
		DB_BankMirror(playerid, bank - g_BankDb[playerid]);
	}
	DB_BankInfoDeliver(playerid, 3, result, 0, bank, 0, 0, 0, 0);
	return 1;
}

DB_BankDeposit(playerid, charId, amount, channel, const opId[])
{
	new bool:ok = amount >= 1 && amount <= 0x3B9ACA00 && (channel == 0 || channel == 1);
	if (!DB_BankBegin(playerid, 1, ok, charId, amount, opId))
	{
		return 0;
	}
	DB_BankFlush(playerid);
	new query[512], name[25];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "CALL bank_deposit('%e', %d, %d, %d, %d, %d)", opId, charId, amount, channel, channel == 0 ? 20000 : 0, 0x3B9ACA00);
	mysql_tquery(g_DB, query, "DB_OnBankCalled", "dsddds", playerid, name, charId, g_BankReq[playerid][bqToken], 1, opId);
	return 0;
}

DB_BankWithdraw(playerid, charId, amount, channel, const opId[])
{
	new bool:ok = amount >= 1 && amount <= 0x3B9ACA00 && (channel == 0 || channel == 1);
	if (!DB_BankBegin(playerid, 2, ok, charId, amount, opId))
	{
		return 0;
	}
	DB_BankFlush(playerid);
	new query[512], name[25], bool:atm = channel == 0;
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "CALL bank_withdraw('%e', %d, %d, %d, %d, %d, %d)", opId, charId, amount, channel, atm ? 5000 : 50000, atm ? 15000 : 100000, 24);
	mysql_tquery(g_DB, query, "DB_OnBankCalled", "dsddds", playerid, name, charId, g_BankReq[playerid][bqToken], 2, opId);
	return 0;
}

public DB_OnBankCalled(playerid, const name[], charId, token, op, const opId[])
{
	if (!DB_BankLive(playerid, name, charId, token, op))
	{
		DB_BankCompensate(opId, op);
		DB_BankStaleRelease(playerid, 0x08000000);
		return 1;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT result, balance_after FROM bank_ops WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnBankResult", "dsddds", playerid, name, charId, token, op, opId);
	return 1;
}

public DB_OnBankResult(playerid, const name[], charId, token, op, const opId[])
{
	new rows = 0, result = -1, balance = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
		cache_get_value_name_int(0, "balance_after", balance);
	}
	if (!DB_BankLive(playerid, name, charId, token, op))
	{
		DB_BankCompensate(opId, op);
		DB_BankStaleRelease(playerid, 0x08000000);
		return 1;
	}
	DB_PendingClear(playerid, 0x08000000);
	new amount = g_BankReq[playerid][bqAmount];
	if (result == 1)
	{
		DB_BankMirror(playerid, op == 1 ? amount : -amount);
		g_BankReq[playerid][bqXferPending] = 1;
	}
	else if (result == -1)
	{
		DB_BankCompensate(opId, op);
	}
	DB_BankDeliver(playerid, op, result, amount, 0, balance, 65535);
	return 1;
}

DB_BankTransfer(playerid, toPlayerid, amount, const opId[], flag)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new bool:ok = amount >= 1 && amount <= 0x3B9ACA00 && toPlayerid >= 0 && toPlayerid < MAX_PLAYERS && toPlayerid != playerid && IsPlayerConnected(toPlayerid);
	new fromChar = gPlayer[playerid][pCharId], toChar = 0;
	if (ok)
	{
		toChar = gPlayer[toPlayerid][pCharId];
		ok = toChar > 0 && toChar != fromChar;
	}
	if (!DB_BankBegin(playerid, 3, ok, fromChar, amount, opId))
	{
		return 0;
	}
	DB_BankFlush(playerid);
	new query[512], name[25], rname[25];
	GetPlayerName(playerid, name, 25);
	GetPlayerName(toPlayerid, rname, 25);
	mysql_format(g_DB, query, 512, "CALL bank_transfer('%e', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", opId, fromChar, toChar, amount, 2, 5, 10, 25000, 50000, 100000, 0x3B9ACA00, 24, flag != 0 ? 1 : 0);
	mysql_tquery(g_DB, query, "DB_OnBankXferCalled", "dsdddsddds", playerid, name, fromChar, g_BankReq[playerid][bqToken], toPlayerid, rname, toChar, g_BankGen[toPlayerid], amount, opId);
	return 0;
}

public DB_OnBankXferCalled(playerid, const name[], charId, token, recv, const rname[], rchar, rgen, amount, const opId[])
{
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT result, fee, balance_after FROM bank_ops WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnBankXferResult", "dsdddsddds", playerid, name, charId, token, recv, rname, rchar, rgen, amount, opId);
	return 1;
}

public DB_OnBankXferResult(playerid, const name[], charId, token, recv, const rname[], rchar, rgen, amount, const opId[])
{
	new rows = 0, result = -1, fee = 0, balance = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
		cache_get_value_name_int(0, "fee", fee);
		cache_get_value_name_int(0, "balance_after", balance);
	}
	new bool:gOk = DB_BankLive(playerid, name, charId, token, 3);
	new bool:rOk = result == 1 && recv >= 0 && recv < 1000 && DB_PlayerMatches(recv, rname) && rchar > 0 && gPlayer[recv][pCharId] == rchar && g_BankGen[recv] == rgen;
	if (gOk)
	{
		DB_PendingClear(playerid, 0x08000000);
	}
	else
	{
		DB_BankStaleRelease(playerid, 0x08000000);
	}
	if (result == 1)
	{
		if (gOk)
		{
			DB_BankMirror(playerid, -(amount + fee));
		}
		if (rOk)
		{
			DB_BankMirror(recv, amount);
		}
	}
	if (!gOk && !rOk)
	{
		return 1;
	}
	DB_BankDeliver(gOk ? playerid : 65535, 3, result, amount, fee, gOk ? balance : 0, rOk ? recv : 65535);
	if (gOk && result == -1)
	{
		DB_BankResync(playerid, charId);
	}
	return 1;
}

DB_BankUndo(const opId[])
{
	if (!DB_IsReady() || !DB_BankOpIdOk(opId))
	{
		if (funcidx("Bank_OnDBUndone") != -1)
		{
			CallLocalFunction("Bank_OnDBUndone", "dsd", 65535, opId, -1);
		}
		return 0;
	}
	new query[512];
	g_BankUndoNotify++;
	mysql_format(g_DB, query, 512, "CALL bank_undo('%e', %d)", opId, 0x3B9ACA00);
	mysql_tquery(g_DB, query, "DB_OnBankUndoCalled", "sd", opId, 1);
	return 0;
}

public DB_OnBankUndoCalled(const opId[], notify)
{
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT undo_result, kind, amount FROM bank_ops WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnBankUndoResult", "sd", opId, notify);
	return 1;
}

public DB_OnBankUndoResult(const opId[], notify)
{
	new rows = 0, result = 8, msg[126], kind[13], amount = 0, owner = 65535;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		new bool:isNull = 1;
		result = -1;
		cache_is_value_name_null(0, "undo_result", isNull);
		if (!isNull)
		{
			cache_get_value_name_int(0, "undo_result", result);
		}
		cache_get_value_name(0, "kind", kind, 13);
		cache_get_value_name_int(0, "amount", amount);
	}
	if (notify)
	{
		if (g_BankUndoNotify > 0)
		{
			g_BankUndoNotify--;
		}
		if (result == 1)
		{
			for (new i = 0; i < MAX_PLAYERS; i++)
			{
				if (!IsPlayerConnected(i) || !g_BankReq[i][bqXferPending] || strcmp(g_BankReq[i][bqOpId], opId, 0, 0x7FFFFFFF) != 0)
				{
					continue;
				}
				if (g_BankReq[i][bqGen] != g_BankGen[i])
				{
					break;
				}
				DB_BankMirror(i, !strcmp(kind, "deposit", 0, 0x7FFFFFFF) ? -amount : amount);
				g_BankReq[i][bqXferPending] = 0;
				owner = i;
				break;
			}
		}
	}
	format(msg, 126, "undo banca op %s%s: esito %d%s", opId, notify ? "" : " (compensazione a sessione scaduta)", result, result == 1 ? "" : " ANOMALIA: verificare a mano");
	if (result != 1 || !notify)
	{
		Log_Write("DBBANK", msg);
	}
	if (notify && funcidx("Bank_OnDBUndone") != -1)
	{
		CallLocalFunction("Bank_OnDBUndone", "dsd", owner, opId, result);
	}
	return 1;
}

bool:DB_BankSystemKindOk(const kind[])
{
	return !strcmp(kind, "paycheck", 0, 0x7FFFFFFF) || !strcmp(kind, "job", 0, 0x7FFFFFFF) || !strcmp(kind, "faction", 0, 0x7FFFFFFF) || !strcmp(kind, "bill", 0, 0x7FFFFFFF) || !strcmp(kind, "house", 0, 0x7FFFFFFF) || !strcmp(kind, "vehicle", 0, 0x7FFFFFFF) || !strcmp(kind, "fine", 0, 0x7FFFFFFF) || !strcmp(kind, "refund", 0, 0x7FFFFFFF) || !strcmp(kind, "admin", 0, 0x7FFFFFFF);
}

DB_BankLog(const opId[], charId, otherCharId, const kind[], delta, balAfter, channel, const ref[])
{
	if (!DB_IsReady() || charId <= 0 || delta == 0 || !DB_BankOpIdOk(opId) || !DB_BankSystemKindOk(kind) || channel < 0 || channel > 2)
	{
		return 0;
	}
	if (strlen(ref) >= 33)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "INSERT IGNORE INTO bank_ledger (op_id, character_id, other_character_id, kind, delta, balance_after, channel, ref) VALUES ('%e', %d, NULLIF(%d, 0), '%e', %d, %d, %d, NULLIF('%e', ''))", opId, charId, otherCharId > 0 ? otherCharId : 0, kind, delta, balAfter, channel, ref);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

public DB_BankFailure(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	DB_PendingClear(playerid, op);
	if (op == 0x10000000)
	{
		DB_BankInfoDeliver(playerid, g_BankInfo[playerid][biInfoOp], -1, g_BankInfo[playerid][biPage], 0, 0, 0, -1, 0);
		return 1;
	}
	new reqOp = g_BankReq[playerid][bqOp];
	DB_BankDeliver(playerid, reqOp, -1, g_BankReq[playerid][bqAmount], 0, 0, 65535);
	if (reqOp == 1 || reqOp == 2)
	{
		DB_BankCompensate(g_BankReq[playerid][bqOpId], reqOp);
	}
	else if (reqOp == 3 && !(g_DBPending[playerid] & 0x10000000) && IsPlayerConnected(playerid))
	{
		DB_BankResync(playerid, gPlayer[playerid][pCharId]);
	}
	return 1;
}

public DB_BankQueryError(const callback[])
{
	if (!strcmp(callback, "DB_OnBankLimits", 0, 0x7FFFFFFF) || !strcmp(callback, "DB_OnBankStatement", 0, 0x7FFFFFFF) || !strcmp(callback, "DB_OnBankSynced", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(0x10000000);
	}
	else if (!strcmp(callback, "DB_OnBankUndo", 0, 13))
	{
		Log_Write("DBBANK", "errore query in annullamento banca: verificare a mano");
		if (g_BankUndoNotify > 0)
		{
			g_BankUndoNotify--;
			if (funcidx("Bank_OnDBUndone") != -1)
			{
				CallLocalFunction("Bank_OnDBUndone", "dsd", 65535, "", -1);
			}
		}
	}
	else
	{
		DB_FailAllPending(0x08000000);
	}
	return 1;
}
