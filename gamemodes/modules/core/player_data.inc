new gPlayer[MAX_PLAYERS][E_PLAYER_DATA];

Player_Reset(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	for (new i = 0; i < _:E_PLAYER_DATA; i++)
	{
		gPlayer[playerid][E_PLAYER_DATA:i] = 0;
	}
	return 0;
}
