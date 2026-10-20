new g_InvRows[MAX_PLAYERS][20][E_INV_ROW];
new g_InvBag[MAX_PLAYERS];
new g_InvReq[MAX_PLAYERS][E_INV_REQ];
new g_InvTokenSeq;
new g_InvOpSeq;
new g_InvUndoNotify;
new s_InvStackTab[11] = {0, 12, 3, 6, 10, 3, 10, 5, 1, 1, 1};

DB_InvStackMax(itemId)
{
	if (itemId < 1 || itemId > 10)
	{
		return 0;
	}
	return s_InvStackTab[itemId];
}

bool:DB_InvItemMovable(itemId)
{
	return itemId >= 1 && itemId <= 8;
}

DB_InvCapacity(bool:hasBag)
{
	return (hasBag ? 8 : 0) + 12;
}

DB_InvMakeOpId(dest[], size, charId)
{
	g_InvOpSeq++;
	format(dest, size, "%08x%08x%08x%08x", charId, g_InvOpSeq, gettime(), GetTickCount());
}

bool:DB_InvOpIdOk(const opId[])
{
	new len = strlen(opId);
	return len >= 1 && len <= 32;
}

bool:DB_InvItemOk(itemId)
{
	return itemId >= 1 && itemId <= 10;
}

bool:DB_InvAddKindOk(const kind[])
{
	return !strcmp(kind, "add", 0, 0x7FFFFFFF) || !strcmp(kind, "pickup", 0, 0x7FFFFFFF);
}

bool:DB_InvLive(playerid, const name[], charId, token, op)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 0;
	}
	if (charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		return 0;
	}
	if (!(g_DBPending[playerid] & 0x04000000))
	{
		return 0;
	}
	return g_InvReq[playerid][iqToken] == token && g_InvReq[playerid][iqOp] == op;
}

DB_InvStaleRelease(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!IsPlayerConnected(playerid))
	{
		DB_PendingClear(playerid, 0x04000000);
	}
}

DB_InvDeliver(playerid, op, result, itemId, qty, dir, houseId, recv)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	switch (op)
	{
		case 1:
		{
			if (funcidx("Inv_OnDBAdded") != -1)
			{
				CallLocalFunction("Inv_OnDBAdded", "dddd", playerid, itemId, qty, result);
			}
		}
		case 2:
		{
			if (funcidx("Inv_OnDBBought") != -1)
			{
				CallLocalFunction("Inv_OnDBBought", "dddd", playerid, itemId, qty, result);
			}
		}
		case 3:
		{
			if (funcidx("Inv_OnDBRemoved") != -1)
			{
				CallLocalFunction("Inv_OnDBRemoved", "dddd", playerid, itemId, qty, result);
			}
		}
		case 4:
		{
			if (funcidx("Inv_OnDBUsed") != -1)
			{
				CallLocalFunction("Inv_OnDBUsed", "ddd", playerid, itemId, result);
			}
		}
		case 5:
		{
			if (funcidx("Inv_OnDBDropped") != -1)
			{
				CallLocalFunction("Inv_OnDBDropped", "dddd", playerid, itemId, qty, result);
			}
		}
		case 6:
		{
			if (funcidx("Inv_OnDBTransferred") != -1)
			{
				CallLocalFunction("Inv_OnDBTransferred", "ddddd", playerid, recv, itemId, qty, result);
			}
		}
		case 7:
		{
			if (funcidx("Inv_OnDBBag") != -1)
			{
				CallLocalFunction("Inv_OnDBBag", "dd", playerid, result);
			}
		}
		case 8:
		{
			if (funcidx("Inv_OnDBFridge") != -1)
			{
				CallLocalFunction("Inv_OnDBFridge", "ddddd", playerid, houseId, dir, qty, result);
			}
		}
	}
}

bool:DB_InvBegin(playerid, op, bool:valid, charId, itemId, qty, const opId[], dir, houseId, recv, recvChar)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_DBPending[playerid] & 0x04000000)
	{
		DB_InvDeliver(playerid, op, 7, itemId, qty, dir, houseId, 65535);
		return 0;
	}
	if (!valid || charId <= 0 || gPlayer[playerid][pCharId] != charId || !DB_InvOpIdOk(opId))
	{
		DB_InvDeliver(playerid, op, 0, itemId, qty, dir, houseId, 65535);
		return 0;
	}
	g_InvReq[playerid][iqToken] = ++g_InvTokenSeq;
	g_InvReq[playerid][iqOp] = op;
	g_InvReq[playerid][iqItemId] = itemId;
	g_InvReq[playerid][iqQty] = qty;
	g_InvReq[playerid][iqDir] = dir;
	g_InvReq[playerid][iqHouseId] = houseId;
	g_InvReq[playerid][iqRecvPlayerId] = recv;
	g_InvReq[playerid][iqRecvChar] = recvChar;
	g_InvReq[playerid][iqRecvName][0] = 0;
	if (recv != 65535 && recv >= 0 && recv < 1000 && IsPlayerConnected(recv))
	{
		GetPlayerName(recv, g_InvReq[playerid][iqRecvName], 25);
	}
	g_InvReq[playerid][iqOpId][0] = 0;
	strcat(g_InvReq[playerid][iqOpId], opId, 33);
	if (!DB_IsReady())
	{
		DB_InvDeliver(playerid, op, -1, itemId, qty, dir, houseId, 65535);
		return 0;
	}
	DB_PendingSet(playerid, 0x04000000);
	return 1;
}

