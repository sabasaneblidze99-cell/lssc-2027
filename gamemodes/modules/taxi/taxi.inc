new Float:g_TxNpcX[6] = {1950.0, 1840.0, 2490.0, 2420.0, 2145.0, 2200.0};
new Float:g_TxNpcY[6] = {-1770.0, -1850.0, -1690.0, -1740.0, -1215.0, -1160.0};
new Float:g_TxNpcZ[6] = {13.5, 13.6, 13.5, 13.5, 24.0, 24.0};
new g_TxNpcD[6] = {1, 1, 2, 2, 3, 3};
new g_TxTimer;
new g_TxPhase[MAX_PLAYERS];
new g_TxOfferCall[MAX_PLAYERS];
new g_TxOfferUntil[MAX_PLAYERS];
new g_TxOfferFromPt[MAX_PLAYERS];
new g_TxOfferToPt[MAX_PLAYERS];
new g_TxPickPt[MAX_PLAYERS];
new g_TxDestPt[MAX_PLAYERS];
new g_TxDwellTick[MAX_PLAYERS];
new g_TxOffTick[MAX_PLAYERS];
new g_TxStartHealth[MAX_PLAYERS];
new g_TxStraightM[MAX_PLAYERS];
new g_TxPeerChar[MAX_PLAYERS];
new Float:g_TxMeterF[MAX_PLAYERS];
new Float:g_TxStartX[MAX_PLAYERS];
new Float:g_TxStartY[MAX_PLAYERS];
new bool:g_TxMeterStarted[MAX_PLAYERS];
new g_TxBoardUntil[MAX_PLAYERS];
new g_TxPayPrompts[MAX_PLAYERS];
new g_TxPayMeters[MAX_PLAYERS];
new g_TxPayToD[MAX_PLAYERS];
new g_TxPayFare[MAX_PLAYERS];
new bool:g_TxPayAccept[MAX_PLAYERS];
new g_TxPayRating[MAX_PLAYERS];
new g_TxStartKind[MAX_PLAYERS];
new g_TxStartPeer[MAX_PLAYERS];
new g_TxStartFromD[MAX_PLAYERS];
new g_TxCancelRide[MAX_PLAYERS];
new bool:g_TxAbort[MAX_PLAYERS];
new g_TxCdUntil[MAX_PLAYERS];
new g_TxRetry[MAX_PLAYERS];
new g_TxRetryAt[MAX_PLAYERS];
new g_TxCallUntil[MAX_PLAYERS];
new g_TxRateRide[MAX_PLAYERS];
new g_TxRateStars[MAX_PLAYERS];
new g_TxRateDriverChar[MAX_PLAYERS];

bool:Taxi_Valid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

Taxi_TickAfter(ms)
{
	new t = GetTickCount() + ms;
	return t == 0 ? 1 : t;
}

bool:Taxi_TickReached(tick)
{
	return tick != 0 && GetTickCount() - tick >= 0;
}

Taxi_Dist2D(Float:x1, Float:y1, Float:x2, Float:y2)
{
	new Float:dx = (x2 - x1), Float:dy = (y2 - y1);
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), floatround_method:1);
}

Taxi_MaxLocalRep(playerid)
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

Taxi_VehHealth(vehicleid)
{
	if (vehicleid <= 0 || !IsValidVehicle(vehicleid))
	{
		return 0;
	}
	new Float:hp;
	GetVehicleHealth(vehicleid, hp);
	return floatround(hp, floatround_method:1);
}

Taxi_PassengerOf(driverid)
{
	if (gPlayer[driverid][pTaxiRideKind] != 2)
	{
		return 65535;
	}
	new p = gPlayer[driverid][pTaxiPeer] - 1;
	if (p < 0 || p >= MAX_PLAYERS || !IsPlayerConnected(p) || gPlayer[p][pCharId] != g_TxPeerChar[driverid] || g_TxPeerChar[driverid] <= 0)
	{
		return 65535;
	}
	return p;
}

Taxi_DriverOf(passengerid)
{
	new d = gPlayer[passengerid][pTaxiPeer] - 1;
	if (d < 0 || d >= 1000 || !IsPlayerConnected(d) || gPlayer[d][pTaxiOnDuty] == 0)
	{
		return 65535;
	}
	if (gPlayer[d][pTaxiPeer] - 1 != passengerid || g_TxPeerChar[d] != gPlayer[passengerid][pCharId])
	{
		return 65535;
	}
	return d;
}

bool:Taxi_AtDepot(playerid)
{
	return GetPlayerInterior(playerid) == 0 && GetPlayerVirtualWorld(playerid) == 0 && IsPlayerInRangeOfPoint(playerid, 6.0, 1771.0, -1895.0, 13.6);
}

bool:Taxi_InTaxi(driverid, vehicleid)
{
	return vehicleid > 0 && GetPlayerVehicleID(driverid) == vehicleid && GetPlayerState(driverid) == PLAYER_STATE_DRIVER;
}

Taxi_ResetPlayer(playerid)
{
	g_TxPhase[playerid] = 0;
	g_TxOfferCall[playerid] = 0;
	g_TxOfferUntil[playerid] = 0;
	g_TxOfferFromPt[playerid] = 0;
	g_TxOfferToPt[playerid] = 0;
	g_TxPickPt[playerid] = 0;
	g_TxDestPt[playerid] = 0;
	g_TxDwellTick[playerid] = 0;
	g_TxOffTick[playerid] = 0;
	g_TxStartHealth[playerid] = 0;
	g_TxStraightM[playerid] = 0;
	g_TxPeerChar[playerid] = 0;
	g_TxMeterF[playerid] = 0.0;
	g_TxStartX[playerid] = 0.0;
	g_TxStartY[playerid] = 0.0;
	g_TxMeterStarted[playerid] = false;
	g_TxBoardUntil[playerid] = 0;
	g_TxPayPrompts[playerid] = 0;
	g_TxPayMeters[playerid] = 0;
	g_TxPayToD[playerid] = 0;
	g_TxPayFare[playerid] = 0;
	g_TxPayAccept[playerid] = false;
	g_TxPayRating[playerid] = 0;
	g_TxStartKind[playerid] = 0;
	g_TxStartPeer[playerid] = 65535;
	g_TxStartFromD[playerid] = 0;
	g_TxCancelRide[playerid] = 0;
	g_TxAbort[playerid] = false;
	g_TxCdUntil[playerid] = 0;
	g_TxRetry[playerid] = 0;
	g_TxRetryAt[playerid] = 0;
	g_TxCallUntil[playerid] = 0;
	g_TxRateRide[playerid] = 0;
	g_TxRateStars[playerid] = 0;
	g_TxRateDriverChar[playerid] = 0;
}

Taxi_ScheduleRetry(playerid)
{
	g_TxRetry[playerid]++;
	g_TxRetryAt[playerid] = Taxi_TickAfter(3000);
}

Taxi_ClearOffer(driverid)
{
	if (g_TxOfferCall[driverid] == 0)
	{
		return 0;
	}
	g_TxOfferCall[driverid] = 0;
	g_TxOfferUntil[driverid] = 0;
	UI_LivingTaxiOfferClose(driverid);
	return 0;
}

Taxi_ScheduleNpc(driverid)
{
	new span = 45001;
	new h = LD_Hash32(gPlayer[driverid][pCharId], gPlayer[driverid][pTaxiOpSeq] + 1, LD_Bucket(gettime()) ^ 11);
	gPlayer[driverid][pTaxiNpcOfferAt] = Taxi_TickAfter(LD_HashMod(h, span) + 45000);
}

