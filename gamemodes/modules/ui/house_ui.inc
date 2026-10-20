new g_UiHsTierName[4][16] = {
	"Monolocale",
	"Bilocale",
	"Casa media",
	"Villa"
};
new g_UiHsTierPrice[4] = {14000, 24000, 45000, 130000};
new g_UiHsTierLevel[4] = {1, 2, 5, 10};
new g_UiHsTierFridge[4] = {10, 16, 24, 40};
new g_UiHsIntName[8][20] = {
	"Denise",
	"Katie",
	"Casa di Ryder",
	"Casa di Sweet",
	"Casa media A",
	"Casa Johnson",
	"Casa media B",
	"Villa di Madd Dogg"
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
new g_UiHsLightDue[MAX_PLAYERS];
new g_UiHsLightCnt[MAX_PLAYERS];
new g_UiHsGasDue[MAX_PLAYERS];
new g_UiHsGasCnt[MAX_PLAYERS];
new g_UiHsItemAct[MAX_PLAYERS][10];
new g_UiHsAmtAction[MAX_PLAYERS];
new g_UiHsAmtMax[MAX_PLAYERS];

UI_HsMoney(value, out[], size)
{
	new raw[16], len = 0, pos = 0;
	if (value < 0)
	{
		value = 0;
	}
	format(raw, 16, "%d", value);
	len = strlen(raw);
	out[0] = 0;
	for (new i = 0; i < len && pos < size - 2; i++)
	{
		if (i > 0 && (len - i) % 3 == 0)
		{
			out[pos++] = 46;
		}
		out[pos++] = raw[i];
	}
	out[pos] = 0;
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
			format(out, size, "{FF6666}STACCATA");
		}
		default:
		{
			format(out, size, "{FFD24D}OFF");
		}
	}
}

UI_HsTitle(playerid, prefix[], out[], size)
{
	format(out, size, "%s - Casa %d", prefix, g_UiHsHouse[playerid]);
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

UI_HsMenuRow(playerid, &n, act, line[], body[], size)
{
	if (n >= 10)
	{
		return 0;
	}
	g_UiHsItemAct[playerid][n++] = act;
	strcat(body, line, size);
}

UI_HouseMenuShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[1024], line[160], title[64], price[16], tmp[16], st[24];
	new n = 0, t = g_UiHsTier[playerid], role = g_UiHsRole[playerid];
	for (new i = 0; i < 10; i++)
	{
		g_UiHsItemAct[playerid][i] = 0;
	}
	UI_HsMoney(g_UiHsTierPrice[t], price, 16);
	format(title, 64, "Casa %d - %s", g_UiHsHouse[playerid], g_UiHsIntName[g_UiHsInt[playerid]]);
	format(line, 160, "{AAAAAA}Interno: {FFFFFF}%s (%s)\n", g_UiHsIntName[g_UiHsInt[playerid]], g_UiHsTierName[t]);
	UI_HsMenuRow(playerid, n, 0, line, body, 1024);
	format(line, 160, "{AAAAAA}Prezzo: {66DD66}$%s{AAAAAA}  (liv. min %d)\n", price, g_UiHsTierLevel[t]);
	UI_HsMenuRow(playerid, n, 0, line, body, 1024);
	if (role == 0)
	{
		UI_HsMenuRow(playerid, n, 0, "{AAAAAA}Stato: {FFD24D}in vendita\n", body, 1024);
		UI_HsMenuRow(playerid, n, 100, "{66DD66}> Compra questa casa\n", body, 1024);
	}
	else if (role == 3)
	{
		UI_HsMenuRow(playerid, n, 0, "{AAAAAA}Stato: {FF6666}proprieta' privata\n", body, 1024);
	}
	else
	{
		UI_HsMenuRow(playerid, n, 0, role == 1 ? "{AAAAAA}Stato: {66DD66}tua proprieta'\n" : "{AAAAAA}Stato: {66AAFF}ospite con chiave\n", body, 1024);
		UI_HsUtilText(g_UiHsLight[playerid], st, 24);
		format(line, 160, "{AAAAAA}Luce: %s{AAAAAA}   Gas: ", st);
		UI_HsUtilText(g_UiHsGas[playerid], st, 24);
		strcat(line, st, 160);
		strcat(line, "\n", 160);
		UI_HsMenuRow(playerid, n, 0, line, body, 1024);
		new bills = g_UiHsLightCnt[playerid] + g_UiHsGasCnt[playerid];
		new due = g_UiHsLightDue[playerid] + g_UiHsGasDue[playerid];
		UI_HsMoney(due, tmp, 16);
		if (role == 1)
		{
			format(line, 160, "{AAAAAA}Bollette: %s%d da pagare ($%s)\n", bills > 0 ? "{FF6666}" : "{66DD66}", bills, tmp);
			UI_HsMenuRow(playerid, n, 0, line, body, 1024);
			format(line, 160, "{FFFFFF}Serratura: %s{FFFFFF} (tocca per cambiare)\n", g_UiHsLocked[playerid] ? "{FF6666}chiusa" : "{66DD66}aperta");
			UI_HsMenuRow(playerid, n, 1, line, body, 1024);
		}
		UI_HsMenuRow(playerid, n, 2, "{FFFFFF}Utenze (luce / gas)\n", body, 1024);
		format(line, 160, "{FFFFFF}Frigo ({FFD24D}%d/%d{FFFFFF} porzioni)\n", g_UiHsFridge[playerid], UI_HsFridgeCap(playerid));
		UI_HsMenuRow(playerid, n, 3, line, body, 1024);
		if (role == 1)
		{
			UI_HsMenuRow(playerid, n, 4, "{FFFFFF}Bollette\n", body, 1024);
			format(line, 160, "{FFFFFF}Chiavi ospiti ({FFD24D}%d/%d{FFFFFF})\n", g_UiHsKeys[playerid], 3);
			UI_HsMenuRow(playerid, n, 5, line, body, 1024);
			UI_HsMenuRow(playerid, n, 6, "{FF6666}Vendi la casa\n", body, 1024);
		}
	}
	Lang_ShowPlayerDialog(playerid, 9200, 2, title, body, "Scegli", "Chiudi");
}

