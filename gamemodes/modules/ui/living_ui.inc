new g_UiLvDistrictName[4][12] = {
	"Tutti",
	"Idlewood",
	"Ganton",
	"Jefferson"
};
new g_UiLvCargoName[4][24] = {
	"Merci varie",
	"Merci generiche",
	"Alimentari",
	"Materiali edili"
};
new g_UiLvKindName[2][24] = {
	"Tutte",
	"Trasporto merci"
};
new g_UiLvTrackName[3][16] = {
	{0},
	"Trasporti",
	"Taxi"
};
new g_UiLvTierName[4][16] = {
	"Principiante",
	"Affidabile",
	"Esperto",
	"Elite"
};
new g_UiLvBlockText[11][48] = {
	{0},
	"Livello insufficiente",
	"Serve piu' reputazione nei trasporti",
	"Serve piu' fiducia nel quartiere",
	"Hai gia' un contratto in corso",
	"Sei gia' impegnato in un altro lavoro",
	"Prenditi una pausa, torna tra poco",
	"Hai lavorato tanto quest'ora",
	"Lasciato da poco: riprovaci piu' tardi",
	"Gia' preso da un altro autista",
	"Non disponibile al momento"
};
new g_UiLvPrjBlockText[5][64] = {
	{0},
	"Potrai donare dopo un po' di tempo trascorso in citta'.",
	"Per oggi hai gia' dato il massimo: grazie davvero.",
	"Questo progetto non raccoglie piu' fondi.",
	"Le donazioni si fanno alla Casa del Quartiere o a una bacheca."
};
new g_UiLvEventName[10][32] = {
	"Evento",
	"Consegna completata",
	"Consegna non riuscita",
	"Corsa in taxi",
	"Noleggio aperto",
	"Noleggio riconsegnato",
	"Crimine segnalato",
	"Pattuglia di passaggio",
	"Progetto della comunita'",
	"Intervento dello staff"
};
new g_UiLvStepTitle[6][40] = {
	{0},
	"Raggiungi la Casa del Quartiere",
	"Leggi /quartiere",
	"Fai la consegna di benvenuto",
	"Apri /reputazione",
	"Scopri /lavoro o /noleggio"
};
new g_UiLvStepText[6][200] = {
	{0},
	"A due passi da Unity Station, sotto la tettoia di Idlewood, c'e' la Casa del Quartiere: la bacheca dei contratti e qualcuno che conosce la zona.",
	"Scopri come sta Idlewood: attivita', sicurezza, le voci della strada. Il quartiere cambia con quello che facciamo tutti.",
	"Accetta alla bacheca la consegna di benvenuto: un carico dal magazzino al negozio di Idlewood. Nessuna fretta, la strada e' breve.",
	"Guarda come ti conosce la citta': il tuo mestiere e il tuo nome nei quartieri crescono con il lavoro fatto bene.",
	"Il Job Center di Pershing Square offre lavori stabili; se ti serve un mezzo, il noleggio e' proprio li' accanto."
};
new g_UiLvStepHint[6][96] = {
	{0},
	"Segui il segnale: la Casa del Quartiere e' vicino alla stazione.",
	"Scrivi /quartiere per conoscere Idlewood.",
	"Alla bacheca, /contratti: c'e' una consegna di benvenuto per te.",
	"Scrivi /reputazione per vedere come ti conosce la citta'.",
	"Ultimo passo: /lavoro o /noleggio, scegli tu."
};
new g_UiLvBdCount[MAX_PLAYERS];
new g_UiLvBdId[MAX_PLAYERS][12];
new g_UiLvBdKind[MAX_PLAYERS][12];
new g_UiLvBdFrom[MAX_PLAYERS][12];
new g_UiLvBdTo[MAX_PLAYERS][12];
new g_UiLvBdCargo[MAX_PLAYERS][12];
new g_UiLvBdReward[MAX_PLAYERS][12];
new g_UiLvBdDist[MAX_PLAYERS][12];
new g_UiLvBdExpAt[MAX_PLAYERS][12];
new bool:g_UiLvBdPremium[MAX_PLAYERS][12];
new g_UiLvBdMinLevel[MAX_PLAYERS][12];
new g_UiLvBdMinRep[MAX_PLAYERS][12];
new g_UiLvBdMinLRep[MAX_PLAYERS][12];
new g_UiLvBdBlock[MAX_PLAYERS][12];
new g_UiLvBdRow[MAX_PLAYERS][12];
new g_UiLvBdRows[MAX_PLAYERS];
new bool:g_UiLvBdAt[MAX_PLAYERS];
new g_UiLvFltDistrict[MAX_PLAYERS];
new g_UiLvFltKind[MAX_PLAYERS];
new bool:g_UiLvFltEligible[MAX_PLAYERS];
new g_UiLvDetIdx[MAX_PLAYERS];
new bool:g_UiLvDetCan[MAX_PLAYERS];
new g_UiLvStId[MAX_PLAYERS];
new bool:g_UiLvHudOn[MAX_PLAYERS];
new g_UiLvHudId[MAX_PLAYERS];
new g_UiLvHudState[MAX_PLAYERS];
new g_UiLvHudDist[MAX_PLAYERS];
new g_UiLvHudExpAt[MAX_PLAYERS];
new g_UiLvHudInteg[MAX_PLAYERS];
new g_UiLvHudDest[MAX_PLAYERS][48];
new g_UiLvDsId[MAX_PLAYERS];
new g_UiLvDsAct[MAX_PLAYERS];
new g_UiLvDsSec[MAX_PLAYERS];
new g_UiLvDsCrime[MAX_PLAYERS];
new g_UiLvDsLRep[MAX_PLAYERS];
new g_UiLvDsFest[MAX_PLAYERS];
new g_UiLvDsRumor[MAX_PLAYERS][96];
new g_UiLvDsEvCount[MAX_PLAYERS];
new g_UiLvDsEvType[MAX_PLAYERS][8];
new g_UiLvDsEvAgo[MAX_PLAYERS][8];
new g_UiLvDsEvValue[MAX_PLAYERS][8];
new bool:g_UiLvRpHas[MAX_PLAYERS][3];
new g_UiLvRpPoints[MAX_PLAYERS][3];
new g_UiLvRpDone[MAX_PLAYERS][3];
new g_UiLvRpFail[MAX_PLAYERS][3];
new g_UiLvRpRSum[MAX_PLAYERS][3];
new g_UiLvRpRCnt[MAX_PLAYERS][3];
new g_UiLvRpDay[MAX_PLAYERS][3];
new bool:g_UiLvRpLHas[MAX_PLAYERS][4];
new g_UiLvRpLocal[MAX_PLAYERS][4];
new g_UiLvRnBlockMin[MAX_PLAYERS];
new g_UiLvRnMaxBlocks[MAX_PLAYERS];
new g_UiLvRnDisc[MAX_PLAYERS];
new g_UiLvRnCount[MAX_PLAYERS];
new g_UiLvRnIdx[MAX_PLAYERS][4];
new g_UiLvRnName[MAX_PLAYERS][4][32];
new g_UiLvRnFee[MAX_PLAYERS][4];
new g_UiLvRnDep[MAX_PLAYERS][4];
new g_UiLvRnLevel[MAX_PLAYERS][4];
new bool:g_UiLvRnAvail[MAX_PLAYERS][4];
new g_UiLvRnSel[MAX_PLAYERS];
new g_UiLvRnBlocks[MAX_PLAYERS];
new g_UiLvRnReturnId[MAX_PLAYERS];
new g_UiLvTxOfferId[MAX_PLAYERS];
new g_UiLvTxOfferLeft[MAX_PLAYERS];
new g_UiLvTxPayId[MAX_PLAYERS];
new g_UiLvTxPayLeft[MAX_PLAYERS];
new g_UiLvTxPayFare[MAX_PLAYERS];
new g_UiLvTxPayDist[MAX_PLAYERS];
new g_UiLvTxPayDisc[MAX_PLAYERS];
new g_UiLvTxPayDriver[MAX_PLAYERS][25];
new g_UiLvTxRateId[MAX_PLAYERS];
new bool:g_UiLvMeterOn[MAX_PLAYERS];
new g_UiLvPjId[MAX_PLAYERS];
new g_UiLvPjDistrict[MAX_PLAYERS];
new g_UiLvPjName[MAX_PLAYERS][64];
new g_UiLvPjGoal[MAX_PLAYERS];
new g_UiLvPjFund[MAX_PLAYERS];
new g_UiLvPjHours[MAX_PLAYERS];
new g_UiLvPjMin[MAX_PLAYERS];
new g_UiLvPjMax[MAX_PLAYERS];
new g_UiLvPjDayLeft[MAX_PLAYERS];
new g_UiLvPjBlock[MAX_PLAYERS];
new g_UiLvPjAmount[MAX_PLAYERS];
new g_UiLvHpStep[MAX_PLAYERS];
new bool:g_UiLvHpOff[MAX_PLAYERS];
new g_UiLvHpView[MAX_PLAYERS];
new g_UiLvHpOffRow[MAX_PLAYERS];
new g_UiLvHintTtl[MAX_PLAYERS];
new g_UiLvTd[MAX_PLAYERS][11];
new bool:g_UiLvTdMade[MAX_PLAYERS];
new g_UiLvToastTtl[MAX_PLAYERS];
new g_UiLvTimer[MAX_PLAYERS];

bool:UI_LvValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

bool:UI_LvReady(playerid)
{
	return UI_LvValid(playerid) && IsPlayerConnected(playerid);
}

bool:UI_LvIsLivingDialog(dialogid)
{
	return dialogid >= 9700 && dialogid <= 9725;
}

UI_LvClampDistrict(districtId)
{
	return districtId >= 1 && districtId <= 3 ? districtId : 0;
}

UI_LvClamp(value, lo, hi)
{
	if (value < lo)
	{
		return lo;
	}
	if (value > hi)
	{
		return hi;
	}
	return value;
}

UI_LvFmt(value, dest[], size)
{
	new s[16], len = 0, bool:neg = value < 0, ch[2];
	if (neg)
	{
		value = -value;
	}
	valstr(s, value, 0);
	len = strlen(s);
	dest[0] = 0;
	if (neg)
	{
		strcat(dest, "-", size);
	}
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

UI_LvFmtTime(seconds, dest[], size)
{
	if (seconds < 0)
	{
		strcopy(dest, "--:--", size);
		return 0;
	}
	format(dest, size, "%02d:%02d", seconds / 60, seconds % 60);
	return 0;
}

UI_LvFmtDist(meters, dest[], size)
{
	if (meters < 0)
	{
		meters = 0;
	}
	if (meters < 1000)
	{
		format(dest, size, "%d m", meters);
	}
	else
	{
		format(dest, size, "%d,%d km", meters / 1000, meters % 1000 / 100);
	}
}

UI_LvFmtAgo(seconds, dest[], size)
{
	if (seconds < 60)
	{
		strcopy(dest, "adesso", size);
	}
	else if (seconds < 3600)
	{
		format(dest, size, "%d min fa", seconds / 60);
	}
	else if (seconds < 86400)
	{
		format(dest, size, "%d h fa", seconds / 3600);
	}
	else
	{
		format(dest, size, "%d g fa", seconds / 86400);
	}
}

UI_LvBar(value, maxValue, const color[], dest[], size)
{
	new filled = 0;
	if (maxValue > 0)
	{
		filled = UI_LvClamp(value, 0, maxValue) * 20 / maxValue;
	}
	format(dest, size, "{%s}", color);
	for (new i = 0; i < 20; i++)
	{
		if (i == filled)
		{
			strcat(dest, "{555555}", size);
		}
		strcat(dest, "|", size);
	}
}

bool:UI_LvParseAmount(const s[], &value)
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

UI_LvTier(points)
{
	if (points >= 600)
	{
		return 3;
	}
	if (points >= 300)
	{
		return 2;
	}
	if (points >= 100)
	{
		return 1;
	}
	return 0;
}

UI_LvTierBonus(tier)
{
	switch (tier)
	{
		case 1:
		{
			return 5;
		}
		case 2:
		{
			return 10;
		}
		case 3:
		{
			return 15;
		}
	}
	return 0;
}

UI_LvTierNext(tier)
{
	switch (tier)
	{
		case 0:
		{
			return 100;
		}
		case 1:
		{
			return 300;
		}
		case 2:
		{
			return 600;
		}
	}
	return 0;
}

UI_LvLocalLabel(rep, dest[], size)
{
	if (rep >= 40)
	{
		format(dest, size, "{66DD66}Di casa{AAAAAA} (-%d%% su noleggio e taxi)", 10);
	}
	else if (rep >= 10)
	{
		format(dest, size, "{FFD24D}Conosciuto{AAAAAA} (-%d%% su noleggio e taxi)", 5);
	}
	else if (rep < 0)
	{
		strcopy(dest, "{FFAA33}Da riconquistare{AAAAAA} (si risale piu' in fretta)", size);
	}
	else
	{
		strcopy(dest, "{FFFFFF}Sconosciuto", size);
	}
}

UI_LvToastColor(kind)
{
	switch (kind)
	{
		case 1:
		{
			return 0x66DD66FF;
		}
		case 2:
		{
			return -5622785;
		}
		case 3:
		{
			return -10066177;
		}
	}
	return -2994689;
}

UI_LvInfo(playerid, const title[], const body[])
{
	Lang_ShowPlayerDialog(playerid, 9725, 0, title, body, "Chiudi", "");
}

PlayerText:UI_LvText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, align);
	PlayerTextDrawColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetProportional(playerid, td, 1);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	return td;
}

