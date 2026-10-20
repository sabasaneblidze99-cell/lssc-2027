new Float:s_HubBoard[3][3] = {
	{1850.0, -1800.0, 13.6},
	{2490.0, -1678.0, 13.5},
	{2133.0, -1215.0, 24.0}
};
new s_HubBoardDistrict[3] = {1, 2, 3};
new s_HubKitItem[3] = {3, 2, 6};
new s_HubKitQty[3] = {2, 1, 2};
new g_HubPickup[3] = {-1, -1, -1};
new Text3D:g_HubLabel[3] = {65535, 65535, 65535};
new g_HubZone = -1;
new g_HubTimer;
new g_HubTickCount;
new g_HubLastRefresh;
new bool:g_HubRefreshed;
new bool:g_HubFestInit[4];
new g_HubFestSeen[4];
new g_HubPrjSeen[4];
new g_HubGoalSeen[4];
new g_HubPrjNameSeen[4][65];
new g_HubFundPrj[4];
new g_HubFund[4];
new g_HubOnb[MAX_PLAYERS];
new g_HubStep[MAX_PLAYERS];
new g_HubAhead[MAX_PLAYERS];
new g_HubHintShown[MAX_PLAYERS];
new g_HubHintDue[MAX_PLAYERS];
new bool:g_HubOpBusy[MAX_PLAYERS];
new g_HubOpStep[MAX_PLAYERS];
new g_HubOpId[MAX_PLAYERS][37];
new g_HubOpRetryAt[MAX_PLAYERS];
new g_HubOpTries[MAX_PLAYERS];
new g_HubKitLeft[MAX_PLAYERS];
new g_HubKitAt[MAX_PLAYERS];
new g_HubKitTries[MAX_PLAYERS];
new bool:g_HubKitReload[MAX_PLAYERS];
new g_HubTarget[MAX_PLAYERS];
new bool:g_HubCpOn[MAX_PLAYERS];
new g_HubAtBoard[MAX_PLAYERS];
new g_HubGreeted[MAX_PLAYERS];
new g_HubDonDay[MAX_PLAYERS];
new g_HubDonToday[MAX_PLAYERS];
new g_HubPrjOpId[MAX_PLAYERS][37];
new g_HubPrjOpProject[MAX_PLAYERS];
new g_HubPrjOpAmount[MAX_PLAYERS];
new g_HubPrjDistrict[MAX_PLAYERS];

bool:Hub_Valid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

bool:Hub_PlayerOk(playerid)
{
	return Util_IsPlaying(playerid) && gPlayer[playerid][pCharId] > 0;
}

Hub_PlayerByChar(charId)
{
	if (charId <= 0)
	{
		return 65535;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && gPlayer[i][pCharId] == charId)
		{
			return i;
		}
	}
	return 65535;
}

Hub_Bit(n)
{
	return 1 << n;
}

Hub_CurStep(playerid)
{
	return g_HubStep[playerid] + 1;
}

bool:Hub_HintsOn(playerid)
{
	return gPlayer[playerid][pHintsOff] == 0;
}

Hub_ResetPlayer(playerid)
{
	if (!Hub_Valid(playerid))
	{
		return 0;
	}
	g_HubOnb[playerid] = 0;
	g_HubStep[playerid] = 0;
	g_HubAhead[playerid] = 0;
	g_HubHintShown[playerid] = 0;
	g_HubHintDue[playerid] = 0;
	g_HubOpBusy[playerid] = 0;
	g_HubOpStep[playerid] = 0;
	g_HubOpId[playerid][0] = 0;
	g_HubOpRetryAt[playerid] = 0;
	g_HubOpTries[playerid] = 0;
	g_HubKitLeft[playerid] = 0;
	g_HubKitAt[playerid] = 0;
	g_HubKitTries[playerid] = 0;
	g_HubKitReload[playerid] = 0;
	g_HubTarget[playerid] = 0;
	g_HubCpOn[playerid] = 0;
	g_HubAtBoard[playerid] = 0;
	g_HubGreeted[playerid] = 0;
	g_HubDonDay[playerid] = 0;
	g_HubDonToday[playerid] = 0;
	g_HubPrjOpId[playerid][0] = 0;
	g_HubPrjOpProject[playerid] = 0;
	g_HubPrjOpAmount[playerid] = 0;
	g_HubPrjDistrict[playerid] = 0;
}

Hub_NearBoard(playerid)
{
	if (!Util_IsPlaying(playerid) || gPlayer[playerid][pCharId] <= 0)
	{
		return -1;
	}
	if (GetPlayerState(playerid) == 9 || GetPlayerState(playerid) == 7)
	{
		return -1;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		return -1;
	}
	for (new b = 0; b < 3; b++)
	{
		if (IsPlayerInRangeOfPoint(playerid, 4.0, s_HubBoard[b][0], s_HubBoard[b][1], s_HubBoard[b][2]))
		{
			return b;
		}
	}
	return -1;
}

bool:Hub_TargetPos(target, &Float:x, &Float:y, &Float:z)
{
	switch (target)
	{
		case 1:
		{
			x = 1850.0;
			y = -1800.0;
			z = 13.6;
		}
		case 2:
		{
			x = 1480.5;
			y = -1736.0;
			z = 13.5;
		}
		case 3:
		{
			x = 1505.0;
			y = -1700.0;
			z = 13.5;
		}
		default:
		{
			return 0;
		}
	}
	return 1;
}

Hub_ClearTarget(playerid)
{
	if (!Hub_Valid(playerid))
	{
		return 0;
	}
	if (g_HubTarget[playerid] != 0 && IsPlayerConnected(playerid))
	{
		RemovePlayerMapIcon(playerid, 97);
	}
	if (g_HubCpOn[playerid] && IsPlayerConnected(playerid) && LD_ActiveFlow(playerid) == 0)
	{
		DisablePlayerCheckpoint(playerid);
	}
	g_HubCpOn[playerid] = 0;
	g_HubTarget[playerid] = 0;
}

