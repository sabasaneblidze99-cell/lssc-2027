new g_FacShort[3][8] = {
	"-",
	"LSPD",
	"EMS"
};
new g_FacRankName[3][7][24] = {
	{"-", "-", "-", "-", "-", "-", "-"},
	{"-", "Cadetto", "Agente", "Agente Scelto", "Sergente", "Tenente", "Capo del Dipartimento"},
	{"-", "Volontario", "Soccorritore", "Paramedico", "Caposquadra", "Medico", "Direttore Sanitario"}
};
new g_FacRankPay[3][7] = {
	{0},
	{0, 150, 200, 250, 320, 400, 500},
	{0, 150, 200, 250, 320, 400, 500}
};
new g_FacRankSkin[3][7] = {
	{0},
	{0, 280, 280, 280, 280, 283, 288},
	{0, 274, 274, 274, 275, 276, 70}
};
new g_FacSkinFemale[3] = {0, 306, 308};
new Float:g_FacHq[3][3] = {
	{0},
	{1554.0, -1675.5, 16.2},
	{1177.0, -1323.5, 14.0}
};
new bool:g_FcCmdSet[MAX_PLAYERS];
new g_FcCmdLast[MAX_PLAYERS];
new bool:g_FcDutySet[MAX_PLAYERS];
new g_FcDutyLast[MAX_PLAYERS];
new bool:g_FcRadioSet[MAX_PLAYERS];
new g_FcRadioLast[MAX_PLAYERS];
new bool:g_FcCuffSet[MAX_PLAYERS];
new g_FcCuffLast[MAX_PLAYERS];
new bool:g_FcHealSet[MAX_PLAYERS];
new g_FcHealLast[MAX_PLAYERS];
new bool:g_FcHealTgtSet[MAX_PLAYERS];
new g_FcHealTgtLast[MAX_PLAYERS];
new Float:g_FcPrevArmor[MAX_PLAYERS];
new g_FcPayMs[MAX_PLAYERS];
new g_FcStillMs[MAX_PLAYERS];
new Float:g_FcLastPos[MAX_PLAYERS][3];
new g_FcKit[MAX_PLAYERS];
new bool:g_FcKitSet[MAX_PLAYERS];
new g_FcKitRefillLast[MAX_PLAYERS];
new g_FcCuffBy[MAX_PLAYERS];
new g_FcCuffTick[MAX_PLAYERS];
new g_FcInjLeftMs[MAX_PLAYERS];
new g_FcInjSec[MAX_PLAYERS];
new bool:g_FcHospPending[MAX_PLAYERS];
new g_FcRevTarget[MAX_PLAYERS];
new g_FcRevTargetChar[MAX_PLAYERS];
new g_FcRevStart[MAX_PLAYERS];
new g_FcReviveBucket[MAX_PLAYERS];
new g_FcReviveCount[MAX_PLAYERS];
new bool:g_FcInvActive[MAX_PLAYERS];
new g_FcInvFrom[MAX_PLAYERS];
new g_FcInvFromChar[MAX_PLAYERS];
new g_FcInvFac[MAX_PLAYERS];
new g_FcInvStart[MAX_PLAYERS];
new bool:g_FcJoinPending[MAX_PLAYERS];
new bool:g_FcFineBusy[MAX_PLAYERS];
new g_FcFineTarget[MAX_PLAYERS];
new g_FcFineReason[MAX_PLAYERS][65];
new bool:g_FcFinePend[MAX_PLAYERS];
new bool:g_FcFineTgtSet[MAX_PLAYERS];
new g_FcFineTgtLast[MAX_PLAYERS];
new g_FcFineBucket[MAX_PLAYERS];
new g_FcFineSum[MAX_PLAYERS];
new bool:g_FcMemBusy[MAX_PLAYERS];
new g_FcMemBusyTick[MAX_PLAYERS];
new g_FcMemMode[MAX_PLAYERS];
new g_FcMemCount[MAX_PLAYERS];
new g_FcMemChar[MAX_PLAYERS][32];
new g_FcMemRank[MAX_PLAYERS][32];
new g_FcMemName[MAX_PLAYERS][32][7];
new g_FcOpName[MAX_PLAYERS][25];
new bool:g_FcSetActive[MAX_PLAYERS];
new g_FcSetTargetChar[MAX_PLAYERS];
new g_FcSetTargetAcc[MAX_PLAYERS];
new g_FcSetStaffAcc[MAX_PLAYERS];
new g_FcSetFac[MAX_PLAYERS];
new g_FcSetRank[MAX_PLAYERS];
new g_FcSetName[MAX_PLAYERS][25];
new g_FcSetStaffName[MAX_PLAYERS][25];
new g_FacTimer;

Fac_CdLeftMs(bool:isSet, last, cooldownMs)
{
	if (!isSet)
	{
		return 0;
	}
	new elapsed = GetTickCount() - last;
	if (elapsed < 0 || elapsed >= cooldownMs)
	{
		return 0;
	}
	return cooldownMs - elapsed;
}

Fac_SecCeil(ms)
{
	if (ms <= 0)
	{
		return 0;
	}
	return (ms + 1000 - 1) / 1000;
}

Faction_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_FcCmdSet[playerid] = 0;
	g_FcCmdLast[playerid] = 0;
	g_FcDutySet[playerid] = 0;
	g_FcDutyLast[playerid] = 0;
	g_FcRadioSet[playerid] = 0;
	g_FcRadioLast[playerid] = 0;
	g_FcCuffSet[playerid] = 0;
	g_FcCuffLast[playerid] = 0;
	g_FcHealSet[playerid] = 0;
	g_FcHealLast[playerid] = 0;
	g_FcHealTgtSet[playerid] = 0;
	g_FcHealTgtLast[playerid] = 0;
	g_FcPrevArmor[playerid] = 0.0;
	g_FcPayMs[playerid] = 0;
	g_FcStillMs[playerid] = 0;
	g_FcLastPos[playerid][0] = 0.0;
	g_FcLastPos[playerid][1] = 0.0;
	g_FcLastPos[playerid][2] = 0.0;
	g_FcKit[playerid] = 0;
	g_FcKitSet[playerid] = 0;
	g_FcKitRefillLast[playerid] = 0;
	g_FcCuffBy[playerid] = 65535;
	g_FcCuffTick[playerid] = 0;
	g_FcInjLeftMs[playerid] = 0;
	g_FcInjSec[playerid] = -1;
	g_FcHospPending[playerid] = 0;
	g_FcRevTarget[playerid] = 65535;
	g_FcRevTargetChar[playerid] = 0;
	g_FcRevStart[playerid] = 0;
	g_FcReviveBucket[playerid] = 0;
	g_FcReviveCount[playerid] = 0;
	g_FcInvActive[playerid] = 0;
	g_FcInvFrom[playerid] = 65535;
	g_FcInvFromChar[playerid] = 0;
	g_FcInvFac[playerid] = 0;
	g_FcInvStart[playerid] = 0;
	g_FcJoinPending[playerid] = 0;
	g_FcFineBusy[playerid] = 0;
	g_FcFineTarget[playerid] = 0;
	g_FcFineReason[playerid][0] = 0;
	g_FcFinePend[playerid] = 0;
	g_FcFineTgtSet[playerid] = 0;
	g_FcFineTgtLast[playerid] = 0;
	g_FcFineBucket[playerid] = 0;
	g_FcFineSum[playerid] = 0;
	g_FcMemBusy[playerid] = 0;
	g_FcMemBusyTick[playerid] = 0;
	g_FcMemMode[playerid] = 0;
	g_FcMemCount[playerid] = 0;
	g_FcOpName[playerid][0] = 0;
	g_FcSetActive[playerid] = 0;
	g_FcSetTargetChar[playerid] = 0;
	g_FcSetTargetAcc[playerid] = 0;
	g_FcSetStaffAcc[playerid] = 0;
	g_FcSetFac[playerid] = 0;
	g_FcSetRank[playerid] = 0;
	g_FcSetName[playerid][0] = 0;
	g_FcSetStaffName[playerid][0] = 0;
}

Fac_ShortName(faction, out[], size)
{
	out[0] = 0;
	if (faction < 0 || faction > 2)
	{
		faction = 0;
	}
	strcat(out, g_FacShort[faction], size);
}

