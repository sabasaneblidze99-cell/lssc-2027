
Money_BankLog(playerid, const kind[], delta, const ref[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new charId = gPlayer[playerid][pCharId];
	if (charId <= 0 || delta == 0 || kind[0] == 0)
	{
		return 0;
	}
	new opId[37];
	DB_BankMakeOpId(opId, 37, charId);
	DB_BankLog(opId, charId, 0, kind, delta, gPlayer[playerid][pBank], 2, ref);
	return 0;
}

Money_Refresh(playerid)
{
	Char_SyncMoney(playerid);
	Char_RefreshHud(playerid);
}

bool:Money_ValidPlayer(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid);
}

bool:Money_Can(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount <= 0)
	{
		return false;
	}
	return gPlayer[playerid][pMoney] >= amount;
}

bool:Money_CanReceive(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount <= 0)
	{
		return false;
	}
	return amount <= 0x3B9ACA00 - gPlayer[playerid][pMoney];
}

bool:Money_Give(playerid, amount)
{
	if (!Money_CanReceive(playerid, amount))
	{
		return false;
	}
	gPlayer[playerid][pMoney] += amount;
	Money_Refresh(playerid);
	return true;
}

bool:Money_Take(playerid, amount)
{
	if (!Money_Can(playerid, amount))
	{
		return false;
	}
	gPlayer[playerid][pMoney] -= amount;
	Money_Refresh(playerid);
	return true;
}

bool:Money_Set(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount < 0 || amount > 0x3B9ACA00)
	{
		return false;
	}
	gPlayer[playerid][pMoney] = amount;
	Money_Refresh(playerid);
	return true;
}

Money_BankRoom(playerid)
{
	if (!Money_ValidPlayer(playerid))
	{
		return 0;
	}
	return 0x3B9ACA00 - gPlayer[playerid][pBank];
}

bool:Money_GiveBank(playerid, amount, const kind[], const ref[])
{
	if (!Money_ValidPlayer(playerid) || amount <= 0 || amount > Money_BankRoom(playerid))
	{
		return false;
	}
	gPlayer[playerid][pBank] += amount;
	Char_RefreshHud(playerid);
	if (kind[0] != 0)
	{
		Money_BankLog(playerid, kind, amount, ref);
	}
	return true;
}

bool:Money_CanBank(playerid, amount)
{
	if (!Money_ValidPlayer(playerid) || amount <= 0)
	{
		return false;
	}
	if (gPlayer[playerid][pBankOpBusy] != 0)
	{
		return false;
	}
	return gPlayer[playerid][pBank] >= amount;
}

bool:Money_TakeBank(playerid, amount, const kind[], const ref[])
{
	if (!Money_CanBank(playerid, amount))
	{
		return false;
	}
	gPlayer[playerid][pBank] -= amount;
	Char_RefreshHud(playerid);
	if (kind[0] != 0)
	{
		Money_BankLog(playerid, kind, -amount, ref);
	}
	return true;
}
