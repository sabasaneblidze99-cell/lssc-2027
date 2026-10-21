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
	new s[48], len = 0, bool:neg = value < 0;
	if (neg)
	{
		value = -value;
	}
	valstr(s, value, false);
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
	return digits > 0 && i == len && value >= 1;
}

UI_BankFindTarget(const input[])
{
	new text[75], len = strlen(input), start = 0, end = len;
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
	strmid(text, input, start, end, 75);
	new bool:numeric = true;
	for (new i = 0; text[i] != 0; i++)
	{
		if (text[i] < 48 || text[i] > 57)
		{
			numeric = false;
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
	new found = -1, name[75];
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || IsPlayerNPC(p))
		{
			continue;
		}
		GetPlayerName(p, name, 75);
		if (!strcmp(name, text, true, 0x7FFFFFFF))
		{
			return p;
		}
		if (strfind(name, text, true, 0) != -1)
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
		g_UiBkTimer[playerid] = SetTimerEx("UI_BankOnTimer", 1000, true, "d", playerid);
	}
}

UI_BankConfReset(playerid)
{
	g_UiBkConfActive[playerid] = false;
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
	PlayerTextDrawAlignment(playerid, td, TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, td, 0x66DD66FF);
	PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
	PlayerTextDrawSetProportional(playerid, td, true);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	g_UiBkToastTd[playerid] = _:td;
	g_UiBkToastCreated[playerid] = true;
	return 0;
}

UI_BankToast(playerid, const text[], bool:ok)
{
	if (!UI_BankValid(playerid) || !IsPlayerConnected(playerid) || text[0] == 0)
	{
		return 0;
	}
	UI_BankToastCreate(playerid);
	new PlayerText:td = PlayerText:g_UiBkToastTd[playerid];
	Lang_PlayerTextDrawSetString(playerid, td, text);
	PlayerTextDrawColour(playerid, td, ok ? 0x66DD66FF : -10066177);
	PlayerTextDrawShow(playerid, td);
	g_UiBkToastTtl[playerid] = 4;
	UI_BankTimerEnsure(playerid);
	return 0;
}

