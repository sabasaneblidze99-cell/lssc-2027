#define HOUSE_MAX (400)          // case caricate (10 di LSSC + 336 di Arizona + margine)
new g_HouseRows[HOUSE_MAX][E_HOUSE_ROW];
new bool:g_HsIntCols;              // colonne int_* presenti (migrazione 015)
new g_HouseCount;
new bool:g_HouseBootLoad;
new g_HouseBills[MAX_PLAYERS][8][E_HOUSE_BILL];
new g_HouseKeyList[MAX_PLAYERS][3][E_HOUSE_KEY];
new g_HouseReq[MAX_PLAYERS][9][E_HOUSE_REQ];
new Float:s_HouseExt[10][4] = {
	{2514.3, -1672.0, 14.1, 180.0},
	{2495.3, -1688.9, 14.0},
	{2465.0, -1698.0, 13.5, 90.0},
	{2065.0, -1703.5, 14.1, 180.0},
	{2032.0, -1665.0, 14.0, 90.0},
	{2120.0, -1920.0, 13.6},
	{2150.0, -1250.0, 23.9, 270.0},
	{2306.0, -1190.0, 27.0, 180.0},
	{2654.0, -1470.0, 30.4, 90.0},
	{2720.0, -1290.0, 59.6, 180.0}
};
new s_HouseIntIdx[10] = {3, 5, 2, 0, 1, 3, 4, 6, 7, 7};
new s_HouseIntTier[8] = {0, 0, 1, 1, 2, 2, 2, 3};
new s_HouseTierPrice[4] = {ECON_HOUSE_T0, ECON_HOUSE_T1, ECON_HOUSE_T2, ECON_HOUSE_T3};

bool:DB_HouseSessionOk(playerid, const name[], ownerId)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return false;
	}
	return ownerId > 0 && gPlayer[playerid][pCharId] == ownerId;
}

DB_HouseStaleRelease(playerid, op)
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

DB_HouseOpSlot(op)
{
	new bit = 512;
	for (new i = 0; i < 9; i++, bit <<= 1)
	{
		if (op == bit)
		{
			return i;
		}
	}
	return -1;
}

bool:DB_HouseBegin(playerid, op, bool:valid, houseId, targetId, kind)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return false;
	}
	if (g_DBPending[playerid] & op)
	{
		DB_HouseDeliver(playerid, op, 0, houseId, targetId, kind);
		return false;
	}
	new slot = DB_HouseOpSlot(op);
	if (slot >= 0)
	{
		g_HouseReq[playerid][slot][hqHouseId] = houseId;
		g_HouseReq[playerid][slot][hqTargetId] = targetId;
		g_HouseReq[playerid][slot][hqKind] = kind;
	}
	if (!DB_IsReady() || !valid)
	{
		DB_DeliverFailure(playerid, op);
		return false;
	}
	DB_PendingSet(playerid, op);
	return true;
}

DB_HouseDeliver(playerid, op, result, h, t, k)
{
	switch (op)
	{
		case 512:
		{
			if (funcidx("House_OnDBBought") != -1)
			{
				CallLocalFunction("House_OnDBBought", "ddd", playerid, h, result);
			}
		}
		case 1024:
		{
			if (funcidx("House_OnDBSold") != -1)
			{
				CallLocalFunction("House_OnDBSold", "ddd", playerid, h, result);
			}
		}
		case 2048:
		{
			if (funcidx("House_OnDBKeyAdded") != -1)
			{
				CallLocalFunction("House_OnDBKeyAdded", "dddd", playerid, h, t, result);
			}
		}
		case 4096:
		{
			if (funcidx("House_OnDBKeyRemoved") != -1)
			{
				CallLocalFunction("House_OnDBKeyRemoved", "dddd", playerid, h, t, result);
			}
		}
		case 8192:
		{
			if (funcidx("House_OnDBKeysLoaded") != -1)
			{
				CallLocalFunction("House_OnDBKeysLoaded", "ddd", playerid, h, -1);
			}
		}
		case 16384:
		{
			if (funcidx("House_OnDBBillsLoaded") != -1)
			{
				CallLocalFunction("House_OnDBBillsLoaded", "ddd", playerid, h, -1);
			}
		}
		case 32768:
		{
			if (funcidx("House_OnDBBillsPaid") != -1)
			{
				CallLocalFunction("House_OnDBBillsPaid", "ddddd", playerid, h, k, result, 0);
			}
		}
		case 65536:
		{
			if (funcidx("House_OnDBFoodLoaded") != -1)
			{
				CallLocalFunction("House_OnDBFoodLoaded", "ddd", playerid, -1, -1);
			}
		}
		case 131072:
		{
			if (funcidx("House_OnDBStored") != -1)
			{
				CallLocalFunction("House_OnDBStored", "ddd", playerid, h, result);
			}
		}
	}
}

public DB_HouseFailure(playerid, op, result)
{
	new slot = DB_HouseOpSlot(op), h = 0, t = 0, k = 0;
	if (playerid >= 0 && playerid < MAX_PLAYERS && slot >= 0)
	{
		h = g_HouseReq[playerid][slot][hqHouseId];
		t = g_HouseReq[playerid][slot][hqTargetId];
		k = g_HouseReq[playerid][slot][hqKind];
	}
	DB_HouseDeliver(playerid, op, result, h, t, k);
	return 1;
}

