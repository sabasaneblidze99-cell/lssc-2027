new g_AuthSeq;

Account_NextSeq(playerid)
{
	g_AuthSeq++;
	if (g_AuthSeq <= 0)
	{
		g_AuthSeq = 1;
	}
	gPlayer[playerid][pAuthSeq] = g_AuthSeq;
	return g_AuthSeq;
}

bool:Account_SeqValid(playerid, seq, phase)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return false;
	}
	return seq != 0 && gPlayer[playerid][pAuthSeq] == seq && gPlayer[playerid][pAuthState] == phase;
}
new g_AccPendingEmail[MAX_PLAYERS][255];

Account_Kick(playerid, const reason[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pAuthState] == 10)
	{
		return 0;
	}
	Util_Send(playerid, -10270721, reason);
	gPlayer[playerid][pAuthState] = 10;
	UI_CloseDialogs(playerid);
	gPlayer[playerid][pKickTimer] = SetTimerEx("Account_OnKickTimer", 750, false, "d", playerid);
	return 0;
}

public Account_OnKickTimer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 1;
	}
	gPlayer[playerid][pKickTimer] = 0;
	Kick(playerid);
	return 1;
}

Account_ShowPrompt(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayer[playerid][pClassRequested])
	{
		return 0;
	}
	if (!CefUI_AuthGate(playerid))
	{
		return 0;
	}
	switch (gPlayer[playerid][pAuthState])
	{
		case 1:
		{
			UI_ShowRegister(playerid);
		}
		case 3:
		{
			UI_ShowLogin(playerid, gPlayer[playerid][pLoginAttempts] > 0);
		}
		case 5:
		{
			UI_ShowGenderSelect(playerid);
		}
		case 6:
		{
			UI_ShowSkinSelect(playerid, gPlayer[playerid][pGender]);
		}
	}
	return 0;
}

Account_OnConnect(playerid)
{
	g_DBPending[playerid] = 0;
	gPlayer[playerid][pAuthState] = 0;
	gPlayer[playerid][pLastReportTick] = GetTickCount() - 30000;
	gPlayer[playerid][pLastGiveMoneyTick] = GetTickCount() - 10000;
	new name[25];
	GetPlayerName(playerid, name, 25);
	if (1 && !Util_IsValidRPName(name))
	{
		Account_Kick(playerid, "სახელი უნდა იყოს ფორმატში სახელი_გვარი (მაგ. Giorgi_Beridze).");
		return 0;
	}
	if (!DB_IsReady())
	{
		Log_Write("ACCOUNT", "connessione rifiutata: database non disponibile");
		Account_Kick(playerid, "სერვისი დროებით მიუწვდომელია. სცადე მოგვიანებით.");
		return 0;
	}
	new ip[16];
	GetPlayerIp(playerid, ip, 16);
	gPlayer[playerid][pAuthState] = 11;
	DB_CheckBan(playerid, name, ip);
	return 0;
}

Account_ContinueConnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pAuthState] != 11)
	{
		return 0;
	}
	gPlayer[playerid][pAuthState] = 0;
	DB_LoadAccount(playerid);
	return 0;
}

Account_OnDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_AccPendingEmail[playerid][0] = 0;
	if (gPlayer[playerid][pKickTimer] != 0)
	{
		KillTimer(gPlayer[playerid][pKickTimer]);
	}
	g_DBPending[playerid] = 0;
	return 0;
}

public Account_OnDBLoaded(playerid, accountId, const hash[], const salt[], adminLevel)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 0)
	{
		return 1;
	}
	if (accountId <= 0)
	{
		if (accountId == -1 || !DB_IsReady())
		{
			Account_Kick(playerid, "სერვისი დროებით მიუწვდომელია. სცადე მოგვიანებით.");
			return 1;
		}
		gPlayer[playerid][pAuthState] = 1;
	}
	else
	{
		strmid(gPlayer[playerid][pPassHash], hash, 0, 64, 65);
		strmid(gPlayer[playerid][pPassSalt], salt, 0, 16, 17);
		gPlayer[playerid][pAccountId] = accountId;
		gPlayer[playerid][pAdminLevel] = clamp(adminLevel, 0, 3);
		gPlayer[playerid][pHashAlgo] = DB_GetHashAlgo(playerid);
		gPlayer[playerid][pMutedUntil] = DB_GetMutedUntil(playerid);
		gPlayer[playerid][pLoginAttempts] = 0;
		new locked = DB_GetLockedUntil(playerid) - gettime();
		if (locked > 0)
		{
			new msg[512];
			format(msg, 512, "ანგარიში დროებით დაბლოკილია: ძალიან ბევრი არასწორი პაროლი. სცადე %d წუთში.", (locked + 59) / 60);
			Account_Kick(playerid, msg);
			return 1;
		}
		gPlayer[playerid][pAuthState] = 3;
	}
	Account_ShowPrompt(playerid);
	return 1;
}

