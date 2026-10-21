new g_UiHsTierName[4][40] = {
	"სტუდიო",
	"ოროთახიანი",
	"საშუალო სახლი",
	"ვილა"
};
new g_UiHsTierLevel[4] = {1, 2, 5, 10};
new g_UiHsTierFridge[4] = {10, 16, 24, 40};
new g_UiHsIntName[8][40] = {
	"Denise",
	"Katie",
	"Ryder-ის სახლი",
	"Sweet-ის სახლი",
	"საშუალო სახლი A",
	"Johnson-ების სახლი",
	"საშუალო სახლი B",
	"Madd Dogg-ის ვილა"
};
new g_UiHsHouse[MAX_PLAYERS];
new g_UiHsRole[MAX_PLAYERS];
new g_UiHsTier[MAX_PLAYERS];
new g_UiHsInt[MAX_PLAYERS];
new bool:g_UiHsLocked[MAX_PLAYERS];
new g_UiHsLight[MAX_PLAYERS];
new g_UiHsGas[MAX_PLAYERS];
new g_UiHsFridge[MAX_PLAYERS];
new g_UiHsBag[MAX_PLAYERS];
new g_UiHsKeys[MAX_PLAYERS];
new g_UiHsLevel[MAX_PLAYERS];
new g_UiHsMoney[MAX_PLAYERS];
new g_UiHsBank[MAX_PLAYERS];
new g_UiHsPrice[MAX_PLAYERS];      // prezzo di vendita della casa (DB), in tetri
new g_UiHsRefund[MAX_PLAYERS];     // rimborso se il proprietario la rivende
new g_UiHsLightDue[MAX_PLAYERS];
new g_UiHsLightCnt[MAX_PLAYERS];
new g_UiHsGasDue[MAX_PLAYERS];
new g_UiHsGasCnt[MAX_PLAYERS];
new g_UiHsItemAct[MAX_PLAYERS][10];
new g_UiHsAmtAction[MAX_PLAYERS];
new g_UiHsAmtMax[MAX_PLAYERS];

UI_HsMoney(value, out[], size)
{
	// importo in tetri -> "1.250,50" (economy/currency.inc)
	return Money_Format(value, out, size);
}

UI_HsValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

UI_HsFridgeCap(playerid)
{
	new t = g_UiHsTier[playerid];
	return g_UiHsTierFridge[t];
}

UI_HsUtilText(ustate, out[], size)
{
	switch (ustate)
	{
		case 1:
		{
			format(out, size, "{66DD66}ON");
		}
		case 2:
		{
			format(out, size, "{FF6666}გათიშული");
		}
		default:
		{
			format(out, size, "{FFD24D}გამორთ.");
		}
	}
}

// prezzo vero della casa e rimborso di rivendita (chiamata da house.inc subito dopo UI_HouseDataSet)
UI_HousePriceSet(playerid, price, refund)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	g_UiHsPrice[playerid] = price < 0 ? 0 : price;
	g_UiHsRefund[playerid] = refund < 0 ? 0 : refund;
	return 1;
}

UI_HsTitle(playerid, const prefix[], out[], size)
{
	format(out, size, "%s - სახლი %d", prefix, g_UiHsHouse[playerid]);
}

UI_HouseDataSet(playerid, houseId, role, tier, interiorIdx, bool:locked, lightState, gasState, fridgeUnits, bagUnits, keysUsed, level, money, bank)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	if (tier < 0)
	{
		tier = 0;
	}
	if (tier >= 4)
	{
		tier = 3;
	}
	if (interiorIdx < 0)
	{
		interiorIdx = 0;
	}
	if (interiorIdx >= 8)
	{
		interiorIdx = 7;
	}
	if (role < 0 || role > 3)
	{
		role = 3;
	}
	g_UiHsHouse[playerid] = houseId;
	g_UiHsRole[playerid] = role;
	g_UiHsTier[playerid] = tier;
	g_UiHsInt[playerid] = interiorIdx;
	g_UiHsLocked[playerid] = locked;
	g_UiHsLight[playerid] = lightState;
	g_UiHsGas[playerid] = gasState;
	g_UiHsFridge[playerid] = fridgeUnits < 0 ? 0 : fridgeUnits;
	g_UiHsBag[playerid] = bagUnits < 0 ? 0 : bagUnits;
	g_UiHsKeys[playerid] = keysUsed;
	g_UiHsLevel[playerid] = level;
	g_UiHsMoney[playerid] = money;
	g_UiHsBank[playerid] = bank;
	return 0;
}

