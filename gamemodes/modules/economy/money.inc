
Money_BankLog(playerid, kind[], delta, ref[])
{
	if (playerid < 0 || playerid >= 1000 || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new charId = gPlayer[playerid][PLAYER_DATA_3];
	if (charId <= 0 || delta == 0 || kind[0] == 0)
	{
		return 0;
	}
	new opId[37];
	DB_BankMakeOpId(opId, 37, charId);
	DB_BankLog(opId, charId, 0, kind, delta, gPlayer[playerid][PLAYER_DATA_89], 2, ref);
}

Money_Refresh(playerid)
{
	Char_SyncMoney(playerid);
	Char_RefreshHud(playerid);
}

bool:Money_ValidPlayer(playerid)
{
	return playerid >= 0 && playerid < 1000 && IsPlayerConnected(playerid);
}

bool:Money_Can(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount <= 0)
	{
		return 0;
	}
	return gPlayer[playerid][PLAYER_DATA_88] >= amount;
}

bool:Money_CanReceive(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount <= 0)
	{
		return 0;
	}
	return amount <= 0x3B9ACA00 - gPlayer[playerid][PLAYER_DATA_88];
}

bool:Money_Give(playerid, amount)
{
	if (!Money_CanReceive(playerid, amount))
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_88] += amount;
	Money_Refresh(playerid);
	return 1;
}

bool:Money_Take(playerid, amount)
{
	if (!Money_Can(playerid, amount))
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_88] -= amount;
	Money_Refresh(playerid);
	return 1;
}

bool:Money_Set(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount < 0 || amount > 0x3B9ACA00)
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_88] = amount;
	Money_Refresh(playerid);
	return 1;
}

Money_BankRoom(playerid)
{
	if (!Money_ValidPlayer(playerid))
	{
		return 0;
	}
	return 0x3B9ACA00 - gPlayer[playerid][PLAYER_DATA_89];
}

bool:Money_GiveBank(playerid, amount, kind[], ref[])
{
	if (!Money_ValidPlayer(playerid) || amount <= 0 || amount > Money_BankRoom(playerid))
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_89] += amount;
	Char_RefreshHud(playerid);
	if (kind[0] != 0)
	{
		Money_BankLog(playerid, kind, amount, ref);
	}
	return 1;
}

bool:Money_CanBank(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount <= 0)
	{
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_265] != 0)
	{
		return 0;
	}
	return gPlayer[playerid][PLAYER_DATA_89] >= amount;
}

bool:Money_TakeBank(playerid, amount, kind[], ref[])
{
	if (!Money_CanBank(playerid, amount))
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_89] -= amount;
	Char_RefreshHud(playerid);
	if (kind[0] != 0)
	{
		Money_BankLog(playerid, kind, -amount, ref);
	}
	return 1;
}
