new g_UiInvName[11][24] = {
	{0},
	"Porzione di cibo",
	"Pasto pronto",
	"Bottiglia d'acqua",
	"Snack",
	"Kit di pronto soccorso",
	"Benda",
	"Sigarette",
	"Accendino",
	"Telefono",
	"Documento d'identita'"
};
new g_UiInvDesc[11][80] = {
	{0},
	"Scatolame e ingredienti per cucinare. Si ripone nel frigo.",
	"Piatto da asporto. Sazia subito.",
	"Acqua naturale da 0,5 l.",
	"Barretta o patatine.",
	"Kit civile di pronto soccorso. Cura e viene consumato.",
	"Garza e cerotti per piccole ferite.",
	"Pacchetto da 20. Serve un accendino.",
	"Necessario per fumare.",
	"Mostra il tuo numero di telefono.",
	"Carta d'identita' di San Andreas."
};
new g_UiInvStack[11] = {0, 12, 3, 6, 10, 3, 10, 5, 1, 1, 1};
new g_UiInvFlags[11] = {0, 2, 3, 3, 3, 3, 3, 3, 2, 1, 1};
new g_UiInvItem[MAX_PLAYERS][20];
new g_UiInvQty[MAX_PLAYERS][20];
new g_UiInvUsed[MAX_PLAYERS];
new g_UiInvMax[MAX_PLAYERS];
new bool:g_UiInvBag[MAX_PLAYERS];
new g_UiInvPage[MAX_PLAYERS];
new g_UiInvRows[MAX_PLAYERS][10];
new g_UiInvRowCount[MAX_PLAYERS];
new g_UiInvNavNext[MAX_PLAYERS];
new g_UiInvNavPrev[MAX_PLAYERS];
new g_UiInvSel[MAX_PLAYERS];
new g_UiInvSelItem[MAX_PLAYERS];
new g_UiInvSelQty[MAX_PLAYERS];
new g_UiInvPendAct[MAX_PLAYERS];
new g_UiInvToastTd[MAX_PLAYERS][3];
new g_UiInvToastItem[MAX_PLAYERS][3];
new g_UiInvToastDelta[MAX_PLAYERS][3];
new g_UiInvToastTtl[MAX_PLAYERS][3];
new bool:g_UiInvToastCreated[MAX_PLAYERS];
new g_UiInvRecvTd[MAX_PLAYERS][4];
new bool:g_UiInvRecvActive[MAX_PLAYERS];
new g_UiInvRecvLeft[MAX_PLAYERS];
new g_UiInvRecvTotal[MAX_PLAYERS];
new g_UiInvTimer[MAX_PLAYERS];

bool:UI_InvValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

bool:UI_InvItemOk(itemId)
{
	return itemId >= 1 && itemId < 11;
}

bool:UI_InvParseNum(const s[], &value)
{
	new i = 0, len = strlen(s), digits = 0;
	value = 0;
	while (i < len && s[i] == 32)
	{
		i++;
	}
	while (i < len && s[i] >= 48 && s[i] <= 57)
	{
		if (digits >= 4)
		{
			return false;
		}
		value = value * 10 + (s[i] - 48);
		digits++;
		i++;
	}
	while (i < len && s[i] == 32)
	{
		i++;
	}
	return digits > 0 && i == len;
}

UI_InvSelReset(playerid)
{
	g_UiInvSel[playerid] = -1;
	g_UiInvSelItem[playerid] = 0;
	g_UiInvSelQty[playerid] = 0;
	g_UiInvPendAct[playerid] = 0;
}

bool:UI_InvSelOk(playerid)
{
	new slot = g_UiInvSel[playerid];
	if (slot < 0 || slot >= 20 || slot >= g_UiInvMax[playerid])
	{
		return false;
	}
	return g_UiInvItem[playerid][slot] == g_UiInvSelItem[playerid] && g_UiInvQty[playerid][slot] > 0;
}

UI_InvClear(playerid)
{
	if (!UI_InvValid(playerid))
	{
		return 0;
	}
	for (new i = 0; i < 20; i++)
	{
		g_UiInvItem[playerid][i] = 0;
		g_UiInvQty[playerid][i] = 0;
	}
	g_UiInvUsed[playerid] = 0;
	return 0;
}

