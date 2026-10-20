new MySQL:g_DB;
new g_DBPending[MAX_PLAYERS];
new g_DBAdminPending[1001];
new g_DBFindTag[1001];
new g_DBHashAlgo[MAX_PLAYERS];
new g_DBMutedUntil[MAX_PLAYERS];
new g_DBLockedUntil[MAX_PLAYERS];

bool:DB_IsReady()
{
	return g_DB != 0;
}

DB_IniTrim(str[])
{
	new len = strlen(str);
	while (len > 0 && str[len - 1] <= 32)
	{
		str[--len] = 0;
	}
	new s = 0;
	while (str[s] && str[s] <= 32)
	{
		s++;
	}
	if (s > 0)
	{
		strdel(str, 0, s);
	}
}

bool:DB_IniRead(path[], host[], user[], pass[], dbname[], &port, &mask, size)
{
	new File:f = fopen(path, 0);
	if (!f)
	{
		return 0;
	}
	new line[320], key[32], val[128], bool:first = 1;
	host[0] = user[0] = pass[0] = dbname[0] = 0;
	port = 3306;
	mask = 0;
	while (fread(f, line, 320, 0))
	{
		if (first)
		{
			first = 0;
			if ((line[0] & 255) == 239 && (line[1] & 255) == 187 && (line[2] & 255) == 191)
			{
				strdel(line, 0, 3);
			}
			else if (line[0] == 65279)
			{
				strdel(line, 0, 1);
			}
		}
		new eq = strfind(line, "=", 0, 0);
		if (eq < 1)
		{
			continue;
		}
		strmid(key, line, 0, eq, 32);
		strmid(val, line, eq + 1, strlen(line), 128);
		DB_IniTrim(key);
		DB_IniTrim(val);
		if (!key[0] || key[0] == 59 || key[0] == 35)
		{
			continue;
		}
		if (!strcmp(key, "hostname", 1, 0x7FFFFFFF))
		{
			strcopy(host, val, size);
			mask |= 1;
		}
		else if (!strcmp(key, "username", 1, 0x7FFFFFFF))
		{
			strcopy(user, val, size);
			mask |= 2;
		}
		else if (!strcmp(key, "password", 1, 0x7FFFFFFF))
		{
			strcopy(pass, val, size);
		}
		else if (!strcmp(key, "database", 1, 0x7FFFFFFF))
		{
			strcopy(dbname, val, size);
			mask |= 4;
		}
		else if (!strcmp(key, "server_port", 1, 0x7FFFFFFF) || !strcmp(key, "port", 1, 0x7FFFFFFF))
		{
			new p = strval(val);
			if (p > 0 && p < 65536)
			{
				port = p;
			}
		}
	}
	fclose(f);
	return 1;
}

DB_ConnectHint(errno, const errtxt[])
{
	switch (errno)
	{
		case 1045:
		{
			print("[DB] suggerimento: accesso negato. Controlla username/password in mysql.ini e che l'utente possa collegarsi da quell'host (es. 'root'@'127.0.0.1').");
		}
		case 1049:
		{
			print("[DB] suggerimento: il database indicato in mysql.ini non esiste. Crealo (CREATE DATABASE ... CHARACTER SET utf8mb4) e importa sql/schema.sql.");
		}
		case 2002, 2003, 2005:
		{
			print("[DB] suggerimento: server MySQL non raggiungibile. Verifica che sia avviato, hostname e server_port (default 3306) in mysql.ini e il firewall.");
		}
		case 1251, 2059, 2061:
		{
			print("[DB] suggerimento: plugin di autenticazione non supportato (MySQL 8: caching_sha2_password). Esegui: ALTER USER 'utente'@'host' IDENTIFIED WITH mysql_native_password BY 'password'; FLUSH PRIVILEGES;");
		}
		default:
		{
			if (strfind(errtxt, "authentication", 1, 0) != -1 || strfind(errtxt, "caching_sha2", 1, 0) != -1)
			{
				print("[DB] suggerimento: possibile problema di autenticazione MySQL 8: ALTER USER 'utente'@'host' IDENTIFIED WITH mysql_native_password BY 'password'; FLUSH PRIVILEGES;");
			}
			else if (errno == 0)
			{
				print("[DB] suggerimento: nessun errore dal plugin. Verifica che mysql.ini sia nella cartella di omp-server (non 'mysql.ini.txt': Windows nasconde le estensioni) e che il plugin mysql sia caricato.");
			}
		}
	}
}