Account_CompleteLogin(playerid, accountId, adminLevel)
{
	new name[25], ip[16], msg[128];
	GetPlayerName(playerid, name, 25);
	GetPlayerIp(playerid, ip, 16);
	gPlayer[playerid][pAccountId] = accountId;
	gPlayer[playerid][pAdminLevel] = adminLevel;
	gPlayer[playerid][pLoggedIn] = 1;
	gPlayer[playerid][pLoginAttempts] = 0;
	gPlayer[playerid][pPassHash][0] = 0;
	gPlayer[playerid][pPassSalt][0] = 0;
	gPlayer[playerid][pAuthState] = 4;
	DB_UpdateLogin(accountId, ip);
	format(msg, 128, "login account %d (%s)", accountId, name);
	Log_Write("ACCOUNT", msg);
	Util_Send(playerid, 0x66DD66FF, "შესვლა წარმატებულია.");
	CharSel_Begin(playerid);       // M15: selezione dei 3 personaggi (character/char_select.inc)
}

Account_SetPendingEmail(playerid, const email[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_AccPendingEmail[playerid][0] = 0;
	if (email[0] != 0 && Valid_Email(email))
	{
		strcopy(g_AccPendingEmail[playerid], email, 255);
	}
	return 0;
}

public UI_OnRegisterSubmit(playerid, const password[])
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 1)
	{
		return 1;
	}
	if (!Valid_PasswordLength(password))
	{
		new err[512];
		format(err, 512, "პაროლი უნდა შეიცავდეს %d-დან %d-მდე სიმბოლოს.", 6, 32);
		Util_Send(playerid, -10270721, err);
		UI_ShowRegister(playerid);
		return 1;
	}
	gPlayer[playerid][pAuthState] = 12;
	bcrypt_hash(playerid, "Account_OnRegisterHashed", password, 11, "d", Account_NextSeq(playerid));
	return 1;
}

public Account_OnRegisterHashed(playerid, seq)
{
	if (!Account_SeqValid(playerid, seq, 12))
	{
		return 1;
	}
	gPlayer[playerid][pAuthSeq] = 0;
	new hash[61];
	bcrypt_get_hash(hash, 61);
	if (strlen(hash) != 60)
	{
		Log_Write("ACCOUNT", "registrazione fallita: hash bcrypt non valido");
		Account_Kick(playerid, "რეგისტრაცია ვერ მოხერხდა. სცადე მოგვიანებით.");
		return 1;
	}
	gPlayer[playerid][pAuthState] = 2;
	DB_CreateAccountEx(playerid, hash, "", 1);
	return 1;
}

public Account_OnDBCreated(playerid, accountId)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 2)
	{
		return 1;
	}
	if (accountId <= 0)
	{
		Log_Write("ACCOUNT", "registrazione fallita");
		Account_Kick(playerid, "რეგისტრაცია ვერ მოხერხდა. სცადე მოგვიანებით.");
		return 1;
	}
	Util_Send(playerid, 0x66DD66FF, "რეგისტრაცია დასრულებულია.");
	Account_CompleteLogin(playerid, accountId, 0);
	if (g_AccPendingEmail[playerid][0] != 0)
	{
		DB_AccountSetEmail(playerid, accountId, g_AccPendingEmail[playerid]);
		g_AccPendingEmail[playerid][0] = 0;
	}
	return 1;
}

Account_LoginFailed(playerid)
{
	gPlayer[playerid][pAuthState] = 3;
	gPlayer[playerid][pAuthSeq] = 0;
	gPlayer[playerid][pLoginAttempts]++;
	new name[25], msg[128];
	GetPlayerName(playerid, name, 25);
	format(msg, 128, "login fallito %s (tentativo %d/%d)", name, gPlayer[playerid][pLoginAttempts], 3);
	Log_Write("ACCOUNT", msg);
	DB_AccountLoginFailed(gPlayer[playerid][pAccountId]);
	if (gPlayer[playerid][pLoginAttempts] >= 3)
	{
		Account_Kick(playerid, "შესვლის ძალიან ბევრი წარუმატებელი მცდელობა.");
		return 0;
	}
	new err[512];
	format(err, 512, "პაროლი არასწორია. დარჩენილი მცდელობა: %d.", 3 - gPlayer[playerid][pLoginAttempts]);
	Util_Send(playerid, -10270721, err);
	UI_ShowLogin(playerid, true);
	return 0;
}

