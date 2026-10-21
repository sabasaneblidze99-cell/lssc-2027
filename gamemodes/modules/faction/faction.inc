new g_FacShort[3][8] = {
	"-",
	"LSPD",
	"EMS"
};
new g_FacRankName[3][7][64] = {
	{"-", "-", "-", "-", "-", "-", "-"},
	{"-", "კადეტი", "ოფიცერი", "უფროსი ოფიცერი", "სერჟანტი", "ლეიტენანტი", "დეპარტამენტის უფროსი"},
	{"-", "მოხალისე", "მაშველი", "პარამედიკი", "ბრიგადირი", "ექიმი", "მთავარი ექიმი"}
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
	{0.0, 0.0, 0.0},
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
	g_FcCmdSet[playerid] = false;
	g_FcCmdLast[playerid] = 0;
	g_FcDutySet[playerid] = false;
	g_FcDutyLast[playerid] = 0;
	g_FcRadioSet[playerid] = false;
	g_FcRadioLast[playerid] = 0;
	g_FcCuffSet[playerid] = false;
	g_FcCuffLast[playerid] = 0;
	g_FcHealSet[playerid] = false;
	g_FcHealLast[playerid] = 0;
	g_FcHealTgtSet[playerid] = false;
	g_FcHealTgtLast[playerid] = 0;
	g_FcPrevArmor[playerid] = 0.0;
	g_FcPayMs[playerid] = 0;
	g_FcStillMs[playerid] = 0;
	g_FcLastPos[playerid][0] = 0.0;
	g_FcLastPos[playerid][1] = 0.0;
	g_FcLastPos[playerid][2] = 0.0;
	g_FcKit[playerid] = 0;
	g_FcKitSet[playerid] = false;
	g_FcKitRefillLast[playerid] = 0;
	g_FcCuffBy[playerid] = 65535;
	g_FcCuffTick[playerid] = 0;
	g_FcInjLeftMs[playerid] = 0;
	g_FcInjSec[playerid] = -1;
	g_FcHospPending[playerid] = false;
	g_FcRevTarget[playerid] = 65535;
	g_FcRevTargetChar[playerid] = 0;
	g_FcRevStart[playerid] = 0;
	g_FcReviveBucket[playerid] = 0;
	g_FcReviveCount[playerid] = 0;
	g_FcInvActive[playerid] = false;
	g_FcInvFrom[playerid] = 65535;
	g_FcInvFromChar[playerid] = 0;
	g_FcInvFac[playerid] = 0;
	g_FcInvStart[playerid] = 0;
	g_FcJoinPending[playerid] = false;
	g_FcFineBusy[playerid] = false;
	g_FcFineTarget[playerid] = 0;
	g_FcFineReason[playerid][0] = 0;
	g_FcFinePend[playerid] = false;
	g_FcFineTgtSet[playerid] = false;
	g_FcFineTgtLast[playerid] = 0;
	g_FcFineBucket[playerid] = 0;
	g_FcFineSum[playerid] = 0;
	g_FcMemBusy[playerid] = false;
	g_FcMemBusyTick[playerid] = 0;
	g_FcMemMode[playerid] = 0;
	g_FcMemCount[playerid] = 0;
	g_FcOpName[playerid][0] = 0;
	g_FcSetActive[playerid] = false;
	g_FcSetTargetChar[playerid] = 0;
	g_FcSetTargetAcc[playerid] = 0;
	g_FcSetStaffAcc[playerid] = 0;
	g_FcSetFac[playerid] = 0;
	g_FcSetRank[playerid] = 0;
	g_FcSetName[playerid][0] = 0;
	g_FcSetStaffName[playerid][0] = 0;
	return 0;
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
	return 0;
}

Faction_UpdateControl(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new bool:lock = gPlayer[playerid][pFrozen] || gPlayer[playerid][pCuffed] || gPlayer[playerid][pWounded];
	TogglePlayerControllable(playerid, !lock);
	return 0;
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
		SendClientMessage(playerid, -10270721, "ფრაქციის შემდეგ ბრძანებამდე მოიცადე %d წამი.", Fac_SecCeil(left));
		return true;
	}
	g_FcCmdSet[playerid] = true;
	g_FcCmdLast[playerid] = GetTickCount();
	return false;
}

bool:Fac_NeedMember(playerid, faction)
{
	new f = gPlayer[playerid][pFaction];
	if (f < 1 || f > 2 || gPlayer[playerid][pFactionRank] < 1 || (faction != 0 && f != faction))
	{
		Util_Send(playerid, -10270721, "ამ ბრძანების გამოყენების უფლება არ გაქვს.");
		return false;
	}
	return true;
}

bool:Fac_NeedActive(playerid)
{
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Util_Send(playerid, -10270721, "ამ მდგომარეობაში ამას ვერ გააკეთებ.");
		return false;
	}
	return true;
}

bool:Fac_NeedDuty(playerid)
{
	if (!gPlayer[playerid][pOnDuty])
	{
		Util_Send(playerid, -10270721, "სამსახურში უნდა იყო (/duty შტაბთან).");
		return false;
	}
	return true;
}

bool:Fac_ParseTarget(playerid, const token[], &targetid)
{
	if (!Util_ParsePlayerId(token, targetid) || !Util_IsPlaying(targetid))
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return false;
	}
	return true;
}

Fac_Emote(playerid, const text[])
{
	new name[75], msg[256];
	GetPlayerName(playerid, name, 75);
	format(msg, 256, "* %s %s", name, text);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
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

Fac_Radio(faction, bool:both, color, const text[])
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
		GivePlayerWeapon(playerid, WEAPON_NITESTICK, 1);
		GivePlayerWeapon(playerid, WEAPON_COLT45, 51);
		GivePlayerWeapon(playerid, WEAPON_SPRAYCAN, 100);
		SetPlayerArmour(playerid, 50.0);
		return 0;
	}
	if (!g_FcKitSet[playerid] || Fac_CdLeftMs(true, g_FcKitRefillLast[playerid], 900000) == 0)
	{
		g_FcKitSet[playerid] = true;
		g_FcKit[playerid] = 8;
		g_FcKitRefillLast[playerid] = GetTickCount();
	}
	return 0;
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
	return 0;
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
	new rn[72];
	Fac_RankName(f, gPlayer[playerid][pFactionRank], rn, 72);
	SendClientMessage(playerid, 0x66DD66FF, "სამსახურში ხარ (%s - %s).", g_FacShort[f], rn);
	Fac_Emote(playerid, "იცვამს ფორმას და სამსახურს იწყებს");
}

