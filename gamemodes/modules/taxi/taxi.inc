new Float:g_TxNpcX[6] = {0x44F3C000, 0x44E60000, 0x451BA000, 0x45174000, 2145.0, 0x45098000};
new Float:g_TxNpcY[6] = {0xC4DD4000, 0xC4E74000, 0xC4D34000, 0xC4D98000, -1215.0, 0xC4910000};
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
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), 1);
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
	return floatround(hp, 1);
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
	return vehicleid > 0 && GetPlayerVehicleID(driverid) == vehicleid && GetPlayerState(driverid) == 2;
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
	g_TxMeterStarted[playerid] = 0;
	g_TxBoardUntil[playerid] = 0;
	g_TxPayPrompts[playerid] = 0;
	g_TxPayMeters[playerid] = 0;
	g_TxPayToD[playerid] = 0;
	g_TxPayFare[playerid] = 0;
	g_TxPayAccept[playerid] = 0;
	g_TxPayRating[playerid] = 0;
	g_TxStartKind[playerid] = 0;
	g_TxStartPeer[playerid] = 65535;
	g_TxStartFromD[playerid] = 0;
	g_TxCancelRide[playerid] = 0;
	g_TxAbort[playerid] = 0;
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
	g_TxMeterStarted[driverid] = 0;
	g_TxBoardUntil[driverid] = 0;
	g_TxPayPrompts[driverid] = 0;
	g_TxCdUntil[driverid] = Taxi_TickAfter(10000);
	if (gPlayer[driverid][pTaxiOnDuty] != 0)
	{
		Taxi_ScheduleNpc(driverid);
	}
}

Taxi_CancelRide(driverid, driverMsg[], passengerMsg[])
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
		g_TxAbort[driverid] = 1;
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
}

Taxi_EndDuty(driverid, reason, bool:closeRental)
{
	Taxi_CancelRide(driverid, "", "La corsa e' stata annullata: il taxi non e' piu' in servizio.");
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
	UI_LivingTaxiOffer(driverid, g_TxOfferCall[driverid], 1, "Cliente", fromD, toD, straight, est, 30);
	new a[16], b[16];
	LD_DistrictName(fromD, a, 16);
	LD_DistrictName(toD, b, 16);
	SendClientMessage(driverid, -2994689, "[Taxi] Chiamata: cliente a %s diretto a %s (~$%d). /accettacorsa entro %d s.", a, b, est, 30);
}

bool:Taxi_CanAccept(driverid, bool:verbose)
{
	if (gPlayer[driverid][pTaxiOnDuty] == 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "Non sei in servizio come tassista (/taxi on al deposito di Unity).");
		}
		return 0;
	}
	if (g_TxPhase[driverid] != 0 || gPlayer[driverid][pTaxiRideId] != 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "Hai gia' una corsa in corso.");
		}
		return 0;
	}
	if (gPlayer[driverid][pTaxiPendOp] != 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "Operazione taxi in corso: attendi.");
		}
		return 0;
	}
	if (Rental_GetVehicle(driverid) == 0)
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "Il tuo taxi non e' disponibile.");
		}
		return 0;
	}
	if (g_TxCdUntil[driverid] != 0 && !Taxi_TickReached(g_TxCdUntil[driverid]))
	{
		if (verbose)
		{
			Util_Send(driverid, -10270721, "Attendi qualche secondo prima di accettare una nuova corsa.");
		}
		return 0;
	}
	return 1;
}

Taxi_SendStart(driverid)
{
	DB_LivingTaxiStart(driverid, g_TxStartKind[driverid], g_TxStartPeer[driverid], g_TxStartFromD[driverid], gPlayer[driverid][pTaxiOpId]);
}