Hub_SetTarget(playerid, target, bool:withCheckpoint)
{
	new Float:x, Float:y, Float:z;
	if (!Hub_TargetPos(target, x, y, z))
	{
		Hub_ClearTarget(playerid);
		return 0;
	}
	Hub_ClearTarget(playerid);
	SetPlayerMapIcon(playerid, 97, x, y, z, 0, -2994689, 1);
	g_HubTarget[playerid] = target;
	if (withCheckpoint && LD_ActiveFlow(playerid) == 0)
	{
		SetPlayerCheckpoint(playerid, x, y, z, 3.0);
		g_HubCpOn[playerid] = 1;
	}
}

Hub_StepTarget(step)
{
	switch (step)
	{
		case 1, 3:
		{
			return 1;
		}
		case 5:
		{
			return 2;
		}
	}
	return 0;
}

Hub_OnInit()
{
	new text[256], name[16];
	for (new b = 0; b < 3; b++)
	{
		g_HubPickup[b] = CreatePickup(1239, 1, s_HubBoard[b][0], s_HubBoard[b][1], s_HubBoard[b][2], 0);
		if (b == 0)
		{
			g_HubLabel[b] = Lang_Create3DTextLabel("Casa del Quartiere di Idlewood\nBacheca contratti - Progetto del quartiere\n/contratti  /contribuisci  /aiuto servizi", -2994689, s_HubBoard[b][0], s_HubBoard[b][1], (0.8 + s_HubBoard[b][2]), 15.0, 0, 0);
		}
		else
		{
			LD_DistrictName(s_HubBoardDistrict[b], name, 16);
			format(text, 256, "Bacheca di quartiere - %s\nUn pezzo di Casa del Quartiere anche qui\n/contratti  /contribuisci", name);
			g_HubLabel[b] = Lang_Create3DTextLabel(text, 0x99CCFFFF, s_HubBoard[b][0], s_HubBoard[b][1], (0.8 + s_HubBoard[b][2]), 15.0, 0, 0);
		}
	}
	g_HubZone = GangZoneCreate((1850.0 - 22.0), (-1800.0 - 22.0), (1850.0 + 22.0), (-1800.0 + 22.0));
	g_HubTickCount = 0;
	g_HubLastRefresh = 0;
	g_HubRefreshed = 0;
	for (new d = 0; d <= 3; d++)
	{
		g_HubFestInit[d] = 0;
		g_HubFestSeen[d] = 0;
		g_HubPrjSeen[d] = 0;
		g_HubGoalSeen[d] = 0;
		g_HubPrjNameSeen[d][0] = 0;
		g_HubFundPrj[d] = 0;
		g_HubFund[d] = 0;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Hub_ResetPlayer(i);
	}
	if (!LD_Listen("Hub_OnDistrictEvent", 40))
	{
		Log_Write("HUB", "registrazione del listener Hub_OnDistrictEvent non riuscita");
	}
	g_HubTimer = SetTimer("Hub_OnTick", 1000, 1);
}

Hub_OnExit()
{
	if (g_HubTimer != 0)
	{
		KillTimer(g_HubTimer);
	}
	g_HubTimer = 0;
	for (new b = 0; b < 3; b++)
	{
		if (g_HubPickup[b] != -1)
		{
			DestroyPickup(g_HubPickup[b]);
		}
		if (g_HubLabel[b] != 65535)
		{
			Delete3DTextLabel(g_HubLabel[b]);
		}
		g_HubPickup[b] = -1;
		g_HubLabel[b] = 65535;
	}
	if (g_HubZone != -1)
	{
		GangZoneDestroy(g_HubZone);
	}
	g_HubZone = -1;
}

Hub_OnPlayerConnect(playerid)
{
	if (!Hub_Valid(playerid))
	{
		return 0;
	}
	Hub_ResetPlayer(playerid);
}

Hub_OnCharReady(playerid)
{
	if (!Hub_Valid(playerid))
	{
		return 0;
	}
	Hub_ResetPlayer(playerid);
	if (g_HubZone != -1)
	{
		GangZoneShowForPlayer(playerid, g_HubZone, -2994880);
	}
	Hub_TryLoad(playerid);
}

Hub_OnPlayerDisconnect(playerid, reason)
{
	if (!Hub_Valid(playerid))
	{
		return 0;
	}
	Hub_ResetPlayer(playerid);
}

Hub_TryLoad(playerid)
{
	if (g_HubOnb[playerid] != 0 || DB_LivingPlayerGet(playerid, 0) == 0)
	{
		return 0;
	}
	new step = LD_Clamp(DB_LivingPlayerGet(playerid, 4), 0, 5);
	new bool:done = DB_LivingPlayerGet(playerid, 5) != 0 || step >= 5;
	g_HubStep[playerid] = step;
	gPlayer[playerid][pOnboardStep] = step;
	gPlayer[playerid][pHintsOff] = DB_LivingPlayerGet(playerid, 6) != 0 ? 1 : 0;
	if (done)
	{
		g_HubOnb[playerid] = 2;
		UI_LivingHelpSet(playerid, 6, !Hub_HintsOn(playerid));
		return 0;
	}
	g_HubOnb[playerid] = 1;
	UI_LivingHelpSet(playerid, Hub_CurStep(playerid), !Hub_HintsOn(playerid));
	if (!Hub_HintsOn(playerid))
	{
		return 0;
	}
	if (step == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Benvenuto a Los Santos. Qui nessuno arriva da solo: a due passi da Unity Station c'e' la Casa del Quartiere di Idlewood.");
		Util_Send(playerid, 0x99CCFFFF, "Se ti va, /aiuto ti accompagna nel primo giorno. Preferisci fare da te? /aiuto off, nessun problema.");
	}
	else
	{
		new msg[256];
		format(msg, 256, "Bentornato. Il tuo primo giorno riprende dal passo %d di %d: /aiuto per rivederlo.", Hub_CurStep(playerid), 5);
		Util_Send(playerid, 0x99CCFFFF, msg);
	}
	g_HubHintDue[playerid] = GetTickCount() + 8000;
	new target = Hub_StepTarget(Hub_CurStep(playerid));
	if (target != 0)
	{
		Hub_SetTarget(playerid, target, 0);
	}
}