PlayerText:UI_LvBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, "_");
	PlayerTextDrawLetterSize(playerid, td, 0.0, (h / 9.0));
	PlayerTextDrawTextSize(playerid, td, (x + w), 0.0);
	PlayerTextDrawUseBox(playerid, td, 1);
	PlayerTextDrawBoxColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawSetOutline(playerid, td, 0);
	return td;
}

UI_LvTdEnsure(playerid)
{
	if (g_UiLvTdMade[playerid])
	{
		return 0;
	}
	g_UiLvTd[playerid][0] = UI_LvBox(playerid, 14.0, 150.0, 200.0, 56.0, 153);
	g_UiLvTd[playerid][1] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 2.0), "CONTRATTO", 0.22, 1.0, -2994689, 1);
	g_UiLvTd[playerid][2] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 15.0), "_", 0.2, 0.9, -1, 1);
	g_UiLvTd[playerid][3] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 27.0), "_", 0.2, 0.9, -1, 1);
	g_UiLvTd[playerid][4] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 39.0), "_", 0.2, 0.9, 0xC8C8C8FF, 1);
	g_UiLvTd[playerid][5] = UI_LvBox(playerid, 14.0, 212.0, 150.0, 32.0, 153);
	g_UiLvTd[playerid][6] = UI_LvText(playerid, (14.0 + 5.0), (212.0 + 2.0), "TASSAMETRO", 0.22, 1.0, -2994689, 1);
	g_UiLvTd[playerid][7] = UI_LvText(playerid, (14.0 + 5.0), (212.0 + 15.0), "_", 0.26, 1.1, 0x66DD66FF, 1);
	g_UiLvTd[playerid][8] = UI_LvBox(playerid, 14.0, 252.0, 220.0, 40.0, 153);
	g_UiLvTd[playerid][9] = UI_LvText(playerid, (14.0 + 5.0), (252.0 + 2.0), "_", 0.2, 0.9, -1, 1);
	PlayerTextDrawTextSize(playerid, g_UiLvTd[playerid][9], ((14.0 + 220.0) - 4.0), 0.0);
	g_UiLvTd[playerid][10] = UI_LvText(playerid, 320.0, 318.0, "_", 0.24, 1.1, -2994689, 2);
	g_UiLvTdMade[playerid] = 1;
	return 0;
}

UI_LvTdShow(playerid, first, last, bool:show)
{
	if (!g_UiLvTdMade[playerid])
	{
		return 0;
	}
	for (new i = first; i <= last; i++)
	{
		if (show)
		{
			PlayerTextDrawShow(playerid, g_UiLvTd[playerid][i]);
		}
		else
		{
			PlayerTextDrawHide(playerid, g_UiLvTd[playerid][i]);
		}
	}
	return 0;
}

bool:UI_LvNeedTimer(playerid)
{
	return g_UiLvToastTtl[playerid] > 0 || g_UiLvHintTtl[playerid] > 0 || g_UiLvTxOfferLeft[playerid] > 0 || g_UiLvTxPayLeft[playerid] > 0 || (g_UiLvHudOn[playerid] && g_UiLvHudExpAt[playerid] != 0);
}

UI_LvTimerStop(playerid)
{
	if (g_UiLvTimer[playerid] != 0)
	{
		KillTimer(g_UiLvTimer[playerid]);
		g_UiLvTimer[playerid] = 0;
	}
}

UI_LvTimerSync(playerid)
{
	if (UI_LvNeedTimer(playerid))
	{
		if (g_UiLvTimer[playerid] == 0)
		{
			g_UiLvTimer[playerid] = SetTimerEx("UI_LivingOnTimer", 1000, 1, "d", playerid);
		}
	}
	else
	{
		UI_LvTimerStop(playerid);
	}
}

UI_LvCefLocalTier(rep)
{
	if (rep >= 40)
	{
		return 2;
	}
	if (rep >= 10)
	{
		return 1;
	}
	if (rep < 0)
	{
		return -1;
	}
	return 0;
}

UI_LvCefDiscount(rep)
{
	if (rep >= 40)
	{
		return 10;
	}
	if (rep >= 10)
	{
		return 5;
	}
	return 0;
}

UI_LvCefBoard(playerid, bool:atBoard)
{
	new now = gettime();
	cef_emit_event(playerid, "lssc:board_begin", 1, atBoard ? 1 : 0, 0, atBoard ? "Bacheca del quartiere" : "Consultazione");
	for (new i = 0; i < g_UiLvBdCount[playerid]; i++)
	{
		new left = g_UiLvBdExpAt[playerid][i] - now;
		cef_emit_event(playerid, "lssc:board_add", 1, g_UiLvBdId[playerid][i], 1, g_UiLvBdKind[playerid][i], 1, g_UiLvBdFrom[playerid][i], 1, g_UiLvBdTo[playerid][i], 1, g_UiLvBdCargo[playerid][i], 1, g_UiLvBdReward[playerid][i], 1, g_UiLvBdDist[playerid][i], 1, left < 0 ? 0 : left, 1, g_UiLvBdPremium[playerid][i] ? 1 : 0, 1, g_UiLvBdMinLevel[playerid][i], 1, g_UiLvBdMinRep[playerid][i], 1, g_UiLvBdMinLRep[playerid][i], 1, g_UiLvBdBlock[playerid][i]);
	}
	cef_emit_event(playerid, "lssc:board_end");
	CefUI_Open(playerid, "board", "");
}

UI_LvCefDistrict(playerid)
{
	new d = g_UiLvDsId[playerid], act = g_UiLvDsAct[playerid], crime = g_UiLvDsCrime[playerid], lrep = g_UiLvDsLRep[playerid];
	new prosp = act < 350 ? 0 : (act >= 650 ? 2 : 1);
	cef_emit_event(playerid, "lssc:district_begin", 1, d, 1, act, 1, g_UiLvDsSec[playerid], 1, crime, 1, lrep, 1, g_UiLvDsFest[playerid], 1, prosp, 1, UI_LvCefLocalTier(lrep), 1, UI_LvCefDiscount(lrep), 1, crime >= 70 ? 1 : 0);
	if (g_UiLvDsRumor[playerid][0] != 0)
	{
		cef_emit_event(playerid, "lssc:district_rumor", 0, g_UiLvDsRumor[playerid]);
	}
	for (new i = 0; i < g_UiLvDsEvCount[playerid]; i++)
	{
		cef_emit_event(playerid, "lssc:district_event", 1, g_UiLvDsEvType[playerid][i], 1, g_UiLvDsEvAgo[playerid][i], 1, g_UiLvDsEvValue[playerid][i]);
	}
	cef_emit_event(playerid, "lssc:district_end");
	CefUI_Open(playerid, "phone", "district");
}

UI_LvCefRep(playerid)
{
	cef_emit_event(playerid, "lssc:rep_begin");
	for (new t = 1; t <= 2; t++)
	{
		new bool:has = g_UiLvRpHas[playerid][t];
		new pts = has ? g_UiLvRpPoints[playerid][t] : 0, tier = UI_LvTier(pts);
		cef_emit_event(playerid, "lssc:rep_track", 1, t, 1, pts, 1, has ? g_UiLvRpDone[playerid][t] : 0, 1, has ? g_UiLvRpFail[playerid][t] : 0, 1, has ? g_UiLvRpRSum[playerid][t] : 0, 1, has ? g_UiLvRpRCnt[playerid][t] : 0, 1, has ? g_UiLvRpDay[playerid][t] : 0, 1, tier, 1, UI_LvTierNext(tier), 1, UI_LvTierBonus(tier));
	}
	for (new d = 1; d <= 3; d++)
	{
		new rep = g_UiLvRpLHas[playerid][d] ? g_UiLvRpLocal[playerid][d] : 0;
		cef_emit_event(playerid, "lssc:rep_local", 1, d, 1, rep, 1, UI_LvCefLocalTier(rep), 1, UI_LvCefDiscount(rep), 1, rep >= 20 ? 1 : 0);
	}
	cef_emit_event(playerid, "lssc:rep_end");
	CefUI_Open(playerid, "phone", "profile");
}

UI_LvCefProject(playerid)
{
	cef_emit_event(playerid, "lssc:project", 1, g_UiLvPjId[playerid], 1, g_UiLvPjDistrict[playerid], 0, g_UiLvPjName[playerid], 1, g_UiLvPjGoal[playerid], 1, g_UiLvPjFund[playerid], 1, g_UiLvPjHours[playerid], 1, g_UiLvPjMin[playerid], 1, g_UiLvPjMax[playerid], 1, g_UiLvPjDayLeft[playerid], 1, g_UiLvPjBlock[playerid]);
	CefUI_Open(playerid, "phone", "project");
}

UI_LivingToast(playerid, const text[], kind)
{
	if (!UI_LvReady(playerid) || text[0] == 0)
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		CefUI_Toast(playerid, text, kind);
		return 0;
	}
	UI_LvTdEnsure(playerid);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][10], text);
	PlayerTextDrawColour(playerid, g_UiLvTd[playerid][10], UI_LvToastColor(kind));
	PlayerTextDrawShow(playerid, g_UiLvTd[playerid][10]);
	g_UiLvToastTtl[playerid] = 4;
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LivingNotify(playerid, const text[], kind)
{
	if (!UI_LvReady(playerid) || text[0] == 0)
	{
		return 0;
	}
	SendClientMessage(playerid, UI_LvToastColor(kind), text);
	UI_LivingToast(playerid, text, kind);
	return 0;
}

UI_CityBoardBegin(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	g_UiLvBdCount[playerid] = 0;
	g_UiLvBdRows[playerid] = 0;
	return 0;
}

