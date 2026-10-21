new Float:s_BankPoints[9][3] = {
	{1462.0, -1010.5, 26.8},
	{1480.5, -1722.0, 13.5},
	{1495.0, -1745.0, 13.5},
	{1731.0, -1870.0, 13.6},
	{1829.0, -1845.0, 13.6},
	{1960.0, -1205.0, 25.1},
	{1190.0, -1335.0, 13.6},
	{365.0, -2040.0, 7.7},
	{2490.0, -1680.0, 13.5}
};
new g_BankPickup[9];
new Text3D:g_BankLabel[9];

bool:Bank_Valid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid);
}

Bank_Tick()
{
	new now = GetTickCount();
	return now != 0 ? now : 1;
}

bool:Bank_IsCounter(point)
{
	return point >= 0 && point < 2;
}

Bank_NearPoint(playerid, bool:counterOnly)
{
	if (!Bank_Valid(playerid))
	{
		return -1;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		return -1;
	}
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	new best = -1, Float:bestD;
	for (new i = 0; i < 9; i++)
	{
		if (counterOnly && !Bank_IsCounter(i))
		{
			continue;
		}
		new Float:radius = Float:((Bank_IsCounter(i) ? 2.5 : 2.0));
		new Float:dx = (px - s_BankPoints[i][0]), Float:dy = (py - s_BankPoints[i][1]), Float:dz = (pz - s_BankPoints[i][2]);
		new Float:d = floatsqroot(((dx * dx) + (dy * dy)));
		if ((d > radius) || (floatabs(dz) > 4.0))
		{
			continue;
		}
		if (best < 0 || (d < bestD))
		{
			best = i;
			bestD = d;
		}
	}
	return best;
}

bool:Bank_ParseAmount(const s[], &value)
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

Bank_FindPlayer(const input[])
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
		return id >= 0 && id < 1000 && Util_IsPlaying(id) ? id : -1;
	}
	new found = -1, name[75];
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!Util_IsPlaying(p))
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

bool:Bank_SameIp(a, b)
{
	if (gPlayer[a][pAccountId] == gPlayer[b][pAccountId])
	{
		return false;
	}
	new ipA[48], ipB[48];
	GetPlayerIp(a, ipA, 48);
	GetPlayerIp(b, ipB, 48);
	return ipA[0] != 0 && !strcmp(ipA, ipB, false, 0x7FFFFFFF);
}

Bank_LocalOffset()
{
	new h = 0, m = 0, s = 0, now = gettime(h, m, s);
	new off = h * 3600 + m * 60 + s - now % 86400;
	if (off > 43200)
	{
		off -= 86400;
	}
	else if (off < -43200)
	{
		off += 86400;
	}
	return off;
}

Bank_FormatWhen(ts, dest[], size)
{
	if (ts <= 0)
	{
		format(dest, size, "--/-- --:--");
		return 0;
	}
	new local = ts + Bank_LocalOffset();
	if (local < 0)
	{
		local = 0;
	}
	new days = local / 86400, secs = local % 86400;
	new z = days + 719468, era = z / 146097, doe = z - era * 146097;
	new yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	new doy = doe - (yoe * 365 + yoe / 4 - yoe / 100), mp = (doy * 5 + 2) / 153;
	new day = doy - (mp * 153 + 2) / 5 + 1, month = mp < 10 ? mp + 3 : mp - 9;
	format(dest, size, "%02d/%02d %02d:%02d", day, month, secs / 3600, secs % 3600 / 60);
	return 0;
}

Bank_KindLabel(const kind[], dest[], size)
{
	if (!strcmp(kind, "deposit", false, 0x7FFFFFFF))
	{
		strcopy(dest, "შეტანა", size);
	}
	else if (!strcmp(kind, "withdraw", false, 0x7FFFFFFF))
	{
		strcopy(dest, "გატანა", size);
	}
	else if (!strcmp(kind, "xfer_out", false, 0x7FFFFFFF))
	{
		strcopy(dest, "გადარიცხვა:", size);
	}
	else if (!strcmp(kind, "xfer_in", false, 0x7FFFFFFF))
	{
		strcopy(dest, "გადმორიცხვა:", size);
	}
	else if (!strcmp(kind, "fee", false, 0x7FFFFFFF))
	{
		strcopy(dest, "საკომისიო", size);
	}
	else if (!strcmp(kind, "paycheck", false, 0x7FFFFFFF))
	{
		strcopy(dest, "ხელფასი", size);
	}
	else if (!strcmp(kind, "job", false, 0x7FFFFFFF))
	{
		strcopy(dest, "სამსახურის ანაზღაურება", size);
	}
	else if (!strcmp(kind, "faction", false, 0x7FFFFFFF))
	{
		strcopy(dest, "ფრაქციის ხელფასი", size);
	}
	else if (!strcmp(kind, "bill", false, 0x7FFFFFFF))
	{
		strcopy(dest, "კომუნალური", size);
	}
	else if (!strcmp(kind, "house", false, 0x7FFFFFFF))
	{
		strcopy(dest, "სახლი", size);
	}
	else if (!strcmp(kind, "vehicle", false, 0x7FFFFFFF))
	{
		strcopy(dest, "მანქანა", size);
	}
	else if (!strcmp(kind, "fine", false, 0x7FFFFFFF))
	{
		strcopy(dest, "ჯარიმა", size);
	}
	else if (!strcmp(kind, "refund", false, 0x7FFFFFFF))
	{
		strcopy(dest, "დაბრუნება", size);
	}
	else if (!strcmp(kind, "admin", false, 0x7FFFFFFF))
	{
		strcopy(dest, "ადმინისტრაცია", size);
	}
	else if (!strcmp(kind, "undo", false, 4))
	{
		strcopy(dest, "სტორნო", size);
	}
	else
	{
		strcopy(dest, kind, size);
	}
}