Hub_SendStep(playerid, step, bool:fresh)
{
	if (fresh)
	{
		DB_LivingMakeOpId(g_HubOpId[playerid], 37, "ob", step, gPlayer[playerid][pCharId]);
		g_HubOpTries[playerid] = 0;
	}
	g_HubOpBusy[playerid] = 1;
	g_HubOpStep[playerid] = step;
	g_HubOpRetryAt[playerid] = 0;
	if (DB_LivingBusy(playerid, 4))
	{
		g_HubOpRetryAt[playerid] = GetTickCount() + 5000;
		return 0;
	}
	DB_LivingOnboardStep(playerid, step, g_HubOpId[playerid]);
}

Hub_Advance(playerid, step)
{
	if (g_HubOnb[playerid] != 1 || step < 1 || step > 5)
	{
		return 0;
	}
	new cur = Hub_CurStep(playerid);
	if (step < cur)
	{
		return 0;
	}
	if (step > cur || g_HubOpBusy[playerid])
	{
		g_HubAhead[playerid] |= Hub_Bit(step);
		return 0;
	}
	Hub_SendStep(playerid, step, 1);
}

Hub_StepBegin(playerid)
{
	new cur = Hub_CurStep(playerid);
	gPlayer[playerid][pOnboardStep] = g_HubStep[playerid];
	UI_LivingHelpSet(playerid, cur, !Hub_HintsOn(playerid));
	UI_LivingHintHide(playerid);
	if (!Hub_HintsOn(playerid))
	{
		Hub_ClearTarget(playerid);
		return 0;
	}
	if (g_HubHintDue[playerid] == 0)
	{
		g_HubHintDue[playerid] = GetTickCount() + 3000;
	}
	new target = Hub_StepTarget(cur);
	if (target == 0)
	{
		Hub_ClearTarget(playerid);
	}
	else if (target != g_HubTarget[playerid])
	{
		Hub_SetTarget(playerid, target, 0);
	}
}

Hub_Finish(playerid, bool:rewarded)
{
	g_HubOnb[playerid] = 2;
	g_HubAhead[playerid] = 0;
	g_HubHintDue[playerid] = 0;
	Hub_ClearTarget(playerid);
	UI_LivingHelpComplete(playerid, rewarded);
	UI_LivingHelpSet(playerid, 6, !Hub_HintsOn(playerid));
}

Hub_StepMessage(playerid, step, paid)
{
	new msg[256];
	switch (step)
	{
		case 1:
		{
			UI_LivingNotify(playerid, "Sei alla Casa del Quartiere: qui la citta' ti chiama per nome. Bacheca, notizie, una mano quando serve.", 1);
		}
		case 2:
		{
			UI_LivingNotify(playerid, "Ora sai come respira Idlewood. Quello che facciamo, lo cambia per tutti.", 1);
		}
		case 3:
		{
			if (paid > 0)
			{
				format(msg, 256, "Consegna di benvenuto fatta: il negozio ringrazia. La Casa del Quartiere ti ha versato $%d sul conto.", paid);
			}
			else
			{
				format(msg, 256, "Consegna di benvenuto fatta: il negozio ringrazia.");
			}
			UI_LivingNotify(playerid, msg, 1);
		}
		case 4:
		{
			UI_LivingNotify(playerid, "La citta' comincia a conoscerti. Ogni lavoro fatto bene lascia un segno.", 1);
		}
	}
}

Hub_KitTick(playerid, now)
{
	if (g_HubKitLeft[playerid] == 0)
	{
		if (g_HubKitReload[playerid] && now - g_HubKitAt[playerid] >= 0 && gPlayer[playerid][pInvOpBusy] == 0)
		{
			g_HubKitReload[playerid] = 0;
			Inv_Reload(playerid);
		}
		return 0;
	}
	if (now - g_HubKitAt[playerid] < 0)
	{
		return 0;
	}
	g_HubKitAt[playerid] = now + 1500;
	if (!Inv_Ready(playerid) || gPlayer[playerid][pInvOpBusy] != 0)
	{
		if (++g_HubKitTries[playerid] >= 40)
		{
			new msg[256];
			format(msg, 256, "pacco accoglienza non consegnato al personaggio %d (inventario non pronto)", gPlayer[playerid][pCharId]);
			Log_Write("HUB", msg);
			g_HubKitLeft[playerid] = 0;
		}
		return 0;
	}
	for (new i = 0; i < 3; i++)
	{
		if (!(g_HubKitLeft[playerid] & Hub_Bit(i)))
		{
			continue;
		}
		g_HubKitLeft[playerid] &= ~Hub_Bit(i);
		if (s_HubKitQty[i] <= 0)
		{
			continue;
		}
		new opId[33];
		format(opId, 33, "obkit%d-%d", gPlayer[playerid][pCharId], i);
		DB_InvAdd(playerid, gPlayer[playerid][pCharId], s_HubKitItem[i], s_HubKitQty[i], opId, "add", 0);
		break;
	}
	if (g_HubKitLeft[playerid] == 0)
	{
		g_HubKitReload[playerid] = 1;
	}
}

