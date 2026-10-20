new g_BankDb[1000];
new g_BankFly[1000];
new g_BankFlyChar[1000];
new g_BankGen[1000];

DB_BankSessionReset(playerid, charId)
{
	g_BankDb[playerid] = gPlayer[playerid][PLAYER_DATA_89];
	g_BankFly[playerid] = 0;
	g_BankFlyChar[playerid] = charId;
	g_BankGen[playerid]++;
}

DB_BankDeltaTake(playerid)
{
	new charId = gPlayer[playerid][PLAYER_DATA_3];
	if (g_BankFlyChar[playerid] != charId)
	{
		g_BankFly[playerid] = 0;
		g_BankFlyChar[playerid] = charId;
	}
	new d = gPlayer[playerid][PLAYER_DATA_89] - g_BankDb[playerid] - g_BankFly[playerid];
	g_BankFly[playerid] += d;
	return d;
}

public DB_OnBankDeltaSaved(playerid, name[], charId, gen, delta)
{
	if (!DB_PlayerMatches(playerid, name) || gPlayer[playerid][PLAYER_DATA_3] != charId || g_BankGen[playerid] != gen || g_BankFlyChar[playerid] != charId)
	{
		return 1;
	}
	g_BankDb[playerid] += delta;
	g_BankFly[playerid] -= delta;
	return 1;
}

public DB_BankDeltaFailed()
{
	for (new i = 0; i < 1000; i++)
	{
		g_BankFly[i] = 0;
	}
	print("[DB] salvataggio personaggio fallito: i delta banca in volo verranno rimandati");
	return 1;
}

DB_LoadCharacter(playerid, accountId)
{
	if (playerid < 0 || playerid >= 1000 || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || accountId <= 0)
	{
		DB_DeliverFailure(playerid, 4);
		return 0;
	}
	new name[25], query[512];
	GetPlayerName(playerid, name, 25);
	new jfCols[29] = ", job, faction, faction_rank";
	mysql_format(g_DB, query, 512, "SELECT id, money, bank, level, exp, health, armor, pos_x, pos_y, pos_z, angle, interior, world, skin, LEAST(played_ms, %d) AS played_ms%s FROM characters WHERE account_id = %d AND slot = %d LIMIT 1", 0x7FFFFFFF, jfCols, accountId, 0);
	DB_PendingSet(playerid, 4);
	mysql_tquery(g_DB, query, "DB_OnCharLoaded", "dsd", playerid, name, accountId);
}

public DB_OnCharLoaded(playerid, name[], accountId)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	DB_PendingClear(playerid, 4);
	if (gPlayer[playerid][PLAYER_DATA_106] != 4 || gPlayer[playerid][PLAYER_DATA_2] != accountId)
	{
		return 1;
	}
	new rows = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		Char_OnDBLoaded(playerid, 0);
		return 1;
	}
	cache_get_value_name_int(0, "id", gPlayer[playerid][PLAYER_DATA_3]);
	cache_get_value_name_int(0, "money", gPlayer[playerid][PLAYER_DATA_88]);
	cache_get_value_name_int(0, "bank", gPlayer[playerid][PLAYER_DATA_89]);
	cache_get_value_name_int(0, "level", gPlayer[playerid][PLAYER_DATA_90]);
	cache_get_value_name_int(0, "exp", gPlayer[playerid][PLAYER_DATA_91]);
	cache_get_value_name_float(0, "health", gPlayer[playerid][PLAYER_DATA_92]);
	cache_get_value_name_float(0, "armor", gPlayer[playerid][PLAYER_DATA_93]);
	cache_get_value_name_float(0, "pos_x", gPlayer[playerid][PLAYER_DATA_94]);
	cache_get_value_name_float(0, "pos_y", gPlayer[playerid][PLAYER_DATA_95]);
	cache_get_value_name_float(0, "pos_z", gPlayer[playerid][PLAYER_DATA_96]);
	cache_get_value_name_float(0, "angle", gPlayer[playerid][PLAYER_DATA_97]);
	cache_get_value_name_int(0, "interior", gPlayer[playerid][PLAYER_DATA_98]);
	cache_get_value_name_int(0, "world", gPlayer[playerid][PLAYER_DATA_99]);
	cache_get_value_name_int(0, "skin", gPlayer[playerid][PLAYER_DATA_100]);
	cache_get_value_name_int(0, "played_ms", gPlayer[playerid][PLAYER_DATA_113]);
	DB_BankSessionReset(playerid, gPlayer[playerid][PLAYER_DATA_3]);
	cache_get_value_name_int(0, "job", gPlayer[playerid][PLAYER_DATA_101]);
	cache_get_value_name_int(0, "faction", gPlayer[playerid][PLAYER_DATA_102]);
	cache_get_value_name_int(0, "faction_rank", gPlayer[playerid][PLAYER_DATA_103]);
	gPlayer[playerid][PLAYER_DATA_104] = gPlayer[playerid][PLAYER_DATA_100];
	Char_OnDBLoaded(playerid, 1);
	return 1;
}