bool:Bank_OpIs(playerid, op)
{
	return Bank_Valid(playerid) && gPlayer[playerid][pBankOpBusy] != 0 && gPlayer[playerid][pBankOp] == op;
}

Bank_BeginOp(playerid, op, amount, point)
{
	gPlayer[playerid][pBankOpBusy] = 1;
	gPlayer[playerid][pBankOp] = op;
	gPlayer[playerid][pBankOpAmount] = amount;
	gPlayer[playerid][pBankOpAux] = 0;
	gPlayer[playerid][pBankOpTarget] = 0;
	gPlayer[playerid][pBankOpPoint] = point;
	gPlayer[playerid][pBankOpTick] = Bank_Tick();
	gPlayer[playerid][pBankOpId][0] = 0;
}

Bank_BeginWrite(playerid, op, amount, point)
{
	Bank_BeginOp(playerid, op, amount, point);
	DB_BankMakeOpId(gPlayer[playerid][pBankOpId], 37, gPlayer[playerid][pCharId]);
}

Bank_EndOp(playerid)
{
	gPlayer[playerid][pBankOpBusy] = 0;
	gPlayer[playerid][pBankOp] = 0;
	gPlayer[playerid][pBankOpAmount] = 0;
	gPlayer[playerid][pBankOpAux] = 0;
	gPlayer[playerid][pBankOpTarget] = 0;
	gPlayer[playerid][pBankOpToken]++;
}

Bank_StartResync(playerid)
{
	gPlayer[playerid][pBankOpBusy] = 1;
	gPlayer[playerid][pBankOp] = 7;
	gPlayer[playerid][pBankOpAux] = 0;
	gPlayer[playerid][pBankOpTick] = Bank_Tick();
	gPlayer[playerid][pBankOpToken]++;
	SetTimerEx("Bank_OnResyncTimer", 2500, false, "dd", playerid, gPlayer[playerid][pBankOpToken]);
}

Bank_ScheduleResync(playerid)
{
	gPlayer[playerid][pBankOpToken]++;
	SetTimerEx("Bank_OnResyncTimer", 2500, false, "dd", playerid, gPlayer[playerid][pBankOpToken]);
}

Bank_ClearPending(playerid, bool:closeUi)
{
	if (closeUi && gPlayer[playerid][pBankPendTarget] != 0)
	{
		UI_BankTransferConfirmClose(playerid);
	}
	gPlayer[playerid][pBankPendTarget] = 0;
	gPlayer[playerid][pBankPendAmount] = 0;
	gPlayer[playerid][pBankPendCharId] = 0;
	gPlayer[playerid][pBankPendTick] = 0;
}

Bank_ConfirmFailed(playerid)
{
	gPlayer[playerid][pBankConfirmFails]++;
	if (gPlayer[playerid][pBankConfirmFails] >= 3)
	{
		gPlayer[playerid][pBankConfirmFails] = 0;
		gPlayer[playerid][pBankXferBlockUntil] = Bank_Tick() + 60000;
		SendClientMessage(playerid, -10270721, "ბევრი ჩავარდნილი ან ვადაგასული დადასტურება: გადარიცხვები შეჩერებულია %d წამით.", 60);
	}
}

Bank_Reply(playerid, bool:ui, const text[])
{
	if (ui)
	{
		UI_BankReject(playerid, text);
	}
	else
	{
		Util_Send(playerid, -10270721, text);
	}
}

Bank_Deny(playerid, bool:ui, const text[])
{
	Util_Send(playerid, -10270721, text);
	if (ui)
	{
		Bank_ClearPending(playerid, true);
		UI_BankClose(playerid);
	}
}

Bank_StateGuard(playerid, bool:ui, bool:counterOnly)
{
	if (!Util_IsPlaying(playerid))
	{
		return -1;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Bank_Deny(playerid, ui, "დაჭრილს ან ბორკილდადებულს ბანკით სარგებლობა არ შეგიძლია.");
		return -1;
	}
	if (IsPlayerInAnyVehicle(playerid))
	{
		Bank_Deny(playerid, ui, "ბანკით სარგებლობისთვის მანქანიდან გადმოდი.");
		return -1;
	}
	if (Job_IsOnShift(playerid))
	{
		Bank_Deny(playerid, ui, "სამსახურის ცვლის დროს ბანკით სარგებლობა არ შეგიძლია.");
		return -1;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Bank_Deny(playerid, ui, "შენობაში ბანკით სარგებლობა არ შეგიძლია: გჭირდება ATM ან გარე სალარო.");
		return -1;
	}
	new point = Bank_NearPoint(playerid, false);
	if (point < 0)
	{
		Bank_Deny(playerid, ui, "ATM-თან ან ბანკის სალაროსთან უნდა იყო.");
		return -1;
	}
	if (counterOnly && !Bank_IsCounter(point))
	{
		Bank_Deny(playerid, ui, "ეს ATM-ია: /banca-სთვის სალარო გამოიყენე.");
		return -1;
	}
	gPlayer[playerid][pBankAtCounter] = Bank_IsCounter(point) ? 1 : 0;
	return point;
}