public Living_OnDBOnboard(playerid, result, step, paid, balance, giveKit, exp)
{
	if (!Hub_Valid(playerid) || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	new sent = g_HubOpStep[playerid], now = GetTickCount();
	switch (result)
	{
		case 1:
		{
			g_HubOpBusy[playerid] = 0;
			g_HubOpId[playerid][0] = 0;
			if (sent == 0)
			{
				Hub_Finish(playerid, 0);
				return 1;
			}
			g_HubStep[playerid] = LD_Clamp(step, 0, 5);
			g_HubAhead[playerid] &= ~Hub_Bit(sent);
			if (giveKit != 0)
			{
				g_HubKitLeft[playerid] = 7;
				g_HubKitAt[playerid] = now;
				g_HubKitTries[playerid] = 0;
				if (exp > 0)
				{
					Level_AddExp(playerid, exp);
					Char_Save(playerid);
				}
			}
			if (g_HubStep[playerid] >= 5)
			{
				Hub_Finish(playerid, giveKit != 0);
				return 1;
			}
			Hub_StepMessage(playerid, sent, paid);
			Hub_StepBegin(playerid);
		}
		case 2, 4:
		{
			g_HubOpBusy[playerid] = 0;
			g_HubOpId[playerid][0] = 0;
			new dbStep = LD_Clamp(step, 0, 5);
			if (result == 2 && (sent == 0 || sent > dbStep || dbStep >= 5))
			{
				g_HubStep[playerid] = dbStep;
				g_HubOnb[playerid] = 2;
				g_HubAhead[playerid] = 0;
				Hub_ClearTarget(playerid);
				UI_LivingHelpSet(playerid, 6, !Hub_HintsOn(playerid));
				return 1;
			}
			g_HubStep[playerid] = dbStep;
			Hub_StepBegin(playerid);
		}
		case -1, 10:
		{
			g_HubOpTries[playerid]++;
			g_HubOpRetryAt[playerid] = now + (g_HubOpTries[playerid] < 5 ? 5000 : 30000);
		}
		default:
		{
			g_HubOpBusy[playerid] = 0;
			g_HubOpId[playerid][0] = 0;
			new msg[256];
			format(msg, 256, "onboarding: esito %d per il passo %d del personaggio %d", result, sent, gPlayer[playerid][pCharId]);
			Log_Write("HUB", msg);
		}
	}
	return 1;
}

Hub_HintsOff(playerid)
{
	switch (g_HubOnb[playerid])
	{
		case 0:
		{
			Util_Send(playerid, -10270721, "Sto ancora recuperando il tuo percorso: riprova tra un attimo.");
		}
		case 2:
		{
			if (Hub_HintsOn(playerid))
			{
				DB_LivingOnboardHints(playerid, 1);
			}
			gPlayer[playerid][pHintsOff] = 1;
			UI_LivingHintHide(playerid);
			Util_Send(playerid, 0x99CCFFFF, "Suggerimenti spenti. La Casa del Quartiere di Idlewood resta aperta, se ti serve.");
		}
		default:
		{
			if (g_HubOpBusy[playerid])
			{
				Util_Send(playerid, -10270721, "Un attimo: sto registrando il tuo ultimo passo. Riprova tra poco.");
				return 0;
			}
			gPlayer[playerid][pHintsOff] = 1;
			g_HubHintDue[playerid] = 0;
			UI_LivingHintHide(playerid);
			Hub_ClearTarget(playerid);
			Hub_SendStep(playerid, 0, 1);
		}
	}
}

Hub_StartWelcome(playerid)
{
	if (g_HubOnb[playerid] != 1 || Hub_CurStep(playerid) != 3)
	{
		if (g_HubOnb[playerid] == 1 && Hub_CurStep(playerid) < 3)
		{
			Util_Send(playerid, 0x99CCFFFF, "La consegna di benvenuto arriva tra poco: prima finisci il passo in corso (/aiuto).");
		}
		else
		{
			Util_Send(playerid, 0x99CCFFFF, "La consegna di benvenuto e' per il primo giorno. Per altri incarichi c'e' la bacheca: /contratti.");
		}
		return 0;
	}
	if (gPlayer[playerid][pFreightWelcome] != 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "La consegna di benvenuto e' gia' in corso: il magazzino di Idlewood ti aspetta.");
		return 0;
	}
	if (Hub_NearBoard(playerid) == -1)
	{
		Util_Send(playerid, 0x99CCFFFF, "La consegna si prende a una bacheca: ti ho segnato la Casa del Quartiere di Idlewood sulla mappa.");
		Hub_SetTarget(playerid, 1, 1);
		return 0;
	}
	new block = LD_CheckStart(playerid);
	if (block != 0)
	{
		LD_SendBlock(playerid, block);
		return 0;
	}
	Hub_ClearTarget(playerid);
	if (!Freight_Begin(playerid, 0, 1, 1, 1, 0, 1))
	{
		Util_Send(playerid, -10270721, "Il furgone della consegna di benvenuto non e' disponibile proprio ora: riprova tra poco.");
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "Un carico di merci dal magazzino al negozio di Idlewood: strada breve, nessuna fretta. Il quartiere ti ringrazia gia'.");
}

Hub_OnWelcomeDelivered(playerid, bool:success)
{
	if (!Hub_Valid(playerid) || !IsPlayerConnected(playerid) || g_HubOnb[playerid] != 1)
	{
		return 0;
	}
	if (success)
	{
		Hub_Advance(playerid, 3);
		return 1;
	}
	Util_Send(playerid, 0x99CCFFFF, "Pazienza, capita a tutti. La consegna di benvenuto resta li': quando vuoi, /aiuto consegna a una bacheca.");
	Hub_StepBegin(playerid);
	return 1;
}

