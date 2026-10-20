new g_DirBoard[24][E_DIR_BOARD];
new g_DirBoardCount;
new bool:g_DirBoardLoaded;
new g_DirExpAt[MAX_PLAYERS];
new g_DirRetry[MAX_PLAYERS];
new g_DirSeq[MAX_PLAYERS];
new g_DirRelAfter[MAX_PLAYERS];
new g_DirRelReason[MAX_PLAYERS];
new bool:g_DirRelSilent[MAX_PLAYERS];
new g_DirCompInteg[MAX_PLAYERS];
new g_DirCompSec[MAX_PLAYERS];
new g_DirCompOp[MAX_PLAYERS][41];
new g_DirCompCt[MAX_PLAYERS];
new g_DirLockCt[MAX_PLAYERS];
new g_DirLockAt[MAX_PLAYERS];

Director_WarehousePos(districtId, &Float:x, &Float:y)
{
	switch (districtId)
	{
		case 1:
		{
			x = 1880.0;
			y = -1780.0;
		}
		case 2:
		{
			x = 2400.0;
			y = -1740.0;
		}
		case 3:
		{
			x = 2145.0;
			y = -1215.0;
		}
		default:
		{
			x = 0.0;
			y = 0.0;
		}
	}
}

Director_ShopPos(districtId, &Float:x, &Float:y)
{
	switch (districtId)
	{
		case 1:
		{
			x = 1927.0;
			y = -1790.0;
		}
		case 2:
		{
			x = 2490.0;
			y = -1690.0;
		}
		case 3:
		{
			x = 2190.0;
			y = -1155.0;
		}
		default:
		{
			x = 0.0;
			y = 0.0;
		}
	}
}

Director_RouteMeters(fromD, toD)
{
	new Float:ax, Float:ay, Float:bx, Float:by;
	Director_WarehousePos(fromD, ax, ay);
	Director_ShopPos(toD, bx, by);
	new Float:dx = (bx - ax), Float:dy = (by - ay);
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), 1);
}

Director_BoardFind(contractId)
{
	for (new i = 0; i < g_DirBoardCount; i++)
	{
		if (g_DirBoard[i][dbContractId] == contractId)
		{
			return i;
		}
	}
	return -1;
}

Director_BestLocal(playerid, fromD, toD)
{
	new a = LD_GetLocalRep(playerid, fromD), b = LD_GetLocalRep(playerid, toD);
	return a > b ? a : b;
}

bool:Director_PremiumOk(playerid, fromD, toD)
{
	return Director_BestLocal(playerid, fromD, toD) >= 20 || LD_GetProPoints(playerid, 1) >= 100;
}

bool:Director_TickBefore(untilTick)
{
	return untilTick != 0 && GetTickCount() - untilTick < 0;
}

Director_BlockCode(playerid, i)
{
	if (i < 0 || i >= g_DirBoardCount)
	{
		return 10;
	}
	if (gPlayer[playerid][pLevel] < g_DirBoard[i][dbMinLevel])
	{
		return 1;
	}
	if (LD_GetProPoints(playerid, 1) < g_DirBoard[i][dbMinRep])
	{
		return 2;
	}
	if (g_DirBoard[i][dbPremium] == 0 && Director_BestLocal(playerid, g_DirBoard[i][dbFromDistrict], g_DirBoard[i][dbToDistrict]) < g_DirBoard[i][dbMinLocalRep])
	{
		return 3;
	}
	if (gPlayer[playerid][pContractId] != 0 || gPlayer[playerid][pFreightWelcome] != 0)
	{
		return 4;
	}
	if (LD_ActiveFlow(playerid) != 0)
	{
		return 5;
	}
	if (Director_TickBefore(gPlayer[playerid][pContractCooldownUntil]))
	{
		return 6;
	}
	if (DB_LivingPlayerGet(playerid, 2) >= 8 || DB_LivingPlayerGet(playerid, 1) >= 2800)
	{
		return 7;
	}
	if (g_DirLockCt[playerid] == g_DirBoard[i][dbContractId] && gettime() - g_DirLockAt[playerid] < 600)
	{
		return 8;
	}
	if (g_DirBoard[i][dbExpiresAt] <= gettime())
	{
		return 9;
	}
	return 0;
}