UI_HsBuyPayShow(playerid)
{
	new t = g_UiHsTier[playerid];
	new body[320], price[16], cash[16], bank[16], tl[48], title[64];
	new p = g_UiHsTierPrice[t];
	UI_HsMoney(p, price, 16);
	UI_HsMoney(g_UiHsMoney[playerid], cash, 16);
	UI_HsMoney(g_UiHsBank[playerid], bank, 16);
	format(body, 320, "{FFFFFF}Contanti\t%s$%s\n", g_UiHsMoney[playerid] >= p ? "{66DD66}" : "{FF6666}", cash);
	format(tl, 48, "{FFFFFF}Banca\t%s$%s\n", g_UiHsBank[playerid] >= p ? "{66DD66}" : "{FF6666}", bank);
	strcat(body, tl, 320);
	format(title, 64, "%s $%s - pagamento", g_UiHsTierName[t], price);
	Lang_ShowPlayerDialog(playerid, 9201, 4, title, body, "Acquista", "Indietro");
}

UI_HsSellConfirmShow(playerid)
{
	new body[320], amount[16], title[64];
	UI_HsMoney(g_UiHsTierPrice[g_UiHsTier[playerid]] * 70 / 100, amount, 16);
	format(body, 320, "{FFFFFF}Vendere la casa {FFD24D}%d{FFFFFF} (%s)?\n\nRiceverai circa {66DD66}$%s{FFFFFF} (%d%% del prezzo) in banca.\nUtenze, chiavi e bollette verranno azzerate; l'operazione non e' reversibile.", g_UiHsHouse[playerid], g_UiHsIntName[g_UiHsInt[playerid]], amount, 70);
	UI_HsTitle(playerid, "Conferma vendita", title, 64);
	Lang_ShowPlayerDialog(playerid, 9202, 0, title, body, "Vendi", "Annulla");
}