UI_InvSet(playerid, slot, itemId, qty)
{
	if (!UI_InvValid(playerid) || slot < 0 || slot >= 20)
	{
		return 0;
	}
	if (!UI_InvItemOk(itemId) || qty <= 0)
	{
		g_UiInvItem[playerid][slot] = 0;
		g_UiInvQty[playerid][slot] = 0;
		return 0;
	}
	g_UiInvItem[playerid][slot] = itemId;
	g_UiInvQty[playerid][slot] = qty;
	return 0;
}

UI_InvMeta(playerid, slotsUsed, slotsMax, bool:bag)
{
	if (!UI_InvValid(playerid))
	{
		return 0;
	}
	if (slotsMax < 1)
	{
		slotsMax = 1;
	}
	if (slotsMax > 20)
	{
		slotsMax = 20;
	}
	if (slotsUsed < 0)
	{
		slotsUsed = 0;
	}
	if (slotsUsed > slotsMax)
	{
		slotsUsed = slotsMax;
	}
	g_UiInvUsed[playerid] = slotsUsed;
	g_UiInvMax[playerid] = slotsMax;
	g_UiInvBag[playerid] = bag;
	return 0;
}

UI_InvListPage(playerid, page)
{
	new slots[20], n = 0;
	new limit = g_UiInvMax[playerid];
	if (limit < 1)
	{
		limit = 12;
	}
	if (limit > 20)
	{
		limit = 20;
	}
	for (new i = 0; i < limit; i++)
	{
		if (g_UiInvItem[playerid][i] != 0)
		{
			slots[n++] = i;
		}
	}
	new title[64], used = g_UiInvUsed[playerid];
	format(title, 64, "Inventario - %d/%d slot - zaino %s", used, limit, g_UiInvBag[playerid] ? "si" : "no");
	if (n == 0)
	{
		new empty[160];
		format(empty, 160, "{FFFFFF}Il tuo inventario e' vuoto.\n\nCapienza: {FFD24D}%d/%d{FFFFFF} slot. Zaino: {FFD24D}%s{FFFFFF}.", used, limit, g_UiInvBag[playerid] ? "si" : "no");
		g_UiInvRowCount[playerid] = 0;
		g_UiInvNavNext[playerid] = -1;
		g_UiInvNavPrev[playerid] = -1;
		Lang_ShowPlayerDialog(playerid, 9400, 0, title, empty, "Chiudi", "");
		return 0;
	}
	new pages = (n + 10 - 1) / 10;
	if (page < 0)
	{
		page = 0;
	}
	if (page >= pages)
	{
		page = pages - 1;
	}
	g_UiInvPage[playerid] = page;
	new body[1024], line[96], name[24];
	format(body, 1024, "Slot\tOggetto\tQuantita'\n");
	new first = page * 10, rows = 0;
	for (new k = first; k < n && rows < 10; k++)
	{
		new slot = slots[k], itemId = g_UiInvItem[playerid][slot];
		strcopy(name, g_UiInvName[itemId], 24);
		format(line, 96, "%d\t%s\t%d/%d\n", slot + 1, name, g_UiInvQty[playerid][slot], g_UiInvStack[itemId]);
		strcat(body, line, 1024);
		g_UiInvRows[playerid][rows++] = slot;
	}
	g_UiInvRowCount[playerid] = rows;
	g_UiInvNavNext[playerid] = -1;
	g_UiInvNavPrev[playerid] = -1;
	new idx = rows;
	if (page < pages - 1)
	{
		strcat(body, "{FFD24D}>> Pagina successiva\t\t\n", 1024);
		g_UiInvNavNext[playerid] = idx++;
	}
	if (page > 0)
	{
		strcat(body, "{FFD24D}<< Pagina precedente\t\t\n", 1024);
		g_UiInvNavPrev[playerid] = idx;
	}
	if (pages > 1)
	{
		format(title, 64, "Inventario %d/%d - zaino %s - pag. %d/%d", used, limit, g_UiInvBag[playerid] ? "si" : "no", page + 1, pages);
	}
	Lang_ShowPlayerDialog(playerid, 9400, 5, title, body, "Apri", "Chiudi");
	return 0;
}

UI_InvShow(playerid)
{
	if (!UI_InvValid(playerid))
	{
		return 0;
	}
	UI_InvSelReset(playerid);
	UI_InvListPage(playerid, 0);
	return 0;
}