Taxi_AcceptNpc(driverid)
{
	if (!Taxi_CanAccept(driverid, 1))
	{
		return 0;
	}
	if (g_TxOfferCall[driverid] < 1001 || Taxi_TickReached(g_TxOfferUntil[driverid]))
	{
		Util_Send(driverid, -10270721, "Nessuna chiamata da accettare (o e' scaduta).");
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
	g_TxAbort[driverid] = 0;
	g_TxStartKind[driverid] = 1;
	g_TxStartPeer[driverid] = 65535;
	g_TxStartFromD[driverid] = g_TxNpcD[fromPt];
	g_TxRetry[driverid] = 0;
	gPlayer[driverid][pTaxiPendOp] = 1;
	DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "ts", gPlayer[driverid][pTaxiOpSeq], gPlayer[driverid][pCharId]);
	Taxi_SendStart(driverid);
}

Taxi_AcceptPlayer(driverid, passengerid)
{
	if (!Taxi_CanAccept(driverid, 1))
	{
		return 0;
	}
	if (!Taxi_Valid(passengerid) || passengerid == driverid || !Util_IsPlaying(passengerid) || g_TxCallUntil[passengerid] == 0 || Taxi_TickReached(g_TxCallUntil[passengerid]))
	{
		Util_Send(driverid, -10270721, "Quella chiamata non e' piu' attiva.");
		return 0;
	}
	if (gPlayer[passengerid][pTaxiPeer] != 0)
	{
		Util_Send(driverid, -10270721, "La chiamata e' gia' stata presa da un altro taxi.");
		return 0;
	}
	if (!Util_InRange(driverid, passengerid, Float:0x44160000))
	{
		Util_Send(driverid, -10270721, "Il cliente e' troppo lontano.");
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
	g_TxAbort[driverid] = 0;
	g_TxStartKind[driverid] = 2;
	g_TxStartPeer[driverid] = passengerid;
	g_TxStartFromD[driverid] = gPlayer[driverid][pTaxiFromDistrict];
	g_TxRetry[driverid] = 0;
	gPlayer[driverid][pTaxiPendOp] = 1;
	DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "ts", gPlayer[passengerid][pCharId], gPlayer[driverid][pCharId]);
	Taxi_SendStart(driverid);
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
			g_TxAbort[driverid] = 0;
			Taxi_CancelRide(driverid, "", "");
			return 0;
		}
		if (npc)
		{
			new pt = g_TxPickPt[driverid];
			SetPlayerCheckpoint(driverid, g_TxNpcX[pt], g_TxNpcY[pt], g_TxNpcZ[pt], 6.0);
			Util_Send(driverid, 0x66DD66FF, "Corsa accettata: raggiungi il cliente (checkpoint) e fermati qualche secondo per farlo salire.");
			return 0;
		}
		new name[25], Float:px, Float:py, Float:pz;
		GetPlayerName(driverid, name, 25);
		GetPlayerPos(p, px, py, pz);
		SetPlayerCheckpoint(driverid, px, py, pz, 6.0);
		g_TxBoardUntil[driverid] = Taxi_TickAfter(180000);
		SendClientMessage(p, 0x66DD66FF, "Il taxi di %s ha accettato la tua chiamata: aspettalo e sali come passeggero.", name);
		Util_Send(driverid, 0x66DD66FF, "Corsa accettata: raggiungi il cliente (checkpoint). Quando e' a bordo: /tassametro start.");
		return 0;
	}
	g_TxAbort[driverid] = 0;
	if (p != INVALID_PLAYER_ID)
	{
		Util_Send(p, -10270721, "Il taxi non ha potuto accettare la corsa: puoi chiamarne un altro.");
	}
	switch (result)
	{
		case 6:
		{
			Util_Send(driverid, -10270721, "Hai raggiunto il massimo di corse con clienti della citta' per quest'ora.");
			Taxi_ResetRide(driverid);
			gPlayer[driverid][pTaxiNpcOfferAt] = Taxi_TickAfter(300000);
			return 0;
		}
		case 7:
		{
			Util_Send(driverid, -10270721, npc ? "Requisiti del servizio taxi non soddisfatti." : "Limite orario di corse con questo cliente raggiunto (o requisiti non soddisfatti).");
		}
		case 4:
		{
			Util_Send(driverid, -10270721, "Risulta gia' una corsa aperta: attendi la sua chiusura.");
		}
		default:
		{
			Util_Send(driverid, -10270721, "Centrale taxi non raggiungibile: corsa non avviata.");
		}
	}
	Taxi_ResetRide(driverid);
}

