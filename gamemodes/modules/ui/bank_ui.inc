new g_UiBkCash[MAX_PLAYERS];
new g_UiBkBank[MAX_PLAYERS];
new g_UiBkDepMax[MAX_PLAYERS];
new g_UiBkWdMax[MAX_PLAYERS];
new g_UiBkWdLeft[MAX_PLAYERS];
new g_UiBkXferMin[MAX_PLAYERS];
new g_UiBkXferMax[MAX_PLAYERS];
new g_UiBkSendLeft[MAX_PLAYERS];
new bool:g_UiBkCounter[MAX_PLAYERS];
new g_UiBkOp[MAX_PLAYERS];
new g_UiBkTarget[MAX_PLAYERS];
new bool:g_UiBkConfActive[MAX_PLAYERS];
new g_UiBkConfTarget[MAX_PLAYERS];
new g_UiBkConfAmount[MAX_PLAYERS];
new g_UiBkConfLeft[MAX_PLAYERS];
new g_UiBkConfName[MAX_PLAYERS][25];
new g_UiBkStPage[MAX_PLAYERS];
new bool:g_UiBkStNext[MAX_PLAYERS];
new g_UiBkStCount[MAX_PLAYERS];
new g_UiBkStNavNext[MAX_PLAYERS];
new g_UiBkStNavPrev[MAX_PLAYERS];
new g_UiBkStWhen[MAX_PLAYERS][15][16];
new g_UiBkStKind[MAX_PLAYERS][15][24];
new g_UiBkStOther[MAX_PLAYERS][15][25];
new g_UiBkStAmount[MAX_PLAYERS][15];
new g_UiBkStBalance[MAX_PLAYERS][15];
new g_UiBkToastTd[MAX_PLAYERS];
new bool:g_UiBkToastCreated[MAX_PLAYERS];
new g_UiBkToastTtl[MAX_PLAYERS];
new g_UiBkTimer[MAX_PLAYERS];

bool:UI_BankValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

UI_BankFmt(value, dest[], size)
{
	new s[16], len = 0, bool:neg = value < 0;
	if (neg)
	{
		value = -value;
	}
	valstr(s, value, 0);
	len = strlen(s);
	dest[0] = 0;
	if (neg)
	{
		strcat(dest, "-", size);
	}
	new ch[2];
	for (new i = 0; i < len; i++)
	{
		if (i > 0 && (len - i) % 3 == 0)
		{
			strcat(dest, ".", size);
		}
		ch[0] = s[i];
		ch[1] = 0;
		strcat(dest, ch, size);
	}
}

bool:UI_BankParseAmount(const s[], &value)
{
	new i = 0, len = strlen(s), digits = 0;
	value = 0;
	while (i < len && s[i] == 32)
	{
		i++;
	}
	while (i < len && s[i] >= 48 && s[i] <= 57)
	{
		if (digits >= 9)
		{
			return 0;
		}
		value = value * 10 + (s[i] - 48);
		digits++;
		i++;
	}
	while (i < len && s[i] == 32)
	{
		i++;
	}
	return digits > 0 && i == len && value >= 1;
}

UI_BankFindTarget(const input[])
{
	new text[25], len = strlen(input), start = 0, end = len;
	while (start < len && input[start] == 32)
	{
		start++;
	}
	while (end > start && input[end - 1] == 32)
	{
		end--;
	}
	if (end <= start || end - start > 24)
	{
		return -1;
	}
	strmid(text, input, start, end, 25);
	new bool:numeric = 1;
	for (new i = 0; text[i] != 0; i++)
	{
		if (text[i] < 48 || text[i] > 57)
		{
			numeric = 0;
			break;
		}
	}
	if (numeric)
	{
		new id = strval(text);
		if (id < 0 || id >= 1000 || !IsPlayerConnected(id) || IsPlayerNPC(id))
		{
			return -1;
		}
		return id;
	}
	new found = -1, name[25];
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || IsPlayerNPC(p))
		{
			continue;
		}
		GetPlayerName(p, name, 25);
		if (!strcmp(name, text, 1, 0x7FFFFFFF))
		{
			return p;
		}
		if (strfind(name, text, 1, 0) != -1)
		{
			if (found != -1)
			{
				return -2;
			}
			found = p;
		}
	}
	return found;
}