bool:Director_BoardVisible(playerid, i)
{
	if (i < 0 || i >= g_DirBoardCount || g_DirBoard[i][dbExpiresAt] <= gettime())
	{
		return 0;
	}
	if (g_DirBoard[i][dbPremium] != 0 && !Director_PremiumOk(playerid, g_DirBoard[i][dbFromDistrict], g_DirBoard[i][dbToDistrict]))
	{
		return 0;
	}
	return 1;
}

Director_ClearContract(playerid)
{
	gPlayer[playerid][pContractId] = 0;
	gPlayer[playerid][pContractPhase] = 0;
	gPlayer[playerid][pContractFromDistrict] = 0;
	gPlayer[playerid][pContractToDistrict] = 0;
	gPlayer[playerid][pContractCargo] = 0;
	gPlayer[playerid][pContractDistance] = 0;
	gPlayer[playerid][pContractReward] = 0;
	gPlayer[playerid][pContractStartTick] = 0;
	gPlayer[playerid][pFreightLegTick] = 0;
	gPlayer[playerid][pContractPendOp] = 0;
	gPlayer[playerid][pContractOpId][0] = 0;
	g_DirExpAt[playerid] = 0;
	g_DirRetry[playerid] = 0;
	g_DirRelAfter[playerid] = 0;
	g_DirRelReason[playerid] = 0;
	g_DirRelSilent[playerid] = 0;
	g_DirCompOp[playerid][0] = 0;
	g_DirCompCt[playerid] = 0;
	g_DirSeq[playerid]++;
}

Director_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	Director_ClearContract(playerid);
	g_DirLockCt[playerid] = 0;
	g_DirLockAt[playerid] = 0;
	g_DirCompInteg[playerid] = 0;
	g_DirCompSec[playerid] = 0;
}

Director_SetOp(playerid, pend, const tag[], ref)
{
	gPlayer[playerid][pContractPendOp] = pend;
	gPlayer[playerid][pContractOpId][0] = 0;
	DB_LivingMakeOpId(gPlayer[playerid][pContractOpId], 41, tag, ref, gPlayer[playerid][pCharId]);
	g_DirRetry[playerid] = 0;
}

Director_Send(playerid)
{
	new ct = gPlayer[playerid][pContractId];
	switch (gPlayer[playerid][pContractPendOp])
	{
		case 1:
		{
			DB_LivingCtAccept(playerid, ct, gPlayer[playerid][pContractOpId]);
		}
		case 2:
		{
			DB_LivingCtPickup(playerid, ct, gPlayer[playerid][pContractOpId]);
		}
		case 3:
		{
			DB_LivingCtComplete(playerid, ct, gPlayer[playerid][pContractOpId], g_DirCompInteg[playerid], g_DirCompSec[playerid]);
		}
		case 4:
		{
			new r = g_DirRelReason[playerid] == 1 || g_DirRelReason[playerid] == 4 ? 1 : 0;
			DB_LivingCtRelease(playerid, ct, gPlayer[playerid][pContractOpId], r);
		}
	}
}

bool:Director_ScheduleRetry(playerid)
{
	if (g_DirRetry[playerid] >= 3)
	{
		return 0;
	}
	g_DirRetry[playerid]++;
	SetTimerEx("Director_OnRetry", 2500, 0, "ddd", playerid, gPlayer[playerid][pCharId], g_DirSeq[playerid]);
	return 1;
}

public Director_OnRetry(playerid, charId, seq)
{
	if (!IsPlayerConnected(playerid) || charId <= 0 || gPlayer[playerid][pCharId] != charId || g_DirSeq[playerid] != seq)
	{
		return 1;
	}
	if (gPlayer[playerid][pContractPendOp] == 0 || gPlayer[playerid][pContractId] == 0)
	{
		return 1;
	}
	Director_Send(playerid);
	return 1;
}