UI_CityBoardAdd(playerid, contractId, kind, fromDistrict, toDistrict, cargo, reward, distanceM, secondsLeft, bool:premium, minLevel, minRep, minLocalRep, blockCode)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	new n = g_UiLvBdCount[playerid];
	if (n < 0 || n >= 12 || contractId <= 0)
	{
		return 0;
	}
	g_UiLvBdId[playerid][n] = contractId;
	g_UiLvBdKind[playerid][n] = kind >= 1 && kind <= 1 ? kind : 1;
	g_UiLvBdFrom[playerid][n] = UI_LvClampDistrict(fromDistrict);
	g_UiLvBdTo[playerid][n] = UI_LvClampDistrict(toDistrict);
	g_UiLvBdCargo[playerid][n] = cargo >= 1 && cargo <= 3 ? cargo : 0;
	g_UiLvBdReward[playerid][n] = reward < 0 ? 0 : reward;
	g_UiLvBdDist[playerid][n] = distanceM < 0 ? 0 : distanceM;
	g_UiLvBdExpAt[playerid][n] = gettime() + (secondsLeft < 0 ? 0 : secondsLeft);
	g_UiLvBdPremium[playerid][n] = premium;
	g_UiLvBdMinLevel[playerid][n] = minLevel;
	g_UiLvBdMinRep[playerid][n] = minRep;
	g_UiLvBdMinLRep[playerid][n] = minLocalRep;
	g_UiLvBdBlock[playerid][n] = blockCode >= 0 && blockCode < 11 ? blockCode : 10;
	g_UiLvBdCount[playerid] = n + 1;
	return 0;
}

bool:UI_LvBoardMatch(playerid, i)
{
	new d = g_UiLvFltDistrict[playerid];
	if (d != 0 && g_UiLvBdFrom[playerid][i] != d && g_UiLvBdTo[playerid][i] != d)
	{
		return 0;
	}
	if (g_UiLvFltKind[playerid] != 0 && g_UiLvBdKind[playerid][i] != g_UiLvFltKind[playerid])
	{
		return 0;
	}
	if (g_UiLvFltEligible[playerid] && g_UiLvBdBlock[playerid][i] != 0)
	{
		return 0;
	}
	return 1;
}

UI_LvRequirement(playerid, i, dest[], size)
{
	if (g_UiLvBdMinRep[playerid][i] > 0)
	{
		format(dest, size, "Liv. %d, rep. %d", g_UiLvBdMinLevel[playerid][i], g_UiLvBdMinRep[playerid][i]);
	}
	else if (g_UiLvBdMinLRep[playerid][i] > 0)
	{
		format(dest, size, "Liv. %d, fiducia %d", g_UiLvBdMinLevel[playerid][i], g_UiLvBdMinLRep[playerid][i]);
	}
	else
	{
		format(dest, size, "Liv. %d", g_UiLvBdMinLevel[playerid][i]);
	}
}

UI_LvBoardRender(playerid)
{
	new body[1536], line[128], title[64], now = gettime();
	new sReward[16], sTime[12], req[40];
	format(body, 1536, "Contratto\tMerce\tCompenso / scade\tRequisito\n{FFD24D}Filtro: %s\t{FFD24D}%s\t{FFD24D}%s\t{AAAAAA}cambia\n", g_UiLvDistrictName[g_UiLvFltDistrict[playerid]], g_UiLvKindName[g_UiLvFltKind[playerid]], g_UiLvFltEligible[playerid] ? "solo per me" : "tutti");
	g_UiLvBdRows[playerid] = 0;
	for (new i = 0; i < g_UiLvBdCount[playerid]; i++)
	{
		if (!UI_LvBoardMatch(playerid, i))
		{
			continue;
		}
		UI_LvFmt(g_UiLvBdReward[playerid][i], sReward, 16);
		UI_LvFmtTime(g_UiLvBdExpAt[playerid][i] - now, sTime, 12);
		new bool:ok = g_UiLvBdBlock[playerid][i] == 0;
		if (ok)
		{
			UI_LvRequirement(playerid, i, req, 40);
		}
		else
		{
			strcopy(req, g_UiLvBlockText[g_UiLvBdBlock[playerid][i]], 40);
		}
		format(line, 128, "%s#%d %s > %s%s\t%s%s\t%s$%s / %s\t%s%s\n", ok ? "{FFFFFF}" : "{888888}", g_UiLvBdId[playerid][i], g_UiLvDistrictName[g_UiLvBdFrom[playerid][i]], g_UiLvDistrictName[g_UiLvBdTo[playerid][i]], g_UiLvBdPremium[playerid][i] ? " *" : "", ok ? "{FFFFFF}" : "{888888}", g_UiLvCargoName[g_UiLvBdCargo[playerid][i]], ok ? "{66DD66}" : "{888888}", sReward, sTime, ok ? "{AAAAAA}" : "{FF8888}", req);
		strcat(body, line, 1536);
		g_UiLvBdRow[playerid][g_UiLvBdRows[playerid]] = i;
		g_UiLvBdRows[playerid]++;
	}
	if (g_UiLvBdRows[playerid] == 0)
	{
		strcat(body, "{AAAAAA}Nessun contratto con questo filtro\t\t\t\n", 1536);
	}
	strcat(body, "{AAAAAA}Aggiorna la bacheca\t\t\t", 1536);
	format(title, 64, "Bacheca contratti%s", g_UiLvBdAt[playerid] ? "" : " (consultazione)");
	Lang_ShowPlayerDialog(playerid, 9700, 5, title, body, "Apri", "Chiudi");
}

UI_CityBoardShow(playerid, bool:atBoard)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	g_UiLvBdAt[playerid] = atBoard;
	if (CefUI_Active(playerid))
	{
		UI_LvCefBoard(playerid, atBoard);
		return 0;
	}
	UI_LvBoardRender(playerid);
	return 0;
}

UI_LvFilterShow(playerid)
{
	new body[384];
	format(body, 384, "Quartiere: {FFD24D}%s\nProfessione: {FFD24D}%s\nMostra: {FFD24D}%s\n{66DD66}Torna alla bacheca", g_UiLvDistrictName[g_UiLvFltDistrict[playerid]], g_UiLvKindName[g_UiLvFltKind[playerid]], g_UiLvFltEligible[playerid] ? "solo quelli che posso accettare" : "tutti i contratti");
	Lang_ShowPlayerDialog(playerid, 9701, 2, "Filtra la bacheca", body, "Cambia", "Indietro");
}

UI_LvDetailRender(playerid, i)
{
	new body[1024], title[64], sReward[16], sTime[12], sDist[16], req[40], status[160];
	new block = g_UiLvBdBlock[playerid][i];
	UI_LvFmt(g_UiLvBdReward[playerid][i], sReward, 16);
	UI_LvFmtTime(g_UiLvBdExpAt[playerid][i] - gettime(), sTime, 12);
	UI_LvFmtDist(g_UiLvBdDist[playerid][i], sDist, 16);
	UI_LvRequirement(playerid, i, req, 40);
	g_UiLvDetIdx[playerid] = i;
	g_UiLvDetCan[playerid] = block == 0 && g_UiLvBdAt[playerid];
	if (block != 0)
	{
		format(status, 160, "{FF8888}%s.", g_UiLvBlockText[block]);
	}
	else if (!g_UiLvBdAt[playerid])
	{
		strcopy(status, "{FFD24D}Puoi accettarlo a una bacheca: Casa del Quartiere di Idlewood, magazzino di Ganton o di Jefferson.", 160);
	}
	else
	{
		strcopy(status, "{66DD66}Puoi accettarlo adesso.", 160);
	}
	format(body, 1024, "{FFFFFF}%s%s\n\n{FFFFFF}Ritiro: {FFD24D}magazzino di %s\n{FFFFFF}Consegna: {FFD24D}negozio di %s\n{FFFFFF}Merce: {FFD24D}%s\n{FFFFFF}Distanza: {FFD24D}%s\n{FFFFFF}Compenso indicativo: {66DD66}$%s\n{FFFFFF}Disponibile ancora per: {FFD24D}%s\n{FFFFFF}Requisiti: {AAAAAA}%s\n\n{AAAAAA}Il mezzo te lo fornisce il magazzino. Il compenso finale dipende da come arriva la merce.\n%s", g_UiLvKindName[g_UiLvBdKind[playerid][i]], g_UiLvBdPremium[playerid][i] ? " {FFD24D}(premium: il quartiere si fida di te)" : "", g_UiLvDistrictName[g_UiLvBdFrom[playerid][i]], g_UiLvDistrictName[g_UiLvBdTo[playerid][i]], g_UiLvCargoName[g_UiLvBdCargo[playerid][i]], sDist, sReward, sTime, req, status);
	format(title, 64, "Contratto #%d", g_UiLvBdId[playerid][i]);
	if (g_UiLvDetCan[playerid])
	{
		Lang_ShowPlayerDialog(playerid, 9702, 0, title, body, "Accetta", "Indietro");
	}
	else
	{
		Lang_ShowPlayerDialog(playerid, 9702, 0, title, body, "Indietro", "Chiudi");
	}
}

UI_LvDetailShow(playerid, i)
{
	if (i < 0 || i >= g_UiLvBdCount[playerid])
	{
		UI_LvBoardRender(playerid);
	}
	else
	{
		UI_LvDetailRender(playerid, i);
	}
}

UI_LvAcceptShow(playerid, i)
{
	new body[512], sTime[12];
	UI_LvFmtTime(g_UiLvBdExpAt[playerid][i] - gettime(), sTime, 12);
	format(body, 512, "{FFFFFF}Accetti il contratto {FFD24D}#%d{FFFFFF}?\n\nUn furgone ti aspetta al magazzino di {FFD24D}%s{FFFFFF}.\nPorta il carico al negozio di {FFD24D}%s{FFFFFF} con calma e attenzione.\n\n{AAAAAA}Puoi seguire un solo contratto alla volta. Il contratto resta valido per %s.", g_UiLvBdId[playerid][i], g_UiLvDistrictName[g_UiLvBdFrom[playerid][i]], g_UiLvDistrictName[g_UiLvBdTo[playerid][i]], sTime);
	Lang_ShowPlayerDialog(playerid, 9703, 0, "Conferma contratto", body, "Conferma", "Annulla");
}

UI_CityContractStatus(playerid, contractId, ctState, fromDistrict, toDistrict, cargo, reward, secondsLeft, integrityPct, distanceM, repPenalty)
{
	if (!UI_LvReady(playerid) || contractId <= 0)
	{
		return 0;
	}
	new body[1024], title[64], sReward[16], sTime[12], sDist[16], sInteg[48], step[128];
	new from = UI_LvClampDistrict(fromDistrict), to = UI_LvClampDistrict(toDistrict);
	g_UiLvStId[playerid] = contractId;
	if (CefUI_Active(playerid))
	{
		if (ctState == 2)
		{
			format(step, 128, "Consegna al negozio di %s", g_UiLvDistrictName[to]);
		}
		else
		{
			format(step, 128, "Carica la merce al magazzino di %s", g_UiLvDistrictName[from]);
		}
		format(sDist, 16, "%s", g_UiLvDistrictName[ctState == 2 ? to : from]);
		cef_emit_event(playerid, "lssc:contract", 1, contractId, 1, ctState, 0, step, 0, sDist, 1, distanceM, 1, secondsLeft, 1, integrityPct, 1, from, 1, to, 1, cargo, 1, reward, 1, repPenalty);
		CefUI_Open(playerid, "phone", "contracts");
		return 0;
	}
	UI_LvFmt(reward < 0 ? 0 : reward, sReward, 16);
	UI_LvFmtTime(secondsLeft, sTime, 12);
	if (distanceM >= 0)
	{
		UI_LvFmtDist(distanceM, sDist, 16);
	}
	else
	{
		strcopy(sDist, "-", 16);
	}
	if (integrityPct >= 0)
	{
		format(sInteg, 48, "%d%%", UI_LvClamp(integrityPct, 0, 100));
	}
	else
	{
		strcopy(sInteg, "-", 48);
	}
	if (ctState == 2)
	{
		format(step, 128, "Carico a bordo: consegnalo al negozio di %s", g_UiLvDistrictName[to]);
	}
	else
	{
		format(step, 128, "Raggiungi il magazzino di %s e carica la merce", g_UiLvDistrictName[from]);
	}
	format(body, 1024, "{FFFFFF}Stato: {FFD24D}%s\n{FFFFFF}Prossimo passo: {FFD24D}%s\n{FFFFFF}Distanza: {FFD24D}%s\n{FFFFFF}Tempo rimasto: {FFD24D}%s\n{FFFFFF}Merce: {FFD24D}%s{FFFFFF}, integrita' {FFD24D}%s\n{FFFFFF}Compenso indicativo: {66DD66}$%s\n\n{AAAAAA}Se lasci il contratto non perdi denaro, ma perdi %d punti di reputazione nei trasporti.", ctState == 2 ? "in consegna" : "accettato", step, sDist, sTime, g_UiLvCargoName[cargo >= 1 && cargo <= 3 ? cargo : 0], sInteg, sReward, repPenalty < 0 ? -repPenalty : repPenalty);
	format(title, 64, "Il tuo contratto #%d", contractId);
	Lang_ShowPlayerDialog(playerid, 9704, 0, title, body, "Lascia", "Chiudi");
	return 0;
}