Taxi_MeterSample(driverid, vehicleid, bool:count)
{
	new Float:x, Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);
	if (count)
	{
		new Float:dx = (x - gPlayer[driverid][pTaxiLastX]), Float:dy = (y - gPlayer[driverid][pTaxiLastY]);
		new Float:step = floatsqroot(((dx * dx) + (dy * dy)));
		if ((step <= Float:0x428C0000))
		{
			g_TxMeterF[driverid] += step;
		}
		gPlayer[driverid][pTaxiMeterM] = floatround(g_TxMeterF[driverid], 1);
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
}

Taxi_MeterBegin(driverid, vehicleid)
{
	Taxi_MeterSample(driverid, vehicleid, 0);
	if (!g_TxMeterStarted[driverid])
	{
		g_TxMeterStarted[driverid] = 1;
		g_TxStartX[driverid] = gPlayer[driverid][pTaxiLastX];
		g_TxStartY[driverid] = gPlayer[driverid][pTaxiLastY];
	}
	gPlayer[driverid][pTaxiMeterOn] = 1;
	Taxi_MeterShow(driverid, 1);
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
	g_TxPayAccept[driverid] = 1;
	g_TxPayRating[driverid] = rating;
	g_TxPayFare[driverid] = LD_TaxiFare(meters);
	g_TxPhase[driverid] = 5;
	g_TxRetry[driverid] = 0;
	gPlayer[driverid][pTaxiToDistrict] = g_TxPayToD[driverid];
	gPlayer[driverid][pTaxiPendOp] = 7;
	DB_LivingMakeOpId(gPlayer[driverid][pTaxiOpId], 41, "tp", gPlayer[driverid][pTaxiRideId], gPlayer[driverid][pCharId]);
	DisablePlayerCheckpoint(driverid);
	Taxi_MeterShow(driverid, 0);
	DB_LivingTaxiPay(driverid, gPlayer[driverid][pTaxiRideId], 65535, meters, g_TxPayToD[driverid], 1, rating, gPlayer[driverid][pTaxiOpId]);
}

