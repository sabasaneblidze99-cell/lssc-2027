new g_LdBoard[24][E_LD_BOARD];
new g_LdHist[8][E_LD_HIST];
new g_LdDistrict[4][E_LD_DISTRICT];
new g_LdBucketSlots = 4;
new g_LdPlayer[MAX_PLAYERS][E_LD_PLAYER];
new g_LdReq[6000][E_LD_REQ];
new g_LdPending[MAX_PLAYERS];
new g_LdTokenSeq;
new g_LdOpSeq;
new g_LdBoardToken;

DB_LivingMakeOpId(dest[], size, const tag[], ref, charId)
{
	g_LdOpSeq++;
	format(dest, size, "%.2s%d-%d-%08x", tag, ref, charId, gettime() << 8 | g_LdOpSeq & 255);
	if (strlen(dest) > 32)
	{
		dest[32] = 0;
	}
}

bool:DB_LivingBusy(playerid, cat)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || cat < 0 || cat >= 6)
	{
		return false;
	}
	return (g_LdPending[playerid] & 1 << cat) != 0;
}

DB_LivingDistrictGet(districtId, E_LD_DISTRICT:field)
{
	if (districtId < 1 || districtId > 3 || _:field == 10)
	{
		return 0;
	}
	return g_LdDistrict[districtId][field];
}

DB_LivingProjectName(districtId, dest[], size)
{
	dest[0] = 0;
	if (districtId < 1 || districtId > 3)
	{
		return 0;
	}
	strcat(dest, g_LdDistrict[districtId][ldProjectName], size);
	return 0;
}

DB_LivingBucketSlots()
{
	return g_LdBucketSlots;
}

DB_LivingPlayerGet(playerid, E_LD_PLAYER:field)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	return g_LdPlayer[playerid][field];
}

DB_LivingPlayerReset(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	for (new i = 0; i < 8; i++)
	{
		g_LdPlayer[playerid][E_LD_PLAYER:i] = 0;
	}
	g_LdPlayer[playerid][lpSecsSincePayout] = -1;
	return 0;
}

DB_LdIdx(playerid, cat)
{
	return playerid * 6 + cat;
}

bool:DB_LdOpIdOk(const opId[])
{
	new len = strlen(opId);
	return len >= 1 && len <= 36;
}

DB_LdCatOf(action)
{
	switch (action)
	{
		case 1 .. 4:
		{
			return 0;
		}
		case 5, 6:
		{
			return 1;
		}
		case 7 .. 10:
		{
			return 2;
		}
		case 11:
		{
			return 3;
		}
		case 12:
		{
			return 4;
		}
	}
	return 5;
}

DB_LdCharOnline(charId)
{
	if (charId <= 0)
	{
		return 65535;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && gPlayer[i][pCharId] == charId)
		{
			return i;
		}
	}
	return 65535;
}

bool:DB_LdLive(playerid, const name[], cat, token, charId)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return false;
	}
	if (charId <= 0 || gPlayer[playerid][pCharId] != charId)
	{
		return false;
	}
	if (!(g_LdPending[playerid] & 1 << cat))
	{
		return false;
	}
	new idx = DB_LdIdx(playerid, cat);
	return g_LdReq[idx][lqToken] == token && g_LdReq[idx][lqGen] == g_BankGen[playerid];
}

DB_LdStaleRelease(playerid, cat)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!IsPlayerConnected(playerid))
	{
		g_LdPending[playerid] &= ~(1 << cat);
	}
	return 0;
}

DB_LdDeliver(playerid, action, result, ref, amount, balance, a, b, c)
{
	switch (DB_LdCatOf(action))
	{
		case 0:
		{
			if (funcidx("Living_OnDBContract") != -1)
			{
				CallLocalFunction("Living_OnDBContract", "ddddddddd", playerid, action, result, ref, amount, balance, a, b, c);
			}
		}
		case 1:
		{
			if (funcidx("Living_OnDBRental") != -1)
			{
				CallLocalFunction("Living_OnDBRental", "dddddddd", playerid, action, result, ref, amount, balance, a, b);
			}
		}
		case 2:
		{
			if (funcidx("Living_OnDBTaxi") != -1)
			{
				CallLocalFunction("Living_OnDBTaxi", "dddddddd", playerid, action, result, ref, amount, balance, a, b);
			}
		}
		case 3:
		{
			if (funcidx("Living_OnDBProject") != -1)
			{
				CallLocalFunction("Living_OnDBProject", "dddddddd", playerid, result, ref, amount, balance, a, b, c);
			}
		}
		case 4:
		{
			if (funcidx("Living_OnDBOnboard") != -1)
			{
				CallLocalFunction("Living_OnDBOnboard", "ddddddd", playerid, result, a, amount, balance, b, c);
			}
		}
		case 5:
		{
			switch (action)
			{
				case 13:
				{
					if (funcidx("Living_OnDBPlayer") != -1)
					{
						CallLocalFunction("Living_OnDBPlayer", "ddd", playerid, result, amount);
					}
				}
				case 14:
				{
					if (funcidx("Living_OnDBBoard") != -1)
					{
						CallLocalFunction("Living_OnDBBoard", "ddd", playerid, result, a);
					}
				}
				case 15:
				{
					if (funcidx("Living_OnDBHistory") != -1)
					{
						CallLocalFunction("Living_OnDBHistory", "dddd", playerid, result, ref, a);
					}
				}
			}
		}
	}
}

bool:DB_LdBegin(playerid, action, bool:valid, ref, aux, const opId[], bool:needOp)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return false;
	}
	new cat = DB_LdCatOf(action);
	if (g_LdPending[playerid] & 1 << cat)
	{
		DB_LdDeliver(playerid, action, 10, ref, 0, 0, 0, 0, 0);
		return false;
	}
	new charId = gPlayer[playerid][pCharId];
	if (!valid || charId <= 0 || (needOp && !DB_LdOpIdOk(opId)))
	{
		DB_LdDeliver(playerid, action, 9, ref, 0, 0, 0, 0, 0);
		return false;
	}
	new idx = DB_LdIdx(playerid, cat);
	g_LdReq[idx][lqToken] = ++g_LdTokenSeq;
	g_LdReq[idx][lqGen] = g_BankGen[playerid];
	g_LdReq[idx][lqAction] = action;
	g_LdReq[idx][lqCharId] = charId;
	g_LdReq[idx][lqRef] = ref;
	g_LdReq[idx][lqAux] = aux;
	g_LdReq[idx][lqOpId][0] = 0;
	strcat(g_LdReq[idx][lqOpId], opId, 37);
	if (!DB_IsReady())
	{
		DB_LdDeliver(playerid, action, -1, ref, 0, 0, 0, 0, 0);
		return false;
	}
	g_LdPending[playerid] |= 1 << cat;
	DB_BankFlush(playerid);
	return true;
}