UI_InvActionShow(playerid)
{
	if (!UI_InvSelOk(playerid))
	{
		SendClientMessage(playerid, -10066177, "L'inventario e' cambiato: riapro la lista.");
		UI_InvSelReset(playerid);
		UI_InvListPage(playerid, g_UiInvPage[playerid]);
		return 0;
	}
	new slot = g_UiInvSel[playerid], itemId = g_UiInvItem[playerid][slot], qty = g_UiInvQty[playerid][slot];
	new title[64], body[256], flags = g_UiInvFlags[itemId], name[24];
	strcopy(name, g_UiInvName[itemId], 24);
	format(title, 64, "%s x%d (slot %d)", name, qty, slot + 1);
	if (flags & 1)
	{
		strcat(body, "Usa\n", 256);
	}
	else
	{
		strcat(body, "{888888}Usa (non utilizzabile){FFFFFF}\n", 256);
	}
	if (flags & 2)
	{
		strcat(body, "Dai a un giocatore\nGetta\n", 256);
	}
	else
	{
		strcat(body, "{888888}Dai (non trasferibile){FFFFFF}\n{888888}Getta (non droppabile){FFFFFF}\n", 256);
	}
	strcat(body, "Descrizione", 256);
	Lang_ShowPlayerDialog(playerid, 9401, 2, title, body, "Scegli", "Indietro");
	return 0;
}

UI_InvItemShow(playerid, slot)
{
	if (!UI_InvValid(playerid))
	{
		return 0;
	}
	if (slot < 0 || slot >= 20 || slot >= g_UiInvMax[playerid] || g_UiInvItem[playerid][slot] == 0)
	{
		SendClientMessage(playerid, -10066177, "Slot vuoto o non valido.");
		return 0;
	}
	g_UiInvSel[playerid] = slot;
	g_UiInvSelItem[playerid] = g_UiInvItem[playerid][slot];
	g_UiInvSelQty[playerid] = 0;
	UI_InvActionShow(playerid);
	return 0;
}

UI_InvInfoShow(playerid)
{
	new slot = g_UiInvSel[playerid], itemId = g_UiInvItem[playerid][slot];
	new body[384], title[64], name[24], desc[80];
	strcopy(name, g_UiInvName[itemId], 24);
	strcopy(desc, g_UiInvDesc[itemId], 80);
	new flags = g_UiInvFlags[itemId];
	format(title, 64, "%s", name);
	format(body, 384, "{FFFFFF}%s\n\nPosseduti: {FFD24D}%d{FFFFFF} (max %d per slot)\nUso: {FFD24D}%s{FFFFFF}\nDare/Gettare: {FFD24D}%s", desc, g_UiInvQty[playerid][slot], g_UiInvStack[itemId], flags & 1 ? "si" : "no", flags & 2 ? "si" : "no");
	Lang_ShowPlayerDialog(playerid, 9402, 0, title, body, "Indietro", "Chiudi");
}

UI_InvQtyShow(playerid, const err[])
{
	new slot = g_UiInvSel[playerid], itemId = g_UiInvItem[playerid][slot], maxQ = g_UiInvQty[playerid][slot];
	new body[320], title[64], name[24];
	strcopy(name, g_UiInvName[itemId], 24);
	format(title, 64, "%s: quantita'", name);
	format(body, 320, "%s{FFFFFF}Quante unita' di {FFD24D}%s{FFFFFF} vuoi %s?\nInserisci un numero da {FFD24D}1{FFFFFF} a {FFD24D}%d{FFFFFF}.", err, name, g_UiInvPendAct[playerid] == 1 ? "dare" : "gettare", maxQ);
	Lang_ShowPlayerDialog(playerid, 9403, 1, title, body, "Avanti", "Indietro");
}

UI_InvTargetShow(playerid, const err[])
{
	new body[320], title[64], name[24], itemId = g_UiInvSelItem[playerid];
	strcopy(name, g_UiInvName[itemId], 24);
	format(title, 64, "Dai %s x%d", name, g_UiInvSelQty[playerid]);
	format(body, 320, "%s{FFFFFF}Inserisci l'{FFD24D}ID{FFFFFF} del giocatore vicino a cui dare l'oggetto.\nDeve trovarsi entro %.0f metri e accettare lo scambio.", err, 3.0);
	Lang_ShowPlayerDialog(playerid, 9404, 1, title, body, "Invia", "Indietro");
}

