new g_RtTimer;
new g_RtFee[MAX_PLAYERS];
new g_RtDeposit[MAX_PLAYERS];
new g_RtOpenTs[MAX_PLAYERS];
new g_RtBlocks[MAX_PLAYERS];
new bool:g_RtAbortOpen[MAX_PLAYERS];
new g_RtCloseMode[MAX_PLAYERS];
new g_RtCloseHealth[MAX_PLAYERS];
new g_RtCloseReason[MAX_PLAYERS];
new g_RtRetry[MAX_PLAYERS];
new g_RtRetryAt[MAX_PLAYERS];

Rental_KindName(vkind, dest[], size)
{
	dest[0] = 0;
	switch (vkind)
	{
		case 1:
		{
			strcat(dest, "Faggio", size);
		}
		case 2:
		{
			strcat(dest, "Previon", size);
		}
		case 3:
		{
			strcat(dest, "Manana", size);
		}
		case 4:
		{
			strcat(dest, "ტაქსი", size);
		}
		default:
		{
			strcat(dest, "veicolo", size);
		}
	}
}

bool:Rental_ValidPlayer(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

Rental_MaxLocalRep(playerid)
{
	new best = 0;
	for (new d = 1; d <= 3; d++)
	{
		new r = LD_GetLocalRep(playerid, d);
		if (d == 1 || r > best)
		{
			best = r;
		}
	}
	return best;
}

Rental_VehHealth(vehicleid)
{
	if (vehicleid <= 0 || !IsValidVehicle(vehicleid))
	{
		return 0;
	}
	new Float:hp;
	GetVehicleHealth(vehicleid, hp);
	return LD_Clamp(floatround(hp, floatround_method:1), 0, 1000);
}

Rental_ResetPlayer(playerid)
{
	if (!Rental_ValidPlayer(playerid))
	{
		return 0;
	}
	g_RtFee[playerid] = 0;
	g_RtDeposit[playerid] = 0;
	g_RtOpenTs[playerid] = 0;
	g_RtBlocks[playerid] = 0;
	g_RtAbortOpen[playerid] = false;
	g_RtCloseMode[playerid] = 0;
	g_RtCloseHealth[playerid] = 0;
	g_RtCloseReason[playerid] = 0;
	g_RtRetry[playerid] = 0;
	g_RtRetryAt[playerid] = 0;
	return 0;
}

Rental_VehDestroy(playerid)
{
	new veh = gPlayer[playerid][pRentalVehicle];
	gPlayer[playerid][pRentalVehicle] = 0;
	if (veh <= 0 || veh >= 2000)
	{
		return 0;
	}
	if (LD_VehOwner(veh) == playerid)
	{
		LD_VehRelease(veh);
	}
	if (IsValidVehicle(veh))
	{
		if (IsPlayerConnected(playerid) && GetPlayerVehicleID(playerid) == veh)
		{
			RemovePlayerFromVehicle(playerid, false);
		}
		DestroyVehicle(veh);
	}
	return 0;
}

Rental_ClearLocal(playerid)
{
	Rental_VehDestroy(playerid);
	gPlayer[playerid][pRentalId] = 0;
	gPlayer[playerid][pRentalClass] = 0;
	gPlayer[playerid][pRentalKind] = 0;
	gPlayer[playerid][pRentalDueAt] = 0;
	gPlayer[playerid][pRentalWarnFlags] = 0;
	gPlayer[playerid][pRentalPendOp] = 0;
	gPlayer[playerid][pRentalOpId][0] = 0;
	Rental_ResetPlayer(playerid);
}

Rental_ScheduleRetry(playerid)
{
	g_RtRetry[playerid]++;
	g_RtRetryAt[playerid] = GetTickCount() + 3000;
	if (g_RtRetryAt[playerid] == 0)
	{
		g_RtRetryAt[playerid] = 1;
	}
}

bool:Rental_BeginOpen(playerid, vkind, blocks, rentClass, streetModel = 0)
{
	new model = vkind == SRENT_VKIND ? streetModel : LD_RentalModel(vkind);
	if (model <= 0 || gPlayer[playerid][pRentalId] != 0 || gPlayer[playerid][pRentalPendOp] != 0)
	{
		return false;
	}
	if (DB_LivingBusy(playerid, 1))
	{
		return false;
	}
	Rental_ResetPlayer(playerid);
	gPlayer[playerid][pRentalClass] = rentClass;
	gPlayer[playerid][pRentalKind] = vkind;
	gPlayer[playerid][pRentalPendOp] = 5;
	g_RtBlocks[playerid] = blocks;
	DB_LivingMakeOpId(gPlayer[playerid][pRentalOpId], 41, "ro", vkind, gPlayer[playerid][pCharId]);
	DB_LivingRentOpen(playerid, vkind, model, blocks, gPlayer[playerid][pRentalOpId]);
	return true;
}

bool:Rental_SpawnVehicle(playerid)
{
	if (gPlayer[playerid][pRentalKind] == SRENT_VKIND)
	{
		return StreetRental_Take(playerid);   // auto di strada: si consegna quella parcheggiata
	}
	new vkind = gPlayer[playerid][pRentalKind], model = LD_RentalModel(vkind);
	new bool:pro = gPlayer[playerid][pRentalClass] == 2;
	new Float:x = 1512.0, Float:y = -1708.0, Float:z = 13.5, Float:a = 180.0;
	if (pro)
	{
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
	}
	if (model <= 0 || !LD_PoolHasRoom())
	{
		return false;
	}
	new veh = CreateVehicle(model, x, y, z, a, pro ? 6 : 1, pro ? 6 : 1, -1, false);
	if (veh == 65535 || veh <= 0 || veh >= 2000)
	{
		return false;
	}
	if (LD_VehRegister(veh, pro ? 2 : 1, playerid) == 0)
	{
		DestroyVehicle(veh);
		return false;
	}
	SetVehicleHealth(veh, float(1000));
	SetVehicleParamsEx(veh, 1, 0, 0, 0, 0, 0, 0);
	gPlayer[playerid][pRentalVehicle] = veh;
	PutPlayerInVehicle(playerid, veh, 0);
	return true;
}

bool:Rental_SendClose(playerid, mode, health, reason)
{
	if (gPlayer[playerid][pRentalId] == 0 || gPlayer[playerid][pRentalPendOp] != 0)
	{
		return false;
	}
	Rental_VehDestroy(playerid);
	g_RtCloseMode[playerid] = mode;
	g_RtCloseHealth[playerid] = LD_Clamp(health, 0, 1000);
	g_RtCloseReason[playerid] = reason;
	g_RtRetry[playerid] = 0;
	g_RtRetryAt[playerid] = 0;
	gPlayer[playerid][pRentalPendOp] = 6;
	DB_LivingMakeOpId(gPlayer[playerid][pRentalOpId], 41, "rc", gPlayer[playerid][pRentalId], gPlayer[playerid][pCharId]);
	DB_LivingRentClose(playerid, gPlayer[playerid][pRentalId], mode, g_RtCloseHealth[playerid], gPlayer[playerid][pRentalOpId]);
	return true;
}

bool:Rental_CloseInterrupted(playerid, reason)
{
	new veh = gPlayer[playerid][pRentalVehicle];
	if (veh > 0 && IsValidVehicle(veh))
	{
		return Rental_SendClose(playerid, 1, Rental_VehHealth(veh), reason);
	}
	return Rental_SendClose(playerid, 3, 0, reason);
}

Rental_NotifyClosed(playerid, result, refund, newState)
{
	new name[48];
	Rental_KindName(gPlayer[playerid][pRentalKind], name, 48);
	new bool:pro = gPlayer[playerid][pRentalClass] == 2;
	new reason = g_RtCloseReason[playerid];
	if (pro)
	{
		if (reason != 100)
		{
			Taxi_OnProRentalClosed(playerid, result, refund);
		}
		return 0;
	}
	if (!Util_IsPlaying(playerid))
	{
		return 0;
	}
	switch (result)
	{
		case 1:
		{
			if (reason == 100)
			{
				Util_Send(playerid, -10270721, "თავისუფალი მანქანა ახლა არ არის: ქირაობა გაუქმდა, თანხა ანგარიშზე დაგიბრუნდა.");
				return 0;
			}
			new minutes = (gettime() - g_RtOpenTs[playerid]) / 60;
			if (g_RtOpenTs[playerid] <= 0)
			{
				minutes = 0;
			}
			if (newState == 3)
			{
				SendClientMessage(playerid, -1, "ქირაობა შეწყდა: გირაო $%d ანგარიშზე დაგიბრუნდა.", refund);
			}
			else
			{
				UI_LivingRentalReceipt(playerid, name, g_RtFee[playerid], g_RtDeposit[playerid], refund, minutes);
			}
		}
		case 5:
		{
			if (refund > 0)
			{
				SendClientMessage(playerid, -1, "ქირაობა დაიხურა დაშვებული დროის შემდეგ: დაგიბრუნდა $%d.", refund);
			}
			else
			{
				Util_Send(playerid, -10270721, "ქირის ვადა და დაშვებული დრო ამოიწურა: მანქანა სააგენტოში დაბრუნდა, გირაო დაკავებულია.");
			}
		}
		case 2:
		{
			Util_Send(playerid, -1, "ქირაობა უკვე დახურული იყო.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "ქირაობის დახურვა ვერ მოხერხდა: შემდეგ შესვლისას ავტომატურად მოგვარდება.");
		}
	}
	return 0;
}

Rental_OnOpenResult(playerid, result, rentalId, amount, dueAt)
{
	if (gPlayer[playerid][pRentalPendOp] != 5)
	{
		if (result == 1)
		{
			Log_Write("RENTAL", "apertura confermata senza richiesta in attesa: lasciata al cleanup del DB");
		}
		return 0;
	}
	gPlayer[playerid][pRentalPendOp] = 0;
	new bool:pro = gPlayer[playerid][pRentalClass] == 2;
	if (result != 1)
	{
		gPlayer[playerid][pRentalClass] = 0;
		gPlayer[playerid][pRentalKind] = 0;
		gPlayer[playerid][pRentalOpId][0] = 0;
		g_RtAbortOpen[playerid] = false;
		if (pro)
		{
			Taxi_OnProRental(playerid, result, 0);
			return 0;
		}
		switch (result)
		{
			case 3:
			{
				Util_Send(playerid, -10270721, "სახსრები არ გყოფნის (ბანკი + ნაღდი) ქირისა და გირაოსთვის.");
			}
			case 7:
			{
				Util_Send(playerid, -10270721, "ახლა ვერ იქირავებ: უკვე გაქვს ქირაობა ან აქტიური კონტრაქტი, ან დონე არ გყოფნის.");
			}
			case 10:
			{
				Util_Send(playerid, -10270721, "ქირაობის სხვა ოპერაცია მიმდინარეობს: სცადე ცოტა ხანში.");
			}
			case 9:
			{
				Util_Send(playerid, -10270721, "ქირაობის არასწორი მოთხოვნა.");
			}
			default:
			{
				Util_Send(playerid, -10270721, "ქირაობის სერვისი დროებით მიუწვდომელია: თანხა არ ჩამოგეჭრა, სცადე ცოტა ხანში.");
			}
		}
		return 0;
	}
	gPlayer[playerid][pRentalId] = rentalId;
	gPlayer[playerid][pRentalDueAt] = dueAt;
	gPlayer[playerid][pRentalWarnFlags] = 0;
	g_RtDeposit[playerid] = LD_RentalDeposit(gPlayer[playerid][pRentalKind]);
	g_RtFee[playerid] = amount > g_RtDeposit[playerid] ? amount - g_RtDeposit[playerid] : 0;
	g_RtOpenTs[playerid] = gettime();
	if (g_RtAbortOpen[playerid] || !Util_IsPlaying(playerid))
	{
		g_RtAbortOpen[playerid] = false;
		Rental_SendClose(playerid, 3, 0, pro ? 3 : 1);
		return 0;
	}
	if (!Rental_SpawnVehicle(playerid))
	{
		Log_Write("RENTAL", "veicolo non creato dopo l'apertura: noleggio annullato con rimborso totale");
		Rental_SendClose(playerid, 5, 0, 100);
		if (pro)
		{
			Taxi_OnProRental(playerid, -1, 0);
		}
		return 0;
	}
	if (pro)
	{
		Taxi_OnProRental(playerid, 1, gPlayer[playerid][pRentalVehicle]);
		return 0;
	}
	new name[24], minutes = (dueAt - gettime() + 60 - 1) / 60;
	if (gPlayer[playerid][pRentalKind] == SRENT_VKIND)
	{
		Veh_StdModelName(GetVehicleModel(gPlayer[playerid][pRentalVehicle]), name);
		SendClientMessage(playerid, 0x66DD66FF, "ქირაობა აქტიურია: %s - %d წუთი ($%d). დაბრუნება ნებისმიერ ადგილას: /riconsegna", name, minutes > 0 ? minutes : 0, g_RtFee[playerid]);
		return 1;
	}
	Rental_KindName(gPlayer[playerid][pRentalKind], name, 16);
	SendClientMessage(playerid, 0x66DD66FF, "ქირაობა აქტიურია: %s %d წუთით (ქირა $%d, გირაო $%d). დააბრუნე აქ: /riconsegna.", name, minutes > 0 ? minutes : 0, g_RtFee[playerid], g_RtDeposit[playerid]);
	return 0;
}

Rental_OnCloseResult(playerid, result, amount, newState)
{
	if (gPlayer[playerid][pRentalPendOp] != 6)
	{
		return 0;
	}
	if (result == -1 || result == 10)
	{
		if (g_RtRetry[playerid] < 3 && IsPlayerConnected(playerid))
		{
			Rental_ScheduleRetry(playerid);
			return 0;
		}
		Log_Write("RENTAL", "chiusura noleggio senza esito dopo i tentativi: la sistema il cleanup del DB (login/avvio)");
	}
	gPlayer[playerid][pRentalPendOp] = 0;
	Rental_NotifyClosed(playerid, result, amount > 0 ? amount : 0, newState);
	Rental_ClearLocal(playerid);
	return 0;
}

public Living_OnDBRental(playerid, action, result, rentalId, amount, balance, cashUsed, dueAt)
{
	if (!Rental_ValidPlayer(playerid) || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	switch (action)
	{
		case 5:
		{
			Rental_OnOpenResult(playerid, result, rentalId, amount, dueAt);
		}
		case 6:
		{
			Rental_OnCloseResult(playerid, result, amount, cashUsed);
		}
	}
	return 1;
}

bool:Rental_AtOffice(playerid, Float:radius)
{
	return GetPlayerInterior(playerid) == 0 && GetPlayerVirtualWorld(playerid) == 0 && IsPlayerInRangeOfPoint(playerid, radius, 1505.0, -1700.0, 13.5);
}

bool:Rental_CanStart(playerid)
{
	if (!Util_IsPlaying(playerid))
	{
		return false;
	}
	if (gPlayer[playerid][pRentalPendOp] != 0)
	{
		Util_Send(playerid, -10270721, "ქირაობის განაცხადი უკვე მუშავდება: მოიცადე.");
		return false;
	}
	new block = LD_CheckStart(playerid);
	if (block != 0)
	{
		LD_SendBlock(playerid, block);
		return false;
	}
	if (!Rental_AtOffice(playerid, 4.0))
	{
		Util_Send(playerid, -10270721, "უნდა იყო Pershing Square-ის ქირაობის სააგენტოში.");
		return false;
	}
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
	{
		Util_Send(playerid, -10270721, "ქირაობისთვის გადმოდი მანქანიდან.");
		return false;
	}
	if (gPlayer[playerid][pLevel] < 1)
	{
		SendClientMessage(playerid, -10270721, "ქირაობისთვის საჭიროა მინიმუმ %d დონე.", 1);
		return false;
	}
	return true;
}

Rental_CmdNoleggio(playerid, const params[])
{
	#pragma unused params
	if (!Rental_CanStart(playerid))
	{
		return 1;
	}
	new rep = Rental_MaxLocalRep(playerid), name[48];
	new bool:room = LD_PoolHasRoom();
	UI_LivingRentalBegin(playerid, 20, 3, LD_DiscountPct(rep));
	for (new k = 1; k <= 3; k++)
	{
		Rental_KindName(k, name, 48);
		UI_LivingRentalAddModel(playerid, k, name, LD_RentalFeeFor(k, 1, rep), LD_RentalDeposit(k), 1, room && gPlayer[playerid][pLevel] >= 1);
	}
	UI_LivingRentalShow(playerid);
	if (!room)
	{
		Util_Send(playerid, -10270721, "თავისუფალი მანქანა ახლა არ არის: სცადე ცოტა ხანში.");
	}
	return 1;
}

public UI_OnRentalConfirm(playerid, modelIdx, blocks)
{
	if (!Rental_CanStart(playerid))
	{
		return 1;
	}
	if (modelIdx < 1 || modelIdx > 3 || blocks < 1 || blocks > 3)
	{
		UI_LivingReject(playerid, "ქირაობის არასწორი არჩევანი.");
		return 1;
	}
	if (!LD_PoolHasRoom())
	{
		UI_LivingReject(playerid, "თავისუფალი მანქანა ახლა არ არის: სცადე ცოტა ხანში.");
		return 1;
	}
	new total = LD_RentalFeeFor(modelIdx, blocks, Rental_MaxLocalRep(playerid)) + LD_RentalDeposit(modelIdx);
	new have = (gPlayer[playerid][pBank] > 0 ? gPlayer[playerid][pBank] : 0) + (gPlayer[playerid][pMoney] > 0 ? gPlayer[playerid][pMoney] : 0);
	if (have < total)
	{
		new msg[480];
		format(msg, 480, "სახსრები არ გყოფნის: საჭიროა $%d (ქირა + გირაო) ბანკსა და ნაღდში.", total);
		UI_LivingReject(playerid, msg);
		return 1;
	}
	if (!Rental_BeginOpen(playerid, modelIdx, blocks, 1))
	{
		UI_LivingReject(playerid, "ქირაობის განაცხადი უკვე მუშავდება: მოიცადე.");
		return 1;
	}
	Util_Send(playerid, -1, "ქირაობის განაცხადი მუშავდება...");
	return 1;
}

bool:Rental_CanReturn(playerid, bool:verbose)
{
	if (!Util_IsPlaying(playerid))
	{
		return false;
	}
	if (gPlayer[playerid][pRentalId] == 0)
	{
		if (verbose)
		{
			Util_Send(playerid, -10270721, "აქტიური ქირაობა არ გაქვს.");
		}
		return false;
	}
	if (gPlayer[playerid][pRentalClass] == 2)
	{
		if (verbose)
		{
			Util_Send(playerid, -10270721, "ტაქსი დეპოში ბრუნდება /taxi off-ით.");
		}
		return false;
	}
	if (gPlayer[playerid][pRentalPendOp] != 0)
	{
		if (verbose)
		{
			Util_Send(playerid, -10270721, "ქირაობის განაცხადი უკვე მუშავდება: მოიცადე.");
		}
		return false;
	}
	new veh = gPlayer[playerid][pRentalVehicle];
	if (veh <= 0 || GetPlayerVehicleID(playerid) != veh || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	{
		if (verbose)
		{
			Util_Send(playerid, -10270721, "ნაქირავები მანქანის საჭესთან უნდა იჯდე.");
		}
		return false;
	}
	if (gPlayer[playerid][pRentalKind] != SRENT_VKIND && !Rental_AtOffice(playerid, 12.0))
	{
		if (verbose)
		{
			Util_Send(playerid, -10270721, "დააბრუნე მანქანა Pershing Square-ის ქირაობის სააგენტოში.");
		}
		return false;
	}
	return true;
}

Rental_CmdRiconsegna(playerid, const params[])
{
	#pragma unused params
	if (!Rental_CanReturn(playerid, true))
	{
		return 1;
	}
	new name[48], health = Rental_VehHealth(gPlayer[playerid][pRentalVehicle]);
	Rental_KindName(gPlayer[playerid][pRentalKind], name, 48);
	new deposit = g_RtDeposit[playerid];
	new refund = LD_RentalRefund(deposit, health);
	if (gPlayer[playerid][pRentalDueAt] > 0 && gettime() > gPlayer[playerid][pRentalDueAt] + 300)
	{
		refund = 0;
	}
	UI_LivingRentalReturnConfirm(playerid, gPlayer[playerid][pRentalId], name, health / 10, deposit, refund);
	return 1;
}

public UI_OnRentalReturnConfirm(playerid, rentalId)
{
	if (rentalId <= 0 || rentalId != gPlayer[playerid][pRentalId])
	{
		UI_LivingReject(playerid, "ეს ქირაობა აღარ არის აქტიური.");
		return 1;
	}
	if (!Rental_CanReturn(playerid, true))
	{
		return 1;
	}
	Rental_SendClose(playerid, 1, Rental_VehHealth(gPlayer[playerid][pRentalVehicle]), 1);
	Util_Send(playerid, -1, "მანქანის დაბრუნება მიმდინარეობს...");
	return 1;
}

Rental_GetVehicle(playerid)
{
	if (!Rental_ValidPlayer(playerid))
	{
		return 0;
	}
	new veh = gPlayer[playerid][pRentalVehicle];
	if (veh <= 0 || !IsValidVehicle(veh))
	{
		return 0;
	}
	return veh;
}

bool:Rental_OpenPro(playerid, vkind)
{
	if (!Rental_ValidPlayer(playerid) || !Util_IsPlaying(playerid) || vkind != 4)
	{
		return false;
	}
	if (!LD_PoolHasRoom())
	{
		return false;
	}
	return Rental_BeginOpen(playerid, vkind, 1, 2);
}

bool:Rental_ClosePro(playerid, reason)
{
	if (!Rental_ValidPlayer(playerid) || gPlayer[playerid][pRentalClass] != 2)
	{
		return false;
	}
	if (gPlayer[playerid][pRentalPendOp] == 5)
	{
		g_RtAbortOpen[playerid] = true;
		return true;
	}
	if (gPlayer[playerid][pRentalId] == 0)
	{
		return false;
	}
	new veh = gPlayer[playerid][pRentalVehicle];
	switch (reason)
	{
		case 1, 4, 5:
		{
			return Rental_SendClose(playerid, 1, Rental_VehHealth(veh), reason);
		}
		case 2, 3:
		{
			return Rental_CloseInterrupted(playerid, reason);
		}
	}
	return Rental_SendClose(playerid, 3, 0, reason);
}

Rental_RetrySend(playerid)
{
	g_RtRetryAt[playerid] = 0;
	if (gPlayer[playerid][pRentalPendOp] != 6 || gPlayer[playerid][pRentalId] == 0)
	{
		return 0;
	}
	DB_LivingRentClose(playerid, gPlayer[playerid][pRentalId], g_RtCloseMode[playerid], g_RtCloseHealth[playerid], gPlayer[playerid][pRentalOpId]);
	return 0;
}

Rental_TickPlayer(playerid, now, nowTick)
{
	if (g_RtRetryAt[playerid] != 0 && nowTick - g_RtRetryAt[playerid] >= 0)
	{
		Rental_RetrySend(playerid);
		return 0;
	}
	if (gPlayer[playerid][pRentalId] == 0 || gPlayer[playerid][pRentalPendOp] != 0)
	{
		return 0;
	}
	new bool:pro = gPlayer[playerid][pRentalClass] == 2;
	new veh = gPlayer[playerid][pRentalVehicle];
	if (veh <= 0 || !IsValidVehicle(veh))
	{
		Log_Write("RENTAL", "veicolo a noleggio scomparso senza OnVehicleDeath: noleggio interrotto (cauzione 100%)");
		if (pro)
		{
			Rental_ClosePro(playerid, 7);
		}
		else
		{
			Rental_SendClose(playerid, 3, 0, 7);
		}
		return 0;
	}
	new due = gPlayer[playerid][pRentalDueAt];
	if (due <= 0)
	{
		return 0;
	}
	new left = due - now;
	if (left <= 300 && !(gPlayer[playerid][pRentalWarnFlags] & 1))
	{
		gPlayer[playerid][pRentalWarnFlags] |= 1;
		SendClientMessage(playerid, -2994689, "შენი ქირის ვადა %d წუთში იწურება.", (left + 60 - 1) / 60);
	}
	if (left <= 60 && !(gPlayer[playerid][pRentalWarnFlags] & 2))
	{
		gPlayer[playerid][pRentalWarnFlags] |= 2;
		Util_Send(playerid, -2994689, pro ? "ტაქსის ქირის ცვლა ერთ წუთში იწურება." : "შენი ქირის ვადა ერთ წუთში იწურება: დააბრუნე სააგენტოში (/riconsegna).");
	}
	if (left <= 0 && !(gPlayer[playerid][pRentalWarnFlags] & 4))
	{
		gPlayer[playerid][pRentalWarnFlags] |= 4;
		if (!pro)
		{
			SendClientMessage(playerid, -10270721, "ქირის ვადა ამოიწურა: დასაბრუნებლად გაქვს %d წუთი, შემდეგ გირაო სააგენტოს რჩება.", 5);
		}
	}
	if (pro)
	{
		if (left <= 0 && (gPlayer[playerid][pTaxiRideId] == 0 || -left >= 150))
		{
			Util_Send(playerid, -1, "ტაქსის ქირის ვადა ამოიწურა: ცვლა სრულდება.");
			Rental_ClosePro(playerid, 5);
		}
		return 0;
	}
	if (-left >= 300)
	{
		Util_Send(playerid, -10270721, "დაშვებული დრო ამოიწურა: მანქანა სააგენტოში ბრუნდება.");
		Rental_SendClose(playerid, 2, 0, 5);
	}
	return 0;
}

public Rental_OnTick()
{
	new now = gettime(), nowTick = GetTickCount();
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i))
		{
			continue;
		}
		if (gPlayer[i][pRentalId] == 0 && g_RtRetryAt[i] == 0)
		{
			continue;
		}
		Rental_TickPlayer(i, now, nowTick);
	}
	return 1;
}

Rental_OnInit()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Rental_ResetPlayer(i);
	}
	if (g_RtTimer != 0)
	{
		KillTimer(g_RtTimer);
	}
	g_RtTimer = SetTimer("Rental_OnTick", 1000, true);
	CreatePickup(1239, 1, 1505.0, -1700.0, 13.5, 0);
	Lang_Create3DTextLabel("მანქანების ქირაობა\n/noleggio  ·  /riconsegna", -2994689, 1505.0, -1700.0, (13.5 + 0.8), 20.0, 0, false);
}