UI_BankTimerStop(playerid)
{
	if (g_UiBkTimer[playerid] != 0)
	{
		KillTimer(g_UiBkTimer[playerid]);
		g_UiBkTimer[playerid] = 0;
	}
}

bool:UI_BankNeedTimer(playerid)
{
	return g_UiBkConfActive[playerid] || g_UiBkToastTtl[playerid] > 0;
}

UI_BankTimerEnsure(playerid)
{
	if (g_UiBkTimer[playerid] == 0)
	{
		g_UiBkTimer[playerid] = SetTimerEx("UI_BankOnTimer", 1000, 1, "d", playerid);
	}
}

UI_BankConfReset(playerid)
{
	g_UiBkConfActive[playerid] = 0;
	g_UiBkConfTarget[playerid] = -1;
	g_UiBkConfAmount[playerid] = 0;
	g_UiBkConfLeft[playerid] = 0;
	g_UiBkConfName[playerid][0] = 0;
}

bool:UI_BankIsBankDialog(dialogid)
{
	return dialogid >= 9500 && dialogid <= 9505;
}

UI_BankSetBalance(playerid, cash, bank)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	g_UiBkCash[playerid] = cash < 0 ? 0 : cash;
	g_UiBkBank[playerid] = bank < 0 ? 0 : bank;
	return 0;
}

UI_BankSetLimits(playerid, depositMax, withdrawMax, withdrawLeft, xferMin, xferMax, sendLeft)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	g_UiBkDepMax[playerid] = depositMax < 0 ? 0 : depositMax;
	g_UiBkWdMax[playerid] = withdrawMax < 0 ? 0 : withdrawMax;
	g_UiBkWdLeft[playerid] = withdrawLeft;
	g_UiBkXferMin[playerid] = xferMin < 1 ? 1 : xferMin;
	g_UiBkXferMax[playerid] = xferMax < 0 ? 0 : xferMax;
	g_UiBkSendLeft[playerid] = sendLeft;
	return 0;
}

UI_BankToastCreate(playerid)
{
	if (g_UiBkToastCreated[playerid])
	{
		return 0;
	}
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, 130.0, 380.0, "_");
	PlayerTextDrawLetterSize(playerid, td, 0.22, 1.0);
	PlayerTextDrawAlignment(playerid, td, 1);
	PlayerTextDrawColour(playerid, td, 0x66DD66FF);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetProportional(playerid, td, 1);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	g_UiBkToastTd[playerid] = td;
	g_UiBkToastCreated[playerid] = 1;
	return 0;
}

UI_BankToast(playerid, const text[], bool:ok)
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid) || text[0] == 0)
	{
		return 0;
	}
	UI_BankToastCreate(playerid);
	new PlayerText:td = g_UiBkToastTd[playerid];
	Lang_PlayerTextDrawSetString(playerid, td, text);
	PlayerTextDrawColour(playerid, td, ok ? 0x66DD66FF : -10066177);
	PlayerTextDrawShow(playerid, td);
	g_UiBkToastTtl[playerid] = 4;
	UI_BankTimerEnsure(playerid);
	return 0;
}

UI_BankMenuShow(playerid)
{
	new title[64], body[320], a[16], b[16], c[16];
	UI_BankFmt(g_UiBkBank[playerid], a, 16);
	UI_BankFmt(g_UiBkCash[playerid], b, 16);
	UI_BankFmt(g_UiBkBank[playerid], c, 16);
	format(title, 64, "%s - Los Santos Bank", g_UiBkCounter[playerid] ? "Sportello" : "ATM");
	format(body, 320, "Operazione\tDettaglio\nSaldo\t{66DD66}$%s\nDeposita\tContanti: $%s\nPrelievo\tBanca: $%s\nBonifico\t{FFD24D}commissione inclusa\nEstratto conto\tultime %d righe", a, b, c, 15);
	g_UiBkOp[playerid] = 0;
	g_UiBkTarget[playerid] = -1;
	Lang_ShowPlayerDialog(playerid, 9500, 5, title, body, "Scegli", "Chiudi");
}