UI_CityContractNone(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	g_UiLvStId[playerid] = 0;
	if (CefUI_Active(playerid))
	{
		cef_emit_event(playerid, "lssc:contract", 1, 0, 1, 0, 0, "", 0, "", 1, -1, 1, -1, 1, -1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);
		CefUI_Open(playerid, "phone", "contracts");
		return 0;
	}
	Lang_ShowPlayerDialog(playerid, 9704, 0, "Nessun contratto", "{FFFFFF}Al momento non stai seguendo nessun contratto.\n\n{AAAAAA}La bacheca della Casa del Quartiere di Idlewood ha sempre qualcosa da fare.", "Bacheca", "Chiudi");
	return 0;
}

UI_LvHudRender(playerid)
{
	new line[128], sDist[16], sTime[12];
	if (g_UiLvHudDist[playerid] >= 0)
	{
		UI_LvFmtDist(g_UiLvHudDist[playerid], sDist, 16);
		format(line, 128, "Verso: %s (%s)", g_UiLvHudDest[playerid], sDist);
	}
	else
	{
		format(line, 128, "Verso: %s", g_UiLvHudDest[playerid]);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][3], line);
	if (g_UiLvHudExpAt[playerid] != 0)
	{
		new left = g_UiLvHudExpAt[playerid] - gettime();
		UI_LvFmtTime(left < 0 ? 0 : left, sTime, 12);
	}
	else
	{
		strcopy(sTime, "--:--", 12);
	}
	if (g_UiLvHudInteg[playerid] >= 0)
	{
		format(line, 128, "Scade tra %s  -  Merce %d%%", sTime, g_UiLvHudInteg[playerid]);
	}
	else
	{
		format(line, 128, "Scade tra %s", sTime);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][4], line);
}

UI_CityContractHud(playerid, contractId, ctState, const objective[], const destination[], distanceM, secondsLeft, integrityPct)
{
	if (!UI_LvReady(playerid) || contractId <= 0)
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		cef_emit_event(playerid, "lssc:contract", 1, contractId, 1, ctState, 0, objective, 0, destination, 1, distanceM, 1, secondsLeft, 1, integrityPct, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);
		g_UiLvHudId[playerid] = contractId;
		return 0;
	}
	UI_LvTdEnsure(playerid);
	new title[128], goal[128];
	g_UiLvHudId[playerid] = contractId;
	g_UiLvHudState[playerid] = ctState;
	g_UiLvHudDist[playerid] = distanceM;
	g_UiLvHudExpAt[playerid] = secondsLeft >= 0 ? gettime() + secondsLeft : 0;
	g_UiLvHudInteg[playerid] = integrityPct >= 0 ? UI_LvClamp(integrityPct, 0, 100) : -1;
	strcopy(g_UiLvHudDest[playerid], destination[0] != 0 ? destination : "-", 48);
	format(title, 128, "CONTRATTO #%d  -  %s", contractId, ctState == 2 ? "IN CONSEGNA" : "ACCETTATO");
	format(goal, 128, "%s", objective[0] != 0 ? objective : "_");
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][1], title);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][2], goal);
	UI_LvHudRender(playerid);
	if (!g_UiLvHudOn[playerid])
	{
		UI_LvTdShow(playerid, 0, 4, 1);
	}
	g_UiLvHudOn[playerid] = 1;
	UI_LvTimerSync(playerid);
	return 0;
}

UI_CityContractHudHide(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		cef_emit_event(playerid, "lssc:contract", 1, 0, 1, 0, 0, "", 0, "", 1, -1, 1, -1, 1, -1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);
	}
	if (g_UiLvHudOn[playerid] && IsPlayerConnected(playerid))
	{
		UI_LvTdShow(playerid, 0, 4, 0);
	}
	g_UiLvHudOn[playerid] = 0;
	g_UiLvHudId[playerid] = 0;
	g_UiLvHudExpAt[playerid] = 0;
	if (IsPlayerConnected(playerid))
	{
		UI_LvTimerSync(playerid);
	}
	return 0;
}

UI_CityDistrictPick(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new body[128];
	format(body, 128, "%s\n%s\n%s", g_UiLvDistrictName[1], g_UiLvDistrictName[2], g_UiLvDistrictName[3]);
	Lang_ShowPlayerDialog(playerid, 9706, 2, "Quale quartiere?", body, "Apri", "Chiudi");
	return 0;
}

UI_CityDistrictBegin(playerid, districtId, activity, security, crime, localRep, festivalHours)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	g_UiLvDsId[playerid] = UI_LvClampDistrict(districtId);
	g_UiLvDsAct[playerid] = UI_LvClamp(activity, 0, 1000);
	g_UiLvDsSec[playerid] = UI_LvClamp(security, 0, 100);
	g_UiLvDsCrime[playerid] = UI_LvClamp(crime, 0, 100);
	g_UiLvDsLRep[playerid] = localRep;
	g_UiLvDsFest[playerid] = festivalHours < 0 ? 0 : festivalHours;
	g_UiLvDsRumor[playerid][0] = 0;
	g_UiLvDsEvCount[playerid] = 0;
	return 0;
}

UI_CityDistrictRumor(playerid, const text[])
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	strcopy(g_UiLvDsRumor[playerid], text, 96);
	return 0;
}

UI_CityDistrictEvent(playerid, type, secondsAgo, value)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	new n = g_UiLvDsEvCount[playerid];
	if (n < 0 || n >= 8)
	{
		return 0;
	}
	g_UiLvDsEvType[playerid][n] = type >= 1 && type <= 9 ? type : 0;
	g_UiLvDsEvAgo[playerid][n] = secondsAgo < 0 ? 0 : secondsAgo;
	g_UiLvDsEvValue[playerid][n] = value;
	g_UiLvDsEvCount[playerid] = n + 1;
	return 0;
}

UI_CityDistrictShow(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new d = g_UiLvDsId[playerid];
	if (d == 0)
	{
		UI_CityDistrictPick(playerid);
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		UI_LvCefDistrict(playerid);
		return 0;
	}
	new body[1280], line[128], title[64], bAct[64], bSec[64], bCrime[64], prosp[96], lrep[80], ago[16];
	new act = g_UiLvDsAct[playerid];
	UI_LvBar(act, 1000, "66DD66", bAct, 64);
	UI_LvBar(g_UiLvDsSec[playerid], 100, "66AAFF", bSec, 64);
	UI_LvBar(g_UiLvDsCrime[playerid], 100, "FF8866", bCrime, 64);
	if (act < 350)
	{
		strcopy(prosp, "{FFAA33}In difficolta'{AAAAAA}: ogni consegna conta per la ricostruzione", 96);
	}
	else if (act >= 650)
	{
		strcopy(prosp, "{66DD66}Fiorente{AAAAAA}: c'e' piu' lavoro per tutti", 96);
	}
	else
	{
		strcopy(prosp, "{FFFFFF}Stabile", 96);
	}
	UI_LvLocalLabel(g_UiLvDsLRep[playerid], lrep, 80);
	format(body, 1280, "{FFFFFF}Prosperita': %s\n\n{FFFFFF}Attivita' economica  %s {FFFFFF}%d/%d\n{FFFFFF}Sicurezza                %s {FFFFFF}%d/%d\n{FFFFFF}Pressione criminale  %s {FFFFFF}%d/%d%s\n\n{FFFFFF}La tua reputazione qui: {FFD24D}%d{FFFFFF} - %s\n", prosp, bAct, act, 1000, bSec, g_UiLvDsSec[playerid], 100, bCrime, g_UiLvDsCrime[playerid], 100, g_UiLvDsCrime[playerid] >= 70 ? "  {FF8866}(zona a rischio: i contratti pagano di piu')" : "", g_UiLvDsLRep[playerid], lrep);
	if (g_UiLvDsFest[playerid] > 0)
	{
		format(line, 128, "{66DD66}Festa di quartiere in corso: ancora %d ore.\n", g_UiLvDsFest[playerid]);
		strcat(body, line, 1280);
	}
	if (g_UiLvDsRumor[playerid][0] != 0)
	{
		format(line, 128, "\n{AAAAAA}Si dice in giro: {FFFFFF}\"%s\"\n", g_UiLvDsRumor[playerid]);
		strcat(body, line, 1280);
	}
	strcat(body, "\n{FFD24D}Ultimi avvenimenti\n", 1280);
	if (g_UiLvDsEvCount[playerid] == 0)
	{
		strcat(body, "{AAAAAA}Una giornata tranquilla, per ora.\n", 1280);
	}
	for (new i = 0; i < g_UiLvDsEvCount[playerid]; i++)
	{
		new v = g_UiLvDsEvValue[playerid][i];
		UI_LvFmtAgo(g_UiLvDsEvAgo[playerid][i], ago, 16);
		if (v > 0)
		{
			format(line, 128, "{AAAAAA}%s  {FFFFFF}%s {66DD66}(+%d attivita')\n", ago, g_UiLvEventName[g_UiLvDsEvType[playerid][i]], v);
		}
		else if (v < 0)
		{
			format(line, 128, "{AAAAAA}%s  {FFFFFF}%s {FF8866}(%d attivita')\n", ago, g_UiLvEventName[g_UiLvDsEvType[playerid][i]], v);
		}
		else
		{
			format(line, 128, "{AAAAAA}%s  {FFFFFF}%s\n", ago, g_UiLvEventName[g_UiLvDsEvType[playerid][i]]);
		}
		strcat(body, line, 1280);
	}
	format(title, 64, "Quartiere: %s", g_UiLvDistrictName[d]);
	Lang_ShowPlayerDialog(playerid, 9707, 0, title, body, "Progetto", "Chiudi");
	return 0;
}

UI_CityRepBegin(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	for (new t = 0; t <= 2; t++)
	{
		g_UiLvRpHas[playerid][t] = 0;
	}
	for (new d = 0; d <= 3; d++)
	{
		g_UiLvRpLHas[playerid][d] = 0;
	}
	return 0;
}

UI_CityRepTrack(playerid, track, points, completed, failed, ratingSum, ratingCnt, dayGain)
{
	if (!UI_LvValid(playerid) || track < 1 || track > 2)
	{
		return 0;
	}
	g_UiLvRpHas[playerid][track] = 1;
	g_UiLvRpPoints[playerid][track] = points < 0 ? 0 : points;
	g_UiLvRpDone[playerid][track] = completed < 0 ? 0 : completed;
	g_UiLvRpFail[playerid][track] = failed < 0 ? 0 : failed;
	g_UiLvRpRSum[playerid][track] = ratingSum < 0 ? 0 : ratingSum;
	g_UiLvRpRCnt[playerid][track] = ratingCnt < 0 ? 0 : ratingCnt;
	g_UiLvRpDay[playerid][track] = dayGain;
	return 0;
}