Taxi_ResetRide(driverid)
{
	new p = Taxi_PassengerOf(driverid);
	if (p != INVALID_PLAYER_ID)
	{
		UI_LivingTaxiMeterHide(p);
		UI_LivingTaxiPayClose(p);
		gPlayer[p][pTaxiPeer] = 0;
	}
	if (IsPlayerConnected(driverid))
	{
		DisablePlayerCheckpoint(driverid);
		UI_LivingTaxiMeterHide(driverid);
	}
	gPlayer[driverid][pTaxiRideId] = 0;
	gPlayer[driverid][pTaxiRideKind] = 0;
	gPlayer[driverid][pTaxiRideState] = 0;
	gPlayer[driverid][pTaxiMeterM] = 0;
	gPlayer[driverid][pTaxiMeterOn] = 0;
	gPlayer[driverid][pTaxiPeer] = 0;
	gPlayer[driverid][pTaxiFromDistrict] = 0;
	gPlayer[driverid][pTaxiToDistrict] = 0;
	g_TxPhase[driverid] = 0;
	g_TxDwellTick[driverid] = 0;
	g_TxOffTick[driverid] = 0;
	g_TxPeerChar[driverid] = 0;
	g_TxMeterF[driverid] = 0.0;
	g_TxMeterStarted[driverid] = false;
	g_TxBoardUntil[driverid] = 0;
	g_TxPayPrompts[driverid] = 0;
	g_TxCdUntil[driverid] = Taxi_TickAfter(10000);
	if (gPlayer[driverid][pTaxiOnDuty] != 0)
	{
		Taxi_ScheduleNpc(driverid);
	}
}

Taxi_CancelRide(driverid, const driverMsg[], const passengerMsg[])
{
	if (g_TxPhase[driverid] == 0 && gPlayer[driverid][pTaxiRideId] == 0)
	{
		return 0;
	}
	if (gPlayer[driverid][pTaxiPendOp] == 7)
	{
		return 0;
	}
	new p = Taxi_PassengerOf(driverid);
	if (p == INVALID_PLAYER_ID && g_TxPhase[driverid] == 1 && g_TxStartKind[driverid] == 2)
	{
		new sp = g_TxStartPeer[driverid];
		if (sp >= 0 && sp < 1000 && IsPlayerConnected(sp) && gPlayer[sp][pTaxiPeer] - 1 == driverid)
		{
			p = sp;
		}
	}
	if (p != INVALID_PLAYER_ID && passengerMsg[0] != 0)
	{
		Util_Send(p, -10270721, passengerMsg);
	}
	if (driverMsg[0] != 0 && IsPlayerConnected(driverid))
	{
		Util_Send(driverid, -10270721, driverMsg);
	}
	if (g_TxPhase[driverid] == 1)
	{
		g_TxAbort[driverid] = true;
		if (p != INVALID_PLAYER_ID)
		{
			gPlayer[p][pTaxiPeer] = 0;
		}
		return 0;
	}
	new ride = gPlayer[driverid][pTaxiRideId];
	if (ride > 0 && gPlayer[driverid][pTaxiPendOp] == 0)
	{
		g_TxCancelRide[driverid] = ride;
		g_TxRetry[driverid] = 0;
		gPlayer[driverid][pTaxiPendOp] = 4;
		DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "tx", ride, gPlayer[driverid][pCharId]);
		DB_LivingTaxiCancel(driverid, ride, gPlayer[driverid][pTaxiOpId]);
	}
	Taxi_ResetRide(driverid);
	return 0;
}

Taxi_EndDuty(driverid, reason, bool:closeRental)
{
	Taxi_CancelRide(driverid, "", "მგზავრობა გაუქმდა: ტაქსი აღარ არის სამსახურში.");
	Taxi_ClearOffer(driverid);
	if (gPlayer[driverid][pTaxiOnDuty] != 0)
	{
		gPlayer[driverid][pTaxiOnDuty] = 0;
		LD_Emit(107, 0, gPlayer[driverid][pCharId], 0, "-");
	}
	gPlayer[driverid][pTaxiNpcOfferAt] = 0;
	if (closeRental)
	{
		Rental_ClosePro(driverid, reason);
	}
}

Taxi_PointsIn(districtId)
{
	new n = 0;
	for (new i = 0; i < 6; i++)
	{
		if (g_TxNpcD[i] == districtId)
		{
			n++;
		}
	}
	return n;
}

Taxi_PickDistrict(h, exclude)
{
	new sum = 0;
	for (new d = 1; d <= 3; d++)
	{
		if (d == exclude || Taxi_PointsIn(d) == 0)
		{
			continue;
		}
		sum += LD_Clamp(District_GetActivity(d), 0, 1000) + 100;
	}
	if (sum <= 0)
	{
		return 0;
	}
	new r = LD_HashMod(h, sum);
	for (new d = 1; d <= 3; d++)
	{
		if (d == exclude || Taxi_PointsIn(d) == 0)
		{
			continue;
		}
		r -= LD_Clamp(District_GetActivity(d), 0, 1000) + 100;
		if (r < 0)
		{
			return d;
		}
	}
	return 0;
}

Taxi_PickPoint(h, districtId)
{
	new n = Taxi_PointsIn(districtId);
	if (n <= 0)
	{
		return -1;
	}
	new k = LD_HashMod(h, n);
	for (new i = 0; i < 6; i++)
	{
		if (g_TxNpcD[i] != districtId)
		{
			continue;
		}
		if (k == 0)
		{
			return i;
		}
		k--;
	}
	return -1;
}

Taxi_NpcOffer(driverid)
{
	new charId = gPlayer[driverid][pCharId];
	new seq = ++gPlayer[driverid][pTaxiOpSeq];
	new h = LD_Hash32(charId, seq, LD_Bucket(gettime()));
	new fromD = Taxi_PickDistrict(h, 0);
	new h2 = LD_Hash32(h, seq, 12);
	new toD = Taxi_PickDistrict(h2, fromD);
	new fromPt = Taxi_PickPoint(h >>> 8, fromD), toPt = Taxi_PickPoint(h2 >>> 8, toD);
	Taxi_ScheduleNpc(driverid);
	if (fromPt < 0 || toPt < 0)
	{
		return 0;
	}
	new straight = Taxi_Dist2D(g_TxNpcX[fromPt], g_TxNpcY[fromPt], g_TxNpcX[toPt], g_TxNpcY[toPt]);
	new est = LD_TaxiNpcPay(LD_TaxiFare(straight), LD_GetProPoints(driverid, 2));
	g_TxOfferCall[driverid] = (seq & 65535) + 1001;
	g_TxOfferUntil[driverid] = Taxi_TickAfter(30000);
	g_TxOfferFromPt[driverid] = fromPt;
	g_TxOfferToPt[driverid] = toPt;
	UI_LivingTaxiOffer(driverid, g_TxOfferCall[driverid], true, "კლიენტი", fromD, toD, straight, est, 30);
	new a[48], b[48];
	LD_DistrictName(fromD, a, 48);
	LD_DistrictName(toD, b, 48);
	SendClientMessage(driverid, -2994689, "[Taxi] გამოძახება: კლიენტი %s-ში, მიემართება %s-ში (~%s). /accettacorsa %d წამში.", a, b, Money_Str(est), 30);
	return 0;
}

bool:Taxi_CanAccept(driverid, bool:verbose)
{
	if (gPlayer[driverid][pTaxiOnDuty] == 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "ტაქსისტად სამსახურში არ ხარ (/taxi on Unity-ს დეპოში).");
		}
		return false;
	}
	if (g_TxPhase[driverid] != 0 || gPlayer[driverid][pTaxiRideId] != 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "უკვე გაქვს მიმდინარე მგზავრობა.");
		}
		return false;
	}
	if (gPlayer[driverid][pTaxiPendOp] != 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "ტაქსის ოპერაცია მიმდინარეობს: მოიცადე.");
		}
		return false;
	}
	if (Rental_GetVehicle(driverid) == 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "შენი ტაქსი მიუწვდომელია.");
		}
		return false;
	}
	if (g_TxCdUntil[driverid] != 0 && !Taxi_TickReached(g_TxCdUntil[driverid]))
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "ახალი მგზავრობის მიღებამდე რამდენიმე წამი მოიცადე.");
		}
		return false;
	}
	return true;
}