Fac_RankName(faction, rank, out[], size)
{
	if (faction < 1 || faction > 2 || rank < 1 || rank > 6)
	{
		format(out, size, "-");
		return 0;
	}
	format(out, size, "%s", g_FacRankName[faction][rank]);
}

Faction_UpdateControl(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new bool:lock = gPlayer[playerid][pFrozen] || gPlayer[playerid][pCuffed] || gPlayer[playerid][pWounded];
	TogglePlayerControllable(playerid, !lock);
}

Fac_FindByChar(charId)
{
	if (charId <= 0)
	{
		return 65535;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && gPlayer[i][pLoggedIn] && gPlayer[i][pCharId] == charId)
		{
			return i;
		}
	}
	return 65535;
}

bool:Fac_ValidPlayer(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && IsPlayerConnected(playerid);
}

bool:Fac_CmdBlocked(playerid)
{
	new left = Fac_CdLeftMs(g_FcCmdSet[playerid], g_FcCmdLast[playerid], 1500);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Attendi %d secondi prima di un altro comando di fazione.", Fac_SecCeil(left));
		return 1;
	}
	g_FcCmdSet[playerid] = 1;
	g_FcCmdLast[playerid] = GetTickCount();
	return 0;
}

bool:Fac_NeedMember(playerid, faction)
{
	new f = gPlayer[playerid][pFaction];
	if (f < 1 || f > 2 || gPlayer[playerid][pFactionRank] < 1 || (faction != 0 && f != faction))
	{
		Util_Send(playerid, -10270721, "Non sei autorizzato a usare questo comando.");
		return 0;
	}
	return 1;
}

bool:Fac_NeedActive(playerid)
{
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Util_Send(playerid, -10270721, "Non puoi farlo nelle tue condizioni.");
		return 0;
	}
	return 1;
}

bool:Fac_NeedDuty(playerid)
{
	if (!gPlayer[playerid][pOnDuty])
	{
		Util_Send(playerid, -10270721, "Devi essere in servizio (/duty vicino alla sede).");
		return 0;
	}
	return 1;
}

bool:Fac_ParseTarget(playerid, token[], &targetid)
{
	if (!Util_ParsePlayerId(token, targetid) || !Util_IsPlaying(targetid))
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 0;
	}
	return 1;
}

Fac_Emote(playerid, text[])
{
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "* %s %s", name, text);
	Util_SendNear(playerid, -81.42773, Float:0x41A00000, msg);
}

Fac_FineCap(rank)
{
	switch (rank)
	{
		case 2:
		{
			return 500;
		}
		case 3:
		{
			return 1000;
		}
		case 4:
		{
			return 2500;
		}
		case 5:
		{
			return 5000;
		}
		case 6:
		{
			return 5000;
		}
	}
	return 0;
}

Fac_KitHudValue(playerid)
{
	return gPlayer[playerid][pFaction] == 2 ? g_FcKit[playerid] : -1;
}

Fac_Radio(faction, bool:both, color, text[])
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!Util_IsPlaying(i) || !gPlayer[i][pOnDuty])
		{
			continue;
		}
		if (!both && gPlayer[i][pFaction] != faction)
		{
			continue;
		}
		Util_Send(i, color, text);
	}
}

Fac_UniformSkin(playerid)
{
	new f = gPlayer[playerid][pFaction], r = gPlayer[playerid][pFactionRank];
	if (f < 1 || f > 2 || r < 1 || r > 6)
	{
		return gPlayer[playerid][pCivilSkin];
	}
	if (Valid_SkinAllowed(1, gPlayer[playerid][pCivilSkin]))
	{
		return g_FacSkinFemale[f];
	}
	return g_FacRankSkin[f][r];
}

Fac_ApplyUniform(playerid)
{
	new skin = Fac_UniformSkin(playerid);
	SetPlayerSkin(playerid, skin);
	gPlayer[playerid][pSkin] = skin;
}

Fac_GiveKit(playerid)
{
	if (gPlayer[playerid][pFaction] == 1)
	{
		ResetPlayerWeapons(playerid);
		GivePlayerWeapon(playerid, 3, 1);
		GivePlayerWeapon(playerid, 22, 51);
		GivePlayerWeapon(playerid, 41, 100);
		SetPlayerArmour(playerid, Float:0x42480000);
		return 0;
	}
	if (!g_FcKitSet[playerid] || Fac_CdLeftMs(1, g_FcKitRefillLast[playerid], 900000) == 0)
	{
		g_FcKitSet[playerid] = 1;
		g_FcKit[playerid] = 8;
		g_FcKitRefillLast[playerid] = GetTickCount();
	}
}

Fac_ReleaseCuffedBy(agentid)
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (gPlayer[i][pCuffed] && g_FcCuffBy[i] == agentid)
		{
			Fac_Uncuff(i);
		}
	}
}

Faction_ForceOff(playerid, bool:applyWorld)
{
	if (!gPlayer[playerid][pOnDuty])
	{
		return 0;
	}
	gPlayer[playerid][pOnDuty] = 0;
	g_FcRevTarget[playerid] = 65535;
	g_FcPayMs[playerid] = 0;
	g_FcStillMs[playerid] = 0;
	if (IsPlayerConnected(playerid))
	{
		UI_FactionHudHide(playerid);
		if (applyWorld)
		{
			SetPlayerSkin(playerid, gPlayer[playerid][pCivilSkin]);
			ResetPlayerWeapons(playerid);
			SetPlayerArmour(playerid, g_FcPrevArmor[playerid]);
		}
	}
	gPlayer[playerid][pSkin] = gPlayer[playerid][pCivilSkin];
	Fac_ReleaseCuffedBy(playerid);
}

Fac_DutyOn(playerid)
{
	new f = gPlayer[playerid][pFaction];
	new Float:ar;
	GetPlayerArmour(playerid, ar);
	g_FcPrevArmor[playerid] = ar;
	gPlayer[playerid][pOnDuty] = 1;
	g_FcPayMs[playerid] = 0;
	g_FcStillMs[playerid] = 0;
	GetPlayerPos(playerid, g_FcLastPos[playerid][0], g_FcLastPos[playerid][1], g_FcLastPos[playerid][2]);
	Fac_ApplyUniform(playerid);
	Fac_GiveKit(playerid);
	UI_FactionHudShow(playerid, f, gPlayer[playerid][pFactionRank]);
	UI_FactionHudUpdate(playerid, f, gPlayer[playerid][pFactionRank], Fac_KitHudValue(playerid));
	new rn[24];
	Fac_RankName(f, gPlayer[playerid][pFactionRank], rn, 24);
	SendClientMessage(playerid, 0x66DD66FF, "Sei in servizio (%s - %s).", g_FacShort[f], rn);
	Fac_Emote(playerid, "indossa la divisa ed entra in servizio");
}

Faction_CmdDuty(playerid)
{
	if (!Fac_NeedMember(playerid, 0) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed] || Job_IsOnShift(playerid))
	{
		Util_Send(playerid, -10270721, "Non puoi cambiare servizio adesso (ferito, ammanettato o turno di lavoro in corso).");
		return 0;
	}
	new f = gPlayer[playerid][pFaction];
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0 || (GetPlayerDistanceFromPoint(playerid, g_FacHq[f][0], g_FacHq[f][1], g_FacHq[f][2]) > 15.0))
	{
		Util_Send(playerid, -10270721, "Devi essere vicino alla sede della tua fazione.");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcDutySet[playerid], g_FcDutyLast[playerid], 120000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Attendi ancora %d secondi prima di cambiare servizio.", Fac_SecCeil(left));
		return 0;
	}
	if (!gPlayer[playerid][pOnDuty] && Admin_CheckMuted(playerid))
	{
		return 0;
	}
	g_FcDutySet[playerid] = 1;
	g_FcDutyLast[playerid] = GetTickCount();
	if (gPlayer[playerid][pOnDuty])
	{
		Faction_ForceOff(playerid, 1);
		Util_Send(playerid, 0x66DD66FF, "Sei fuori servizio.");
		Fac_Emote(playerid, "si toglie la divisa ed esce dal servizio");
	}
	else
	{
		Fac_DutyOn(playerid);
	}
}