Hub_ShowServices(playerid)
{
	new msg[256];
	Util_Send(playerid, 0x99CCFFFF, "Casa del Quartiere di Idlewood - cosa trovi qui (e alle bacheche di Ganton e Jefferson):");
	Util_Send(playerid, -1, " /contratti - la bacheca: si consulta ovunque, si accetta qui o a una bacheca di quartiere");
	Util_Send(playerid, -1, " /quartiere [nome] - come sta il quartiere e cosa si dice in strada");
	format(msg, 256, " /contribuisci [importo] [quartiere] - il progetto della comunita': da $%d a $%d, max $%d al giorno, dal conto", 50, 500, 1000);
	Util_Send(playerid, -1, msg);
	Util_Send(playerid, -1, " /documento richiedi - lo sportello documenti");
	Util_Send(playerid, -1, " /aiuto lavoro - la strada per il Job Center di Pershing Square  |  /aiuto noleggio - il noleggio mezzi");
}

Hub_CmdAiuto(playerid, params[])
{
	if (!Hub_PlayerOk(playerid))
	{
		return 1;
	}
	new pos = 0, sub[24];
	Util_NextToken(params, pos, sub, 24);
	if (sub[0] == 0)
	{
		if (g_HubOnb[playerid] == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "Sto recuperando il tuo percorso: riprova tra un attimo. Intanto: /aiuto servizi.");
			return 1;
		}
		UI_LivingHelpSet(playerid, g_HubOnb[playerid] == 2 ? 6 : Hub_CurStep(playerid), !Hub_HintsOn(playerid));
		UI_LivingHelpShow(playerid);
		return 1;
	}
	if (!strcmp(sub, "off", 1, 0x7FFFFFFF))
	{
		Hub_HintsOff(playerid);
		return 1;
	}
	if (!strcmp(sub, "servizi", 1, 0x7FFFFFFF) || !strcmp(sub, "hub", 1, 0x7FFFFFFF))
	{
		Hub_ShowServices(playerid);
		return 1;
	}
	if (!strcmp(sub, "consegna", 1, 0x7FFFFFFF))
	{
		Hub_StartWelcome(playerid);
		return 1;
	}
	if (!strcmp(sub, "mappa", 1, 0x7FFFFFFF))
	{
		if (g_HubOnb[playerid] == 1)
		{
			UI_OnHelpWaypoint(playerid, Hub_CurStep(playerid));
		}
		else
		{
			Util_Send(playerid, 0x99CCFFFF, "Nessun passo da segnare. Per la Casa del Quartiere: /aiuto casa.");
		}
		return 1;
	}
	if (!strcmp(sub, "casa", 1, 0x7FFFFFFF))
	{
		Hub_SetTarget(playerid, 1, 1);
		Util_Send(playerid, 0x99CCFFFF, "Ti ho segnato la Casa del Quartiere di Idlewood sulla mappa.");
		return 1;
	}
	if (!strcmp(sub, "lavoro", 1, 0x7FFFFFFF))
	{
		Hub_SetTarget(playerid, 2, 1);
		Util_Send(playerid, 0x99CCFFFF, "Il Job Center e' a Pershing Square, vicino al municipio: te l'ho segnato sulla mappa. Li', /lavoro prendi.");
		return 1;
	}
	if (!strcmp(sub, "noleggio", 1, 0x7FFFFFFF))
	{
		Hub_SetTarget(playerid, 3, 1);
		Util_Send(playerid, 0x99CCFFFF, "Il noleggio mezzi e' a Pershing Square: te l'ho segnato sulla mappa. Li', /noleggio.");
		return 1;
	}
	Util_Send(playerid, 0x99CCFFFF, "Uso: /aiuto [servizi|mappa|consegna|casa|lavoro|noleggio|off]");
	return 1;
}

public UI_OnHelpWaypoint(playerid, step)
{
	if (!Hub_PlayerOk(playerid))
	{
		return 1;
	}
	if (g_HubOnb[playerid] != 1 || step != Hub_CurStep(playerid))
	{
		Util_Send(playerid, 0x99CCFFFF, "Questo passo non e' quello in corso: /aiuto per vedere a che punto sei.");
		return 1;
	}
	switch (step)
	{
		case 1:
		{
			Hub_SetTarget(playerid, 1, 1);
			Util_Send(playerid, 0x99CCFFFF, "Segui il segnale: la Casa del Quartiere e' a due passi da Unity Station.");
		}
		case 3:
		{
			if (Hub_NearBoard(playerid) != -1)
			{
				Util_Send(playerid, 0x99CCFFFF, "Sei gia' a una bacheca: /aiuto consegna e si parte.");
			}
			else
			{
				Hub_SetTarget(playerid, 1, 1);
				Util_Send(playerid, 0x99CCFFFF, "Ti ho segnato la Casa del Quartiere: li', /aiuto consegna.");
			}
		}
		case 5:
		{
			Hub_SetTarget(playerid, 2, 1);
			Util_Send(playerid, 0x99CCFFFF, "Ti ho segnato il Job Center di Pershing Square. Puoi anche solo scrivere /lavoro o /noleggio da qui.");
		}
		default:
		{
			Util_Send(playerid, 0x99CCFFFF, "Non serve andare lontano: il comando funziona ovunque tu sia (/aiuto per i dettagli).");
		}
	}
	return 1;
}

public UI_OnHelpHintsOff(playerid)
{
	if (Hub_PlayerOk(playerid))
	{
		Hub_HintsOff(playerid);
	}
	return 1;
}