Bank_Guard(playerid, bool:ui, bool:counterOnly)
{
	new point = Bank_StateGuard(playerid, ui, counterOnly);
	if (point < 0)
	{
		return -1;
	}
	new now = Bank_Tick();
	if (gPlayer[playerid][pBankOpBusy] != 0)
	{
		new op = gPlayer[playerid][pBankOp];
		if ((op == 4 || op == 5) && now - gPlayer[playerid][pBankOpTick] >= 15000)
		{
			Bank_EndOp(playerid);
		}
		else
		{
			Bank_Reply(playerid, ui, "საბანკო ოპერაცია უკვე მიმდინარეობს: მოიცადე.");
			return -1;
		}
	}
	if (gPlayer[playerid][pBankPendTarget] != 0)
	{
		if (now - gPlayer[playerid][pBankPendTick] >= 35000)
		{
			Bank_ClearPending(playerid, true);
		}
		else
		{
			Util_Send(playerid, -10270721, "გადარიცხვა დადასტურებას ელოდება.");
			return -1;
		}
	}
	if (gPlayer[playerid][pBankLastOpTick] != 0 && now - gPlayer[playerid][pBankLastOpTick] < 2000)
	{
		Bank_Reply(playerid, ui, "მოიცადე, სანამ ახალ საბანკო ოპერაციას გააკეთებ.");
		return -1;
	}
	gPlayer[playerid][pBankLastOpTick] = now;
	return point;
}