UI_BankMenuShow(playerid)
{
	new title[576], body[320], a[48], b[48], c[48];
	UI_BankFmt(g_UiBkBank[playerid], a, 48);
	UI_BankFmt(g_UiBkCash[playerid], b, 48);
	UI_BankFmt(g_UiBkBank[playerid], c, 48);
	format(title, 576, "%s - Los Santos Bank", g_UiBkCounter[playerid] ? "სალარო" : "ATM");
	format(body, 320, "ოპერაცია\tდეტალი\nბალანსი\t{66DD66}$%s\nშეტანა\tნაღდი: $%s\nგამოტანა\tბანკი: $%s\nგადარიცხვა\t{FFD24D}საკომისიოს ჩათვლით\nამონაწერი\tბოლო %d ჩანაწერი", a, b, c, 15);
	g_UiBkOp[playerid] = 0;
	g_UiBkTarget[playerid] = -1;
	Lang_ShowPlayerDialog(playerid, 9500, DIALOG_STYLE_TABLIST_HEADERS, title, body, "არჩევა", "დახურვა");
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
	new body[1024], cash[48], bank[48], total[48], wd[144], snd[144], tmp[48];
	UI_BankFmt(g_UiBkCash[playerid], cash, 48);
	UI_BankFmt(g_UiBkBank[playerid], bank, 48);
	UI_BankFmt(g_UiBkCash[playerid] + g_UiBkBank[playerid], total, 48);
	if (g_UiBkWdLeft[playerid] >= 0)
	{
		UI_BankFmt(g_UiBkWdLeft[playerid], tmp, 48);
		format(wd, 144, "{FFD24D}$%s", tmp);
	}
	else
	{
		strcopy(wd, "{AAAAAA}n/d", 144);
	}
	if (g_UiBkSendLeft[playerid] >= 0)
	{
		UI_BankFmt(g_UiBkSendLeft[playerid], tmp, 48);
		format(snd, 144, "{FFD24D}$%s", tmp);
	}
	else
	{
		strcopy(snd, "{AAAAAA}n/d", 144);
	}
	format(body, 1024, "{FFFFFF}ნაღდი ფული: {66DD66}$%s\n{FFFFFF}საბანკო ანგარიში: {66DD66}$%s\n{FFFFFF}სულ: {66DD66}$%s\n\n{FFFFFF}დღეს გამოსატანი: %s\n{FFFFFF}დღეს გადასარიცხი: %s", cash, bank, total, wd, snd);
	Lang_ShowPlayerDialog(playerid, 9505, DIALOG_STYLE_MSGBOX, "ბალანსი", body, "მენიუ", "დახურვა");
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
			return false;
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
		new msg[480];
		switch (op)
		{
			case 0:
			{
				strcopy(msg, "შესატანი ნაღდი ფული არ გაქვს (ან ოპერაციის ლიმიტი ამოიწურა).", 160);
			}
			case 1:
			{
				strcopy(msg, "ახლა ვერ გამოიტან: ბალანსი ან დღიური ლიმიტი ამოწურულია.", 160);
			}
			default:
			{
				strcopy(msg, "ახლა ვერ გადარიცხავ: ბალანსი არ გყოფნის ან დღიური ლიმიტი ამოიწურა.", 160);
			}
		}
		SendClientMessage(playerid, -10066177, msg);
		UI_BankToast(playerid, "ოპერაცია მიუწვდომელია", false);
		UI_BankMenuShow(playerid);
		return 0;
	}
	g_UiBkOp[playerid] = op;
	new title[576], body[1024], sLo[48], sHi[48], sCash[48], sBank[48], sub[288];
	UI_BankFmt(lo, sLo, 48);
	UI_BankFmt(hi, sHi, 48);
	UI_BankFmt(g_UiBkCash[playerid], sCash, 48);
	UI_BankFmt(g_UiBkBank[playerid], sBank, 48);
	switch (op)
	{
		case 0:
		{
			strcopy(title, "ნაღდი ფულის შეტანა", 64);
			format(sub, 288, "ხელმისაწვდომი ნაღდი: {66DD66}$%s", sCash);
		}
		case 1:
		{
			strcopy(title, "ანგარიშიდან გამოტანა", 64);
			format(sub, 288, "საბანკო ბალანსი: {66DD66}$%s", sBank);
		}
		default:
		{
			strcopy(title, "გადარიცხვა: თანხა", 64);
			format(sub, 288, "საბანკო ბალანსი: {66DD66}$%s{FFFFFF} (საკომისიო ემატება)", sBank);
		}
	}
	format(body, 1024, "%s{FFFFFF}%s\nდასაშვები თანხა: {FFD24D}$%s{FFFFFF}-დან {FFD24D}$%s{FFFFFF}-მდე.\nშეიყვანე მთელი რიცხვი:", err, sub, sLo, sHi);
	Lang_ShowPlayerDialog(playerid, 9501, DIALOG_STYLE_INPUT, title, body, "შემდეგი", "უკან");
	return 0;
}