UI_BankMenu(playerid, bool:counter)
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	g_UiBkCounter[playerid] = counter;
	UI_BankMenuShow(playerid);
	return 0;
}

UI_BankBalanceShow(playerid)
{
	new body[512], cash[16], bank[16], total[16], wd[48], snd[48], tmp[16];
	UI_BankFmt(g_UiBkCash[playerid], cash, 16);
	UI_BankFmt(g_UiBkBank[playerid], bank, 16);
	UI_BankFmt(g_UiBkCash[playerid] + g_UiBkBank[playerid], total, 16);
	if (g_UiBkWdLeft[playerid] >= 0)
	{
		UI_BankFmt(g_UiBkWdLeft[playerid], tmp, 16);
		format(wd, 48, "{FFD24D}$%s", tmp);
	}
	else
	{
		strcopy(wd, "{AAAAAA}n/d", 48);
	}
	if (g_UiBkSendLeft[playerid] >= 0)
	{
		UI_BankFmt(g_UiBkSendLeft[playerid], tmp, 16);
		format(snd, 48, "{FFD24D}$%s", tmp);
	}
	else
	{
		strcopy(snd, "{AAAAAA}n/d", 48);
	}
	format(body, 512, "{FFFFFF}Contanti: {66DD66}$%s\n{FFFFFF}Conto bancario: {66DD66}$%s\n{FFFFFF}Totale: {66DD66}$%s\n\n{FFFFFF}Prelevabile oggi: %s\n{FFFFFF}Inviabile oggi: %s", cash, bank, total, wd, snd);
	Lang_ShowPlayerDialog(playerid, 9505, 0, "Saldo", body, "Menu", "Chiudi");
}

bool:UI_BankRange(playerid, op, &lo, &hi)
{
	lo = 1;
	switch (op)
	{
		case 0:
		{
			hi = g_UiBkCash[playerid];
			if (g_UiBkDepMax[playerid] > 0 && hi > g_UiBkDepMax[playerid])
			{
				hi = g_UiBkDepMax[playerid];
			}
		}
		case 1:
		{
			hi = g_UiBkBank[playerid];
			if (g_UiBkWdMax[playerid] > 0 && hi > g_UiBkWdMax[playerid])
			{
				hi = g_UiBkWdMax[playerid];
			}
			if (g_UiBkWdLeft[playerid] >= 0 && hi > g_UiBkWdLeft[playerid])
			{
				hi = g_UiBkWdLeft[playerid];
			}
		}
		case 2:
		{
			lo = g_UiBkXferMin[playerid];
			hi = g_UiBkBank[playerid];
			if (g_UiBkXferMax[playerid] > 0 && hi > g_UiBkXferMax[playerid])
			{
				hi = g_UiBkXferMax[playerid];
			}
			if (g_UiBkSendLeft[playerid] >= 0 && hi > g_UiBkSendLeft[playerid])
			{
				hi = g_UiBkSendLeft[playerid];
			}
		}
		default:
		{
			hi = 0;
			return 0;
		}
	}
	if (hi > 0x3B9ACA00)
	{
		hi = 0x3B9ACA00;
	}
	return hi >= lo;
}