Director_RunDeferred(playerid)
{
	new r = g_DirRelAfter[playerid];
	g_DirRelAfter[playerid] = 0;
	if (r != 0 && gPlayer[playerid][pContractId] != 0 && gPlayer[playerid][pContractPendOp] == 0)
	{
		Director_Release(playerid, r);
	}
}

Director_EndLocal(playerid, freightReason, bool:released, relReason)
{
	new ct = gPlayer[playerid][pContractId], charId = gPlayer[playerid][pCharId], fromD = gPlayer[playerid][pContractFromDistrict];
	Freight_End(playerid, freightReason);
	Director_ClearContract(playerid);
	if (released && ct > 0)
	{
		g_DirLockCt[playerid] = ct;
		g_DirLockAt[playerid] = gettime();
		LD_Emit(103, fromD, charId, relReason, "-");
	}
	DB_LivingBoard(65535);
}

Director_OnInit()
{
	g_DirBoardCount = 0;
	g_DirBoardLoaded = 0;
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		Director_ResetPlayer(p);
	}
	LD_Listen("Director_OnDistrictEvent", 10);
}

Director_OnExit()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		Director_ClearContract(p);
	}
	g_DirBoardCount = 0;
}

Director_OnPlayerConnect(playerid)
{
	Director_ResetPlayer(playerid);
}

Director_OnCharReady(playerid)
{
	#pragma unused playerid
	if (!g_DirBoardLoaded || g_DirBoardCount < 4)
	{
		Director_OnCycle();
	}
}

Director_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	new ct = gPlayer[playerid][pContractId];
	if (ct != 0 && gPlayer[playerid][pContractPendOp] == 0 && gPlayer[playerid][pContractPhase] != 0)
	{
		new op[41];
		DB_LivingMakeOpId(op, 41, "cr", ct, gPlayer[playerid][pCharId]);
		DB_LivingCtRelease(playerid, ct, op, 0);
	}
	Director_ResetPlayer(playerid);
}

public Director_OnDistrictEvent(type, districtId, charId, value, const opId[])
{
	if (type == 9 || type == 8)
	{
		DB_LivingBoard(65535);
	}
	return 1;
}

Director_PlayersInGame()
{
	new n = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (Util_IsPlaying(i) && gPlayer[i][pLivingReady] != 0)
		{
			n++;
		}
	}
	return n;
}

Director_Generate(bucket)
{
	new slots = DB_LivingBucketSlots();
	for (new s = 0; s < slots; s++)
	{
		new fromD = 0, toD = 0, cargo = 0;
		if (!LD_ContractSlot(bucket, s, fromD, toD, cargo))
		{
			continue;
		}
		new meters = Director_RouteMeters(fromD, toD);
		if (meters < 300)
		{
			continue;
		}
		new bool:premium = 0;
		if (s >= 3)
		{
			new h = LD_Hash32(bucket, s, 2);
			premium = LD_HashMod(h >>> 24, 2) == 0;
		}
		DB_LivingGenerate(bucket, s, fromD, toD, fromD, toD, cargo, meters, 2, 0, 0, premium ? 1 : 0);
	}
}

public Director_OnCycle()
{
	if (!LD_IsReady())
	{
		return 1;
	}
	new now = gettime();
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || gPlayer[p][pContractId] == 0 || gPlayer[p][pContractPendOp] != 0)
		{
			continue;
		}
		if (gPlayer[p][pContractPhase] != 1 && gPlayer[p][pContractPhase] != 2)
		{
			continue;
		}
		if (g_DirExpAt[p] > 0 && now >= g_DirExpAt[p])
		{
			Director_Release(p, 5);
		}
	}
	if (Director_PlayersInGame() > 0)
	{
		Director_Generate(LD_Bucket(now));
	}
	DB_LivingBoard(65535);
	return 1;
}