public DB_HouseGlobalFailure(const callback[])
{
	if (!strcmp(callback, "DB_OnHouseSeeded", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnHouseSeedUtil", false, 0x7FFFFFFF))
	{
		g_HouseBootLoad = false;
		g_HouseCount = 0;
		if (funcidx("House_OnDBSeeded") != -1)
		{
			CallLocalFunction("House_OnDBSeeded", "d", -1);
		}
		if (funcidx("House_OnDBLoaded") != -1)
		{
			CallLocalFunction("House_OnDBLoaded", "d", -1);
		}
	}
	else if (!strcmp(callback, "DB_OnHousesLoaded", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnHouseKeysAllLoaded", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnHouseLoadUtil", false, 0x7FFFFFFF))
	{
		g_HouseCount = 0;
		if (funcidx("House_OnDBLoaded") != -1)
		{
			CallLocalFunction("House_OnDBLoaded", "d", -1);
		}
	}
	else if (!strcmp(callback, "DB_OnHouseBillAdded", false, 0x7FFFFFFF))
	{
		if (funcidx("House_OnDBBillAdded") != -1)
		{
			CallLocalFunction("House_OnDBBillAdded", "dddd", 0, -1, -1, 0);
		}
	}
	else if (!strcmp(callback, "DB_OnHouseBillsAgedStep", false, 0x7FFFFFFF) || !strcmp(callback, "DB_OnHouseBillsAged", false, 0x7FFFFFFF))
	{
		if (funcidx("House_OnDBBillsAged") != -1)
		{
			CallLocalFunction("House_OnDBBillsAged", "dd", 0, -1);
		}
	}
	return 1;
}

DB_HouseCompensate(what, ownerId, houseId, kind, token, bool:recut)
{
	if (!DB_IsReady() || ownerId <= 0 || houseId <= 0)
	{
		Log_Write("DBHOUSE", "compensazione casa NON eseguita (db non pronto o id non valido): verificare a mano");
		return 0;
	}
	new query[768];
	switch (what)
	{
		case 1:
		{
			mysql_format(g_DB, query, 768, "UPDATE houses SET owner_id = NULL, price_paid = NULL, locked = 1 WHERE id = %d AND owner_id = %d", houseId, ownerId);
		}
		case 2:
		{
			mysql_format(g_DB, query, 768, "UPDATE houses SET owner_id = %d WHERE id = %d AND owner_id IS NULL", ownerId, houseId);
		}
		default:
		{
			mysql_format(g_DB, query, 768, "UPDATE house_bills b JOIN houses h ON h.id = b.house_id SET b.paid = 0, b.paid_at = NULL, b.pay_id = NULL WHERE b.house_id = %d AND b.kind = %d AND b.paid = 1 AND b.paid_at = FROM_UNIXTIME(%d) AND h.owner_id = %d AND b.pay_id = (SELECT m FROM (SELECT MAX(pay_id) AS m FROM house_bills WHERE house_id = %d AND kind = %d AND paid = 1) t)", houseId, kind, token, ownerId, houseId, kind);
			if (recut)
			{
				DB_HouseSetCut(houseId, kind, true);
			}
		}
	}
	mysql_tquery(g_DB, query, "DB_OnHouseCompensated", "ddd", what, ownerId, houseId);
	return 0;
}

public DB_OnHouseCompensated(what, ownerId, houseId)
{
	new msg[384];
	format(msg, 384, "%s casa %d (owner %d) a sessione scaduta: righe=%d%s", what == 1 ? "rollback acquisto" : (what == 2 ? "ripristino vendita" : "annullo pagamento bollette"), houseId, ownerId, cache_affected_rows(), cache_affected_rows() >= 1 ? "" : " ANOMALIA: verificare a mano");
	Log_Write("DBHOUSE", msg);
	if (what != 3 && funcidx("House_OnDBCompensated") != -1)
	{
		CallLocalFunction("House_OnDBCompensated", "d", houseId);
	}
	return 1;
}

DB_HouseResetState(houseId)
{
	if (!DB_IsReady() || houseId <= 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "INSERT INTO house_utilities (house_id) VALUES (%d) ON DUPLICATE KEY UPDATE luce_on = 1, gas_on = 1, luce_cut = 0, gas_cut = 0, luce_min = 0, gas_min = 0, cycle_min = 0, fridge_units = 0, fridge_off_min = 0", houseId);
	mysql_tquery(g_DB, query, "", "");
	mysql_format(g_DB, query, 768, "DELETE FROM house_keys WHERE house_id = %d", houseId);
	mysql_tquery(g_DB, query, "", "");
	mysql_format(g_DB, query, 768, "DELETE FROM house_bills WHERE house_id = %d AND paid = 0", houseId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_SeedHouses()
{
	if (!DB_IsReady())
	{
		g_HouseBootLoad = false;
		g_HouseCount = 0;
		if (funcidx("House_OnDBSeeded") != -1)
		{
			CallLocalFunction("House_OnDBSeeded", "d", -1);
		}
		if (funcidx("House_OnDBLoaded") != -1)
		{
			CallLocalFunction("House_OnDBLoaded", "d", -1);
		}
		return 0;
	}
	new query[2304], row[480];
	// INSERT IGNORE: le 10 case di base si creano solo se mancano. Prima c'era ON DUPLICATE KEY UPDATE, che su un database
	// installato da zero sovrascriveva le case Arizona con id 1-10 (vedi sql/migrations/016_arizona_houses_fix.sql).
	query = "INSERT IGNORE INTO houses (id, tier, interior_idx, ext_x, ext_y, ext_z, ext_a, price) VALUES ";
	for (new i = 0; i < 10; i++)
	{
		new intIdx = s_HouseIntIdx[i];
		new tier = s_HouseIntTier[intIdx];
		mysql_format(g_DB, row, 480, "%s(%d, %d, %d, %f, %f, %f, %f, %d)", i == 0 ? "" : ", ", i + 1, tier, intIdx, s_HouseExt[i], s_HouseExt[i][1], s_HouseExt[i][2], s_HouseExt[i][3], s_HouseTierPrice[tier]);
		strcat(query, row, 2304);
	}
	mysql_tquery(g_DB, query, "DB_OnHouseSeeded", "");
	return 0;
}

public DB_OnHouseSeeded()
{
	mysql_tquery(g_DB, "INSERT IGNORE INTO house_utilities (house_id) SELECT id FROM houses", "DB_OnHouseSeedUtil", "");
	return 1;
}

public DB_OnHouseSeedUtil()
{
	if (funcidx("House_OnDBSeeded") != -1)
	{
		CallLocalFunction("House_OnDBSeeded", "d", 1);
	}
	if (g_HouseBootLoad)
	{
		g_HouseBootLoad = false;
		DB_LoadHouses();
	}
	return 1;
}

DB_BootHouses()
{
	g_HouseBootLoad = true;
	DB_SeedHouses();
}

DB_LoadHouses()
{
	if (!DB_IsReady())
	{
		g_HouseCount = 0;
		if (funcidx("House_OnDBLoaded") != -1)
		{
			CallLocalFunction("House_OnDBLoaded", "d", -1);
		}
		return 0;
	}
	mysql_tquery(g_DB, "INSERT IGNORE INTO house_utilities (house_id) SELECT id FROM houses", "DB_OnHouseLoadUtil", "");
	return 0;
}

public DB_OnHouseLoadUtil()
{
	// le colonne dell'interno personalizzato esistono solo dopo la migrazione 015
	mysql_tquery(g_DB, "SELECT COUNT(*) AS n FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'houses' AND COLUMN_NAME = 'int_game'", "DB_OnHouseColsChecked", "");
	return 1;
}

public DB_OnHouseColsChecked()
{
	new rows = 0, n = 0;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "n", n);
	}
	g_HsIntCols = n > 0;
	new query[1400];
	mysql_format(g_DB, query, sizeof query, "SELECT h.id, %s, IFNULL(h.owner_id, 0) AS owner_id, IFNULL(a.name, '') AS owner_name, h.tier, h.interior_idx, h.ext_x, h.ext_y, h.ext_z, h.ext_a, h.price, IFNULL(h.price_paid, 0) AS price_paid, h.locked, IFNULL(u.luce_on, 1) AS luce_on, IFNULL(u.gas_on, 1) AS gas_on, IFNULL(u.luce_cut, 0) AS luce_cut, IFNULL(u.gas_cut, 0) AS gas_cut, IFNULL(u.luce_min, 0) AS luce_min, IFNULL(u.gas_min, 0) AS gas_min, IFNULL(u.cycle_min, 0) AS cycle_min, IFNULL(u.fridge_units, 0) AS fridge_units, IFNULL(u.fridge_off_min, 0) AS fridge_off_min, (SELECT COUNT(*) FROM house_bills b WHERE b.house_id = h.id AND b.paid = 0) AS bills FROM houses h LEFT JOIN house_utilities u ON u.house_id = h.id LEFT JOIN characters c ON c.id = h.owner_id LEFT JOIN accounts a ON a.id = c.account_id ORDER BY h.id LIMIT %d", g_HsIntCols ? ("IFNULL(h.int_game, -1) AS int_game, IFNULL(h.int_x, 0) AS int_x, IFNULL(h.int_y, 0) AS int_y, IFNULL(h.int_z, 0) AS int_z") : ("-1 AS int_game, 0 AS int_x, 0 AS int_y, 0 AS int_z"), HOUSE_MAX);
	mysql_tquery(g_DB, query, "DB_OnHousesLoaded", "");
	return 1;
}

public DB_OnHousesLoaded()
{
	new rows = 0, n = 0, tmp = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows && n < HOUSE_MAX; i++)
	{
		cache_get_value_name_int(i, "id", g_HouseRows[n][hrId]);
		cache_get_value_name_int(i, "int_game", g_HouseRows[n][hrIntGame]);
		cache_get_value_name_float(i, "int_x", g_HouseRows[n][hrIntX]);
		cache_get_value_name_float(i, "int_y", g_HouseRows[n][hrIntY]);
		cache_get_value_name_float(i, "int_z", g_HouseRows[n][hrIntZ]);
		cache_get_value_name_int(i, "owner_id", g_HouseRows[n][hrOwnerId]);
		cache_get_value_name(i, "owner_name", g_HouseRows[n][hrOwnerName], 25);
		cache_get_value_name_int(i, "tier", g_HouseRows[n][hrTier]);
		cache_get_value_name_int(i, "interior_idx", g_HouseRows[n][hrInteriorIdx]);
		cache_get_value_name_float(i, "ext_x", g_HouseRows[n][hrExtX]);
		cache_get_value_name_float(i, "ext_y", g_HouseRows[n][hrExtY]);
		cache_get_value_name_float(i, "ext_z", g_HouseRows[n][hrExtZ]);
		cache_get_value_name_float(i, "ext_a", g_HouseRows[n][hrExtA]);
		cache_get_value_name_int(i, "price", g_HouseRows[n][hrPrice]);
		cache_get_value_name_int(i, "price_paid", g_HouseRows[n][hrPricePaid]);
		cache_get_value_name_int(i, "locked", tmp);
		g_HouseRows[n][hrLocked] = tmp != 0;
		cache_get_value_name_int(i, "luce_on", tmp);
		g_HouseRows[n][hrLuceOn] = tmp != 0;
		cache_get_value_name_int(i, "gas_on", tmp);
		g_HouseRows[n][hrGasOn] = tmp != 0;
		cache_get_value_name_int(i, "luce_cut", tmp);
		g_HouseRows[n][hrLuceCut] = tmp != 0;
		cache_get_value_name_int(i, "gas_cut", tmp);
		g_HouseRows[n][hrGasCut] = tmp != 0;
		cache_get_value_name_int(i, "luce_min", g_HouseRows[n][hrLuceMin]);
		cache_get_value_name_int(i, "gas_min", g_HouseRows[n][hrGasMin]);
		cache_get_value_name_int(i, "cycle_min", g_HouseRows[n][hrCycleMin]);
		cache_get_value_name_int(i, "fridge_units", g_HouseRows[n][hrFridgeUnits]);
		cache_get_value_name_int(i, "fridge_off_min", g_HouseRows[n][hrFridgeOffMin]);
		cache_get_value_name_int(i, "bills", g_HouseRows[n][hrBills]);
		g_HouseRows[n][hrKeyCount] = 0;
		for (new k = 0; k < 3; k++)
		{
			g_HouseRows[n][hrKeyChars][k] = 0;
		}
		n++;
	}
	g_HouseCount = n;
	mysql_tquery(g_DB, "SELECT house_id, char_id FROM house_keys ORDER BY house_id, created_at, char_id", "DB_OnHouseKeysAllLoaded", "");
	return 1;
}

public DB_OnHouseKeysAllLoaded()
{
	new rows = 0, hid = 0, cid = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows; i++)
	{
		cache_get_value_name_int(i, "house_id", hid);
		cache_get_value_name_int(i, "char_id", cid);
		for (new n = 0; n < g_HouseCount; n++)
		{
			if (g_HouseRows[n][hrId] != hid)
			{
				continue;
			}
			if (g_HouseRows[n][hrKeyCount] < 3)
			{
				g_HouseRows[n][hrKeyChars][g_HouseRows[n][hrKeyCount]] = cid;
				g_HouseRows[n][hrKeyCount]++;
			}
			break;
		}
	}
	if (funcidx("House_OnDBLoaded") != -1)
	{
		CallLocalFunction("House_OnDBLoaded", "d", g_HouseCount);
	}
	return 1;
}