UI_HouseBillsSet(playerid, lightDue, lightCount, gasDue, gasCount)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	g_UiHsLightDue[playerid] = lightDue < 0 ? 0 : lightDue;
	g_UiHsLightCnt[playerid] = lightCount < 0 ? 0 : lightCount;
	g_UiHsGasDue[playerid] = gasDue < 0 ? 0 : gasDue;
	g_UiHsGasCnt[playerid] = gasCount < 0 ? 0 : gasCount;
	return 0;
}

UI_HsBillsTotal(playerid)
{
	new total = g_UiHsLightDue[playerid] + g_UiHsGasDue[playerid];
	if (g_UiHsLight[playerid] == 2)
	{
		total += 40;
	}
	if (g_UiHsGas[playerid] == 2)
	{
		total += 40;
	}
	return total;
}

UI_HsMenuRow(playerid, &n, act, const line[], body[], size)
{
	if (n >= 10)
	{
		return 0;
	}
	g_UiHsItemAct[playerid][n++] = act;
	strcat(body, line, size);
	return 0;
}

UI_HouseMenuShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[1024], line[480], title[576], price[48], tmp[48], st[72];
	new n = 0, t = g_UiHsTier[playerid], role = g_UiHsRole[playerid];
	for (new i = 0; i < 10; i++)
	{
		g_UiHsItemAct[playerid][i] = 0;
	}
	UI_HsMoney(g_UiHsPrice[playerid], price, 48);
	format(title, 576, "სახლი %d - %s", g_UiHsHouse[playerid], g_UiHsIntName[g_UiHsInt[playerid]]);
	format(line, 480, "{AAAAAA}ინტერიერი: {FFFFFF}%s (%s)\n", g_UiHsIntName[g_UiHsInt[playerid]], g_UiHsTierName[t]);
	UI_HsMenuRow(playerid, n, 0, line, body, 1024);
	format(line, 480, "{AAAAAA}ფასი: {66DD66}₾%s{AAAAAA}  (მინ. დონე %d)\n", price, g_UiHsTierLevel[t]);
	UI_HsMenuRow(playerid, n, 0, line, body, 1024);
	if (role == 0)
	{
		UI_HsMenuRow(playerid, n, 0, "{AAAAAA}სტატუსი: {FFD24D}იყიდება\n", body, 1024);
		UI_HsMenuRow(playerid, n, 100, "{66DD66}> იყიდე ეს სახლი\n", body, 1024);
	}
	else if (role == 3)
	{
		UI_HsMenuRow(playerid, n, 0, "{AAAAAA}სტატუსი: {FF6666}კერძო საკუთრება\n", body, 1024);
	}
	else
	{
		UI_HsMenuRow(playerid, n, 0, role == 1 ? "{AAAAAA}სტატუსი: {66DD66}შენი საკუთრება\n" : "{AAAAAA}სტატუსი: {66AAFF}სტუმარი გასაღებით\n", body, 1024);
		UI_HsUtilText(g_UiHsLight[playerid], st, 72);
		format(line, 480, "{AAAAAA}შუქი: %s{AAAAAA}   გაზი: ", st);
		UI_HsUtilText(g_UiHsGas[playerid], st, 72);
		strcat(line, st, 480);
		strcat(line, "\n", 480);
		UI_HsMenuRow(playerid, n, 0, line, body, 1024);
		new bills = g_UiHsLightCnt[playerid] + g_UiHsGasCnt[playerid];
		new due = g_UiHsLightDue[playerid] + g_UiHsGasDue[playerid];
		UI_HsMoney(due, tmp, 48);
		if (role == 1)
		{
			format(line, 480, "{AAAAAA}გადასახადები: %s%d გადასახდელი (₾%s)\n", bills > 0 ? "{FF6666}" : "{66DD66}", bills, tmp);
			UI_HsMenuRow(playerid, n, 0, line, body, 1024);
			format(line, 480, "{FFFFFF}საკეტი: %s{FFFFFF} (შესაცვლელად დააჭირე)\n", g_UiHsLocked[playerid] ? "{FF6666}ჩაკეტილი" : "{66DD66}ღია");
			UI_HsMenuRow(playerid, n, 1, line, body, 1024);
		}
		UI_HsMenuRow(playerid, n, 2, "{FFFFFF}კომუნალური (შუქი / გაზი)\n", body, 1024);
		format(line, 480, "{FFFFFF}მაცივარი ({FFD24D}%d/%d{FFFFFF} პორცია)\n", g_UiHsFridge[playerid], UI_HsFridgeCap(playerid));
		UI_HsMenuRow(playerid, n, 3, line, body, 1024);
		if (role == 1)
		{
			UI_HsMenuRow(playerid, n, 4, "{FFFFFF}გადასახადები\n", body, 1024);
			format(line, 480, "{FFFFFF}სტუმრის გასაღებები ({FFD24D}%d/%d{FFFFFF})\n", g_UiHsKeys[playerid], 3);
			UI_HsMenuRow(playerid, n, 5, line, body, 1024);
			UI_HsMenuRow(playerid, n, 6, "{FF6666}სახლის გაყიდვა\n", body, 1024);
		}
	}
	Lang_ShowPlayerDialog(playerid, 9200, DIALOG_STYLE_LIST, title, body, "არჩევა", "დახურვა");
	return 0;
}