UI_InvDropConfirmShow(playerid)
{
	new body[320], name[24], itemId = g_UiInvSelItem[playerid];
	strcopy(name, g_UiInvName[itemId], 24);
	format(body, 320, "{FFFFFF}Gettare {FFD24D}%s x%d{FFFFFF}?\n\n{FF6666}L'oggetto finira' a terra e scomparira' dopo %d minuti. Chiunque vicino potra' raccoglierlo.", name, g_UiInvSelQty[playerid], 10);
	Lang_ShowPlayerDialog(playerid, 9405, 0, "Conferma: getta", body, "Getta", "Annulla");
}

UI_InvTimerStop(playerid)
{
	if (g_UiInvTimer[playerid] != 0)
	{
		KillTimer(g_UiInvTimer[playerid]);
		g_UiInvTimer[playerid] = 0;
	}
}

UI_InvTimerEnsure(playerid)
{
	if (g_UiInvTimer[playerid] == 0)
	{
		g_UiInvTimer[playerid] = SetTimerEx("UI_InvOnTimer", 1000, true, "d", playerid);
	}
}

PlayerText:UI_InvText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, align);
	PlayerTextDrawColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetProportional(playerid, td, true);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	return td;
}

PlayerText:UI_InvBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, "_");
	PlayerTextDrawLetterSize(playerid, td, 0.0, (h / 9.0));
	PlayerTextDrawTextSize(playerid, td, (x + w), 0.0);
	PlayerTextDrawUseBox(playerid, td, true);
	PlayerTextDrawBoxColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawSetOutline(playerid, td, 0);
	return td;
}

UI_InvRecvDestroyTds(playerid)
{
	for (new i = 0; i < 4; i++)
	{
		if (g_UiInvRecvTd[playerid][i] != 65535)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiInvRecvTd[playerid][i]);
		}
		g_UiInvRecvTd[playerid][i] = 65535;
	}
}

UI_InvRecvRefresh(playerid)
{
	new str[48], left = g_UiInvRecvLeft[playerid], total = g_UiInvRecvTotal[playerid];
	format(str, 48, "Scambio in attesa: %d s", left);
	Lang_PlayerTextDrawSetString(playerid, g_UiInvRecvTd[playerid][1], str);
	new Float:fraction = Float:((total > 0 ? (float(left) / float(total)) : 0));
	if ((fraction < 0.0))
	{
		fraction = 0.0;
	}
	if ((fraction > 1.0))
	{
		fraction = 1.0;
	}
	PlayerTextDrawTextSize(playerid, PlayerText:g_UiInvRecvTd[playerid][3], (0.1 + ((210.0 + 6.0) + (208.0 * fraction))), 0.0);
	PlayerTextDrawShow(playerid, PlayerText:g_UiInvRecvTd[playerid][1]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiInvRecvTd[playerid][3]);
}

UI_InvRecvEnd(playerid)
{
	g_UiInvRecvActive[playerid] = 0;
	g_UiInvRecvLeft[playerid] = 0;
	UI_InvRecvDestroyTds(playerid);
}

bool:UI_InvNeedTimer(playerid)
{
	if (g_UiInvRecvActive[playerid])
	{
		return true;
	}
	for (new i = 0; i < 3; i++)
	{
		if (g_UiInvToastTtl[playerid][i] > 0)
		{
			return true;
		}
	}
	return false;
}