public Living_OnDBBoard(playerid, result, count)
{
	if (playerid != INVALID_PLAYER_ID || result != 1)
	{
		return 1;
	}
	new n = 0;
	for (new i = 0; i < count && n < 24; i++)
	{
		if (g_LdBoard[i][lbState] != 0 && g_LdBoard[i][lbState] != 7)
		{
			continue;
		}
		g_DirBoard[n][dbContractId] = g_LdBoard[i][lbId];
		g_DirBoard[n][dbFromDistrict] = g_LdBoard[i][lbFromDistrict];
		g_DirBoard[n][dbToDistrict] = g_LdBoard[i][lbToDistrict];
		g_DirBoard[n][dbCargo] = g_LdBoard[i][lbCargo];
		g_DirBoard[n][dbDistanceM] = g_LdBoard[i][lbDistanceM];
		g_DirBoard[n][dbReward] = g_LdBoard[i][lbReward];
		g_DirBoard[n][dbExpiresAt] = g_LdBoard[i][lbExpiresAt];
		g_DirBoard[n][dbMinLevel] = g_LdBoard[i][lbMinLevel];
		g_DirBoard[n][dbMinRep] = g_LdBoard[i][lbMinRep];
		g_DirBoard[n][dbMinLocalRep] = g_LdBoard[i][lbMinLocalRep];
		g_DirBoard[n][dbPremium] = g_LdBoard[i][lbPremium];
		n++;
	}
	g_DirBoardCount = n;
	g_DirBoardLoaded = 1;
	LD_Emit(104, 0, 0, n, "-");
	return 1;
}

Director_BoardCount()
{
	return g_DirBoardCount;
}

bool:Director_BoardGet(index, &contractId, &fromD, &toD, &cargo, &reward, &distanceM, &expiresAt, &minLevel, &minRep, &minLocalRep, &premium)
{
	if (index < 0 || index >= g_DirBoardCount)
	{
		contractId = 0;
		fromD = 0;
		toD = 0;
		cargo = 0;
		reward = 0;
		distanceM = 0;
		expiresAt = 0;
		minLevel = 0;
		minRep = 0;
		minLocalRep = 0;
		premium = 0;
		return 0;
	}
	contractId = g_DirBoard[index][dbContractId];
	fromD = g_DirBoard[index][dbFromDistrict];
	toD = g_DirBoard[index][dbToDistrict];
	cargo = g_DirBoard[index][dbCargo];
	reward = g_DirBoard[index][dbReward];
	distanceM = g_DirBoard[index][dbDistanceM];
	expiresAt = g_DirBoard[index][dbExpiresAt];
	minLevel = g_DirBoard[index][dbMinLevel];
	minRep = g_DirBoard[index][dbMinRep];
	minLocalRep = g_DirBoard[index][dbMinLocalRep];
	premium = g_DirBoard[index][dbPremium];
	return 1;
}

Director_ExpiresAt(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	return g_DirExpAt[playerid];
}

Director_BlockMsg(playerid, code)
{
	switch (code)
	{
		case 1:
		{
			Util_Send(playerid, -10270721, "Serve un livello piu' alto per questo contratto.");
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "La tua reputazione nei trasporti non basta ancora per questo contratto.");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, "Il quartiere non ti conosce abbastanza per questo contratto.");
		}
		case 4:
		{
			Util_Send(playerid, -10270721, "Hai gia' un contratto attivo (/contratto).");
		}
		case 5:
		{
			LD_SendBlock(playerid, 4);
		}
		case 6:
		{
			Util_Send(playerid, -10270721, "Prenditi un attimo: potrai accettare un nuovo contratto tra poco.");
		}
		case 7:
		{
			Util_Send(playerid, -10270721, "Per quest'ora hai lavorato abbastanza: riprova piu' tardi.");
		}
		case 8:
		{
			Util_Send(playerid, -10270721, "Hai lasciato questo contratto da poco: scegline un altro.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "Questo contratto non e' piu' disponibile.");
		}
	}
}