DB_BuyHouse(playerid, houseId, ownerCharId, pricePaid, maxHouses)
{
	if (!DB_HouseBegin(playerid, 512, houseId > 0 && ownerCharId > 0, houseId, 0, 0))
	{
		return 0;
	}
	new name[75], query[768];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 768, "UPDATE houses SET owner_id = %d, price_paid = %d, locked = 1 WHERE id = %d AND owner_id IS NULL AND (%d <= 0 OR (SELECT n FROM (SELECT COUNT(*) AS n FROM houses WHERE owner_id = %d) t) < %d)", ownerCharId, max(pricePaid, 0), houseId, maxHouses, ownerCharId, maxHouses);
	mysql_tquery(g_DB, query, "DB_OnHouseBought", "dsddd", playerid, name, ownerCharId, houseId, max(pricePaid, 0));
	return 0;
}

public DB_OnHouseBought(playerid, const name[], ownerId, houseId, pricePaid)
{
	new affected = cache_affected_rows();
	if (affected == 1)
	{
		DB_HouseResetState(houseId);
	}
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		if (affected == 1)
		{
			DB_HouseCompensate(1, ownerId, houseId, 0, 0, false);
		}
		else if (funcidx("House_OnDBCompensated") != -1)
		{
			CallLocalFunction("House_OnDBCompensated", "d", houseId);
		}
		DB_HouseStaleRelease(playerid, 512);
		return 1;
	}
	DB_PendingClear(playerid, 512);
	if (funcidx("House_OnDBBought") != -1)
	{
		CallLocalFunction("House_OnDBBought", "ddd", playerid, houseId, affected == 1 ? 1 : 0);
	}
	return 1;
}