UI_HsBuyPayShow(playerid)
{
	new t = g_UiHsTier[playerid];
	new body[640], price[48], cash[48], bank[48], tl[144], title[576];
	new p = g_UiHsPrice[playerid];
	UI_HsMoney(p, price, 48);
	UI_HsMoney(g_UiHsMoney[playerid], cash, 48);
	UI_HsMoney(g_UiHsBank[playerid], bank, 48);
	format(body, 640, "{FFFFFF}ნაღდი ფული\t%s₾%s\n", g_UiHsMoney[playerid] >= p ? "{66DD66}" : "{FF6666}", cash);
	format(tl, 144, "{FFFFFF}ბანკი\t%s₾%s\n", g_UiHsBank[playerid] >= p ? "{66DD66}" : "{FF6666}", bank);
	strcat(body, tl, 640);
	format(title, 576, "%s ₾%s - გადახდა", g_UiHsTierName[t], price);
	Lang_ShowPlayerDialog(playerid, 9201, DIALOG_STYLE_TABLIST, title, body, "ყიდვა", "უკან");
}

UI_HsSellConfirmShow(playerid)
{
	new body[640], amount[48], title[192];
	UI_HsMoney(g_UiHsRefund[playerid], amount, 48);
	format(body, 640, "{FFFFFF}გაყიდო სახლი {FFD24D}%d{FFFFFF} (%s)?\n\nბანკში მიიღებ დაახლოებით {66DD66}₾%s{FFFFFF}-ს (ფასის %d%%).\nკომუნალური, გასაღებები და გადასახადები განულდება; ოპერაცია შეუქცევადია.", g_UiHsHouse[playerid], g_UiHsIntName[g_UiHsInt[playerid]], amount, ECON_HOUSE_RESALE_PCT);
	UI_HsTitle(playerid, "გაყიდვის დადასტურება", title, 192);
	Lang_ShowPlayerDialog(playerid, 9202, DIALOG_STYLE_MSGBOX, title, body, "გაყიდვა", "გაუქმება");
}