Rental_OnExit()
{
	if (g_RtTimer != 0)
	{
		KillTimer(g_RtTimer);
	}
	g_RtTimer = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (gPlayer[i][pRentalVehicle] != 0)
		{
			Rental_VehDestroy(i);
		}
	}
}

Rental_OnPlayerConnect(playerid)
{
	Rental_ResetPlayer(playerid);
}

Rental_OnCharReady(playerid)
{
	#pragma unused playerid
}

Rental_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	if (!Rental_ValidPlayer(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pRentalPendOp] == 0 && gPlayer[playerid][pRentalId] != 0)
	{
		if (gPlayer[playerid][pRentalClass] == 2)
		{
			Rental_ClosePro(playerid, 2);
		}
		else
		{
			Rental_CloseInterrupted(playerid, 2);
		}
	}
	Rental_VehDestroy(playerid);
	Rental_ResetPlayer(playerid);
	return 0;
}

Rental_OnPlayerDeath(playerid)
{
	if (!Rental_ValidPlayer(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pRentalPendOp] == 5)
	{
		g_RtAbortOpen[playerid] = true;
		return 0;
	}
	if (gPlayer[playerid][pRentalId] == 0 || gPlayer[playerid][pRentalPendOp] != 0)
	{
		return 0;
	}
	if (gPlayer[playerid][pRentalClass] == 2)
	{
		return 0;
	}
	if (Rental_CloseInterrupted(playerid, 3))
	{
		Util_Send(playerid, -1, "ქირაობა შეწყდა: მანქანა სააგენტოში ბრუნდება.");
	}
	return 0;
}