Fac_PayTick(playerid, now)
{
	if (now - gPlayer[playerid][pLastActiveTick] > 10000)
	{
		return 0;
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	new Float:dx = (x - g_FcLastPos[playerid][0]), Float:dy = (y - g_FcLastPos[playerid][1]), Float:dz = (z - g_FcLastPos[playerid][2]);
	if ((floatsqroot((((dx * dx) + (dy * dy)) + (dz * dz))) < 0.5))
	{
		g_FcStillMs[playerid] += 500;
	}
	else
	{
		g_FcStillMs[playerid] = 0;
		g_FcLastPos[playerid][0] = x;
		g_FcLastPos[playerid][1] = y;
		g_FcLastPos[playerid][2] = z;
	}
	if (g_FcStillMs[playerid] > 120000)
	{
		return 0;
	}
	g_FcPayMs[playerid] += 500;
	if (g_FcPayMs[playerid] < 900000)
	{
		return 0;
	}
	g_FcPayMs[playerid] -= 900000;
	new f = gPlayer[playerid][pFaction], amount = g_FacRankPay[f][gPlayer[playerid][pFactionRank]];
	new paid = Economy_PaycheckPaid(amount, Money_BankRoom(playerid));
	if (paid > 0 && Money_GiveBank(playerid, paid, "faction", ""))
	{
		SendClientMessage(playerid, 0x66DD66FF, "Stipendio %s: $%d versati in banca (in aggiunta al paycheck). Saldo: $%d.", g_FacShort[f], paid, gPlayer[playerid][pBank]);
		Char_Save(playerid);
	}
	else
	{
		Util_Send(playerid, -10270721, "Il conto bancario e' al limite: stipendio di fazione non versato.");
	}
}

Fac_Uncuff(targetid)
{
	if (!gPlayer[targetid][pCuffed])
	{
		return 0;
	}
	gPlayer[targetid][pCuffed] = 0;
	g_FcCuffBy[targetid] = 65535;
	g_FcCuffTick[targetid] = 0;
	if (IsPlayerConnected(targetid))
	{
		SetPlayerSpecialAction(targetid, 0);
		ClearAnimations(targetid, 0);
		Faction_UpdateControl(targetid);
		Util_Send(targetid, 0x66DD66FF, "Sei stato liberato dalle manette.");
	}
}

Faction_CmdAmmanetta(playerid, params[], bool:cuff)
{
	if (!Fac_NeedMember(playerid, 1) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[12], target = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, cuff ? "Uso: /ammanetta id" : "Uso: /smanetta id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 2.0))
	{
		Util_Send(playerid, -10270721, "Bersaglio non valido o troppo lontano.");
		return 0;
	}
	new tn[25], an[25], msg[128];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(target, tn, 25);
	if (cuff)
	{
		new left = Fac_CdLeftMs(g_FcCuffSet[playerid], g_FcCuffLast[playerid], 10000);
		if (left > 0)
		{
			SendClientMessage(playerid, -10270721, "Attendi ancora %d secondi.", Fac_SecCeil(left));
			return 0;
		}
		if (gPlayer[target][pCuffed] || gPlayer[target][pWounded] || GetPlayerState(target) != 1)
		{
			Util_Send(playerid, -10270721, "Il bersaglio non puo' essere ammanettato (gia' ammanettato, ferito o non a piedi).");
			return 0;
		}
		g_FcCuffSet[playerid] = 1;
		g_FcCuffLast[playerid] = GetTickCount();
		gPlayer[target][pCuffed] = 1;
		g_FcCuffBy[target] = playerid;
		g_FcCuffTick[target] = GetTickCount();
		Job_Cancel(target, "sei stato ammanettato.");
		Faction_UpdateControl(target);
		SetPlayerSpecialAction(target, 24);
		format(msg, 128, "ammanetta %s", tn);
		Fac_Emote(playerid, msg);
		DB_FactionLogAdd(1, gPlayer[playerid][pCharId], gPlayer[target][pCharId], "cuff", "");
		return 0;
	}
	if (!gPlayer[target][pCuffed])
	{
		Util_Send(playerid, -10270721, "Il bersaglio non e' ammanettato.");
		return 0;
	}
	Fac_Uncuff(target);
	format(msg, 128, "toglie le manette a %s", tn);
	Fac_Emote(playerid, msg);
	DB_FactionLogAdd(1, gPlayer[playerid][pCharId], gPlayer[target][pCharId], "uncuff", "");
}

Faction_CmdRadio(playerid, params[], bool:interdept)
{
	if (!Fac_NeedMember(playerid, 0) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid))
	{
		return 0;
	}
	if (Admin_CheckMuted(playerid))
	{
		return 0;
	}
	new text[97];
	if (!Util_SanitizeText(params, text, 96))
	{
		Util_Send(playerid, 0x99CCFFFF, interdept ? "Uso: /dep testo" : "Uso: /r testo");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcRadioSet[playerid], g_FcRadioLast[playerid], 1000);
	if (left > 0)
	{
		Util_Send(playerid, -10270721, "Rallenta con la radio.");
		return 0;
	}
	g_FcRadioSet[playerid] = 1;
	g_FcRadioLast[playerid] = GetTickCount();
	new f = gPlayer[playerid][pFaction], rn[24], name[25], msg[256];
	Fac_RankName(f, gPlayer[playerid][pFactionRank], rn, 24);
	GetPlayerName(playerid, name, 25);
	if (interdept)
	{
		format(msg, 256, "[DIP] %s %s %s: %s", g_FacShort[f], rn, name, text);
		Fac_Radio(f, 1, -4698625, msg);
	}
	else
	{
		format(msg, 256, "[RADIO] %s %s: %s", rn, name, text);
		Fac_Radio(f, 0, 0x8D8DFFFF, msg);
	}
}

Faction_CmdMulta(playerid, params[])
{
	if (!Fac_NeedMember(playerid, 1) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new rank = gPlayer[playerid][pFactionRank], cap = Fac_FineCap(rank);
	if (cap <= 0)
	{
		SendClientMessage(playerid, -10270721, "Il tuo grado non consente di emettere multe (minimo grado 2).");
		return 0;
	}
	new pos = 0, tid[12], tam[12], target = 0, amount = 0, reason[65];
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, tam, 12);
	if (tid[0] == 0 || tam[0] == 0 || !Util_ParseInt(tam, amount) || !Util_SanitizeText(params[pos], reason, 64))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /multa id importo motivo");
		return 0;
	}
	if (strlen(reason) < 3)
	{
		SendClientMessage(playerid, -10270721, "Motivo troppo corto (minimo %d caratteri).", 3);
		return 0;
	}
	for (new i = 0; reason[i] != 0; i++)
	{
		if (reason[i] == 37)
		{
			reason[i] = 35;
		}
	}
	if (amount < 50 || amount > cap)
	{
		SendClientMessage(playerid, -10270721, "Importo non valido: da $%d a $%d per il tuo grado.", 50, cap);
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tid, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 5.0))
	{
		Util_Send(playerid, -10270721, "Bersaglio non valido o troppo lontano.");
		return 0;
	}
	if (g_FcFineBusy[playerid] || g_FcFinePend[target])
	{
		Util_Send(playerid, -10270721, "Una multa e' gia' in corso di registrazione: attendi.");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcFineTgtSet[target], g_FcFineTgtLast[target], 600000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Questo cittadino e' stato multato da poco: riprova tra %d minuti.", (Fac_SecCeil(left) + 60 - 1) / 60);
		return 0;
	}
	new bucket = gPlayer[playerid][pSessionMs] / 3600000;
	if (bucket != g_FcFineBucket[playerid])
	{
		g_FcFineBucket[playerid] = bucket;
		g_FcFineSum[playerid] = 0;
	}
	if (g_FcFineSum[playerid] + amount > 10000)
	{
		SendClientMessage(playerid, -10270721, "Hai raggiunto il limite orario di multe ($%d).", 10000);
		return 0;
	}
	if (gPlayer[target][pMoney] + gPlayer[target][pBank] < amount)
	{
		Util_Send(playerid, -10270721, "Il cittadino non ha fondi sufficienti: multa rifiutata.");
		return 0;
	}
	g_FcFineBusy[playerid] = 1;
	g_FcFineTarget[playerid] = gPlayer[target][pCharId];
	g_FcFinePend[target] = 1;
	strmid(g_FcFineReason[playerid], reason, 0, strlen(reason), 65);
	DB_AddFine(playerid, gPlayer[playerid][pCharId], gPlayer[target][pCharId], 1, amount, reason);
}