UI_CityRepLocal(playerid, districtId, rep)
{
	new d = UI_LvClampDistrict(districtId);
	if (!UI_LvValid(playerid) || d == 0)
	{
		return 0;
	}
	g_UiLvRpLHas[playerid][d] = 1;
	g_UiLvRpLocal[playerid][d] = rep;
	return 0;
}

UI_CityRepShow(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		UI_LvCefRep(playerid);
		return 0;
	}
	new body[1280], line[192], bar[64], lrep[80];
	strcopy(body, "{FFD24D}Il tuo mestiere\n", 1280);
	for (new t = 1; t <= 2; t++)
	{
		new pts = g_UiLvRpHas[playerid][t] ? g_UiLvRpPoints[playerid][t] : 0;
		new tier = UI_LvTier(pts), next = UI_LvTierNext(tier);
		if (next > 0)
		{
			UI_LvBar(pts, next, "FFD24D", bar, 64);
			format(line, 192, "{FFFFFF}%s: {FFD24D}%s{FFFFFF} (+%d%%) - %d punti\n  %s {AAAAAA}%s tra %d punti\n", g_UiLvTrackName[t], g_UiLvTierName[tier], UI_LvTierBonus(tier), pts, bar, g_UiLvTierName[tier + 1], next - pts);
		}
		else
		{
			format(line, 192, "{FFFFFF}%s: {66DD66}%s{FFFFFF} (+%d%%) - %d punti, il grado piu' alto\n", g_UiLvTrackName[t], g_UiLvTierName[tier], UI_LvTierBonus(tier), pts);
		}
		strcat(body, line, 1280);
		if (g_UiLvRpHas[playerid][t])
		{
			if (g_UiLvRpRCnt[playerid][t] > 0)
			{
				new avg10 = g_UiLvRpRSum[playerid][t] * 10 / g_UiLvRpRCnt[playerid][t];
				format(line, 192, "  {AAAAAA}Completati %d, non riusciti %d, voto medio %d,%d/5, oggi +%d\n", g_UiLvRpDone[playerid][t], g_UiLvRpFail[playerid][t], avg10 / 10, avg10 % 10, g_UiLvRpDay[playerid][t]);
			}
			else
			{
				format(line, 192, "  {AAAAAA}Completati %d, non riusciti %d, oggi +%d\n", g_UiLvRpDone[playerid][t], g_UiLvRpFail[playerid][t], g_UiLvRpDay[playerid][t]);
			}
		}
		else
		{
			strcopy(line, "  {AAAAAA}Non hai ancora iniziato: c'e' sempre una prima volta.\n", 192);
		}
		strcat(body, line, 1280);
	}
	strcat(body, "\n{FFD24D}Il tuo nome nei quartieri\n", 1280);
	for (new d = 1; d <= 3; d++)
	{
		new rep = g_UiLvRpLHas[playerid][d] ? g_UiLvRpLocal[playerid][d] : 0;
		UI_LvLocalLabel(rep, lrep, 80);
		format(line, 192, "{FFFFFF}%s: {FFD24D}%d{FFFFFF} - %s%s\n", g_UiLvDistrictName[d], rep, lrep, rep >= 20 ? " {66DD66}[contratti premium]" : "");
		strcat(body, line, 1280);
	}
	strcat(body, "\n{AAAAAA}La reputazione non si compra: cresce con il lavoro, ogni giorno un passo alla volta.", 1280);
	Lang_ShowPlayerDialog(playerid, 9708, 0, "Reputazione", body, "Chiudi", "");
	return 0;
}

UI_LivingRentalBegin(playerid, blockMinutes, maxBlocks, discountPct)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	g_UiLvRnBlockMin[playerid] = blockMinutes < 1 ? 1 : blockMinutes;
	g_UiLvRnMaxBlocks[playerid] = UI_LvClamp(maxBlocks, 1, 6);
	g_UiLvRnDisc[playerid] = UI_LvClamp(discountPct, 0, 100);
	g_UiLvRnCount[playerid] = 0;
	g_UiLvRnSel[playerid] = -1;
	g_UiLvRnBlocks[playerid] = 0;
	return 0;
}

UI_LivingRentalAddModel(playerid, modelIdx, const name[], feePerBlock, deposit, minLevel, bool:available)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	new n = g_UiLvRnCount[playerid];
	if (n < 0 || n >= 4)
	{
		return 0;
	}
	g_UiLvRnIdx[playerid][n] = modelIdx;
	strcopy(g_UiLvRnName[playerid][n], name, 32);
	g_UiLvRnFee[playerid][n] = feePerBlock < 0 ? 0 : feePerBlock;
	g_UiLvRnDep[playerid][n] = deposit < 0 ? 0 : deposit;
	g_UiLvRnLevel[playerid][n] = minLevel;
	g_UiLvRnAvail[playerid][n] = available;
	g_UiLvRnCount[playerid] = n + 1;
	return 0;
}

UI_LvRentModelShow(playerid)
{
	new body[1024], line[128], title[64], sFee[16], sDep[16];
	format(body, 1024, "Veicolo\tCanone / %d min\tCauzione\tLivello\n", g_UiLvRnBlockMin[playerid]);
	for (new i = 0; i < g_UiLvRnCount[playerid]; i++)
	{
		new bool:ok = g_UiLvRnAvail[playerid][i];
		UI_LvFmt(g_UiLvRnFee[playerid][i], sFee, 16);
		UI_LvFmt(g_UiLvRnDep[playerid][i], sDep, 16);
		format(line, 128, "%s%s\t%s$%s\t%s$%s\t%s%d\n", ok ? "{FFFFFF}" : "{888888}", g_UiLvRnName[playerid][i], ok ? "{66DD66}" : "{888888}", sFee, ok ? "{FFD24D}" : "{888888}", sDep, ok ? "{AAAAAA}" : "{888888}", g_UiLvRnLevel[playerid][i]);
		strcat(body, line, 1024);
	}
	if (g_UiLvRnDisc[playerid] > 0)
	{
		format(title, 64, "Noleggio - sconto residente %d%%", g_UiLvRnDisc[playerid]);
	}
	else
	{
		strcopy(title, "Noleggio veicoli", 64);
	}
	Lang_ShowPlayerDialog(playerid, 9709, 5, title, body, "Scegli", "Chiudi");
}

UI_LivingRentalShow(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	if (g_UiLvRnCount[playerid] == 0)
	{
		UI_LvInfo(playerid, "Noleggio veicoli", "{FFFFFF}Oggi non ci sono mezzi disponibili. Ripassa piu' tardi.");
		return 0;
	}
	UI_LvRentModelShow(playerid);
	return 0;
}

UI_LvRentBlocksShow(playerid)
{
	new s = g_UiLvRnSel[playerid], body[512], line[128], title[64], sTot[16];
	body[0] = 0;
	for (new b = 1; b <= g_UiLvRnMaxBlocks[playerid]; b++)
	{
		UI_LvFmt(g_UiLvRnFee[playerid][s] * b, sTot, 16);
		format(line, 128, "{FFFFFF}%d min\t{66DD66}$%s\n", g_UiLvRnBlockMin[playerid] * b, sTot);
		strcat(body, line, 512);
	}
	format(title, 64, "%s - per quanto tempo?", g_UiLvRnName[playerid][s]);
	Lang_ShowPlayerDialog(playerid, 9710, 4, title, body, "Avanti", "Indietro");
}

UI_LvRentSummaryShow(playerid)
{
	new s = g_UiLvRnSel[playerid], b = g_UiLvRnBlocks[playerid], body[1024], disc[64];
	new sFee[16], sDep[16], sTot[16];
	UI_LvFmt(g_UiLvRnFee[playerid][s] * b, sFee, 16);
	UI_LvFmt(g_UiLvRnDep[playerid][s], sDep, 16);
	UI_LvFmt(g_UiLvRnFee[playerid][s] * b + g_UiLvRnDep[playerid][s], sTot, 16);
	if (g_UiLvRnDisc[playerid] > 0)
	{
		format(disc, 64, " {AAAAAA}(sconto residente %d%% incluso)", g_UiLvRnDisc[playerid]);
	}
	else
	{
		disc[0] = 0;
	}
	format(body, 1024, "{FFFFFF}Veicolo: {FFD24D}%s\n{FFFFFF}Durata: {FFD24D}%d minuti\n{FFFFFF}Canone: {66DD66}$%s%s\n{FFFFFF}Cauzione: {FFD24D}$%s {AAAAAA}(ti torna alla riconsegna, in base alle condizioni del mezzo)\n{FFFFFF}Addebito ora: {66DD66}$%s\n\n{AAAAAA}Si paga dal conto in banca, poi dai contanti. Riporta il mezzo qui con /riconsegna:\ndopo la scadenza c'e' qualche minuto di tolleranza, poi la cauzione resta all'agenzia.", g_UiLvRnName[playerid][s], g_UiLvRnBlockMin[playerid] * b, sFee, disc, sDep, sTot);
	Lang_ShowPlayerDialog(playerid, 9711, 0, "Riepilogo noleggio", body, "Noleggia", "Indietro");
}

UI_LivingRentalReturnConfirm(playerid, rentalId, const modelName[], healthPct, deposit, refund)
{
	if (!UI_LvReady(playerid) || rentalId <= 0)
	{
		return 0;
	}
	new body[512], sDep[16], sRef[16];
	g_UiLvRnReturnId[playerid] = rentalId;
	UI_LvFmt(deposit < 0 ? 0 : deposit, sDep, 16);
	UI_LvFmt(refund < 0 ? 0 : refund, sRef, 16);
	format(body, 512, "{FFFFFF}Riconsegni la {FFD24D}%s{FFFFFF}?\n\nCondizioni del mezzo: {FFD24D}%d%%\n{FFFFFF}Cauzione versata: {FFD24D}$%s\n{FFFFFF}Ti restituiamo: {66DD66}$%s\n\n{AAAAAA}Il canone non si rimborsa. Grazie per averci scelto.", modelName, UI_LvClamp(healthPct, 0, 100), sDep, sRef);
	Lang_ShowPlayerDialog(playerid, 9712, 0, "Riconsegna", body, "Riconsegna", "Annulla");
	return 0;
}

UI_LivingRentalReceipt(playerid, const modelName[], fee, deposit, refund, minutesUsed)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new body[512], sFee[16], sDep[16], sRef[16], sKept[16];
	UI_LvFmt(fee < 0 ? 0 : fee, sFee, 16);
	UI_LvFmt(deposit < 0 ? 0 : deposit, sDep, 16);
	UI_LvFmt(refund < 0 ? 0 : refund, sRef, 16);
	UI_LvFmt(deposit - refund < 0 ? 0 : deposit - refund, sKept, 16);
	format(body, 512, "{FFFFFF}Veicolo: {FFD24D}%s\n{FFFFFF}Utilizzo: {FFD24D}%d minuti\n{FFFFFF}Canone pagato: {FFD24D}$%s\n{FFFFFF}Cauzione: {FFD24D}$%s\n{FFFFFF}Rimborsata: {66DD66}$%s\n{FFFFFF}Trattenuta per danni o ritardo: {FF8866}$%s\n\n{AAAAAA}Buona strada, e a presto.", modelName, minutesUsed < 0 ? 0 : minutesUsed, sFee, sDep, sRef, sKept);
	Lang_ShowPlayerDialog(playerid, 9713, 0, "Ricevuta noleggio", body, "Chiudi", "");
	return 0;
}