UI_HouseUtilityShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[512], line[128], st[24], title[64];
	strcat(body, "Utenza\tStato\tAzione\n", 512);
	UI_HsUtilText(g_UiHsLight[playerid], st, 24);
	switch (g_UiHsLight[playerid])
	{
		case 2:
		{
			format(line, 128, "{FFFFFF}Luce\t%s\t{FF6666}Paga le bollette\n", st);
		}
		case 1:
		{
			format(line, 128, "{FFFFFF}Luce\t%s\t{AAAAAA}Spegni\n", st);
		}
		default:
		{
			format(line, 128, "{FFFFFF}Luce\t%s\t{AAAAAA}Accendi\n", st);
		}
	}
	strcat(body, line, 512);
	UI_HsUtilText(g_UiHsGas[playerid], st, 24);
	if (g_UiHsRole[playerid] != 1)
	{
		format(line, 128, "{777777}Gas\t%s\t{777777}Solo proprietario\n", st);
	}
	else
	{
		switch (g_UiHsGas[playerid])
		{
			case 2:
			{
				format(line, 128, "{FFFFFF}Gas\t%s\t{FF6666}Paga le bollette\n", st);
			}
			case 1:
			{
				format(line, 128, "{FFFFFF}Gas\t%s\t{AAAAAA}Spegni\n", st);
			}
			default:
			{
				format(line, 128, "{FFFFFF}Gas\t%s\t{AAAAAA}Accendi\n", st);
			}
		}
	}
	strcat(body, line, 512);
	UI_HsTitle(playerid, "Utenze", title, 64);
	Lang_ShowPlayerDialog(playerid, 9203, 5, title, body, "Cambia", "Indietro");
}

UI_HouseFridgeShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[512], line[128], title[64], n = 0;
	new cap = UI_HsFridgeCap(playerid), units = g_UiHsFridge[playerid];
	new bool:owner = g_UiHsRole[playerid] == 1;
	for (new i = 0; i < 10; i++)
	{
		g_UiHsItemAct[playerid][i] = 0;
	}
	format(line, 128, "{AAAAAA}Scorte: {FFD24D}%d/%d{AAAAAA} porzioni  -  con te: %d/%d\n", units, cap, g_UiHsBag[playerid], 12);
	UI_HsMenuRow(playerid, n, 0, line, body, 512);
	if (g_UiHsLight[playerid] != 1)
	{
		UI_HsMenuRow(playerid, n, 0, "{FF6666}Luce spenta: il frigo non raffredda, le scorte si deteriorano\n", body, 512);
	}
	format(line, 128, "%sMangia freddo (+%d sazieta')\n", units > 0 ? "{FFFFFF}" : "{777777}", 25);
	UI_HsMenuRow(playerid, n, 1, line, body, 512);
	format(line, 128, "%sCucina (+%d sazieta', +%d salute%s)\n", units > 0 && g_UiHsGas[playerid] == 1 ? "{FFFFFF}" : "{777777}", 45, 5, g_UiHsGas[playerid] == 1 ? "" : " - gas non attivo");
	UI_HsMenuRow(playerid, n, 2, line, body, 512);
	if (owner)
	{
		format(line, 128, "%sRiponi spesa nel frigo\n", g_UiHsBag[playerid] > 0 && units < cap ? "{FFFFFF}" : "{777777}");
		UI_HsMenuRow(playerid, n, 3, line, body, 512);
		format(line, 128, "%sPrendi porzioni dal frigo\n", units > 0 && g_UiHsBag[playerid] < 12 ? "{FFFFFF}" : "{777777}");
		UI_HsMenuRow(playerid, n, 4, line, body, 512);
	}
	UI_HsTitle(playerid, "Frigo", title, 64);
	Lang_ShowPlayerDialog(playerid, 9204, 2, title, body, "Scegli", "Indietro");
}

UI_HsAmountShow(playerid, action, maxAmount)
{
	new body[192], title[64];
	g_UiHsAmtAction[playerid] = action;
	g_UiHsAmtMax[playerid] = maxAmount;
	format(body, 192, "{FFFFFF}%s\nQuantita' (1-%d):", action == 3 ? "Porzioni da riporre nel frigo." : "Porzioni da prendere dal frigo.", maxAmount);
	UI_HsTitle(playerid, action == 3 ? "Riponi" : "Prendi", title, 64);
	Lang_ShowPlayerDialog(playerid, 9205, 1, title, body, "Conferma", "Indietro");
}