UI_BankAmountShow(playerid, op, const err[])
{
	new lo = 0, hi = 0;
	if (!UI_BankRange(playerid, op, lo, hi))
	{
		new msg[160];
		switch (op)
		{
			case 0:
			{
				strcopy(msg, "Non hai contanti da depositare (o limite per operazione raggiunto).", 160);
			}
			case 1:
			{
				strcopy(msg, "Non puoi prelevare ora: saldo o limite giornaliero esauriti.", 160);
			}
			default:
			{
				strcopy(msg, "Non puoi inviare bonifici ora: saldo insufficiente o limite giornaliero raggiunto.", 160);
			}
		}
		SendClientMessage(playerid, -10066177, msg);
		UI_BankToast(playerid, "Operazione non disponibile", 0);
		UI_BankMenuShow(playerid);
		return 0;
	}
	g_UiBkOp[playerid] = op;
	new title[64], body[512], sLo[16], sHi[16], sCash[16], sBank[16], sub[96];
	UI_BankFmt(lo, sLo, 16);
	UI_BankFmt(hi, sHi, 16);
	UI_BankFmt(g_UiBkCash[playerid], sCash, 16);
	UI_BankFmt(g_UiBkBank[playerid], sBank, 16);
	switch (op)
	{
		case 0:
		{
			strcopy(title, "Deposita contanti", 64);
			format(sub, 96, "Contanti disponibili: {66DD66}$%s", sCash);
		}
		case 1:
		{
			strcopy(title, "Preleva dal conto", 64);
			format(sub, 96, "Saldo bancario: {66DD66}$%s", sBank);
		}
		default:
		{
			strcopy(title, "Bonifico: importo", 64);
			format(sub, 96, "Saldo bancario: {66DD66}$%s{FFFFFF} (la commissione si aggiunge)", sBank);
		}
	}
	format(body, 512, "%s{FFFFFF}%s\nImporto ammesso: da {FFD24D}$%s{FFFFFF} a {FFD24D}$%s{FFFFFF}.\nInserisci un numero intero:", err, sub, sLo, sHi);
	Lang_ShowPlayerDialog(playerid, 9501, 1, title, body, "Avanti", "Indietro");
	return 0;
}

UI_BankTargetShow(playerid, const err[])
{
	new body[320];
	format(body, 320, "%s{FFFFFF}Inserisci l'{FFD24D}ID{FFFFFF} o parte del {FFD24D}nome{FFFFFF} del giocatore online che riceve il bonifico.\nIl destinatario non deve confermare: il credito e' immediato.", err);
	Lang_ShowPlayerDialog(playerid, 9502, 1, "Bonifico: destinatario", body, "Avanti", "Indietro");
}

UI_BankTransferConfirm(playerid, targetId, amount, fee, balanceAfter, ttlSec)
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (targetId < 0 || targetId >= 1000 || !IsPlayerConnected(targetId) || amount < 1 || fee < 0)
	{
		return 0;
	}
	if (ttlSec <= 0)
	{
		ttlSec = 30;
	}
	g_UiBkConfActive[playerid] = 1;
	g_UiBkConfTarget[playerid] = targetId;
	g_UiBkConfAmount[playerid] = amount;
	g_UiBkConfLeft[playerid] = ttlSec;
	GetPlayerName(targetId, g_UiBkConfName[playerid], 25);
	UI_BankTimerEnsure(playerid);
	new body[512], sAmount[16], sFee[16], sTotal[16], sAfter[16];
	UI_BankFmt(amount, sAmount, 16);
	UI_BankFmt(fee, sFee, 16);
	UI_BankFmt(amount + fee, sTotal, 16);
	UI_BankFmt(balanceAfter, sAfter, 16);
	format(body, 512, "{FFFFFF}Destinatario: {FFD24D}%s (ID %d)\n{FFFFFF}Importo: {66DD66}$%s\n{FFFFFF}Commissione: {FF6666}$%s\n{FFFFFF}Totale addebitato: {FF6666}$%s\n{FFFFFF}Saldo dopo l'operazione: {66DD66}$%s\n\n{AAAAAA}La conferma scade tra %d secondi.", g_UiBkConfName[playerid], targetId, sAmount, sFee, sTotal, sAfter, ttlSec);
	Lang_ShowPlayerDialog(playerid, 9503, 0, "Conferma bonifico", body, "Conferma", "Annulla");
	return 0;
}

UI_BankTransferConfirmClose(playerid)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	UI_BankConfReset(playerid);
	if (GetPlayerDialogID(playerid) == 9503)
	{
		HidePlayerDialog(playerid);
	}
	if (!UI_BankNeedTimer(playerid))
	{
		UI_BankTimerStop(playerid);
	}
	return 0;
}

UI_BankStatementBegin(playerid, page, bool:hasNext)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	if (page < 0)
	{
		page = 0;
	}
	if (page >= 5)
	{
		page = 4;
	}
	g_UiBkStPage[playerid] = page;
	g_UiBkStNext[playerid] = hasNext && page < 4;
	g_UiBkStCount[playerid] = 0;
	return 0;
}