UI_HouseUtilityShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[1024], line[384], st[72], title[192];
	strcat(body, "კომუნალური\tსტატუსი\tმოქმედება\n", 1024);
	UI_HsUtilText(g_UiHsLight[playerid], st, 72);
	switch (g_UiHsLight[playerid])
	{
		case 2:
		{
			format(line, 384, "{FFFFFF}შუქი\t%s\t{FF6666}გადაიხადე გადასახადები\n", st);
		}
		case 1:
		{
			format(line, 384, "{FFFFFF}შუქი\t%s\t{AAAAAA}გამორთვა\n", st);
		}
		default:
		{
			format(line, 384, "{FFFFFF}შუქი\t%s\t{AAAAAA}ჩართვა\n", st);
		}
	}
	strcat(body, line, 1024);
	UI_HsUtilText(g_UiHsGas[playerid], st, 72);
	if (g_UiHsRole[playerid] != 1)
	{
		format(line, 384, "{777777}გაზი\t%s\t{777777}მხოლოდ მფლობელი\n", st);
	}
	else
	{
		switch (g_UiHsGas[playerid])
		{
			case 2:
			{
				format(line, 384, "{FFFFFF}გაზი\t%s\t{FF6666}გადაიხადე გადასახადები\n", st);
			}
			case 1:
			{
				format(line, 384, "{FFFFFF}გაზი\t%s\t{AAAAAA}გამორთვა\n", st);
			}
			default:
			{
				format(line, 384, "{FFFFFF}გაზი\t%s\t{AAAAAA}ჩართვა\n", st);
			}
		}
	}
	strcat(body, line, 1024);
	UI_HsTitle(playerid, "კომუნალური", title, 192);
	Lang_ShowPlayerDialog(playerid, 9203, DIALOG_STYLE_TABLIST_HEADERS, title, body, "შეცვლა", "უკან");
	return 0;
}

UI_HouseFridgeShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[512], line[384], title[192], n = 0;
	new cap = UI_HsFridgeCap(playerid), units = g_UiHsFridge[playerid];
	new bool:owner = g_UiHsRole[playerid] == 1;
	for (new i = 0; i < 10; i++)
	{
		g_UiHsItemAct[playerid][i] = 0;
	}
	format(line, 384, "{AAAAAA}მარაგი: {FFD24D}%d/%d{AAAAAA} პორცია  -  თან: %d/%d\n", units, cap, g_UiHsBag[playerid], 12);
	UI_HsMenuRow(playerid, n, 0, line, body, 512);
	if (g_UiHsLight[playerid] != 1)
	{
		UI_HsMenuRow(playerid, n, 0, "{FF6666}შუქი გამორთულია: მაცივარი არ აციებს, მარაგი ფუჭდება\n", body, 512);
	}
	format(line, 384, "%sცივად ჭამა (+%d სიმაძღრე)\n", units > 0 ? "{FFFFFF}" : "{777777}", 25);
	UI_HsMenuRow(playerid, n, 1, line, body, 512);
	format(line, 384, "%sმომზადება (+%d სიმაძღრე, +%d ჯანმრთელობა%s)\n", units > 0 && g_UiHsGas[playerid] == 1 ? "{FFFFFF}" : "{777777}", 45, 5, g_UiHsGas[playerid] == 1 ? "" : " - გაზი არ მუშაობს");
	UI_HsMenuRow(playerid, n, 2, line, body, 512);
	if (owner)
	{
		format(line, 384, "%sპროდუქტების ჩალაგება მაცივარში\n", g_UiHsBag[playerid] > 0 && units < cap ? "{FFFFFF}" : "{777777}");
		UI_HsMenuRow(playerid, n, 3, line, body, 512);
		format(line, 384, "%sპორციების აღება მაცივრიდან\n", units > 0 && g_UiHsBag[playerid] < 12 ? "{FFFFFF}" : "{777777}");
		UI_HsMenuRow(playerid, n, 4, line, body, 512);
	}
	UI_HsTitle(playerid, "მაცივარი", title, 192);
	Lang_ShowPlayerDialog(playerid, 9204, DIALOG_STYLE_LIST, title, body, "არჩევა", "უკან");
	return 0;
}

