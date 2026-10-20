
DB_GetHashAlgo(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS ? g_DBHashAlgo[playerid] : 0;
}

DB_GetMutedUntil(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS ? g_DBMutedUntil[playerid] : 0;
}

DB_GetLockedUntil(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS ? g_DBLockedUntil[playerid] : 0;
}

DB_LoadAccount(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	g_DBHashAlgo[playerid] = 0;
	g_DBMutedUntil[playerid] = 0;
	g_DBLockedUntil[playerid] = 0;
	if (!DB_IsReady())
	{
		DB_DeliverFailure(playerid, 1);
		return 0;
	}
	new name[25], query[512];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "SELECT id, password_hash, salt, hash_algo, muted_until, admin_level, locked_until FROM accounts WHERE name = '%e' LIMIT 1", name);
	DB_PendingSet(playerid, 1);
	mysql_tquery(g_DB, query, "DB_OnAccountLoaded", "ds", playerid, name);
	return 0;
}

public DB_OnAccountLoaded(playerid, const name[])
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	DB_PendingClear(playerid, 1);
	if (gPlayer[playerid][pAuthState] != 0)
	{
		return 1;
	}
	new rows = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		Account_OnDBLoaded(playerid, 0, "", "", 0);
		return 1;
	}
	new accountId = 0, adminLevel = 0;
	new hash[65], salt[17];
	cache_get_value_name_int(0, "id", accountId);
	cache_get_value_name(0, "password_hash", hash, 65);
	cache_get_value_name(0, "salt", salt, 17);
	cache_get_value_name_int(0, "admin_level", adminLevel);
	cache_get_value_name_int(0, "hash_algo", g_DBHashAlgo[playerid]);
	cache_get_value_name_int(0, "muted_until", g_DBMutedUntil[playerid]);
	cache_get_value_name_int(0, "locked_until", g_DBLockedUntil[playerid]);
	Account_OnDBLoaded(playerid, accountId, hash, salt, adminLevel);
	return 1;
}

bool:DB_HashFormatOk(const hash[], const salt[], algo)
{
	if (algo == 0)
	{
		return strlen(hash) == 64 && strlen(salt) == 16;
	}
	if (algo == 1)
	{
		return strlen(hash) == 60 && strlen(salt) < 17;
	}
	return 0;
}

DB_CreateAccountEx(playerid, const hash[], const salt[], algo)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || !DB_HashFormatOk(hash, salt, algo))
	{
		DB_DeliverFailure(playerid, 2);
		return 0;
	}
	new name[25], query[512];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "INSERT IGNORE INTO accounts (name, password_hash, salt, hash_algo) VALUES ('%e', '%e', '%e', %d)", name, hash, salt, algo);
	DB_PendingSet(playerid, 2);
	mysql_tquery(g_DB, query, "DB_OnAccountCreated", "ds", playerid, name);
	return 0;
}

public DB_OnAccountCreated(playerid, const name[])
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	DB_PendingClear(playerid, 2);
	if (gPlayer[playerid][pAuthState] != 2)
	{
		return 1;
	}
	new accountId = 0;
	if (cache_affected_rows() == 1)
	{
		accountId = cache_insert_id();
	}
	Account_OnDBCreated(playerid, accountId);
	return 1;
}

DB_UpdateLogin(accountId, const ip[])
{
	if (!DB_IsReady() || accountId <= 0)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE accounts SET last_login = NOW(), last_ip = '%e', failed_logins = 0 WHERE id = %d", ip, accountId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_UpdatePasswordHash(accountId, const hash[], algo)
{
	if (!DB_IsReady() || accountId <= 0)
	{
		return 0;
	}
	new len = strlen(hash);
	if (!((algo == 0 && len == 64) || (algo == 1 && len == 60)))
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE accounts SET password_hash = '%e', hash_algo = %d WHERE id = %d", hash, algo, accountId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_AccountLoginFailed(accountId)
{
	if (!DB_IsReady() || accountId <= 0)
	{
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE accounts SET locked_until = IF(failed_logins + 1 >= %d, UNIX_TIMESTAMP() + %d, locked_until), failed_logins = IF(failed_logins + 1 >= %d, 0, failed_logins + 1) WHERE id = %d", 9, 600, 9, accountId);
	mysql_tquery(g_DB, query, "", "");
	return 0;
}

DB_AccountSetEmail(playerid, accountId, const email[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!DB_IsReady() || accountId <= 0)
	{
		Account_OnDBEmailSet(playerid, -1);
		return 0;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "UPDATE IGNORE accounts SET email = '%e', email_verified = 0, email_token_hash = NULL WHERE id = %d", email, accountId);
	mysql_tquery(g_DB, query, "DB_OnAccountEmailSet", "dds", playerid, accountId, email);
	return 0;
}

public DB_OnAccountEmailSet(playerid, accountId, const email[])
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAccountId] != accountId)
	{
		return 1;
	}
	new query[512];
	mysql_format(g_DB, query, 512, "SELECT (email <=> '%e') AS same FROM accounts WHERE id = %d", email, accountId);
	mysql_tquery(g_DB, query, "DB_OnAccountEmailCheck", "dd", playerid, accountId);
	return 1;
}

public DB_OnAccountEmailCheck(playerid, accountId)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAccountId] != accountId)
	{
		return 1;
	}
	new rows = 0, same = 0;
	cache_get_row_count(rows);
	if (rows < 1)
	{
		Account_OnDBEmailSet(playerid, -1);
		return 1;
	}
	cache_get_value_name_int(0, "same", same);
	Account_OnDBEmailSet(playerid, same ? 1 : 0);
	return 1;
}