DB_InvRun(playerid, op, const query[])
{
	new name[25];
	GetPlayerName(playerid, name, 25);
	mysql_tquery(g_DB, query, "DB_OnInvCalled", "dsddds", playerid, name, gPlayer[playerid][pCharId], g_InvReq[playerid][iqToken], op, g_InvReq[playerid][iqOpId]);
}

DB_InvCompensate(const opId[], op)
{
	if (op != 1 && op != 2 && op != 3 && op != 5 && op != 7)
	{
		return 0;
	}
	if (!DB_IsReady() || !DB_InvOpIdOk(opId))
	{
		Log_Write("DBINV", "compensazione NON eseguita (db non pronto o op_id non valido): verificare a mano");
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "CALL inv_undo('%e')", opId);
	mysql_tquery(g_DB, query, "DB_OnInvUndoCalled", "sd", opId, 0);
}

DB_InvLoad(playerid, charId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_DBPending[playerid] & 0x02000000)
	{
		return 0;
	}
	if (!DB_IsReady() || charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		DB_DeliverFailure(playerid, 0x02000000);
		return 0;
	}
	new name[25], query[512];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "SELECT c.inv_bag, COALESCE(i.slot, 0) AS slot, COALESCE(i.item_id, 0) AS item_id, COALESCE(i.quantity, 0) AS quantity FROM characters c LEFT JOIN inventory_items i ON i.character_id = c.id WHERE c.id = %d ORDER BY i.slot", charId);
	DB_PendingSet(playerid, 0x02000000);
	mysql_tquery(g_DB, query, "DB_OnInvLoaded", "dsd", playerid, name, charId);
}

public DB_OnInvLoaded(playerid, const name[], charId)
{
	if (!DB_PlayerMatches(playerid, name) || charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		if (playerid >= 0 && playerid < MAX_PLAYERS && !IsPlayerConnected(playerid))
		{
			DB_PendingClear(playerid, 0x02000000);
		}
		return 1;
	}
	DB_PendingClear(playerid, 0x02000000);
	new rows = 0, count = 0, bag = 0, slot = 0, item = 0, qty = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		if (funcidx("Inv_OnDBLoaded") != -1)
		{
			CallLocalFunction("Inv_OnDBLoaded", "dd", playerid, -1);
		}
		return 1;
	}
	cache_get_value_name_int(0, "inv_bag", bag);
	g_InvBag[playerid] = bag != 0 ? 1 : 0;
	for (new r = 0; r < rows; r++)
	{
		item = 0;
		cache_get_value_name_int(r, "item_id", item);
		if (item <= 0)
		{
			continue;
		}
		if (count >= 20)
		{
			Log_Write("DBINV", "righe inventario oltre il massimo: troncate (anomalia)");
			break;
		}
		slot = 0;
		qty = 0;
		cache_get_value_name_int(r, "slot", slot);
		cache_get_value_name_int(r, "quantity", qty);
		g_InvRows[playerid][count][irSlot] = slot;
		g_InvRows[playerid][count][irItem] = item;
		g_InvRows[playerid][count][irQty] = qty;
		count++;
	}
	if (funcidx("Inv_OnDBLoaded") != -1)
	{
		CallLocalFunction("Inv_OnDBLoaded", "dd", playerid, count);
	}
	return 1;
}

DB_InvAddCommon(playerid, op, charId, itemId, qty, const opId[], const kind[], maxTotal)
{
	new bool:ok = DB_InvItemOk(itemId) && qty >= 1 && qty <= 1000 && maxTotal >= 0;
	if (!DB_InvBegin(playerid, op, ok, charId, itemId, qty, opId, 0, 0, 65535, 0))
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "CALL inv_add('%e', %d, %d, %d, %d, %d, %d, %d, '%e')", opId, charId, itemId, qty, DB_InvStackMax(itemId), 12, 8, maxTotal, kind);
	DB_InvRun(playerid, op, query);
}