bool:DB_ConnectFinish(MySQL:h, const where[])
{
	new errbuf[128], errno = 0;
	if (h != 0)
	{
		errno = mysql_errno(h);
	}
	if (h == 0 || errno != 0)
	{
		errbuf[0] = 0;
		mysql_error(errbuf, 128, h);
		printf("[DB] connessione fallita (%s): errno %d, %s", where, errno, errbuf[0] ? errbuf : "(nessun messaggio dal plugin)");
		DB_ConnectHint(errno, errbuf);
		if (h != 0)
		{
			mysql_close(h);
		}
		return 0;
	}
	g_DB = h;
	mysql_set_charset("utf8mb4", g_DB);
	DB_AlignCollation();
	print("[DB] connesso (utf8mb4)");
	return 1;
}

DB_AlignCollation()
{
	mysql_tquery(g_DB, "SET NAMES utf8mb4 COLLATE utf8mb4_unicode_ci", "", "");
}

bool:DB_Connect()
{
	if (DB_IsReady())
	{
		return 1;
	}
	new host[128], user[128], pass[128], dbname[128], port = 0, mask = 0;
	new bool:read = DB_IniRead("mysql.ini", host, user, pass, dbname, port, mask, 128);
	if (!read)
	{
		read = DB_IniRead("../mysql.ini", host, user, pass, dbname, port, mask, 128);
	}
	if (!read)
	{
		new MySQL:h = mysql_connect_file("mysql.ini");
		if (h != 0 && mysql_errno(h) == 0)
		{
			g_DB = h;
			mysql_set_charset("utf8mb4", g_DB);
			DB_AlignCollation();
			print("[DB] connesso (utf8mb4, via mysql_connect_file)");
			return 1;
		}
		printf("[DB] connessione con '%s' fallita: il file va nella radice del server (accanto a omp-server) oppure in scriptfiles/.", "mysql.ini");
		print("[DB] attenzione: su Windows controlla che non si chiami 'mysql.ini.txt' (estensioni nascoste) e che le chiavi siano hostname, username, password, database, server_port.");
		DB_ConnectFinish(h, "mysql_connect_file");
		return 0;
	}
	if (mask != 7)
	{
		printf("[DB] %s: chiavi mancanti o vuote:%s%s%s (servono hostname, username, database; password e server_port opzionali).", "mysql.ini", mask & 1 ? "" : " hostname", mask & 2 ? "" : " username", mask & 4 ? "" : " database");
		return 0;
	}
	new MySQLOpt:opts = mysql_init_options();
	mysql_set_option(opts, 3, port);
	new MySQL:h2 = mysql_connect(host, user, pass, dbname, opts);
	pass[0] = 0;
	new where[96];
	format(where, 96, "%s@%s:%d db=%s", user, host, port, dbname);
	return DB_ConnectFinish(h2, where);
}

DB_Shutdown()
{
	if (!DB_IsReady())
	{
		return 0;
	}
	mysql_close(g_DB);
	g_DB = 0;
	print("[DB] connessione chiusa");
}

