new g_BankDb[MAX_PLAYERS];
new g_BankFly[MAX_PLAYERS];
new g_BankFlyChar[MAX_PLAYERS];
new g_BankGen[MAX_PLAYERS];

DB_BankSessionReset(playerid, charId)
{
	g_BankDb[playerid] = gPlayer[playerid][pBank];
	g_BankFly[playerid] = 0;
	g_BankFlyChar[playerid] = charId;
	g_BankGen[playerid]++;
}

DB_BankDeltaTake(playerid)
{
	new charId = gPlayer[playerid][pCharId];
	if (g_BankFlyChar[playerid] != charId)
	{
		g_BankFly[playerid] = 0;
		g_BankFlyChar[playerid] = charId;
	}
	new d = gPlayer[playerid][pBank] - g_BankDb[playerid] - g_BankFly[playerid];
	g_BankFly[playerid] += d;
	return d;
}

public DB_OnBankDeltaSaved(playerid, name[], charId, gen, delta)
{
	if (!DB_PlayerMatches(playerid, name) || gPlayer[playerid][pCharId] != charId || g_BankGen[playerid] != gen || g_BankFlyChar[playerid] != charId)
	{
		return 1;
	}
	g_BankDb[playerid] += delta;
	g_BankFly[playerid] -= delta;
	return 1;
}

public DB_BankDeltaFailed()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		g_BankFly[i] = 0;
	}
	print("[DB] salvataggio personaggio fallito: i delta banca in volo verranno rimandati");
	return 1;
}

DB_LoadCharacter(playerid, accountId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
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
	new jfCols[64] = ", job, faction, faction_rank, COALESCE(char_name, '') AS cn";
	mysql_format(g_DB, query, 512, "SELECT id, money, bank, level, exp, health, armor, pos_x, pos_y, pos_z, angle, interior, world, skin, LEAST(played_ms, %d) AS played_ms%s FROM characters WHERE account_id = %d AND slot = %d LIMIT 1", 0x7FFFFFFF, jfCols, accountId, CharSel_Slot(playerid));
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
	if (gPlayer[playerid][pAuthState] != 4 || gPlayer[playerid][pAccountId] != accountId)
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
	cache_get_value_name_int(0, "id", gPlayer[playerid][pCharId]);
	cache_get_value_name_int(0, "money", gPlayer[playerid][pMoney]);
	cache_get_value_name_int(0, "bank", gPlayer[playerid][pBank]);
	cache_get_value_name_int(0, "level", gPlayer[playerid][pLevel]);
	cache_get_value_name_int(0, "exp", gPlayer[playerid][pExp]);
	cache_get_value_name_float(0, "health", gPlayer[playerid][pHealth]);
	cache_get_value_name_float(0, "armor", gPlayer[playerid][pArmor]);
	cache_get_value_name_float(0, "pos_x", gPlayer[playerid][pPosX]);
	cache_get_value_name_float(0, "pos_y", gPlayer[playerid][pPosY]);
	cache_get_value_name_float(0, "pos_z", gPlayer[playerid][pPosZ]);
	cache_get_value_name_float(0, "angle", gPlayer[playerid][pAngle]);
	cache_get_value_name_int(0, "interior", gPlayer[playerid][pInterior]);
	cache_get_value_name_int(0, "world", gPlayer[playerid][pWorld]);
	cache_get_value_name_int(0, "skin", gPlayer[playerid][pSkin]);
	cache_get_value_name_int(0, "played_ms", gPlayer[playerid][pPlayedMs]);
	DB_BankSessionReset(playerid, gPlayer[playerid][pCharId]);
	cache_get_value_name_int(0, "job", gPlayer[playerid][pJob]);
	cache_get_value_name_int(0, "faction", gPlayer[playerid][pFaction]);
	cache_get_value_name_int(0, "faction_rank", gPlayer[playerid][pFactionRank]);
	new cn[25];
	cache_get_value_name(0, "cn", cn, sizeof cn);
	CharSel_SetLoadedName(playerid, cn);
	gPlayer[playerid][pCivilSkin] = gPlayer[playerid][pSkin];
	Char_OnDBLoaded(playerid, 1);
	return 1;
}