DB_SellHouse(playerid, houseId, ownerCharId)
{
	if (!DB_HouseBegin(playerid, 1024, houseId > 0 && ownerCharId > 0, houseId, 0, 0))
	{
		return 0;
	}
	new name[75], query[768];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 768, "UPDATE houses SET owner_id = NULL WHERE id = %d AND owner_id = %d", houseId, ownerCharId);
	mysql_tquery(g_DB, query, "DB_OnHouseSold", "dsdd", playerid, name, ownerCharId, houseId);
	return 0;
}

public DB_OnHouseSold(playerid, const name[], ownerId, houseId)
{
	new affected = cache_affected_rows();
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		if (affected == 1)
		{
			DB_HouseCompensate(2, ownerId, houseId, 0, 0, false);
		}
		else if (funcidx("House_OnDBCompensated") != -1)
		{
			CallLocalFunction("House_OnDBCompensated", "d", houseId);
		}
		DB_HouseStaleRelease(playerid, 1024);
		return 1;
	}
	if (affected == 1)
	{
		new query[768];
		mysql_format(g_DB, query, 768, "UPDATE houses SET price_paid = NULL, locked = 1 WHERE id = %d AND owner_id IS NULL", houseId);
		mysql_tquery(g_DB, query, "", "");
		DB_HouseResetState(houseId);
	}
	DB_PendingClear(playerid, 1024);
	if (funcidx("House_OnDBSold") != -1)
	{
		CallLocalFunction("House_OnDBSold", "ddd", playerid, houseId, affected == 1 ? 1 : 0);
	}
	return 1;
}