Fac_Debit(playerid, amount)
{
	new taken = 0, cash = gPlayer[playerid][pMoney] < amount ? gPlayer[playerid][pMoney] : amount;
	if (cash > 0 && Money_Take(playerid, cash))
	{
		taken += cash;
	}
	new rest = amount - taken;
	if (rest > 0)
	{
		new bank = gPlayer[playerid][pBank] < rest ? gPlayer[playerid][pBank] : rest;
		if (bank > 0 && Money_TakeBank(playerid, bank, "fine", ""))
		{
			taken += bank;
		}
	}
	return taken;
}

public Fine_OnDBAdded(playerid, targetChar, fineId, amount)
{
	new target = Fac_FindByChar(targetChar), issuer = Fac_ValidPlayer(playerid) ? playerid : 65535;
	new reason[65], bool:hasReason = 0;
	if (issuer != INVALID_PLAYER_ID)
	{
		if (g_FcFineBusy[issuer] && g_FcFineTarget[issuer] == targetChar)
		{
			g_FcFineBusy[issuer] = 0;
			strmid(reason, g_FcFineReason[issuer], 0, strlen(g_FcFineReason[issuer]), 65);
			hasReason = 1;
		}
		else
		{
			issuer = INVALID_PLAYER_ID;
		}
	}
	if (target != INVALID_PLAYER_ID)
	{
		g_FcFinePend[target] = 0;
	}
	new an[25], msg[128];
	if (issuer != INVALID_PLAYER_ID)
	{
		GetPlayerName(issuer, an, 25);
	}
	else
	{
		format(an, 25, "un agente");
	}
	if (fineId <= 0)
	{
		if (issuer != INVALID_PLAYER_ID)
		{
			Util_Send(issuer, -10270721, fineId < 0 ? "Errore del servizio: multa non registrata, nessun addebito." : "Multa rifiutata dal sistema: nessun addebito.");
		}
		return 1;
	}
	if (target == INVALID_PLAYER_ID)
	{
		format(msg, 128, "multa %d ($%d) registrata ma bersaglio char %d non piu' online: nessun addebito", fineId, amount, targetChar);
		Log_Write("FINE", msg);
		if (issuer != INVALID_PLAYER_ID)
		{
			Util_Send(issuer, -10270721, "Il cittadino si e' disconnesso: la multa e' registrata ma non e' stato possibile addebitarla.");
		}
		return 1;
	}
	new taken = Fac_Debit(target, amount);
	if (taken < amount)
	{
		format(msg, 128, "multa %d: addebitati $%d su $%d (fondi cambiati durante la registrazione)", fineId, taken, amount);
		Log_Write("FINE", msg);
	}
	g_FcFineTgtSet[target] = 1;
	g_FcFineTgtLast[target] = GetTickCount();
	Char_Save(target);
	SendClientMessage(target, -10270721, "Hai ricevuto una multa di $%d da %s%s%s.", taken, an, hasReason ? ": " : "", reason);
	if (issuer != INVALID_PLAYER_ID)
	{
		new bucket = gPlayer[issuer][pSessionMs] / 3600000;
		if (bucket != g_FcFineBucket[issuer])
		{
			g_FcFineBucket[issuer] = bucket;
			g_FcFineSum[issuer] = 0;
		}
		g_FcFineSum[issuer] += amount;
		new tn[25];
		GetPlayerName(target, tn, 25);
		SendClientMessage(issuer, 0x66DD66FF, "Multa #%d di $%d a %s registrata.", fineId, taken, tn);
		format(msg, 128, "consegna una multa a %s", tn);
		Fac_Emote(issuer, msg);
	}
	return 1;
}

Faction_CmdMulte(playerid)
{
	if (gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	DB_LoadFines(playerid, gPlayer[playerid][pCharId]);
}

public Fine_OnDBLoaded(playerid, count)
{
	if (!Fac_ValidPlayer(playerid))
	{
		return 1;
	}
	if (count < 0)
	{
		Util_Send(playerid, -10270721, "Servizio temporaneamente non disponibile: riprova piu' tardi.");
		return 1;
	}
	if (count == 0)
	{
		Util_Send(playerid, -1, "Non hai multe registrate.");
		return 1;
	}
	SendClientMessage(playerid, -2994689, "--- Ultime multe ricevute ---");
	new now = gettime(), ago[24];
	for (new i = 0; i < count && i < 5; i++)
	{
		Admin_FormatDuration(now - g_FineRows[i][frTimestamp], ago, 24);
		SendClientMessage(playerid, -1, "#%d  $%d  %s  (%s)  %s fa", g_FineRows[i][frId], g_FineRows[i][frAmount], g_FineRows[i][frReason], g_FineRows[i][frIssuer], ago);
	}
	return 1;
}

Fac_UseCharge(playerid)
{
	if (g_FcKit[playerid] > 0)
	{
		g_FcKit[playerid]--;
	}
	UI_FactionHudUpdate(playerid, gPlayer[playerid][pFaction], gPlayer[playerid][pFactionRank], g_FcKit[playerid]);
}

Faction_CmdCura(playerid, params[])
{
	if (!Fac_NeedMember(playerid, 2) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[12], target = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /cura id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "Bersaglio non valido o troppo lontano.");
		return 0;
	}
	if (gPlayer[target][pWounded])
	{
		Util_Send(playerid, -10270721, "Il bersaglio e' ferito: usa /rianima.");
		return 0;
	}
	if (g_FcKit[playerid] <= 0)
	{
		Util_Send(playerid, -10270721, "Il kit medico e' esaurito (si ricarica a /duty on).");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcHealSet[playerid], g_FcHealLast[playerid], 20000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Attendi ancora %d secondi.", Fac_SecCeil(left));
		return 0;
	}
	left = Fac_CdLeftMs(g_FcHealTgtSet[target], g_FcHealTgtLast[target], 60000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Il paziente e' stato curato da poco: attendi %d secondi.", Fac_SecCeil(left));
		return 0;
	}
	new Float:hp;
	GetPlayerHealth(target, hp);
	if ((hp >= Float:0x42C80000))
	{
		Util_Send(playerid, -10270721, "Il bersaglio e' in piena salute.");
		return 0;
	}
	hp = (Float:0x42200000 + hp);
	if ((hp > Float:0x42C80000))
	{
		hp = Float:0x42C80000;
	}
	SetPlayerHealth(target, hp);
	g_FcHealSet[playerid] = 1;
	g_FcHealLast[playerid] = GetTickCount();
	g_FcHealTgtSet[target] = 1;
	g_FcHealTgtLast[target] = GetTickCount();
	Fac_UseCharge(playerid);
	new tn[25], msg[128];
	GetPlayerName(target, tn, 25);
	format(msg, 128, "presta le prime cure a %s", tn);
	Fac_Emote(playerid, msg);
	SendClientMessage(target, 0x66DD66FF, "Sei stato curato (salute %.0f).", hp);
	DB_FactionLogAdd(2, gPlayer[playerid][pCharId], gPlayer[target][pCharId], "heal", "");
}

Fac_ReviveCancel(medic, reason[])
{
	g_FcRevTarget[medic] = 65535;
	g_FcRevTargetChar[medic] = 0;
	if (IsPlayerConnected(medic))
	{
		ClearAnimations(medic, 0);
		if (reason[0] != 0)
		{
			Util_Send(medic, -10270721, reason);
		}
	}
}