Taxi_PayPrompt(driverid)
{
	new p = Taxi_PassengerOf(driverid), veh = Rental_GetVehicle(driverid);
	if (p == INVALID_PLAYER_ID || veh == 0)
	{
		Taxi_CancelRide(driverid, "Corsa annullata: cliente o taxi non piu' disponibili.", "");
		return 0;
	}
	new Float:x, Float:y, Float:z;
	GetVehiclePos(veh, x, y, z);
	new meters = gPlayer[driverid][pTaxiMeterM];
	if (meters < 200 || Taxi_Dist2D(g_TxStartX[driverid], g_TxStartY[driverid], x, y) < 150)
	{
		Taxi_CancelRide(driverid, "Corsa troppo breve: annullata senza addebito.", "Corsa troppo breve: annullata senza addebito.");
		return 0;
	}
	new rep = Taxi_MaxLocalRep(p), name[25];
	GetPlayerName(driverid, name, 25);
	g_TxPayMeters[driverid] = meters;
	g_TxPayToD[driverid] = District_At(x, y);
	g_TxPayFare[driverid] = LD_TaxiFareFor(meters, rep);
	g_TxPayRating[driverid] = 0;
	gPlayer[driverid][pTaxiToDistrict] = g_TxPayToD[driverid];
	g_TxPhase[driverid] = 4;
	Taxi_MeterShow(driverid, 0);
	UI_LivingTaxiPayConfirm(p, gPlayer[driverid][pTaxiRideId], name, meters, g_TxPayFare[driverid], LD_DiscountPct(rep), 60);
	SendClientMessage(driverid, -1, "Tassametro fermo: $%d per %d m. In attesa della conferma del cliente.", g_TxPayFare[driverid], meters);
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
	new p = Taxi_PassengerOf(driverid), other[25];
	if (npc || p == INVALID_PLAYER_ID)
	{
		format(other, 25, "%s", "Cliente");
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
			UI_LivingTaxiReceipt(driverid, 1, rideId, other, g_TxPayMeters[driverid], npc ? net : net + tax, tax, net);
			if (result == 6)
			{
				Util_Send(driverid, -2994689, "Tetto orario del lavoro dinamico raggiunto: e' stato pagato solo il residuo.");
			}
			LD_Emit(3, g_TxPayToD[driverid], gPlayer[driverid][pCharId], net, gPlayer[driverid][pTaxiOpId]);
		}
		case 8:
		{
			gPlayer[driverid][pTaxiRideState] = 3;
			Util_Send(driverid, -10270721, "Il cliente ha contestato la corsa: nessun pagamento, la verifichera' lo staff.");
		}
		case 3:
		{
			Taxi_CancelRide(driverid, "Il cliente non ha fondi sufficienti in banca: corsa annullata.", "");
			return 0;
		}
		case 2:
		{
			Util_Send(driverid, -1, "La corsa risultava gia' chiusa.");
		}
		case -1, 10:
		{
			Util_Send(driverid, -10270721, "Pagamento non confermato dalla centrale: la corsa verra' verificata.");
		}
		default:
		{
			Util_Send(driverid, -10270721, "Corsa non piu' valida: chiusa senza pagamento.");
		}
	}
	Taxi_ResetRide(driverid);
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
				Util_Send(playerid, 0x66DD66FF, "Grazie per il tuo voto.");
				new d = DB_LdCharOnline(g_TxRateDriverChar[playerid]);
				if (d != 65535 && v2 > 0)
				{
					SendClientMessage(d, -1, "[Taxi] Nuovo voto: %d stelle (media %d.%02d).", v1, v2 / 100, v2 % 100);
				}
			}
			else if (result == 2)
			{
				Util_Send(playerid, -1, "Hai gia' votato questa corsa.");
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
			new name[25], meters = 0;
			name[0] = 0;
			if (d != 65535)
			{
				GetPlayerName(d, name, 25);
				meters = g_TxPayMeters[d];
				g_TxRateDriverChar[playerid] = gPlayer[d][pCharId];
			}
			UI_LivingTaxiMeterHide(playerid);
			UI_LivingTaxiReceipt(playerid, 0, rideId, name, meters, charged, 0, 0);
			if (g_TxRateDriverChar[playerid] > 0 && gPlayer[playerid][pTaxiPendOp] == 0)
			{
				g_TxRateRide[playerid] = rideId;
				UI_LivingTaxiRate(playerid, rideId, name);
			}
		}
		case 8:
		{
			Util_Send(playerid, -1, "Corsa contestata: nessun addebito, la verifichera' lo staff.");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, "Fondi insufficienti in banca per pagare la corsa: corsa annullata.");
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
		Util_Send(playerid, -10270721, "Quella chiamata non e' piu' attiva.");
	}
	return 1;
}

public UI_OnTaxiPayConfirm(playerid, rideId, accept)
{
	new d = Taxi_DriverOf(playerid);
	if (d == 65535 || rideId <= 0 || gPlayer[d][pTaxiRideId] != rideId || g_TxPhase[d] != 4 || gPlayer[d][pTaxiPendOp] != 0)
	{
		UI_LivingReject(playerid, "Questa corsa non e' piu' in attesa di pagamento.");
		return 1;
	}
	g_TxPhase[d] = 5;
	g_TxPayAccept[d] = accept != 0;
	g_TxRetry[d] = 0;
	gPlayer[d][pTaxiPendOp] = 7;
	DB_LivingMakeOpId(gPlayer[d][pTaxiOpId], 41, "tp", rideId, gPlayer[d][pCharId]);
	Util_Send(playerid, -1, accept != 0 ? "Pagamento in corso..." : "Contestazione registrata: la corsa restera' in sospeso per lo staff.");
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
		Taxi_CancelRide(d, "Il cliente non ha confermato il pagamento: corsa annullata.", "Non hai confermato il pagamento: corsa annullata.");
		return 1;
	}
	g_TxPhase[d] = 3;
	Util_Send(d, -2994689, "Il cliente non ha risposto: /tassametro stop per richiedere di nuovo il pagamento, /tassametro annulla per chiudere.");
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