DB_SetHouseLock(houseId, ownerCharId, bool:locked)
{
	if (!DB_IsReady() || houseId <= 0 || ownerCharId <= 0)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE houses SET locked = %d WHERE id = %d AND owner_id = %d", locked ? 1 : 0, houseId, ownerCharId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_SetUtility(houseId, actorCharId, utility, bool:on)
{
	if (!DB_IsReady() || houseId <= 0 || actorCharId <= 0)
	{
		return 0;
	}
	new query[768];
	if (utility == 0)
	{
		mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.luce_on = %d WHERE u.house_id = %d AND u.luce_cut = 0 AND (h.owner_id = %d OR EXISTS (SELECT 1 FROM house_keys k WHERE k.house_id = h.id AND k.char_id = %d))", on ? 1 : 0, houseId, actorCharId, actorCharId);
	}
	else if (utility == 1)
	{
		mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.gas_on = %d WHERE u.house_id = %d AND u.gas_cut = 0 AND h.owner_id = %d", on ? 1 : 0, houseId, actorCharId);
	}
	else
	{
		return 0;
	}
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_FridgeAdd(houseId, actorCharId, delta)
{
	if (!DB_IsReady() || houseId <= 0 || actorCharId <= 0 || delta == 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.fridge_units = LEAST(65535, GREATEST(0, CAST(u.fridge_units AS SIGNED) + %d)) WHERE u.house_id = %d AND (h.owner_id = %d OR EXISTS (SELECT 1 FROM house_keys k WHERE k.house_id = h.id AND k.char_id = %d))", delta, houseId, actorCharId, actorCharId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_FridgeAddSys(houseId, delta)
{
	if (!DB_IsReady() || houseId <= 0 || delta == 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE house_utilities SET fridge_units = LEAST(65535, GREATEST(0, CAST(fridge_units AS SIGNED) + %d)) WHERE house_id = %d", delta, houseId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_ReloadFridge(houseId)
{
	if (!DB_IsReady() || houseId <= 0)
	{
		if (funcidx("House_OnDBFridgeReloaded") != -1)
		{
			CallLocalFunction("House_OnDBFridgeReloaded", "dd", houseId, -1);
		}
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "SELECT fridge_units FROM house_utilities WHERE house_id = %d", houseId);
	mysql_tquery(g_DB, query, "DB_OnFridgeReloaded", "d", houseId);
	return 0;
}

public DB_OnFridgeReloaded(houseId)
{
	new units = -1;
	if (cache_num_rows() > 0)
	{
		cache_get_value_name_int(0, "fridge_units", units);
	}
	if (funcidx("House_OnDBFridgeReloaded") != -1)
	{
		CallLocalFunction("House_OnDBFridgeReloaded", "dd", houseId, units);
	}
	return 1;
}

DB_SaveHouseTick(houseId, ownerCharId, luceMin, gasMin, cycleMin, fridgeOffMin)
{
	if (!DB_IsReady() || houseId <= 0 || ownerCharId <= 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.luce_min = %d, u.gas_min = %d, u.cycle_min = %d, u.fridge_off_min = %d WHERE u.house_id = %d AND h.owner_id = %d", clamp(luceMin, 0, 65535), clamp(gasMin, 0, 65535), clamp(cycleMin, 0, 65535), clamp(fridgeOffMin, 0, 65535), houseId, ownerCharId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_HouseSetCut(houseId, utility, bool:cut)
{
	if (!DB_IsReady() || houseId <= 0)
	{
		return 0;
	}
	new query[768];
	if (utility == 0)
	{
		mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.luce_cut = %d, u.luce_on = %d WHERE u.house_id = %d AND h.owner_id IS NOT NULL", cut ? 1 : 0, cut ? 0 : 1, houseId);
	}
	else if (utility == 1)
	{
		mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.gas_cut = %d, u.gas_on = %d WHERE u.house_id = %d AND h.owner_id IS NOT NULL", cut ? 1 : 0, cut ? 0 : 1, houseId);
	}
	else
	{
		return 0;
	}
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_CutUtility(houseId, utility)
{
	DB_HouseSetCut(houseId, utility, true);
}

DB_LoadFood(playerid, charId)
{
	if (!DB_HouseBegin(playerid, 65536, charId > 0, 0, 0, 0))
	{
		return 0;
	}
	new name[75], query[512];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 512, "SELECT hunger, groceries FROM characters WHERE id = %d", charId);
	mysql_tquery(g_DB, query, "DB_OnHouseFoodLoaded", "dsd", playerid, name, charId);
	return 0;
}

public DB_OnHouseFoodLoaded(playerid, const name[], charId)
{
	if (!DB_HouseSessionOk(playerid, name, charId))
	{
		DB_HouseStaleRelease(playerid, 65536);
		return 1;
	}
	DB_PendingClear(playerid, 65536);
	new rows = 0, hunger = -1, groceries = -1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "hunger", hunger);
		cache_get_value_name_int(0, "groceries", groceries);
	}
	if (funcidx("House_OnDBFoodLoaded") != -1)
	{
		CallLocalFunction("House_OnDBFoodLoaded", "ddd", playerid, hunger, groceries);
	}
	return 1;
}

DB_SaveHunger(charId, hunger)
{
	if (!DB_IsReady() || charId <= 0)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE characters SET hunger = %d WHERE id = %d", clamp(hunger, 0, 100), charId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

public DB_OnHouseStored(playerid, const name[], ownerId, houseId, n)
{
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		DB_HouseStaleRelease(playerid, 131072);
		return 1;
	}
	DB_PendingClear(playerid, 131072);
	if (funcidx("House_OnDBStored") != -1)
	{
		CallLocalFunction("House_OnDBStored", "ddd", playerid, houseId, cache_affected_rows() >= 1 ? n : 0);
	}
	return 1;
}

DB_AddKey(playerid, houseId, ownerCharId, targetCharId)
{
	if (!DB_HouseBegin(playerid, 2048, houseId > 0 && ownerCharId > 0 && targetCharId > 0, houseId, targetCharId, 0))
	{
		return 0;
	}
	new name[75], query[768];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 768, "INSERT IGNORE INTO house_keys (house_id, char_id) SELECT h.id, %d FROM houses h WHERE h.id = %d AND h.owner_id = %d AND h.owner_id <> %d AND (SELECT COUNT(*) FROM house_keys k WHERE k.house_id = h.id) < %d", targetCharId, houseId, ownerCharId, targetCharId, 3);
	mysql_tquery(g_DB, query, "DB_OnHouseKeyAdded", "dsddd", playerid, name, ownerCharId, houseId, targetCharId);
	return 0;
}

public DB_OnHouseKeyAdded(playerid, const name[], ownerId, houseId, targetId)
{
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		DB_HouseStaleRelease(playerid, 2048);
		return 1;
	}
	DB_PendingClear(playerid, 2048);
	if (funcidx("House_OnDBKeyAdded") != -1)
	{
		CallLocalFunction("House_OnDBKeyAdded", "dddd", playerid, houseId, targetId, cache_affected_rows() == 1 ? 1 : 0);
	}
	return 1;
}

DB_RemoveKey(playerid, houseId, ownerCharId, targetCharId)
{
	if (!DB_HouseBegin(playerid, 4096, houseId > 0 && ownerCharId > 0 && targetCharId > 0, houseId, targetCharId, 0))
	{
		return 0;
	}
	new name[75], query[768];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 768, "DELETE k FROM house_keys k JOIN houses h ON h.id = k.house_id WHERE k.house_id = %d AND k.char_id = %d AND h.owner_id = %d", houseId, targetCharId, ownerCharId);
	mysql_tquery(g_DB, query, "DB_OnHouseKeyRemoved", "dsddd", playerid, name, ownerCharId, houseId, targetCharId);
	return 0;
}

public DB_OnHouseKeyRemoved(playerid, const name[], ownerId, houseId, targetId)
{
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		DB_HouseStaleRelease(playerid, 4096);
		return 1;
	}
	DB_PendingClear(playerid, 4096);
	if (funcidx("House_OnDBKeyRemoved") != -1)
	{
		CallLocalFunction("House_OnDBKeyRemoved", "dddd", playerid, houseId, targetId, cache_affected_rows() == 1 ? 1 : 0);
	}
	return 1;
}

DB_LoadKeys(playerid, houseId, ownerCharId)
{
	if (!DB_HouseBegin(playerid, 8192, houseId > 0 && ownerCharId > 0, houseId, 0, 0))
	{
		return 0;
	}
	new name[75], query[768];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 768, "SELECT k.char_id, IFNULL(a.name, '') AS name FROM house_keys k JOIN houses h ON h.id = k.house_id JOIN characters c ON c.id = k.char_id JOIN accounts a ON a.id = c.account_id WHERE k.house_id = %d AND h.owner_id = %d ORDER BY k.created_at, k.char_id LIMIT %d", houseId, ownerCharId, 3);
	mysql_tquery(g_DB, query, "DB_OnHouseKeysLoaded", "dsdd", playerid, name, ownerCharId, houseId);
	return 0;
}

public DB_OnHouseKeysLoaded(playerid, const name[], ownerId, houseId)
{
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		DB_HouseStaleRelease(playerid, 8192);
		return 1;
	}
	DB_PendingClear(playerid, 8192);
	new rows = 0, n = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows && n < 3; i++)
	{
		cache_get_value_name_int(i, "char_id", g_HouseKeyList[playerid][n][hkCharId]);
		cache_get_value_name(i, "name", g_HouseKeyList[playerid][n][hkName], 25);
		n++;
	}
	if (funcidx("House_OnDBKeysLoaded") != -1)
	{
		CallLocalFunction("House_OnDBKeysLoaded", "ddd", playerid, houseId, n);
	}
	return 1;
}