Bank_PushUi(playerid)
{
	new bool:counter = gPlayer[playerid][pBankAtCounter] != 0;
	new wdMax = counter ? 50000 : 5000;
	new wdDay = counter ? 100000 : 15000;
	new wdLeft = -1, sendLeft = -1;
	if (gPlayer[playerid][pBankLimitsLoaded])
	{
		wdLeft = gPlayer[playerid][pBankWithdrawnToday] >= wdDay ? 0 : wdDay - gPlayer[playerid][pBankWithdrawnToday];
		sendLeft = gPlayer[playerid][pBankXferOutToday] >= 50000 ? 0 : 50000 - gPlayer[playerid][pBankXferOutToday];
	}
	UI_BankSetBalance(playerid, gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
	UI_BankSetLimits(playerid, counter ? 0 : 20000, wdMax, wdLeft, 10, 25000, sendLeft);
}

Bank_RefreshLimits(playerid)
{
	if (!Bank_Valid(playerid) || gPlayer[playerid][pBankOpBusy] != 0 || gPlayer[playerid][pBankPendTarget] != 0 || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	Bank_BeginOp(playerid, 4, 0, gPlayer[playerid][pBankOpPoint]);
	gPlayer[playerid][pBankOpAux] = 0;
	DB_BankLimits(playerid, gPlayer[playerid][pCharId]);
	return 0;
}

Bank_CmdSaldo(playerid)
{
	SendClientMessage(playerid, -2994689, "--- ბალანსი ---");
	SendClientMessage(playerid, -1, "ნაღდი: $%d  ბანკი: $%d  სულ: $%d", gPlayer[playerid][pMoney], gPlayer[playerid][pBank], gPlayer[playerid][pMoney] + gPlayer[playerid][pBank]);
}

Bank_CmdAtm(playerid, bool:counterOnly)
{
	new point = Bank_Guard(playerid, false, counterOnly);
	if (point < 0)
	{
		return 0;
	}
	Bank_PushUi(playerid);
	UI_BankMenu(playerid, Bank_IsCounter(point));
	Bank_RefreshLimits(playerid);
	return 0;
}

public UI_OnBankBalanceRefresh(playerid)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	Bank_PushUi(playerid);
	Bank_RefreshLimits(playerid);
	return 1;
}

Bank_Deposit(playerid, amount, bool:ui)
{
	new point = Bank_Guard(playerid, ui, false);
	if (point < 0)
	{
		return 0;
	}
	if (amount < 1)
	{
		Bank_Reply(playerid, ui, "არასწორი თანხა.");
		return 0;
	}
	if (amount > gPlayer[playerid][pMoney])
	{
		Bank_Reply(playerid, ui, "საკმარისი ნაღდი ფული არ გაქვს.");
		return 0;
	}
	if (!Bank_IsCounter(point) && amount > 20000)
	{
		new msg[528];
		format(msg, 528, "ATM-ის ლიმიტი შეტანაზე: $%d ერთ ოპერაციაზე. მეტი თანხისთვის სალარო გამოიყენე.", 20000);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (amount > Money_BankRoom(playerid))
	{
		Bank_Reply(playerid, ui, "შენი ანგარიში დასაშვებ ლიმიტს გადააჭარბებდა.");
		return 0;
	}
	Bank_BeginWrite(playerid, 1, amount, point);
	gPlayer[playerid][pBankOpAux] = ui ? 1 : 0;
	DB_BankDeposit(playerid, gPlayer[playerid][pCharId], amount, Bank_IsCounter(point) ? 1 : 0, gPlayer[playerid][pBankOpId]);
	return 0;
}

public UI_OnBankDeposit(playerid, amount)
{
	if (Bank_Valid(playerid))
	{
		Bank_Deposit(playerid, amount, true);
	}
	return 1;
}

Bank_ResultText(op, result, dest[], size)
{
	switch (result)
	{
		case 0:
		{
			strcopy(dest, "ოპერაცია უარყოფილია.", size);
		}
		case 2:
		{
			strcopy(dest, op == 3 ? "მიმღების ანგარიში ლიმიტზეა: გადარიცხვა უარყოფილია." : "შენი ანგარიში დასაშვებ ლიმიტს გადააჭარბებდა.", size);
		}
		case 3:
		{
			strcopy(dest, op == 3 ? "არასაკმარისი თანხა: საჭიროა თანხა საკომისიოსთან ერთად." : "ანგარიშზე არასაკმარისი თანხაა.", size);
		}
		case 4:
		{
			strcopy(dest, op == 3 ? "თანხა გადარიცხვის ლიმიტებს სცდება." : "თანხა ამ წერტილის ოპერაციის ლიმიტს აჭარბებს: სალარო გამოიყენე.", size);
		}
		case 5:
		{
			strcopy(dest, op == 3 ? "გამავალი გადარიცხვების დღიური ლიმიტი ამოწურულია." : "გატანის დღიური ლიმიტი ამოწურულია.", size);
		}
		case 6:
		{
			strcopy(dest, "მიმღებმა შემომავალი გადარიცხვების დღიურ ლიმიტს მიაღწია.", size);
		}
		case 7:
		{
			strcopy(dest, "საბანკო სისტემა დატვირთულია: ცოტა ხანში სცადე.", size);
		}
		default:
		{
			strcopy(dest, "საბანკო ოპერაცია ვერ შესრულდა.", size);
		}
	}
}

Bank_StartUndo(playerid, const reason[])
{
	new opId[111];
	strcopy(opId, gPlayer[playerid][pBankOpId], 111);
	gPlayer[playerid][pBankOp] = 6;
	gPlayer[playerid][pBankOpAux] = 0;
	gPlayer[playerid][pBankOpTick] = Bank_Tick();
	Util_Send(playerid, -10270721, reason);
	DB_BankUndo(opId);
}

public Bank_OnDBDeposit(playerid, amount, result, balance)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (!Bank_OpIs(playerid, 1) || gPlayer[playerid][pBankOpAmount] != amount)
	{
		if (result == 1)
		{
			new opId[111];
			DB_BankLastOpId(playerid, opId, 111);
			Log_Write("BANK", "deposito OK senza lock corrispondente: annullamento");
			DB_BankUndo(opId);
		}
		return 1;
	}
	new bool:ui = gPlayer[playerid][pBankOpAux] != 0;
	if (result == 1)
	{
		if (!Money_Take(playerid, amount))
		{
			Bank_StartUndo(playerid, "შეტანისას ნაღდი ფული არ იყო საკმარისი: ოპერაცია გაუქმდა.");
			Char_RefreshHud(playerid);
			return 1;
		}
		Bank_EndOp(playerid);
		Char_Save(playerid);
		Char_RefreshHud(playerid);
		new msg[528];
		format(msg, 528, "შეიტანე $%d. ნაღდი: $%d  ბანკი: $%d.", amount, gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
		Util_Send(playerid, 0x66DD66FF, msg);
		if (ui)
		{
			UI_BankToast(playerid, "თანხა შეტანილია", true);
		}
		return 1;
	}
	if (result == -1)
	{
		Util_Send(playerid, -10270721, "საბანკო სისტემის შეცდომა: თანხა არ გადაადგილებულა. ბალანსს ვამოწმებ, მოიცადე.");
		Bank_StartResync(playerid);
		return 1;
	}
	Bank_EndOp(playerid);
	new text[528];
	Bank_ResultText(1, result, text, 528);
	Bank_Reply(playerid, ui, text);
	return 1;
}

Bank_Withdraw(playerid, amount, bool:ui)
{
	new point = Bank_Guard(playerid, ui, false);
	if (point < 0)
	{
		return 0;
	}
	if (amount < 1)
	{
		Bank_Reply(playerid, ui, "არასწორი თანხა.");
		return 0;
	}
	if (amount > gPlayer[playerid][pBank])
	{
		Bank_Reply(playerid, ui, "ანგარიშზე არასაკმარისი თანხაა.");
		return 0;
	}
	new bool:counter = Bank_IsCounter(point), maxOp = counter ? 50000 : 5000;
	if (amount > maxOp)
	{
		new msg[528];
		format(msg, 528, "%s ლიმიტი გატანაზე: $%d ერთ ოპერაციაზე.%s", counter ? "სალაროს" : "ATM-ის", maxOp, counter ? "" : " მეტი თანხისთვის სალარო გამოიყენე.");
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (amount > 0x3B9ACA00 - gPlayer[playerid][pMoney])
	{
		Bank_Reply(playerid, ui, "ძალიან ბევრი ნაღდი გაქვს: ამ თანხას ვერ გაიტან.");
		return 0;
	}
	Bank_BeginWrite(playerid, 2, amount, point);
	gPlayer[playerid][pBankOpAux] = ui ? 1 : 0;
	DB_BankWithdraw(playerid, gPlayer[playerid][pCharId], amount, counter ? 1 : 0, gPlayer[playerid][pBankOpId]);
	return 0;
}

public UI_OnBankWithdraw(playerid, amount)
{
	if (Bank_Valid(playerid))
	{
		Bank_Withdraw(playerid, amount, true);
	}
	return 1;
}

public Bank_OnDBWithdraw(playerid, amount, result, balance)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (!Bank_OpIs(playerid, 2) || gPlayer[playerid][pBankOpAmount] != amount)
	{
		if (result == 1)
		{
			new opId[111];
			DB_BankLastOpId(playerid, opId, 111);
			Log_Write("BANK", "prelievo OK senza lock corrispondente: annullamento");
			DB_BankUndo(opId);
		}
		return 1;
	}
	new bool:ui = gPlayer[playerid][pBankOpAux] != 0;
	if (result == 1)
	{
		if (!Money_Give(playerid, amount))
		{
			Bank_StartUndo(playerid, "ნაღდ ფულს ვეღარ მიიღებ (ლიმიტი): გატანა გაუქმდა.");
			Char_RefreshHud(playerid);
			return 1;
		}
		if (gPlayer[playerid][pBankLimitsLoaded])
		{
			gPlayer[playerid][pBankWithdrawnToday] += amount;
		}
		Bank_EndOp(playerid);
		Char_Save(playerid);
		Char_RefreshHud(playerid);
		new msg[528];
		format(msg, 528, "გაიტანე $%d. ნაღდი: $%d  ბანკი: $%d.", amount, gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
		Util_Send(playerid, 0x66DD66FF, msg);
		if (ui)
		{
			UI_BankToast(playerid, "თანხა გატანილია", true);
		}
		return 1;
	}
	if (result == -1)
	{
		Util_Send(playerid, -10270721, "საბანკო სისტემის შეცდომა: თანხა არ გადაადგილებულა. ბალანსს ვამოწმებ, მოიცადე.");
		Bank_StartResync(playerid);
		return 1;
	}
	Bank_EndOp(playerid);
	new text[528];
	Bank_ResultText(2, result, text, 528);
	if (result == 5 && gPlayer[playerid][pBankLimitsLoaded])
	{
		gPlayer[playerid][pBankLimitsLoaded] = 0;
	}
	Bank_Reply(playerid, ui, text);
	return 1;
}

public Bank_OnDBUndone(playerid, const opId[], result)
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!Bank_OpIs(i, 6))
		{
			continue;
		}
		if (opId[0] != 0 && strcmp(gPlayer[i][pBankOpId], opId, false, 0x7FFFFFFF) != 0)
		{
			continue;
		}
		if (result == 1)
		{
			Bank_EndOp(i);
			Char_RefreshHud(i);
			SendClientMessage(i, -1, "ოპერაცია გაუქმდა, თანხა არ გადაადგილებულა. ნაღდი: $%d  ბანკი: $%d.", gPlayer[i][pMoney], gPlayer[i][pBank]);
		}
		else if (result == -1 && gPlayer[i][pBankOpAux] < 3)
		{
			new retryId[111];
			strcopy(retryId, gPlayer[i][pBankOpId], 111);
			gPlayer[i][pBankOpAux]++;
			DB_BankUndo(retryId);
		}
		else
		{
			new msg[528], name[75];
			GetPlayerName(i, name, 75);
			format(msg, 528, "ANOMALIA annullamento banca %s op %s esito %d: verificare a mano (saldo e contanti)", name, gPlayer[i][pBankOpId], result);
			Log_Write("BANK", msg);
			Bank_EndOp(i);
			Util_Send(i, -10270721, "საბანკო შეცდომა ავტომატურად ვერ გამოსწორდა: მიმართე ადმინისტრაციას (მიუთითე თარიღი და დრო).");
		}
	}
	return 1;
}