Taxi_CmdTaxi(playerid, params[])
{
	new pos = 0, tok[16];
	Util_NextToken(params, pos, tok, 16);
	if (!strcmp(tok, "on", 1, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (gPlayer[playerid][pTaxiOnDuty] != 0)
		{
			Util_Send(playerid, -10270721, "Sei gia' in servizio.");
			return 1;
		}
		if (gPlayer[playerid][pRentalPendOp] != 0 || gPlayer[playerid][pTaxiPendOp] != 0)
		{
			Util_Send(playerid, -10270721, "Pratica in corso: attendi.");
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
			SendClientMessage(playerid, -10270721, "Serve almeno il livello %d per guidare un taxi.", 2);
			return 1;
		}
		if (!Taxi_AtDepot(playerid) || GetPlayerState(playerid) != 1)
		{
			Util_Send(playerid, -10270721, "Presentati a piedi al deposito taxi di Unity Station.");
			return 1;
		}
		new total = 360;
		if ((gPlayer[playerid][pBank] > 0 ? gPlayer[playerid][pBank] : 0) + (gPlayer[playerid][pMoney] > 0 ? gPlayer[playerid][pMoney] : 0) < total)
		{
			SendClientMessage(playerid, -10270721, "Servono $%d (canone $%d/h + cauzione $%d) tra banca e contanti.", total, 60, 300);
			return 1;
		}
		if (!LD_PoolHasRoom() || !Rental_OpenPro(playerid, 4))
		{
			Util_Send(playerid, -10270721, "Nessun taxi disponibile al momento: riprova tra poco.");
			return 1;
		}
		Util_Send(playerid, -1, "Pratica del taxi in corso...");
		return 1;
	}
	if (!strcmp(tok, "off", 1, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (gPlayer[playerid][pTaxiOnDuty] == 0)
		{
			Util_Send(playerid, -10270721, "Non sei in servizio.");
			return 1;
		}
		if (g_TxPhase[playerid] != 0 || gPlayer[playerid][pTaxiRideId] != 0 || gPlayer[playerid][pTaxiPendOp] != 0)
		{
			Util_Send(playerid, -10270721, "Concludi o annulla prima la corsa in corso (/tassametro annulla).");
			return 1;
		}
		if (Rental_GetVehicle(playerid) != 0 && !Taxi_AtDepot(playerid))
		{
			Util_Send(playerid, -10270721, "Riporta il taxi al deposito di Unity Station per chiudere il turno.");
			return 1;
		}
		Taxi_EndDuty(playerid, 1, 1);
		Util_Send(playerid, -1, "Fine turno: riconsegna del taxi in corso...");
		return 1;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0)
	{
		SendClientMessage(playerid, -1, "In servizio. Punti taxi: %d. Comandi: /accettacorsa [id] · /tassametro start|stop|annulla · /taxi off (al deposito).", LD_GetProPoints(playerid, 2));
	}
	else
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /taxi on|off al deposito di Unity Station (canone orario + cauzione).");
	}
	return 1;
}

Taxi_CmdChiamataxi(playerid, params[])
{
	if (!Util_IsPlaying(playerid) || gPlayer[playerid][pLivingReady] == 0)
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0)
	{
		Util_Send(playerid, -10270721, "Sei in servizio come tassista.");
		return 1;
	}
	if (gPlayer[playerid][pTaxiPeer] != 0)
	{
		Util_Send(playerid, -10270721, "Un taxi ha gia' accettato la tua chiamata.");
		return 1;
	}
	if (gPlayer[playerid][pTaxiCallTick] != 0 && GetTickCount() - gPlayer[playerid][pTaxiCallTick] < 60000)
	{
		SendClientMessage(playerid, -10270721, "Hai gia' chiamato un taxi: attendi %d secondi.", (60000 - (GetTickCount() - gPlayer[playerid][pTaxiCallTick])) / 1000 + 1);
		return 1;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Util_Send(playerid, -10270721, "Devi trovarti all'aperto per chiamare un taxi.");
		return 1;
	}
	gPlayer[playerid][pTaxiCallTick] = GetTickCount();
	if (gPlayer[playerid][pTaxiCallTick] == 0)
	{
		gPlayer[playerid][pTaxiCallTick] = 1;
	}
	g_TxCallUntil[playerid] = Taxi_TickAfter(60000);
	new name[25], Float:px, Float:py, Float:pz, sent = 0, area[16];
	GetPlayerName(playerid, name, 25);
	GetPlayerPos(playerid, px, py, pz);
	new fromD = District_At(px, py);
	LD_DistrictName(fromD, area, 16);
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
		if (!Util_InRange(d, playerid, Float:0x44160000))
		{
			continue;
		}
		new Float:dx, Float:dy, Float:dz;
		GetPlayerPos(d, dx, dy, dz);
		SendClientMessage(d, -2994689, "[Taxi] %s (id %d) chiede un taxi a %s. /accettacorsa %d", name, playerid, area, playerid);
		if (g_TxOfferCall[d] == 0)
		{
			g_TxOfferCall[d] = playerid + 1;
			g_TxOfferUntil[d] = Taxi_TickAfter(30000);
			UI_LivingTaxiOffer(d, playerid + 1, 0, name, fromD, fromD, Taxi_Dist2D(dx, dy, px, py), LD_TaxiFare(0), 30);
		}
		sent++;
	}
	if (sent > 0)
	{
		SendClientMessage(playerid, 0x66DD66FF, "Chiamata inviata a %d taxi in zona: attendi una risposta.", sent);
	}
	else
	{
		Util_Send(playerid, -10270721, "Nessun taxi in servizio nelle vicinanze. Riprova piu' tardi.");
	}
	return 1;
}