bool:Director_Accept(playerid, contractId)
{
	if (!Util_IsPlaying(playerid))
	{
		return 0;
	}
	if (!LD_IsReady() || gPlayer[playerid][pLivingReady] == 0)
	{
		LD_SendBlock(playerid, 1);
		return 0;
	}
	if (gPlayer[playerid][pContractPendOp] != 0 || DB_LivingBusy(playerid, 0))
	{
		Util_Send(playerid, -10270721, "Un'operazione sul contratto e' gia' in corso: attendi.");
		return 0;
	}
	if (Hub_NearBoard(playerid) < 0)
	{
		Util_Send(playerid, -10270721, "I contratti si accettano a una bacheca: Casa del Quartiere di Idlewood, magazzino di Ganton o di Jefferson.");
		return 0;
	}
	new block = LD_CheckStart(playerid);
	if (block != 0)
	{
		LD_SendBlock(playerid, block);
		return 0;
	}
	if (gPlayer[playerid][pLevel] < 2)
	{
		Director_BlockMsg(playerid, 1);
		return 0;
	}
	new i = Director_BoardFind(contractId);
	if (i < 0 || !Director_BoardVisible(playerid, i))
	{
		Director_BlockMsg(playerid, 9);
		return 0;
	}
	new code = Director_BlockCode(playerid, i);
	if (code != 0)
	{
		Director_BlockMsg(playerid, code);
		return 0;
	}
	gPlayer[playerid][pContractId] = contractId;
	gPlayer[playerid][pContractPhase] = 0;
	gPlayer[playerid][pContractFromDistrict] = g_DirBoard[i][dbFromDistrict];
	gPlayer[playerid][pContractToDistrict] = g_DirBoard[i][dbToDistrict];
	gPlayer[playerid][pContractCargo] = g_DirBoard[i][dbCargo];
	gPlayer[playerid][pContractDistance] = g_DirBoard[i][dbDistanceM];
	gPlayer[playerid][pContractReward] = g_DirBoard[i][dbReward];
	g_DirExpAt[playerid] = g_DirBoard[i][dbExpiresAt];
	Director_SetOp(playerid, 1, "ca", contractId);
	Director_Send(playerid);
	return 1;
}

bool:Director_Pickup(playerid)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pContractId] == 0 || gPlayer[playerid][pContractPhase] != 1)
	{
		return 0;
	}
	if (gPlayer[playerid][pContractPendOp] != 0)
	{
		return 0;
	}
	Director_SetOp(playerid, 2, "cp", gPlayer[playerid][pContractId]);
	Director_Send(playerid);
	return 1;
}

bool:Director_Complete(playerid, integrityPct, seconds)
{
	new ct = gPlayer[playerid][pContractId];
	if (!IsPlayerConnected(playerid) || ct == 0 || gPlayer[playerid][pContractPhase] != 2)
	{
		return 0;
	}
	if (gPlayer[playerid][pContractPendOp] != 0)
	{
		return 0;
	}
	g_DirCompInteg[playerid] = LD_Clamp(integrityPct, 0, 100);
	g_DirCompSec[playerid] = seconds > 0 ? seconds : 0;
	gPlayer[playerid][pContractPendOp] = 3;
	g_DirRetry[playerid] = 0;
	if (g_DirCompCt[playerid] == ct && g_DirCompOp[playerid][0] != 0)
	{
		gPlayer[playerid][pContractOpId][0] = 0;
		strcat(gPlayer[playerid][pContractOpId], g_DirCompOp[playerid], 41);
	}
	else
	{
		DB_LivingMakeOpId(gPlayer[playerid][pContractOpId], 41, "cc", ct, gPlayer[playerid][pCharId]);
		g_DirCompCt[playerid] = ct;
		g_DirCompOp[playerid][0] = 0;
		strcat(g_DirCompOp[playerid], gPlayer[playerid][pContractOpId], 41);
	}
	Director_Send(playerid);
	return 1;
}

bool:Director_Release(playerid, reason)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pContractId] == 0)
	{
		return 0;
	}
	if (gPlayer[playerid][pContractPendOp] == 4)
	{
		return 1;
	}
	if (gPlayer[playerid][pContractPendOp] != 0)
	{
		g_DirRelAfter[playerid] = reason;
		return 1;
	}
	Freight_End(playerid, reason);
	g_DirRelReason[playerid] = reason;
	Director_SetOp(playerid, 4, "cr", gPlayer[playerid][pContractId]);
	Director_Send(playerid);
	return 1;
}