Taxi_SendStart(driverid)
{
	DB_LivingTaxiStart(driverid, g_TxStartKind[driverid], g_TxStartPeer[driverid], g_TxStartFromD[driverid], gPlayer[driverid][pTaxiOpId]);
}

Taxi_AcceptNpc(driverid)
{
	if (!Taxi_CanAccept(driverid, true))
	{
		return 0;
	}
	if (g_TxOfferCall[driverid] < 1001 || Taxi_TickReached(g_TxOfferUntil[driverid]))
	{
		Util_Send(driverid, -10270721, "მისაღები გამოძახება არ არის (ან ვადა გაუვიდა).");
		return 0;
	}
	new fromPt = g_TxOfferFromPt[driverid], toPt = g_TxOfferToPt[driverid];
	Taxi_ClearOffer(driverid);
	gPlayer[driverid][pTaxiRideKind] = 1;
	gPlayer[driverid][pTaxiFromDistrict] = g_TxNpcD[fromPt];
	gPlayer[driverid][pTaxiToDistrict] = g_TxNpcD[toPt];
	g_TxPickPt[driverid] = fromPt;
	g_TxDestPt[driverid] = toPt;
	g_TxStraightM[driverid] = Taxi_Dist2D(g_TxNpcX[fromPt], g_TxNpcY[fromPt], g_TxNpcX[toPt], g_TxNpcY[toPt]);
	g_TxPhase[driverid] = 1;
	g_TxAbort[driverid] = false;
	g_TxStartKind[driverid] = 1;
	g_TxStartPeer[driverid] = 65535;
	g_TxStartFromD[driverid] = g_TxNpcD[fromPt];
	g_TxRetry[driverid] = 0;
	gPlayer[driverid][pTaxiPendOp] = 1;
	DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "ts", gPlayer[driverid][pTaxiOpSeq], gPlayer[driverid][pCharId]);
	Taxi_SendStart(driverid);
	return 0;
}

Taxi_AcceptPlayer(driverid, passengerid)
{
	if (!Taxi_CanAccept(driverid, true))
	{
		return 0;
	}
	if (!Taxi_Valid(passengerid) || passengerid == driverid || !Util_IsPlaying(passengerid) || g_TxCallUntil[passengerid] == 0 || Taxi_TickReached(g_TxCallUntil[passengerid]))
	{
		Util_Send(driverid, -10270721, "ეს გამოძახება აღარ არის აქტიური.");
		return 0;
	}
	if (gPlayer[passengerid][pTaxiPeer] != 0)
	{
		Util_Send(driverid, -10270721, "ეს გამოძახება სხვა ტაქსიმ უკვე აიღო.");
		return 0;
	}
	if (!Util_InRange(driverid, passengerid, 600.0))
	{
		Util_Send(driverid, -10270721, "კლიენტი ძალიან შორსაა.");
		return 0;
	}
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(passengerid, px, py, pz);
	g_TxCallUntil[passengerid] = 0;
	gPlayer[passengerid][pTaxiPeer] = driverid + 1;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (i != driverid && g_TxOfferCall[i] == passengerid + 1)
		{
			Taxi_ClearOffer(i);
		}
	}
	Taxi_ClearOffer(driverid);
	gPlayer[driverid][pTaxiRideKind] = 2;
	gPlayer[driverid][pTaxiFromDistrict] = District_At(px, py);
	gPlayer[driverid][pTaxiToDistrict] = 0;
	gPlayer[driverid][pTaxiPeer] = passengerid + 1;
	g_TxPeerChar[driverid] = gPlayer[passengerid][pCharId];
	g_TxPhase[driverid] = 1;
	g_TxAbort[driverid] = false;
	g_TxStartKind[driverid] = 2;
	g_TxStartPeer[driverid] = passengerid;
	g_TxStartFromD[driverid] = gPlayer[driverid][pTaxiFromDistrict];
	g_TxRetry[driverid] = 0;
	gPlayer[driverid][pTaxiPendOp] = 1;
	DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "ts", gPlayer[passengerid][pCharId], gPlayer[driverid][pCharId]);
	Taxi_SendStart(driverid);
	return 0;
}

Taxi_OnStartResult(driverid, result, rideId)
{
	if (gPlayer[driverid][pTaxiPendOp] != 1)
	{
		return 0;
	}
	if ((result == -1 || result == 10) && g_TxRetry[driverid] < 3)
	{
		Taxi_ScheduleRetry(driverid);
		return 0;
	}
	gPlayer[driverid][pTaxiPendOp] = 0;
	g_TxRetry[driverid] = 0;
	g_TxRetryAt[driverid] = 0;
	new bool:npc = gPlayer[driverid][pTaxiRideKind] == 1;
	new p = Taxi_PassengerOf(driverid);
	if (result == 1 && rideId > 0)
	{
		gPlayer[driverid][pTaxiRideId] = rideId;
		gPlayer[driverid][pTaxiRideState] = 1;
		g_TxPhase[driverid] = 2;
		g_TxDwellTick[driverid] = 0;
		g_TxOffTick[driverid] = 0;
		if (g_TxAbort[driverid] || gPlayer[driverid][pTaxiOnDuty] == 0 || (!npc && p == INVALID_PLAYER_ID))
		{
			g_TxAbort[driverid] = false;
			Taxi_CancelRide(driverid, "", "");
			return 0;
		}
		if (npc)
		{
			new pt = g_TxPickPt[driverid];
			SetPlayerCheckpoint(driverid, g_TxNpcX[pt], g_TxNpcY[pt], g_TxNpcZ[pt], 6.0);
			Util_Send(driverid, 0x66DD66FF, "მგზავრობა მიღებულია: მიდი კლიენტთან (ჩეკპოინტი) და რამდენიმე წამით გაჩერდი, რომ ჩაჯდეს.");
			return 0;
		}
		new name[75], Float:px, Float:py, Float:pz;
		GetPlayerName(driverid, name, 75);
		GetPlayerPos(p, px, py, pz);
		SetPlayerCheckpoint(driverid, px, py, pz, 6.0);
		g_TxBoardUntil[driverid] = Taxi_TickAfter(180000);
		SendClientMessage(p, 0x66DD66FF, "%s-ის ტაქსიმ მიიღო შენი გამოძახება: დაელოდე და ჩაჯექი მგზავრად.", name);
		Util_Send(driverid, 0x66DD66FF, "მგზავრობა მიღებულია: მიდი კლიენტთან (ჩეკპოინტი). როცა ჩაჯდება: /tassametro start.");
		return 0;
	}
	g_TxAbort[driverid] = false;
	if (p != INVALID_PLAYER_ID)
	{
		Util_Send(p, -10270721, "ტაქსიმ მგზავრობა ვერ მიიღო: შეგიძლია სხვა გამოიძახო.");
	}
	switch (result)
	{
		case 6:
		{
			Util_Send(driverid, -10270721, "ამ საათში ქალაქის კლიენტებთან მგზავრობების მაქსიმუმს მიაღწიე.");
			Taxi_ResetRide(driverid);
			gPlayer[driverid][pTaxiNpcOfferAt] = Taxi_TickAfter(300000);
			return 0;
		}
		case 7:
		{
			Util_Send(driverid, -10270721, npc ? "ტაქსის სერვისის მოთხოვნები არ სრულდება." : "ამ კლიენტთან მგზავრობების საათობრივი ლიმიტი ამოიწურა (ან მოთხოვნები არ სრულდება).");
		}
		case 4:
		{
			Util_Send(driverid, -10270721, "ღია მგზავრობა უკვე არსებობს: დაელოდე მის დახურვას.");
		}
		default:
		{
			Util_Send(driverid, -10270721, "ტაქსის დისპეჩერი მიუწვდომელია: მგზავრობა არ დაიწყო.");
		}
	}
	Taxi_ResetRide(driverid);
	return 0;
}