Taxi_CmdAccettacorsa(playerid, params[])
{
	new pos = 0, tok[16], id = 0;
	Util_NextToken(params, pos, tok, 16);
	if (tok[0] != 0)
	{
		if (!Util_ParseInt(tok, id) || id < 0 || id >= 1000)
		{
			Util_Send(playerid, -10270721, "Uso: /accettacorsa [id giocatore]");
			return 1;
		}
		Taxi_AcceptPlayer(playerid, id);
		return 1;
	}
	new call = g_TxOfferCall[playerid];
	if (call <= 0 || Taxi_TickReached(g_TxOfferUntil[playerid]))
	{
		Util_Send(playerid, -10270721, "Nessuna chiamata da accettare. Per un giocatore: /accettacorsa id.");
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

Taxi_CmdTassametro(playerid, params[])
{
	new pos = 0, tok[16];
	Util_NextToken(params, pos, tok, 16);
	if (gPlayer[playerid][pTaxiOnDuty] == 0 || gPlayer[playerid][pTaxiRideId] == 0)
	{
		Util_Send(playerid, -10270721, "Non hai una corsa in corso.");
		return 1;
	}
	if (gPlayer[playerid][pTaxiRideKind] != 2)
	{
		Util_Send(playerid, -1, "Con i clienti della citta' il tassametro e' automatico (prelievo -> arrivo).");
		return 1;
	}
	if (gPlayer[playerid][pTaxiPendOp] != 0)
	{
		Util_Send(playerid, -10270721, "Operazione taxi in corso: attendi.");
		return 1;
	}
	new veh = Rental_GetVehicle(playerid), p = Taxi_PassengerOf(playerid);
	if (!strcmp(tok, "start", 1, 0x7FFFFFFF) && tok[0] != 0)
	{
		if (g_TxPhase[playerid] != 2 && g_TxPhase[playerid] != 3)
		{
			Util_Send(playerid, -10270721, "In attesa della conferma di pagamento del cliente.");
			return 1;
		}
		if (gPlayer[playerid][pTaxiMeterOn] != 0)
		{
			Util_Send(playerid, -10270721, "Il tassametro e' gia' acceso.");
			return 1;
		}
		if (!Taxi_InTaxi(playerid, veh) || p == INVALID_PLAYER_ID || GetPlayerVehicleID(p) != veh || GetPlayerState(p) != 3)
		{
			Util_Send(playerid, -10270721, "Devi essere al volante del taxi con il cliente a bordo.");
			return 1;
		}
		DisablePlayerCheckpoint(playerid);
		g_TxPhase[playerid] = 3;
		Taxi_MeterBegin(playerid, veh);
		Util_Send(playerid, 0x66DD66FF, "Tassametro acceso. A destinazione: /tassametro stop.");
		Util_Send(p, -1, "Il tassametro e' acceso.");
		return 1;
	}
	if (!strcmp(tok, "stop", 1, 0x7FFFFFFF) && tok[0] != 0)
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
		Util_Send(playerid, -10270721, "Il tassametro non e' acceso.");
		return 1;
	}
	if (!strcmp(tok, "annulla", 1, 0x7FFFFFFF) && tok[0] != 0)
	{
		Taxi_CancelRide(playerid, "Corsa annullata senza addebito.", "Il tassista ha annullato la corsa: nessun addebito.");
		return 1;
	}
	Util_Send(playerid, 0x99CCFFFF, "Uso: /tassametro start|stop|annulla");
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
				SendClientMessage(playerid, -10270721, "Fondi insufficienti: servono $%d (canone + cauzione).", 360);
			}
			case 7:
			{
				Util_Send(playerid, -10270721, "Non puoi prendere servizio: livello insufficiente o un altro noleggio/contratto attivo.");
			}
			default:
			{
				Util_Send(playerid, -10270721, "Nessun taxi assegnato: riprova tra poco (nessun addebito).");
			}
		}
		return 0;
	}
	Taxi_ResetPlayer(playerid);
	gPlayer[playerid][pTaxiOnDuty] = 1;
	gPlayer[playerid][pTaxiRideId] = 0;
	Taxi_ScheduleNpc(playerid);
	LD_Emit(107, 0, gPlayer[playerid][pCharId], 1, "-");
	SendClientMessage(playerid, 0x66DD66FF, "Sei in servizio (canone $%d/h, cauzione $%d). Le chiamate arrivano da sole: /accettacorsa. Fine turno: /taxi off al deposito.", 60, 300);
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
		Taxi_EndDuty(playerid, 5, 0);
	}
	if (!Util_IsPlaying(playerid))
	{
		return 1;
	}
	switch (result)
	{
		case 1:
		{
			SendClientMessage(playerid, -1, "Taxi riconsegnato: cauzione restituita $%d (in base alle condizioni del mezzo).", refund);
		}
		case 5:
		{
			Util_Send(playerid, -10270721, "Taxi riconsegnato oltre la tolleranza: cauzione trattenuta.");
		}
		case 2:
		{
			Util_Send(playerid, -1, "Il noleggio del taxi risultava gia' chiuso.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "Chiusura del taxi non confermata: verra' sistemata al prossimo accesso.");
		}
	}
	return 1;
}