Rental_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	new mod = LD_VehModule(vehicleid);
	if (mod != 1 && mod != 2)
	{
		return 0;
	}
	new owner = LD_VehOwner(vehicleid);
	if (ispassenger != 0 || owner == playerid || owner == 65535)
	{
		return 0;
	}
	ClearAnimations(playerid, SYNC_NONE);
	Util_Send(playerid, -10270721, mod == 2 ? "ეს ტაქსი სამსახურშია: მხოლოდ მგზავრად შეგიძლია ჩაჯდე." : "ეს მანქანა სხვა კლიენტს აქვს ნაქირავები.");
	return 0;
}

Rental_OnPlayerExitVehicle(playerid, vehicleid)
{
	#pragma unused playerid, vehicleid
}

Rental_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate, t_PLAYER_STATE:oldstate)
{
	#pragma unused oldstate
	if (newstate != PLAYER_STATE_DRIVER)
	{
		return 0;
	}
	new veh = GetPlayerVehicleID(playerid), mod = LD_VehModule(veh);
	if (mod != 1 && mod != 2)
	{
		return 0;
	}
	new owner = LD_VehOwner(veh);
	if (owner == playerid)
	{
		return 0;
	}
	RemovePlayerFromVehicle(playerid, false);
	Util_Send(playerid, -10270721, "ამ მანქანის მართვა მხოლოდ დამქირავებელს შეუძლია.");
	return 0;
}

Rental_OnVehicleDeath(vehicleid, killerid)
{
	#pragma unused killerid
	new mod = LD_VehModule(vehicleid);
	if (mod != 1 && mod != 2)
	{
		return 0;
	}
	new owner = LD_VehOwner(vehicleid);
	if (owner == 65535 || !Rental_ValidPlayer(owner) || gPlayer[owner][pRentalVehicle] != vehicleid)
	{
		LD_VehRelease(vehicleid);
		DestroyVehicle(vehicleid);
		return 0;
	}
	if (gPlayer[owner][pRentalClass] == 2)
	{
		Rental_ClosePro(owner, 4);
		return 0;
	}
	if (Rental_SendClose(owner, 1, 0, 4))
	{
		Util_Send(owner, -10270721, "ნაქირავები მანქანა განადგურდა: ზარალს გირაო ფარავს.");
	}
	return 0;
}
