new Float:s_BankPoints[9][3] = {
	{1462.0, -1010.5, 26.8},
	{1480.5, -1722.0, 13.5},
	{1495.0, -1745.0, 13.5},
	{1731.0, 0xC4E9C000, 13.6},
	{1829.0, -1845.0, 13.6},
	{0x44F50000, -1205.0, 25.1},
	{0x4494C000, -1335.0, 13.6},
	{365.0, 0xC4FF0000, 7.7},
	{0x451BA000, 0xC4D20000, 13.5}
};
new g_BankPickup[9];
new g_BankLabel[9];

bool:Bank_Valid(playerid)
{
	return playerid >= 0 && playerid < 1000 && IsPlayerConnected(playerid);
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
		new Float:dx = (Float:(px) - Float:(s_BankPoints[i][0])), Float:dy = (Float:(py) - Float:(s_BankPoints[i][1])), Float:dz = (Float:(pz) - Float:(s_BankPoints[i][2]));
		new Float:d = floatsqroot((Float:((Float:(dx) * Float:(dx))) + Float:((Float:(dy) * Float:(dy)))));
		if ((Float:(d) > Float:(radius)) || (Float:(floatabs(dz)) > 4.0))
		{
			continue;
		}
		if (best < 0 || (Float:(d) < Float:(bestD)))
		{
			best = i;
			bestD = d;
		}
	}
	return best;
}

bool:Bank_ParseAmount(s[], &value)
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