public Taxi_OnDistrictEvent(type, districtId, charId, value, opId[])
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
		new d = g_TxDestPt[driverid], name[16];
		g_TxPhase[driverid] = 3;
		g_TxStartHealth[driverid] = Taxi_VehHealth(vehicleid);
		Taxi_MeterBegin(driverid, vehicleid);
		SetPlayerCheckpoint(driverid, g_TxNpcX[d], g_TxNpcY[d], g_TxNpcZ[d], 6.0);
		LD_DistrictName(g_TxNpcD[d], name, 16);
		SendClientMessage(driverid, 0x66DD66FF, "Cliente a bordo: portalo a %s (checkpoint).", name);
		return 0;
	}
	Taxi_NpcPay(driverid, vehicleid);
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
			Util_Send(driverid, -10270721, "Il tuo taxi non e' piu' disponibile: fuori servizio.");
			Taxi_EndDuty(driverid, 4, 0);
		}
		else if (g_TxPhase[driverid] >= 2 && g_TxPhase[driverid] <= 4)
		{
			Taxi_CancelRide(driverid, "Taxi non disponibile: corsa annullata.", "Il taxi non e' piu' disponibile: corsa annullata.");
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
			SendClientMessage(driverid, -10270721, "Torna sul taxi entro %d secondi o la corsa sara' annullata.", 45);
		}
		else if (Taxi_TickReached(g_TxOffTick[driverid]))
		{
			Taxi_CancelRide(driverid, "Hai lasciato il taxi: corsa annullata.", "Il tassista ha lasciato il taxi: corsa annullata.");
			return 0;
		}
	}
	if (gPlayer[driverid][pTaxiMeterOn] != 0)
	{
		Taxi_MeterSample(driverid, veh, inVeh);
		Taxi_MeterShow(driverid, 1);
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
		Taxi_CancelRide(driverid, "Il cliente non e' piu' disponibile: corsa annullata.", "");
		return 0;
	}
	if (phase == 2 && !g_TxMeterStarted[driverid] && Taxi_TickReached(g_TxBoardUntil[driverid]))
	{
		Taxi_CancelRide(driverid, "Il cliente non e' salito in tempo: corsa annullata.", "Non sei salito sul taxi in tempo: corsa annullata.");
		return 0;
	}
	if (gPlayer[driverid][pTaxiMeterOn] != 0 && GetPlayerVehicleID(p) != veh)
	{
		Taxi_MeterStop(driverid);
	}
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
				Util_Send(i, -1, "Nessun taxi ha risposto alla tua chiamata.");
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
	g_TxTimer = SetTimer("Taxi_OnTick", 1000, 1);
	LD_Listen("Taxi_OnDistrictEvent", 20);
	CreatePickup(1239, 1, 1771.0, -1895.0, 13.6, 0);
	Lang_Create3DTextLabel("Deposito taxi\n/taxi on  ·  /taxi off", -2994689, 1771.0, -1895.0, (13.6 + 0.8), Float:0x41A00000, 0, 0);
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
}