UI_HouseBillsShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new body[512], line[160], a[16], b[16], title[64];
	strcat(body, "Utenza\tDa pagare\tImporto\tNote\n", 512);
	UI_HsMoney(g_UiHsLightDue[playerid], a, 16);
	format(line, 160, "{FFFFFF}Luce\t%d\t$%s\t%s\n", g_UiHsLightCnt[playerid], a, g_UiHsLight[playerid] == 2 ? "{FF6666}staccata" : "{AAAAAA}-");
	strcat(body, line, 512);
	UI_HsMoney(g_UiHsGasDue[playerid], b, 16);
	format(line, 160, "{FFFFFF}Gas\t%d\t$%s\t%s\n", g_UiHsGasCnt[playerid], b, g_UiHsGas[playerid] == 2 ? "{FF6666}staccata" : "{AAAAAA}-");
	strcat(body, line, 512);
	new total = UI_HsBillsTotal(playerid);
	UI_HsMoney(total, a, 16);
	new fees = total - g_UiHsLightDue[playerid] - g_UiHsGasDue[playerid];
	if (fees > 0)
	{
		UI_HsMoney(fees, b, 16);
		format(line, 160, "{FFD24D}Totale\t%d\t$%s\t{AAAAAA}incl. $%s riattivazione\n", g_UiHsLightCnt[playerid] + g_UiHsGasCnt[playerid], a, b);
	}
	else
	{
		format(line, 160, "{FFD24D}Totale\t%d\t$%s\t{AAAAAA}-\n", g_UiHsLightCnt[playerid] + g_UiHsGasCnt[playerid], a);
	}
	strcat(body, line, 512);
	UI_HsTitle(playerid, "Bollette", title, 64);
	Lang_ShowPlayerDialog(playerid, 9206, 5, title, body, "Paga tutte", "Indietro");
}

UI_HsBillsPayShow(playerid)
{
	new body[320], total[16], cash[16], bank[16], tl[48], title[64];
	new p = UI_HsBillsTotal(playerid);
	UI_HsMoney(p, total, 16);
	UI_HsMoney(g_UiHsMoney[playerid], cash, 16);
	UI_HsMoney(g_UiHsBank[playerid], bank, 16);
	format(body, 320, "{FFFFFF}Banca\t%s$%s\n", g_UiHsBank[playerid] >= p ? "{66DD66}" : "{FF6666}", bank);
	format(tl, 48, "{FFFFFF}Contanti\t%s$%s\n", g_UiHsMoney[playerid] >= p ? "{66DD66}" : "{FF6666}", cash);
	strcat(body, tl, 320);
	format(title, 64, "Paga bollette $%s", total);
	Lang_ShowPlayerDialog(playerid, 9207, 4, title, body, "Paga", "Indietro");
}

bool:UI_HsParseAmount(text[], &value)
{
	new len = strlen(text);
	if (len < 1 || len > 4)
	{
		return 0;
	}
	value = 0;
	for (new i = 0; i < len; i++)
	{
		if (text[i] < 48 || text[i] > 57)
		{
			return 0;
		}
		value = value * 10 + (text[i] - 48);
	}
	return 1;
}