UI_LivingTaxiOffer(playerid, callId, bool:npc, const passenger[], fromDistrict, toDistrict, distanceM, fareEstimate, ttlSec)
{
	if (!UI_LvReady(playerid) || callId <= 0)
	{
		return 0;
	}
	new body[512], sDist[16], sFare[16];
	if (ttlSec <= 0)
	{
		ttlSec = 30;
	}
	g_UiLvTxOfferId[playerid] = callId;
	g_UiLvTxOfferLeft[playerid] = ttlSec;
	UI_LvFmtDist(distanceM, sDist, 16);
	UI_LvFmt(fareEstimate < 0 ? 0 : fareEstimate, sFare, 16);
	format(body, 512, "{FFFFFF}%s {FFD24D}%s{FFFFFF} cerca un taxi.\n\nDa: {FFD24D}%s\n{FFFFFF}A: {FFD24D}%s\n{FFFFFF}Percorso stimato: {FFD24D}%s\n{FFFFFF}Tariffa stimata: {66DD66}$%s\n\n{AAAAAA}Hai %d secondi per rispondere (anche con /accettacorsa).", npc ? "Un cliente," : "Il giocatore", passenger[0] != 0 ? passenger : "-", g_UiLvDistrictName[UI_LvClampDistrict(fromDistrict)], g_UiLvDistrictName[UI_LvClampDistrict(toDistrict)], sDist, sFare, ttlSec);
	Lang_ShowPlayerDialog(playerid, 9714, 0, "Chiamata taxi", body, "Accetta corsa", "Ignora");
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LivingTaxiOfferClose(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	g_UiLvTxOfferId[playerid] = 0;
	g_UiLvTxOfferLeft[playerid] = 0;
	if (!IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (GetPlayerDialogID(playerid) == 9714)
	{
		HidePlayerDialog(playerid);
	}
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LivingTaxiMeter(playerid, fare, distanceM, bool:running)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	UI_LvTdEnsure(playerid);
	new line[128], sFare[16], sDist[16];
	UI_LvFmt(fare < 0 ? 0 : fare, sFare, 16);
	UI_LvFmtDist(distanceM, sDist, 16);
	format(line, 128, "$%s  -  %s", sFare, sDist);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][7], line);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][6], running ? "TASSAMETRO - in corsa" : "TASSAMETRO - fermo");
	PlayerTextDrawColour(playerid, g_UiLvTd[playerid][7], running ? 0x66DD66FF : 0xC8C8C8FF);
	UI_LvTdShow(playerid, 5, 7, 1);
	g_UiLvMeterOn[playerid] = 1;
	return 0;
}

UI_LivingTaxiMeterHide(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	if (g_UiLvMeterOn[playerid] && IsPlayerConnected(playerid))
	{
		UI_LvTdShow(playerid, 5, 7, 0);
	}
	g_UiLvMeterOn[playerid] = 0;
	return 0;
}

UI_LvTaxiPayShow(playerid)
{
	new body[640], sDist[16], sFare[16], disc[64];
	UI_LvFmtDist(g_UiLvTxPayDist[playerid], sDist, 16);
	UI_LvFmt(g_UiLvTxPayFare[playerid], sFare, 16);
	if (g_UiLvTxPayDisc[playerid] > 0)
	{
		format(disc, 64, " {AAAAAA}(sconto residente %d%% incluso)", g_UiLvTxPayDisc[playerid]);
	}
	else
	{
		disc[0] = 0;
	}
	format(body, 640, "{FFFFFF}Corsa con {FFD24D}%s{FFFFFF} conclusa.\n\nPercorso: {FFD24D}%s\n{FFFFFF}Importo: {66DD66}$%s%s\n\n{AAAAAA}Il pagamento parte dal tuo conto in banca. Se qualcosa non va puoi contestare:\nla corsa restera' in sospeso e lo staff la verifichera'. Rispondi entro %d secondi.", g_UiLvTxPayDriver[playerid], sDist, sFare, disc, g_UiLvTxPayLeft[playerid]);
	Lang_ShowPlayerDialog(playerid, 9715, 0, "Pagamento corsa", body, "Paga", "Contesta");
}

UI_LivingTaxiPayConfirm(playerid, rideId, const driver[], distanceM, fare, discountPct, ttlSec)
{
	if (!UI_LvReady(playerid) || rideId <= 0)
	{
		return 0;
	}
	if (ttlSec <= 0)
	{
		ttlSec = 60;
	}
	g_UiLvTxPayId[playerid] = rideId;
	g_UiLvTxPayLeft[playerid] = ttlSec;
	g_UiLvTxPayFare[playerid] = fare < 0 ? 0 : fare;
	g_UiLvTxPayDist[playerid] = distanceM < 0 ? 0 : distanceM;
	g_UiLvTxPayDisc[playerid] = UI_LvClamp(discountPct, 0, 100);
	strcopy(g_UiLvTxPayDriver[playerid], driver[0] != 0 ? driver : "-", 25);
	UI_LvTaxiPayShow(playerid);
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LvTaxiPayReset(playerid)
{
	g_UiLvTxPayId[playerid] = 0;
	g_UiLvTxPayLeft[playerid] = 0;
}

UI_LivingTaxiPayClose(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	UI_LvTaxiPayReset(playerid);
	if (!IsPlayerConnected(playerid))
	{
		return 0;
	}
	new dlg = GetPlayerDialogID(playerid);
	if (dlg == 9715 || dlg == 9716)
	{
		HidePlayerDialog(playerid);
	}
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LivingTaxiRate(playerid, rideId, const driver[])
{
	if (!UI_LvReady(playerid) || rideId <= 0)
	{
		return 0;
	}
	new title[64];
	g_UiLvTxRateId[playerid] = rideId;
	format(title, 64, "Com'e' andata con %s?", driver[0] != 0 ? driver : "il tassista");
	Lang_ShowPlayerDialog(playerid, 9717, 2, title, "{66DD66}***** Ottima\n{66DD66}**** Buona\n{FFD24D}*** Nella norma\n{FFAA33}** Da migliorare\n{FF8866}* Deludente", "Vota", "Salta");
	return 0;
}

UI_LivingTaxiReceipt(playerid, bool:asDriver, rideId, const other[], distanceM, fare, tax, net)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new body[512], title[64], sDist[16], sFare[16], sTax[16], sNet[16];
	UI_LvFmtDist(distanceM, sDist, 16);
	UI_LvFmt(fare < 0 ? 0 : fare, sFare, 16);
	UI_LvFmt(tax < 0 ? 0 : tax, sTax, 16);
	UI_LvFmt(net < 0 ? 0 : net, sNet, 16);
	if (asDriver)
	{
		format(body, 512, "{FFFFFF}Cliente: {FFD24D}%s\n{FFFFFF}Percorso: {FFD24D}%s\n{FFFFFF}Tariffa: {FFD24D}$%s\n{FFFFFF}Tassa comunale: {FF8866}$%s\n{FFFFFF}Accreditato in banca: {66DD66}$%s\n\n{AAAAAA}Buon lavoro, la citta' gira anche grazie a te.", other[0] != 0 ? other : "-", sDist, sFare, sTax, sNet);
	}
	else
	{
		format(body, 512, "{FFFFFF}Tassista: {FFD24D}%s\n{FFFFFF}Percorso: {FFD24D}%s\n{FFFFFF}Pagato: {66DD66}$%s\n\n{AAAAAA}Grazie per aver viaggiato con i taxi di Los Santos.", other[0] != 0 ? other : "-", sDist, sFare);
	}
	format(title, 64, "Ricevuta corsa #%d", rideId);
	Lang_ShowPlayerDialog(playerid, 9718, 0, title, body, "Chiudi", "");
	return 0;
}

UI_LivingProjectSet(playerid, projectId, districtId, const name[], goal, fund, hoursLeft, donMin, donMax, dayLeft, blockCode)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	g_UiLvPjId[playerid] = projectId;
	g_UiLvPjDistrict[playerid] = UI_LvClampDistrict(districtId);
	strcopy(g_UiLvPjName[playerid], name, 64);
	g_UiLvPjGoal[playerid] = goal < 1 ? 1 : goal;
	g_UiLvPjFund[playerid] = fund < 0 ? 0 : fund;
	g_UiLvPjHours[playerid] = hoursLeft < 0 ? 0 : hoursLeft;
	g_UiLvPjMin[playerid] = donMin < 1 ? 1 : donMin;
	g_UiLvPjMax[playerid] = donMax < 0 ? 0 : donMax;
	g_UiLvPjDayLeft[playerid] = dayLeft < 0 ? 0 : dayLeft;
	g_UiLvPjBlock[playerid] = blockCode >= 0 && blockCode < 5 ? blockCode : 3;
	g_UiLvPjAmount[playerid] = 0;
	return 0;
}

bool:UI_LvPrjRange(playerid, &lo, &hi)
{
	lo = g_UiLvPjMin[playerid];
	hi = g_UiLvPjMax[playerid];
	if (hi > g_UiLvPjDayLeft[playerid])
	{
		hi = g_UiLvPjDayLeft[playerid];
	}
	if (hi > 0x3B9ACA00)
	{
		hi = 0x3B9ACA00;
	}
	return g_UiLvPjId[playerid] > 0 && g_UiLvPjBlock[playerid] == 0 && hi >= lo;
}

UI_LivingProjectShow(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		UI_LvCefProject(playerid);
		return 0;
	}
	if (g_UiLvPjId[playerid] <= 0)
	{
		UI_LvInfo(playerid, "Progetti della comunita'", "{FFFFFF}In questo quartiere non c'e' un progetto aperto, per ora.\n\n{AAAAAA}Il prossimo partira' presto: tieni d'occhio la bacheca.");
		return 0;
	}
	new body[1024], title[64], bar[64], sFund[16], sGoal[16], sLo[16], sHi[16], foot[160], lo = 0, hi = 0;
	new bool:can = UI_LvPrjRange(playerid, lo, hi);
	UI_LvBar(g_UiLvPjFund[playerid], g_UiLvPjGoal[playerid], "66DD66", bar, 64);
	UI_LvFmt(g_UiLvPjFund[playerid], sFund, 16);
	UI_LvFmt(g_UiLvPjGoal[playerid], sGoal, 16);
	if (can)
	{
		UI_LvFmt(lo, sLo, 16);
		UI_LvFmt(hi, sHi, 16);
		format(foot, 160, "{66DD66}Puoi contribuire oggi da $%s a $%s.", sLo, sHi);
	}
	else if (g_UiLvPjBlock[playerid] != 0)
	{
		format(foot, 160, "{FFD24D}%s", g_UiLvPrjBlockText[g_UiLvPjBlock[playerid]]);
	}
	else
	{
		strcopy(foot, "{FFD24D}Per oggi hai gia' dato il massimo: grazie davvero.", 160);
	}
	format(body, 1024, "{FFFFFF}%s\n{AAAAAA}Quartiere di %s\n\n%s\n{FFFFFF}Raccolti {66DD66}$%s{FFFFFF} su {FFD24D}$%s{FFFFFF} - mancano %d giorni e %d ore\n\n{AAAAAA}Il fondo cresce con ogni contratto e corsa da e verso il quartiere, senza togliere\nnulla al tuo compenso, e con le donazioni di chi ci vive. Ogni aiuto conta.\n\n%s", g_UiLvPjName[playerid], g_UiLvDistrictName[g_UiLvPjDistrict[playerid]], bar, sFund, sGoal, g_UiLvPjHours[playerid] / 24, g_UiLvPjHours[playerid] % 24, foot);
	strcopy(title, "Progetto della comunita'", 64);
	if (can)
	{
		Lang_ShowPlayerDialog(playerid, 9719, 0, title, body, "Contribuisci", "Chiudi");
	}
	else
	{
		Lang_ShowPlayerDialog(playerid, 9719, 0, title, body, "Chiudi", "");
	}
	return 0;
}