Faction_CmdDuty(playerid)
{
	if (!Fac_NeedMember(playerid, 0) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed] || Job_IsOnShift(playerid))
	{
		Util_Send(playerid, -10270721, "სამსახურის შეცვლა ახლა არ შეგიძლია (დაჭრილი, ბორკილდადებული ან ცვლაზე ხარ).");
		return 0;
	}
	new f = gPlayer[playerid][pFaction];
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0 || (GetPlayerDistanceFromPoint(playerid, g_FacHq[f][0], g_FacHq[f][1], g_FacHq[f][2]) > 15.0))
	{
		Util_Send(playerid, -10270721, "შენი ფრაქციის შტაბთან უნდა იყო.");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcDutySet[playerid], g_FcDutyLast[playerid], 120000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "სამსახურის შეცვლამდე მოიცადე კიდევ %d წამი.", Fac_SecCeil(left));
		return 0;
	}
	if (!gPlayer[playerid][pOnDuty] && Admin_CheckMuted(playerid))
	{
		return 0;
	}
	g_FcDutySet[playerid] = true;
	g_FcDutyLast[playerid] = GetTickCount();
	if (gPlayer[playerid][pOnDuty])
	{
		Faction_ForceOff(playerid, true);
		Util_Send(playerid, 0x66DD66FF, "სამსახურიდან გამოხვედი.");
		Fac_Emote(playerid, "იხდის ფორმას და სამსახურს ასრულებს");
	}
	else
	{
		Fac_DutyOn(playerid);
	}
	return 0;
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
		SendClientMessage(playerid, 0x66DD66FF, "%s ხელფასი: $%d ჩაირიცხა ბანკში (paycheck-ის გარდა). ბალანსი: $%d.", g_FacShort[f], paid, gPlayer[playerid][pBank]);
		Char_Save(playerid);
	}
	else
	{
		Util_Send(playerid, -10270721, "საბანკო ანგარიში ლიმიტზეა: ფრაქციის ხელფასი არ ჩაირიცხა.");
	}
	return 0;
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
		SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);
		ClearAnimations(targetid, SYNC_NONE);
		Faction_UpdateControl(targetid);
		Util_Send(targetid, 0x66DD66FF, "ბორკილები მოგხსნეს.");
	}
	return 0;
}

Faction_CmdAmmanetta(playerid, const params[], bool:cuff)
{
	if (!Fac_NeedMember(playerid, 1) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[36], target = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, cuff ? "გამოყენება: /ammanetta id" : "გამოყენება: /smanetta id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 2.0))
	{
		Util_Send(playerid, -10270721, "სამიზნე არასწორია ან ძალიან შორსაა.");
		return 0;
	}
	new tn[75], an[75], msg[384];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(target, tn, 75);
	if (cuff)
	{
		new left = Fac_CdLeftMs(g_FcCuffSet[playerid], g_FcCuffLast[playerid], 10000);
		if (left > 0)
		{
			SendClientMessage(playerid, -10270721, "დაელოდე კიდევ %d წამი.", Fac_SecCeil(left));
			return 0;
		}
		if (gPlayer[target][pCuffed] || gPlayer[target][pWounded] || GetPlayerState(target) != PLAYER_STATE_ONFOOT)
		{
			Util_Send(playerid, -10270721, "სამიზნეს ბორკილს ვერ დაადებ (უკვე ბორკილდადებულია, დაჭრილია ან ფეხით არ არის).");
			return 0;
		}
		g_FcCuffSet[playerid] = true;
		g_FcCuffLast[playerid] = GetTickCount();
		gPlayer[target][pCuffed] = 1;
		g_FcCuffBy[target] = playerid;
		g_FcCuffTick[target] = GetTickCount();
		Job_Cancel(target, "ბორკილი დაგადეს.");
		Faction_UpdateControl(target);
		SetPlayerSpecialAction(target, SPECIAL_ACTION_CUFFED);
		format(msg, 384, "%s-ს ბორკილს ადებს", tn);
		Fac_Emote(playerid, msg);
		DB_FactionLogAdd(1, gPlayer[playerid][pCharId], gPlayer[target][pCharId], "cuff", "");
		return 0;
	}
	if (!gPlayer[target][pCuffed])
	{
		Util_Send(playerid, -10270721, "სამიზნე ბორკილდადებული არ არის.");
		return 0;
	}
	Fac_Uncuff(target);
	format(msg, 384, "%s-ს ბორკილს ხსნის", tn);
	Fac_Emote(playerid, msg);
	DB_FactionLogAdd(1, gPlayer[playerid][pCharId], gPlayer[target][pCharId], "uncuff", "");
	return 0;
}

Faction_CmdRadio(playerid, const params[], bool:interdept)
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
		Util_Send(playerid, 0x99CCFFFF, interdept ? "გამოყენება: /dep ტექსტი" : "გამოყენება: /r ტექსტი");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcRadioSet[playerid], g_FcRadioLast[playerid], 1000);
	if (left > 0)
	{
		Util_Send(playerid, -10270721, "რაციით ცოტა ნელა.");
		return 0;
	}
	g_FcRadioSet[playerid] = true;
	g_FcRadioLast[playerid] = GetTickCount();
	new f = gPlayer[playerid][pFaction], rn[72], name[75], msg[256];
	Fac_RankName(f, gPlayer[playerid][pFactionRank], rn, 72);
	GetPlayerName(playerid, name, 75);
	if (interdept)
	{
		format(msg, 256, "[DIP] %s %s %s: %s", g_FacShort[f], rn, name, text);
		Fac_Radio(f, true, -4698625, msg);
	}
	else
	{
		format(msg, 256, "[რაცია] %s %s: %s", rn, name, text);
		Fac_Radio(f, false, 0x8D8DFFFF, msg);
	}
	return 0;
}

