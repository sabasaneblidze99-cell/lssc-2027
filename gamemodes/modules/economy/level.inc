
Level_StepExp(level)
{
	return level * 2 + 2;
}

Level_ExpForLevel(level)
{
	level = clamp(level, 1, 50);
	return (level - 1) * 2 * level / 2 + (level - 1) * 2;
}

Level_FromExp(exp)
{
	new n = 1;
	while (n < 50 && Level_ExpForLevel(n + 1) <= exp)
	{
		n++;
	}
	return n;
}

Level_Announce(playerid)
{
	SendClientMessage(playerid, -2994689, "Complimenti! Hai raggiunto il livello %d.", gPlayer[playerid][PLAYER_DATA_90]);
}

bool:Level_AddExp(playerid, amount)
{
	if (playerid < 0 || playerid >= 1000 || !IsPlayerConnected(playerid) || amount <= 0)
	{
		return 0;
	}
	new maxExp = Level_ExpForLevel(50), exp = gPlayer[playerid][PLAYER_DATA_91];
	exp = amount > maxExp - exp ? maxExp : exp + amount;
	gPlayer[playerid][PLAYER_DATA_91] = exp;
	new newLevel = Level_FromExp(exp), bool:up = newLevel > gPlayer[playerid][PLAYER_DATA_90];
	if (up)
	{
		gPlayer[playerid][PLAYER_DATA_90] = newLevel;
		Level_Announce(playerid);
	}
	Char_RefreshHud(playerid);
	return up;
}

bool:Level_Set(playerid, level)
{
	if (playerid < 0 || playerid >= 1000 || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (level < 1 || level > 50)
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_90] = level;
	gPlayer[playerid][PLAYER_DATA_91] = Level_ExpForLevel(level);
	Char_RefreshHud(playerid);
	return 1;
}