UI_BankStatementAdd(playerid, const when[], const kind[], const other[], amount, balanceAfter)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	new n = g_UiBkStCount[playerid];
	if (n < 0 || n >= 15)
	{
		return 0;
	}
	strcopy(g_UiBkStWhen[playerid][n], when, 16);
	strcopy(g_UiBkStKind[playerid][n], kind, 24);
	strcopy(g_UiBkStOther[playerid][n], other, 25);
	g_UiBkStAmount[playerid][n] = amount;
	g_UiBkStBalance[playerid][n] = balanceAfter;
	g_UiBkStCount[playerid] = n + 1;
	return 0;
}

UI_BankStatementShow(playerid)
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new body[2048], line[160], title[64], n = g_UiBkStCount[playerid];
	new page = g_UiBkStPage[playerid];
	g_UiBkStNavNext[playerid] = -1;
	g_UiBkStNavPrev[playerid] = -1;
	if (n == 0)
	{
		format(title, 64, "Estratto conto - pag. %d", page + 1);
		Lang_ShowPlayerDialog(playerid, 9504, 0, title, "{FFFFFF}Nessun movimento da mostrare.", page > 0 ? "Precedente" : "Menu", "Chiudi");
		if (page == 0)
		{
			g_UiBkStNavPrev[playerid] = -1;
		}
		return 0;
	}
	strcopy(body, "Data\tOperazione\tImporto\tSaldo\n", 2048);
	for (new i = 0; i < n; i++)
	{
		new sAmt[16], sBal[16], what[56];
		UI_BankFmt(g_UiBkStAmount[playerid][i] < 0 ? -g_UiBkStAmount[playerid][i] : g_UiBkStAmount[playerid][i], sAmt, 16);
		UI_BankFmt(g_UiBkStBalance[playerid][i], sBal, 16);
		if (g_UiBkStOther[playerid][i][0] != 0)
		{
			format(what, 56, "%s %s", g_UiBkStKind[playerid][i], g_UiBkStOther[playerid][i]);
		}
		else
		{
			strcopy(what, g_UiBkStKind[playerid][i], 56);
		}
		format(line, 160, "%s\t%s\t%s$%s\t$%s\n", g_UiBkStWhen[playerid][i], what, g_UiBkStAmount[playerid][i] < 0 ? "{FF6666}-" : "{66DD66}+", sAmt, sBal);
		strcat(body, line, 2048);
	}
	new idx = n;
	if (g_UiBkStNext[playerid])
	{
		strcat(body, "{FFD24D}>> Pagina successiva\t\t\t\n", 2048);
		g_UiBkStNavNext[playerid] = idx++;
	}
	if (page > 0)
	{
		strcat(body, "{FFD24D}<< Pagina precedente\t\t\t\n", 2048);
		g_UiBkStNavPrev[playerid] = idx;
	}
	format(title, 64, "Estratto conto - pag. %d/%d", page + 1, 5);
	Lang_ShowPlayerDialog(playerid, 9504, 5, title, body, "Vai", "Menu");
	return 0;
}

UI_BankReject(playerid, const msg[])
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	UI_BankConfReset(playerid);
	if (msg[0] != 0)
	{
		SendClientMessage(playerid, -10066177, msg);
		UI_BankToast(playerid, msg, 0);
	}
	UI_BankMenuShow(playerid);
	return 0;
}

UI_BankClose(playerid)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	UI_BankConfReset(playerid);
	if (UI_BankIsBankDialog(GetPlayerDialogID(playerid)))
	{
		HidePlayerDialog(playerid);
	}
	if (!UI_BankNeedTimer(playerid))
	{
		UI_BankTimerStop(playerid);
	}
	return 0;
}