Faction_CmdMulta(playerid, const params[])
{
	if (!Fac_NeedMember(playerid, 1) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new rank = gPlayer[playerid][pFactionRank], cap = Fac_FineCap(rank);
	if (cap <= 0)
	{
		SendClientMessage(playerid, -10270721, "შენი წოდებით ჯარიმის დაწერა არ შეგიძლია (მინიმუმ მე-2 წოდება).");
		return 0;
	}
	new pos = 0, tid[36], tam[36], target = 0, amount = 0, reason[65];
	Util_NextToken(params, pos, tid, 36);
	Util_NextToken(params, pos, tam, 36);
	if (tid[0] == 0 || tam[0] == 0 || !Util_ParseInt(tam, amount) || !Util_SanitizeText(params[pos], reason, 64))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /multa id თანხა მიზეზი");
		return 0;
	}
	if (strlen(reason) < 3)
	{
		SendClientMessage(playerid, -10270721, "მიზეზი ძალიან მოკლეა (მინიმუმ %d სიმბოლო).", 3);
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
		SendClientMessage(playerid, -10270721, "არასწორი თანხა: შენი წოდებისთვის $%d-დან $%d-მდე.", 50, cap);
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tid, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 5.0))
	{
		Util_Send(playerid, -10270721, "სამიზნე არასწორია ან ძალიან შორსაა.");
		return 0;
	}
	if (g_FcFineBusy[playerid] || g_FcFinePend[target])
	{
		Util_Send(playerid, -10270721, "ჯარიმა უკვე ფორმდება: დაელოდე.");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcFineTgtSet[target], g_FcFineTgtLast[target], 600000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "ეს მოქალაქე ახლახან დაჯარიმდა: სცადე %d წუთში.", (Fac_SecCeil(left) + 60 - 1) / 60);
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
		SendClientMessage(playerid, -10270721, "ჯარიმების საათობრივ ლიმიტს მიაღწიე ($%d).", 10000);
		return 0;
	}
	if (gPlayer[target][pMoney] + gPlayer[target][pBank] < amount)
	{
		Util_Send(playerid, -10270721, "მოქალაქეს საკმარისი თანხა არ აქვს: ჯარიმა უარყოფილია.");
		return 0;
	}
	g_FcFineBusy[playerid] = true;
	g_FcFineTarget[playerid] = gPlayer[target][pCharId];
	g_FcFinePend[target] = true;
	strmid(g_FcFineReason[playerid], reason, 0, strlen(reason), 65);
	DB_AddFine(playerid, gPlayer[playerid][pCharId], gPlayer[target][pCharId], 1, amount, reason);
	return 0;
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
	new reason[195], bool:hasReason = false;
	if (issuer != INVALID_PLAYER_ID)
	{
		if (g_FcFineBusy[issuer] && g_FcFineTarget[issuer] == targetChar)
		{
			g_FcFineBusy[issuer] = false;
			strmid(reason, g_FcFineReason[issuer], 0, strlen(g_FcFineReason[issuer]), 195);
			hasReason = true;
		}
		else
		{
			issuer = INVALID_PLAYER_ID;
		}
	}
	if (target != INVALID_PLAYER_ID)
	{
		g_FcFinePend[target] = false;
	}
	new an[75], msg[384];
	if (issuer != INVALID_PLAYER_ID)
	{
		GetPlayerName(issuer, an, 75);
	}
	else
	{
		format(an, 75, "un agente");
	}
	if (fineId <= 0)
	{
		if (issuer != INVALID_PLAYER_ID)
		{
			Util_Send(issuer, -10270721, fineId < 0 ? "სერვისის შეცდომა: ჯარიმა არ გაფორმდა, თანხა არ ჩამოიჭრა." : "სისტემამ ჯარიმა უარყო: თანხა არ ჩამოიჭრა.");
		}
		return 1;
	}
	if (target == INVALID_PLAYER_ID)
	{
		format(msg, 384, "multa %d ($%d) registrata ma bersaglio char %d non piu' online: nessun addebito", fineId, amount, targetChar);
		Log_Write("FINE", msg);
		if (issuer != INVALID_PLAYER_ID)
		{
			Util_Send(issuer, -10270721, "მოქალაქე გავიდა: ჯარიმა გაფორმდა, მაგრამ თანხის ჩამოჭრა ვერ მოხერხდა.");
		}
		return 1;
	}
	new taken = Fac_Debit(target, amount);
	if (taken < amount)
	{
		format(msg, 384, "multa %d: addebitati $%d su $%d (fondi cambiati durante la registrazione)", fineId, taken, amount);
		Log_Write("FINE", msg);
	}
	g_FcFineTgtSet[target] = true;
	g_FcFineTgtLast[target] = GetTickCount();
	Char_Save(target);
	SendClientMessage(target, -10270721, "მიიღე $%d-ის ჯარიმა %s-სგან%s%s.", taken, an, hasReason ? ": " : "", reason);
	if (issuer != INVALID_PLAYER_ID)
	{
		new bucket = gPlayer[issuer][pSessionMs] / 3600000;
		if (bucket != g_FcFineBucket[issuer])
		{
			g_FcFineBucket[issuer] = bucket;
			g_FcFineSum[issuer] = 0;
		}
		g_FcFineSum[issuer] += amount;
		new tn[75];
		GetPlayerName(target, tn, 75);
		SendClientMessage(issuer, 0x66DD66FF, "ჯარიმა #%d, $%d, %s-ზე გაფორმდა.", fineId, taken, tn);
		format(msg, 384, "%s-ს ჯარიმას გადასცემს", tn);
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
	return 0;
}