bool:UI_HouseOnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	new houseId = g_UiHsHouse[playerid];
	switch (dialogid)
	{
		case 9200:
		{
			if (!response)
			{
				return 1;
			}
			if (listitem < 0 || listitem >= 10 || g_UiHsItemAct[playerid][listitem] == 0)
			{
				UI_HouseMenuShow(playerid);
				return 1;
			}
			new act = g_UiHsItemAct[playerid][listitem];
			if (act == 100)
			{
				if (g_UiHsLevel[playerid] < g_UiHsTierLevel[g_UiHsTier[playerid]])
				{
					SendClientMessage(playerid, -10066177, "Il tuo livello e' troppo basso per questa casa.");
					UI_HouseMenuShow(playerid);
					return 1;
				}
				UI_HsBuyPayShow(playerid);
				return 1;
			}
			if (act == 6)
			{
				UI_HsSellConfirmShow(playerid);
				return 1;
			}
			UI_OnHouseAction(playerid, houseId, act);
			return 1;
		}
		case 9201:
		{
			if (!response || listitem < 0 || listitem > 1)
			{
				UI_HouseMenuShow(playerid);
				return 1;
			}
			new p = g_UiHsTierPrice[g_UiHsTier[playerid]];
			new funds = listitem == 1 ? g_UiHsBank[playerid] : g_UiHsMoney[playerid];
			if (funds < p)
			{
				SendClientMessage(playerid, -10066177, "Fondi insufficienti con questo metodo di pagamento.");
				UI_HsBuyPayShow(playerid);
				return 1;
			}
			UI_OnHouseBuyConfirm(playerid, houseId, listitem);
			return 1;
		}
		case 9202:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return 1;
			}
			UI_OnHouseAction(playerid, houseId, 6);
			return 1;
		}
		case 9203:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return 1;
			}
			if (listitem != 0 && listitem != 1)
			{
				UI_HouseUtilityShow(playerid);
				return 1;
			}
			new st = listitem == 0 ? g_UiHsLight[playerid] : g_UiHsGas[playerid];
			if (st == 2)
			{
				SendClientMessage(playerid, -10066177, "Utenza staccata per morosita': paga le bollette per riattivarla.");
				UI_HouseUtilityShow(playerid);
				return 1;
			}
			if (listitem == 1 && g_UiHsRole[playerid] != 1)
			{
				SendClientMessage(playerid, -10066177, "Solo il proprietario puo' gestire il gas.");
				UI_HouseUtilityShow(playerid);
				return 1;
			}
			UI_OnHouseUtilityToggle(playerid, houseId, listitem, st == 1 ? 0 : 1);
			return 1;
		}
		case 9204:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return 1;
			}
			new act = listitem >= 0 && listitem < 10 ? g_UiHsItemAct[playerid][listitem] : 0;
			new units = g_UiHsFridge[playerid], cap = UI_HsFridgeCap(playerid);
			switch (act)
			{
				case 1:
				{
					if (units <= 0)
					{
						SendClientMessage(playerid, -10066177, "Il frigo e' vuoto.");
						UI_HouseFridgeShow(playerid);
						return 1;
					}
					UI_OnHouseFridgeAction(playerid, houseId, 1, 0);
				}
				case 2:
				{
					if (units <= 0)
					{
						SendClientMessage(playerid, -10066177, "Il frigo e' vuoto.");
						UI_HouseFridgeShow(playerid);
						return 1;
					}
					if (g_UiHsGas[playerid] != 1)
					{
						SendClientMessage(playerid, -10066177, "Il gas non e' attivo: non puoi cucinare.");
						UI_HouseFridgeShow(playerid);
						return 1;
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
						SendClientMessage(playerid, -10066177, "Non hai spesa da riporre o il frigo e' pieno.");
						UI_HouseFridgeShow(playerid);
						return 1;
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
						SendClientMessage(playerid, -10066177, "Frigo vuoto o borsa piena.");
						UI_HouseFridgeShow(playerid);
						return 1;
					}
					UI_HsAmountShow(playerid, 4, m);
				}
				default:
				{
					UI_HouseFridgeShow(playerid);
				}
			}
			return 1;
		}
		case 9205:
		{
			if (!response)
			{
				UI_HouseFridgeShow(playerid);
				return 1;
			}
			new amount = 0;
			if (!UI_HsParseAmount(inputtext, amount) || amount < 1 || amount > g_UiHsAmtMax[playerid])
			{
				SendClientMessage(playerid, -10066177, "Quantita' non valida.");
				UI_HsAmountShow(playerid, g_UiHsAmtAction[playerid], g_UiHsAmtMax[playerid]);
				return 1;
			}
			UI_OnHouseFridgeAction(playerid, houseId, g_UiHsAmtAction[playerid], amount);
			return 1;
		}
		case 9206:
		{
			if (!response)
			{
				UI_HouseMenuShow(playerid);
				return 1;
			}
			if (UI_HsBillsTotal(playerid) <= 0)
			{
				SendClientMessage(playerid, 0x66DD66FF, "Nessuna bolletta da pagare.");
				UI_HouseBillsShow(playerid);
				return 1;
			}
			UI_HsBillsPayShow(playerid);
			return 1;
		}
		case 9207:
		{
			if (!response || listitem < 0 || listitem > 1)
			{
				UI_HouseBillsShow(playerid);
				return 1;
			}
			new bool:bank = listitem == 0;
			new funds = bank ? g_UiHsBank[playerid] : g_UiHsMoney[playerid];
			if (funds < UI_HsBillsTotal(playerid))
			{
				SendClientMessage(playerid, -10066177, "Fondi insufficienti con questo metodo di pagamento.");
				UI_HsBillsPayShow(playerid);
				return 1;
			}
			UI_OnHouseBillsPayAll(playerid, houseId, bank ? 1 : 0);
			return 1;
		}
		case 9208:
		{
			return 1;
		}
	}
	return 0;
}
new g_UiHh[MAX_PLAYERS][7];
new bool:g_UiHhCreated[MAX_PLAYERS];
new bool:g_UiHhDark[MAX_PLAYERS];
new bool:g_UiHhHunger[MAX_PLAYERS];
new bool:g_UiHhHome[MAX_PLAYERS];

PlayerText:UI_HhText(playerid, Float:x, Float:y, text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, align);
	PlayerTextDrawColor(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetProportional(playerid, td, 1);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColor(playerid, td, 255);
	return td;
}