Faction_CmdRianima(playerid, params[])
{
	if (!Fac_NeedMember(playerid, 2) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[12], target = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /rianima id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "Bersaglio non valido o troppo lontano.");
		return 0;
	}
	if (!gPlayer[target][pWounded])
	{
		Util_Send(playerid, -10270721, "Il bersaglio non e' ferito.");
		return 0;
	}
	if (g_FcKit[playerid] <= 0)
	{
		Util_Send(playerid, -10270721, "Il kit medico e' esaurito (si ricarica a /duty on).");
		return 0;
	}
	if (g_FcRevTarget[playerid] != 65535)
	{
		Util_Send(playerid, -10270721, "Stai gia' rianimando qualcuno.");
		return 0;
	}
	g_FcRevTarget[playerid] = target;
	g_FcRevTargetChar[playerid] = gPlayer[target][pCharId];
	g_FcRevStart[playerid] = GetTickCount();
	ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, 1, 0, 0, 0, 0, 0);
	new tn[25], msg[128];
	GetPlayerName(target, tn, 25);
	format(msg, 128, "inizia a rianimare %s", tn);
	Fac_Emote(playerid, msg);
	SendClientMessage(playerid, -1, "Rianimazione in corso (%d secondi): non allontanarti.", 8);
}

Fac_ReviveDone(medic, target)
{
	g_FcRevTarget[medic] = 65535;
	g_FcRevTargetChar[medic] = 0;
	ClearAnimations(medic, 0);
	gPlayer[target][pWounded] = 0;
	g_FcInjLeftMs[target] = 0;
	SetPlayerHealth(target, Float:0x42700000);
	ClearAnimations(target, 0);
	Faction_UpdateControl(target);
	UI_InjuryEndShow(target, 2, 0);
	Fac_UseCharge(medic);
	new mn[25], tn[25], msg[128];
	GetPlayerName(medic, mn, 25);
	GetPlayerName(target, tn, 25);
	SendClientMessage(target, 0x66DD66FF, "%s ti ha rianimato.", mn);
	format(msg, 128, "rianima %s", tn);
	Fac_Emote(medic, msg);
	DB_FactionLogAdd(2, gPlayer[medic][pCharId], gPlayer[target][pCharId], "revive", "");
	new bucket = gPlayer[medic][pSessionMs] / 3600000;
	if (bucket != g_FcReviveBucket[medic])
	{
		g_FcReviveBucket[medic] = bucket;
		g_FcReviveCount[medic] = 0;
	}
	if (g_FcReviveCount[medic] < 6 && Money_GiveBank(medic, 100, "faction", ""))
	{
		g_FcReviveCount[medic]++;
		SendClientMessage(medic, 0x66DD66FF, "Bonus rianimazione: $%d versati in banca.", 100);
		Char_Save(medic);
	}
}

Fac_ApplyInjuredPose(playerid)
{
	ApplyAnimation(playerid, "CRACK", "crckidle2", 4.1, 1, 0, 0, 1, 0, 0);
}

Faction_Hospitalize(playerid)
{
	new fee = Fac_Debit(playerid, 200);
	gPlayer[playerid][pWounded] = 0;
	g_FcInjLeftMs[playerid] = 0;
	g_FcHospPending[playerid] = 0;
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerPos(playerid, 1182.0, Float:0xC4A64000, 13.9);
	SetPlayerFacingAngle(playerid, Float:0x43870000);
	SetCameraBehindPlayer(playerid);
	ClearAnimations(playerid, 0);
	SetPlayerHealth(playerid, Float:0x42C80000);
	Faction_UpdateControl(playerid);
	UI_InjuryEndShow(playerid, 1, fee);
	Char_Save(playerid);
	new name[25], msg[128];
	GetPlayerName(playerid, name, 25);
	format(msg, 128, "ricovero %s: addebitati $%d", name, fee);
	Log_Write("INJURY", msg);
}

Fac_ClearInvite(targetid, bool:closeDialog)
{
	g_FcInvActive[targetid] = 0;
	g_FcInvFrom[targetid] = 65535;
	g_FcInvFromChar[targetid] = 0;
	g_FcInvFac[targetid] = 0;
	g_FcInvStart[targetid] = 0;
	if (closeDialog && IsPlayerConnected(targetid))
	{
		UI_FactionInviteClose(targetid);
	}
}

Fac_DoInvite(playerid, target)
{
	new f = gPlayer[playerid][pFaction];
	if (f < 1 || gPlayer[playerid][pFactionRank] < 5)
	{
		Util_Send(playerid, -10270721, "Non sei autorizzato a invitare.");
		return 0;
	}
	if (!Fac_NeedActive(playerid))
	{
		return 0;
	}
	if (!Util_IsPlaying(target) || target == playerid)
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 0;
	}
	if (!Util_InRange(playerid, target, 5.0))
	{
		Util_Send(playerid, -10270721, "Il giocatore e' troppo lontano.");
		return 0;
	}
	if (gPlayer[target][pFaction] != 0)
	{
		Util_Send(playerid, -10270721, "Il giocatore appartiene gia' a una fazione.");
		return 0;
	}
	if (gPlayer[target][pLevel] < 3)
	{
		SendClientMessage(playerid, -10270721, "Il giocatore deve essere almeno livello %d.", 3);
		return 0;
	}
	if ((g_FcInvActive[target] && Fac_CdLeftMs(1, g_FcInvStart[target], 60000) > 0) || g_FcJoinPending[target])
	{
		Util_Send(playerid, -10270721, "Il giocatore ha gia' un invito in sospeso.");
		return 0;
	}
	g_FcInvActive[target] = 1;
	g_FcInvFrom[target] = playerid;
	g_FcInvFromChar[target] = gPlayer[playerid][pCharId];
	g_FcInvFac[target] = f;
	g_FcInvStart[target] = GetTickCount();
	new an[25], tn[25];
	GetPlayerName(playerid, an, 25);
	GetPlayerName(target, tn, 25);
	UI_FactionInviteShow(target, f, an, 60);
	SendClientMessage(target, -1, "%s ti invita in %s. Accetta dal pannello o con /accetta (valido %d secondi).", an, g_FacShort[f], 60);
	SendClientMessage(playerid, 0x66DD66FF, "Invito inviato a %s.", tn);
}

Faction_CmdInvita(playerid, params[])
{
	if (!Fac_NeedMember(playerid, 0) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[12], target = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /invita id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	Fac_DoInvite(playerid, target);
}

public UI_OnFactionInviteTarget(playerid, targetId)
{
	if (!Util_IsPlaying(playerid) || Fac_CmdBlocked(playerid))
	{
		return 1;
	}
	Fac_DoInvite(playerid, targetId);
	return 1;
}

Fac_InviteReply(playerid, bool:accept)
{
	if (!g_FcInvActive[playerid])
	{
		return 0;
	}
	new from = g_FcInvFrom[playerid], fromChar = g_FcInvFromChar[playerid], f = g_FcInvFac[playerid];
	new bool:expired = Fac_CdLeftMs(1, g_FcInvStart[playerid], 60000) <= 0;
	Fac_ClearInvite(playerid, 0);
	if (!Util_IsPlaying(playerid))
	{
		return 0;
	}
	if (expired)
	{
		Util_Send(playerid, -10270721, "L'invito e' scaduto.");
		return 0;
	}
	if (!accept)
	{
		Util_Send(playerid, -1, "Hai rifiutato l'invito.");
		if (Fac_ValidPlayer(from) && gPlayer[from][pCharId] == fromChar)
		{
			Util_Send(from, -1, "L'invito e' stato rifiutato.");
		}
		return 0;
	}
	if (!Util_IsPlaying(from) || gPlayer[from][pCharId] != fromChar || gPlayer[from][pFaction] != f || gPlayer[from][pFactionRank] < 5)
	{
		Util_Send(playerid, -10270721, "Chi ti ha invitato non e' piu' disponibile.");
		return 0;
	}
	if (gPlayer[playerid][pFaction] != 0 || gPlayer[playerid][pLevel] < 3 || g_FcJoinPending[playerid])
	{
		Util_Send(playerid, -10270721, "Non puoi entrare in una fazione adesso.");
		return 0;
	}
	g_FcJoinPending[playerid] = 1;
	GetPlayerName(playerid, g_FcOpName[from], 25);
	DB_FactionJoin(from, fromChar, gPlayer[playerid][pCharId], f);
}

public UI_OnFactionInviteReply(playerid, accept)
{
	Fac_InviteReply(playerid, accept != 0);
	return 1;
}

Faction_CmdAccetta(playerid)
{
	if (!g_FcInvActive[playerid])
	{
		Util_Send(playerid, -10270721, "Non hai inviti in sospeso.");
		return 0;
	}
	UI_FactionInviteClose(playerid);
	Fac_InviteReply(playerid, 1);
}

Fac_DoRankOp(playerid, action, targetChar, targetRank, targetName[])
{
	new f = gPlayer[playerid][pFaction], own = gPlayer[playerid][pFactionRank];
	if (f < 1 || own < 5)
	{
		Util_Send(playerid, -10270721, "Non sei autorizzato.");
		return 0;
	}
	if (!Fac_NeedActive(playerid))
	{
		return 0;
	}
	if (targetChar <= 0 || targetChar == gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "Bersaglio non valido.");
		return 0;
	}
	if (targetRank < 1 || targetRank >= own)
	{
		Util_Send(playerid, -10270721, "Puoi agire solo su membri di grado inferiore al tuo.");
		return 0;
	}
	strmid(g_FcOpName[playerid], targetName, 0, strlen(targetName), 25);
	switch (action)
	{
		case 4:
		{
			if (targetRank + 1 >= own)
			{
				Util_Send(playerid, -10270721, "Il nuovo grado deve restare inferiore al tuo.");
				return 0;
			}
			DB_FactionPromote(playerid, gPlayer[playerid][pCharId], targetChar, f, targetRank + 1);
		}
		case 5:
		{
			if (targetRank <= 1)
			{
				Util_Send(playerid, -10270721, "Il membro e' gia' al grado minimo.");
				return 0;
			}
			DB_FactionDemote(playerid, gPlayer[playerid][pCharId], targetChar, f, targetRank - 1);
		}
		case 6:
		{
			DB_FactionFire(playerid, gPlayer[playerid][pCharId], targetChar, f);
		}
	}
}