Taxi_MeterSample(driverid, vehicleid, bool:count)
{
	new Float:x, Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);
	if (count)
	{
		new Float:dx = (x - gPlayer[driverid][pTaxiLastX]), Float:dy = (y - gPlayer[driverid][pTaxiLastY]);
		new Float:step = floatsqroot(((dx * dx) + (dy * dy)));
		if ((step <= 70.0))
		{
			g_TxMeterF[driverid] += step;
		}
		gPlayer[driverid][pTaxiMeterM] = floatround(g_TxMeterF[driverid], floatround_method:1);
	}
	gPlayer[driverid][pTaxiLastX] = x;
	gPlayer[driverid][pTaxiLastY] = y;
}

Taxi_MeterShow(driverid, bool:running)
{
	new meters = gPlayer[driverid][pTaxiMeterM], p = Taxi_PassengerOf(driverid);
	if (gPlayer[driverid][pTaxiRideKind] == 1)
	{
		UI_LivingTaxiMeter(driverid, LD_TaxiFare(meters), meters, running);
		return 0;
	}
	new fare = p != INVALID_PLAYER_ID ? LD_TaxiFareFor(meters, Taxi_MaxLocalRep(p)) : LD_TaxiFare(meters);
	UI_LivingTaxiMeter(driverid, fare, meters, running);
	if (p != INVALID_PLAYER_ID)
	{
		UI_LivingTaxiMeter(p, fare, meters, running);
	}
	return 0;
}

Taxi_MeterBegin(driverid, vehicleid)
{
	Taxi_MeterSample(driverid, vehicleid, false);
	if (!g_TxMeterStarted[driverid])
	{
		g_TxMeterStarted[driverid] = true;
		g_TxStartX[driverid] = gPlayer[driverid][pTaxiLastX];
		g_TxStartY[driverid] = gPlayer[driverid][pTaxiLastY];
	}
	gPlayer[driverid][pTaxiMeterOn] = 1;
	Taxi_MeterShow(driverid, true);
}

Taxi_NpcPay(driverid, vehicleid)
{
	gPlayer[driverid][pTaxiMeterOn] = 0;
	new maxM = g_TxStraightM[driverid] * 250 / 100 + 300;
	new meters = gPlayer[driverid][pTaxiMeterM];
	if (meters > maxM)
	{
		meters = maxM;
	}
	new now = Taxi_VehHealth(vehicleid), start = g_TxStartHealth[driverid], rating = 0;
	if (start > 0 && (start - now) * 100 / start < 10)
	{
		rating = 4;
	}
	g_TxPayMeters[driverid] = meters;
	g_TxPayToD[driverid] = g_TxNpcD[g_TxDestPt[driverid]];
	g_TxPayAccept[driverid] = true;
	g_TxPayRating[driverid] = rating;
	g_TxPayFare[driverid] = LD_TaxiFare(meters);
	g_TxPhase[driverid] = 5;
	g_TxRetry[driverid] = 0;
	gPlayer[driverid][pTaxiToDistrict] = g_TxPayToD[driverid];
	gPlayer[driverid][pTaxiPendOp] = 7;
	DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "tp", gPlayer[driverid][pTaxiRideId], gPlayer[driverid][pCharId]);
	DisablePlayerCheckpoint(driverid);
	Taxi_MeterShow(driverid, false);
	DB_LivingTaxiPay(driverid, gPlayer[driverid][pTaxiRideId], 65535, meters, g_TxPayToD[driverid], true, rating, gPlayer[driverid][pTaxiOpId]);
}

Taxi_PayPrompt(driverid)
{
	new p = Taxi_PassengerOf(driverid), veh = Rental_GetVehicle(driverid);
	if (p == INVALID_PLAYER_ID || veh == 0)
	{
		Taxi_CancelRide(driverid, "მგზავრობა გაუქმდა: კლიენტი ან ტაქსი აღარ არის ხელმისაწვდომი.", "");
		return 0;
	}
	new Float:x, Float:y, Float:z;
	GetVehiclePos(veh, x, y, z);
	new meters = gPlayer[driverid][pTaxiMeterM];
	if (meters < 200 || Taxi_Dist2D(g_TxStartX[driverid], g_TxStartY[driverid], x, y) < 150)
	{
		Taxi_CancelRide(driverid, "მგზავრობა ძალიან მოკლეა: გაუქმდა თანხის ჩამოჭრის გარეშე.", "მგზავრობა ძალიან მოკლეა: გაუქმდა თანხის ჩამოჭრის გარეშე.");
		return 0;
	}
	new rep = Taxi_MaxLocalRep(p), name[75];
	GetPlayerName(driverid, name, 75);
	g_TxPayMeters[driverid] = meters;
	g_TxPayToD[driverid] = District_At(x, y);
	g_TxPayFare[driverid] = LD_TaxiFareFor(meters, rep);
	g_TxPayRating[driverid] = 0;
	gPlayer[driverid][pTaxiToDistrict] = g_TxPayToD[driverid];
	g_TxPhase[driverid] = 4;
	Taxi_MeterShow(driverid, false);
	UI_LivingTaxiPayConfirm(p, gPlayer[driverid][pTaxiRideId], name, meters, g_TxPayFare[driverid], LD_DiscountPct(rep), 60);
	SendClientMessage(driverid, -1, "ტაქსომეტრი გაჩერდა: %s %d მ-ზე. ველოდებით კლიენტის დადასტურებას.", Money_Str(g_TxPayFare[driverid]), meters);
	return 0;
}

Taxi_MeterStop(driverid)
{
	if (gPlayer[driverid][pTaxiMeterOn] == 0)
	{
		return 0;
	}
	new veh = Rental_GetVehicle(driverid);
	if (veh != 0)
	{
		Taxi_MeterSample(driverid, veh, Taxi_InTaxi(driverid, veh));
	}
	gPlayer[driverid][pTaxiMeterOn] = 0;
	Taxi_PayPrompt(driverid);
	return 0;
}

Taxi_OnPayResult(driverid, result, rideId, net, points, tax)
{
	if (gPlayer[driverid][pTaxiPendOp] != 7)
	{
		return 0;
	}
	if ((result == -1 || result == 10) && g_TxRetry[driverid] < 3)
	{
		Taxi_ScheduleRetry(driverid);
		return 0;
	}
	gPlayer[driverid][pTaxiPendOp] = 0;
	g_TxRetry[driverid] = 0;
	g_TxRetryAt[driverid] = 0;
	new bool:npc = gPlayer[driverid][pTaxiRideKind] == 1;
	new p = Taxi_PassengerOf(driverid), other[225];
	if (npc || p == INVALID_PLAYER_ID)
	{
		format(other, 225, "%s", "კლიენტი");
	}
	else
	{
		GetPlayerName(p, other, 25);
	}
	switch (result)
	{
		case 1, 6:
		{
			gPlayer[driverid][pTaxiRideState] = 2;
			if (points > 0)
			{
				LD_SetProPoints(driverid, 2, points);
			}
			UI_LivingTaxiReceipt(driverid, true, rideId, other, g_TxPayMeters[driverid], npc ? net : net + tax, tax, net);
			if (result == 6)
			{
				Util_Send(driverid, -2994689, "დინამიური სამსახურის საათობრივი ლიმიტი ამოიწურა: გადაიხადა მხოლოდ ნაშთი.");
			}
			LD_Emit(3, g_TxPayToD[driverid], gPlayer[driverid][pCharId], net, gPlayer[driverid][pTaxiOpId]);
		}
		case 8:
		{
			gPlayer[driverid][pTaxiRideState] = 3;
			Util_Send(driverid, -10270721, "კლიენტმა მგზავრობა გაასაჩივრა: გადახდა არ მოხდა, შეამოწმებს ადმინისტრაცია.");
		}
		case 3:
		{
			Taxi_CancelRide(driverid, "კლიენტს ბანკში საკმარისი თანხა არ აქვს: მგზავრობა გაუქმდა.", "");
			return 0;
		}
		case 2:
		{
			Util_Send(driverid, -1, "მგზავრობა უკვე დახურული იყო.");
		}
		case -1, 10:
		{
			Util_Send(driverid, -10270721, "დისპეჩერმა გადახდა არ დაადასტურა: მგზავრობა შემოწმდება.");
		}
		default:
		{
			Util_Send(driverid, -10270721, "მგზავრობა აღარ არის მოქმედი: დაიხურა გადახდის გარეშე.");
		}
	}
	Taxi_ResetRide(driverid);
	return 0;
}