DB_CreateCharacter(playerid, accountId)
{
	if (playerid < 0 || playerid >= 1000 || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || accountId <= 0)
	{
		DB_DeliverFailure(playerid, 8);
		return 0;
	}
	new name[25], query[512];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "INSERT IGNORE INTO characters (account_id, slot, money, bank, level, exp, health, armor, pos_x, pos_y, pos_z, angle, interior, world, skin, played_ms) VALUES (%d, %d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %d, %d, %d, %d)", accountId, 0, gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89], gPlayer[playerid][PLAYER_DATA_90], gPlayer[playerid][PLAYER_DATA_91], gPlayer[playerid][PLAYER_DATA_92], gPlayer[playerid][PLAYER_DATA_93], gPlayer[playerid][PLAYER_DATA_94], gPlayer[playerid][PLAYER_DATA_95], gPlayer[playerid][PLAYER_DATA_96], gPlayer[playerid][PLAYER_DATA_97], gPlayer[playerid][PLAYER_DATA_98], gPlayer[playerid][PLAYER_DATA_99], gPlayer[playerid][PLAYER_DATA_100], max(gPlayer[playerid][PLAYER_DATA_113], 0));
	g_BankDb[playerid] = gPlayer[playerid][PLAYER_DATA_89];
	DB_PendingSet(playerid, 8);
	mysql_tquery(g_DB, query, "DB_OnCharCreated", "ds", playerid, name);
}

public DB_OnCharCreated(playerid, name[])
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	DB_PendingClear(playerid, 8);
	if (gPlayer[playerid][PLAYER_DATA_106] != 7)
	{
		return 1;
	}
	new charId = 0;
	if (cache_affected_rows() == 1)
	{
		charId = cache_insert_id();
	}
	gPlayer[playerid][PLAYER_DATA_3] = charId;
	if (charId > 0)
	{
		DB_BankSessionReset(playerid, charId);
	}
	if (charId > 0)
	{
		gPlayer[playerid][PLAYER_DATA_104] = gPlayer[playerid][PLAYER_DATA_100];
	}
	Char_OnDBCreated(playerid, charId);
	return 1;
}

DB_SaveCharacter(playerid)
{
	if (playerid < 0 || playerid >= 1000 || !DB_IsReady())
	{
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_3] <= 0)
	{
		return 0;
	}
	new query[768], skin = gPlayer[playerid][PLAYER_DATA_100], name[25];
	GetPlayerName(playerid, name, 25);
	new bankDelta = DB_BankDeltaTake(playerid);
	skin = gPlayer[playerid][PLAYER_DATA_104];
	mysql_format(g_DB, query, 768, "UPDATE characters SET money = %d, bank = GREATEST(0, LEAST(%d, CAST(bank AS SIGNED) + (%d))), level = %d, exp = %d, health = %f, armor = %f, pos_x = %f, pos_y = %f, pos_z = %f, angle = %f, interior = %d, world = %d, skin = %d, played_ms = %d, last_save = NOW() WHERE id = %d", gPlayer[playerid][PLAYER_DATA_88], 0x3B9ACA00, bankDelta, gPlayer[playerid][PLAYER_DATA_90], gPlayer[playerid][PLAYER_DATA_91], gPlayer[playerid][PLAYER_DATA_92], gPlayer[playerid][PLAYER_DATA_93], gPlayer[playerid][PLAYER_DATA_94], gPlayer[playerid][PLAYER_DATA_95], gPlayer[playerid][PLAYER_DATA_96], gPlayer[playerid][PLAYER_DATA_97], gPlayer[playerid][PLAYER_DATA_98], gPlayer[playerid][PLAYER_DATA_99], skin, max(gPlayer[playerid][PLAYER_DATA_113], 0), gPlayer[playerid][PLAYER_DATA_3]);
	mysql_tquery(g_DB, query, "DB_OnBankDeltaSaved", "dsddd", playerid, name, gPlayer[playerid][PLAYER_DATA_3], g_BankGen[playerid], bankDelta);
}