Bank_FindPlayer(input[])
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
		return id >= 0 && id < 1000 && Util_IsPlaying(id) ? id : -1;
	}
	new found = -1, name[25];
	for (new p = 0; p < 1000; p++)
	{
		if (!Util_IsPlaying(p))
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

bool:Bank_SameIp(a, b)
{
	if (gPlayer[a][PLAYER_DATA_2] == gPlayer[b][PLAYER_DATA_2])
	{
		return 0;
	}
	new ipA[16], ipB[16];
	GetPlayerIp(a, ipA, 16);
	GetPlayerIp(b, ipB, 16);
	return ipA[0] != 0 && !strcmp(ipA, ipB, 0, 0x7FFFFFFF);
}

Bank_LocalOffset()
{
	new h = 0, m = 0, s = 0, now = gettime(h, m, s);
	new off = h * 3600 + m * 60 + s - now % 86400;
	if (off > 43200)
	{
		off -= 86400;
	}
	else
	{
		if (off < -43200)
		{
			off += 86400;
		}
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
}

Bank_KindLabel(kind[], dest[], size)
{
	if (!strcmp(kind, "deposit", 0, 0x7FFFFFFF))
	{
		strcopy(dest, "Deposito", size);
	}
	else
	{
		if (!strcmp(kind, "withdraw", 0, 0x7FFFFFFF))
		{
			strcopy(dest, "Prelievo", size);
		}
		else
		{
			if (!strcmp(kind, "xfer_out", 0, 0x7FFFFFFF))
			{
				strcopy(dest, "Bonifico a", size);
			}
			else
			{
				if (!strcmp(kind, "xfer_in", 0, 0x7FFFFFFF))
				{
					strcopy(dest, "Bonifico da", size);
				}
				else
				{
					if (!strcmp(kind, "fee", 0, 0x7FFFFFFF))
					{
						strcopy(dest, "Commissione", size);
					}
					else
					{
						if (!strcmp(kind, "paycheck", 0, 0x7FFFFFFF))
						{
							strcopy(dest, "Stipendio", size);
						}
						else
						{
							if (!strcmp(kind, "job", 0, 0x7FFFFFFF))
							{
								strcopy(dest, "Compenso lavoro", size);
							}
							else
							{
								if (!strcmp(kind, "faction", 0, 0x7FFFFFFF))
								{
									strcopy(dest, "Stipendio fazione", size);
								}
								else
								{
									if (!strcmp(kind, "bill", 0, 0x7FFFFFFF))
									{
										strcopy(dest, "Bolletta", size);
									}
									else
									{
										if (!strcmp(kind, "house", 0, 0x7FFFFFFF))
										{
											strcopy(dest, "Casa", size);
										}
										else
										{
											if (!strcmp(kind, "vehicle", 0, 0x7FFFFFFF))
											{
												strcopy(dest, "Veicolo", size);
											}
											else
											{
												if (!strcmp(kind, "fine", 0, 0x7FFFFFFF))
												{
													strcopy(dest, "Multa", size);
												}
												else
												{
													if (!strcmp(kind, "refund", 0, 0x7FFFFFFF))
													{
														strcopy(dest, "Rimborso", size);
													}
													else
													{
														if (!strcmp(kind, "admin", 0, 0x7FFFFFFF))
														{
															strcopy(dest, "Staff", size);
														}
														else
														{
															if (!strcmp(kind, "undo", 0, 4))
															{
																strcopy(dest, "Storno", size);
															}
															else
															{
																strcopy(dest, kind, size);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

bool:Bank_OpIs(playerid, op)
{
	return Bank_Valid(playerid) && gPlayer[playerid][PLAYER_DATA_265] != 0 && gPlayer[playerid][PLAYER_DATA_266] == op;
}

Bank_BeginOp(playerid, op, amount, point)
{
	gPlayer[playerid][PLAYER_DATA_265] = 1;
	gPlayer[playerid][PLAYER_DATA_266] = op;
	gPlayer[playerid][PLAYER_DATA_267] = amount;
	gPlayer[playerid][PLAYER_DATA_268] = 0;
	gPlayer[playerid][PLAYER_DATA_269] = 0;
	gPlayer[playerid][PLAYER_DATA_270] = point;
	gPlayer[playerid][PLAYER_DATA_271] = Bank_Tick();
	gPlayer[playerid][PLAYER_DATA_273][0] = 0;
}

Bank_BeginWrite(playerid, op, amount, point)
{
	Bank_BeginOp(playerid, op, amount, point);
	DB_BankMakeOpId(gPlayer[playerid][PLAYER_DATA_273], 37, gPlayer[playerid][PLAYER_DATA_3]);
}

Bank_EndOp(playerid)
{
	gPlayer[playerid][PLAYER_DATA_265] = 0;
	gPlayer[playerid][PLAYER_DATA_266] = 0;
	gPlayer[playerid][PLAYER_DATA_267] = 0;
	gPlayer[playerid][PLAYER_DATA_268] = 0;
	gPlayer[playerid][PLAYER_DATA_269] = 0;
	gPlayer[playerid][PLAYER_DATA_272]++;
}

Bank_StartResync(playerid)
{
	gPlayer[playerid][PLAYER_DATA_265] = 1;
	gPlayer[playerid][PLAYER_DATA_266] = 7;
	gPlayer[playerid][PLAYER_DATA_268] = 0;
	gPlayer[playerid][PLAYER_DATA_271] = Bank_Tick();
	gPlayer[playerid][PLAYER_DATA_272]++;
	SetTimerEx("Bank_OnResyncTimer", 2500, 0, "dd", playerid, gPlayer[playerid][PLAYER_DATA_272]);
}

Bank_ScheduleResync(playerid)
{
	gPlayer[playerid][PLAYER_DATA_272]++;
	SetTimerEx("Bank_OnResyncTimer", 2500, 0, "dd", playerid, gPlayer[playerid][PLAYER_DATA_272]);
}

Bank_ClearPending(playerid, bool:closeUi)
{
	if (closeUi && gPlayer[playerid][PLAYER_DATA_310] != 0)
	{
		UI_BankTransferConfirmClose(playerid);
	}
	gPlayer[playerid][PLAYER_DATA_310] = 0;
	gPlayer[playerid][PLAYER_DATA_311] = 0;
	gPlayer[playerid][PLAYER_DATA_312] = 0;
	gPlayer[playerid][PLAYER_DATA_313] = 0;
}

Bank_ConfirmFailed(playerid)
{
	gPlayer[playerid][PLAYER_DATA_316]++;
	if (gPlayer[playerid][PLAYER_DATA_316] >= 3)
	{
		gPlayer[playerid][PLAYER_DATA_316] = 0;
		gPlayer[playerid][PLAYER_DATA_317] = Bank_Tick() + 60000;
		SendClientMessage(playerid, -10270721, "Troppe conferme di bonifico fallite o scadute: bonifici sospesi per %d secondi.", 60);
	}
}

Bank_Reply(playerid, bool:ui, text[])
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

Bank_Deny(playerid, bool:ui, text[])
{
	Util_Send(playerid, -10270721, text);
	if (ui)
	{
		Bank_ClearPending(playerid, 1);
		UI_BankClose(playerid);
	}
}

Bank_StateGuard(playerid, bool:ui, bool:counterOnly)
{
	if (!Util_IsPlaying(playerid))
	{
		return -1;
	}
	if (gPlayer[playerid][PLAYER_DATA_123] || gPlayer[playerid][PLAYER_DATA_122])
	{
		Bank_Deny(playerid, ui, "Non puoi usare la banca da ferito o ammanettato.");
		return -1;
	}
	if (IsPlayerInAnyVehicle(playerid))
	{
		Bank_Deny(playerid, ui, "Scendi dal veicolo per usare la banca.");
		return -1;
	}
	if (Job_IsOnShift(playerid))
	{
		Bank_Deny(playerid, ui, "Non puoi usare la banca durante un turno di lavoro.");
		return -1;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Bank_Deny(playerid, ui, "Non puoi usare la banca all'interno di un edificio: serve un ATM o uno sportello all'aperto.");
		return -1;
	}
	new point = Bank_NearPoint(playerid, 0);
	if (point < 0)
	{
		Bank_Deny(playerid, ui, "Devi essere vicino a un ATM o a uno sportello bancario.");
		return -1;
	}
	if (counterOnly && !Bank_IsCounter(point))
	{
		Bank_Deny(playerid, ui, "Questo e' un ATM: per /banca usa uno sportello.");
		return -1;
	}
	gPlayer[playerid][PLAYER_DATA_321] = Bank_IsCounter(point) ? 1 : 0;
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
	if (gPlayer[playerid][PLAYER_DATA_265] != 0)
	{
		new op = gPlayer[playerid][PLAYER_DATA_266];
		if ((op == 4 || op == 5) && now - gPlayer[playerid][PLAYER_DATA_271] >= 15000)
		{
			Bank_EndOp(playerid);
		}
		else
		{
			Bank_Reply(playerid, ui, "Hai un'operazione bancaria in corso: attendi.");
			return -1;
		}
	}
	if (gPlayer[playerid][PLAYER_DATA_310] != 0)
	{
		if (now - gPlayer[playerid][PLAYER_DATA_313] >= 35000)
		{
			Bank_ClearPending(playerid, 1);
		}
		else
		{
			Util_Send(playerid, -10270721, "Hai un bonifico in attesa di conferma.");
			return -1;
		}
	}
	if (gPlayer[playerid][PLAYER_DATA_315] != 0 && now - gPlayer[playerid][PLAYER_DATA_315] < 2000)
	{
		Bank_Reply(playerid, ui, "Attendi un attimo prima di un'altra operazione bancaria.");
		return -1;
	}
	gPlayer[playerid][PLAYER_DATA_315] = now;
	return point;
}

Bank_PushUi(playerid)
{
	new bool:counter = gPlayer[playerid][PLAYER_DATA_321] != 0;
	new wdMax = counter ? 50000 : 5000;
	new wdDay = counter ? 100000 : 15000;
	new wdLeft = -1, sendLeft = -1;
	if (gPlayer[playerid][PLAYER_DATA_318])
	{
		wdLeft = gPlayer[playerid][PLAYER_DATA_319] >= wdDay ? 0 : wdDay - gPlayer[playerid][PLAYER_DATA_319];
		sendLeft = gPlayer[playerid][PLAYER_DATA_320] >= 50000 ? 0 : 50000 - gPlayer[playerid][PLAYER_DATA_320];
	}
	UI_BankSetBalance(playerid, gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89]);
	UI_BankSetLimits(playerid, counter ? 0 : 20000, wdMax, wdLeft, 10, 25000, sendLeft);
}

Bank_RefreshLimits(playerid)
{
	if (!Bank_Valid(playerid) || gPlayer[playerid][PLAYER_DATA_265] != 0 || gPlayer[playerid][PLAYER_DATA_310] != 0 || gPlayer[playerid][PLAYER_DATA_3] <= 0)
	{
		return 0;
	}
	Bank_BeginOp(playerid, 4, 0, gPlayer[playerid][PLAYER_DATA_270]);
	gPlayer[playerid][PLAYER_DATA_268] = 0;
	DB_BankLimits(playerid, gPlayer[playerid][PLAYER_DATA_3]);
}

Bank_CmdSaldo(playerid)
{
	SendClientMessage(playerid, -2994689, "--- Saldo ---");
	SendClientMessage(playerid, -1, "Contanti: $%d  Banca: $%d  Totale: $%d", gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89], gPlayer[playerid][PLAYER_DATA_88] + gPlayer[playerid][PLAYER_DATA_89]);
}

Bank_CmdAtm(playerid, bool:counterOnly)
{
	new point = Bank_Guard(playerid, 0, counterOnly);
	if (point < 0)
	{
		return 0;
	}
	Bank_PushUi(playerid);
	UI_BankMenu(playerid, Bank_IsCounter(point));
	Bank_RefreshLimits(playerid);
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
	new point = Bank_Guard(playerid, ui, 0);
	if (point < 0)
	{
		return 0;
	}
	if (amount < 1)
	{
		Bank_Reply(playerid, ui, "Importo non valido.");
		return 0;
	}
	if (amount > gPlayer[playerid][PLAYER_DATA_88])
	{
		Bank_Reply(playerid, ui, "Non hai abbastanza contanti.");
		return 0;
	}
	if (!Bank_IsCounter(point) && amount > 20000)
	{
		new msg[176];
		format(msg, 176, "Limite ATM per deposito: $%d per operazione. Usa uno sportello per importi maggiori.", 20000);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (amount > Money_BankRoom(playerid))
	{
		Bank_Reply(playerid, ui, "Il tuo conto raggiungerebbe il limite massimo consentito.");
		return 0;
	}
	Bank_BeginWrite(playerid, 1, amount, point);
	gPlayer[playerid][PLAYER_DATA_268] = ui ? 1 : 0;
	DB_BankDeposit(playerid, gPlayer[playerid][PLAYER_DATA_3], amount, Bank_IsCounter(point) ? 1 : 0, gPlayer[playerid][PLAYER_DATA_273]);
}

public UI_OnBankDeposit(playerid, amount)
{
	if (Bank_Valid(playerid))
	{
		Bank_Deposit(playerid, amount, 1);
	}
	return 1;
}

Bank_ResultText(op, result, dest[], size)
{
	switch (result)
	{
		case 0:
		{
			strcopy(dest, "Operazione rifiutata.", size);
		}
		case 2:
		{
			strcopy(dest, op == 3 ? "Il conto del destinatario ha raggiunto il limite: bonifico rifiutato." : "Il tuo conto raggiungerebbe il limite massimo consentito.", size);
		}
		case 3:
		{
			strcopy(dest, op == 3 ? "Fondi insufficienti: servono importo piu' commissione." : "Fondi insufficienti sul conto.", size);
		}
		case 4:
		{
			strcopy(dest, op == 3 ? "Importo fuori dai limiti del bonifico." : "Importo oltre il limite per operazione di questo punto: usa uno sportello.", size);
		}
		case 5:
		{
			strcopy(dest, op == 3 ? "Limite giornaliero di bonifici in uscita raggiunto." : "Limite giornaliero di prelievo raggiunto.", size);
		}
		case 6:
		{
			strcopy(dest, "Il destinatario ha raggiunto il limite giornaliero di bonifici in entrata.", size);
		}
		case 7:
		{
			strcopy(dest, "Sistema bancario occupato: riprova tra un attimo.", size);
		}
		default:
		{
			strcopy(dest, "Operazione bancaria non riuscita.", size);
		}
	}
}

Bank_StartUndo(playerid, reason[])
{
	new opId[37];
	strcopy(opId, gPlayer[playerid][PLAYER_DATA_273], 37);
	gPlayer[playerid][PLAYER_DATA_266] = 6;
	gPlayer[playerid][PLAYER_DATA_268] = 0;
	gPlayer[playerid][PLAYER_DATA_271] = Bank_Tick();
	Util_Send(playerid, -10270721, reason);
	DB_BankUndo(opId);
}

public Bank_OnDBDeposit(playerid, amount, result, balance)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (!Bank_OpIs(playerid, 1) || gPlayer[playerid][PLAYER_DATA_267] != amount)
	{
		if (result == 1)
		{
			new opId[37];
			DB_BankLastOpId(playerid, opId, 37);
			Log_Write("BANK", "deposito OK senza lock corrispondente: annullamento");
			DB_BankUndo(opId);
		}
		return 1;
	}
	new bool:ui = gPlayer[playerid][PLAYER_DATA_268] != 0;
	if (result == 1)
	{
		if (!Money_Take(playerid, amount))
		{
			Bank_StartUndo(playerid, "Contanti insufficienti al momento del deposito: operazione annullata.");
			Char_RefreshHud(playerid);
			return 1;
		}
		Bank_EndOp(playerid);
		Char_Save(playerid);
		Char_RefreshHud(playerid);
		new msg[176];
		format(msg, 176, "Hai depositato $%d. Contanti: $%d  Banca: $%d.", amount, gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89]);
		Util_Send(playerid, 0x66DD66FF, msg);
		if (ui)
		{
			UI_BankToast(playerid, "Deposito eseguito", 1);
		}
		return 1;
	}
	if (result == -1)
	{
		Util_Send(playerid, -10270721, "Errore del sistema bancario: nessun importo e' stato spostato. Verifico il saldo, attendi.");
		Bank_StartResync(playerid);
		return 1;
	}
	Bank_EndOp(playerid);
	new text[176];
	Bank_ResultText(1, result, text, 176);
	Bank_Reply(playerid, ui, text);
	return 1;
}

Bank_Withdraw(playerid, amount, bool:ui)
{
	new point = Bank_Guard(playerid, ui, 0);
	if (point < 0)
	{
		return 0;
	}
	if (amount < 1)
	{
		Bank_Reply(playerid, ui, "Importo non valido.");
		return 0;
	}
	if (amount > gPlayer[playerid][PLAYER_DATA_89])
	{
		Bank_Reply(playerid, ui, "Fondi insufficienti sul conto.");
		return 0;
	}
	new bool:counter = Bank_IsCounter(point), maxOp = counter ? 50000 : 5000;
	if (amount > maxOp)
	{
		new msg[176];
		format(msg, 176, "Limite %s per prelievo: $%d per operazione.%s", counter ? "dello sportello" : "ATM", maxOp, counter ? "" : " Usa uno sportello per importi maggiori.");
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (amount > 0x3B9ACA00 - gPlayer[playerid][PLAYER_DATA_88])
	{
		Bank_Reply(playerid, ui, "Hai troppi contanti: non puoi prelevare questa somma.");
		return 0;
	}
	Bank_BeginWrite(playerid, 2, amount, point);
	gPlayer[playerid][PLAYER_DATA_268] = ui ? 1 : 0;
	DB_BankWithdraw(playerid, gPlayer[playerid][PLAYER_DATA_3], amount, counter ? 1 : 0, gPlayer[playerid][PLAYER_DATA_273]);
}

public UI_OnBankWithdraw(playerid, amount)
{
	if (Bank_Valid(playerid))
	{
		Bank_Withdraw(playerid, amount, 1);
	}
	return 1;
}

public Bank_OnDBWithdraw(playerid, amount, result, balance)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (!Bank_OpIs(playerid, 2) || gPlayer[playerid][PLAYER_DATA_267] != amount)
	{
		if (result == 1)
		{
			new opId[37];
			DB_BankLastOpId(playerid, opId, 37);
			Log_Write("BANK", "prelievo OK senza lock corrispondente: annullamento");
			DB_BankUndo(opId);
		}
		return 1;
	}
	new bool:ui = gPlayer[playerid][PLAYER_DATA_268] != 0;
	if (result == 1)
	{
		if (!Money_Give(playerid, amount))
		{
			Bank_StartUndo(playerid, "Non puoi ricevere i contanti (limite raggiunto): prelievo annullato.");
			Char_RefreshHud(playerid);
			return 1;
		}
		if (gPlayer[playerid][PLAYER_DATA_318])
		{
			gPlayer[playerid][PLAYER_DATA_319] += amount;
		}
		Bank_EndOp(playerid);
		Char_Save(playerid);
		Char_RefreshHud(playerid);
		new msg[176];
		format(msg, 176, "Hai prelevato $%d. Contanti: $%d  Banca: $%d.", amount, gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89]);
		Util_Send(playerid, 0x66DD66FF, msg);
		if (ui)
		{
			UI_BankToast(playerid, "Prelievo eseguito", 1);
		}
		return 1;
	}
	if (result == -1)
	{
		Util_Send(playerid, -10270721, "Errore del sistema bancario: nessun importo e' stato spostato. Verifico il saldo, attendi.");
		Bank_StartResync(playerid);
		return 1;
	}
	Bank_EndOp(playerid);
	new text[176];
	Bank_ResultText(2, result, text, 176);
	if (result == 5 && gPlayer[playerid][PLAYER_DATA_318])
	{
		gPlayer[playerid][PLAYER_DATA_318] = 0;
	}
	Bank_Reply(playerid, ui, text);
	return 1;
}

public Bank_OnDBUndone(playerid, opId[], result)
{
	for (new i = 0; i < 1000; i++)
	{
		if (!Bank_OpIs(i, 6))
		{
			continue;
		}
		if (opId[0] != 0 && strcmp(gPlayer[i][PLAYER_DATA_273], opId, 0, 0x7FFFFFFF) != 0)
		{
			continue;
		}
		if (result == 1)
		{
			Bank_EndOp(i);
			Char_RefreshHud(i);
			SendClientMessage(i, -1, "Operazione annullata: nessun importo spostato. Contanti: $%d  Banca: $%d.", gPlayer[i][PLAYER_DATA_88], gPlayer[i][PLAYER_DATA_89]);
		}
		else
		{
			if (result == -1 && gPlayer[i][PLAYER_DATA_268] < 3)
			{
				new retryId[37];
				strcopy(retryId, gPlayer[i][PLAYER_DATA_273], 37);
				gPlayer[i][PLAYER_DATA_268]++;
				DB_BankUndo(retryId);
			}
			else
			{
				new msg[176], name[25];
				GetPlayerName(i, name, 25);
				format(msg, 176, "ANOMALIA annullamento banca %s op %s esito %d: verificare a mano (saldo e contanti)", name, gPlayer[i][PLAYER_DATA_273], result);
				Log_Write("BANK", msg);
				Bank_EndOp(i);
				Util_Send(i, -10270721, "Errore bancario non risolto automaticamente: contatta lo staff (segnala data e ora).");
			}
		}
	}
	return 1;
}

public Bank_OnResyncTimer(playerid, tok)
{
	if (!Bank_OpIs(playerid, 7) || gPlayer[playerid][PLAYER_DATA_272] != tok)
	{
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_268] >= 3)
	{
		new msg[176], name[25];
		GetPlayerName(playerid, name, 25);
		format(msg, 176, "resync banca fallito per %s dopo %d tentativi: verificare a mano", name, 3);
		Log_Write("BANK", msg);
		Bank_EndOp(playerid);
		Util_Send(playerid, -10270721, "Impossibile verificare il saldo bancario ora: controlla con /saldo piu' tardi o contatta lo staff.");
		return 1;
	}
	gPlayer[playerid][PLAYER_DATA_268]++;
	DB_BankResync(playerid, gPlayer[playerid][PLAYER_DATA_3]);
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
		SendClientMessage(playerid, -1, "Saldo bancario verificato: $%d.", gPlayer[playerid][PLAYER_DATA_89]);
		return 1;
	}
	Bank_ScheduleResync(playerid);
	return 1;
}

bool:Bank_XferCheck(playerid, target, amount, bool:ui)
{
	new now = Bank_Tick(), msg[176];
	if (gPlayer[playerid][PLAYER_DATA_317] != 0)
	{
		new left = gPlayer[playerid][PLAYER_DATA_317] - now;
		if (left > 0)
		{
			format(msg, 176, "Bonifici sospesi per altri %d secondi (troppe conferme fallite).", (left + 1000 - 1) / 1000);
			Bank_Reply(playerid, ui, msg);
			return 0;
		}
		gPlayer[playerid][PLAYER_DATA_317] = 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_90] < 3)
	{
		format(msg, 176, "Servono almeno il livello %d per fare bonifici.", 3);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_113] < 3600000)
	{
		format(msg, 176, "Servono almeno %d minuti di gioco con questo personaggio per fare bonifici.", 60);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (target < 0 || target >= 1000 || target == playerid || !Util_IsPlaying(target) || gPlayer[target][PLAYER_DATA_3] <= 0)
	{
		Bank_Reply(playerid, ui, "Destinatario non valido: deve essere un altro giocatore online e in gioco.");
		return 0;
	}
	if (amount < 10 || amount > 25000)
	{
		format(msg, 176, "Importo del bonifico: da $%d a $%d.", 10, 25000);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	new fee = DB_BankXferFee(amount), total = amount + fee;
	if (gPlayer[playerid][PLAYER_DATA_89] < total)
	{
		format(msg, 176, "Fondi insufficienti: servono $%d (importo $%d + commissione $%d), hai $%d.", total, amount, fee, gPlayer[playerid][PLAYER_DATA_89]);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	if (amount > 0x3B9ACA00 - gPlayer[target][PLAYER_DATA_89])
	{
		Bank_Reply(playerid, ui, "Il destinatario non puo' ricevere questa somma (conto al limite).");
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_314] != 0 && now - gPlayer[playerid][PLAYER_DATA_314] < 30000)
	{
		format(msg, 176, "Attendi ancora %d secondi prima di un nuovo bonifico.", (30000 - (now - gPlayer[playerid][PLAYER_DATA_314]) + 1000 - 1) / 1000);
		Bank_Reply(playerid, ui, msg);
		return 0;
	}
	return 1;
}

Bank_Transfer(playerid, target, amount, bool:ui)
{
	new point = Bank_Guard(playerid, ui, 0);
	if (point < 0)
	{
		return 0;
	}
	if (!Bank_XferCheck(playerid, target, amount, ui))
	{
		return 0;
	}
	Bank_BeginOp(playerid, 4, amount, point);
	gPlayer[playerid][PLAYER_DATA_268] = ui ? 2 : 1;
	gPlayer[playerid][PLAYER_DATA_269] = target + 1;
	DB_BankLimits(playerid, gPlayer[playerid][PLAYER_DATA_3]);
}

public UI_OnBankTransfer(playerid, targetId, amount)
{
	if (Bank_Valid(playerid))
	{
		Bank_Transfer(playerid, targetId, amount, 1);
	}
	return 1;
}

Bank_ShowConfirm(playerid, target, amount)
{
	new fee = DB_BankXferFee(amount);
	gPlayer[playerid][PLAYER_DATA_310] = target + 1;
	gPlayer[playerid][PLAYER_DATA_311] = amount;
	gPlayer[playerid][PLAYER_DATA_312] = gPlayer[target][PLAYER_DATA_3];
	gPlayer[playerid][PLAYER_DATA_313] = Bank_Tick();
	UI_BankSetBalance(playerid, gPlayer[playerid][PLAYER_DATA_88], gPlayer[playerid][PLAYER_DATA_89]);
	UI_BankTransferConfirm(playerid, target, amount, fee, gPlayer[playerid][PLAYER_DATA_89] - amount - fee, 30);
}

public Bank_OnDBLimits(playerid, result, withdrawn, xferOut, xferIn, sinceXfer)
{
	if (!Bank_OpIs(playerid, 4))
	{
		return 1;
	}
	new aux = gPlayer[playerid][PLAYER_DATA_268], amount = gPlayer[playerid][PLAYER_DATA_267], target = gPlayer[playerid][PLAYER_DATA_269] - 1;
	Bank_EndOp(playerid);
	if (result == 1)
	{
		gPlayer[playerid][PLAYER_DATA_318] = 1;
		gPlayer[playerid][PLAYER_DATA_319] = withdrawn;
		gPlayer[playerid][PLAYER_DATA_320] = xferOut;
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
		Bank_Reply(playerid, ui, result == 7 ? "Sistema bancario occupato: riprova tra un attimo." : "Servizio bancario non disponibile: riprova piu' tardi.");
		return 1;
	}
	if (Bank_StateGuard(playerid, ui, 0) < 0)
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
		new msg[176];
		format(msg, 176, "Attendi ancora %d secondi prima di un nuovo bonifico.", left);
		Bank_Reply(playerid, ui, msg);
		return 1;
	}
	if (xferOut >= 50000 || amount > 50000 - xferOut)
	{
		new msg[176], room = xferOut >= 50000 ? 0 : 50000 - xferOut;
		format(msg, 176, "Limite giornaliero di bonifici in uscita: puoi ancora inviare $%d nelle prossime %d ore.", room, 24);
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
	if (gPlayer[playerid][PLAYER_DATA_310] == targetId + 1 && gPlayer[playerid][PLAYER_DATA_311] == amount)
	{
		Bank_ClearPending(playerid, 0);
	}
	return 1;
}

public UI_OnBankTransferExpired(playerid, targetId, amount)
{
	if (!Bank_Valid(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_310] == targetId + 1 && gPlayer[playerid][PLAYER_DATA_311] == amount)
	{
		Bank_ClearPending(playerid, 0);
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
	if (gPlayer[playerid][PLAYER_DATA_310] != targetId + 1 || gPlayer[playerid][PLAYER_DATA_311] != amount)
	{
		return 1;
	}
	new pendChar = gPlayer[playerid][PLAYER_DATA_312], pendAt = gPlayer[playerid][PLAYER_DATA_313];
	Bank_ClearPending(playerid, 0);
	if (gPlayer[playerid][PLAYER_DATA_265] != 0)
	{
		return 1;
	}
	if (Bank_Tick() - pendAt > 35000)
	{
		Util_Send(playerid, -10270721, "La conferma del bonifico e' scaduta: nessun addebito.");
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	new point = Bank_StateGuard(playerid, 0, 0);
	if (point < 0)
	{
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	if (targetId < 0 || targetId >= 1000 || !Util_IsPlaying(targetId) || gPlayer[targetId][PLAYER_DATA_3] != pendChar)
	{
		Util_Send(playerid, -10270721, "Il destinatario non e' piu' disponibile: bonifico annullato.");
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	if (!Bank_XferCheck(playerid, targetId, amount, 0))
	{
		Bank_ConfirmFailed(playerid);
		return 1;
	}
	new bool:sameIp = Bank_SameIp(playerid, targetId);
	new sName[25], tName[25], fee = DB_BankXferFee(amount);
	GetPlayerName(playerid, sName, 25);
	GetPlayerName(targetId, tName, 25);
	Bank_BeginWrite(playerid, 3, amount, point);
	gPlayer[playerid][PLAYER_DATA_269] = targetId + 1;
	gPlayer[playerid][PLAYER_DATA_314] = Bank_Tick();
	if (amount >= 10000 || sameIp)
	{
		new reason[176];
		format(reason, 176, "%s -> %s $%d (+$%d comm.) op %.8s%s", sName, tName, amount, fee, gPlayer[playerid][PLAYER_DATA_273], sameIp ? " STESSO IP" : "");
		DB_AdminLog(0, gPlayer[playerid][PLAYER_DATA_2], sameIp ? "bank_xfer_ip" : "bank_xfer", reason);
		if (sameIp)
		{
			Log_Write("BANK", reason);
		}
	}
	Char_Save(playerid);
	DB_BankTransfer(playerid, targetId, amount, gPlayer[playerid][PLAYER_DATA_273], sameIp ? 1 : 0);
	return 1;
}

public Bank_OnDBTransfer(playerid, targetid, amount, fee, result, balance)
{
	if (Bank_Valid(playerid))
	{
		new bool:mine = Bank_OpIs(playerid, 3) && gPlayer[playerid][PLAYER_DATA_267] == amount;
		new tName[25];
		if (mine && gPlayer[playerid][PLAYER_DATA_269] > 0 && Bank_Valid(gPlayer[playerid][PLAYER_DATA_269] - 1))
		{
			GetPlayerName(gPlayer[playerid][PLAYER_DATA_269] - 1, tName, 25);
		}
		if (result == 1)
		{
			if (mine)
			{
				Bank_EndOp(playerid);
			}
			gPlayer[playerid][PLAYER_DATA_316] = 0;
			if (gPlayer[playerid][PLAYER_DATA_318])
			{
				gPlayer[playerid][PLAYER_DATA_320] += amount;
			}
			Char_RefreshHud(playerid);
			SendClientMessage(playerid, 0x66DD66FF, "Bonifico di $%d%s%s eseguito. Commissione: $%d. Saldo: $%d.", amount, tName[0] != 0 ? " a " : "", tName, fee, gPlayer[playerid][PLAYER_DATA_89]);
		}
		else
		{
			if (result == -1)
			{
				Util_Send(playerid, -10270721, "Errore del sistema bancario: esito del bonifico incerto. Verifico il saldo, attendi.");
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
				gPlayer[playerid][PLAYER_DATA_314] = 0;
				new text[176];
				Bank_ResultText(3, result, text, 176);
				Util_Send(playerid, -10270721, text);
				Bank_ConfirmFailed(playerid);
			}
		}
	}
	if (targetid >= 0 && targetid < 1000 && result == 1 && Bank_Valid(targetid))
	{
		new sName[25];
		if (Bank_Valid(playerid))
		{
			GetPlayerName(playerid, sName, 25);
		}
		Char_RefreshHud(targetid);
		SendClientMessage(targetid, 0x66DD66FF, "Hai ricevuto un bonifico di $%d%s%s. Saldo: $%d.", amount, sName[0] != 0 ? " da " : "", sName, gPlayer[targetid][PLAYER_DATA_89]);
	}
	return 1;
}

Bank_Statement(playerid, page, bool:ui)
{
	new point = Bank_Guard(playerid, ui, 0);
	if (point < 0)
	{
		return 0;
	}
	if (page < 0 || page >= 5)
	{
		page = 0;
	}
	Bank_BeginOp(playerid, 5, 0, point);
	gPlayer[playerid][PLAYER_DATA_268] = page;
	gPlayer[playerid][PLAYER_DATA_269] = ui ? 1 : 0;
	DB_BankStatement(playerid, gPlayer[playerid][PLAYER_DATA_3], page);
}

public UI_OnBankStatement(playerid, page)
{
	if (Bank_Valid(playerid))
	{
		Bank_Statement(playerid, page, 1);
	}
	return 1;
}

public Bank_OnDBStatement(playerid, result, page, count, bool:more)
{
	if (!Bank_OpIs(playerid, 5))
	{
		return 1;
	}
	new bool:ui = gPlayer[playerid][PLAYER_DATA_269] != 0;
	Bank_EndOp(playerid);
	if (result != 1)
	{
		Bank_Reply(playerid, ui, result == 7 ? "Sistema bancario occupato: riprova tra un attimo." : "Estratto conto non disponibile: riprova piu' tardi.");
		return 1;
	}
	UI_BankStatementBegin(playerid, page, more);
	for (new i = 0; i < count; i++)
	{
		new when[16], kind[24], other[25];
		Bank_FormatWhen(g_BankStmt[i][BANK_STMT_0], when, 16);
		Bank_KindLabel(g_BankStmt[i][BANK_STMT_1], kind, 24);
		if (g_BankStmt[i][BANK_STMT_16][0] != 0)
		{
			strcopy(other, g_BankStmt[i][BANK_STMT_16], 25);
		}
		else
		{
			strcopy(other, g_BankStmt[i][BANK_STMT_41], 25);
		}
		UI_BankStatementAdd(playerid, when, kind, other, g_BankStmt[i][BANK_STMT_14], g_BankStmt[i][BANK_STMT_15]);
	}
	UI_BankStatementShow(playerid);
	return 1;
}

Bank_CmdDeposita(playerid, params[])
{
	new amount = 0;
	if (!Bank_ParseAmount(params, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /deposita importo (intero positivo, vicino a un ATM o sportello)");
		return 0;
	}
	Bank_Deposit(playerid, amount, 0);
}

Bank_CmdPreleva(playerid, params[])
{
	new amount = 0;
	if (!Bank_ParseAmount(params, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /preleva importo (intero positivo, vicino a un ATM o sportello)");
		return 0;
	}
	Bank_Withdraw(playerid, amount, 0);
}

Bank_CmdBonifico(playerid, params[])
{
	new pos = 0, tTarget[25], tAmount[12], amount = 0;
	Util_NextToken(params, pos, tTarget, 25);
	Util_NextToken(params, pos, tAmount, 12);
	if (tTarget[0] == 0 || !Bank_ParseAmount(tAmount, amount))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /bonifico id|nome importo (commissione 2%, serve un ATM o uno sportello)");
		return 0;
	}
	new target = Bank_FindPlayer(tTarget);
	if (target == -2)
	{
		Util_Send(playerid, -10270721, "Piu' giocatori corrispondono: usa l'ID o un nome piu' preciso.");
		return 0;
	}
	if (target < 0)
	{
		Util_Send(playerid, -10270721, "Giocatore non trovato tra quelli in gioco.");
		return 0;
	}
	Bank_Transfer(playerid, target, amount, 0);
}

Bank_CmdEstratto(playerid)
{
	Bank_Statement(playerid, 0, 0);
}

Bank_OnInit()
{
	for (new i = 0; i < 9; i++)
	{
		new bool:counter = Bank_IsCounter(i);
		g_BankPickup[i] = CreatePickup(1274, 1, s_BankPoints[i][0], s_BankPoints[i][1], s_BankPoints[i][2], 0);
		g_BankLabel[i] = Lang_Create3DTextLabel(counter ? "Banca - Sportello\nUsa /atm" : "ATM\nUsa /atm", 0x66DD66FF, s_BankPoints[i][0], s_BankPoints[i][1], (1.0 + Float:(s_BankPoints[i][2])), 12.0, 0, 0);
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
		g_BankLabel[i] = 65535;
	}
}

Bank_OnPlayerConnect(playerid)
{
	if (playerid < 0 || playerid >= 1000)
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_265] = 0;
}

Bank_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= 1000)
	{
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_265] != 0 && gPlayer[playerid][PLAYER_DATA_266] >= 1 && gPlayer[playerid][PLAYER_DATA_266] <= 3)
	{
		new msg[176], name[25];
		GetPlayerName(playerid, name, 25);
		format(msg, 176, "%s esce con operazione bancaria %d in volo (importo %d, op %s)", name, gPlayer[playerid][PLAYER_DATA_266], gPlayer[playerid][PLAYER_DATA_267], gPlayer[playerid][PLAYER_DATA_273]);
		Log_Write("BANK", msg);
	}
	gPlayer[playerid][PLAYER_DATA_310] = 0;
}
