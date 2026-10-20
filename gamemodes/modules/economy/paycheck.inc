new g_EconomyTimer;

Economy_OnInit()
{
	g_EconomyTimer = SetTimer("Economy_OnTick", 1000, 1);
}

Economy_OnExit()
{
	if (g_EconomyTimer != 0)
	{
		KillTimer(g_EconomyTimer);
	}
	g_EconomyTimer = 0;
}

Economy_PaycheckAmount(level)
{
	return level * 25 + 300;
}

Economy_PaycheckPaid(amount, room)
{
	if (room <= 0 || amount <= 0)
	{
		return 0;
	}
	return amount > room ? room : amount;
}

Economy_Paycheck(playerid)
{
	new amount = Economy_PaycheckAmount(gPlayer[playerid][PLAYER_DATA_90]);
	new paid = Economy_PaycheckPaid(amount, Money_BankRoom(playerid));
	SendClientMessage(playerid, -2994689, "--- Stipendio ---");
	if (paid > 0 && Money_GiveBank(playerid, paid, "paycheck", ""))
	{
		SendClientMessage(playerid, 0x66DD66FF, "Stipendio: $%d versato in banca. Saldo: $%d.", paid, gPlayer[playerid][PLAYER_DATA_89]);
		if (paid < amount)
		{
			SendClientMessage(playerid, -10270721, "Il conto ha raggiunto il limite: parte dello stipendio e' andata persa.");
		}
	}
	else
	{
		SendClientMessage(playerid, -10270721, "Il conto bancario e' al limite: stipendio non versato.");
	}
	Level_AddExp(playerid, 1);
	Char_Save(playerid);
}

public Economy_OnTick()
{
	new now = GetTickCount();
	for (new i = 0; i < 1000; i++)
	{
		if (!gPlayer[i][PLAYER_DATA_1] || !Util_IsPlaying(i))
		{
			continue;
		}
		if (now - gPlayer[i][PLAYER_DATA_114] > 10000)
		{
			continue;
		}
		gPlayer[i][PLAYER_DATA_112] += 1000;
		gPlayer[i][PLAYER_DATA_113] += 1000;
		gPlayer[i][PLAYER_DATA_111] += 1000;
		if (gPlayer[i][PLAYER_DATA_111] >= 900000)
		{
			gPlayer[i][PLAYER_DATA_111] -= 900000;
			Economy_Paycheck(i);
		}
	}
	return 1;
}