public Bank_OnResyncTimer(playerid, tok)
{
	if (!Bank_OpIs(playerid, 7) || gPlayer[playerid][pBankOpToken] != tok)
	{
		return 1;
	}
	if (gPlayer[playerid][pBankOpAux] >= 3)
	{
		new msg[528], name[75];
		GetPlayerName(playerid, name, 75);
		format(msg, 528, "resync banca fallito per %s dopo %d tentativi: verificare a mano", name, 3);
		Log_Write("BANK", msg);
		Bank_EndOp(playerid);
		Util_Send(playerid, -10270721, "ბალანსის შემოწმება ახლა ვერ ხერხდება: მოგვიანებით გამოიყენე /saldo ან მიმართე ადმინისტრაციას.");
		return 1;
	}
	gPlayer[playerid][pBankOpAux]++;
	DB_BankResync(playerid, gPlayer[playerid][pCharId]);
	return 1;
}

public Bank_OnDBSynced(playerid, result, balance)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (result == 1)
	{
		Char_RefreshHud(playerid);
	}
	if (!Bank_OpIs(playerid, 7))
	{
		return 1;
	}
	if (result == 1)
	{
		Bank_EndOp(playerid);
		SendClientMessage(playerid, -1, "საბანკო ბალანსი შემოწმებულია: $%d.", gPlayer[playerid][pBank]);
		return 1;
	}
	Bank_ScheduleResync(playerid);
	return 1;
}