Hub_DayLeft(playerid)
{
	new day = gettime() / 86400;
	if (g_HubDonDay[playerid] != day)
	{
		g_HubDonDay[playerid] = day;
		g_HubDonToday[playerid] = 0;
	}
	new left = 1000 - g_HubDonToday[playerid];
	return left > 0 ? left : 0;
}

Hub_ProjectFund(districtId)
{
	new prj = DB_LivingDistrictGet(districtId, 6), fund = DB_LivingDistrictGet(districtId, 8);
	if (prj > 0 && g_HubFundPrj[districtId] == prj && g_HubFund[districtId] > fund)
	{
		fund = g_HubFund[districtId];
	}
	return fund;
}

bool:Hub_ProjectOpen(districtId)
{
	return DB_LivingDistrictGet(districtId, 6) > 0 && DB_LivingDistrictGet(districtId, 9) > gettime();
}

Hub_ProjectBlock(playerid, districtId)
{
	if (!Hub_ProjectOpen(districtId))
	{
		return 3;
	}
	if (Hub_NearBoard(playerid) == -1)
	{
		return 4;
	}
	if (gPlayer[playerid][pPlayedMs] / 60000 < 60)
	{
		return 1;
	}
	if (Hub_DayLeft(playerid) < 50)
	{
		return 2;
	}
	return 0;
}

Hub_ProjectFill(playerid, districtId)
{
	new name[65], prj = DB_LivingDistrictGet(districtId, 6), hours = 0;
	DB_LivingProjectName(districtId, name, 65);
	new left = DB_LivingDistrictGet(districtId, 9) - gettime();
	if (left > 0)
	{
		hours = (left + 3600 - 1) / 3600;
	}
	UI_LivingProjectSet(playerid, Hub_ProjectOpen(districtId) ? prj : 0, districtId, name, DB_LivingDistrictGet(districtId, 7), Hub_ProjectFund(districtId), hours, 50, 500, Hub_DayLeft(playerid), Hub_ProjectBlock(playerid, districtId));
}

Hub_DistrictOfProject(projectId)
{
	if (projectId <= 0)
	{
		return 0;
	}
	for (new d = 1; d <= 3; d++)
	{
		if (DB_LivingDistrictGet(d, 6) == projectId)
		{
			return d;
		}
	}
	return 0;
}

Hub_CmdContribuisci(playerid, params[])
{
	if (!Hub_PlayerOk(playerid))
	{
		return 1;
	}
	new pos = 0, tok[24], amount = 0, districtId = 0;
	Util_NextToken(params, pos, tok, 24);
	if (tok[0] != 0)
	{
		if (Util_ParseInt(tok, amount))
		{
			if (amount <= 0)
			{
				Util_Send(playerid, -10270721, "Importo non valido: scrivi un numero intero positivo (es. /contribuisci 100).");
				return 1;
			}
			Util_NextToken(params, pos, tok, 24);
			if (tok[0] != 0)
			{
				districtId = LD_ParseDistrict(tok);
			}
		}
		else
		{
			districtId = LD_ParseDistrict(tok);
		}
		if (tok[0] != 0 && districtId == 0)
		{
			Util_Send(playerid, -10270721, "Quartiere sconosciuto: Idlewood, Ganton o Jefferson.");
			return 1;
		}
	}
	if (districtId == 0)
	{
		new b = Hub_NearBoard(playerid);
		districtId = b != -1 ? s_HubBoardDistrict[b] : 1;
	}
	Hub_ProjectFill(playerid, districtId);
	if (amount <= 0 || Hub_ProjectBlock(playerid, districtId) != 0)
	{
		UI_LivingProjectShow(playerid);
	}
	else
	{
		UI_LivingProjectContribute(playerid, amount);
	}
	return 1;
}

public UI_OnProjectOpen(playerid, districtId)
{
	if (!Hub_PlayerOk(playerid) || districtId < 1 || districtId > 3)
	{
		return 1;
	}
	Hub_ProjectFill(playerid, districtId);
	UI_LivingProjectShow(playerid);
	return 1;
}

public UI_OnProjectContribute(playerid, projectId, amount)
{
	if (!Hub_PlayerOk(playerid))
	{
		return 1;
	}
	if (!LD_IsReady())
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new districtId = Hub_DistrictOfProject(projectId);
	if (districtId == 0 || !Hub_ProjectOpen(districtId))
	{
		UI_LivingReject(playerid, "Questo progetto non raccoglie piu' fondi: ne partira' presto un altro.");
		return 1;
	}
	new msg[256];
	switch (Hub_ProjectBlock(playerid, districtId))
	{
		case 4:
		{
			UI_LivingReject(playerid, "Le donazioni si fanno alla Casa del Quartiere di Idlewood o a una bacheca di quartiere.");
			return 1;
		}
		case 1:
		{
			format(msg, 256, "Potrai donare dopo almeno %d minuti trascorsi in citta'. Intanto, grazie del pensiero.", 60);
			UI_LivingReject(playerid, msg);
			return 1;
		}
		case 2:
		{
			UI_LivingReject(playerid, "Per oggi hai gia' dato il massimo: grazie davvero.");
			return 1;
		}
	}
	if (!LD_DonationValid(amount, g_HubDonToday[playerid]))
	{
		format(msg, 256, "Importo non valido: da $%d a $%d, al massimo $%d al giorno.", 50, 500, 1000);
		UI_LivingReject(playerid, msg);
		return 1;
	}
	if (gPlayer[playerid][pDonationPending] != 0 || DB_LivingBusy(playerid, 3))
	{
		UI_LivingReject(playerid, "Un attimo: la tua donazione precedente e' ancora in registrazione.");
		return 1;
	}
	if (gPlayer[playerid][pBank] < amount)
	{
		UI_LivingReject(playerid, "Le donazioni partono dal conto in banca e li' non c'e' abbastanza. Nessun problema: ogni contratto aiuta il fondo.");
		return 1;
	}
	if (g_HubPrjOpId[playerid][0] == 0 || g_HubPrjOpProject[playerid] != projectId || g_HubPrjOpAmount[playerid] != amount)
	{
		DB_LivingMakeOpId(g_HubPrjOpId[playerid], 37, "pj", projectId, gPlayer[playerid][pCharId]);
		g_HubPrjOpProject[playerid] = projectId;
		g_HubPrjOpAmount[playerid] = amount;
	}
	gPlayer[playerid][pDonationPending] = 9;
	g_HubPrjDistrict[playerid] = districtId;
	DB_LivingProjectContrib(playerid, projectId, amount, g_HubPrjOpId[playerid]);
	return 1;
}