Faction_CmdRankOp(playerid, params[], action)
{
	if (!Fac_NeedMember(playerid, 0) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[12], target = 0;
	Util_NextToken(params, pos, tok, 12);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, action == 4 ? "Uso: /promuovi id" : (action == 5 ? "Uso: /degrada id" : "Uso: /licenzia id"));
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (gPlayer[target][pFaction] != gPlayer[playerid][pFaction])
	{
		Util_Send(playerid, -10270721, "Il giocatore non appartiene alla tua fazione.");
		return 0;
	}
	new tn[25];
	GetPlayerName(target, tn, 25);
	Fac_DoRankOp(playerid, action, gPlayer[target][pCharId], gPlayer[target][pFactionRank], tn);
}

public UI_OnFactionMemberAction(playerid, action, memberId)
{
	if (!Util_IsPlaying(playerid) || Fac_CmdBlocked(playerid))
	{
		return 1;
	}
	for (new i = 0; i < g_FcMemCount[playerid]; i++)
	{
		if (g_FcMemChar[playerid][i] != memberId)
		{
			continue;
		}
		new name[25];
		strunpack(name, g_FcMemName[playerid][i], 25);
		Fac_DoRankOp(playerid, action, memberId, g_FcMemRank[playerid][i], name);
		return 1;
	}
	Util_Send(playerid, -10270721, "Membro non piu' in lista: riapri il menu.");
	return 1;
}

Fac_ApplyChange(target, cmd, faction, rank)
{
	new oldFac = gPlayer[target][pFaction], rn[24];
	switch (cmd)
	{
		case 1:
		{
			gPlayer[target][pFaction] = faction;
			gPlayer[target][pFactionRank] = rank;
		}
		case 2, 3:
		{
			gPlayer[target][pFactionRank] = rank;
		}
		case 4, 5:
		{
			gPlayer[target][pFaction] = 0;
			gPlayer[target][pFactionRank] = 0;
		}
		case 6:
		{
			gPlayer[target][pFaction] = faction;
			gPlayer[target][pFactionRank] = faction == 0 ? 0 : rank;
		}
	}
	new nf = gPlayer[target][pFaction], nr = gPlayer[target][pFactionRank];
	if (gPlayer[target][pOnDuty])
	{
		if (nf != oldFac || nf == 0)
		{
			Faction_ForceOff(target, Util_IsPlaying(target));
			Util_Send(target, -1, "Sei stato messo fuori servizio.");
		}
		else
		{
			Fac_ApplyUniform(target);
			UI_FactionHudShow(target, nf, nr);
			UI_FactionHudUpdate(target, nf, nr, Fac_KitHudValue(target));
		}
	}
	Fac_RankName(nf, nr, rn, 24);
	switch (cmd)
	{
		case 1:
		{
			SendClientMessage(target, 0x66DD66FF, "Sei entrato in %s con il grado %s.", g_FacShort[nf], rn);
		}
		case 2:
		{
			SendClientMessage(target, 0x66DD66FF, "Sei stato promosso a %s (%s).", rn, g_FacShort[nf]);
		}
		case 3:
		{
			SendClientMessage(target, -10270721, "Sei stato degradato a %s (%s).", rn, g_FacShort[nf]);
		}
		case 4:
		{
			SendClientMessage(target, -10270721, "Sei stato licenziato da %s.", g_FacShort[faction]);
		}
		case 5:
		{
			SendClientMessage(target, -1, "Hai lasciato %s.", g_FacShort[faction]);
		}
		case 6:
		{
			if (nf == 0)
			{
				Util_Send(target, -5963521, "Sei stato rimosso dalla fazione da uno staff.");
			}
			else
			{
				SendClientMessage(target, -5963521, "Uno staff ti ha assegnato a %s con il grado %s.", g_FacShort[nf], rn);
			}
		}
	}
}

Fac_SetCtxByTarget(targetChar)
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FcSetActive[i] && g_FcSetTargetChar[i] == targetChar)
		{
			return i;
		}
	}
	return 65535;
}

public Faction_OnDBChanged(playerid, targetChar, cmd, faction, rank, result)
{
	new target = Fac_FindByChar(targetChar);
	if (target != INVALID_PLAYER_ID && cmd == 1)
	{
		g_FcJoinPending[target] = 0;
	}
	new req = Fac_ValidPlayer(playerid) ? playerid : 65535;
	new ctx = cmd == 6 ? Fac_SetCtxByTarget(targetChar) : 65535;
	if (result == 1)
	{
		if (target != INVALID_PLAYER_ID)
		{
			Fac_ApplyChange(target, cmd, faction, rank);
		}
		new rn[24], tn[25];
		if (target != INVALID_PLAYER_ID)
		{
			GetPlayerName(target, tn, 25);
		}
		else if (req != INVALID_PLAYER_ID)
		{
			strmid(tn, g_FcOpName[req], 0, strlen(g_FcOpName[req]), 25);
		}
		if (ctx != 65535)
		{
			strmid(tn, g_FcSetName[ctx], 0, strlen(g_FcSetName[ctx]), 25);
		}
		Fac_RankName(faction, rank, rn, 24);
		if (cmd == 6 && ctx != 65535)
		{
			new detail[128], logDetail[128];
			if (faction == 0)
			{
				format(detail, 128, "rimosso dalla fazione");
			}
			else
			{
				format(detail, 128, "%s grado %d", g_FacShort[faction], rank);
			}
			format(logDetail, 128, "%s: %s", tn, detail);
			Admin_LogRaw(g_FcSetStaffAcc[ctx], g_FcSetStaffName[ctx], g_FcSetTargetAcc[ctx], "setfazione", logDetail);
			Admin_AdmCmd(g_FcSetStaffName[ctx], "impostato la fazione di", tn, detail, "");
		}
		else if (req != INVALID_PLAYER_ID)
		{
			switch (cmd)
			{
				case 1:
				{
					SendClientMessage(req, 0x66DD66FF, "%s e' entrato in %s.", tn, g_FacShort[faction]);
				}
				case 2:
				{
					SendClientMessage(req, 0x66DD66FF, "%s promosso a %s.", tn, rn);
				}
				case 3:
				{
					SendClientMessage(req, 0x66DD66FF, "%s degradato a %s.", tn, rn);
				}
				case 4:
				{
					SendClientMessage(req, 0x66DD66FF, "%s licenziato.", tn);
				}
			}
		}
		new msg[128];
		format(msg, 128, "cmd %d char %d fazione %d grado %d", cmd, targetChar, faction, rank);
		Log_Write("FACTION", msg);
	}
	else
	{
		new notify = ctx != 65535 && IsPlayerConnected(ctx) ? ctx : req;
		if (notify != 65535)
		{
			if (result < 0)
			{
				Util_Send(notify, -10270721, "Servizio temporaneamente non disponibile: operazione non eseguita.");
			}
			else
			{
				Util_Send(notify, -10270721, "Operazione rifiutata (gradi, fazione o livello non idonei, o un'altra operazione e' in corso).");
			}
		}
		if (target != INVALID_PLAYER_ID && cmd == 1)
		{
			Util_Send(target, -10270721, "Non e' stato possibile completare l'ingresso in fazione.");
		}
	}
	if (ctx != 65535)
	{
		g_FcSetActive[ctx] = 0;
	}
	return 1;
}