Taxi_OnPlayerDisconnect(playerid, reason)
{
	if (!Taxi_Valid(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0 || g_TxPhase[playerid] != 0)
	{
		Taxi_EndDuty(playerid, 2, 1);
	}
	new d = Taxi_DriverOf(playerid);
	if (d != 65535)
	{
		Taxi_CancelRide(d, "Il cliente si e' disconnesso: corsa annullata.", "");
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (i != playerid && g_TxOfferCall[i] == playerid + 1)
		{
			Taxi_ClearOffer(i);
		}
	}
	Taxi_ResetPlayer(playerid);
}

Taxi_OnPlayerDeath(playerid)
{
	if (!Taxi_Valid(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0)
	{
		Util_Send(playerid, -10270721, "Turno di taxi interrotto.");
		Taxi_EndDuty(playerid, 3, 1);
	}
	new d = Taxi_DriverOf(playerid);
	if (d != 65535)
	{
		Taxi_CancelRide(d, "Il cliente non e' in condizione di proseguire: corsa annullata.", "");
	}
	g_TxCallUntil[playerid] = 0;
}

Taxi_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
}

Taxi_OnPlayerExitVehicle(playerid, vehicleid)
{
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
	if (newstate == 3 && g_TxPhase[d] == 2 && GetPlayerVehicleID(playerid) == Rental_GetVehicle(d))
	{
		DisablePlayerCheckpoint(d);
		Util_Send(d, 0x66DD66FF, "Il cliente e' a bordo: /tassametro start.");
		return 0;
	}
	if (oldstate == 3 && gPlayer[d][pTaxiMeterOn] != 0)
	{
		Taxi_MeterStop(d);
	}
}

Taxi_OnVehicleDeath(vehicleid, killerid)
{
	if (LD_VehModule(vehicleid) != 2)
	{
		return 0;
	}
	new d = LD_VehOwner(vehicleid);
	if (d == 65535 || !Taxi_Valid(d) || gPlayer[d][pTaxiOnDuty] == 0)
	{
		return 0;
	}
	Util_Send(d, -10270721, "Il taxi e' stato distrutto: fine del turno.");
	Taxi_EndDuty(d, 4, 1);
}