DB_AddBill(houseId, kind, amount, maxPending)
{
	if (!DB_IsReady() || houseId <= 0 || kind < 0 || kind > 1)
	{
		if (funcidx("House_OnDBBillAdded") != -1)
		{
			CallLocalFunction("House_OnDBBillAdded", "dddd", houseId, kind, -1, amount);
		}
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "INSERT INTO house_bills (house_id, kind, amount) SELECT h.id, %d, %d FROM houses h WHERE h.id = %d AND h.owner_id IS NOT NULL AND (SELECT COUNT(*) FROM house_bills b WHERE b.house_id = h.id AND b.kind = %d AND b.paid = 0) < %d", kind, max(amount, 0), houseId, kind, maxPending);
	mysql_tquery(g_DB, query, "DB_OnHouseBillAdded", "ddd", houseId, kind, max(amount, 0));
	return 0;
}

public DB_OnHouseBillAdded(houseId, kind, amount)
{
	new billId = cache_affected_rows() == 1 ? cache_insert_id() : 0;
	if (funcidx("House_OnDBBillAdded") != -1)
	{
		CallLocalFunction("House_OnDBBillAdded", "dddd", houseId, kind, billId, amount);
	}
	return 1;
}

DB_AgeBills(houseId, dueCycles)
{
	if (!DB_IsReady() || houseId <= 0)
	{
		if (funcidx("House_OnDBBillsAged") != -1)
		{
			CallLocalFunction("House_OnDBBillsAged", "dd", houseId, -1);
		}
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE house_bills SET cycles_age = LEAST(cycles_age + 1, 255) WHERE house_id = %d AND paid = 0", houseId);
	mysql_tquery(g_DB, query, "DB_OnHouseBillsAgedStep", "dd", houseId, dueCycles);
	return 0;
}

public DB_OnHouseBillsAgedStep(houseId, dueCycles)
{
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT kind, MAX(cycles_age) AS age FROM house_bills WHERE house_id = %d AND paid = 0 GROUP BY kind", houseId);
	mysql_tquery(g_DB, query, "DB_OnHouseBillsAged", "dd", houseId, dueCycles);
	return 1;
}

public DB_OnHouseBillsAged(houseId, dueCycles)
{
	new rows = 0, kind = 0, age = 0, mask = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows; i++)
	{
		cache_get_value_name_int(i, "kind", kind);
		cache_get_value_name_int(i, "age", age);
		if (age >= dueCycles && kind >= 0 && kind <= 1)
		{
			mask |= 1 << kind;
		}
	}
	if (funcidx("House_OnDBBillsAged") != -1)
	{
		CallLocalFunction("House_OnDBBillsAged", "dd", houseId, mask);
	}
	return 1;
}