public UI_OnLoginSubmit(playerid, const password[])
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != 3)
	{
		return 1;
	}
	if (!Valid_PasswordLength(password))
	{
		Account_LoginFailed(playerid);
		return 1;
	}
	if (gPlayer[playerid][pHashAlgo] == 1)
	{
		gPlayer[playerid][pAuthState] = 13;
		bcrypt_verify(playerid, "Account_OnLoginVerified", password, gPlayer[playerid][pPassHash], "dd", Account_NextSeq(playerid), gPlayer[playerid][pAccountId]);
		return 1;
	}
	new hash[65];
	SHA256_Hash(password, gPlayer[playerid][pPassSalt], hash, 65);
	if (strcmp(hash, gPlayer[playerid][pPassHash], true, 0x7FFFFFFF) != 0)
	{
		Account_LoginFailed(playerid);
		return 1;
	}
	gPlayer[playerid][pAuthState] = 12;
	bcrypt_hash(playerid, "Account_OnMigrateHashed", password, 11, "dd", Account_NextSeq(playerid), gPlayer[playerid][pAccountId]);
	return 1;
}

public Account_OnLoginVerified(playerid, bool:success, seq, accountId)
{
	if (!Account_SeqValid(playerid, seq, 13) || gPlayer[playerid][pAccountId] != accountId)
	{
		return 1;
	}
	if (!success)
	{
		Account_LoginFailed(playerid);
		return 1;
	}
	gPlayer[playerid][pAuthSeq] = 0;
	Account_CompleteLogin(playerid, accountId, gPlayer[playerid][pAdminLevel]);
	return 1;
}

public Account_OnMigrateHashed(playerid, seq, accountId)
{
	if (!Account_SeqValid(playerid, seq, 12) || gPlayer[playerid][pAccountId] != accountId)
	{
		return 1;
	}
	gPlayer[playerid][pAuthSeq] = 0;
	new hash[61];
	bcrypt_get_hash(hash, 61);
	if (strlen(hash) == 60)
	{
		DB_UpdatePasswordHash(accountId, hash, 1);
		Log_Write("ACCOUNT", "password migrata a bcrypt");
	}
	else
	{
		Log_Write("ACCOUNT", "migrazione bcrypt fallita: resta l'hash legacy");
	}
	Account_CompleteLogin(playerid, accountId, gPlayer[playerid][pAdminLevel]);
	return 1;
}

public UI_OnDialogCancel(playerid, dialogid)
{
	if (!IsPlayerConnected(playerid))
	{
		return 1;
	}
	switch (gPlayer[playerid][pAuthState])
	{
		case 1, 3:
		{
			Account_Kick(playerid, "შესვლა გაუქმებულია.");
		}
		case 5:
		{
			UI_ShowGenderSelect(playerid);
		}
		case 6:
		{
			gPlayer[playerid][pAuthState] = 5;
			UI_ShowGenderSelect(playerid);
		}
	}
	return 1;
}

public Account_OnDBEmailSet(playerid, result)
{
	if (!IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (result == 1)
	{
		Util_Send(playerid, 0x66DD66FF, "ელფოსტა შენახულია. დადასტურება ჯერ არ ხდება: ის მხოლოდ ანგარიშის აღსადგენად გამოიყენება.");
	}
	else if (result == 0)
	{
		Util_Send(playerid, -10270721, "ეს ელფოსტა უკვე სხვა ანგარიშს ეკუთვნის.");
	}
	else
	{
		Util_Send(playerid, -10270721, "ელფოსტის შენახვა ახლა ვერ მოხერხდა. სცადე მოგვიანებით: /email");
	}
	return 1;
}

Account_CmdEmail(playerid, const params[])
{
	new pos = 0, email[255];
	Util_NextToken(params, pos, email, 255);
	if (email[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /email მისამართი@მაგალითი.ge (არასავალდებულო, ანგარიშის აღსადგენად)");
		return 0;
	}
	if (!Valid_Email(email))
	{
		Util_Send(playerid, -10270721, "ელფოსტის მისამართი არასწორია.");
		return 0;
	}
	DB_AccountSetEmail(playerid, gPlayer[playerid][pAccountId], email);
	return 0;
}