DB_LdSend(playerid, action, const query[], other)
{
	new cat = DB_LdCatOf(action), idx = DB_LdIdx(playerid, cat), name[75], ogen = 0;
	GetPlayerName(playerid, name, 75);
	if (other != INVALID_PLAYER_ID)
	{
		ogen = g_BankGen[other];
		DB_BankFlush(other);
	}
	mysql_tquery(g_DB, query, "DB_OnLdCalled", "dsddddddds", playerid, name, cat, g_LdReq[idx][lqToken], action, g_LdReq[idx][lqCharId], g_LdReq[idx][lqAux], other, ogen, g_LdReq[idx][lqOpId]);
}

DB_LdCompensate(action, charId, aux, const opId[], bool:stale)
{
	if (!DB_IsReady() || charId <= 0)
	{
		Log_Write("DBLIVING", "compensazione NON eseguita (db non pronto): verificare a mano");
		return 0;
	}
	new query[1024];
	if (action == 5 && DB_LdOpIdOk(opId))
	{
		mysql_format(g_DB, query, 1024, "CALL sp_rental_undo('%e', %d, %d)", opId, charId, aux > 0 ? aux : 0);
		mysql_tquery(g_DB, query, "DB_OnLdCleanup", "d", charId);
	}
	if (stale && DB_LdCharOnline(charId) == 65535)
	{
		mysql_format(g_DB, query, 1024, "CALL sp_ld_char_cleanup(%d, NULL)", charId);
		mysql_tquery(g_DB, query, "DB_OnLdCleanup", "d", charId);
	}
	return 0;
}

public DB_OnLdCleanup(charId)
{
	return 1;
}

public DB_OnLdCalled(playerid, const name[], cat, token, action, charId, aux, other, otherGen, const opId[])
{
	new query[1024];
	mysql_format(g_DB, query, 1024, "SELECT result, ref, amount, balance_after, other_character_id, other_amount, other_balance_after, v1, v2, v3 FROM ld_ops WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnLdResult", "dsddddddds", playerid, name, cat, token, action, charId, aux, other, otherGen, opId);
	return 1;
}

public DB_OnLdResult(playerid, const name[], cat, token, action, charId, aux, other, otherGen, const opId[])
{
	new rows = 0, result = -1, ref = 0, amount = 0, balance = 0, ochar = 0, oamount = 0, obalance = 0, v1 = 0, v2 = 0, v3 = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
		cache_get_value_name_int(0, "ref", ref);
		cache_get_value_name_int(0, "amount", amount);
		cache_get_value_name_int(0, "balance_after", balance);
		cache_get_value_name_int(0, "other_character_id", ochar);
		cache_get_value_name_int(0, "other_amount", oamount);
		cache_get_value_name_int(0, "other_balance_after", obalance);
		cache_get_value_name_int(0, "v1", v1);
		cache_get_value_name_int(0, "v2", v2);
		cache_get_value_name_int(0, "v3", v3);
	}
	new bool:live = DB_LdLive(playerid, name, cat, token, charId);
	new bool:paid = result == 1 || result == 6;
	if (action == 8 && other >= 0 && other < MAX_PLAYERS && IsPlayerConnected(other) && ochar > 0 && gPlayer[other][pCharId] == ochar && g_BankGen[other] == otherGen)
	{
		if (paid && oamount > 0)
		{
			DB_BankMirror(other, -oamount);
		}
		if (funcidx("Living_OnDBTaxiCharged") != -1)
		{
			CallLocalFunction("Living_OnDBTaxiCharged", "ddddd", other, ref, result, paid ? oamount : 0, obalance);
		}
	}
	if (!live)
	{
		DB_LdCompensate(action, charId, aux, opId, true);
		DB_LdStaleRelease(playerid, cat);
		return 1;
	}
	g_LdPending[playerid] &= ~(1 << cat);
	if (result == -1)
	{
		if (action == 5)
		{
			DB_LivingRentUndo(playerid, charId, aux, opId);
		}
		DB_LdDeliver(playerid, action, -1, g_LdReq[DB_LdIdx(playerid, cat)][lqRef], 0, 0, 0, 0, 0);
		return 1;
	}
	switch (action)
	{
		case 4:
		{
			if (paid && amount > 0)
			{
				DB_BankMirror(playerid, amount);
			}
		}
		case 5:
		{
			if (result == 1)
			{
				DB_BankMirror(playerid, -(amount - oamount));
				if (aux > oamount)
				{
					gPlayer[playerid][pMoney] += aux - oamount;
				}
			}
			else if (aux > 0)
			{
				gPlayer[playerid][pMoney] += aux;
			}
		}
		case 6:
		{
			if (result == 1 || result == 5)
			{
				if (amount > 0)
				{
					DB_BankMirror(playerid, amount);
				}
			}
		}
		case 8:
		{
			if (paid && amount > 0)
			{
				DB_BankMirror(playerid, amount);
			}
		}
		case 11:
		{
			if (result == 1)
			{
				DB_BankMirror(playerid, -amount);
			}
		}
		case 12:
		{
			if (result == 1 && amount > 0)
			{
				DB_BankMirror(playerid, amount);
			}
		}
	}
	switch (action)
	{
		case 5:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, oamount, v1, 0);
		}
		case 6:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, v1, 0, 0);
		}
		case 8:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, v1, v3, 0);
		}
		case 7, 9, 10:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, v1, v2, 0);
		}
		case 11:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, v1, v2, v3);
		}
		case 12:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, v1, v2, v3);
		}
		default:
		{
			DB_LdDeliver(playerid, action, result, ref, amount, balance, v1, v2, v3);
		}
	}
	return 1;
}

DB_LivingRentUndo(playerid, charId, cash, const opId[])
{
	if (!DB_IsReady() || !DB_LdOpIdOk(opId))
	{
		Log_Write("DBLIVING", "undo noleggio NON eseguito (db non pronto): verificare a mano");
		return 0;
	}
	new query[1024], name[75];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 1024, "CALL sp_rental_undo('%e', %d, %d)", opId, charId, cash > 0 ? cash : 0);
	mysql_tquery(g_DB, query, "DB_OnLdUndoCalled", "dsdds", playerid, name, g_BankGen[playerid], charId, opId);
	return 0;
}

public DB_OnLdUndoCalled(playerid, const name[], gen, charId, const opId[])
{
	new query[1024];
	mysql_format(g_DB, query, 1024, "SELECT amount FROM ld_ops WHERE op_id = CONCAT('%e', '-x')", opId);
	mysql_tquery(g_DB, query, "DB_OnLdUndoResult", "dsdds", playerid, name, gen, charId, opId);
	return 1;
}