UI_BankTargetShow(playerid, const err[])
{
	new body[640];
	format(body, 640, "%s{FFFFFF}შეიყვანე იმ ონლაინ მოთამაშის {FFD24D}ID{FFFFFF} ან {FFD24D}სახელის{FFFFFF} ნაწილი, ვინც გადარიცხვას იღებს.\nმიმღებმა არ უნდა დაადასტუროს: თანხა მაშინვე ჩაირიცხება.", err);
	Lang_ShowPlayerDialog(playerid, 9502, DIALOG_STYLE_INPUT, "გადარიცხვა: მიმღები", body, "შემდეგი", "უკან");
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
	g_UiBkConfActive[playerid] = true;
	g_UiBkConfTarget[playerid] = targetId;
	g_UiBkConfAmount[playerid] = amount;
	g_UiBkConfLeft[playerid] = ttlSec;
	GetPlayerName(targetId, g_UiBkConfName[playerid], 25);
	UI_BankTimerEnsure(playerid);
	new body[1024], sAmount[48], sFee[48], sTotal[48], sAfter[48];
	UI_BankFmt(amount, sAmount, 48);
	UI_BankFmt(fee, sFee, 48);
	UI_BankFmt(amount + fee, sTotal, 48);
	UI_BankFmt(balanceAfter, sAfter, 48);
	format(body, 1024, "{FFFFFF}მიმღები: {FFD24D}%s (ID %d)\n{FFFFFF}თანხა: {66DD66}$%s\n{FFFFFF}საკომისიო: {FF6666}$%s\n{FFFFFF}სულ ჩამოგეჭრება: {FF6666}$%s\n{FFFFFF}ბალანსი ოპერაციის შემდეგ: {66DD66}$%s\n\n{AAAAAA}დადასტურების ვადა %d წამში იწურება.", g_UiBkConfName[playerid], targetId, sAmount, sFee, sTotal, sAfter, ttlSec);
	Lang_ShowPlayerDialog(playerid, 9503, DIALOG_STYLE_MSGBOX, "გადარიცხვის დადასტურება", body, "დადასტურება", "გაუქმება");
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
	new body[4096], line[480], title[576], n = g_UiBkStCount[playerid];
	new page = g_UiBkStPage[playerid];
	g_UiBkStNavNext[playerid] = -1;
	g_UiBkStNavPrev[playerid] = -1;
	if (n == 0)
	{
		format(title, 576, "ამონაწერი - გვ. %d", page + 1);
		Lang_ShowPlayerDialog(playerid, 9504, DIALOG_STYLE_MSGBOX, title, "{FFFFFF}საჩვენებელი ოპერაციები არ არის.", page > 0 ? "წინა" : "მენიუ", "დახურვა");
		if (page == 0)
		{
			g_UiBkStNavPrev[playerid] = -1;
		}
		return 0;
	}
	strcopy(body, "თარიღი\tოპერაცია\tთანხა\tბალანსი\n", 2048);
	for (new i = 0; i < n; i++)
	{
		new sAmt[48], sBal[48], what[168];
		UI_BankFmt(g_UiBkStAmount[playerid][i] < 0 ? -g_UiBkStAmount[playerid][i] : g_UiBkStAmount[playerid][i], sAmt, 48);
		UI_BankFmt(g_UiBkStBalance[playerid][i], sBal, 48);
		if (g_UiBkStOther[playerid][i][0] != 0)
		{
			format(what, 168, "%s %s", g_UiBkStKind[playerid][i], g_UiBkStOther[playerid][i]);
		}
		else
		{
			strcopy(what, g_UiBkStKind[playerid][i], 168);
		}
		format(line, 480, "%s\t%s\t%s$%s\t$%s\n", g_UiBkStWhen[playerid][i], what, g_UiBkStAmount[playerid][i] < 0 ? "{FF6666}-" : "{66DD66}+", sAmt, sBal);
		strcat(body, line, 4096);
	}
	new idx = n;
	if (g_UiBkStNext[playerid])
	{
		strcat(body, "{FFD24D}>> შემდეგი გვერდი\t\t\t\n", 4096);
		g_UiBkStNavNext[playerid] = idx++;
	}
	if (page > 0)
	{
		strcat(body, "{FFD24D}<< წინა გვერდი\t\t\t\n", 4096);
		g_UiBkStNavPrev[playerid] = idx;
	}
	format(title, 576, "ამონაწერი - გვ. %d/%d", page + 1, 5);
	Lang_ShowPlayerDialog(playerid, 9504, DIALOG_STYLE_TABLIST_HEADERS, title, body, "გადასვლა", "მენიუ");
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
		UI_BankToast(playerid, msg, false);
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
		PlayerTextDrawDestroy(playerid, PlayerText:g_UiBkToastTd[playerid]);
	}
	g_UiBkToastTd[playerid] = 65535;
	g_UiBkToastCreated[playerid] = false;
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
	g_UiBkCounter[playerid] = false;
	g_UiBkOp[playerid] = 0;
	g_UiBkTarget[playerid] = -1;
	g_UiBkStPage[playerid] = 0;
	g_UiBkStNext[playerid] = false;
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
			PlayerTextDrawHide(playerid, PlayerText:g_UiBkToastTd[playerid]);
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
			SendClientMessage(playerid, -2994689, "გადარიცხვის დადასტურების ვადა ამოიწურა: თანხა არ ჩამოგეჭრა.");
			UI_BankToast(playerid, "გადარიცხვის ვადა ამოიწურა", false);
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
		return false;
	}
	switch (dialogid)
	{
		case 9500:
		{
			if (!response)
			{
				return true;
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
					UI_BankToast(playerid, "ამონაწერი იტვირთება...", true);
					UI_OnBankStatement(playerid, 0);
				}
				default:
				{
					UI_BankMenuShow(playerid);
				}
			}
			return true;
		}
		case 9505:
		{
			if (response)
			{
				UI_BankMenuShow(playerid);
			}
			return true;
		}
		case 9502:
		{
			if (!response)
			{
				UI_BankMenuShow(playerid);
				return true;
			}
			new target = UI_BankFindTarget(inputtext);
			if (target == -2)
			{
				UI_BankTargetShow(playerid, "{FF6666}რამდენიმე მოთამაშე ემთხვევა: გამოიყენე ID ან უფრო ზუსტი სახელი.\n\n");
				return true;
			}
			if (target < 0)
			{
				UI_BankTargetShow(playerid, "{FF6666}ონლაინ მოთამაშეებში ვერ მოიძებნა.\n\n");
				return true;
			}
			if (target == playerid)
			{
				UI_BankTargetShow(playerid, "{FF6666}საკუთარ თავს ვერ გადაურიცხავ.\n\n");
				return true;
			}
			g_UiBkTarget[playerid] = target;
			UI_BankAmountShow(playerid, 2, "");
			return true;
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
				return true;
			}
			new amount = 0, lo = 0, hi = 0;
			if (!UI_BankRange(playerid, op, lo, hi))
			{
				UI_BankAmountShow(playerid, op, "");
				return true;
			}
			if (!UI_BankParseAmount(inputtext, amount))
			{
				UI_BankAmountShow(playerid, op, "{FF6666}არასწორი თანხა: მხოლოდ ციფრები, დადებითი მთელი რიცხვი.\n\n");
				return true;
			}
			if (amount < lo || amount > hi)
			{
				new err[480], sLo[48], sHi[48];
				UI_BankFmt(lo, sLo, 48);
				UI_BankFmt(hi, sHi, 48);
				format(err, 480, "{FF6666}თანხა ლიმიტს გარეთაა: $%s-დან $%s-მდე.\n\n", sLo, sHi);
				UI_BankAmountShow(playerid, op, err);
				return true;
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
						UI_BankTargetShow(playerid, "{FF6666}მიმღები აღარ არის ხელმისაწვდომი: აირჩიე ხელახლა.\n\n");
						return true;
					}
					UI_OnBankTransfer(playerid, target, amount);
				}
			}
			return true;
		}
		case 9503:
		{
			if (!g_UiBkConfActive[playerid])
			{
				return true;
			}
			new target = g_UiBkConfTarget[playerid], amount = g_UiBkConfAmount[playerid];
			if (!response)
			{
				UI_BankConfReset(playerid);
				if (!UI_BankNeedTimer(playerid))
				{
					UI_BankTimerStop(playerid);
				}
				UI_BankToast(playerid, "გადარიცხვა გაუქმდა", false);
				UI_OnBankTransferCancel(playerid, target, amount);
				return true;
			}
			new name[75], bool:same = false;
			if (target >= 0 && target < MAX_PLAYERS && IsPlayerConnected(target))
			{
				GetPlayerName(target, name, 75);
				same = !strcmp(name, g_UiBkConfName[playerid], false, 0x7FFFFFFF);
			}
			UI_BankConfReset(playerid);
			if (!UI_BankNeedTimer(playerid))
			{
				UI_BankTimerStop(playerid);
			}
			if (!same)
			{
				SendClientMessage(playerid, -10066177, "მიმღები აღარ არის ონლაინ: გადარიცხვა გაუქმდა.");
				UI_BankToast(playerid, "მიმღები მიუწვდომელია", false);
				UI_OnBankTransferCancel(playerid, target, amount);
				return true;
			}
			UI_OnBankTransferConfirm(playerid, target, amount);
			return true;
		}
		case 9504:
		{
			if (!response)
			{
				UI_BankMenuShow(playerid);
				return true;
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
				return true;
			}
			if (listitem >= 0 && listitem == g_UiBkStNavNext[playerid] && g_UiBkStNext[playerid])
			{
				UI_BankToast(playerid, "ამონაწერი იტვირთება...", true);
				UI_OnBankStatement(playerid, g_UiBkStPage[playerid] + 1);
			}
			else if (listitem >= 0 && listitem == g_UiBkStNavPrev[playerid] && g_UiBkStPage[playerid] > 0)
			{
				UI_BankToast(playerid, "ამონაწერი იტვირთება...", true);
				UI_OnBankStatement(playerid, g_UiBkStPage[playerid] - 1);
			}
			else
			{
				UI_BankStatementShow(playerid);
			}
			return true;
		}
	}
	return true;
}