bool:Bank_XferCheck(playerid, target, amount, bool:ui)
{
	new now = Bank_Tick(), msg[528];
	if (gPlayer[playerid][pBankXferBlockUntil] != 0)
	{
		new left = gPlayer[playerid][pBankXferBlockUntil] - now;
		if (left > 0)
		{
			format(msg, 528, "გადარიცხვები შეჩერებულია კიდევ %d წამით (ბევრი ჩავარდნილი დადასტურება).", (left + 1000 - 1) / 1000);
			Bank_Reply(playerid, ui, msg);
			return false;
		}
		gPlayer[playerid][pBankXferBlockUntil] = 0;
	}
	if (gPlayer[playerid][pLevel] < 3)
	{
		format(msg, 528, "გადარიცხვისთვის მინიმუმ %d დონე გჭირდება.", 3);
		Bank_Reply(playerid, ui, msg);
		return false;
	}
	if (gPlayer[playerid][pPlayedMs] < 3600000)
	{
		format(msg, 528, "გადარიცხვისთვის ამ პერსონაჟით მინიმუმ %d წუთი უნდა გქონდეს ნათამაშები.", 60);
		Bank_Reply(playerid, ui, msg);
		return false;
	}
	if (target < 0 || target >= MAX_PLAYERS || target == playerid || !Util_IsPlaying(target) || gPlayer[target][pCharId] <= 0)
	{
		Bank_Reply(playerid, ui, "არასწორი მიმღები: უნდა იყოს სხვა მოთამაშე, ონლაინ და თამაშში.");
		return false;
	}
	if (amount < 10 || amount > 25000)
	{
		format(msg, 528, "გადარიცხვის თანხა: $%d-დან $%d-მდე.", 10, 25000);
		Bank_Reply(playerid, ui, msg);
		return false;
	}
	new fee = DB_BankXferFee(amount), total = amount + fee;
	if (gPlayer[playerid][pBank] < total)
	{
		format(msg, 528, "არასაკმარისი თანხა: საჭიროა $%d (თანხა $%d + საკომისიო $%d), გაქვს $%d.", total, amount, fee, gPlayer[playerid][pBank]);
		Bank_Reply(playerid, ui, msg);
		return false;
	}
	if (amount > 0x3B9ACA00 - gPlayer[target][pBank])
	{
		Bank_Reply(playerid, ui, "მიმღები ამ თანხას ვერ მიიღებს (ანგარიში ლიმიტზეა).");
		return false;
	}
	if (gPlayer[playerid][pBankLastXferTick] != 0 && now - gPlayer[playerid][pBankLastXferTick] < 30000)
	{
		format(msg, 528, "ახალ გადარიცხვამდე მოიცადე კიდევ %d წამი.", (30000 - (now - gPlayer[playerid][pBankLastXferTick]) + 1000 - 1) / 1000);
		Bank_Reply(playerid, ui, msg);
		return false;
	}
	return true;
}

Bank_Transfer(playerid, target, amount, bool:ui)
{
	new point = Bank_Guard(playerid, ui, false);
	if (point < 0)
	{
		return 0;
	}
	if (!Bank_XferCheck(playerid, target, amount, ui))
	{
		return 0;
	}
	Bank_BeginOp(playerid, 4, amount, point);
	gPlayer[playerid][pBankOpAux] = ui ? 2 : 1;
	gPlayer[playerid][pBankOpTarget] = target + 1;
	DB_BankLimits(playerid, gPlayer[playerid][pCharId]);
	return 0;
}

public UI_OnBankTransfer(playerid, targetId, amount)
{
	if (Bank_Valid(playerid))
	{
		Bank_Transfer(playerid, targetId, amount, true);
	}
	return 1;
}

Bank_ShowConfirm(playerid, target, amount)
{
	new fee = DB_BankXferFee(amount);
	gPlayer[playerid][pBankPendTarget] = target + 1;
	gPlayer[playerid][pBankPendAmount] = amount;
	gPlayer[playerid][pBankPendCharId] = gPlayer[target][pCharId];
	gPlayer[playerid][pBankPendTick] = Bank_Tick();
	UI_BankSetBalance(playerid, gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
	UI_BankTransferConfirm(playerid, target, amount, fee, gPlayer[playerid][pBank] - amount - fee, 30);
}

public Bank_OnDBLimits(playerid, result, withdrawn, xferOut, xferIn, sinceXfer)
{
	if (!Bank_OpIs(playerid, 4))
	{
		return 1;
	}
	new aux = gPlayer[playerid][pBankOpAux], amount = gPlayer[playerid][pBankOpAmount], target = gPlayer[playerid][pBankOpTarget] - 1;
	Bank_EndOp(playerid);
	if (result == 1)
	{
		gPlayer[playerid][pBankLimitsLoaded] = 1;
		gPlayer[playerid][pBankWithdrawnToday] = withdrawn;
		gPlayer[playerid][pBankXferOutToday] = xferOut;
	}
	if (aux == 0)
	{
		if (result == 1)
		{
			Bank_PushUi(playerid);
		}
		return 1;
	}
	new bool:ui = aux == 2;
	if (result != 1)
	{
		Bank_Reply(playerid, ui, result == 7 ? "საბანკო სისტემა დატვირთულია: ცოტა ხანში სცადე." : "საბანკო სერვისი მიუწვდომელია: მოგვიანებით სცადე.");
		return 1;
	}
	if (Bank_StateGuard(playerid, ui, false) < 0)
	{
		return 1;
	}
	if (!Bank_XferCheck(playerid, target, amount, ui))
	{
		return 1;
	}
	if (sinceXfer >= 0 && sinceXfer * 1000 < 30000)
	{
		new left = (30000 - sinceXfer * 1000 + 1000 - 1) / 1000;
		new msg[528];
		format(msg, 528, "ახალ გადარიცხვამდე მოიცადე კიდევ %d წამი.", left);
		Bank_Reply(playerid, ui, msg);
		return 1;
	}
	if (xferOut >= 50000 || amount > 50000 - xferOut)
	{
		new msg[528], room = xferOut >= 50000 ? 0 : 50000 - xferOut;
		format(msg, 528, "გამავალი გადარიცხვების დღიური ლიმიტი: კიდევ $%d შეგიძლია გაგზავნო მომდევნო %d საათში.", room, 24);
		Bank_Reply(playerid, ui, msg);
		return 1;
	}
	Bank_ShowConfirm(playerid, target, amount);
	return 1;
}

public UI_OnBankTransferCancel(playerid, targetId, amount)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][pBankPendTarget] == targetId + 1 && gPlayer[playerid][pBankPendAmount] == amount)
	{
		Bank_ClearPending(playerid, false);
	}
	return 1;
}