public Fine_OnDBLoaded(playerid, count)
{
	if (!Fac_ValidPlayer(playerid))
	{
		return 1;
	}
	if (count < 0)
	{
		Util_Send(playerid, -10270721, "სერვისი დროებით მიუწვდომელია: სცადე მოგვიანებით.");
		return 1;
	}
	if (count == 0)
	{
		Util_Send(playerid, -1, "გაფორმებული ჯარიმები არ გაქვს.");
		return 1;
	}
	SendClientMessage(playerid, -2994689, "--- ბოლო მიღებული ჯარიმები ---");
	new now = gettime(), ago[72];
	for (new i = 0; i < count && i < 5; i++)
	{
		Admin_FormatDuration(now - g_FineRows[i][frTimestamp], ago, 72);
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

Faction_CmdCura(playerid, const params[])
{
	if (!Fac_NeedMember(playerid, 2) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[36], target = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /cura id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "სამიზნე არასწორია ან ძალიან შორსაა.");
		return 0;
	}
	if (gPlayer[target][pWounded])
	{
		Util_Send(playerid, -10270721, "სამიზნე დაჭრილია: გამოიყენე /rianima.");
		return 0;
	}
	if (g_FcKit[playerid] <= 0)
	{
		Util_Send(playerid, -10270721, "სამედიცინო ნაკრები ამოიწურა (ივსება /duty on-ით).");
		return 0;
	}
	new left = Fac_CdLeftMs(g_FcHealSet[playerid], g_FcHealLast[playerid], 20000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "დაელოდე კიდევ %d წამი.", Fac_SecCeil(left));
		return 0;
	}
	left = Fac_CdLeftMs(g_FcHealTgtSet[target], g_FcHealTgtLast[target], 60000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "პაციენტს ახლახან უმკურნალეს: დაელოდე %d წამი.", Fac_SecCeil(left));
		return 0;
	}
	new Float:hp;
	GetPlayerHealth(target, hp);
	if ((hp >= 100.0))
	{
		Util_Send(playerid, -10270721, "სამიზნე სრულად ჯანმრთელია.");
		return 0;
	}
	hp = (40.0 + hp);
	if ((hp > 100.0))
	{
		hp = 100.0;
	}
	SetPlayerHealth(target, hp);
	g_FcHealSet[playerid] = true;
	g_FcHealLast[playerid] = GetTickCount();
	g_FcHealTgtSet[target] = true;
	g_FcHealTgtLast[target] = GetTickCount();
	Fac_UseCharge(playerid);
	new tn[75], msg[384];
	GetPlayerName(target, tn, 75);
	format(msg, 384, "%s-ს პირველ დახმარებას უწევს", tn);
	Fac_Emote(playerid, msg);
	SendClientMessage(target, 0x66DD66FF, "გიმკურნალეს (ჯანმრთელობა %.0f).", hp);
	DB_FactionLogAdd(2, gPlayer[playerid][pCharId], gPlayer[target][pCharId], "heal", "");
	return 0;
}

Fac_ReviveCancel(medic, const reason[])
{
	g_FcRevTarget[medic] = 65535;
	g_FcRevTargetChar[medic] = 0;
	if (IsPlayerConnected(medic))
	{
		ClearAnimations(medic, SYNC_NONE);
		if (reason[0] != 0)
		{
			Util_Send(medic, -10270721, reason);
		}
	}
}

Faction_CmdRianima(playerid, const params[])
{
	if (!Fac_NeedMember(playerid, 2) || !Fac_NeedDuty(playerid) || !Fac_NeedActive(playerid) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[36], target = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /rianima id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (target == playerid || !Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "სამიზნე არასწორია ან ძალიან შორსაა.");
		return 0;
	}
	if (!gPlayer[target][pWounded])
	{
		Util_Send(playerid, -10270721, "სამიზნე დაჭრილი არ არის.");
		return 0;
	}
	if (g_FcKit[playerid] <= 0)
	{
		Util_Send(playerid, -10270721, "სამედიცინო ნაკრები ამოიწურა (ივსება /duty on-ით).");
		return 0;
	}
	if (g_FcRevTarget[playerid] != 65535)
	{
		Util_Send(playerid, -10270721, "უკვე ვიღაცას აცოცხლებ.");
		return 0;
	}
	g_FcRevTarget[playerid] = target;
	g_FcRevTargetChar[playerid] = gPlayer[target][pCharId];
	g_FcRevStart[playerid] = GetTickCount();
	ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, true, false, false, false, 0, SYNC_NONE);
	new tn[75], msg[384];
	GetPlayerName(target, tn, 75);
	format(msg, 384, "%s-ს გაცოცხლებას იწყებს", tn);
	Fac_Emote(playerid, msg);
	SendClientMessage(playerid, -1, "მიმდინარეობს გაცოცხლება (%d წამი): არ გაშორდე.", 8);
	return 0;
}

Fac_ReviveDone(medic, target)
{
	g_FcRevTarget[medic] = 65535;
	g_FcRevTargetChar[medic] = 0;
	ClearAnimations(medic, SYNC_NONE);
	gPlayer[target][pWounded] = 0;
	g_FcInjLeftMs[target] = 0;
	SetPlayerHealth(target, 60.0);
	ClearAnimations(target, SYNC_NONE);
	Faction_UpdateControl(target);
	UI_InjuryEndShow(target, 2, 0);
	Fac_UseCharge(medic);
	new mn[75], tn[75], msg[384];
	GetPlayerName(medic, mn, 75);
	GetPlayerName(target, tn, 75);
	SendClientMessage(target, 0x66DD66FF, "%s-მ გაგაცოცხლა.", mn);
	format(msg, 384, "%s-ს აცოცხლებს", tn);
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
		SendClientMessage(medic, 0x66DD66FF, "გაცოცხლების ბონუსი: $%d ჩაირიცხა ბანკში.", 100);
		Char_Save(medic);
	}
}

Fac_ApplyInjuredPose(playerid)
{
	ApplyAnimation(playerid, "CRACK", "crckidle2", 4.1, true, false, false, true, 0, SYNC_NONE);
}