Fac_RequestMembers(playerid, mode)
{
	new f = gPlayer[playerid][pFaction];
	if (f < 1)
	{
		Util_Send(playerid, -10270721, "Non appartieni a nessuna fazione.");
		return 0;
	}
	if (g_FcMemBusy[playerid] && Fac_CdLeftMs(1, g_FcMemBusyTick[playerid], 10000) > 0)
	{
		Util_Send(playerid, -10270721, "Richiesta gia' in corso: attendi.");
		return 0;
	}
	g_FcMemBusy[playerid] = 1;
	g_FcMemBusyTick[playerid] = GetTickCount();
	g_FcMemMode[playerid] = mode;
	DB_FactionMembers(playerid, f);
}

Faction_CmdFazione(playerid, params[])
{
	if (gPlayer[playerid][pFaction] < 1 || gPlayer[playerid][pFactionRank] < 1)
	{
		Util_Send(playerid, -10270721, "Non appartieni a nessuna fazione.");
		return 0;
	}
	new pos = 0, sub[16];
	Util_NextToken(params, pos, sub, 16);
	if (sub[0] == 0 || !strcmp(sub, "info", 1, 0x7FFFFFFF))
	{
		Fac_RequestMembers(playerid, 0);
		return 0;
	}
	if (!strcmp(sub, "membri", 1, 0x7FFFFFFF))
	{
		Fac_RequestMembers(playerid, 2);
		return 0;
	}
	if (!strcmp(sub, "lascia", 1, 0x7FFFFFFF))
	{
		if (Fac_CmdBlocked(playerid) || !Fac_NeedActive(playerid))
		{
			return 0;
		}
		GetPlayerName(playerid, g_FcOpName[playerid], 25);
		DB_FactionLeave(playerid, gPlayer[playerid][pCharId], gPlayer[playerid][pFaction]);
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "Uso: /fazione [info|membri|lascia]");
}

public UI_OnFactionMenuAction(playerid, action)
{
	if (!Util_IsPlaying(playerid) || gPlayer[playerid][pFaction] < 1)
	{
		return 1;
	}
	if (action != 2 && gPlayer[playerid][pFactionRank] < 5)
	{
		return 1;
	}
	Fac_RequestMembers(playerid, action);
	return 1;
}

public Faction_OnDBMembers(playerid, faction, count)
{
	if (!Fac_ValidPlayer(playerid))
	{
		return 1;
	}
	g_FcMemBusy[playerid] = 0;
	if (count < 0)
	{
		Util_Send(playerid, -10270721, "Servizio temporaneamente non disponibile: riprova piu' tardi.");
		return 1;
	}
	if (gPlayer[playerid][pFaction] != faction || faction < 1)
	{
		return 1;
	}
	new mode = g_FcMemMode[playerid], online = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (Util_IsPlaying(i) && gPlayer[i][pFaction] == faction)
		{
			online++;
		}
	}
	if (mode == 0)
	{
		UI_FactionMenuShow(playerid, faction, gPlayer[playerid][pFactionRank], gPlayer[playerid][pOnDuty], online, count);
		return 1;
	}
	UI_FactionMemberListClear(playerid);
	g_FcMemCount[playerid] = 0;
	for (new i = 0; i < count; i++)
	{
		new other = Fac_FindByChar(g_FacMembers[i][fmCharId]), status = 0;
		if (other != INVALID_PLAYER_ID)
		{
			status = gPlayer[other][pOnDuty] ? 2 : 1;
		}
		if (!UI_FactionMemberListAdd(playerid, g_FacMembers[i][fmCharId], g_FacMembers[i][fmName], g_FacMembers[i][fmRank], status))
		{
			break;
		}
		new n = g_FcMemCount[playerid];
		if (n >= 32)
		{
			break;
		}
		g_FcMemChar[playerid][n] = g_FacMembers[i][fmCharId];
		g_FcMemRank[playerid][n] = g_FacMembers[i][fmRank];
		strpack(g_FcMemName[playerid][n], g_FacMembers[i][fmName], 7);
		g_FcMemCount[playerid] = n + 1;
	}
	UI_FactionMembersShow(playerid, faction, gPlayer[playerid][pFactionRank], mode);
	return 1;
}

Fac_SetStart(staff, targetChar, targetAcc, targetName[], faction, rank)
{
	g_FcSetActive[staff] = 1;
	g_FcSetTargetChar[staff] = targetChar;
	g_FcSetTargetAcc[staff] = targetAcc;
	g_FcSetStaffAcc[staff] = gPlayer[staff][pAccountId];
	g_FcSetFac[staff] = faction;
	g_FcSetRank[staff] = rank;
	strmid(g_FcSetName[staff], targetName, 0, strlen(targetName), 25);
	GetPlayerName(staff, g_FcSetStaffName[staff], 25);
	DB_FactionSet(staff, targetChar, faction, rank);
}

bool:Faction_CmdSetfazione(playerid, params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return 0;
	}
	new pos = 0, who[25], tf[12], tr[12], faction = 0, rank = 0;
	Util_NextToken(params, pos, who, 25);
	Util_NextToken(params, pos, tf, 12);
	Util_NextToken(params, pos, tr, 12);
	if (who[0] == 0 || !Util_ParseInt(tf, faction) || faction < 0 || faction > 2 || (faction != 0 && (!Util_ParseInt(tr, rank) || rank < 1 || rank > 6)))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Uso: /setfazione id|Nome_Cognome fazione(%d-%d, 0 = rimuovi) grado(1-%d)", 0, 2, 6);
		return 1;
	}
	if (faction == 0)
	{
		rank = 0;
	}
	if (g_FcSetActive[playerid])
	{
		Util_Send(playerid, -10270721, "Hai gia' un'operazione /setfazione in corso.");
		return 1;
	}
	new targetid = INVALID_PLAYER_ID;
	if (Util_ParsePlayerId(who, targetid))
	{
		if (!Util_IsPlaying(targetid) || gPlayer[targetid][pCharId] <= 0)
		{
			Util_Send(playerid, -10270721, "Giocatore non valido.");
			return 1;
		}
	}
	else
	{
		targetid = Admin_FindByName(who);
	}
	if (targetid != INVALID_PLAYER_ID && gPlayer[targetid][pLoggedIn] && gPlayer[targetid][pCharId] > 0)
	{
		new tn[25];
		GetPlayerName(targetid, tn, 25);
		Fac_SetStart(playerid, gPlayer[targetid][pCharId], gPlayer[targetid][pAccountId], tn, faction, rank);
		return 1;
	}
	if (!Util_IsValidRPName(who))
	{
		Util_Send(playerid, -10270721, "Giocatore non valido (usa id oppure Nome_Cognome).");
		return 1;
	}
	g_FcSetActive[playerid] = 1;
	g_FcSetTargetChar[playerid] = 0;
	g_FcSetTargetAcc[playerid] = 0;
	g_FcSetStaffAcc[playerid] = gPlayer[playerid][pAccountId];
	g_FcSetFac[playerid] = faction;
	g_FcSetRank[playerid] = rank;
	strmid(g_FcSetName[playerid], who, 0, strlen(who), 25);
	GetPlayerName(playerid, g_FcSetStaffName[playerid], 25);
	DB_FactionFindByName(playerid, who, 1);
	return 1;
}