UI_BankUiDestroy(playerid)
{
	if (!UI_BankValid(playerid))
	{
		return 0;
	}
	UI_BankTimerStop(playerid);
	if (g_UiBkToastCreated[playerid])
	{
		PlayerTextDrawDestroy(playerid, g_UiBkToastTd[playerid]);
	}
	g_UiBkToastTd[playerid] = 65535;
	g_UiBkToastCreated[playerid] = 0;
	g_UiBkToastTtl[playerid] = 0;
	UI_BankConfReset(playerid);
	g_UiBkCash[playerid] = 0;
	g_UiBkBank[playerid] = 0;
	g_UiBkDepMax[playerid] = 0;
	g_UiBkWdMax[playerid] = 0;
	g_UiBkWdLeft[playerid] = -1;
	g_UiBkXferMin[playerid] = 1;
	g_UiBkXferMax[playerid] = 0;
	g_UiBkSendLeft[playerid] = -1;
	g_UiBkCounter[playerid] = 0;
	g_UiBkOp[playerid] = 0;
	g_UiBkTarget[playerid] = -1;
	g_UiBkStPage[playerid] = 0;
	g_UiBkStNext[playerid] = 0;
	g_UiBkStCount[playerid] = 0;
	g_UiBkStNavNext[playerid] = -1;
	g_UiBkStNavPrev[playerid] = -1;
	return 0;
}

public UI_BankOnTimer(playerid)
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_UiBkToastTtl[playerid] > 0)
	{
		g_UiBkToastTtl[playerid]--;
		if (g_UiBkToastTtl[playerid] <= 0 && g_UiBkToastCreated[playerid])
		{
			PlayerTextDrawHide(playerid, g_UiBkToastTd[playerid]);
		}
	}
	if (g_UiBkConfActive[playerid])
	{
		g_UiBkConfLeft[playerid]--;
		if (g_UiBkConfLeft[playerid] <= 0)
		{
			new target = g_UiBkConfTarget[playerid], amount = g_UiBkConfAmount[playerid];
			UI_BankConfReset(playerid);
			if (GetPlayerDialogID(playerid) == 9503)
			{
				HidePlayerDialog(playerid);
			}
			SendClientMessage(playerid, -2994689, "La conferma del bonifico e' scaduta: nessun addebito.");
			UI_BankToast(playerid, "Bonifico scaduto", 0);
			UI_OnBankTransferExpired(playerid, target, amount);
		}
	}
	if (!UI_BankNeedTimer(playerid))
	{
		UI_BankTimerStop(playerid);
	}
	return 1;
}