bool:DB_PlayerMatches(playerid, const name[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new cur[25];
	GetPlayerName(playerid, cur, 25);
	return strcmp(cur, name, 0, 0x7FFFFFFF) == 0;
}

DB_PendingSet(playerid, op)
{
	g_DBPending[playerid] |= op;
}

DB_PendingClear(playerid, op)
{
	g_DBPending[playerid] &= ~op;
}

DB_DeliverFailure(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	DB_PendingClear(playerid, op);
	if (!IsPlayerConnected(playerid))
	{
		return 0;
	}
	switch (op)
	{
		case 1:
		{
			Account_OnDBLoaded(playerid, -1, "", "", 0);
		}
		case 2:
		{
			Account_OnDBCreated(playerid, 0);
		}
		case 4:
		{
			if (funcidx("Char_OnDBError") != -1)
			{
				CallLocalFunction("Char_OnDBError", "d", playerid);
			}
			else
			{
				Char_OnDBLoaded(playerid, 0);
			}
		}
		case 8:
		{
			Char_OnDBCreated(playerid, 0);
		}
		case 16:
		{
			if (funcidx("Admin_OnBanChecked") != -1)
			{
				CallLocalFunction("Admin_OnBanChecked", "ddsd", playerid, 0, "", 0);
			}
		}
		case 32:
		{
			if (funcidx("Veh_OnDBLoaded") != -1)
			{
				CallLocalFunction("Veh_OnDBLoaded", "dd", playerid, -1);
			}
		}
		case 256:
		{
			if (funcidx("Veh_OnDBCounted") != -1)
			{
				CallLocalFunction("Veh_OnDBCounted", "dd", playerid, -1);
			}
		}
		case 64:
		{
			if (funcidx("Veh_OnDBCreated") != -1)
			{
				CallLocalFunction("Veh_OnDBCreated", "dds", playerid, 0, "");
			}
		}
		case 128:
		{
			if (funcidx("Veh_OnDBDeleted") != -1)
			{
				CallLocalFunction("Veh_OnDBDeleted", "ddd", playerid, 0, -1);
			}
		}
		case 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072:
		{
			if (funcidx("DB_HouseFailure") != -1)
			{
				CallLocalFunction("DB_HouseFailure", "ddd", playerid, op, -1);
			}
		}
		case 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216:
		{
			if (funcidx("DB_JobFacFailure") != -1)
			{
				CallLocalFunction("DB_JobFacFailure", "dd", playerid, op);
			}
		}
		case 33554432, 67108864:
		{
			if (funcidx("DB_InvFailure") != -1)
			{
				CallLocalFunction("DB_InvFailure", "dd", playerid, op);
			}
		}
		case 134217728, 268435456:
		{
			if (funcidx("DB_BankFailure") != -1)
			{
				CallLocalFunction("DB_BankFailure", "dd", playerid, op);
			}
		}
		case 536870912, 1073741824:
		{
			if (funcidx("DB_StoryFailure") != -1)
			{
				CallLocalFunction("DB_StoryFailure", "dd", playerid, op);
			}
		}
	}
}

DB_DeliverAdminFailure(slot, op)
{
	if (slot < 0 || slot > 1000)
	{
		return 0;
	}
	g_DBAdminPending[slot] &= ~op;
	new req = slot == 1000 ? 65535 : slot;
	if (req != INVALID_PLAYER_ID && !IsPlayerConnected(req))
	{
		return 0;
	}
	switch (op)
	{
		case 1:
		{
			if (funcidx("Admin_OnWarnAdded") != -1)
			{
				CallLocalFunction("Admin_OnWarnAdded", "ddd", req, 0, -1);
			}
		}
		case 2:
		{
			if (funcidx("Admin_OnBanAdded") != -1)
			{
				CallLocalFunction("Admin_OnBanAdded", "dsd", req, "", 0);
			}
		}
		case 4:
		{
			if (funcidx("Admin_OnUnbanned") != -1)
			{
				CallLocalFunction("Admin_OnUnbanned", "dsd", req, "", -1);
			}
		}
		case 8:
		{
			if (funcidx("Admin_OnAccountFound") != -1)
			{
				CallLocalFunction("Admin_OnAccountFound", "dddsds", req, g_DBFindTag[slot], -1, "", 0, "");
			}
		}
	}
}

DB_FailAllAdminPending(op)
{
	for (new i = 0; i <= 1000; i++)
	{
		if (g_DBAdminPending[i] & op)
		{
			DB_DeliverAdminFailure(i, op);
		}
	}
}

DB_FailAllPending(op)
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_DBPending[i] & op)
		{
			DB_DeliverFailure(i, op);
		}
	}
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
	printf("[DB] errore %d (callback '%s'): %s", errorid, callback, error);
	if (!strcmp(callback, "DB_OnAccountLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(1);
	}
	else if (!strcmp(callback, "DB_OnAccountCreated", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(2);
	}
	else if (!strcmp(callback, "DB_OnCharLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(4);
	}
	else if (!strcmp(callback, "DB_OnCharCreated", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(8);
	}
	else if (!strcmp(callback, "DB_OnBanChecked", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(16);
	}
	else if (!strcmp(callback, "DB_OnVehiclesLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(32);
	}
	else if (!strcmp(callback, "DB_OnVehiclesCounted", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(256);
	}
	else if (!strcmp(callback, "DB_OnVehicleCreated", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(64);
	}
	else if (!strcmp(callback, "DB_OnVehicleDeleted", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(128);
	}
	else if (!strcmp(callback, "DB_OnHouseBought", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(512);
	}
	else if (!strcmp(callback, "DB_OnHouseSold", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(1024);
	}
	else if (!strcmp(callback, "DB_OnHouseKeyAdded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(2048);
	}
	else if (!strcmp(callback, "DB_OnHouseKeyRemoved", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(4096);
	}
	else if (!strcmp(callback, "DB_OnHouseKeysLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(8192);
	}
	else if (!strcmp(callback, "DB_OnHouseBillsLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(16384);
	}
	else if (!strcmp(callback, "DB_OnHouseBillsPaid", 0, 0x7FFFFFFF) || !strcmp(callback, "DB_OnHouseRestored", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(32768);
	}
	else if (!strcmp(callback, "DB_OnHouseFoodLoaded", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(65536);
	}
	else if (!strcmp(callback, "DB_OnHouseStored", 0, 0x7FFFFFFF))
	{
		DB_FailAllPending(131072);
	}
	else if (!strcmp(callback, "DB_OnHouse", 0, 10) && funcidx("DB_HouseGlobalFailure") != -1)
	{
		CallLocalFunction("DB_HouseGlobalFailure", "s", callback);
	}
	else if (!strcmp(callback, "DB_OnJF", 0, 7) && funcidx("DB_JobFacQueryError") != -1)
	{
		CallLocalFunction("DB_JobFacQueryError", "s", callback);
	}
	else if (!strcmp(callback, "DB_OnInv", 0, 8) && funcidx("DB_InvQueryError") != -1)
	{
		CallLocalFunction("DB_InvQueryError", "s", callback);
	}
	else if (!strcmp(callback, "DB_OnBankDeltaSaved", 0, 0x7FFFFFFF))
	{
		if (funcidx("DB_BankDeltaFailed") != -1)
		{
			CallLocalFunction("DB_BankDeltaFailed", "");
		}
	}
	else if (!strcmp(callback, "DB_OnBank", 0, 9) && funcidx("DB_BankQueryError") != -1)
	{
		CallLocalFunction("DB_BankQueryError", "s", callback);
	}
	else if (!strcmp(callback, "DB_OnStory", 0, 10) && funcidx("DB_StoryQueryError") != -1)
	{
		CallLocalFunction("DB_StoryQueryError", "s", callback);
	}
	else if (!strcmp(callback, "DB_OnLd", 0, 7) && funcidx("DB_LivingQueryError") != -1)
	{
		CallLocalFunction("DB_LivingQueryError", "s", callback);
	}
	else if (!strcmp(callback, "DB_OnWarnInserted", 0, 0x7FFFFFFF) || !strcmp(callback, "DB_OnWarnCounted", 0, 0x7FFFFFFF))
	{
		DB_FailAllAdminPending(1);
	}
	else if (!strcmp(callback, "DB_OnBanInserted", 0, 0x7FFFFFFF))
	{
		DB_FailAllAdminPending(2);
	}
	else if (!strcmp(callback, "DB_OnUnbanDone", 0, 0x7FFFFFFF))
	{
		DB_FailAllAdminPending(4);
	}
	else if (!strcmp(callback, "DB_OnAccountFound", 0, 0x7FFFFFFF))
	{
		DB_FailAllAdminPending(8);
	}
	return 1;
}