UI_InvGiveConfirmShow(playerid, const giverName[], itemId, qty, ttlSec)
{
	if (!UI_InvValid(playerid) || !UI_InvItemOk(itemId) || qty <= 0)
	{
		return 0;
	}
	if (ttlSec <= 0)
	{
		ttlSec = 15;
	}
	if (g_UiInvRecvActive[playerid])
	{
		UI_InvRecvEnd(playerid);
	}
	for (new i = 0; i < 4; i++)
	{
		g_UiInvRecvTd[playerid][i] = 65535;
	}
	g_UiInvRecvTd[playerid][0] = _:UI_InvBox(playerid, 210.0, 124.0, 220.0, 26.0, 170);
	g_UiInvRecvTd[playerid][1] = UI_InvText(playerid, (210.0 + 6.0), (124.0 + 2.0), "Scambio in attesa", 0.24, 1.1, -2994689, t_TEXT_DRAW_ALIGN:1);
	g_UiInvRecvTd[playerid][2] = UI_InvBox(playerid, (210.0 + 6.0), (124.0 + 17.0), 208.0, 5.0, 0x444444FF);
	g_UiInvRecvTd[playerid][3] = UI_InvBox(playerid, (210.0 + 6.0), (124.0 + 17.0), 208.0, 5.0, -2994689);
	g_UiInvRecvActive[playerid] = 1;
	g_UiInvRecvTotal[playerid] = ttlSec;
	g_UiInvRecvLeft[playerid] = ttlSec;
	PlayerTextDrawShow(playerid, PlayerText:g_UiInvRecvTd[playerid][0]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiInvRecvTd[playerid][2]);
	UI_InvRecvRefresh(playerid);
	UI_InvTimerEnsure(playerid);
	new body[320], name[24], giver[25];
	strcopy(name, g_UiInvName[itemId], 24);
	strcopy(giver, giverName, 25);
	format(body, 320, "{FFD24D}%s{FFFFFF} vuole darti {FFD24D}%s x%d{FFFFFF}.\n\nLa richiesta scade tra {FF6666}%d{FFFFFF} secondi.\nSe non c'e' posto nel tuo inventario lo scambio fallira'.", giver, name, qty, ttlSec);
	Lang_ShowPlayerDialog(playerid, 9406, 0, "Oggetto in arrivo", body, "Accetta", "Rifiuta");
	return 0;
}

UI_InvGiveConfirmClose(playerid)
{
	if (!UI_InvValid(playerid))
	{
		return 0;
	}
	if (g_UiInvRecvActive[playerid])
	{
		UI_InvRecvEnd(playerid);
	}
	if (GetPlayerDialogID(playerid) == 9406)
	{
		HidePlayerDialog(playerid);
	}
	if (!UI_InvNeedTimer(playerid))
	{
		UI_InvTimerStop(playerid);
	}
	return 0;
}

UI_InvToastCreate(playerid)
{
	if (g_UiInvToastCreated[playerid])
	{
		return 0;
	}
	for (new i = 0; i < 3; i++)
	{
		g_UiInvToastTd[playerid][i] = UI_InvText(playerid, 130.0, (404.0 + (12.0 * _:(i))), "_", 0.22, 1.0, 0x66DD66FF, t_TEXT_DRAW_ALIGN:1);
		g_UiInvToastTtl[playerid][i] = 0;
		g_UiInvToastItem[playerid][i] = 0;
		g_UiInvToastDelta[playerid][i] = 0;
	}
	g_UiInvToastCreated[playerid] = 1;
	return 0;
}

UI_InvToastRedraw(playerid)
{
	if (!g_UiInvToastCreated[playerid])
	{
		return 0;
	}
	new str[48];
	for (new i = 0; i < 3; i++)
	{
		if (g_UiInvToastTtl[playerid][i] <= 0 || !UI_InvItemOk(g_UiInvToastItem[playerid][i]))
		{
			PlayerTextDrawHide(playerid, PlayerText:g_UiInvToastTd[playerid][i]);
			continue;
		}
		new delta = g_UiInvToastDelta[playerid][i];
		format(str, 48, "%s%d %s", delta > 0 ? "+" : "", delta, g_UiInvName[g_UiInvToastItem[playerid][i]]);
		Lang_PlayerTextDrawSetString(playerid, g_UiInvToastTd[playerid][i], str);
		PlayerTextDrawColour(playerid, PlayerText:g_UiInvToastTd[playerid][i], delta > 0 ? 0x66DD66FF : -10066177);
		PlayerTextDrawShow(playerid, PlayerText:g_UiInvToastTd[playerid][i]);
	}
	return 0;
}

UI_InvToast(playerid, itemId, delta)
{
	if (!UI_InvValid(playerid) || !UI_InvItemOk(itemId) || delta == 0)
	{
		return 0;
	}
	UI_InvToastCreate(playerid);
	if (g_UiInvToastTtl[playerid][0] > 0 && g_UiInvToastItem[playerid][0] == itemId && g_UiInvToastDelta[playerid][0] > 0 == delta > 0)
	{
		g_UiInvToastDelta[playerid][0] += delta;
		g_UiInvToastTtl[playerid][0] = 4;
	}
	else
	{
		for (new i = 2; i > 0; i--)
		{
			g_UiInvToastItem[playerid][i] = g_UiInvToastItem[playerid][i - 1];
			g_UiInvToastDelta[playerid][i] = g_UiInvToastDelta[playerid][i - 1];
			g_UiInvToastTtl[playerid][i] = g_UiInvToastTtl[playerid][i - 1];
		}
		g_UiInvToastItem[playerid][0] = itemId;
		g_UiInvToastDelta[playerid][0] = delta;
		g_UiInvToastTtl[playerid][0] = 4;
	}
	UI_InvToastRedraw(playerid);
	UI_InvTimerEnsure(playerid);
	return 0;
}