UI_HsAmountShow(playerid, action, maxAmount)
{
	new body[576], title[192];
	g_UiHsAmtAction[playerid] = action;
	g_UiHsAmtMax[playerid] = maxAmount;
	format(body, 576, "{FFFFFF}%s\nრაოდენობა (1-%d):", action == 3 ? "მაცივარში ჩასალაგებელი პორციები." : "მაცივრიდან ასაღები პორციები.", maxAmount);
	UI_HsTitle(playerid, action == 3 ? "ჩალაგება" : "აღება", title, 192);
	Lang_ShowPlayerDialog(playerid, 9205, DIALOG_STYLE_INPUT, title, body, "დადასტურება", "უკან");
}

UI_HouseBillsShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[1024], line[480], a[48], b[48], title[192];
	strcat(body, "კომუნალური\tგადასახდელი\tთანხა\tშენიშვნა\n", 1024);
	UI_HsMoney(g_UiHsLightDue[playerid], a, 48);
	format(line, 480, "{FFFFFF}შუქი\t%d\t₾%s\t%s\n", g_UiHsLightCnt[playerid], a, g_UiHsLight[playerid] == 2 ? "{FF6666}გათიშული" : "{AAAAAA}-");
	strcat(body, line, 1024);
	UI_HsMoney(g_UiHsGasDue[playerid], b, 48);
	format(line, 480, "{FFFFFF}გაზი\t%d\t₾%s\t%s\n", g_UiHsGasCnt[playerid], b, g_UiHsGas[playerid] == 2 ? "{FF6666}გათიშული" : "{AAAAAA}-");
	strcat(body, line, 1024);
	new total = UI_HsBillsTotal(playerid);
	UI_HsMoney(total, a, 48);
	new fees = total - g_UiHsLightDue[playerid] - g_UiHsGasDue[playerid];
	if (fees > 0)
	{
		UI_HsMoney(fees, b, 48);
		format(line, 480, "{FFD24D}სულ\t%d\t₾%s\t{AAAAAA}მ.შ. ₾%s აღდგენა\n", g_UiHsLightCnt[playerid] + g_UiHsGasCnt[playerid], a, b);
	}
	else
	{
		format(line, 480, "{FFD24D}სულ\t%d\t₾%s\t{AAAAAA}-\n", g_UiHsLightCnt[playerid] + g_UiHsGasCnt[playerid], a);
	}
	strcat(body, line, 1024);
	UI_HsTitle(playerid, "გადასახადები", title, 192);
	Lang_ShowPlayerDialog(playerid, 9206, DIALOG_STYLE_TABLIST_HEADERS, title, body, "ყველას გადახდა", "უკან");
	return 0;
}

UI_HsBillsPayShow(playerid)
{
	new body[640], total[48], cash[48], bank[48], tl[144], title[576];
	new p = UI_HsBillsTotal(playerid);
	UI_HsMoney(p, total, 48);
	UI_HsMoney(g_UiHsMoney[playerid], cash, 48);
	UI_HsMoney(g_UiHsBank[playerid], bank, 48);
	format(body, 640, "{FFFFFF}ბანკი\t%s₾%s\n", g_UiHsBank[playerid] >= p ? "{66DD66}" : "{FF6666}", bank);
	format(tl, 144, "{FFFFFF}ნაღდი ფული\t%s₾%s\n", g_UiHsMoney[playerid] >= p ? "{66DD66}" : "{FF6666}", cash);
	strcat(body, tl, 640);
	format(title, 576, "გადასახადების გადახდა ₾%s", total);
	Lang_ShowPlayerDialog(playerid, 9207, DIALOG_STYLE_TABLIST, title, body, "გადახდა", "უკან");
}

bool:UI_HsParseAmount(const text[], &value)
{
	new len = strlen(text);
	if (len < 1 || len > 4)
	{
		return false;
	}
	value = 0;
	for (new i = 0; i < len; i++)
	{
		if (text[i] < 48 || text[i] > 57)
		{
			return false;
		}
		value = value * 10 + (text[i] - 48);
	}
	return true;
}