PlayerText:UI_HhBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, "_");
	PlayerTextDrawLetterSize(playerid, td, 0.0, (h / 9.0));
	PlayerTextDrawTextSize(playerid, td, (x + w), 0.0);
	PlayerTextDrawUseBox(playerid, td, 1);
	PlayerTextDrawBoxColor(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
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
	g_UiHh[playerid][2] = UI_HhText(playerid, (4.0 + left), (458.0 + 1.0), "Sazieta' 0%", 0.2, 0.9, -1, 1);
	g_UiHh[playerid][3] = UI_HhBox(playerid, (4.0 + left), (458.0 + 12.0), 142.0, 4.0, 0x444444FF);
	g_UiHh[playerid][4] = UI_HhBox(playerid, (4.0 + left), (458.0 + 12.0), 142.0, 4.0, 0x66DD66FF);
	g_UiHh[playerid][5] = UI_HhBox(playerid, homeLeft, 458.0, 150.0, 18.0, 153);
	g_UiHh[playerid][6] = UI_HhText(playerid, (4.0 + homeLeft), (458.0 + 3.0), "Luce ON | Gas ON | Frigo 0/0", 0.2, 0.9, -1, 1);
	g_UiHhCreated[playerid] = 1;
}

UI_HouseDarkShow(playerid)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	UI_HhCreate(playerid);
	g_UiHhDark[playerid] = 1;
	PlayerTextDrawShow(playerid, g_UiHh[playerid][0]);
}

UI_HouseDarkHide(playerid)
{
	if (!UI_HsValid(playerid) || !g_UiHhCreated[playerid])
	{
		return 0;
	}
	g_UiHhDark[playerid] = 0;
	PlayerTextDrawHide(playerid, g_UiHh[playerid][0]);
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
	new str[32];
	format(str, 32, "Sazieta' %d%%", hunger);
	Lang_PlayerTextDrawSetString(playerid, g_UiHh[playerid][2], str);
	PlayerTextDrawColor(playerid, g_UiHh[playerid][2], color);
	PlayerTextDrawBoxColor(playerid, g_UiHh[playerid][4], barColor);
	new Float:fraction = (float(hunger) / float(100));
	PlayerTextDrawTextSize(playerid, g_UiHh[playerid][4], (0.1 + ((4.0 + (632.0 - 150.0)) + (142.0 * fraction))), 0.0);
	if (!g_UiHhHunger[playerid])
	{
		g_UiHhHunger[playerid] = 1;
		PlayerTextDrawShow(playerid, g_UiHh[playerid][1]);
		PlayerTextDrawShow(playerid, g_UiHh[playerid][3]);
	}
	PlayerTextDrawShow(playerid, g_UiHh[playerid][2]);
	PlayerTextDrawShow(playerid, g_UiHh[playerid][4]);
}

UI_HouseHudUpdate(playerid, bool:lightOn, bool:gasOn, fridgeUnits, fridgeCap)
{
	if (!UI_HsValid(playerid))
	{
		return 0;
	}
	UI_HhCreate(playerid);
	new str[64];
	format(str, 64, "Luce %s | Gas %s | Frigo %d/%d", lightOn ? "ON" : "OFF", gasOn ? "ON" : "OFF", fridgeUnits, fridgeCap);
	Lang_PlayerTextDrawSetString(playerid, g_UiHh[playerid][6], str);
	if (!g_UiHhHome[playerid])
	{
		g_UiHhHome[playerid] = 1;
		PlayerTextDrawShow(playerid, g_UiHh[playerid][5]);
	}
	PlayerTextDrawShow(playerid, g_UiHh[playerid][6]);
}

UI_HouseHudHide(playerid)
{
	if (!UI_HsValid(playerid) || !g_UiHhCreated[playerid])
	{
		return 0;
	}
	g_UiHhHome[playerid] = 0;
	PlayerTextDrawHide(playerid, g_UiHh[playerid][5]);
	PlayerTextDrawHide(playerid, g_UiHh[playerid][6]);
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
			PlayerTextDrawDestroy(playerid, g_UiHh[playerid][i]);
			g_UiHh[playerid][i] = 65535;
		}
	}
	g_UiHhCreated[playerid] = 0;
	g_UiHhDark[playerid] = 0;
	g_UiHhHunger[playerid] = 0;
	g_UiHhHome[playerid] = 0;
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
}
