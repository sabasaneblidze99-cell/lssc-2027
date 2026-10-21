new g_EconomyTimer;

Economy_OnInit()
{
	g_EconomyTimer = SetTimer("Economy_OnTick", 1000, true);
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
	// sostegno cittadino ogni 15 min di gioco attivo (economy/econ_config.inc)
	return min(ECON_PAYCHECK_BASE + level * ECON_PAYCHECK_PER_LEVEL, ECON_PAYCHECK_MAX);
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
	new amount = Economy_PaycheckAmount(gPlayer[playerid][pLevel]);
	new paid = Economy_PaycheckPaid(amount, Money_BankRoom(playerid));
	SendClientMessage(playerid, -2994689, "--- ხელფასი ---");
	if (paid > 0 && Money_GiveBank(playerid, paid, "paycheck", ""))
	{
		SendClientMessage(playerid, 0x66DD66FF, "ხელფასი: %s ჩაირიცხა ბანკში. ბალანსი: %s.", Money_Str(paid), Money_Str(gPlayer[playerid][pBank]));
		if (paid < amount)
		{
			SendClientMessage(playerid, -10270721, "ანგარიში ლიმიტზეა: ხელფასის ნაწილი დაიკარგა.");
		}
	}
	else
	{
		SendClientMessage(playerid, -10270721, "საბანკო ანგარიში ლიმიტზეა: ხელფასი არ ჩაირიცხა.");
	}
	Level_AddExp(playerid, 1);
	Char_Save(playerid);
}

public Economy_OnTick()
{
	new now = GetTickCount();
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!gPlayer[i][pSpawned] || !Util_IsPlaying(i))
		{
			continue;
		}
		if (now - gPlayer[i][pLastActiveTick] > 10000)
		{
			continue;
		}
		gPlayer[i][pSessionMs] += 1000;
		gPlayer[i][pPlayedMs] += 1000;
		gPlayer[i][pPaycheckMs] += 1000;
		if (gPlayer[i][pPaycheckMs] >= 900000)
		{
			gPlayer[i][pPaycheckMs] -= 900000;
			Economy_Paycheck(i);
		}
	}
	return 1;
}