bool:UI_HouseOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	if (!UI_HsValid(playerid))
	{
		return false;
	}
	new houseId = g_UiHsHouse[playerid];
	switch (dialogid)
	{
		case 9200:
		{
			if (!response)
			{
				return true;
			}
			if (listitem < 0 || listitem >= 10 || g_UiHsItemAct[playerid][listitem] == 0)
			{
				UI_HouseMenuShow(playerid);
				return true;
			}
			new act = g_UiHsItemAct[playerid][listitem];
			if (act == 100)
			{
				if (g_UiHsLevel[playerid] < g_UiHsTierLevel[g_UiHsTier[playerid]])
				{
					SendClientMessage(playerid, -10066177, "ამ სახლისთვის შენი დონე ძალიან დაბალია.");
					UI_HouseMenuShow(playerid);
					return true;
				}
				UI_HsBuyPayShow(playerid);
				return true;
			}
			if (act == 6)
			{
				UI_HsSellConfirmShow(playerid);
				return true;
			}
			UI_OnHouseAction(playerid, houseId, act);
			return true;
		}
		case 9201:
		{
			if (!response || listitem < 0 || listitem > 1)
			{
				UI_HouseMenuShow(playerid);
				return true;
			}
			new p = g_UiHsPrice[playerid];
			new funds = listitem == 1 ? g_UiHsBank[playerid] : g_UiHsMoney[playerid];
			if (funds < p)
			{
				SendClientMessage(playerid, -10066177, "ამ გადახდის მეთოდით სახსრები არ გყოფნის.");
				UI_HsBuyPayShow(playerid);
				return true;
			}
			UI_OnHouseBuyConfirm(playerid, houseId, listitem);
			return true;
		}
		case 9202:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return true;
			}
			UI_OnHouseAction(playerid, houseId, 6);
			return true;
		}
		case 9203:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return true;
			}
			if (listitem != 0 && listitem != 1)
			{
				UI_HouseUtilityShow(playerid);
				return true;
			}
			new st = listitem == 0 ? g_UiHsLight[playerid] : g_UiHsGas[playerid];
			if (st == 2)
			{
				SendClientMessage(playerid, -10066177, "კომუნალური გათიშულია დავალიანების გამო: გადაიხადე გადასახადები აღსადგენად.");
				UI_HouseUtilityShow(playerid);
				return true;
			}
			if (listitem == 1 && g_UiHsRole[playerid] != 1)
			{
				SendClientMessage(playerid, -10066177, "გაზის მართვა მხოლოდ მფლობელს შეუძლია.");
				UI_HouseUtilityShow(playerid);
				return true;
			}
			UI_OnHouseUtilityToggle(playerid, houseId, listitem, st == 1 ? 0 : 1);
			return true;
		}
		case 9204:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return true;
			}
			new act = listitem >= 0 && listitem < 10 ? g_UiHsItemAct[playerid][listitem] : 0;
			new units = g_UiHsFridge[playerid], cap = UI_HsFridgeCap(playerid);
			switch (act)
			{
				case 1:
				{
					if (units <= 0)
					{
						SendClientMessage(playerid, -10066177, "მაცივარი ცარიელია.");
						UI_HouseFridgeShow(playerid);
						return true;
					}
					UI_OnHouseFridgeAction(playerid, houseId, 1, 0);
				}
				case 2:
				{
					if (units <= 0)
					{
						SendClientMessage(playerid, -10066177, "მაცივარი ცარიელია.");
						UI_HouseFridgeShow(playerid);
						return true;
					}
					if (g_UiHsGas[playerid] != 1)
					{
						SendClientMessage(playerid, -10066177, "გაზი არ მუშაობს: მომზადება არ შეგიძლია.");
						UI_HouseFridgeShow(playerid);
						return true;
					}
					UI_OnHouseFridgeAction(playerid, houseId, 2, 0);
				}
				case 3:
				{
					new m = g_UiHsBag[playerid];
					if (cap - units < m)
					{
						m = cap - units;
					}
					if (m <= 0)
					{
						SendClientMessage(playerid, -10066177, "ჩასალაგებელი პროდუქტები არ გაქვს ან მაცივარი სავსეა.");
						UI_HouseFridgeShow(playerid);
						return true;
					}
					UI_HsAmountShow(playerid, 3, m);
				}
				case 4:
				{
					new m = units;
					if (12 - g_UiHsBag[playerid] < m)
					{
						m = 12 - g_UiHsBag[playerid];
					}
					if (m <= 0)
					{
						SendClientMessage(playerid, -10066177, "მაცივარი ცარიელია ან ჩანთა სავსეა.");
						UI_HouseFridgeShow(playerid);
						return true;
					}
					UI_HsAmountShow(playerid, 4, m);
				}
				default:
				{
					UI_HouseFridgeShow(playerid);
				}
			}
			return true;
		}
		case 9205:
		{
			if (!response)
			{
				UI_HouseFridgeShow(playerid);
				return true;
			}
			new amount = 0;
			if (!UI_HsParseAmount(inputtext, amount) || amount < 1 || amount > g_UiHsAmtMax[playerid])
			{
				SendClientMessage(playerid, -10066177, "არასწორი რაოდენობა.");
				UI_HsAmountShow(playerid, g_UiHsAmtAction[playerid], g_UiHsAmtMax[playerid]);
				return true;
			}
			UI_OnHouseFridgeAction(playerid, houseId, g_UiHsAmtAction[playerid], amount);
			return true;
		}
		case 9206:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return true;
			}
			if (UI_HsBillsTotal(playerid) <= 0)
			{
				SendClientMessage(playerid, 0x66DD66FF, "გადასახდელი კომუნალური გადასახადები არ გაქვს.");
				UI_HouseBillsShow(playerid);
				return true;
			}
			UI_HsBillsPayShow(playerid);
			return true;
		}
		case 9207:
		{
			if (!response || listitem < 0 || listitem > 1)
			{
				UI_HouseBillsShow(playerid);
				return true;
			}
			new bool:bank = listitem == 0;
			new funds = bank ? g_UiHsBank[playerid] : g_UiHsMoney[playerid];
			if (funds < UI_HsBillsTotal(playerid))
			{
				SendClientMessage(playerid, -10066177, "ამ გადახდის მეთოდით საკმარისი თანხა არ გაქვს.");
				UI_HsBillsPayShow(playerid);
				return true;
			}
			UI_OnHouseBillsPayAll(playerid, houseId, bank ? 1 : 0);
			return true;
		}
		case 9208:
		{
			return true;
		}
	}
	return false;
}
new PlayerText:g_UiHh[MAX_PLAYERS][7];
new bool:g_UiHhCreated[MAX_PLAYERS];
new bool:g_UiHhDark[MAX_PLAYERS];
new bool:g_UiHhHunger[MAX_PLAYERS];
new bool:g_UiHhHome[MAX_PLAYERS];