UI_LvPrjAmountShow(playerid, const err[])
{
	new lo = 0, hi = 0, body[512], sLo[16], sHi[16];
	if (!UI_LvPrjRange(playerid, lo, hi))
	{
		UI_LivingProjectShow(playerid);
		return 0;
	}
	UI_LvFmt(lo, sLo, 16);
	UI_LvFmt(hi, sHi, 16);
	format(body, 512, "%s{FFFFFF}Quanto vuoi donare a {FFD24D}%s{FFFFFF}?\nImporto ammesso: da {FFD24D}$%s{FFFFFF} a {FFD24D}$%s{FFFFFF}. Inserisci un numero intero:", err, g_UiLvPjName[playerid], sLo, sHi);
	Lang_ShowPlayerDialog(playerid, 9720, 1, "Contribuisci", body, "Avanti", "Indietro");
	return 0;
}

UI_LvPrjConfirmShow(playerid)
{
	new body[384], sAmt[16];
	UI_LvFmt(g_UiLvPjAmount[playerid], sAmt, 16);
	format(body, 384, "{FFFFFF}Doni {66DD66}$%s{FFFFFF} a {FFD24D}%s{FFFFFF}?\n\n{AAAAAA}L'importo si paga dal conto in banca, poi dai contanti, e non si restituisce.\nIl quartiere se ne ricordera'.", sAmt, g_UiLvPjName[playerid]);
	Lang_ShowPlayerDialog(playerid, 9721, 0, "Conferma donazione", body, "Dona", "Indietro");
}

bool:UI_LvPrjCheck(playerid, amount, err[], size)
{
	new lo = 0, hi = 0, sLo[16], sHi[16];
	err[0] = 0;
	if (!UI_LvPrjRange(playerid, lo, hi))
	{
		return 0;
	}
	if (amount < lo || amount > hi)
	{
		UI_LvFmt(lo, sLo, 16);
		UI_LvFmt(hi, sHi, 16);
		format(err, size, "{FF6666}Importo fuori limite: da $%s a $%s.\n\n", sLo, sHi);
		return 0;
	}
	return 1;
}

UI_LivingProjectContribute(playerid, amount)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new lo = 0, hi = 0, err[160];
	if (!UI_LvPrjRange(playerid, lo, hi))
	{
		UI_LivingProjectShow(playerid);
		return 0;
	}
	if (amount <= 0)
	{
		UI_LvPrjAmountShow(playerid, "");
		return 0;
	}
	if (!UI_LvPrjCheck(playerid, amount, err, 160))
	{
		UI_LvPrjAmountShow(playerid, err);
		return 0;
	}
	g_UiLvPjAmount[playerid] = amount;
	UI_LvPrjConfirmShow(playerid);
	return 0;
}

UI_LivingHelpSet(playerid, step, bool:hintsOff)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	g_UiLvHpStep[playerid] = UI_LvClamp(step, 1, 6);
	g_UiLvHpOff[playerid] = hintsOff;
	return 0;
}

UI_LivingHelpShow(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new body[1024], line[128], title[64], cur = g_UiLvHpStep[playerid];
	if (cur < 1)
	{
		cur = 1;
	}
	body[0] = 0;
	for (new s = 1; s <= 5; s++)
	{
		if (s < cur)
		{
			format(line, 128, "{66DD66}[fatto] {AAAAAA}%d. %s\n", s, g_UiLvStepTitle[s]);
		}
		else if (s == cur)
		{
			format(line, 128, "{FFD24D}[ora] {FFFFFF}%d. %s\n", s, g_UiLvStepTitle[s]);
		}
		else
		{
			format(line, 128, "{888888}%d. %s\n", s, g_UiLvStepTitle[s]);
		}
		strcat(body, line, 1024);
	}
	g_UiLvHpOffRow[playerid] = -1;
	if (cur <= 5 && !g_UiLvHpOff[playerid])
	{
		strcat(body, "{AAAAAA}Spegni i suggerimenti", 1024);
		g_UiLvHpOffRow[playerid] = 5;
	}
	if (cur > 5)
	{
		strcopy(title, "Primo giorno - completato", 64);
	}
	else
	{
		format(title, 64, "Primo giorno - passo %d di %d", cur, 5);
	}
	Lang_ShowPlayerDialog(playerid, 9722, 2, title, body, "Apri", "Chiudi");
	return 0;
}

UI_LvHelpStepShow(playerid, step)
{
	new body[512], title[64], cur = g_UiLvHpStep[playerid];
	g_UiLvHpView[playerid] = step;
	format(body, 512, "{FFFFFF}%s\n\n%s", g_UiLvStepText[step], step < cur ? "{66DD66}Fatto." : (step == cur ? "{FFD24D}E' il tuo prossimo passo." : "{AAAAAA}Ci arriverai presto."));
	format(title, 64, "%d. %s", step, g_UiLvStepTitle[step]);
	if (step == cur && !g_UiLvHpOff[playerid])
	{
		Lang_ShowPlayerDialog(playerid, 9723, 0, title, body, "Segna sulla mappa", "Indietro");
	}
	else
	{
		Lang_ShowPlayerDialog(playerid, 9723, 0, title, body, "Indietro", "Chiudi");
	}
}

UI_LivingHint(playerid, step)
{
	if (!UI_LvReady(playerid) || step < 1 || step > 5 || g_UiLvHpOff[playerid])
	{
		return 0;
	}
	UI_LvTdEnsure(playerid);
	new text[160];
	format(text, 160, "~y~Primo giorno %d/%d~n~~w~%s", step, 5, g_UiLvStepHint[step]);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][9], text);
	UI_LvTdShow(playerid, 8, 9, 1);
	g_UiLvHintTtl[playerid] = 10;
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LivingHintHide(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	if (g_UiLvHintTtl[playerid] > 0 && IsPlayerConnected(playerid))
	{
		UI_LvTdShow(playerid, 8, 9, 0);
	}
	g_UiLvHintTtl[playerid] = 0;
	if (IsPlayerConnected(playerid))
	{
		UI_LvTimerSync(playerid);
	}
	return 0;
}

UI_LivingHelpComplete(playerid, bool:rewarded)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	g_UiLvHpStep[playerid] = 6;
	UI_LivingHintHide(playerid);
	if (rewarded)
	{
		UI_LvInfo(playerid, "Benvenuto a Los Santos", "{FFFFFF}Hai fatto il tuo primo giorno in citta'.\n\nLa Casa del Quartiere ti ha lasciato un {66DD66}pacco accoglienza{FFFFFF}: acqua, un pasto pronto e qualche benda.\n\n{AAAAAA}Da qui in poi la strada la scegli tu. Idlewood ti aspetta, quando vuoi.");
	}
	else
	{
		UI_LvInfo(playerid, "Suggerimenti spenti", "{FFFFFF}Va bene, niente piu' suggerimenti.\n\n{AAAAAA}Se ti serve una mano, la Casa del Quartiere di Idlewood e' sempre aperta.");
	}
	return 0;
}

UI_LvConfirmsReset(playerid)
{
	g_UiLvTxOfferId[playerid] = 0;
	g_UiLvTxOfferLeft[playerid] = 0;
	UI_LvTaxiPayReset(playerid);
	g_UiLvRnReturnId[playerid] = 0;
	g_UiLvTxRateId[playerid] = 0;
}

UI_LivingClose(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	UI_LvConfirmsReset(playerid);
	if (!IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (UI_LvIsLivingDialog(GetPlayerDialogID(playerid)))
	{
		HidePlayerDialog(playerid);
	}
	UI_LvTimerSync(playerid);
	return 0;
}

UI_LivingReject(playerid, const msg[])
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	UI_LivingClose(playerid);
	if (CefUI_Active(playerid))
	{
		if (msg[0] != 0)
		{
			SendClientMessage(playerid, -10066177, msg);
		}
		CefUI_Reject(playerid, msg);
		return 0;
	}
	if (msg[0] != 0)
	{
		UI_LivingNotify(playerid, msg, 3);
	}
	return 0;
}

UI_LivingUiDestroy(playerid)
{
	if (!UI_LvValid(playerid))
	{
		return 0;
	}
	UI_LvTimerStop(playerid);
	if (g_UiLvTdMade[playerid])
	{
		for (new i = 0; i < 11; i++)
		{
			PlayerTextDrawDestroy(playerid, g_UiLvTd[playerid][i]);
			g_UiLvTd[playerid][i] = 65535;
		}
	}
	g_UiLvTdMade[playerid] = 0;
	g_UiLvToastTtl[playerid] = 0;
	g_UiLvHintTtl[playerid] = 0;
	g_UiLvHudOn[playerid] = 0;
	g_UiLvHudId[playerid] = 0;
	g_UiLvHudExpAt[playerid] = 0;
	g_UiLvMeterOn[playerid] = 0;
	UI_LvConfirmsReset(playerid);
	g_UiLvBdCount[playerid] = 0;
	g_UiLvBdRows[playerid] = 0;
	g_UiLvBdAt[playerid] = 0;
	g_UiLvFltDistrict[playerid] = 0;
	g_UiLvFltKind[playerid] = 0;
	g_UiLvFltEligible[playerid] = 0;
	g_UiLvDetIdx[playerid] = -1;
	g_UiLvDetCan[playerid] = 0;
	g_UiLvStId[playerid] = 0;
	g_UiLvDsId[playerid] = 0;
	g_UiLvDsEvCount[playerid] = 0;
	g_UiLvDsRumor[playerid][0] = 0;
	UI_CityRepBegin(playerid);
	g_UiLvRnCount[playerid] = 0;
	g_UiLvRnSel[playerid] = -1;
	g_UiLvRnBlocks[playerid] = 0;
	g_UiLvPjId[playerid] = 0;
	g_UiLvPjAmount[playerid] = 0;
	g_UiLvHpStep[playerid] = 1;
	g_UiLvHpOff[playerid] = 0;
	g_UiLvHpView[playerid] = 0;
	g_UiLvHpOffRow[playerid] = -1;
	return 0;
}

public UI_LivingOnTimer(playerid)
{
	if (!UI_LvReady(playerid))
	{
		if (UI_LvValid(playerid))
		{
			UI_LvTimerStop(playerid);
		}
		return 0;
	}
	if (g_UiLvToastTtl[playerid] > 0)
	{
		g_UiLvToastTtl[playerid]--;
		if (g_UiLvToastTtl[playerid] <= 0 && g_UiLvTdMade[playerid])
		{
			PlayerTextDrawHide(playerid, g_UiLvTd[playerid][10]);
		}
	}
	if (g_UiLvHintTtl[playerid] > 0)
	{
		g_UiLvHintTtl[playerid]--;
		if (g_UiLvHintTtl[playerid] <= 0)
		{
			UI_LvTdShow(playerid, 8, 9, 0);
		}
	}
	if (g_UiLvHudOn[playerid] && g_UiLvHudExpAt[playerid] != 0)
	{
		UI_LvHudRender(playerid);
	}
	if (g_UiLvTxOfferLeft[playerid] > 0)
	{
		g_UiLvTxOfferLeft[playerid]--;
		if (g_UiLvTxOfferLeft[playerid] <= 0)
		{
			g_UiLvTxOfferId[playerid] = 0;
			if (GetPlayerDialogID(playerid) == 9714)
			{
				HidePlayerDialog(playerid);
			}
			UI_LivingToast(playerid, "La chiamata e' passata a un altro taxi", 2);
		}
	}
	if (g_UiLvTxPayLeft[playerid] > 0)
	{
		g_UiLvTxPayLeft[playerid]--;
		if (g_UiLvTxPayLeft[playerid] <= 0)
		{
			new ride = g_UiLvTxPayId[playerid], dlg = GetPlayerDialogID(playerid);
			UI_LvTaxiPayReset(playerid);
			if (dlg == 9715 || dlg == 9716)
			{
				HidePlayerDialog(playerid);
			}
			UI_LivingToast(playerid, "Tempo scaduto per il pagamento della corsa", 2);
			if (ride > 0)
			{
				UI_OnTaxiPayExpired(playerid, ride);
			}
		}
	}
	UI_LvTimerSync(playerid);
	return 1;
}