Director_OnAccept(playerid, result, contractId, v2)
{
	new charId = gPlayer[playerid][pCharId], msg[160];
	if (result == 1)
	{
		gPlayer[playerid][pContractPendOp] = 0;
		gPlayer[playerid][pContractPhase] = 1;
		gPlayer[playerid][pContractStartTick] = GetTickCount();
		gPlayer[playerid][pContractCooldownUntil] = GetTickCount() + 20000;
		if (v2 > 0)
		{
			g_DirExpAt[playerid] = v2;
		}
		if (!Freight_Begin(playerid, contractId, gPlayer[playerid][pContractFromDistrict], gPlayer[playerid][pContractToDistrict], gPlayer[playerid][pContractCargo], gPlayer[playerid][pContractDistance], 0))
		{
			Util_Send(playerid, -10270721, "Il magazzino non ha un mezzo disponibile adesso: contratto rimesso in bacheca senza penalita'.");
			Director_Release(playerid, 6);
			return 0;
		}
		LD_Emit(101, gPlayer[playerid][pContractFromDistrict], charId, contractId, gPlayer[playerid][pContractOpId]);
		DB_LivingBoard(65535);
		Director_RunDeferred(playerid);
		return 0;
	}
	if (result == -1)
	{
		if (Director_ScheduleRetry(playerid))
		{
			return 0;
		}
		g_DirRelSilent[playerid] = 1;
		g_DirRelReason[playerid] = 6;
		Director_SetOp(playerid, 4, "cr", contractId);
		Director_Send(playerid);
		Util_Send(playerid, -10270721, "Il servizio contratti non risponde: riprova tra poco.");
		return 0;
	}
	Director_ClearContract(playerid);
	switch (result)
	{
		case 2:
		{
			Util_Send(playerid, -10270721, "Hai gia' chiuso questo contratto.");
		}
		case 4:
		{
			Util_Send(playerid, -10270721, "Qualcun altro ha appena preso questo contratto.");
		}
		case 5:
		{
			Util_Send(playerid, -10270721, "Questo contratto e' scaduto.");
		}
		case 6:
		{
			Director_BlockMsg(playerid, 7);
		}
		case 7:
		{
			Util_Send(playerid, -10270721, "Non soddisfi ancora i requisiti di questo contratto (livello, reputazione o attesa tra contratti).");
		}
		case 10:
		{
			Util_Send(playerid, -10270721, "Un'operazione e' gia' in corso: attendi.");
		}
		default:
		{
			format(msg, 160, "Contratto non accettato (esito %d).", result);
			Util_Send(playerid, -10270721, msg);
		}
	}
	DB_LivingBoard(65535);
}

Director_AfterFreight(playerid, contractId, bool:terminal, relReason)
{
	if (gPlayer[playerid][pContractId] != contractId || gPlayer[playerid][pContractPendOp] != 0)
	{
		return 0;
	}
	if (terminal)
	{
		Director_Release(playerid, relReason);
	}
	else
	{
		Director_RunDeferred(playerid);
	}
}

Director_OnPickup(playerid, result)
{
	new ct = gPlayer[playerid][pContractId];
	if (result == -1 && Director_ScheduleRetry(playerid))
	{
		return 0;
	}
	gPlayer[playerid][pContractPendOp] = 0;
	if (result == 1 || result == 2)
	{
		gPlayer[playerid][pContractPhase] = 2;
		if (gPlayer[playerid][pFreightLegTick] == 0)
		{
			gPlayer[playerid][pFreightLegTick] = GetTickCount();
		}
		LD_Emit(102, gPlayer[playerid][pContractFromDistrict], gPlayer[playerid][pCharId], ct, gPlayer[playerid][pContractOpId]);
		Freight_OnPickupResult(playerid, 1);
		Director_AfterFreight(playerid, ct, 0, 0);
		return 0;
	}
	new bool:terminal = result != -1 && result != 10;
	Freight_OnPickupResult(playerid, result);
	Director_AfterFreight(playerid, ct, terminal, 5);
}