Taxi_RetrySend(playerid)
{
	g_TxRetryAt[playerid] = 0;
	switch (gPlayer[playerid][pTaxiPendOp])
	{
		case 1:
		{
			new sp = g_TxStartPeer[playerid];
			if (g_TxStartKind[playerid] == 2 && (sp < 0 || sp >= 1000 || !IsPlayerConnected(sp)))
			{
				g_TxStartPeer[playerid] = 65535;
			}
			Taxi_SendStart(playerid);
		}
		case 7:
		{
			new p = Taxi_PassengerOf(playerid);
			DB_LivingTaxiPay(playerid, gPlayer[playerid][pTaxiRideId], p, g_TxPayMeters[playerid], g_TxPayToD[playerid], g_TxPayAccept[playerid], g_TxPayRating[playerid], gPlayer[playerid][pTaxiOpId]);
		}
		case 4:
		{
			DB_LivingTaxiCancel(playerid, g_TxCancelRide[playerid], gPlayer[playerid][pTaxiOpId]);
		}
		case 8:
		{
			DB_LivingTaxiRate(playerid, g_TxRateRide[playerid], g_TxRateStars[playerid], gPlayer[playerid][pTaxiOpId]);
		}
	}
}

public Living_OnDBTaxi(playerid, action, result, rideId, amount, balance, v1, v2)
{
	if (!Taxi_Valid(playerid) || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	switch (action)
	{
		case 7:
		{
			Taxi_OnStartResult(playerid, result, rideId);
		}
		case 8:
		{
			Taxi_OnPayResult(playerid, result, rideId, amount, v1, v2);
		}
		case 9:
		{
			if (gPlayer[playerid][pTaxiPendOp] != 4)
			{
				return 1;
			}
			if ((result == -1 || result == 10) && g_TxRetry[playerid] < 3)
			{
				Taxi_ScheduleRetry(playerid);
				return 1;
			}
			if (result == -1 || result == 10)
			{
				Log_Write("TAXI", "annullo corsa senza esito: la chiude il cleanup del DB (logout/avvio)");
			}
			gPlayer[playerid][pTaxiPendOp] = 0;
			g_TxCancelRide[playerid] = 0;
			g_TxRetry[playerid] = 0;
			g_TxRetryAt[playerid] = 0;
		}
		case 10:
		{
			if (gPlayer[playerid][pTaxiPendOp] != 8)
			{
				return 1;
			}
			if ((result == -1 || result == 10) && g_TxRetry[playerid] < 3)
			{
				Taxi_ScheduleRetry(playerid);
				return 1;
			}
			gPlayer[playerid][pTaxiPendOp] = 0;
			g_TxRetry[playerid] = 0;
			g_TxRetryAt[playerid] = 0;
			if (result == 1)
			{
				Util_Send(playerid, 0x66DD66FF, "მადლობა შეფასებისთვის.");
				new d = DB_LdCharOnline(g_TxRateDriverChar[playerid]);
				if (d != 65535 && v2 > 0)
				{
					SendClientMessage(d, -1, "[Taxi] ახალი შეფასება: %d ვარსკვლავი (საშუალო %d.%02d).", v1, v2 / 100, v2 % 100);
				}
			}
			else if (result == 2)
			{
				Util_Send(playerid, -1, "ეს მგზავრობა უკვე შეაფასე.");
			}
			g_TxRateRide[playerid] = 0;
			g_TxRateStars[playerid] = 0;
			g_TxRateDriverChar[playerid] = 0;
		}
	}
	return 1;
}

public Living_OnDBTaxiCharged(playerid, rideId, result, charged, balance)
{
	if (!Taxi_Valid(playerid) || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new d = Taxi_DriverOf(playerid);
	switch (result)
	{
		case 1, 6:
		{
			new name[75], meters = 0;
			name[0] = 0;
			if (d != 65535)
			{
				GetPlayerName(d, name, 75);
				meters = g_TxPayMeters[d];
				g_TxRateDriverChar[playerid] = gPlayer[d][pCharId];
			}
			UI_LivingTaxiMeterHide(playerid);
			UI_LivingTaxiReceipt(playerid, false, rideId, name, meters, charged, 0, 0);
			if (g_TxRateDriverChar[playerid] > 0 && gPlayer[playerid][pTaxiPendOp] == 0)
			{
				g_TxRateRide[playerid] = rideId;
				UI_LivingTaxiRate(playerid, rideId, name);
			}
		}
		case 8:
		{
			Util_Send(playerid, -1, "მგზავრობა გასაჩივრდა: თანხა არ ჩამოგეჭრა, შეამოწმებს ადმინისტრაცია.");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, "ბანკში თანხა არ გყოფნის მგზავრობის გადასახდელად: მგზავრობა გაუქმდა.");
		}
	}
	return 1;
}

public UI_OnTaxiCallAccept(playerid, callId)
{
	if (callId <= 0)
	{
		return 1;
	}
	if (callId <= 1000)
	{
		Taxi_AcceptPlayer(playerid, callId - 1);
	}
	else if (callId == g_TxOfferCall[playerid])
	{
		Taxi_AcceptNpc(playerid);
	}
	else
	{
		Util_Send(playerid, -10270721, "ეს გამოძახება აღარ არის აქტიური.");
	}
	return 1;
}

public UI_OnTaxiPayConfirm(playerid, rideId, accept)
{
	new d = Taxi_DriverOf(playerid);
	if (d == 65535 || rideId <= 0 || gPlayer[d][pTaxiRideId] != rideId || g_TxPhase[d] != 4 || gPlayer[d][pTaxiPendOp] != 0)
	{
		UI_LivingReject(playerid, "ეს მგზავრობა აღარ ელოდება გადახდას.");
		return 1;
	}
	g_TxPhase[d] = 5;
	g_TxPayAccept[d] = accept != 0;
	g_TxRetry[d] = 0;
	gPlayer[d][pTaxiPendOp] = 7;
	DB_LivingMakeOpId(gPlayer[d][pTaxiOpId], 41, "tp", rideId, gPlayer[d][pCharId]);
	Util_Send(playerid, -1, accept != 0 ? "გადახდა მიმდინარეობს..." : "საჩივარი დარეგისტრირდა: მგზავრობას ადმინისტრაცია განიხილავს.");
	DB_LivingTaxiPay(d, rideId, playerid, g_TxPayMeters[d], g_TxPayToD[d], g_TxPayAccept[d], 0, gPlayer[d][pTaxiOpId]);
	return 1;
}

public UI_OnTaxiPayExpired(playerid, rideId)
{
	new d = Taxi_DriverOf(playerid);
	if (d == 65535 || gPlayer[d][pTaxiRideId] != rideId || g_TxPhase[d] != 4)
	{
		return 1;
	}
	g_TxPayPrompts[d]++;
	if (g_TxPayPrompts[d] >= 2)
	{
		Taxi_CancelRide(d, "კლიენტმა გადახდა არ დაადასტურა: მგზავრობა გაუქმდა.", "გადახდა არ დაადასტურე: მგზავრობა გაუქმდა.");
		return 1;
	}
	g_TxPhase[d] = 3;
	Util_Send(d, -2994689, "კლიენტმა არ უპასუხა: /tassametro stop — გადახდის ხელახლა მოთხოვნა, /tassametro annulla — დახურვა.");
	return 1;
}

public UI_OnTaxiRate(playerid, rideId, stars)
{
	if (rideId <= 0 || rideId != g_TxRateRide[playerid] || stars < 1 || stars > 5 || gPlayer[playerid][pTaxiPendOp] != 0)
	{
		return 1;
	}
	g_TxRateStars[playerid] = stars;
	g_TxRetry[playerid] = 0;
	gPlayer[playerid][pTaxiPendOp] = 8;
	DB_LivingMakeOpId(gPlayer[playerid][pTaxiOpId], 41, "tr", rideId, gPlayer[playerid][pCharId]);
	DB_LivingTaxiRate(playerid, rideId, stars, gPlayer[playerid][pTaxiOpId]);
	return 1;
}

Taxi_CmdTaxi(playerid, const params[])
{
	new pos = 0, tok[48];
	Util_NextToken(params, pos, tok, 48);
	if (!strcmp(tok, "on", true, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (gPlayer[playerid][pTaxiOnDuty] != 0)
		{
			Util_Send(playerid, -10270721, "უკვე სამსახურში ხარ.");
			return 1;
		}
		if (gPlayer[playerid][pRentalPendOp] != 0 || gPlayer[playerid][pTaxiPendOp] != 0)
		{
			Util_Send(playerid, -10270721, "განაცხადი მუშავდება: მოიცადე.");
			return 1;
		}
		new block = LD_CheckStart(playerid);
		if (block != 0)
		{
			LD_SendBlock(playerid, block);
			return 1;
		}
		if (gPlayer[playerid][pLevel] < 2)
		{
			SendClientMessage(playerid, -10270721, "ტაქსის სამართავად საჭიროა მინიმუმ %d დონე.", 2);
			return 1;
		}
		if (!Taxi_AtDepot(playerid) || GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
		{
			Util_Send(playerid, -10270721, "მიდი ფეხით Unity Station-ის ტაქსის დეპოში.");
			return 1;
		}
		new total = ECON_TAXI_RENT_HOUR + ECON_TAXI_DEPOSIT;
		if ((gPlayer[playerid][pBank] > 0 ? gPlayer[playerid][pBank] : 0) + (gPlayer[playerid][pMoney] > 0 ? gPlayer[playerid][pMoney] : 0) < total)
		{
			SendClientMessage(playerid, -10270721, "საჭიროა %s (ქირა %s/სთ + გირაო %s) ბანკსა და ნაღდში.", Money_Str(total), Money_Str(ECON_TAXI_RENT_HOUR), Money_Str(ECON_TAXI_DEPOSIT));
			return 1;
		}
		if (!LD_PoolHasRoom() || !Rental_OpenPro(playerid, 4))
		{
			Util_Send(playerid, -10270721, "თავისუფალი ტაქსი ახლა არ არის: სცადე ცოტა ხანში.");
			return 1;
		}
		Util_Send(playerid, -1, "ტაქსის განაცხადი მუშავდება...");
		return 1;
	}
	if (!strcmp(tok, "off", true, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (gPlayer[playerid][pTaxiOnDuty] == 0)
		{
			Util_Send(playerid, -10270721, "სამსახურში არ ხარ.");
			return 1;
		}
		if (g_TxPhase[playerid] != 0 || gPlayer[playerid][pTaxiRideId] != 0 || gPlayer[playerid][pTaxiPendOp] != 0)
		{
			Util_Send(playerid, -10270721, "ჯერ დაასრულე ან გააუქმე მიმდინარე მგზავრობა (/tassametro annulla).");
			return 1;
		}
		if (Rental_GetVehicle(playerid) != 0 && !Taxi_AtDepot(playerid))
		{
			Util_Send(playerid, -10270721, "ცვლის დასახურად დააბრუნე ტაქსი Unity Station-ის დეპოში.");
			return 1;
		}
		Taxi_EndDuty(playerid, 1, true);
		Util_Send(playerid, -1, "ცვლის დასასრული: ტაქსის დაბრუნება მიმდინარეობს...");
		return 1;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0)
	{
		SendClientMessage(playerid, -1, "სამსახურში ხარ. ტაქსის ქულები: %d. ბრძანებები: /accettacorsa [id] · /tassametro start|stop|annulla · /taxi off (დეპოში).", LD_GetProPoints(playerid, 2));
	}
	else
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /taxi on|off Unity Station-ის დეპოში (საათობრივი ქირა + გირაო).");
	}
	return 1;
}

Taxi_CmdChiamataxi(playerid, const params[])
{
	#pragma unused params
	if (!Util_IsPlaying(playerid) || gPlayer[playerid][pLivingReady] == 0)
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0)
	{
		Util_Send(playerid, -10270721, "ტაქსისტად სამსახურში ხარ.");
		return 1;
	}
	if (gPlayer[playerid][pTaxiPeer] != 0)
	{
		Util_Send(playerid, -10270721, "შენი გამოძახება ტაქსიმ უკვე მიიღო.");
		return 1;
	}
	if (gPlayer[playerid][pTaxiCallTick] != 0 && GetTickCount() - gPlayer[playerid][pTaxiCallTick] < 60000)
	{
		SendClientMessage(playerid, -10270721, "ტაქსი უკვე გამოიძახე: მოიცადე %d წამი.", (60000 - (GetTickCount() - gPlayer[playerid][pTaxiCallTick])) / 1000 + 1);
		return 1;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Util_Send(playerid, -10270721, "ტაქსის გამოსაძახებლად გარეთ უნდა იყო.");
		return 1;
	}
	gPlayer[playerid][pTaxiCallTick] = GetTickCount();
	if (gPlayer[playerid][pTaxiCallTick] == 0)
	{
		gPlayer[playerid][pTaxiCallTick] = 1;
	}
	g_TxCallUntil[playerid] = Taxi_TickAfter(60000);
	new name[75], Float:px, Float:py, Float:pz, sent = 0, area[48];
	GetPlayerName(playerid, name, 75);
	GetPlayerPos(playerid, px, py, pz);
	new fromD = District_At(px, py);
	LD_DistrictName(fromD, area, 48);
	for (new d = 0; d < 1000; d++)
	{
		if (d == playerid || !IsPlayerConnected(d) || gPlayer[d][pTaxiOnDuty] == 0)
		{
			continue;
		}
		if (g_TxPhase[d] != 0 || gPlayer[d][pTaxiRideId] != 0 || gPlayer[d][pTaxiPendOp] != 0)
		{
			continue;
		}
		if (!Util_InRange(d, playerid, 600.0))
		{
			continue;
		}
		new Float:dx, Float:dy, Float:dz;
		GetPlayerPos(d, dx, dy, dz);
		SendClientMessage(d, -2994689, "[Taxi] %s (id %d) ტაქსს იძახებს: %s. /accettacorsa %d", name, playerid, area, playerid);
		if (g_TxOfferCall[d] == 0)
		{
			g_TxOfferCall[d] = playerid + 1;
			g_TxOfferUntil[d] = Taxi_TickAfter(30000);
			UI_LivingTaxiOffer(d, playerid + 1, false, name, fromD, fromD, Taxi_Dist2D(dx, dy, px, py), LD_TaxiFare(0), 30);
		}
		sent++;
	}
	if (sent > 0)
	{
		SendClientMessage(playerid, 0x66DD66FF, "გამოძახება გაეგზავნა ზონაში %d ტაქსს: დაელოდე პასუხს.", sent);
	}
	else
	{
		Util_Send(playerid, -10270721, "ახლოს სამსახურში მყოფი ტაქსი არ არის. სცადე მოგვიანებით.");
	}
	return 1;
}

Taxi_CmdAccettacorsa(playerid, const params[])
{
	new pos = 0, tok[48], id = 0;
	Util_NextToken(params, pos, tok, 48);
	if (tok[0] != 0)
	{
		if (!Util_ParseInt(tok, id) || id < 0 || id >= 1000)
		{
			Util_Send(playerid, -10270721, "გამოყენება: /accettacorsa [მოთამაშის id]");
			return 1;
		}
		Taxi_AcceptPlayer(playerid, id);
		return 1;
	}
	new call = g_TxOfferCall[playerid];
	if (call <= 0 || Taxi_TickReached(g_TxOfferUntil[playerid]))
	{
		Util_Send(playerid, -10270721, "მისაღები გამოძახება არ არის. მოთამაშისთვის: /accettacorsa id.");
		return 1;
	}
	if (call <= 1000)
	{
		Taxi_AcceptPlayer(playerid, call - 1);
	}
	else
	{
		Taxi_AcceptNpc(playerid);
	}
	return 1;
}

Taxi_CmdTassametro(playerid, const params[])
{
	new pos = 0, tok[48];
	Util_NextToken(params, pos, tok, 48);
	if (gPlayer[playerid][pTaxiOnDuty] == 0 || gPlayer[playerid][pTaxiRideId] == 0)
	{
		Util_Send(playerid, -10270721, "მიმდინარე მგზავრობა არ გაქვს.");
		return 1;
	}
	if (gPlayer[playerid][pTaxiRideKind] != 2)
	{
		Util_Send(playerid, -1, "ქალაქის კლიენტებთან ტაქსომეტრი ავტომატურია (აყვანა -> ჩასვლა).");
		return 1;
	}
	if (gPlayer[playerid][pTaxiPendOp] != 0)
	{
		Util_Send(playerid, -10270721, "ტაქსის ოპერაცია მიმდინარეობს: მოიცადე.");
		return 1;
	}
	new veh = Rental_GetVehicle(playerid), p = Taxi_PassengerOf(playerid);
	if (!strcmp(tok, "start", true, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (g_TxPhase[playerid] != 2 && g_TxPhase[playerid] != 3)
		{
			Util_Send(playerid, -10270721, "ველოდებით კლიენტის მიერ გადახდის დადასტურებას.");
			return 1;
		}
		if (gPlayer[playerid][pTaxiMeterOn] != 0)
		{
			Util_Send(playerid, -10270721, "ტაქსომეტრი უკვე ჩართულია.");
			return 1;
		}
		if (!Taxi_InTaxi(playerid, veh) || p == INVALID_PLAYER_ID || GetPlayerVehicleID(p) != veh || GetPlayerState(p) != PLAYER_STATE_PASSENGER)
		{
			Util_Send(playerid, -10270721, "ტაქსის საჭესთან უნდა იჯდე, კლიენტით სალონში.");
			return 1;
		}
		DisablePlayerCheckpoint(playerid);
		g_TxPhase[playerid] = 3;
		Taxi_MeterBegin(playerid, veh);
		Util_Send(playerid, 0x66DD66FF, "ტაქსომეტრი ჩაირთო. დანიშნულების ადგილზე: /tassametro stop.");
		Util_Send(p, -1, "ტაქსომეტრი ჩართულია.");
		return 1;
	}
	if (!strcmp(tok, "stop", true, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (gPlayer[playerid][pTaxiMeterOn] != 0)
		{
			Taxi_MeterStop(playerid);
			return 1;
		}
		if (g_TxPhase[playerid] == 3 && g_TxMeterStarted[playerid])
		{
			Taxi_PayPrompt(playerid);
			return 1;
		}
		Util_Send(playerid, -10270721, "ტაქსომეტრი ჩართული არ არის.");
		return 1;
	}
	if (!strcmp(tok, "annulla", true, 0x7FFFFFFF) && tok[0] != 0)
	{
		Taxi_CancelRide(playerid, "მგზავრობა გაუქმდა თანხის ჩამოჭრის გარეშე.", "ტაქსისტმა მგზავრობა გააუქმა: თანხა არ ჩამოგეჭრა.");
		return 1;
	}
	Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /tassametro start|stop|annulla");
	return 1;
}

Taxi_OnProRental(playerid, result, vehicleid)
{
	if (!Taxi_Valid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (result != 1 || vehicleid <= 0)
	{
		switch (result)
		{
			case 3:
			{
				SendClientMessage(playerid, -10270721, "სახსრები არ გყოფნის: საჭიროა %s (ქირა + გირაო).", Money_Str(ECON_TAXI_RENT_HOUR + ECON_TAXI_DEPOSIT));
			}
			case 7:
			{
				Util_Send(playerid, -10270721, "სამსახურს ვერ დაიწყებ: დონე არ გყოფნის ან სხვა ქირაობა/კონტრაქტი აქტიურია.");
			}
			default:
			{
				Util_Send(playerid, -10270721, "ტაქსი არ მოგენიჭა: სცადე ცოტა ხანში (თანხა არ ჩამოგეჭრა).");
			}
		}
		return 0;
	}
	Taxi_ResetPlayer(playerid);
	gPlayer[playerid][pTaxiOnDuty] = 1;
	gPlayer[playerid][pTaxiRideId] = 0;
	Taxi_ScheduleNpc(playerid);
	LD_Emit(107, 0, gPlayer[playerid][pCharId], 1, "-");
	SendClientMessage(playerid, 0x66DD66FF, "სამსახურში ხარ (ქირა %s/სთ, გირაო %s). გამოძახებები თავად მოვა: /accettacorsa. ცვლის დასასრული: /taxi off დეპოში.", Money_Str(ECON_TAXI_RENT_HOUR), Money_Str(ECON_TAXI_DEPOSIT));
	return 1;
}

Taxi_OnProRentalClosed(playerid, result, refund)
{
	if (!Taxi_Valid(playerid) || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0 || g_TxPhase[playerid] != 0)
	{
		Taxi_EndDuty(playerid, 5, false);
	}
	if (!Util_IsPlaying(playerid))
	{
		return 1;
	}
	switch (result)
	{
		case 1:
		{
			SendClientMessage(playerid, -1, "ტაქსი დაბრუნებულია: დაგიბრუნდა გირაო %s (მანქანის მდგომარეობის მიხედვით).", Money_Str(refund));
		}
		case 5:
		{
			Util_Send(playerid, -10270721, "ტაქსი დაშვებული დროის შემდეგ დააბრუნე: გირაო დაკავებულია.");
		}
		case 2:
		{
			Util_Send(playerid, -1, "ტაქსის ქირაობა უკვე დახურული იყო.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "ტაქსის დახურვა არ დადასტურდა: შემდეგ შესვლისას მოგვარდება.");
		}
	}
	return 1;
}

public Taxi_OnDistrictEvent(type, districtId, charId, value, const opId[])
{
	return 1;
}

Taxi_TickNpc(driverid, vehicleid, bool:inVeh)
{
	new pt = g_TxPhase[driverid] == 2 ? g_TxPickPt[driverid] : g_TxDestPt[driverid];
	if (!inVeh || (GetVehicleDistanceFromPoint(vehicleid, g_TxNpcX[pt], g_TxNpcY[pt], g_TxNpcZ[pt]) > 6.0) || (float(Veh_SpeedKmh(vehicleid)) > 12.0))
	{
		g_TxDwellTick[driverid] = 0;
		return 0;
	}
	if (g_TxDwellTick[driverid] == 0)
	{
		g_TxDwellTick[driverid] = Taxi_TickAfter(4000);
		return 0;
	}
	if (!Taxi_TickReached(g_TxDwellTick[driverid]))
	{
		return 0;
	}
	g_TxDwellTick[driverid] = 0;
	if (g_TxPhase[driverid] == 2)
	{
		new d = g_TxDestPt[driverid], name[48];
		g_TxPhase[driverid] = 3;
		g_TxStartHealth[driverid] = Taxi_VehHealth(vehicleid);
		Taxi_MeterBegin(driverid, vehicleid);
		SetPlayerCheckpoint(driverid, g_TxNpcX[d], g_TxNpcY[d], g_TxNpcZ[d], 6.0);
		LD_DistrictName(g_TxNpcD[d], name, 48);
		SendClientMessage(driverid, 0x66DD66FF, "კლიენტი სალონშია: წაიყვანე %s-ში (ჩეკპოინტი).", name);
		return 0;
	}
	Taxi_NpcPay(driverid, vehicleid);
	return 0;
}

Taxi_TickDriver(driverid)
{
	new veh = Rental_GetVehicle(driverid);
	if (g_TxOfferCall[driverid] != 0 && Taxi_TickReached(g_TxOfferUntil[driverid]))
	{
		g_TxOfferCall[driverid] = 0;
		g_TxOfferUntil[driverid] = 0;
	}
	if (veh == 0)
	{
		if (gPlayer[driverid][pRentalId] == 0 && gPlayer[driverid][pRentalPendOp] == 0)
		{
			Util_Send(driverid, -10270721, "შენი ტაქსი აღარ არის ხელმისაწვდომი: სამსახურს გარეთ ხარ.");
			Taxi_EndDuty(driverid, 4, false);
		}
		else if (g_TxPhase[driverid] >= 2 && g_TxPhase[driverid] <= 4)
		{
			Taxi_CancelRide(driverid, "ტაქსი მიუწვდომელია: მგზავრობა გაუქმდა.", "ტაქსი აღარ არის ხელმისაწვდომი: მგზავრობა გაუქმდა.");
		}
		return 0;
	}
	new phase = g_TxPhase[driverid];
	new bool:inVeh = Taxi_InTaxi(driverid, veh);
	if (phase == 2 || phase == 3)
	{
		if (inVeh)
		{
			g_TxOffTick[driverid] = 0;
		}
		else if (g_TxOffTick[driverid] == 0)
		{
			g_TxOffTick[driverid] = Taxi_TickAfter(45000);
			SendClientMessage(driverid, -10270721, "დაბრუნდი ტაქსში %d წამში, თორემ მგზავრობა გაუქმდება.", 45);
		}
		else if (Taxi_TickReached(g_TxOffTick[driverid]))
		{
			Taxi_CancelRide(driverid, "ტაქსი დატოვე: მგზავრობა გაუქმდა.", "ტაქსისტმა ტაქსი დატოვა: მგზავრობა გაუქმდა.");
			return 0;
		}
	}
	if (gPlayer[driverid][pTaxiMeterOn] != 0)
	{
		Taxi_MeterSample(driverid, veh, inVeh);
		Taxi_MeterShow(driverid, true);
	}
	if (phase == 0)
	{
		if (gPlayer[driverid][pTaxiPendOp] == 0 && g_TxOfferCall[driverid] == 0 && gPlayer[driverid][pTaxiNpcOfferAt] != 0 && Taxi_TickReached(gPlayer[driverid][pTaxiNpcOfferAt]) && gPlayer[driverid][pRentalPendOp] == 0)
		{
			Taxi_NpcOffer(driverid);
		}
		return 0;
	}
	if (gPlayer[driverid][pTaxiRideKind] == 1)
	{
		if (phase == 2 || phase == 3)
		{
			Taxi_TickNpc(driverid, veh, inVeh);
		}
		return 0;
	}
	if (phase < 2 || phase > 4)
	{
		return 0;
	}
	new p = Taxi_PassengerOf(driverid);
	if (p == INVALID_PLAYER_ID)
	{
		Taxi_CancelRide(driverid, "კლიენტი აღარ არის ხელმისაწვდომი: მგზავრობა გაუქმდა.", "");
		return 0;
	}
	if (phase == 2 && !g_TxMeterStarted[driverid] && Taxi_TickReached(g_TxBoardUntil[driverid]))
	{
		Taxi_CancelRide(driverid, "კლიენტი დროულად არ ჩაჯდა: მგზავრობა გაუქმდა.", "ტაქსში დროულად არ ჩაჯექი: მგზავრობა გაუქმდა.");
		return 0;
	}
	if (gPlayer[driverid][pTaxiMeterOn] != 0 && GetPlayerVehicleID(p) != veh)
	{
		Taxi_MeterStop(driverid);
	}
	return 0;
}

public Taxi_OnTick()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i))
		{
			continue;
		}
		if (g_TxRetryAt[i] != 0 && Taxi_TickReached(g_TxRetryAt[i]))
		{
			Taxi_RetrySend(i);
			continue;
		}
		if (g_TxCallUntil[i] != 0 && Taxi_TickReached(g_TxCallUntil[i]))
		{
			g_TxCallUntil[i] = 0;
			if (gPlayer[i][pTaxiPeer] == 0)
			{
				Util_Send(i, -1, "შენს გამოძახებას არცერთმა ტაქსმა არ უპასუხა.");
			}
		}
		if (gPlayer[i][pTaxiOnDuty] != 0)
		{
			Taxi_TickDriver(i);
		}
	}
	return 1;
}

Taxi_OnInit()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Taxi_ResetPlayer(i);
	}
	if (g_TxTimer != 0)
	{
		KillTimer(g_TxTimer);
	}
	g_TxTimer = SetTimer("Taxi_OnTick", 1000, true);
	LD_Listen("Taxi_OnDistrictEvent", 20);
	CreatePickup(1239, 1, 1771.0, -1895.0, 13.6, 0);
	Lang_Create3DTextLabel("ტაქსის დეპო\n/taxi on  ·  /taxi off", -2994689, 1771.0, -1895.0, (13.6 + 0.8), 20.0, 0, false);
}