public Faction_OnDBFound(playerid, tag, charId, name[], faction, rank, level)
{
	if (tag != 1 || !Fac_ValidPlayer(playerid) || !g_FcSetActive[playerid] || g_FcSetTargetChar[playerid] != 0)
	{
		return 1;
	}
	if (charId <= 0)
	{
		g_FcSetActive[playerid] = 0;
		Util_Send(playerid, -10270721, charId < 0 ? "Servizio temporaneamente non disponibile: riprova piu' tardi." : "Personaggio non trovato.");
		return 1;
	}
	new f = g_FcSetFac[playerid], r = g_FcSetRank[playerid];
	Fac_SetStart(playerid, charId, 0, name, f, r);
	return 1;
}

public Faction_OnTick()
{
	new now = GetTickCount();
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i))
		{
			continue;
		}
		if (g_FcInvActive[i] && Fac_CdLeftMs(1, g_FcInvStart[i], 60000) <= 0)
		{
			new from = g_FcInvFrom[i], fromChar = g_FcInvFromChar[i];
			Fac_ClearInvite(i, 1);
			Util_Send(i, -1, "L'invito in fazione e' scaduto.");
			if (Fac_ValidPlayer(from) && gPlayer[from][pCharId] == fromChar)
			{
				Util_Send(from, -1, "Il tuo invito e' scaduto.");
			}
		}
		if (!gPlayer[i][pSpawned] || !Util_IsPlaying(i))
		{
			continue;
		}
		if (gPlayer[i][pCuffed])
		{
			new by = g_FcCuffBy[i];
			if (Fac_CdLeftMs(1, g_FcCuffTick[i], 600000) <= 0 || !Util_IsPlaying(by) || !gPlayer[by][pOnDuty])
			{
				Fac_Uncuff(i);
			}
		}
		if (gPlayer[i][pOnDuty])
		{
			if (gPlayer[i][pFaction] < 1 || Admin_IsMuted(i))
			{
				Faction_ForceOff(i, 1);
				Util_Send(i, -10270721, "Sei stato messo fuori servizio.");
			}
			else
			{
				Fac_PayTick(i, now);
			}
		}
		if (g_FcRevTarget[i] != 65535)
		{
			new t = g_FcRevTarget[i];
			if (!gPlayer[i][pOnDuty] || gPlayer[i][pWounded] || gPlayer[i][pCuffed])
			{
				Fac_ReviveCancel(i, "Rianimazione interrotta.");
			}
			else if (!Util_IsPlaying(t) || gPlayer[t][pCharId] != g_FcRevTargetChar[i] || !gPlayer[t][pWounded])
			{
				Fac_ReviveCancel(i, "Rianimazione interrotta: il paziente non e' piu' ferito.");
			}
			else if (!Util_InRange(i, t, (3.0 + 1.0)))
			{
				Fac_ReviveCancel(i, "Rianimazione interrotta: ti sei allontanato.");
			}
			else if (now - g_FcRevStart[i] >= 8000)
			{
				if (g_FcKit[i] > 0)
				{
					Fac_ReviveDone(i, t);
				}
				else
				{
					Fac_ReviveCancel(i, "Kit medico esaurito.");
				}
			}
		}
		if (gPlayer[i][pWounded])
		{
			g_FcInjLeftMs[i] -= 500;
			if (g_FcInjLeftMs[i] <= 0)
			{
				Faction_Hospitalize(i);
				continue;
			}
			new sec = Fac_SecCeil(g_FcInjLeftMs[i]);
			if (sec != g_FcInjSec[i])
			{
				g_FcInjSec[i] = sec;
				UI_InjuryHudUpdate(i, sec);
				Fac_ApplyInjuredPose(i);
			}
		}
	}
	return 1;
}

Faction_OnInit()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		Faction_ResetPlayer(p);
	}
	g_FacTimer = SetTimer("Faction_OnTick", 500, 1);
}

Faction_OnExit()
{
	if (g_FacTimer != 0)
	{
		KillTimer(g_FacTimer);
	}
	g_FacTimer = 0;
}

Faction_OnPlayerConnect(playerid)
{
	Faction_ResetPlayer(playerid);
}

Faction_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	Fac_ReleaseCuffedBy(playerid);
	if (gPlayer[playerid][pOnDuty])
	{
		Faction_ForceOff(playerid, 1);
	}
	if (g_FcInvActive[playerid])
	{
		Fac_ClearInvite(playerid, 0);
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FcInvActive[i] && g_FcInvFrom[i] == playerid)
		{
			Fac_ClearInvite(i, 1);
		}
		if (g_FcRevTarget[i] == playerid)
		{
			Fac_ReviveCancel(i, "Rianimazione interrotta: il paziente si e' scollegato.");
		}
	}
	if (gPlayer[playerid][pWounded])
	{
		gPlayer[playerid][pWounded] = 0;
		gPlayer[playerid][pPosX] = 1182.0;
		gPlayer[playerid][pPosY] = Float:0xC4A64000;
		gPlayer[playerid][pPosZ] = 13.9;
		gPlayer[playerid][pAngle] = Float:0x43870000;
		gPlayer[playerid][pInterior] = 0;
		gPlayer[playerid][pWorld] = 0;
		gPlayer[playerid][pHealth] = Float:0x42C80000;
		gPlayer[playerid][pArmor] = 0.0;
		gPlayer[playerid][pSpawned] = 0;
	}
	gPlayer[playerid][pSkin] = gPlayer[playerid][pCivilSkin];
	Faction_ResetPlayer(playerid);
}

Faction_OnPlayerDeath(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayer[playerid][pSpawned])
	{
		return 0;
	}
	Fac_ReleaseCuffedBy(playerid);
	if (gPlayer[playerid][pCuffed])
	{
		gPlayer[playerid][pCuffed] = 0;
		g_FcCuffBy[playerid] = 65535;
	}
	if (gPlayer[playerid][pOnDuty])
	{
		Faction_ForceOff(playerid, 0);
	}
	g_FcRevTarget[playerid] = 65535;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FcRevTarget[i] == playerid)
		{
			Fac_ReviveCancel(i, "Rianimazione interrotta.");
		}
	}
	if (gPlayer[playerid][pWounded])
	{
		g_FcHospPending[playerid] = 1;
		return 0;
	}
	gPlayer[playerid][pWounded] = 1;
	g_FcInjLeftMs[playerid] = 180000;
	g_FcInjSec[playerid] = -1;
	UI_InjuryHudHide(playerid);
}

Faction_OnPlayerSpawn(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayer[playerid][pSpawned])
	{
		return 0;
	}
	if (gPlayer[playerid][pOnDuty])
	{
		Faction_ForceOff(playerid, 1);
	}
	if (g_FcHospPending[playerid])
	{
		Faction_Hospitalize(playerid);
		return 0;
	}
	if (!gPlayer[playerid][pWounded])
	{
		return 0;
	}
	SetPlayerHealth(playerid, 25.0);
	Faction_UpdateControl(playerid);
	Fac_ApplyInjuredPose(playerid);
	g_FcInjLeftMs[playerid] = 180000;
	g_FcInjSec[playerid] = 180;
	UI_InjuryHudShow(playerid, 180);
	SendClientMessage(playerid, -10270721, "Sei ferito. Attendi i soccorsi (EMS): senza aiuto verrai ricoverato tra %d secondi ($%d).", 180, 200);
}

Faction_SendHelp(playerid)
{
	new f = gPlayer[playerid][pFaction], rank = gPlayer[playerid][pFactionRank];
	if (f < 1 || rank < 1)
	{
		return 0;
	}
	SendClientMessage(playerid, -2994689, "--- Fazione %s ---", g_FacShort[f]);
	Util_Send(playerid, 0x99CCFFFF, "/fazione [info|membri|lascia] · /duty (vicino alla sede) · /r testo · /dep testo");
	if (f == 1)
	{
		Util_Send(playerid, 0x99CCFFFF, "/ammanetta id · /smanetta id");
		if (rank >= 2)
		{
			Util_Send(playerid, 0x99CCFFFF, "/multa id importo motivo");
		}
	}
	else
	{
		Util_Send(playerid, 0x99CCFFFF, "/cura id · /rianima id");
	}
	if (rank >= 5)
	{
		Util_Send(playerid, 0x99CCFFFF, "/invita id · /promuovi id · /degrada id · /licenzia id");
	}
}