Faction_Hospitalize(playerid)
{
	new fee = Fac_Debit(playerid, 200);
	gPlayer[playerid][pWounded] = 0;
	g_FcInjLeftMs[playerid] = 0;
	g_FcHospPending[playerid] = false;
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerPos(playerid, 1182.0, -1330.0, 13.9);
	SetPlayerFacingAngle(playerid, 270.0);
	SetCameraBehindPlayer(playerid);
	ClearAnimations(playerid, SYNC_NONE);
	SetPlayerHealth(playerid, 100.0);
	Faction_UpdateControl(playerid);
	UI_InjuryEndShow(playerid, 1, fee);
	Char_Save(playerid);
	new name[75], msg[384];
	GetPlayerName(playerid, name, 75);
	format(msg, 384, "ricovero %s: addebitati $%d", name, fee);
	Log_Write("INJURY", msg);
}

Fac_ClearInvite(targetid, bool:closeDialog)
{
	g_FcInvActive[targetid] = false;
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
		Util_Send(playerid, -10270721, "მოწვევის უფლება არ გაქვს.");
		return 0;
	}
	if (!Fac_NeedActive(playerid))
	{
		return 0;
	}
	if (!Util_IsPlaying(target) || target == playerid)
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
		return 0;
	}
	if (!Util_InRange(playerid, target, 5.0))
	{
		Util_Send(playerid, -10270721, "მოთამაშე ძალიან შორსაა.");
		return 0;
	}
	if (gPlayer[target][pFaction] != 0)
	{
		Util_Send(playerid, -10270721, "მოთამაშე უკვე ფრაქციის წევრია.");
		return 0;
	}
	if (gPlayer[target][pLevel] < 3)
	{
		SendClientMessage(playerid, -10270721, "მოთამაშე მინიმუმ %d დონის უნდა იყოს.", 3);
		return 0;
	}
	if ((g_FcInvActive[target] && Fac_CdLeftMs(true, g_FcInvStart[target], 60000) > 0) || g_FcJoinPending[target])
	{
		Util_Send(playerid, -10270721, "მოთამაშეს უკვე აქვს მოლოდინში მყოფი მოწვევა.");
		return 0;
	}
	g_FcInvActive[target] = true;
	g_FcInvFrom[target] = playerid;
	g_FcInvFromChar[target] = gPlayer[playerid][pCharId];
	g_FcInvFac[target] = f;
	g_FcInvStart[target] = GetTickCount();
	new an[75], tn[75];
	GetPlayerName(playerid, an, 75);
	GetPlayerName(target, tn, 75);
	UI_FactionInviteShow(target, f, an, 60);
	SendClientMessage(target, -1, "%s გიწვევს %s-ში. დაეთანხმე პანელიდან ან /accetta-თი (მოქმედებს %d წამი).", an, g_FacShort[f], 60);
	SendClientMessage(playerid, 0x66DD66FF, "მოწვევა გაეგზავნა: %s.", tn);
	return 0;
}

Faction_CmdInvita(playerid, const params[])
{
	if (!Fac_NeedMember(playerid, 0) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[36], target = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /invita id");
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	Fac_DoInvite(playerid, target);
	return 0;
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
	new bool:expired = Fac_CdLeftMs(true, g_FcInvStart[playerid], 60000) <= 0;
	Fac_ClearInvite(playerid, false);
	if (!Util_IsPlaying(playerid))
	{
		return 0;
	}
	if (expired)
	{
		Util_Send(playerid, -10270721, "მოწვევის ვადა ამოიწურა.");
		return 0;
	}
	if (!accept)
	{
		Util_Send(playerid, -1, "მოწვევაზე უარი თქვი.");
		if (Fac_ValidPlayer(from) && gPlayer[from][pCharId] == fromChar)
		{
			Util_Send(from, -1, "მოწვევაზე უარი თქვეს.");
		}
		return 0;
	}
	if (!Util_IsPlaying(from) || gPlayer[from][pCharId] != fromChar || gPlayer[from][pFaction] != f || gPlayer[from][pFactionRank] < 5)
	{
		Util_Send(playerid, -10270721, "ვინც მოგიწვია, აღარ არის ხელმისაწვდომი.");
		return 0;
	}
	if (gPlayer[playerid][pFaction] != 0 || gPlayer[playerid][pLevel] < 3 || g_FcJoinPending[playerid])
	{
		Util_Send(playerid, -10270721, "ახლა ფრაქციაში ვერ გაწევრიანდები.");
		return 0;
	}
	g_FcJoinPending[playerid] = true;
	GetPlayerName(playerid, g_FcOpName[from], 25);
	DB_FactionJoin(from, fromChar, gPlayer[playerid][pCharId], f);
	return 0;
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
		Util_Send(playerid, -10270721, "მოლოდინში მყოფი მოწვევა არ გაქვს.");
		return 0;
	}
	UI_FactionInviteClose(playerid);
	Fac_InviteReply(playerid, true);
	return 0;
}

Fac_DoRankOp(playerid, action, targetChar, targetRank, const targetName[])
{
	new f = gPlayer[playerid][pFaction], own = gPlayer[playerid][pFactionRank];
	if (f < 1 || own < 5)
	{
		Util_Send(playerid, -10270721, "ამის უფლება არ გაქვს.");
		return 0;
	}
	if (!Fac_NeedActive(playerid))
	{
		return 0;
	}
	if (targetChar <= 0 || targetChar == gPlayer[playerid][pCharId])
	{
		Util_Send(playerid, -10270721, "არასწორი სამიზნე.");
		return 0;
	}
	if (targetRank < 1 || targetRank >= own)
	{
		Util_Send(playerid, -10270721, "მხოლოდ შენზე დაბალი წოდების წევრებზე შეგიძლია მოქმედება.");
		return 0;
	}
	strmid(g_FcOpName[playerid], targetName, 0, strlen(targetName), 25);
	switch (action)
	{
		case 4:
		{
			if (targetRank + 1 >= own)
			{
				Util_Send(playerid, -10270721, "ახალი წოდება შენსაზე დაბალი უნდა იყოს.");
				return 0;
			}
			DB_FactionPromote(playerid, gPlayer[playerid][pCharId], targetChar, f, targetRank + 1);
		}
		case 5:
		{
			if (targetRank <= 1)
			{
				Util_Send(playerid, -10270721, "წევრი უკვე მინიმალურ წოდებაზეა.");
				return 0;
			}
			DB_FactionDemote(playerid, gPlayer[playerid][pCharId], targetChar, f, targetRank - 1);
		}
		case 6:
		{
			DB_FactionFire(playerid, gPlayer[playerid][pCharId], targetChar, f);
		}
	}
	return 0;
}