DB_CreateCharacter(playerid, accountId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || accountId <= 0)
	{
		DB_DeliverFailure(playerid, 8);
		return 0;
	}
	new name[25], query[768], cn[25], first[21], last[21], birth[11];
	GetPlayerName(playerid, name, 25);
	CharSel_Identity(playerid, cn, first, last, birth);
	mysql_format(g_DB, query, 768, "INSERT IGNORE INTO characters (account_id, slot, char_name, first_name, last_name, birth_date, gender, money, bank, level, exp, health, armor, pos_x, pos_y, pos_z, angle, interior, world, skin, played_ms) VALUES (%d, %d, '%e', '%e', '%e', NULLIF('%e', ''), %d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %d, %d, %d, %d)", accountId, CharSel_Slot(playerid), cn, first, last, birth, gPlayer[playerid][pGender] == 1 ? 1 : 0, gPlayer[playerid][pMoney], gPlayer[playerid][pBank], gPlayer[playerid][pLevel], gPlayer[playerid][pExp], gPlayer[playerid][pHealth], gPlayer[playerid][pArmor], gPlayer[playerid][pPosX], gPlayer[playerid][pPosY], gPlayer[playerid][pPosZ], gPlayer[playerid][pAngle], gPlayer[playerid][pInterior], gPlayer[playerid][pWorld], gPlayer[playerid][pSkin], max(gPlayer[playerid][pPlayedMs], 0));
	g_BankDb[playerid] = gPlayer[playerid][pBank];
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
	if (gPlayer[playerid][pAuthState] != 7)
	{
		return 1;
	}
	new charId = 0;
	if (cache_affected_rows() == 1)
	{
		charId = cache_insert_id();
	}
	gPlayer[playerid][pCharId] = charId;
	if (charId > 0)
	{
		DB_BankSessionReset(playerid, charId);
		new q[128];   // numero di telefono in gioco come nella migrazione 012 (5550000 + id)
		mysql_format(g_DB, q, sizeof q, "UPDATE characters SET phone_number = 5550000 + id WHERE id = %d AND id < 10000 AND phone_number IS NULL", charId);
		mysql_tquery(g_DB, q);
	}
	if (charId > 0)
	{
		gPlayer[playerid][pCivilSkin] = gPlayer[playerid][pSkin];
	}
	Char_OnDBCreated(playerid, charId);
	return 1;
}

DB_SaveCharacter(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !DB_IsReady())
	{
		return 0;
	}
	if (gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	new query[768], skin = gPlayer[playerid][pSkin], name[25];
	GetPlayerName(playerid, name, 25);
	new bankDelta = DB_BankDeltaTake(playerid);
	skin = gPlayer[playerid][pCivilSkin];
	mysql_format(g_DB, query, 768, "UPDATE characters SET money = %d, bank = GREATEST(0, LEAST(%d, CAST(bank AS SIGNED) + (%d))), level = %d, exp = %d, health = %f, armor = %f, pos_x = %f, pos_y = %f, pos_z = %f, angle = %f, interior = %d, world = %d, skin = %d, played_ms = %d, last_save = NOW() WHERE id = %d", gPlayer[playerid][pMoney], 0x3B9ACA00, bankDelta, gPlayer[playerid][pLevel], gPlayer[playerid][pExp], gPlayer[playerid][pHealth], gPlayer[playerid][pArmor], gPlayer[playerid][pPosX], gPlayer[playerid][pPosY], gPlayer[playerid][pPosZ], gPlayer[playerid][pAngle], gPlayer[playerid][pInterior], gPlayer[playerid][pWorld], skin, max(gPlayer[playerid][pPlayedMs], 0), gPlayer[playerid][pCharId]);
	mysql_tquery(g_DB, query, "DB_OnBankDeltaSaved", "dsddd", playerid, name, gPlayer[playerid][pCharId], g_BankGen[playerid], bankDelta);
}