DB_InvAdd(playerid, charId, itemId, qty, const opId[], const kind[], maxTotal)
{
	if (!DB_InvAddKindOk(kind))
	{
		DB_InvDeliver(playerid, 1, 0, itemId, qty, 0, 0, 65535);
		return 0;
	}
	DB_InvAddCommon(playerid, 1, charId, itemId, qty, opId, kind, maxTotal);
}

DB_InvBuy(playerid, charId, itemId, qty, const opId[], maxTotal)
{
	DB_InvAddCommon(playerid, 2, charId, itemId, qty, opId, "buy", maxTotal);
}

DB_InvRemoveCommon(playerid, op, charId, itemId, qty, const opId[], const kind[])
{
	new bool:ok = DB_InvItemOk(itemId) && qty >= 1 && qty <= 1000;
	if (!DB_InvBegin(playerid, op, ok, charId, itemId, qty, opId, 0, 0, 65535, 0))
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "CALL inv_remove('%e', %d, %d, %d, %d, %d, %d, '%e')", opId, charId, itemId, qty, DB_InvStackMax(itemId), 12, 8, kind);
	DB_InvRun(playerid, op, query);
}

DB_InvUse(playerid, charId, itemId, const opId[])
{
	DB_InvRemoveCommon(playerid, 4, charId, itemId, 1, opId, "use");
}

DB_InvDrop(playerid, charId, itemId, qty, const opId[])
{
	if (DB_InvItemOk(itemId) && !DB_InvItemMovable(itemId))
	{
		DB_InvDeliver(playerid, 5, 4, itemId, qty, 0, 0, 65535);
		return 0;
	}
	DB_InvRemoveCommon(playerid, 5, charId, itemId, qty, opId, "drop");
}

DB_InvSetBag(playerid, charId, const opId[])
{
	if (!DB_InvBegin(playerid, 7, 1, charId, 0, 1, opId, 0, 0, 65535, 0))
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "CALL inv_set_bag('%e', %d, %d, %d)", opId, charId, 12, 8);
	DB_InvRun(playerid, 7, query);
}

DB_FridgeMove(playerid, charId, houseId, itemId, qty, direction, const opId[], fridgeCap, carryMax)
{
	new bool:ok = houseId > 0 && itemId == 1 && qty >= 1 && qty <= 1000 && (direction == 1 || direction == 2) && fridgeCap >= 1 && fridgeCap <= 65535 && carryMax >= 0;
	if (!DB_InvBegin(playerid, 8, ok, charId, itemId, qty, opId, direction, houseId, 65535, 0))
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "CALL inv_fridge('%e', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", opId, charId, houseId, itemId, qty, direction, DB_InvStackMax(itemId), 12, 8, fridgeCap, carryMax);
	DB_InvRun(playerid, 8, query);
}

public DB_OnInvCalled(playerid, const name[], charId, token, op, const opId[])
{
	if (!DB_InvLive(playerid, name, charId, token, op))
	{
		DB_InvCompensate(opId, op);
		DB_InvStaleRelease(playerid);
		return 1;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT result FROM inventory_log WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnInvResult", "dsddds", playerid, name, charId, token, op, opId);
	return 1;
}

public DB_OnInvResult(playerid, const name[], charId, token, op, const opId[])
{
	new rows = 0, result = -1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
	}
	if (!DB_InvLive(playerid, name, charId, token, op))
	{
		DB_InvCompensate(opId, op);
		DB_InvStaleRelease(playerid);
		return 1;
	}
	DB_PendingClear(playerid, 0x04000000);
	DB_InvDeliver(playerid, op, result, g_InvReq[playerid][iqItemId], g_InvReq[playerid][iqQty], g_InvReq[playerid][iqDir], g_InvReq[playerid][iqHouseId], 65535);
	return 1;
}