public Living_OnDBProject(playerid, result, projectId, donated, balance, points, fund, projectState)
{
	if (!Hub_Valid(playerid) || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	gPlayer[playerid][pDonationPending] = 0;
	new msg[256], name[65], dname[16], d = g_HubPrjDistrict[playerid];
	if (d < 1 || d > 3)
	{
		d = Hub_DistrictOfProject(projectId);
	}
	if (result != -1)
	{
		g_HubPrjOpId[playerid][0] = 0;
		g_HubPrjOpProject[playerid] = 0;
		g_HubPrjOpAmount[playerid] = 0;
	}
	switch (result)
	{
		case 1:
		{
			Hub_DayLeft(playerid);
			g_HubDonToday[playerid] += donated;
			DB_LivingProjectName(d, name, 65);
			LD_DistrictName(d, dname, 16);
			if (d >= 1 && d <= 3)
			{
				g_HubFundPrj[d] = projectId;
				g_HubFund[d] = fund;
			}
			format(msg, 256, "Grazie! Hai donato $%d per \"%s\": raccolti $%d su $%d.", donated, name, fund, DB_LivingDistrictGet(d, 7));
			UI_LivingNotify(playerid, msg, 1);
			if (points > 0)
			{
				format(msg, 256, "%s se ne ricordera': reputazione locale +%d.", dname, points);
				Util_Send(playerid, 0x66DD66FF, msg);
			}
			if (projectState == 2)
			{
				Util_Send(playerid, 0x66DD66FF, "E' stata la tua donazione a completare il progetto. Il quartiere fa festa!");
				Hub_RequestRefresh(1);
			}
			format(msg, 256, "personaggio %d dona %d al progetto %d (fondo %d, stato %d)", gPlayer[playerid][pCharId], donated, projectId, fund, projectState);
			Log_Write("HUB", msg);
		}
		case 2:
		{
			UI_LivingReject(playerid, "Questa donazione risulta gia' registrata.");
		}
		case 3:
		{
			UI_LivingReject(playerid, "Le donazioni partono dal conto in banca e li' non c'e' abbastanza.");
		}
		case 4, 5:
		{
			UI_LivingReject(playerid, "Questo progetto non raccoglie piu' fondi: ne partira' presto un altro.");
			Hub_RequestRefresh(1);
		}
		case 7:
		{
			format(msg, 256, "Per ora non si puo': servono almeno %d minuti in citta' e al massimo $%d donati al giorno.", 60, 1000);
			UI_LivingReject(playerid, msg);
		}
		case 9:
		{
			format(msg, 256, "Importo non valido: da $%d a $%d.", 50, 500);
			UI_LivingReject(playerid, msg);
		}
		case 10:
		{
			UI_LivingReject(playerid, "Un attimo: c'e' gia' una donazione in registrazione.");
		}
		default:
		{
			UI_LivingReject(playerid, "Il registro della Casa del Quartiere non ha risposto. Riprova con lo stesso importo: non verra' addebitato due volte.");
		}
	}
	return 1;
}

Hub_RequestRefresh(bool:force)
{
	if (!DB_IsReady())
	{
		return 0;
	}
	new now = GetTickCount();
	if (!force && g_HubRefreshed && now - g_HubLastRefresh < 60000)
	{
		return 0;
	}
	g_HubRefreshed = 1;
	g_HubLastRefresh = now;
	DB_LivingLoadDistricts();
}

Hub_Broadcast(color, const text[])
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (Util_IsPlaying(i))
		{
			Util_Send(i, color, text);
		}
	}
}

Hub_CheckFestivals()
{
	new msg[256], dname[16], opId[37];
	for (new d = 1; d <= 3; d++)
	{
		if (DB_LivingDistrictGet(d, 0) == 0)
		{
			continue;
		}
		new fest = DB_LivingDistrictGet(d, 4), prj = DB_LivingDistrictGet(d, 6);
		if (!g_HubFestInit[d])
		{
			g_HubFestInit[d] = 1;
			g_HubFestSeen[d] = fest;
			g_HubPrjSeen[d] = prj;
			g_HubGoalSeen[d] = DB_LivingDistrictGet(d, 7);
			DB_LivingProjectName(d, g_HubPrjNameSeen[d], 65);
			continue;
		}
		if (fest == g_HubFestSeen[d] && prj == g_HubPrjSeen[d])
		{
			continue;
		}
		LD_DistrictName(d, dname, 16);
		if (fest > g_HubFestSeen[d] && g_HubPrjSeen[d] > 0)
		{
			format(msg, 256, "%s ce l'ha fatta: \"%s\" e' completato grazie a chi ci lavora e a chi ha donato. Festa di quartiere per %d ore!", dname, g_HubPrjNameSeen[d], 72);
			Hub_Broadcast(0x66DD66FF, msg);
			format(opId, 37, "pj%d-done", g_HubPrjSeen[d]);
			LD_Emit(8, d, 0, g_HubGoalSeen[d], opId);
		}
		else if (prj > 0 && prj != g_HubPrjSeen[d])
		{
			new name[65];
			DB_LivingProjectName(d, name, 65);
			format(msg, 256, "A %s parte un nuovo progetto della comunita': \"%s\". Ogni contratto da e per il quartiere lo aiuta.", dname, name);
			Hub_Broadcast(0x99CCFFFF, msg);
		}
		g_HubFestSeen[d] = fest;
		g_HubPrjSeen[d] = prj;
		g_HubGoalSeen[d] = DB_LivingDistrictGet(d, 7);
		DB_LivingProjectName(d, g_HubPrjNameSeen[d], 65);
	}
}