Director_OnComplete(playerid, result, amount, balance, v1, v2)
{
	new ct = gPlayer[playerid][pContractId], toD = gPlayer[playerid][pContractToDistrict], charId = gPlayer[playerid][pCharId];
	if (result == -1 && Director_ScheduleRetry(playerid))
	{
		return 0;
	}
	gPlayer[playerid][pContractPendOp] = 0;
	new bool:paid = result == 1 || result == 6;
	if (result == -1 || result == 10 || result == 4)
	{
		if (result == 4)
		{
			g_DirCompOp[playerid][0] = 0;
			g_DirCompCt[playerid] = 0;
		}
		Freight_OnCompleteResult(playerid, result, 0, 0);
		Director_AfterFreight(playerid, ct, 0, 0);
		return 0;
	}
	new op[41];
	strcat(op, gPlayer[playerid][pContractOpId], 41);
	if (paid || result == 8)
	{
		LD_SetProPoints(playerid, 1, v1);
	}
	if (paid && v2 > 0)
	{
		District_SetCache(toD, v2, -1, -1);
	}
	if (paid || result == 8 || result == 2)
	{
		Freight_OnCompleteResult(playerid, result, paid ? amount : 0, balance);
		Director_EndLocal(playerid, 0, 0, 0);
		gPlayer[playerid][pContractCooldownUntil] = GetTickCount() + 90000;
		if (paid)
		{
			District_Emit(1, toD, charId, amount, op);
		}
		else if (result == 8)
		{
			District_Emit(2, toD, charId, ct, op);
		}
		District_ReloadPlayer(playerid);
		return 0;
	}
	g_DirCompOp[playerid][0] = 0;
	g_DirCompCt[playerid] = 0;
	Freight_OnCompleteResult(playerid, result, 0, balance);
	Director_AfterFreight(playerid, ct, 1, 5);
}

Director_OnReleased(playerid, result, v2)
{
	if (result == -1 && Director_ScheduleRetry(playerid))
	{
		return 0;
	}
	if (result == 1 && v2 >= 0)
	{
		LD_SetProPoints(playerid, 1, v2);
	}
	new reason = g_DirRelReason[playerid], bool:silent = g_DirRelSilent[playerid];
	if (result == -1)
	{
		Log_Write("DIRECTOR", "rilascio contratto senza esito dal DB: lo completa la pulizia al prossimo login/boot");
	}
	if (!silent && result == 1)
	{
		if (reason == 1 || reason == 4)
		{
			Util_Send(playerid, -10270721, "Hai lasciato il contratto: nessuna perdita di denaro, ma la tua reputazione nei trasporti ne risente.");
		}
		else if (reason != 5)
		{
			Util_Send(playerid, -1, "Contratto interrotto senza penalita': e' tornato in bacheca.");
		}
	}
	Director_EndLocal(playerid, reason, 1, reason);
}

public Living_OnDBContract(playerid, action, result, contractId, amount, balance, v1, v2, v3)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][pContractId] == 0 || (contractId != 0 && contractId != gPlayer[playerid][pContractId]))
	{
		if (result != 10)
		{
			new msg[160];
			format(msg, 160, "esito contratto fuori sequenza: azione %d contratto %d esito %d", action, contractId, result);
			Log_Write("DIRECTOR", msg);
		}
		return 1;
	}
	switch (action)
	{
		case 1:
		{
			if (gPlayer[playerid][pContractPendOp] == 1)
			{
				Director_OnAccept(playerid, result, gPlayer[playerid][pContractId], v2);
			}
		}
		case 2:
		{
			if (gPlayer[playerid][pContractPendOp] == 2)
			{
				Director_OnPickup(playerid, result);
			}
		}
		case 4:
		{
			if (gPlayer[playerid][pContractPendOp] == 3)
			{
				Director_OnComplete(playerid, result, amount, balance, v1, v2);
			}
		}
		case 3:
		{
			if (gPlayer[playerid][pContractPendOp] == 4)
			{
				Director_OnReleased(playerid, result, v2);
			}
		}
	}
	return 1;
}