PlayerText:UI_HhText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, align);
	PlayerTextDrawColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
	PlayerTextDrawSetProportional(playerid, td, true);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	return td;
}

PlayerText:UI_HhBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, "_");
	PlayerTextDrawLetterSize(playerid, td, 0.0, (h / 9.0));
	PlayerTextDrawTextSize(playerid, td, (x + w), 0.0);
	PlayerTextDrawUseBox(playerid, td, true);
	PlayerTextDrawBoxColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawSetOutline(playerid, td, 0);
	return td;
}

UI_HhCreate(playerid)
{
	if (g_UiHhCreated[playerid])
	{
		return 0;
	}
	new Float:left = (632.0 - 150.0);
	new Float:homeLeft = ((left - 150.0) - 4.0);
	g_UiHh[playerid][0] = UI_HhBox(playerid, 0.0, 0.0, 640.0, 490.0, 176);
	g_UiHh[playerid][1] = UI_HhBox(playerid, left, 458.0, 150.0, 18.0, 153);
	g_UiHh[playerid][2] = UI_HhText(playerid, (4.0 + left), (458.0 + 1.0), "სიმაძღრე 0%", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiHh[playerid][3] = UI_HhBox(playerid, (4.0 + left), (458.0 + 12.0), 142.0, 4.0, 0x444444FF);
	g_UiHh[playerid][4] = UI_HhBox(playerid, (4.0 + left), (458.0 + 12.0), 142.0, 4.0, 0x66DD66FF);
	g_UiHh[playerid][5] = UI_HhBox(playerid, homeLeft, 458.0, 150.0, 18.0, 153);
	g_UiHh[playerid][6] = UI_HhText(playerid, (4.0 + homeLeft), (458.0 + 3.0), "შუქი ON | გაზი ON | მაცივარი 0/0", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiHhCreated[playerid] = true;
	return 0;
}

UI_HouseDarkShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	UI_HhCreate(playerid);
	g_UiHhDark[playerid] = true;
	PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][0]);
	return 0;
}