public UI_OnBankTransferExpired(playerid, targetId, amount)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][pBankPendTarget] == targetId + 1 && gPlayer[playerid][pBankPendAmount] == amount)
	{
		Bank_ClearPending(playerid, false);
		Bank_ConfirmFailed(playerid);
	}
	return 1;
}

public UI_OnBankTransferConfirm(playerid, targetId, amount)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][pBankPendTarget] != targetId + 1 || gPlayer[playerid][pBankPendAmount] != amount)
	{
		return 1;
	}
	new pendChar = gPlayer[playerid][pBankPendCharId], pendAt = gPlayer[playerid][pBankPendTick];
	Bank_ClearPending(playerid, false);
	if (gPlayer[playerid][pBankOpBusy] != 0)
	{
		return 1;
	}
	if (Bank_Tick() - pendAt > 35000)
	{
		Util_Send(playerid, -10270721, "გადარიცხვის დადასტურების ვადა ამოიწურა: თანხა არ ჩამოგეჭრა.");
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	new point = Bank_StateGuard(playerid, false, false);
	if (point < 0)
	{
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	if (targetId < 0 || targetId >= 1000 || !Util_IsPlaying(targetId) || gPlayer[targetId][pCharId] != pendChar)
	{
		Util_Send(playerid, -10270721, "მიმღები აღარ არის ხელმისაწვდომი: გადარიცხვა გაუქმდა.");
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	if (!Bank_XferCheck(playerid, targetId, amount, false))
	{
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	new bool:sameIp = Bank_SameIp(playerid, targetId);
	new sName[75], tName[75], fee = DB_BankXferFee(amount);
	GetPlayerName(playerid, sName, 75);
	GetPlayerName(targetId, tName, 75);
	Bank_BeginWrite(playerid, 3, amount, point);
	gPlayer[playerid][pBankOpTarget] = targetId + 1;
	gPlayer[playerid][pBankLastXferTick] = Bank_Tick();
	if (amount >= 10000 || sameIp)
	{
		new reason[528];
		format(reason, 528, "%s -> %s $%d (+$%d comm.) op %.8s%s", sName, tName, amount, fee, gPlayer[playerid][pBankOpId], sameIp ? " STESSO IP" : "");
		DB_AdminLog(0, gPlayer[playerid][pAccountId], sameIp ? "bank_xfer_ip" : "bank_xfer", reason);
		if (sameIp)
		{
			Log_Write("BANK", reason);
		}
	}
	Char_Save(playerid);
	DB_BankTransfer(playerid, targetId, amount, gPlayer[playerid][pBankOpId], sameIp ? 1 : 0);
	return 1;
}

public Bank_OnDBTransfer(playerid, targetid, amount, fee, result, balance)
{
	if (Bank_Valid(playerid))
	{
		new bool:mine = Bank_OpIs(playerid, 3) && gPlayer[playerid][pBankOpAmount] == amount;
		new tName[75];
		if (mine && gPlayer[playerid][pBankOpTarget] > 0 && Bank_Valid(gPlayer[playerid][pBankOpTarget] - 1))
		{
			GetPlayerName(gPlayer[playerid][pBankOpTarget] - 1, tName, 75);
		}
		if (result == 1)
		{
			if (mine)
			{
				Bank_EndOp(playerid);
			}
			gPlayer[playerid][pBankConfirmFails] = 0;
			if (gPlayer[playerid][pBankLimitsLoaded])
			{
				gPlayer[playerid][pBankXferOutToday] += amount;
			}
			Char_RefreshHud(playerid);
			SendClientMessage(playerid, 0x66DD66FF, "გადარიცხვა $%d%s%s შესრულდა. საკომისიო: $%d. ბალანსი: $%d.", amount, tName[0] != 0 ? " მიმღები: " : "", tName, fee, gPlayer[playerid][pBank]);
		}
		else if (result == -1)
		{
			Util_Send(playerid, -10270721, "საბანკო სისტემის შეცდომა: გადარიცხვის შედეგი უცნობია. ბალანსს ვამოწმებ, მოიცადე.");
			if (mine)
			{
				Bank_StartResync(playerid);
			}
		}
		else
		{
			if (mine)
			{
				Bank_EndOp(playerid);
			}
			gPlayer[playerid][pBankLastXferTick] = 0;
			new text[528];
			Bank_ResultText(3, result, text, 528);
			Util_Send(playerid, -10270721, text);
			Bank_ConfirmFailed(playerid);
		}
	}
	if (targetid >= 0 && targetid < MAX_PLAYERS && result == 1 && Bank_Valid(targetid))
	{
		new sName[75];
		if (Bank_Valid(playerid))
		{
			GetPlayerName(playerid, sName, 75);
		}
		Char_RefreshHud(targetid);
		SendClientMessage(targetid, 0x66DD66FF, "მიიღე გადარიცხვა $%d%s%s. ბალანსი: $%d.", amount, sName[0] != 0 ? ", გამგზავნი: " : "", sName, gPlayer[targetid][pBank]);
	}
	return 1;
}

Bank_Statement(playerid, page, bool:ui)
{
	new point = Bank_Guard(playerid, ui, false);
	if (point < 0)
	{
		return 0;
	}
	if (page < 0 || page >= 5)
	{
		page = 0;
	}
	Bank_BeginOp(playerid, 5, 0, point);
	gPlayer[playerid][pBankOpAux] = page;
	gPlayer[playerid][pBankOpTarget] = ui ? 1 : 0;
	DB_BankStatement(playerid, gPlayer[playerid][pCharId], page);
	return 0;
}

public UI_OnBankStatement(playerid, page)
{
	if (Bank_Valid(playerid))
	{
		Bank_Statement(playerid, page, true);
	}
	return 1;
}

public Bank_OnDBStatement(playerid, result, page, count, bool:more)
{
	if (!Bank_OpIs(playerid, 5))
	{
		return 1;
	}
	new bool:ui = gPlayer[playerid][pBankOpTarget] != 0;
	Bank_EndOp(playerid);
	if (result != 1)
	{
		Bank_Reply(playerid, ui, result == 7 ? "საბანკო სისტემა დატვირთულია: ცოტა ხანში სცადე." : "ამონაწერი მიუწვდომელია: მოგვიანებით სცადე.");
		return 1;
	}
	UI_BankStatementBegin(playerid, page, more);
	for (new i = 0; i < count; i++)
	{
		new when[48], kind[72], other[75];
		Bank_FormatWhen(g_BankStmt[i][bsTimestamp], when, 48);
		Bank_KindLabel(g_BankStmt[i][bsKind], kind, 72);
		if (g_BankStmt[i][bsOther][0] != 0)
		{
			strcopy(other, g_BankStmt[i][bsOther], 75);
		}
		else
		{
			strcopy(other, g_BankStmt[i][bsRef], 75);
		}
		UI_BankStatementAdd(playerid, when, kind, other, g_BankStmt[i][bsDelta], g_BankStmt[i][bsBalance]);
	}
	UI_BankStatementShow(playerid);
	return 1;
}

Bank_CmdDeposita(playerid, const params[])
{
	new amount = 0;
	if (!Bank_ParseAmount(params, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /deposita თანხა (მთელი დადებითი რიცხვი, ATM-თან ან სალაროსთან)");
		return 0;
	}
	Bank_Deposit(playerid, amount, false);
	return 0;
}

Bank_CmdPreleva(playerid, const params[])
{
	new amount = 0;
	if (!Bank_ParseAmount(params, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /preleva თანხა (მთელი დადებითი რიცხვი, ATM-თან ან სალაროსთან)");
		return 0;
	}
	Bank_Withdraw(playerid, amount, false);
	return 0;
}

Bank_CmdBonifico(playerid, const params[])
{
	new pos = 0, tTarget[75], tAmount[36], amount = 0;
	Util_NextToken(params, pos, tTarget, 75);
	Util_NextToken(params, pos, tAmount, 36);
	if (tTarget[0] == 0 || !Bank_ParseAmount(tAmount, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /bonifico id|სახელი თანხა (საკომისიო 2%, საჭიროა ATM ან სალარო)");
		return 0;
	}
	new target = Bank_FindPlayer(tTarget);
	if (target == -2)
	{
		Util_Send(playerid, -10270721, "რამდენიმე მოთამაშე ემთხვევა: გამოიყენე ID ან უფრო ზუსტი სახელი.");
		return 0;
	}
	if (target < 0)
	{
		Util_Send(playerid, -10270721, "მოთამაშე თამაშში ვერ მოიძებნა.");
		return 0;
	}
	Bank_Transfer(playerid, target, amount, false);
	return 0;
}

Bank_CmdEstratto(playerid)
{
	Bank_Statement(playerid, 0, false);
}

Bank_OnInit()
{
	for (new i = 0; i < 9; i++)
	{
		new bool:counter = Bank_IsCounter(i);
		g_BankPickup[i] = CreatePickup(1274, 1, s_BankPoints[i][0], s_BankPoints[i][1], s_BankPoints[i][2], 0);
		g_BankLabel[i] = Lang_Create3DTextLabel(counter ? "ბანკი - სალარო\nგამოიყენე /atm" : "ATM\nგამოიყენე /atm", 0x66DD66FF, s_BankPoints[i][0], s_BankPoints[i][1], (1.0 + s_BankPoints[i][2]), 12.0, 0, false);
	}
}

Bank_OnExit()
{
	for (new i = 0; i < 9; i++)
	{
		if (g_BankPickup[i] >= 0)
		{
			DestroyPickup(g_BankPickup[i]);
		}
		Delete3DTextLabel(g_BankLabel[i]);
		g_BankPickup[i] = -1;
		g_BankLabel[i] = INVALID_3DTEXT_ID;
	}
}

Bank_OnPlayerConnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	gPlayer[playerid][pBankOpBusy] = 0;
	return 0;
}

Bank_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (gPlayer[playerid][pBankOpBusy] != 0 && gPlayer[playerid][pBankOp] >= 1 && gPlayer[playerid][pBankOp] <= 3)
	{
		new msg[528], name[75];
		GetPlayerName(playerid, name, 75);
		format(msg, 528, "%s esce con operazione bancaria %d in volo (importo %d, op %s)", name, gPlayer[playerid][pBankOp], gPlayer[playerid][pBankOpAmount], gPlayer[playerid][pBankOpId]);
		Log_Write("BANK", msg);
	}
	gPlayer[playerid][pBankPendTarget] = 0;
	return 0;
}