DB_InvTransfer(giverId, receiverId, itemId, qty, const opId[])
{
	if (giverId < 0 || giverId >= 1000 || !IsPlayerConnected(giverId))
	{
		return 0;
	}
	new bool:ok = receiverId >= 0 && receiverId < 1000 && receiverId != giverId && IsPlayerConnected(receiverId) && DB_InvItemOk(itemId) && qty >= 1 && qty <= 1000;
	new giverChar = gPlayer[giverId][pCharId], recvChar = 0;
	if (ok)
	{
		recvChar = gPlayer[receiverId][pCharId];
		ok = recvChar > 0 && recvChar != giverChar;
	}
	if (ok && !DB_InvItemMovable(itemId))
	{
		DB_InvDeliver(giverId, 6, 4, itemId, qty, 0, 0, 65535);
		return 0;
	}
	if (!DB_InvBegin(giverId, 6, ok, giverChar, itemId, qty, opId, 0, 0, receiverId, recvChar))
	{
		return 0;
	}
	new query[512], gname[25];
	GetPlayerName(giverId, gname, 25);
	mysql_format(g_DB, query, 512, "CALL inv_transfer('%e', %d, %d, %d, %d, %d, %d, %d)", opId, giverChar, recvChar, itemId, qty, DB_InvStackMax(itemId), 12, 8);
	mysql_tquery(g_DB, query, "DB_OnInvGiveCalled", "dsdddsddds", giverId, gname, giverChar, g_InvReq[giverId][iqToken], receiverId, g_InvReq[giverId][iqRecvName], recvChar, itemId, qty, opId);
}

public DB_OnInvGiveCalled(playerid, const name[], charId, token, recv, const rname[], rchar, itemId, qty, const opId[])
{
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT result FROM inventory_log WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnInvGiveResult", "dsdddsddds", playerid, name, charId, token, recv, rname, rchar, itemId, qty, opId);
	return 1;
}

public DB_OnInvGiveResult(playerid, const name[], charId, token, recv, const rname[], rchar, itemId, qty, const opId[])
{
	new rows = 0, result = -1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
	}
	new bool:gOk = DB_InvLive(playerid, name, charId, token, 6);
	new bool:rOk = result == 1 && recv >= 0 && recv < 1000 && DB_PlayerMatches(recv, rname) && rchar > 0 && gPlayer[recv][pCharId] == rchar;
	if (gOk)
	{
		DB_PendingClear(playerid, 0x04000000);
	}
	else
	{
		DB_InvStaleRelease(playerid);
	}
	if (!gOk && !rOk)
	{
		return 1;
	}
	DB_InvDeliver(gOk ? playerid : 65535, 6, result, itemId, qty, 0, 0, rOk ? recv : 65535);
	return 1;
}

DB_InvUndo(const opId[])
{
	new query[512];
	if (!DB_IsReady() || !DB_InvOpIdOk(opId))
	{
		if (funcidx("Inv_OnDBUndone") != -1)
		{
			CallLocalFunction("Inv_OnDBUndone", "sd", opId, -1);
		}
		return 0;
	}
	g_InvUndoNotify++;
	mysql_format(g_DB, query, 512, "CALL inv_undo('%e')", opId);
	mysql_tquery(g_DB, query, "DB_OnInvUndoCalled", "sd", opId, 1);
}

public DB_OnInvUndoCalled(const opId[], notify)
{
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT undo_result FROM inventory_log WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnInvUndoResult", "sd", opId, notify);
	return 1;
}

public DB_OnInvUndoResult(const opId[], notify)
{
	new rows = 0, result = 8, msg[112];
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
	}
	format(msg, 112, "undo inventario op %s%s: esito %d%s", opId, notify ? "" : " (compensazione a sessione scaduta)", result, result == 1 ? "" : " ANOMALIA: verificare a mano");
	if (result != 1 || !notify)
	{
		Log_Write("DBINV", msg);
	}
	if (notify)
	{
		if (g_InvUndoNotify > 0)
		{
			g_InvUndoNotify--;
		}
		if (funcidx("Inv_OnDBUndone") != -1)
		{
			CallLocalFunction("Inv_OnDBUndone", "sd", opId, result);
		}
	}
	return 1;
}

public DB_InvFailure(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	DB_PendingClear(playerid, op);
	if (op == 0x02000000)
	{
		if (funcidx("Inv_OnDBLoaded") != -1)
		{
			CallLocalFunction("Inv_OnDBLoaded", "dd", playerid, -1);
		}
		return 1;
	}
	DB_InvDeliver(playerid, g_InvReq[playerid][iqOp], -1, g_InvReq[playerid][iqItemId], g_InvReq[playerid][iqQty], g_InvReq[playerid][iqDir], g_InvReq[playerid][iqHouseId], 65535);
	return 1;
}

public DB_InvQueryError(const callback[])
{
	if (!strcmp(callback, "DB_OnInvLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(0x02000000);
	}
	else if (!strcmp(callback, "DB_OnInvUndo", 0, 12))
	{
		Log_Write("DBINV", "errore query in annullamento inventario: verificare a mano");
		if (g_InvUndoNotify > 0)
		{
			g_InvUndoNotify--;
			if (funcidx("Inv_OnDBUndone") != -1)
			{
				CallLocalFunction("Inv_OnDBUndone", "sd", "", -1);
			}
		}
	}
	else
	{
		DB_FailAllPending(0x04000000);
	}
	return 1;
}