UI_HouseDarkHide(playerid)
{
	if (!UI_HsValid(playerid) || !g_UiHhCreated[playerid])
	{
		return 0;
	}
	g_UiHhDark[playerid] = false;
	PlayerTextDrawHide(playerid, PlayerText:g_UiHh[playerid][0]);
	return 0;
}

UI_HungerHudUpdate(playerid, hunger)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		return 0;    // la fame e' nel pannello CEF (ui/hud_cef.inc)
	}
	UI_HhCreate(playerid);
	if (hunger < 0)
	{
		hunger = 0;
	}
	if (hunger > 100)
	{
		hunger = 100;
	}
	new color = -1, barColor = 0x66DD66FF;
	if (hunger <= 10)
	{
		color = -10066177;
		barColor = -12303105;
	}
	else if (hunger <= 20)
	{
		color = -2994689;
		barColor = -2994689;
	}
	new str[288];
	format(str, 288, "სიმაძღრე %d%%", hunger);
	Lang_PlayerTextDrawSetString(playerid, g_UiHh[playerid][2], str);
	PlayerTextDrawColour(playerid, PlayerText:g_UiHh[playerid][2], color);
	PlayerTextDrawBoxColour(playerid, PlayerText:g_UiHh[playerid][4], barColor);
	new Float:fraction = (float(hunger) / float(100));
	PlayerTextDrawTextSize(playerid, PlayerText:g_UiHh[playerid][4], (0.1 + ((4.0 + (632.0 - 150.0)) + (142.0 * fraction))), 0.0);
	if (!g_UiHhHunger[playerid])
	{
		g_UiHhHunger[playerid] = true;
		PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][1]);
		PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][3]);
	}
	PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][2]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][4]);
	return 0;
}

UI_HouseHudUpdate(playerid, bool:lightOn, bool:gasOn, fridgeUnits, fridgeCap)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	UI_HhCreate(playerid);
	new str[576];
	format(str, 576, "შუქი %s | გაზი %s | მაცივარი %d/%d", lightOn ? "ON" : "OFF", gasOn ? "ON" : "OFF", fridgeUnits, fridgeCap);
	Lang_PlayerTextDrawSetString(playerid, g_UiHh[playerid][6], str);
	if (!g_UiHhHome[playerid])
	{
		g_UiHhHome[playerid] = true;
		PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][5]);
	}
	PlayerTextDrawShow(playerid, PlayerText:g_UiHh[playerid][6]);
	return 0;
}

UI_HouseHudHide(playerid)
{
	if (!UI_HsValid(playerid) || !g_UiHhCreated[playerid])
	{
		return 0;
	}
	g_UiHhHome[playerid] = false;
	PlayerTextDrawHide(playerid, PlayerText:g_UiHh[playerid][5]);
	PlayerTextDrawHide(playerid, PlayerText:g_UiHh[playerid][6]);
	return 0;
}

UI_HouseUiDestroy(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	if (g_UiHhCreated[playerid])
	{
		for (new i = 0; i < 7; i++)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiHh[playerid][i]);
			g_UiHh[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
		}
	}
	g_UiHhCreated[playerid] = false;
	g_UiHhDark[playerid] = false;
	g_UiHhHunger[playerid] = false;
	g_UiHhHome[playerid] = false;
	g_UiHsHouse[playerid] = 0;
	g_UiHsRole[playerid] = 3;
	g_UiHsLightDue[playerid] = 0;
	g_UiHsLightCnt[playerid] = 0;
	g_UiHsGasDue[playerid] = 0;
	g_UiHsGasCnt[playerid] = 0;
	for (new i = 0; i < 10; i++)
	{
		g_UiHsItemAct[playerid][i] = 0;
	}
	return 0;
}