public UI_InvOnTimer(playerid)
{
	if (!UI_InvValid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new redraw = 0;
	for (new i = 0; i < 3; i++)
	{
		if (g_UiInvToastTtl[playerid][i] > 0)
		{
			g_UiInvToastTtl[playerid][i]--;
			redraw = 1;
		}
	}
	if (redraw)
	{
		UI_InvToastRedraw(playerid);
	}
	if (g_UiInvRecvActive[playerid])
	{
		g_UiInvRecvLeft[playerid]--;
		if (g_UiInvRecvLeft[playerid] <= 0)
		{
			UI_InvRecvEnd(playerid);
			if (GetPlayerDialogID(playerid) == 9406)
			{
				HidePlayerDialog(playerid);
			}
			SendClientMessage(playerid, -2994689, "La richiesta di scambio e' scaduta.");
			UI_OnInvGiveExpired(playerid);
		}
		else
		{
			UI_InvRecvRefresh(playerid);
		}
	}
	if (!UI_InvNeedTimer(playerid))
	{
		UI_InvTimerStop(playerid);
	}
	return 1;
}

bool:UI_InvOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	if (!UI_InvValid(playerid) || dialogid < 9400 || dialogid > 9406)
	{
		return false;
	}
	switch (dialogid)
	{
		case 9400:
		{
			if (!response)
			{
				return true;
			}
			if (listitem < 0)
			{
				return true;
			}
			if (listitem < g_UiInvRowCount[playerid])
			{
				new slot = g_UiInvRows[playerid][listitem];
				UI_InvItemShow(playerid, slot);
				return true;
			}
			if (listitem == g_UiInvNavNext[playerid])
			{
				UI_InvListPage(playerid, g_UiInvPage[playerid] + 1);
			}
			else if (listitem == g_UiInvNavPrev[playerid])
			{
				UI_InvListPage(playerid, g_UiInvPage[playerid] - 1);
			}
			else
			{
				UI_InvListPage(playerid, g_UiInvPage[playerid]);
			}
			return true;
		}
		case 9401:
		{
			if (!response)
			{
				UI_InvSelReset(playerid);
				UI_InvListPage(playerid, g_UiInvPage[playerid]);
				return true;
			}
			if (!UI_InvSelOk(playerid))
			{
				UI_InvActionShow(playerid);
				return true;
			}
			new slot = g_UiInvSel[playerid], itemId = g_UiInvSelItem[playerid], flags = g_UiInvFlags[itemId];
			switch (listitem)
			{
				case 0:
				{
					if (!(flags & 1))
					{
						SendClientMessage(playerid, -10066177, "Questo oggetto non si puo' usare.");
						UI_InvActionShow(playerid);
						return true;
					}
					UI_InvSelReset(playerid);
					UI_OnInvUse(playerid, slot);
				}
				case 1, 2:
				{
					if (!(flags & 2))
					{
						SendClientMessage(playerid, -10066177, listitem == 1 ? "Questo oggetto non si puo' dare." : "Questo oggetto non si puo' gettare.");
						UI_InvActionShow(playerid);
						return true;
					}
					g_UiInvPendAct[playerid] = listitem;
					g_UiInvSelQty[playerid] = 1;
					if (g_UiInvQty[playerid][slot] > 1)
					{
						UI_InvQtyShow(playerid, "");
					}
					else if (listitem == 1)
					{
						UI_InvTargetShow(playerid, "");
					}
					else
					{
						UI_InvDropConfirmShow(playerid);
					}
				}
				case 3:
				{
					UI_InvInfoShow(playerid);
				}
				default:
				{
					UI_InvActionShow(playerid);
				}
			}
			return true;
		}
		case 9402:
		{
			if (response)
			{
				UI_InvActionShow(playerid);
			}
			else
			{
				UI_InvSelReset(playerid);
			}
			return true;
		}
		case 9403:
		{
			if (!response)
			{
				UI_InvActionShow(playerid);
				return true;
			}
			if (!UI_InvSelOk(playerid))
			{
				UI_InvActionShow(playerid);
				return true;
			}
			new q = 0, maxQ = g_UiInvQty[playerid][g_UiInvSel[playerid]];
			if (!UI_InvParseNum(inputtext, q) || q < 1 || q > maxQ)
			{
				new err[144];
				format(err, 144, "{FF6666}Quantita' non valida: serve un numero da 1 a %d.\n\n", maxQ);
				UI_InvQtyShow(playerid, err);
				return true;
			}
			g_UiInvSelQty[playerid] = q;
			if (g_UiInvPendAct[playerid] == 1)
			{
				UI_InvTargetShow(playerid, "");
			}
			else
			{
				UI_InvDropConfirmShow(playerid);
			}
			return true;
		}
		case 9404:
		{
			if (!response)
			{
				if (g_UiInvQty[playerid][g_UiInvSel[playerid]] > 1 && UI_InvSelOk(playerid))
				{
					UI_InvQtyShow(playerid, "");
				}
				else
				{
					UI_InvActionShow(playerid);
				}
				return true;
			}
			if (!UI_InvSelOk(playerid) || g_UiInvSelQty[playerid] < 1 || g_UiInvSelQty[playerid] > g_UiInvQty[playerid][g_UiInvSel[playerid]])
			{
				UI_InvActionShow(playerid);
				return true;
			}
			new target = 0;
			if (!UI_InvParseNum(inputtext, target))
			{
				UI_InvTargetShow(playerid, "{FF6666}Inserisci l'ID numerico del giocatore.\n\n");
				return true;
			}
			if (target < 0 || target >= MAX_PLAYERS || !IsPlayerConnected(target))
			{
				UI_InvTargetShow(playerid, "{FF6666}ID non valido o giocatore non connesso.\n\n");
				return true;
			}
			if (target == playerid)
			{
				UI_InvTargetShow(playerid, "{FF6666}Non puoi dare oggetti a te stesso.\n\n");
				return true;
			}
			new slot = g_UiInvSel[playerid], qty = g_UiInvSelQty[playerid];
			UI_InvSelReset(playerid);
			UI_OnInvGive(playerid, slot, target, qty);
			return true;
		}
		case 9405:
		{
			if (!response)
			{
				UI_InvActionShow(playerid);
				return true;
			}
			if (!UI_InvSelOk(playerid) || g_UiInvSelQty[playerid] < 1 || g_UiInvSelQty[playerid] > g_UiInvQty[playerid][g_UiInvSel[playerid]])
			{
				UI_InvActionShow(playerid);
				return true;
			}
			new slot = g_UiInvSel[playerid], qty = g_UiInvSelQty[playerid];
			UI_InvSelReset(playerid);
			UI_OnInvDrop(playerid, slot, qty);
			return true;
		}
		case 9406:
		{
			if (!g_UiInvRecvActive[playerid])
			{
				return true;
			}
			UI_InvRecvEnd(playerid);
			if (!UI_InvNeedTimer(playerid))
			{
				UI_InvTimerStop(playerid);
			}
			UI_OnInvGiveReply(playerid, response ? 1 : 0);
			return true;
		}
	}
	return false;
}

UI_InvUiDestroy(playerid)
{
	if (!UI_InvValid(playerid))
	{
		return 0;
	}
	UI_InvTimerStop(playerid);
	if (g_UiInvToastCreated[playerid])
	{
		for (new i = 0; i < 3; i++)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiInvToastTd[playerid][i]);
			g_UiInvToastTd[playerid][i] = 65535;
			g_UiInvToastTtl[playerid][i] = 0;
			g_UiInvToastItem[playerid][i] = 0;
			g_UiInvToastDelta[playerid][i] = 0;
		}
	}
	g_UiInvToastCreated[playerid] = 0;
	if (g_UiInvRecvActive[playerid])
	{
		UI_InvRecvEnd(playerid);
	}
	g_UiInvRecvActive[playerid] = 0;
	UI_InvClear(playerid);
	g_UiInvMax[playerid] = 0;
	g_UiInvBag[playerid] = 0;
	g_UiInvPage[playerid] = 0;
	g_UiInvRowCount[playerid] = 0;
	g_UiInvNavNext[playerid] = -1;
	g_UiInvNavPrev[playerid] = -1;
	UI_InvSelReset(playerid);
	return 0;
}