DB_LoadBills(playerid, houseId, ownerCharId)
{
	if (!DB_HouseBegin(playerid, 16384, houseId > 0 && ownerCharId > 0, houseId, 0, 0))
	{
		return 0;
	}
	new name[75], query[768];
	GetPlayerName(playerid, name, 75);
	mysql_format(g_DB, query, 768, "SELECT b.id, b.kind, b.amount, b.cycles_age FROM house_bills b JOIN houses h ON h.id = b.house_id WHERE b.house_id = %d AND h.owner_id = %d AND b.paid = 0 ORDER BY b.kind, b.id LIMIT %d", houseId, ownerCharId, 8);
	mysql_tquery(g_DB, query, "DB_OnHouseBillsLoaded", "dsdd", playerid, name, ownerCharId, houseId);
	return 0;
}

public DB_OnHouseBillsLoaded(playerid, const name[], ownerId, houseId)
{
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		DB_HouseStaleRelease(playerid, 16384);
		return 1;
	}
	DB_PendingClear(playerid, 16384);
	new rows = 0, n = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows && n < 8; i++)
	{
		cache_get_value_name_int(i, "id", g_HouseBills[playerid][n][hbId]);
		cache_get_value_name_int(i, "kind", g_HouseBills[playerid][n][hbKind]);
		cache_get_value_name_int(i, "amount", g_HouseBills[playerid][n][hbAmount]);
		cache_get_value_name_int(i, "cycles_age", g_HouseBills[playerid][n][hbCyclesAge]);
		n++;
	}
	if (funcidx("House_OnDBBillsLoaded") != -1)
	{
		CallLocalFunction("House_OnDBBillsLoaded", "ddd", playerid, houseId, n);
	}
	return 1;
}