bool:UI_BankOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	if (!UI_BankValid(playerid) || !UI_BankIsBankDialog(dialogid))
	{
		return 0;
	}
	switch (dialogid)
	{
		case 9500:
		{
			if (!response)
			{
				return 1;
			}
			UI_OnBankBalanceRefresh(playerid);
			switch (listitem)
			{
				case 0:
				{
					UI_BankBalanceShow(playerid);
				}
				case 1:
				{
					UI_BankAmountShow(playerid, 0, "");
				}
				case 2:
				{
					UI_BankAmountShow(playerid, 1, "");
				}
				case 3:
				{
					g_UiBkTarget[playerid] = -1;
					UI_BankTargetShow(playerid, "");
				}
				case 4:
				{
					UI_BankToast(playerid, "Caricamento estratto conto...", 1);
					UI_OnBankStatement(playerid, 0);
				}
				default:
				{
					UI_BankMenuShow(playerid);
				}
			}
			return 1;
		}
		case 9505:
		{
			if (response)
			{
				UI_BankMenuShow(playerid);
			}
			return 1;
		}
		case 9502:
		{
			if (!response)
			{
				UI_BankMenuShow(playerid);
				return 1;
			}
			new target = UI_BankFindTarget(inputtext);
			if (target == -2)
			{
				UI_BankTargetShow(playerid, "{FF6666}Piu' giocatori corrispondono: usa l'ID o un nome piu' preciso.\n\n");
				return 1;
			}
			if (target < 0)
			{
				UI_BankTargetShow(playerid, "{FF6666}Giocatore non trovato tra gli online.\n\n");
				return 1;
			}
			if (target == playerid)
			{
				UI_BankTargetShow(playerid, "{FF6666}Non puoi inviare un bonifico a te stesso.\n\n");
				return 1;
			}
			g_UiBkTarget[playerid] = target;
			UI_BankAmountShow(playerid, 2, "");
			return 1;
		}
		case 9501:
		{
			new op = g_UiBkOp[playerid];
			if (!response)
			{
				if (op == 2)
				{
					UI_BankTargetShow(playerid, "");
				}
				else
				{
					UI_BankMenuShow(playerid);
				}
				return 1;
			}
			new amount = 0, lo = 0, hi = 0;
			if (!UI_BankRange(playerid, op, lo, hi))
			{
				UI_BankAmountShow(playerid, op, "");
				return 1;
			}
			if (!UI_BankParseAmount(inputtext, amount))
			{
				UI_BankAmountShow(playerid, op, "{FF6666}Importo non valido: solo cifre, intero positivo.\n\n");
				return 1;
			}
			if (amount < lo || amount > hi)
			{
				new err[160], sLo[16], sHi[16];
				UI_BankFmt(lo, sLo, 16);
				UI_BankFmt(hi, sHi, 16);
				format(err, 160, "{FF6666}Importo fuori limite: da $%s a $%s.\n\n", sLo, sHi);
				UI_BankAmountShow(playerid, op, err);
				return 1;
			}
			switch (op)
			{
				case 0:
				{
					UI_OnBankDeposit(playerid, amount);
				}
				case 1:
				{
					UI_OnBankWithdraw(playerid, amount);
				}
				default:
				{
					new target = g_UiBkTarget[playerid];
					if (target < 0 || target >= MAX_PLAYERS || !IsPlayerConnected(target) || target == playerid)
					{
						g_UiBkTarget[playerid] = -1;
						UI_BankTargetShow(playerid, "{FF6666}Il destinatario non e' piu' disponibile: sceglilo di nuovo.\n\n");
						return 1;
					}
					UI_OnBankTransfer(playerid, target, amount);
				}
			}
			return 1;
		}
		case 9503:
		{
			if (!g_UiBkConfActive[playerid])
			{
				return 1;
			}
			new target = g_UiBkConfTarget[playerid], amount = g_UiBkConfAmount[playerid];
			if (!response)
			{
				UI_BankConfReset(playerid);
				if (!UI_BankNeedTimer(playerid))
				{
					UI_BankTimerStop(playerid);
				}
				UI_BankToast(playerid, "Bonifico annullato", 0);
				UI_OnBankTransferCancel(playerid, target, amount);
				return 1;
			}
			new name[25], bool:same = 0;
			if (target >= 0 && target < MAX_PLAYERS && IsPlayerConnected(target))
			{
				GetPlayerName(target, name, 25);
				same = !strcmp(name, g_UiBkConfName[playerid], 0, 0x7FFFFFFF);
			}
			UI_BankConfReset(playerid);
			if (!UI_BankNeedTimer(playerid))
			{
				UI_BankTimerStop(playerid);
			}
			if (!same)
			{
				SendClientMessage(playerid, -10066177, "Il destinatario non e' piu' online: bonifico annullato.");
				UI_BankToast(playerid, "Destinatario non disponibile", 0);
				UI_OnBankTransferCancel(playerid, target, amount);
				return 1;
			}
			UI_OnBankTransferConfirm(playerid, target, amount);
			return 1;
		}
		case 9504:
		{
			if (!response)
			{
				UI_BankMenuShow(playerid);
				return 1;
			}
			if (g_UiBkStCount[playerid] == 0)
			{
				if (g_UiBkStPage[playerid] > 0)
				{
					UI_OnBankStatement(playerid, g_UiBkStPage[playerid] - 1);
				}
				else
				{
					UI_BankMenuShow(playerid);
				}
				return 1;
			}
			if (listitem >= 0 && listitem == g_UiBkStNavNext[playerid] && g_UiBkStNext[playerid])
			{
				UI_BankToast(playerid, "Caricamento estratto conto...", 1);
				UI_OnBankStatement(playerid, g_UiBkStPage[playerid] + 1);
			}
			else if (listitem >= 0 && listitem == g_UiBkStNavPrev[playerid] && g_UiBkStPage[playerid] > 0)
			{
				UI_BankToast(playerid, "Caricamento estratto conto...", 1);
				UI_OnBankStatement(playerid, g_UiBkStPage[playerid] - 1);
			}
			else
			{
				UI_BankStatementShow(playerid);
			}
			return 1;
		}
	}
	return 1;
}