Taxi_OnExit()
{
	if (g_TxTimer != 0)
	{
		KillTimer(g_TxTimer);
	}
	g_TxTimer = 0;
}

Taxi_OnPlayerConnect(playerid)
{
	Taxi_ResetPlayer(playerid);
}

Taxi_OnCharReady(playerid)
{
	#pragma unused playerid
}

Taxi_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	if (!Taxi_Valid(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0 || g_TxPhase[playerid] != 0)
	{
		Taxi_EndDuty(playerid, 2, true);
	}
	new d = Taxi_DriverOf(playerid);
	if (d != 65535)
	{
		Taxi_CancelRide(d, "კლიენტი გავიდა სერვერიდან: მგზავრობა გაუქმდა.", "");
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (i != playerid && g_TxOfferCall[i] == playerid + 1)
		{
			Taxi_ClearOffer(i);
		}
	}
	Taxi_ResetPlayer(playerid);
	return 0;
}

Taxi_OnPlayerDeath(playerid)
{
	if (!Taxi_Valid(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0)
	{
		Util_Send(playerid, -10270721, "ტაქსის ცვლა შეწყდა.");
		Taxi_EndDuty(playerid, 3, true);
	}
	new d = Taxi_DriverOf(playerid);
	if (d != 65535)
	{
		Taxi_CancelRide(d, "კლიენტი ვეღარ გააგრძელებს: მგზავრობა გაუქმდა.", "");
	}
	g_TxCallUntil[playerid] = 0;
	return 0;
}

Taxi_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	#pragma unused ispassenger, playerid, vehicleid
}