Faction_CmdRankOp(playerid, const params[], action)
{
	if (!Fac_NeedMember(playerid, 0) || Fac_CmdBlocked(playerid))
	{
		return 0;
	}
	new pos = 0, tok[36], target = 0;
	Util_NextToken(params, pos, tok, 36);
	if (tok[0] == 0)
	{
		Util_Send(playerid, 0x99CCFFFF, action == 4 ? "გამოყენება: /promuovi id" : (action == 5 ? "გამოყენება: /degrada id" : "გამოყენება: /licenzia id"));
		return 0;
	}
	if (!Fac_ParseTarget(playerid, tok, target))
	{
		return 0;
	}
	if (gPlayer[target][pFaction] != gPlayer[playerid][pFaction])
	{
		Util_Send(playerid, -10270721, "მოთამაშე შენი ფრაქციის წევრი არ არის.");
		return 0;
	}
	new tn[75];
	GetPlayerName(target, tn, 75);
	Fac_DoRankOp(playerid, action, gPlayer[target][pCharId], gPlayer[target][pFactionRank], tn);
	return 0;
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
	Util_Send(playerid, -10270721, "წევრი სიაში აღარ არის: თავიდან გახსენი მენიუ.");
	return 1;
}

Fac_ApplyChange(target, cmd, faction, rank)
{
	new oldFac = gPlayer[target][pFaction], rn[72];
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
			Util_Send(target, -1, "სამსახურიდან გაგიყვანეს.");
		}
		else
		{
			Fac_ApplyUniform(target);
			UI_FactionHudShow(target, nf, nr);
			UI_FactionHudUpdate(target, nf, nr, Fac_KitHudValue(target));
		}
	}
	Fac_RankName(nf, nr, rn, 72);
	switch (cmd)
	{
		case 1:
		{
			SendClientMessage(target, 0x66DD66FF, "შეხვედი %s-ში, წოდება: %s.", g_FacShort[nf], rn);
		}
		case 2:
		{
			SendClientMessage(target, 0x66DD66FF, "დაგაწინაურეს: %s (%s).", rn, g_FacShort[nf]);
		}
		case 3:
		{
			SendClientMessage(target, -10270721, "ჩამოგაქვეითეს: %s (%s).", rn, g_FacShort[nf]);
		}
		case 4:
		{
			SendClientMessage(target, -10270721, "გაგათავისუფლეს: %s.", g_FacShort[faction]);
		}
		case 5:
		{
			SendClientMessage(target, -1, "დატოვე %s.", g_FacShort[faction]);
		}
		case 6:
		{
			if (nf == 0)
			{
				Util_Send(target, -5963521, "ადმინისტრატორმა ფრაქციიდან ამოგშალა.");
			}
			else
			{
				SendClientMessage(target, -5963521, "ადმინისტრატორმა მიგაკუთვნა %s-ს, წოდება: %s.", g_FacShort[nf], rn);
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
		g_FcJoinPending[target] = false;
	}
	new req = Fac_ValidPlayer(playerid) ? playerid : 65535;
	new ctx = cmd == 6 ? Fac_SetCtxByTarget(targetChar) : 65535;
	if (result == 1)
	{
		if (target != INVALID_PLAYER_ID)
		{
			Fac_ApplyChange(target, cmd, faction, rank);
		}
		new rn[72], tn[75];
		if (target != INVALID_PLAYER_ID)
		{
			GetPlayerName(target, tn, 75);
		}
		else if (req != INVALID_PLAYER_ID)
		{
			strmid(tn, g_FcOpName[req], 0, strlen(g_FcOpName[req]), 75);
		}
		if (ctx != 65535)
		{
			strmid(tn, g_FcSetName[ctx], 0, strlen(g_FcSetName[ctx]), 75);
		}
		Fac_RankName(faction, rank, rn, 72);
		if (cmd == 6 && ctx != 65535)
		{
			new detail[384], logDetail[384];
			if (faction == 0)
			{
				format(detail, 384, "ფრაქციიდან ამოშლილია");
			}
			else
			{
				format(detail, 384, "%s, წოდება %d", g_FacShort[faction], rank);
			}
			format(logDetail, 384, "%s: %s", tn, detail);
			Admin_LogRaw(g_FcSetStaffAcc[ctx], g_FcSetStaffName[ctx], g_FcSetTargetAcc[ctx], "setfazione", logDetail);
			Admin_AdmCmd(g_FcSetStaffName[ctx], "დაუყენა ფრაქცია", tn, detail, "");
		}
		else if (req != INVALID_PLAYER_ID)
		{
			switch (cmd)
			{
				case 1:
				{
					SendClientMessage(req, 0x66DD66FF, "%s შემოუერთდა %s-ს.", tn, g_FacShort[faction]);
				}
				case 2:
				{
					SendClientMessage(req, 0x66DD66FF, "%s დაწინაურდა: %s.", tn, rn);
				}
				case 3:
				{
					SendClientMessage(req, 0x66DD66FF, "%s ჩამოქვეითდა: %s.", tn, rn);
				}
				case 4:
				{
					SendClientMessage(req, 0x66DD66FF, "%s გათავისუფლდა.", tn);
				}
			}
		}
		new msg[384];
		format(msg, 384, "cmd %d char %d fazione %d grado %d", cmd, targetChar, faction, rank);
		Log_Write("FACTION", msg);
	}
	else
	{
		new notify = ctx != 65535 && IsPlayerConnected(ctx) ? ctx : req;
		if (notify != 65535)
		{
			if (result < 0)
			{
				Util_Send(notify, -10270721, "სერვისი დროებით მიუწვდომელია: ოპერაცია არ შესრულდა.");
			}
			else
			{
				Util_Send(notify, -10270721, "ოპერაცია უარყოფილია (წოდება, ფრაქცია ან დონე არ შეესაბამება, ან სხვა ოპერაცია მიმდინარეობს).");
			}
		}
		if (target != INVALID_PLAYER_ID && cmd == 1)
		{
			Util_Send(target, -10270721, "ფრაქციაში გაწევრიანება ვერ დასრულდა.");
		}
	}
	if (ctx != 65535)
	{
		g_FcSetActive[ctx] = false;
	}
	return 1;
}

Fac_RequestMembers(playerid, mode)
{
	new f = gPlayer[playerid][pFaction];
	if (f < 1)
	{
		Util_Send(playerid, -10270721, "არცერთი ფრაქციის წევრი არ ხარ.");
		return 0;
	}
	if (g_FcMemBusy[playerid] && Fac_CdLeftMs(true, g_FcMemBusyTick[playerid], 10000) > 0)
	{
		Util_Send(playerid, -10270721, "მოთხოვნა უკვე მუშავდება: დაელოდე.");
		return 0;
	}
	g_FcMemBusy[playerid] = true;
	g_FcMemBusyTick[playerid] = GetTickCount();
	g_FcMemMode[playerid] = mode;
	DB_FactionMembers(playerid, f);
	return 0;
}

Faction_CmdFazione(playerid, const params[])
{
	if (gPlayer[playerid][pFaction] < 1 || gPlayer[playerid][pFactionRank] < 1)
	{
		Util_Send(playerid, -10270721, "არცერთი ფრაქციის წევრი არ ხარ.");
		return 0;
	}
	new pos = 0, sub[48];
	Util_NextToken(params, pos, sub, 48);
	if (sub[0] == 0 || !strcmp(sub, "info", true, 0x7FFFFFFF))
	{
		Fac_RequestMembers(playerid, 0);
		return 0;
	}
	if (!strcmp(sub, "membri", true, 0x7FFFFFFF))
	{
		Fac_RequestMembers(playerid, 2);
		return 0;
	}
	if (!strcmp(sub, "lascia", true, 0x7FFFFFFF))
	{
		if (Fac_CmdBlocked(playerid) || !Fac_NeedActive(playerid))
		{
			return 0;
		}
		GetPlayerName(playerid, g_FcOpName[playerid], 25);
		DB_FactionLeave(playerid, gPlayer[playerid][pCharId], gPlayer[playerid][pFaction]);
		return 0;
	}
	Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /fazione [info|membri|lascia]");
	return 0;
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
	g_FcMemBusy[playerid] = false;
	if (count < 0)
	{
		Util_Send(playerid, -10270721, "სერვისი დროებით მიუწვდომელია: სცადე მოგვიანებით.");
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
		UI_FactionMenuShow(playerid, faction, gPlayer[playerid][pFactionRank], bool:gPlayer[playerid][pOnDuty], online, count);
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

Fac_SetStart(staff, targetChar, targetAcc, const targetName[], faction, rank)
{
	g_FcSetActive[staff] = true;
	g_FcSetTargetChar[staff] = targetChar;
	g_FcSetTargetAcc[staff] = targetAcc;
	g_FcSetStaffAcc[staff] = gPlayer[staff][pAccountId];
	g_FcSetFac[staff] = faction;
	g_FcSetRank[staff] = rank;
	strmid(g_FcSetName[staff], targetName, 0, strlen(targetName), 25);
	GetPlayerName(staff, g_FcSetStaffName[staff], 25);
	DB_FactionSet(staff, targetChar, faction, rank);
}

bool:Faction_CmdSetfazione(playerid, const params[])
{
	if (Util_GetAdmin(playerid) < 3)
	{
		return false;
	}
	new pos = 0, who[75], tf[36], tr[36], faction = 0, rank = 0;
	Util_NextToken(params, pos, who, 75);
	Util_NextToken(params, pos, tf, 36);
	Util_NextToken(params, pos, tr, 36);
	if (who[0] == 0 || !Util_ParseInt(tf, faction) || faction < 0 || faction > 2 || (faction != 0 && (!Util_ParseInt(tr, rank) || rank < 1 || rank > 6)))
	{
		SendClientMessage(playerid, 0x99CCFFFF, "გამოყენება: /setfazione id|სახელი_გვარი ფრაქცია(%d-%d, 0 = ამოშლა) წოდება(1-%d)", 0, 2, 6);
		return true;
	}
	if (faction == 0)
	{
		rank = 0;
	}
	if (g_FcSetActive[playerid])
	{
		Util_Send(playerid, -10270721, "უკვე გაქვს მიმდინარე /setfazione ოპერაცია.");
		return true;
	}
	new targetid = INVALID_PLAYER_ID;
	if (Util_ParsePlayerId(who, targetid))
	{
		if (!Util_IsPlaying(targetid) || gPlayer[targetid][pCharId] <= 0)
		{
			Util_Send(playerid, -10270721, "არასწორი მოთამაშე.");
			return true;
		}
	}
	else
	{
		targetid = Admin_FindByName(who);
	}
	if (targetid != INVALID_PLAYER_ID && gPlayer[targetid][pLoggedIn] && gPlayer[targetid][pCharId] > 0)
	{
		new tn[75];
		GetPlayerName(targetid, tn, 75);
		Fac_SetStart(playerid, gPlayer[targetid][pCharId], gPlayer[targetid][pAccountId], tn, faction, rank);
		return true;
	}
	if (!Util_IsValidRPName(who))
	{
		Util_Send(playerid, -10270721, "არასწორი მოთამაშე (მიუთითე id ან სახელი_გვარი).");
		return true;
	}
	g_FcSetActive[playerid] = true;
	g_FcSetTargetChar[playerid] = 0;
	g_FcSetTargetAcc[playerid] = 0;
	g_FcSetStaffAcc[playerid] = gPlayer[playerid][pAccountId];
	g_FcSetFac[playerid] = faction;
	g_FcSetRank[playerid] = rank;
	strmid(g_FcSetName[playerid], who, 0, strlen(who), 25);
	GetPlayerName(playerid, g_FcSetStaffName[playerid], 25);
	DB_FactionFindByName(playerid, who, 1);
	return true;
}

public Faction_OnDBFound(playerid, tag, charId, const name[], faction, rank, level)
{
	if (tag != 1 || !Fac_ValidPlayer(playerid) || !g_FcSetActive[playerid] || g_FcSetTargetChar[playerid] != 0)
	{
		return 1;
	}
	if (charId <= 0)
	{
		g_FcSetActive[playerid] = false;
		Util_Send(playerid, -10270721, charId < 0 ? "სერვისი დროებით მიუწვდომელია: სცადე მოგვიანებით." : "პერსონაჟი ვერ მოიძებნა.");
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
		if (g_FcInvActive[i] && Fac_CdLeftMs(true, g_FcInvStart[i], 60000) <= 0)
		{
			new from = g_FcInvFrom[i], fromChar = g_FcInvFromChar[i];
			Fac_ClearInvite(i, true);
			Util_Send(i, -1, "ფრაქციის მოწვევის ვადა ამოიწურა.");
			if (Fac_ValidPlayer(from) && gPlayer[from][pCharId] == fromChar)
			{
				Util_Send(from, -1, "შენი მოწვევის ვადა ამოიწურა.");
			}
		}
		if (!gPlayer[i][pSpawned] || !Util_IsPlaying(i))
		{
			continue;
		}
		if (gPlayer[i][pCuffed])
		{
			new by = g_FcCuffBy[i];
			if (Fac_CdLeftMs(true, g_FcCuffTick[i], 600000) <= 0 || !Util_IsPlaying(by) || !gPlayer[by][pOnDuty])
			{
				Fac_Uncuff(i);
			}
		}
		if (gPlayer[i][pOnDuty])
		{
			if (gPlayer[i][pFaction] < 1 || Admin_IsMuted(i))
			{
				Faction_ForceOff(i, true);
				Util_Send(i, -10270721, "სამსახურიდან გაგიყვანეს.");
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
				Fac_ReviveCancel(i, "გაცოცხლება შეწყდა.");
			}
			else if (!Util_IsPlaying(t) || gPlayer[t][pCharId] != g_FcRevTargetChar[i] || !gPlayer[t][pWounded])
			{
				Fac_ReviveCancel(i, "გაცოცხლება შეწყდა: პაციენტი აღარ არის დაჭრილი.");
			}
			else if (!Util_InRange(i, t, (3.0 + 1.0)))
			{
				Fac_ReviveCancel(i, "გაცოცხლება შეწყდა: დაშორდი.");
			}
			else if (now - g_FcRevStart[i] >= 8000)
			{
				if (g_FcKit[i] > 0)
				{
					Fac_ReviveDone(i, t);
				}
				else
				{
					Fac_ReviveCancel(i, "სამედიცინო ნაკრები ამოიწურა.");
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
	g_FacTimer = SetTimer("Faction_OnTick", 500, true);
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
		Faction_ForceOff(playerid, true);
	}
	if (g_FcInvActive[playerid])
	{
		Fac_ClearInvite(playerid, false);
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FcInvActive[i] && g_FcInvFrom[i] == playerid)
		{
			Fac_ClearInvite(i, true);
		}
		if (g_FcRevTarget[i] == playerid)
		{
			Fac_ReviveCancel(i, "გაცოცხლება შეწყდა: პაციენტი გავიდა.");
		}
	}
	if (gPlayer[playerid][pWounded])
	{
		gPlayer[playerid][pWounded] = 0;
		gPlayer[playerid][pPosX] = 1182.0;
		gPlayer[playerid][pPosY] = -1330.0;
		gPlayer[playerid][pPosZ] = 13.9;
		gPlayer[playerid][pAngle] = 270.0;
		gPlayer[playerid][pInterior] = 0;
		gPlayer[playerid][pWorld] = 0;
		gPlayer[playerid][pHealth] = 100.0;
		gPlayer[playerid][pArmor] = 0.0;
		gPlayer[playerid][pSpawned] = 0;
	}
	gPlayer[playerid][pSkin] = gPlayer[playerid][pCivilSkin];
	Faction_ResetPlayer(playerid);
	return 0;
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
		Faction_ForceOff(playerid, false);
	}
	g_FcRevTarget[playerid] = 65535;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FcRevTarget[i] == playerid)
		{
			Fac_ReviveCancel(i, "გაცოცხლება შეწყდა.");
		}
	}
	if (gPlayer[playerid][pWounded])
	{
		g_FcHospPending[playerid] = true;
		return 0;
	}
	gPlayer[playerid][pWounded] = 1;
	g_FcInjLeftMs[playerid] = 180000;
	g_FcInjSec[playerid] = -1;
	UI_InjuryHudHide(playerid);
	return 0;
}

Faction_OnPlayerSpawn(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayer[playerid][pSpawned])
	{
		return 0;
	}
	if (gPlayer[playerid][pOnDuty])
	{
		Faction_ForceOff(playerid, true);
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
	SendClientMessage(playerid, -10270721, "დაჭრილი ხარ. დაელოდე დახმარებას (EMS): დახმარების გარეშე %d წამში ჰოსპიტალიზდები ($%d).", 180, 200);
	return 0;
}

Faction_SendHelp(playerid)
{
	new f = gPlayer[playerid][pFaction], rank = gPlayer[playerid][pFactionRank];
	if (f < 1 || rank < 1)
	{
		return 0;
	}
	SendClientMessage(playerid, -2994689, "--- ფრაქცია %s ---", g_FacShort[f]);
	Util_Send(playerid, 0x99CCFFFF, "/fazione [info|membri|lascia] · /duty (შტაბთან) · /r ტექსტი · /dep ტექსტი");
	if (f == 1)
	{
		Util_Send(playerid, 0x99CCFFFF, "/ammanetta id · /smanetta id");
		if (rank >= 2)
		{
			Util_Send(playerid, 0x99CCFFFF, "/multa id თანხა მიზეზი");
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
	return 0;
}
