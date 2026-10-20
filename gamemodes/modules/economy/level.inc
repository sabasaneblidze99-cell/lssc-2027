
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
	SendClientMessage(playerid, -2994689, "Complimenti! Hai raggiunto il livello %d.", gPlayer[playerid][pLevel]);
}

bool:Level_AddExp(playerid, amount)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid) || amount <= 0)
	{
		return 0;
	}
	new maxExp = Level_ExpForLevel(50), exp = gPlayer[playerid][pExp];
	exp = amount > maxExp - exp ? maxExp : exp + amount;
	gPlayer[playerid][pExp] = exp;
	new newLevel = Level_FromExp(exp), bool:up = newLevel > gPlayer[playerid][pLevel];
	if (up)
	{
		gPlayer[playerid][pLevel] = newLevel;
		Level_Announce(playerid);
	}
	Char_RefreshHud(playerid);
	return up;
}

bool:Level_Set(playerid, level)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (level < 1 || level > 50)
	{
		return 0;
	}
	gPlayer[playerid][pLevel] = level;
	gPlayer[playerid][pExp] = Level_ExpForLevel(level);
	Char_RefreshHud(playerid);
	return 1;
}