bool:UI_LivingOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	if (!UI_LvValid(playerid) || !UI_LvIsLivingDialog(dialogid))
	{
		return 0;
	}
	switch (dialogid)
	{
		case 9700:
		{
			if (!response)
			{
				return 1;
			}
			if (listitem == 0)
			{
				UI_LvFilterShow(playerid);
				return 1;
			}
			new row = listitem - 1, rows = g_UiLvBdRows[playerid];
			if (row >= 0 && row < rows)
			{
				UI_LvDetailShow(playerid, g_UiLvBdRow[playerid][row]);
				return 1;
			}
			if (row == (rows == 0 ? 1 : rows))
			{
				UI_LivingToast(playerid, "Aggiorno la bacheca...", 0);
				UI_OnContractBoardRefresh(playerid);
				return 1;
			}
			UI_LvBoardRender(playerid);
			return 1;
		}
		case 9701:
		{
			if (!response)
			{
				UI_LvBoardRender(playerid);
				return 1;
			}
			switch (listitem)
			{
				case 0:
				{
					g_UiLvFltDistrict[playerid] = (g_UiLvFltDistrict[playerid] + 1) % 4;
				}
				case 1:
				{
					g_UiLvFltKind[playerid] = (g_UiLvFltKind[playerid] + 1) % 2;
				}
				case 2:
				{
					g_UiLvFltEligible[playerid] = !g_UiLvFltEligible[playerid];
				}
				default:
				{
					UI_LvBoardRender(playerid);
					return 1;
				}
			}
			UI_LvFilterShow(playerid);
			return 1;
		}
		case 9702:
		{
			new i = g_UiLvDetIdx[playerid];
			if (g_UiLvDetCan[playerid])
			{
				if (response && i >= 0 && i < g_UiLvBdCount[playerid])
				{
					UI_LvAcceptShow(playerid, i);
				}
				else
				{
					UI_LvBoardRender(playerid);
				}
			}
			else if (response)
			{
				UI_LvBoardRender(playerid);
			}
			return 1;
		}
		case 9703:
		{
			new i = g_UiLvDetIdx[playerid];
			if (i < 0 || i >= g_UiLvBdCount[playerid])
			{
				UI_LvBoardRender(playerid);
				return 1;
			}
			if (!response)
			{
				UI_LvDetailShow(playerid, i);
				return 1;
			}
			g_UiLvDetCan[playerid] = 0;
			UI_LivingToast(playerid, "Contratto richiesto, un attimo...", 0);
			UI_OnContractAccept(playerid, g_UiLvBdId[playerid][i]);
			return 1;
		}
		case 9704:
		{
			if (!response)
			{
				return 1;
			}
			if (g_UiLvStId[playerid] <= 0)
			{
				UI_OnContractBoardRefresh(playerid);
				return 1;
			}
			new body[256];
			format(body, 256, "{FFFFFF}Vuoi davvero lasciare il contratto {FFD24D}#%d{FFFFFF}?\n\n{AAAAAA}Il mezzo tornera' al magazzino e il contratto tornera' in bacheca per qualcun altro.", g_UiLvStId[playerid]);
			Lang_ShowPlayerDialog(playerid, 9705, 0, "Lasciare il contratto?", body, "Lascia", "Indietro");
			return 1;
		}
		case 9705:
		{
			new id = g_UiLvStId[playerid];
			if (id <= 0)
			{
				return 1;
			}
			if (!response)
			{
				UI_LivingToast(playerid, "Il contratto resta tuo", 1);
				return 1;
			}
			g_UiLvStId[playerid] = 0;
			UI_OnContractAbandon(playerid, id);
			return 1;
		}
		case 9706:
		{
			if (!response)
			{
				return 1;
			}
			if (listitem >= 0 && listitem < 3)
			{
				UI_OnDistrictPick(playerid, listitem + 1);
			}
			return 1;
		}
		case 9707:
		{
			if (response && g_UiLvDsId[playerid] != 0)
			{
				UI_OnProjectOpen(playerid, g_UiLvDsId[playerid]);
			}
			return 1;
		}
		case 9708, 9713, 9718, 9725:
		{
			return 1;
		}
		case 9709:
		{
			if (!response)
			{
				return 1;
			}
			if (listitem < 0 || listitem >= g_UiLvRnCount[playerid])
			{
				UI_LvRentModelShow(playerid);
				return 1;
			}
			if (!g_UiLvRnAvail[playerid][listitem])
			{
				UI_LivingToast(playerid, "Questo mezzo non e' disponibile per te ora", 2);
				UI_LvRentModelShow(playerid);
				return 1;
			}
			g_UiLvRnSel[playerid] = listitem;
			UI_LvRentBlocksShow(playerid);
			return 1;
		}
		case 9710:
		{
			new s = g_UiLvRnSel[playerid];
			if (!response || s < 0 || s >= g_UiLvRnCount[playerid])
			{
				UI_LvRentModelShow(playerid);
				return 1;
			}
			if (listitem < 0 || listitem >= g_UiLvRnMaxBlocks[playerid])
			{
				UI_LvRentBlocksShow(playerid);
				return 1;
			}
			g_UiLvRnBlocks[playerid] = listitem + 1;
			UI_LvRentSummaryShow(playerid);
			return 1;
		}
		case 9711:
		{
			new s = g_UiLvRnSel[playerid], b = g_UiLvRnBlocks[playerid];
			if (s < 0 || s >= g_UiLvRnCount[playerid] || b < 1)
			{
				UI_LvRentModelShow(playerid);
				return 1;
			}
			if (!response)
			{
				UI_LvRentBlocksShow(playerid);
				return 1;
			}
			g_UiLvRnSel[playerid] = -1;
			g_UiLvRnBlocks[playerid] = 0;
			UI_LivingToast(playerid, "Preparo il mezzo...", 0);
			UI_OnRentalConfirm(playerid, g_UiLvRnIdx[playerid][s], b);
			return 1;
		}
		case 9712:
		{
			new id = g_UiLvRnReturnId[playerid];
			g_UiLvRnReturnId[playerid] = 0;
			if (id <= 0 || !response)
			{
				return 1;
			}
			UI_OnRentalReturnConfirm(playerid, id);
			return 1;
		}
		case 9714:
		{
			new id = g_UiLvTxOfferId[playerid];
			g_UiLvTxOfferId[playerid] = 0;
			g_UiLvTxOfferLeft[playerid] = 0;
			UI_LvTimerSync(playerid);
			if (id <= 0 || !response)
			{
				return 1;
			}
			UI_OnTaxiCallAccept(playerid, id);
			return 1;
		}
		case 9715:
		{
			new id = g_UiLvTxPayId[playerid];
			if (id <= 0)
			{
				return 1;
			}
			if (response)
			{
				UI_LvTaxiPayReset(playerid);
				UI_LvTimerSync(playerid);
				UI_OnTaxiPayConfirm(playerid, id, 1);
				return 1;
			}
			Lang_ShowPlayerDialog(playerid, 9716, 0, "Contestare la corsa?", "{FFFFFF}Se contesti, la corsa non viene pagata e resta in sospeso per lo staff.\n\n{AAAAAA}Usalo solo se qualcosa e' andato davvero storto.", "Contesta", "Indietro");
			return 1;
		}
		case 9716:
		{
			new id = g_UiLvTxPayId[playerid];
			if (id <= 0)
			{
				return 1;
			}
			if (!response)
			{
				UI_LvTaxiPayShow(playerid);
				return 1;
			}
			UI_LvTaxiPayReset(playerid);
			UI_LvTimerSync(playerid);
			UI_OnTaxiPayConfirm(playerid, id, 0);
			return 1;
		}
		case 9717:
		{
			new id = g_UiLvTxRateId[playerid];
			g_UiLvTxRateId[playerid] = 0;
			if (id <= 0 || !response || listitem < 0 || listitem >= 5)
			{
				return 1;
			}
			UI_LivingToast(playerid, "Grazie per il tuo voto", 1);
			UI_OnTaxiRate(playerid, id, 5 - listitem);
			return 1;
		}
		case 9719:
		{
			new lo = 0, hi = 0;
			if (response && UI_LvPrjRange(playerid, lo, hi))
			{
				UI_LvPrjAmountShow(playerid, "");
			}
			return 1;
		}
		case 9720:
		{
			if (!response)
			{
				UI_LivingProjectShow(playerid);
				return 1;
			}
			new amount = 0, err[160];
			if (!UI_LvParseAmount(inputtext, amount))
			{
				UI_LvPrjAmountShow(playerid, "{FF6666}Importo non valido: solo cifre, intero positivo.\n\n");
				return 1;
			}
			if (!UI_LvPrjCheck(playerid, amount, err, 160))
			{
				if (err[0] != 0)
				{
					UI_LvPrjAmountShow(playerid, err);
				}
				else
				{
					UI_LivingProjectShow(playerid);
				}
				return 1;
			}
			g_UiLvPjAmount[playerid] = amount;
			UI_LvPrjConfirmShow(playerid);
			return 1;
		}
		case 9721:
		{
			new amount = g_UiLvPjAmount[playerid], err[160];
			if (!response)
			{
				UI_LvPrjAmountShow(playerid, "");
				return 1;
			}
			if (amount <= 0 || !UI_LvPrjCheck(playerid, amount, err, 160))
			{
				UI_LivingProjectShow(playerid);
				return 1;
			}
			g_UiLvPjAmount[playerid] = 0;
			UI_LivingToast(playerid, "Grazie: registro la tua donazione...", 1);
			UI_OnProjectContribute(playerid, g_UiLvPjId[playerid], amount);
			return 1;
		}
		case 9722:
		{
			if (!response)
			{
				return 1;
			}
			if (listitem >= 0 && listitem < 5)
			{
				UI_LvHelpStepShow(playerid, listitem + 1);
				return 1;
			}
			if (listitem >= 0 && listitem == g_UiLvHpOffRow[playerid])
			{
				Lang_ShowPlayerDialog(playerid, 9724, 0, "Spegnere i suggerimenti?", "{FFFFFF}Il percorso del primo giorno si chiude qui, senza il pacco accoglienza.\n\n{AAAAAA}Potrai sempre chiedere a chi incontri: Los Santos e' piu' gentile di quanto sembri.", "Spegni", "Indietro");
				return 1;
			}
			UI_LivingHelpShow(playerid);
			return 1;
		}
		case 9723:
		{
			new step = g_UiLvHpView[playerid];
			new bool:mapBtn = step == g_UiLvHpStep[playerid] && !g_UiLvHpOff[playerid];
			if (mapBtn)
			{
				if (response && step >= 1 && step <= 5)
				{
					UI_LivingToast(playerid, "Ti segno la strada sulla mappa", 1);
					UI_OnHelpWaypoint(playerid, step);
				}
				else
				{
					UI_LivingHelpShow(playerid);
				}
			}
			else if (response)
			{
				UI_LivingHelpShow(playerid);
			}
			return 1;
		}
		case 9724:
		{
			if (!response)
			{
				UI_LivingHelpShow(playerid);
				return 1;
			}
			g_UiLvHpOff[playerid] = 1;
			UI_LivingHintHide(playerid);
			UI_OnHelpHintsOff(playerid);
			return 1;
		}
	}
	return 1;
}