DB_PayBills(playerid, houseId, ownerCharId, kind, expectedTotal, bool:restoreCut)
{
	if (!DB_HouseBegin(playerid, 32768, houseId > 0 && ownerCharId > 0 && kind >= 0 && kind <= 1 && expectedTotal >= 0, houseId, 0, kind))
	{
		return 0;
	}
	new name[75], query[1536], token = gettime();
	new col[24];
	GetPlayerName(playerid, name, 75);
	if (expectedTotal == 0)
	{
		if (!restoreCut)
		{
			DB_PendingClear(playerid, 32768);
			DB_HouseDeliver(playerid, 32768, 0, houseId, 0, kind);
			return 0;
		}
		DB_HouseRestoreStep(playerid, name, ownerCharId, houseId, kind, 0, token);
		return 0;
	}
	strcat(col, kind == 1 ? "gas" : "luce", 24);
	if (restoreCut)
	{
		mysql_format(g_DB, query, 1536, "UPDATE house_bills b JOIN houses h ON h.id = b.house_id JOIN house_utilities u ON u.house_id = h.id SET b.paid = 1, b.paid_at = FROM_UNIXTIME(%d), b.pay_id = (SELECT m FROM (SELECT MIN(id) AS m FROM house_bills WHERE house_id = %d AND kind = %d AND paid = 0) pm), u.%s_cut = 0, u.%s_on = 1 WHERE b.house_id = %d AND b.kind = %d AND b.paid = 0 AND h.owner_id = %d AND u.%s_cut = 1 AND (SELECT s FROM (SELECT COALESCE(SUM(amount), 0) AS s FROM house_bills WHERE house_id = %d AND kind = %d AND paid = 0) t) = %d", token, houseId, kind, col, col, houseId, kind, ownerCharId, col, houseId, kind, expectedTotal);
	}
	else
	{
		mysql_format(g_DB, query, 1536, "UPDATE house_bills b JOIN houses h ON h.id = b.house_id SET b.paid = 1, b.paid_at = FROM_UNIXTIME(%d), b.pay_id = (SELECT m FROM (SELECT MIN(id) AS m FROM house_bills WHERE house_id = %d AND kind = %d AND paid = 0) pm) WHERE b.house_id = %d AND b.kind = %d AND b.paid = 0 AND h.owner_id = %d AND (SELECT s FROM (SELECT COALESCE(SUM(amount), 0) AS s FROM house_bills WHERE house_id = %d AND kind = %d AND paid = 0) t) = %d", token, houseId, kind, houseId, kind, ownerCharId, houseId, kind, expectedTotal);
	}
	mysql_tquery(g_DB, query, "DB_OnHouseBillsPaid", "dsdddddd", playerid, name, ownerCharId, houseId, kind, expectedTotal, token, restoreCut ? 1 : 0);
	return 0;
}

public DB_OnHouseBillsPaid(playerid, const name[], ownerId, houseId, kind, total, token, restore)
{
	new affected = cache_affected_rows();
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		if (affected >= 1)
		{
			DB_HouseCompensate(3, ownerId, houseId, kind, token, restore != 0);
		}
		DB_HouseStaleRelease(playerid, 32768);
		return 1;
	}
	DB_PendingClear(playerid, 32768);
	if (affected < 1)
	{
		if (funcidx("House_OnDBBillsPaid") != -1)
		{
			CallLocalFunction("House_OnDBBillsPaid", "ddddd", playerid, houseId, kind, 0, 0);
		}
		return 1;
	}
	if (funcidx("House_OnDBBillsPaid") != -1)
	{
		CallLocalFunction("House_OnDBBillsPaid", "ddddd", playerid, houseId, kind, total, restore);
	}
	return 1;
}

DB_HouseRestoreStep(playerid, const name[], ownerId, houseId, kind, total, token)
{
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE house_utilities u JOIN houses h ON h.id = u.house_id SET u.%s_cut = 0, u.%s_on = 1 WHERE u.house_id = %d AND h.owner_id = %d AND u.%s_cut = 1", kind == 1 ? "gas" : "luce", kind == 1 ? "gas" : "luce", houseId, ownerId, kind == 1 ? "gas" : "luce");
	mysql_tquery(g_DB, query, "DB_OnHouseRestored", "dsddddd", playerid, name, ownerId, houseId, kind, total, token);
}

public DB_OnHouseRestored(playerid, const name[], ownerId, houseId, kind, total, token)
{
	new restored = cache_affected_rows() == 1 ? 1 : 0;
	if (!DB_HouseSessionOk(playerid, name, ownerId))
	{
		if (restored)
		{
			DB_HouseCompensate(3, ownerId, houseId, kind, token, true);
		}
		DB_HouseStaleRelease(playerid, 32768);
		return 1;
	}
	DB_PendingClear(playerid, 32768);
	if (funcidx("House_OnDBBillsPaid") != -1)
	{
		CallLocalFunction("House_OnDBBillsPaid", "ddddd", playerid, houseId, kind, total, restored);
	}
	return 1;
}

DB_RevertBillPayment(houseId, ownerCharId, kind, bool:recut)
{
	if (!DB_IsReady() || houseId <= 0 || ownerCharId <= 0 || kind < 0 || kind > 1)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE house_bills b JOIN houses h ON h.id = b.house_id SET b.paid = 0, b.paid_at = NULL, b.pay_id = NULL WHERE b.house_id = %d AND b.kind = %d AND b.paid = 1 AND h.owner_id = %d AND b.pay_id = (SELECT m FROM (SELECT MAX(pay_id) AS m FROM house_bills WHERE house_id = %d AND kind = %d AND paid = 1) t)", houseId, kind, ownerCharId, houseId, kind);
	mysql_tquery(g_DB, query, "", "");
	if (recut)
	{
		DB_HouseSetCut(houseId, kind, true);
	}
	return 0;
}

DB_RestoreSoldHouse(houseId, ownerCharId, pricePaid)
{
	if (!DB_IsReady() || ownerCharId <= 0 || houseId <= 0)
	{
		Log_Write("DBHOUSE", "ripristino vendita NON eseguito (db non pronto o id non valido): verificare a mano");
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE houses SET owner_id = %d, price_paid = IF(%d > 0, %d, price_paid) WHERE id = %d AND owner_id IS NULL", ownerCharId, pricePaid, pricePaid, houseId);
	mysql_tquery(g_DB, query, "DB_OnHouseCompensated", "ddd", 2, ownerCharId, houseId);
	return 0;
}