Taxi_OnPlayerExitVehicle(playerid, vehicleid)
{
	#pragma unused playerid, vehicleid
}

Taxi_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate, t_PLAYER_STATE:oldstate)
{
	if (!Taxi_Valid(playerid))
	{
		return 0;
	}
	new d = Taxi_DriverOf(playerid);
	if (d == 65535)
	{
		return 0;
	}
	if (newstate == PLAYER_STATE_PASSENGER && g_TxPhase[d] == 2 && GetPlayerVehicleID(playerid) == Rental_GetVehicle(d))
	{
		DisablePlayerCheckpoint(d);
		Util_Send(d, 0x66DD66FF, "კლიენტი სალონშია: /tassametro start.");
		return 0;
	}
	if (oldstate == PLAYER_STATE_PASSENGER && _:gPlayer[d][pTaxiMeterOn] != 0)
	{
		Taxi_MeterStop(d);
	}
	return 0;
}

Taxi_OnVehicleDeath(vehicleid, killerid)
{
	#pragma unused killerid
	if (LD_VehModule(vehicleid) != 2)
	{
		return 0;
	}
	new d = LD_VehOwner(vehicleid);
	if (d == 65535 || !Taxi_Valid(d) || gPlayer[d][pTaxiOnDuty] == 0)
	{
		return 0;
	}
	Util_Send(d, -10270721, "ტაქსი განადგურდა: ცვლა დასრულდა.");
	Taxi_EndDuty(d, 4, true);
	return 0;
}