public DB_OnLdUndoResult(playerid, const name[], gen, charId, const opId[])
{
	new rows = 0, amount = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "amount", amount);
	}
	else
	{
		Log_Write("DBLIVING", "undo noleggio senza esito: verificare a mano (ld_ops <op>-x)");
	}
	if (amount > 0 && DB_PlayerMatches(playerid, name) && gPlayer[playerid][pCharId] == charId && g_BankGen[playerid] == gen)
	{
		DB_BankMirror(playerid, amount);
	}
	return 1;
}

DB_LivingCtAccept(playerid, contractId, const opId[])
{
	if (!DB_LdBegin(playerid, 1, bool:(contractId > 0), contractId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_ct_accept(%d, %d, '%e', %d)", contractId, gPlayer[playerid][pCharId], opId, gPlayer[playerid][pLevel]);
	DB_LdSend(playerid, 1, query, 65535);
	return 0;
}

DB_LivingCtPickup(playerid, contractId, const opId[])
{
	if (!DB_LdBegin(playerid, 2, bool:(contractId > 0), contractId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_ct_pickup(%d, %d, '%e')", contractId, gPlayer[playerid][pCharId], opId);
	DB_LdSend(playerid, 2, query, 65535);
	return 0;
}

DB_LivingCtRelease(playerid, contractId, const opId[], reason)
{
	if (!DB_LdBegin(playerid, 3, contractId > 0 && (reason == 0 || reason == 1), contractId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_ct_release(%d, %d, '%e', %d)", contractId, gPlayer[playerid][pCharId], opId, reason);
	DB_LdSend(playerid, 3, query, 65535);
	return 0;
}

DB_LivingCtComplete(playerid, contractId, const opId[], integrityPct, seconds)
{
	if (!DB_LdBegin(playerid, 4, contractId > 0 && integrityPct >= 0 && integrityPct <= 100 && seconds >= 0, contractId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_ct_complete(%d, %d, '%e', %d, %d)", contractId, gPlayer[playerid][pCharId], opId, integrityPct, seconds);
	DB_LdSend(playerid, 4, query, 65535);
	return 0;
}

DB_LdRentTotal(vkind, blocks)
{
	switch (vkind)
	{
		case 1:
		{
			return blocks * 30 + 200;
		}
		case 2:
		{
			return blocks * 60 + 400;
		}
		case 3:
		{
			return blocks * 70 + 450;
		}
		case 4:
		{
			return blocks * 60 + 300;
		}
	}
	return 0;
}

DB_LivingRentOpen(playerid, vkind, model, blocks, const opId[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new total = DB_LdRentTotal(vkind, blocks), cash = 0;
	new bool:ok = total > 0 && blocks >= 1 && blocks <= 3 && model >= 0 && model <= 65535;
	if (ok && !DB_LivingBusy(playerid, 1) && gPlayer[playerid][pBank] < total)
	{
		cash = total - (gPlayer[playerid][pBank] > 0 ? gPlayer[playerid][pBank] : 0);
		if (cash > gPlayer[playerid][pMoney])
		{
			cash = gPlayer[playerid][pMoney] > 0 ? gPlayer[playerid][pMoney] : 0;
		}
	}
	if (!DB_LdBegin(playerid, 5, ok, 0, cash, opId, true))
	{
		return 0;
	}
	gPlayer[playerid][pMoney] -= cash;
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_rental_open('%e', %d, %d, %d, %d, %d, %d)", opId, gPlayer[playerid][pCharId], vkind, model, blocks, cash, gPlayer[playerid][pLevel]);
	DB_LdSend(playerid, 5, query, 65535);
	return 0;
}

DB_LivingRentClose(playerid, rentalId, mode, health, const opId[])
{
	if (!DB_LdBegin(playerid, 6, rentalId > 0 && (mode == 1 || mode == 2 || mode == 3 || mode == 5), rentalId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_rental_close('%e', %d, %d, %d, %d)", opId, rentalId, gPlayer[playerid][pCharId], mode, health > 0 ? health : 0);
	DB_LdSend(playerid, 6, query, 65535);
	return 0;
}

DB_LivingTaxiStart(playerid, kind, passengerid, fromDistrict, const opId[])
{
	new pchar = 0;
	if (kind == 2 && passengerid >= 0 && passengerid < MAX_PLAYERS && IsPlayerConnected(passengerid))
	{
		pchar = gPlayer[passengerid][pCharId];
	}
	new bool:ok = ((kind == 1 && passengerid == INVALID_PLAYER_ID) || (kind == 2 && pchar > 0 && passengerid != playerid)) && fromDistrict >= 0 && fromDistrict <= 3;
	if (!DB_LdBegin(playerid, 7, ok, 0, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_taxi_start('%e', %d, %d, %d, %d, %d)", opId, kind, gPlayer[playerid][pCharId], pchar, fromDistrict, gPlayer[playerid][pLevel]);
	DB_LdSend(playerid, 7, query, 65535);
	return 0;
}

DB_LivingTaxiPay(playerid, rideId, passengerid, meters, toDistrict, bool:accept, rating, const opId[])
{
	new bool:ok = rideId > 0 && meters >= 0 && toDistrict >= 0 && toDistrict <= 3 && rating >= 0 && rating <= 5;
	if (!DB_LdBegin(playerid, 8, ok, rideId, 0, opId, true))
	{
		return 0;
	}
	new query[1024], other = INVALID_PLAYER_ID;
	if (passengerid >= 0 && passengerid < MAX_PLAYERS && IsPlayerConnected(passengerid) && gPlayer[passengerid][pCharId] > 0)
	{
		other = passengerid;
	}
	mysql_format(g_DB, query, 1024, "CALL sp_taxi_pay('%e', %d, %d, %d, %d, %d, %d)", opId, rideId, gPlayer[playerid][pCharId], meters, toDistrict, accept ? 1 : 0, rating);
	DB_LdSend(playerid, 8, query, other);
	return 0;
}

DB_LivingTaxiCancel(playerid, rideId, const opId[])
{
	if (!DB_LdBegin(playerid, 9, bool:(rideId > 0), rideId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_taxi_cancel('%e', %d, %d)", opId, rideId, gPlayer[playerid][pCharId]);
	DB_LdSend(playerid, 9, query, 65535);
	return 0;
}

DB_LivingTaxiRate(passengerid, rideId, stars, const opId[])
{
	if (!DB_LdBegin(passengerid, 10, rideId > 0 && stars >= 1 && stars <= 5, rideId, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_taxi_rate('%e', %d, %d, %d)", opId, rideId, gPlayer[passengerid][pCharId], stars);
	DB_LdSend(passengerid, 10, query, 65535);
	return 0;
}

DB_LivingProjectContrib(playerid, projectId, amount, const opId[])
{
	if (!DB_LdBegin(playerid, 11, projectId > 0 && amount > 0, projectId, 0, opId, true))
	{
		return 0;
	}
	new query[1024], played = gPlayer[playerid][pPlayedMs] / 60000;
	mysql_format(g_DB, query, 1024, "CALL sp_project_contrib('%e', %d, %d, %d, %d)", opId, gPlayer[playerid][pCharId], projectId, amount, played > 0 ? played : 0);
	DB_LdSend(playerid, 11, query, 65535);
	return 0;
}

DB_LivingOnboardStep(playerid, step, const opId[])
{
	if (!DB_LdBegin(playerid, 12, step >= 0 && step <= 5, 0, 0, opId, true))
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_onboard_step('%e', %d, %d)", opId, gPlayer[playerid][pCharId], step);
	DB_LdSend(playerid, 12, query, 65535);
	return 0;
}

DB_LivingOnboardHints(playerid, bool:off)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid) || gPlayer[playerid][pCharId] <= 0 || !DB_IsReady())
	{
		return 0;
	}
	g_LdPlayer[playerid][lpHintsOff] = off ? 1 : 0;
	gPlayer[playerid][pHintsOff] = off ? 1 : 0;
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_onboard_hints(%d, %d)", gPlayer[playerid][pCharId], off ? 1 : 0);
	mysql_tquery(g_DB, query, "DB_OnLdFire", "");
	return 0;
}

public DB_OnLdFire()
{
	return 1;
}

DB_LivingLoadPlayer(playerid, bool:cleanup)
{
	new opId[111];
	opId[0] = 0;
	if (playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid) && cleanup)
	{
		DB_LivingMakeOpId(opId, 111, "lc", 0, gPlayer[playerid][pCharId]);
	}
	if (!DB_LdBegin(playerid, 13, true, 0, 0, opId, cleanup))
	{
		return 0;
	}
	new idx = DB_LdIdx(playerid, 5), name[75], query[1024], charId = gPlayer[playerid][pCharId];
	GetPlayerName(playerid, name, 75);
	if (cleanup)
	{
		mysql_format(g_DB, query, 1024, "CALL sp_ld_char_cleanup(%d, '%e')", charId, opId);
		mysql_tquery(g_DB, query, "DB_OnLdPlayerCalled", "dsdds", playerid, name, g_LdReq[idx][lqToken], charId, opId);
	}
	else
	{
		DB_OnLdPlayerCalled(playerid, name, g_LdReq[idx][lqToken], charId, "");
	}
	return 0;
}

public DB_OnLdPlayerCalled(playerid, const name[], token, charId, const opId[])
{
	new query[1024];
	mysql_format(g_DB, query, 1024, "SELECT COALESCE((SELECT amount FROM ld_ops WHERE op_id = '%e'), 0) AS refund, ld_pro_points(c.id, 1) AS p1, ld_pro_points(c.id, 2) AS p2, ld_local_rep(c.id, 1) AS r1, ld_local_rep(c.id, 2) AS r2, ld_local_rep(c.id, 3) AS r3, COALESCE(o.step, 0) AS step, COALESCE(o.hints_off, 0) AS hoff, (o.done_at IS NOT NULL) AS odone, ld_work_earned(c.id) AS earned, (SELECT COUNT(*) FROM ct_payouts WHERE character_id = c.id AND paid_at > NOW() - INTERVAL 1 HOUR) AS ctn, COALESCE(TIMESTAMPDIFF(SECOND, (SELECT MAX(paid_at) FROM ct_payouts WHERE character_id = c.id), NOW()), -1) AS since, COALESCE((SELECT ROUND(AVG(z.rating) * 100) FROM (SELECT rating FROM taxi_rides WHERE driver_char = c.id AND rating IS NOT NULL ORDER BY id DESC LIMIT %d) z), 0) AS rating FROM characters c LEFT JOIN ld_onboarding o ON o.character_id = c.id WHERE c.id = %d", opId, 20, charId);
	mysql_tquery(g_DB, query, "DB_OnLdPlayer", "dsdd", playerid, name, token, charId);
	return 1;
}

public DB_OnLdPlayer(playerid, const name[], token, charId)
{
	if (!DB_LdLive(playerid, name, 5, token, charId))
	{
		DB_LdStaleRelease(playerid, 5);
		return 1;
	}
	g_LdPending[playerid] &= -33;
	new rows = 0, refund = 0, p1 = 0, p2 = 0, r1 = 0, r2 = 0, r3 = 0, step = 0, hoff = 0, odone = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		DB_LdDeliver(playerid, 13, 9, 0, 0, 0, 0, 0, 0);
		return 1;
	}
	cache_get_value_name_int(0, "refund", refund);
	cache_get_value_name_int(0, "p1", p1);
	cache_get_value_name_int(0, "p2", p2);
	cache_get_value_name_int(0, "r1", r1);
	cache_get_value_name_int(0, "r2", r2);
	cache_get_value_name_int(0, "r3", r3);
	cache_get_value_name_int(0, "step", step);
	cache_get_value_name_int(0, "hoff", hoff);
	cache_get_value_name_int(0, "odone", odone);
	cache_get_value_name_int(0, "earned", g_LdPlayer[playerid][lpWorkEarned]);
	cache_get_value_name_int(0, "ctn", g_LdPlayer[playerid][lpPayoutsLastHour]);
	cache_get_value_name_int(0, "since", g_LdPlayer[playerid][lpSecsSincePayout]);
	cache_get_value_name_int(0, "rating", g_LdPlayer[playerid][lpTaxiRating]);
	g_LdPlayer[playerid][lpLoaded] = 1;
	g_LdPlayer[playerid][lpOnboardStep] = step;
	g_LdPlayer[playerid][lpOnboardDone] = odone;
	g_LdPlayer[playerid][lpHintsOff] = hoff;
	if (refund > 0)
	{
		DB_BankMirror(playerid, refund);
	}
	gPlayer[playerid][pProPoints][1] = p1;
	gPlayer[playerid][pProPoints][2] = p2;
	gPlayer[playerid][pDistrictRep][1] = r1;
	gPlayer[playerid][pDistrictRep][2] = r2;
	gPlayer[playerid][pDistrictRep][3] = r3;
	gPlayer[playerid][pOnboardStep] = step;
	gPlayer[playerid][pHintsOff] = hoff;
	DB_LdDeliver(playerid, 13, 1, 0, refund, 0, 0, 0, 0);
	return 1;
}

DB_LivingBoard(playerid)
{
	new query[1024], name[75], token = 0;
	name[0] = 0;
	if (playerid == INVALID_PLAYER_ID)
	{
		if (!DB_IsReady())
		{
			if (funcidx("Living_OnDBBoard") != -1)
			{
				CallLocalFunction("Living_OnDBBoard", "ddd", 65535, -1, 0);
			}
			return 0;
		}
		token = ++g_LdBoardToken;
	}
	else
	{
		if (!DB_LdBegin(playerid, 14, true, 0, 0, "", false))
		{
			return 0;
		}
		token = g_LdReq[DB_LdIdx(playerid, 5)][lqToken];
		GetPlayerName(playerid, name, 75);
	}
	mysql_format(g_DB, query, 1024, "SELECT id, from_district, to_district, from_pt, to_pt, cargo, distance_m, reward, UNIX_TIMESTAMP(expires_at) AS exp, min_level, min_rep, min_local_rep, premium, state FROM ct_contracts WHERE state IN (0, 7) AND expires_at > NOW() ORDER BY expires_at, id LIMIT %d", 24);
	mysql_tquery(g_DB, query, "DB_OnLdBoard", "dsd", playerid, name, token);
	return 0;
}

public DB_OnLdBoard(playerid, const name[], token)
{
	if (playerid == INVALID_PLAYER_ID)
	{
		if (token != g_LdBoardToken)
		{
			return 1;
		}
	}
	else
	{
		if (!DB_LdLive(playerid, name, 5, token, gPlayer[playerid][pCharId]))
		{
			DB_LdStaleRelease(playerid, 5);
			return 1;
		}
		g_LdPending[playerid] &= -33;
	}
	new rows = 0, count = 0;
	cache_get_row_count(rows);
	for (new r = 0; r < rows && count < 24; r++)
	{
		cache_get_value_name_int(r, "id", g_LdBoard[count][lbId]);
		cache_get_value_name_int(r, "from_district", g_LdBoard[count][lbFromDistrict]);
		cache_get_value_name_int(r, "to_district", g_LdBoard[count][lbToDistrict]);
		cache_get_value_name_int(r, "from_pt", g_LdBoard[count][lbFromPoint]);
		cache_get_value_name_int(r, "to_pt", g_LdBoard[count][lbToPoint]);
		cache_get_value_name_int(r, "cargo", g_LdBoard[count][lbCargo]);
		cache_get_value_name_int(r, "distance_m", g_LdBoard[count][lbDistanceM]);
		cache_get_value_name_int(r, "reward", g_LdBoard[count][lbReward]);
		cache_get_value_name_int(r, "exp", g_LdBoard[count][lbExpiresAt]);
		cache_get_value_name_int(r, "min_level", g_LdBoard[count][lbMinLevel]);
		cache_get_value_name_int(r, "min_rep", g_LdBoard[count][lbMinRep]);
		cache_get_value_name_int(r, "min_local_rep", g_LdBoard[count][lbMinLocalRep]);
		cache_get_value_name_int(r, "premium", g_LdBoard[count][lbPremium]);
		cache_get_value_name_int(r, "state", g_LdBoard[count][lbState]);
		count++;
	}
	if (funcidx("Living_OnDBBoard") != -1)
	{
		CallLocalFunction("Living_OnDBBoard", "ddd", playerid, 1, count);
	}
	return 1;
}

DB_LivingHistory(playerid, districtId)
{
	if (!DB_LdBegin(playerid, 15, districtId >= 1 && districtId <= 3, districtId, districtId, "", false))
	{
		return 0;
	}
	new query[1024], name[75];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 1024, "SELECT type, UNIX_TIMESTAMP(created_at) AS ts, value, act_delta, (actor_char <=> %d) AS mine FROM ld_events WHERE district_id = %d ORDER BY id DESC LIMIT %d", gPlayer[playerid][pCharId], districtId, 8);
	mysql_tquery(g_DB, query, "DB_OnLdHist", "dsdd", playerid, name, g_LdReq[DB_LdIdx(playerid, 5)], districtId);
	return 0;
}

public DB_OnLdHist(playerid, const name[], token, districtId)
{
	if (!DB_LdLive(playerid, name, 5, token, gPlayer[playerid][pCharId]))
	{
		DB_LdStaleRelease(playerid, 5);
		return 1;
	}
	g_LdPending[playerid] &= -33;
	new rows = 0, count = 0;
	cache_get_row_count(rows);
	for (new r = 0; r < rows && count < 8; r++)
	{
		cache_get_value_name_int(r, "type", g_LdHist[count][lhType]);
		cache_get_value_name_int(r, "ts", g_LdHist[count][lhTimestamp]);
		cache_get_value_name_int(r, "value", g_LdHist[count][lhValue]);
		cache_get_value_name_int(r, "act_delta", g_LdHist[count][lhActDelta]);
		cache_get_value_name_int(r, "mine", g_LdHist[count][lhMine]);
		count++;
	}
	DB_LdDeliver(playerid, 15, 1, districtId, 0, 0, count, 0, 0);
	return 1;
}

DB_LivingLoadDistricts()
{
	if (!DB_IsReady())
	{
		if (funcidx("Living_OnDBDistricts") != -1)
		{
			CallLocalFunction("Living_OnDBDistricts", "dd", -1, 0);
		}
		return 0;
	}
	mysql_tquery(g_DB, "SELECT d.id, d.activity, d.security, d.crime, COALESCE(UNIX_TIMESTAMP(d.fest_until), 0) AS fest, UNIX_TIMESTAMP(d.last_tick_at) AS lt, COALESCE(p.id, 0) AS pid, COALESCE(p.name, '') AS pname, COALESCE(p.goal, 0) AS goal, COALESCE(FLOOR(p.fund), 0) AS fund, COALESCE(UNIX_TIMESTAMP(p.ends_at), 0) AS pend, ct_bucket_slots() AS slots FROM ld_districts d LEFT JOIN ld_projects p ON p.active_district = d.id ORDER BY d.id", "DB_OnLdDistricts", "");
	return 0;
}

public DB_OnLdDistricts()
{
	new rows = 0, count = 0, id = 0;
	cache_get_row_count(rows);
	for (new r = 0; r < rows; r++)
	{
		cache_get_value_name_int(r, "id", id);
		if (id < 1 || id > 3)
		{
			continue;
		}
		g_LdDistrict[id][ldLoaded] = 1;
		cache_get_value_name_int(r, "activity", g_LdDistrict[id][ldActivity]);
		cache_get_value_name_int(r, "security", g_LdDistrict[id][ldSecurity]);
		cache_get_value_name_int(r, "crime", g_LdDistrict[id][ldCrime]);
		cache_get_value_name_int(r, "fest", g_LdDistrict[id][ldFestUntil]);
		cache_get_value_name_int(r, "lt", g_LdDistrict[id][ldLastTickAt]);
		cache_get_value_name_int(r, "pid", g_LdDistrict[id][ldProjectId]);
		cache_get_value_name(r, "pname", g_LdDistrict[id][ldProjectName], 65);
		cache_get_value_name_int(r, "goal", g_LdDistrict[id][ldProjectGoal]);
		cache_get_value_name_int(r, "fund", g_LdDistrict[id][ldProjectFund]);
		cache_get_value_name_int(r, "pend", g_LdDistrict[id][ldProjectEndsAt]);
		cache_get_value_name_int(r, "slots", g_LdBucketSlots);
		count++;
	}
	if (funcidx("Living_OnDBDistricts") != -1)
	{
		CallLocalFunction("Living_OnDBDistricts", "dd", count > 0 ? 1 : -1, count);
	}
	return 1;
}

DB_LivingBoot()
{
	if (!DB_IsReady())
	{
		if (funcidx("Living_OnDBBoot") != -1)
		{
			CallLocalFunction("Living_OnDBBoot", "d", -1);
		}
		return 0;
	}
	mysql_tquery(g_DB, "CALL sp_ld_boot()", "DB_OnLdBoot", "");
	return 0;
}

public DB_OnLdBoot()
{
	if (funcidx("Living_OnDBBoot") != -1)
	{
		CallLocalFunction("Living_OnDBBoot", "d", 1);
	}
	DB_LivingLoadDistricts();
	return 1;
}

DB_LivingTick()
{
	if (!DB_IsReady())
	{
		return 0;
	}
	mysql_tquery(g_DB, "CALL sp_ld_tick_all()", "DB_OnLdFire", "");
	mysql_tquery(g_DB, "CALL sp_ct_sweep()", "DB_OnLdTicked", "");
	return 0;
}

public DB_OnLdTicked()
{
	DB_LivingLoadDistricts();
	return 1;
}

DB_LivingGenerate(bucket, slot, fromPt, toPt, fromD, toD, cargo, meters, minLevel, minRep, minLocalRep, premium)
{
	if (!DB_IsReady() || bucket < 0 || slot < 0)
	{
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_ct_generate(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", bucket, slot, 1, fromPt, toPt, fromD, toD, cargo, meters, minLevel, minRep, minLocalRep, premium ? 1 : 0);
	mysql_tquery(g_DB, query, "DB_OnLdFire", "");
	return 0;
}

DB_LivingEmit(const opId[], districtId, type, actorChar, value)
{
	if (!DB_IsReady() || !DB_LdOpIdOk(opId))
	{
		if (funcidx("Living_OnDBEmit") != -1)
		{
			CallLocalFunction("Living_OnDBEmit", "ddddd", DB_IsReady() ? 9 : -1, districtId, 0, 0, 0);
		}
		return 0;
	}
	new query[1024];
	mysql_format(g_DB, query, 1024, "CALL sp_ld_emit('%e', %d, %d, %d, %d)", opId, districtId, type, actorChar > 0 ? actorChar : 0, value);
	mysql_tquery(g_DB, query, "DB_OnLdGlobalCalled", "ddss", 1, 65535, "", opId);
	return 0;
}

DB_LivingAdminSet(playerid, const opId[], districtId, activity, security, crime)
{
	if (!DB_IsReady() || !DB_LdOpIdOk(opId) || playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		if (funcidx("Living_OnDBAdminSet") != -1)
		{
			CallLocalFunction("Living_OnDBAdminSet", "dddddd", playerid, DB_IsReady() ? 9 : -1, districtId, 0, 0, 0);
		}
		return 0;
	}
	new query[1024], name[75];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 1024, "CALL sp_ld_admin_set('%e', %d, %d, %d, %d, %d)", opId, districtId, activity, security, crime, gPlayer[playerid][pCharId]);
	mysql_tquery(g_DB, query, "DB_OnLdGlobalCalled", "ddss", 2, playerid, name, opId);
	return 0;
}

public DB_OnLdGlobalCalled(kind, playerid, const name[], const opId[])
{
	new query[1024];
	mysql_format(g_DB, query, 1024, "SELECT result, ref, v1, v2, v3 FROM ld_ops WHERE op_id = '%e'", opId);
	mysql_tquery(g_DB, query, "DB_OnLdGlobalResult", "ddss", kind, playerid, name, opId);
	return 1;
}

public DB_OnLdGlobalResult(kind, playerid, const name[], const opId[])
{
	new rows = 0, result = -1, ref = 0, v1 = 0, v2 = 0, v3 = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "result", result);
		cache_get_value_name_int(0, "ref", ref);
		cache_get_value_name_int(0, "v1", v1);
		cache_get_value_name_int(0, "v2", v2);
		cache_get_value_name_int(0, "v3", v3);
	}
	if (result == 1 && ref >= 1 && ref <= 3)
	{
		g_LdDistrict[ref][ldActivity] = v1;
		g_LdDistrict[ref][ldSecurity] = v2;
		g_LdDistrict[ref][ldCrime] = v3;
	}
	if (kind == 1)
	{
		if (funcidx("Living_OnDBEmit") != -1)
		{
			CallLocalFunction("Living_OnDBEmit", "ddddd", result, ref, v1, v2, v3);
		}
	}
	else if (DB_PlayerMatches(playerid, name) && funcidx("Living_OnDBAdminSet") != -1)
	{
		CallLocalFunction("Living_OnDBAdminSet", "dddddd", playerid, result, ref, v1, v2, v3);
	}
	return 1;
}

DB_LdCfgAdd(query[], size, const key[], value)
{
	new part[192];
	mysql_format(g_DB, part, 192, "%s('%e', %d)", query[0] ? "," : "", key, value);
	if (strlen(query) + strlen(part) + 48 >= size)
	{
		DB_LdCfgFlush(query, size);
		mysql_format(g_DB, part, 192, "('%e', %d)", key, value);
	}
	strcat(query, part, size);
}

DB_LdCfgFlush(query[], size)
{
	#pragma unused size
	if (!query[0])
	{
		return 0;
	}
	new full[1088];
	format(full, 1088, "INSERT INTO ld_config (k, v) VALUES %s ON DUPLICATE KEY UPDATE v = VALUES(v)", query);
	mysql_tquery(g_DB, full, "DB_OnLdFire", "");
	query[0] = 0;
	return 0;
}

DB_LivingPushConfig()
{
	if (!DB_IsReady())
	{
		return 0;
	}
	new q[1024];
	q[0] = 0;
	DB_LdCfgAdd(q, 1024, "MONEY_MAX", 0x3B9ACA00);
	DB_LdCfgAdd(q, 1024, "LD_WORK_HOURLY_CAP", 2800);
	DB_LdCfgAdd(q, 1024, "LD_MIN_LEVEL", 2);
	DB_LdCfgAdd(q, 1024, "LD_TICK_MS", 600000);
	DB_LdCfgAdd(q, 1024, "LD_TICK_CATCHUP_MAX", 144);
	DB_LdCfgAdd(q, 1024, "LD_TICK_DIV", 20);
	DB_LdCfgAdd(q, 1024, "LD_TICK_MIN_STEP", 1);
	DB_LdCfgAdd(q, 1024, "LD_ACT_MAX", 1000);
	DB_LdCfgAdd(q, 1024, "LD_SEC_MAX", 100);
	DB_LdCfgAdd(q, 1024, "LD_CRIME_MAX", 100);
	DB_LdCfgAdd(q, 1024, "LD_D1_ACT_BASE", 500);
	DB_LdCfgAdd(q, 1024, "LD_D1_SEC_BASE", 45);
	DB_LdCfgAdd(q, 1024, "LD_D1_CRIME_BASE", 50);
	DB_LdCfgAdd(q, 1024, "LD_D2_ACT_BASE", 450);
	DB_LdCfgAdd(q, 1024, "LD_D2_SEC_BASE", 35);
	DB_LdCfgAdd(q, 1024, "LD_D2_CRIME_BASE", 60);
	DB_LdCfgAdd(q, 1024, "LD_D3_ACT_BASE", 500);
	DB_LdCfgAdd(q, 1024, "LD_D3_SEC_BASE", 40);
	DB_LdCfgAdd(q, 1024, "LD_D3_CRIME_BASE", 55);
	DB_LdCfgAdd(q, 1024, "LD_ACT_DELIVERY_DEST", 6);
	DB_LdCfgAdd(q, 1024, "LD_ACT_DELIVERY_ORIG", 3);
	DB_LdCfgAdd(q, 1024, "LD_ACT_RIDE", 2);
	DB_LdCfgAdd(q, 1024, "LD_ACT_FAIL", -3);
	DB_LdCfgAdd(q, 1024, "LD_ACT_HOURLY_GAIN_CAP", 60);
	DB_LdCfgAdd(q, 1024, "LD_CRIME_EVENT", 5);
	DB_LdCfgAdd(q, 1024, "LD_SEC_CRIME_EVENT", -2);
	DB_LdCfgAdd(q, 1024, "LD_SEC_TICK_CRIME_AT", 40);
	DB_LdCfgAdd(q, 1024, "LD_ACT_MULT_MIN_PCT", 90);
	DB_LdCfgAdd(q, 1024, "LD_ACT_MULT_MAX_PCT", 110);
	DB_LdCfgAdd(q, 1024, "LD_CRIME_HAZARD_AT", 70);
	DB_LdCfgAdd(q, 1024, "LD_CRIME_HAZARD_PCT", 10);
	DB_LdCfgAdd(q, 1024, "LD_DELIV_HOURLY_PER_CHAR", 3);
	DB_LdCfgAdd(q, 1024, "LD_PAIR_LOCK_S", 1800);
	DB_LdCfgAdd(q, 1024, "LD_LOCALREP_MIN", -50);
	DB_LdCfgAdd(q, 1024, "LD_LOCALREP_MAX", 100);
	DB_LdCfgAdd(q, 1024, "LD_LOCALREP_PER_EVENT", 2);
	DB_LdCfgAdd(q, 1024, "LD_REP_RECOVER_PER_EVENT", 4);
	DB_LdCfgAdd(q, 1024, "LD_LOCALREP_DAY_CAP", 10);
	DB_LdCfgAdd(q, 1024, "LD_PREMIUM_LOCALREP", 20);
	DB_LdCfgAdd(q, 1024, "LD_PREMIUM_PCT", 20);
	DB_LdCfgAdd(q, 1024, "LD_REP_KNOWN", 10);
	DB_LdCfgAdd(q, 1024, "LD_REP_HOME", 40);
	DB_LdCfgAdd(q, 1024, "LD_DISC_KNOWN_PCT", 5);
	DB_LdCfgAdd(q, 1024, "LD_DISC_HOME_PCT", 10);
	DB_LdCfgAdd(q, 1024, "LD_PROSP_LOW", 350);
	DB_LdCfgAdd(q, 1024, "LD_PROSP_HIGH", 650);
	DB_LdCfgAdd(q, 1024, "LD_PROSP_BOARD_DELTA", 1);
	DB_LdCfgAdd(q, 1024, "LD_PROSP_BOARD_MIN", 3);
	DB_LdCfgAdd(q, 1024, "LD_PROSP_LOW_BONUS_PCT", 10);
	DB_LdCfgAdd(q, 1024, "CT_BUCKET_S", 1800);
	DB_LdCfgAdd(q, 1024, "CT_PER_BUCKET", 4);
	DB_LdCfgAdd(q, 1024, "CT_EXPIRE_EXTRA_S", 3600);
	DB_LdCfgAdd(q, 1024, "CT_ACCEPT_CD_MS", 20000);
	DB_LdCfgAdd(q, 1024, "CT_DONE_CD_MS", 90000);
	DB_LdCfgAdd(q, 1024, "CT_HOURLY_MAX", 8);
	DB_LdCfgAdd(q, 1024, "CT_REACCEPT_LOCK_S", 600);
	DB_LdCfgAdd(q, 1024, "CT_PREMIUM_PROREP", 100);
	DB_LdCfgAdd(q, 1024, "FRT_BASE", 180);
	DB_LdCfgAdd(q, 1024, "FRT_PER_M_X100", 14);
	DB_LdCfgAdd(q, 1024, "FRT_MIN_DIST", 300);
	DB_LdCfgAdd(q, 1024, "FRT_CARGO_TYPES", 3);
	DB_LdCfgAdd(q, 1024, "FRT_MIN_SECONDS", 25);
	DB_LdCfgAdd(q, 1024, "FRT_MIN_MPS", 40);
	DB_LdCfgAdd(q, 1024, "FRT_TIME_BASE_S", 120);
	DB_LdCfgAdd(q, 1024, "FRT_TIME_MPS_X10", 60);
	DB_LdCfgAdd(q, 1024, "FRT_INTEG_OK", 70);
	DB_LdCfgAdd(q, 1024, "FRT_INTEG_PENALTY_PCT", 25);
	DB_LdCfgAdd(q, 1024, "FRT_INTEG_FAIL", 30);
	DB_LdCfgAdd(q, 1024, "FRT_ABANDON_REP", -5);
	DB_LdCfgAdd(q, 1024, "PRO_REP_DELIVERY", 12);
	DB_LdCfgAdd(q, 1024, "PRO_REP_ONTIME", 4);
	DB_LdCfgAdd(q, 1024, "PRO_REP_FAIL", -15);
	DB_LdCfgAdd(q, 1024, "PRO_REP_RIDE", 8);
	DB_LdCfgAdd(q, 1024, "PRO_REP_DAY_CAP", 60);
	DB_LdCfgAdd(q, 1024, "PRO_TIER1", 100);
	DB_LdCfgAdd(q, 1024, "PRO_TIER2", 300);
	DB_LdCfgAdd(q, 1024, "PRO_TIER3", 600);
	DB_LdCfgAdd(q, 1024, "PRO_BONUS1_PCT", 5);
	DB_LdCfgAdd(q, 1024, "PRO_BONUS2_PCT", 10);
	DB_LdCfgAdd(q, 1024, "PRO_BONUS3_PCT", 15);
	DB_LdCfgAdd(q, 1024, "RENT_MIN_LEVEL", 1);
	DB_LdCfgAdd(q, 1024, "RENT_BLOCK_MIN", 20);
	DB_LdCfgAdd(q, 1024, "RENT_BLOCKS_MAX", 3);
	DB_LdCfgAdd(q, 1024, "RENT_GRACE_MS", 300000);
	DB_LdCfgAdd(q, 1024, "RENT_HEALTH_ZERO", 300);
	DB_LdCfgAdd(q, 1024, "RENT_HEALTH_FULL", 1000);
	DB_LdCfgAdd(q, 1024, "RENT_FEE_FAGGIO", 30);
	DB_LdCfgAdd(q, 1024, "RENT_FEE_PREVION", 60);
	DB_LdCfgAdd(q, 1024, "RENT_FEE_MANANA", 70);
	DB_LdCfgAdd(q, 1024, "RENT_DEP_FAGGIO", 200);
	DB_LdCfgAdd(q, 1024, "RENT_DEP_PREVION", 400);
	DB_LdCfgAdd(q, 1024, "RENT_DEP_MANANA", 450);
	DB_LdCfgAdd(q, 1024, "TAXI_MIN_LEVEL", 2);
	DB_LdCfgAdd(q, 1024, "TAXI_DEP", 300);
	DB_LdCfgAdd(q, 1024, "TAXI_RENT_HOUR", 60);
	DB_LdCfgAdd(q, 1024, "TAXI_BASE", 15);
	DB_LdCfgAdd(q, 1024, "TAXI_PER_M_X100", 12);
	DB_LdCfgAdd(q, 1024, "TAXI_TAX_PCT", 10);
	DB_LdCfgAdd(q, 1024, "TAXI_NPC_HOURLY", 10);
	DB_LdCfgAdd(q, 1024, "TAXI_PAIR_HOURLY", 3);
	DB_LdCfgAdd(q, 1024, "TAXI_RATING_WINDOW", 20);
	DB_LdCfgAdd(q, 1024, "PRJ_GOAL", 6000);
	DB_LdCfgAdd(q, 1024, "PRJ_DAYS", 7);
	DB_LdCfgAdd(q, 1024, "PRJ_FUND_PCT", 8);
	DB_LdCfgAdd(q, 1024, "PRJ_FUND_RIDE_X10", 15);
	DB_LdCfgAdd(q, 1024, "PRJ_DON_MIN", 50);
	DB_LdCfgAdd(q, 1024, "PRJ_DON_MAX", 500);
	DB_LdCfgAdd(q, 1024, "PRJ_DON_DAY", 1000);
	DB_LdCfgAdd(q, 1024, "PRJ_DON_REP_PER", 100);
	DB_LdCfgAdd(q, 1024, "PRJ_DON_MIN_PLAYED_MIN", 60);
	DB_LdCfgAdd(q, 1024, "PRJ_DAY_CAP", 15);
	DB_LdCfgAdd(q, 1024, "PRJ_REP_DONE", 5);
	DB_LdCfgAdd(q, 1024, "PRJ_REP_MIN_POINTS", 3);
	DB_LdCfgAdd(q, 1024, "PRJ_ACT_BONUS", 100);
	DB_LdCfgAdd(q, 1024, "PRJ_FEST_H", 72);
	DB_LdCfgAdd(q, 1024, "PRJ_FEST_ACT_MIN", 600);
	DB_LdCfgAdd(q, 1024, "ONB_STEPS", 5);
	DB_LdCfgAdd(q, 1024, "ONB_REWARD", 120);
	DB_LdCfgAdd(q, 1024, "ONB_EXP", 1);
	DB_LdCfgFlush(q, 1024);
	return 0;
}

public DB_LivingQueryError(const callback[])
{
	if (!strcmp(callback, "DB_OnLdDistricts", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnLdTicked", false, 0x7FFFFFFF))
	{
		if (funcidx("Living_OnDBDistricts") != -1)
		{
			CallLocalFunction("Living_OnDBDistricts", "dd", -1, 0);
		}
		return 1;
	}
	if (!strcmp(callback, "DB_OnLdBoot", false, 0x7FFFFFFF))
	{
		if (funcidx("Living_OnDBBoot") != -1)
		{
			CallLocalFunction("Living_OnDBBoot", "d", -1);
		}
		return 1;
	}
	if (!strcmp(callback, "DB_OnLdGlobal", false, 13) || !strcmp(callback, "DB_OnLdFire", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnLdCleanup", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnLdUndo", false, 11))
	{
		Log_Write("DBLIVING", "errore query di sistema/compensazione living: verificare a mano");
		return 1;
	}
	if (!strcmp(callback, "DB_OnLdBoard", false, 0x7FFFFFFF))
	{
		if (funcidx("Living_OnDBBoard") != -1)
		{
			CallLocalFunction("Living_OnDBBoard", "ddd", 65535, -1, 0);
		}
	}
	new bool:info = !strcmp(callback, "DB_OnLdPlayer", false, 13) || !strcmp(callback, "DB_OnLdBoard", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnLdHist", false, 0x7FFFFFFF);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		for (new cat = 0; cat < 6; cat++)
		{
			if (!(g_LdPending[i] & 1 << cat))
			{
				continue;
			}
			if (info != (cat == 5))
			{
				continue;
			}
			g_LdPending[i] &= ~(1 << cat);
			if (!IsPlayerConnected(i))
			{
				continue;
			}
			new idx = DB_LdIdx(i, cat), action = g_LdReq[idx][lqAction];
			if (action == 5)
			{
				DB_LivingRentUndo(i, g_LdReq[idx][lqCharId], g_LdReq[idx][lqAux], g_LdReq[idx][lqOpId]);
			}
			DB_LdDeliver(i, action, -1, action == 15 ? g_LdReq[idx][lqAux] : g_LdReq[idx][lqRef], 0, 0, 0, 0, 0);
		}
	}
	return 1;
}