public Hub_OnDistrictEvent(type, districtId, charId, value, const opId[])
{
	switch (type)
	{
		case 106:
		{
			if (value != 2 && value != 4 && value != 5)
			{
				return 1;
			}
			new playerid = Hub_PlayerByChar(charId);
			if (playerid != INVALID_PLAYER_ID)
			{
				Hub_Advance(playerid, value);
			}
		}
		case 108:
		{
			new playerid = Hub_PlayerByChar(charId);
			if (playerid != INVALID_PLAYER_ID)
			{
				Hub_TryLoad(playerid);
			}
		}
		case 105:
		{
			Hub_CheckFestivals();
		}
	}
	return 1;
}

Hub_GreetBoard(playerid, board)
{
	if (g_HubGreeted[playerid] & Hub_Bit(board))
	{
		return 0;
	}
	g_HubGreeted[playerid] |= Hub_Bit(board);
	if (board == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Casa del Quartiere di Idlewood: bacheca (/contratti), progetto del quartiere (/contribuisci), notizie (/quartiere).");
		Util_Send(playerid, 0x99CCFFFF, "Qui si ricomincia, si da' una mano, ci si conosce. Tutti i servizi: /aiuto servizi.");
		return 0;
	}
	new msg[256], dname[16];
	LD_DistrictName(s_HubBoardDistrict[board], dname, 16);
	format(msg, 256, "Bacheca di quartiere di %s: qui accetti i contratti (/contratti) e sostieni il progetto della zona (/contribuisci).", dname);
	Util_Send(playerid, 0x99CCFFFF, msg);
}

Hub_PlayerTick(playerid, now)
{
	if (g_HubOnb[playerid] == 0)
	{
		Hub_TryLoad(playerid);
	}
	new board = Hub_NearBoard(playerid);
	if (board + 1 != g_HubAtBoard[playerid])
	{
		g_HubAtBoard[playerid] = board + 1;
		if (board != -1)
		{
			Hub_GreetBoard(playerid, board);
			if (g_HubOnb[playerid] == 1 && Hub_CurStep(playerid) == 3 && gPlayer[playerid][pFreightWelcome] == 0)
			{
				Util_Send(playerid, 0x99CCFFFF, "C'e' una consegna di benvenuto che ti aspetta: /aiuto consegna.");
			}
		}
	}
	if (g_HubTarget[playerid] != 0)
	{
		if (g_HubCpOn[playerid] && LD_ActiveFlow(playerid) != 0)
		{
			g_HubCpOn[playerid] = 0;
		}
		new Float:x, Float:y, Float:z;
		if (Hub_TargetPos(g_HubTarget[playerid], x, y, z) && IsPlayerInRangeOfPoint(playerid, 6.0, x, y, z))
		{
			new target = g_HubTarget[playerid];
			Hub_ClearTarget(playerid);
			if (target == 2)
			{
				Util_Send(playerid, 0x99CCFFFF, "Eccoti al Job Center: /lavoro per vedere cosa offre la citta'.");
			}
			else if (target == 3)
			{
				Util_Send(playerid, 0x99CCFFFF, "Eccoti al noleggio: /noleggio per scegliere un mezzo.");
			}
		}
	}
	if (g_HubOnb[playerid] == 1)
	{
		new cur = Hub_CurStep(playerid);
		if (g_HubOpBusy[playerid])
		{
			if (g_HubOpRetryAt[playerid] != 0 && now - g_HubOpRetryAt[playerid] >= 0)
			{
				Hub_SendStep(playerid, g_HubOpStep[playerid], 0);
			}
		}
		else if (cur == 1 && board == 0)
		{
			Hub_Advance(playerid, 1);
		}
		else if (g_HubAhead[playerid] & Hub_Bit(cur))
		{
			Hub_Advance(playerid, cur);
		}
		if (Hub_HintsOn(playerid) && g_HubHintDue[playerid] != 0 && now - g_HubHintDue[playerid] >= 0 && !g_HubOpBusy[playerid])
		{
			new last = gPlayer[playerid][pLastHintTick];
			if (g_HubHintShown[playerid] & Hub_Bit(cur))
			{
				g_HubHintDue[playerid] = 0;
			}
			else if (last == 0 || now - last >= 90000)
			{
				UI_LivingHint(playerid, cur);
				gPlayer[playerid][pLastHintTick] = now != 0 ? now : 1;
				g_HubHintShown[playerid] |= Hub_Bit(cur);
				g_HubHintDue[playerid] = 0;
			}
		}
	}
	Hub_KitTick(playerid, now);
}

public Hub_OnTick()
{
	if (!LD_IsReady())
	{
		return 1;
	}
	new now = GetTickCount();
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!Util_IsPlaying(i) || gPlayer[i][pLivingReady] == 0 || gPlayer[i][pCharId] <= 0)
		{
			continue;
		}
		Hub_PlayerTick(i, now);
	}
	if (++g_HubTickCount >= 5)
	{
		g_HubTickCount = 0;
		Hub_CheckFestivals();
	}
	return 1;
}
