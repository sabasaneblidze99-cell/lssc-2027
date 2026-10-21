new g_UiLvDistrictName[4][16] = {
	"ყველა",
	"Idlewood",
	"Ganton",
	"Jefferson"
};
new g_UiLvCargoName[4][56] = {
	"სხვადასხვა ტვირთი",
	"ზოგადი ტვირთი",
	"სურსათი",
	"სამშენებლო მასალები"
};
new g_UiLvKindName[2][56] = {
	"ყველა",
	"ტვირთის გადაზიდვა"
};
new g_UiLvTrackName[3][32] = {
	{0},
	"ტრანსპორტი",
	"ტაქსი"
};
new g_UiLvTierName[4][32] = {
	"დამწყები",
	"სანდო",
	"გამოცდილი",
	"ელიტა"
};
new g_UiLvBlockText[11][104] = {
	{0},
	"დონე არ გყოფნის",
	"ტრანსპორტში მეტი რეპუტაცია გჭირდება",
	"უბანში მეტი ნდობა გჭირდება",
	"უკვე გაქვს მიმდინარე კონტრაქტი",
	"უკვე სხვა სამსახურით ხარ დაკავებული",
	"დაისვენე, ცოტა ხანში დაბრუნდი",
	"ამ საათში ბევრი იმუშავე",
	"ახლახან დატოვე: მოგვიანებით სცადე",
	"უკვე აიღო სხვა მძღოლმა",
	"ამჟამად მიუწვდომელია"
};
new g_UiLvPrjBlockText[5][176] = {
	{0},
	"შემოწირულობას ქალაქში გარკვეული დროის გატარების შემდეგ შეძლებ.",
	"დღეს მაქსიმუმი უკვე გაიღე: დიდი მადლობა.",
	"ეს პროექტი თანხას აღარ აგროვებს.",
	"შემოწირულობა კეთდება უბნის სახლში ან დაფაზე."
};
new g_UiLvEventName[10][72] = {
	"ღონისძიება",
	"მიწოდება დასრულდა",
	"მიწოდება ჩაიშალა",
	"ტაქსით მგზავრობა",
	"ქირაობა დაიწყო",
	"ნაქირავები დაბრუნდა",
	"დანაშაულის შეტყობინება",
	"გამვლელი პატრული",
	"საზოგადოების პროექტი",
	"ადმინისტრაციის ჩარევა"
};
new g_UiLvStepTitle[6][96] = {
	{0},
	"მიდი უბნის სახლში",
	"წაიკითხე /quartiere",
	"შეასრულე მისასალმებელი მიწოდება",
	"გახსენი /reputazione",
	"გაეცანი /lavoro-ს ან /noleggio-ს"
};
new g_UiLvStepText[6][296] = {
	{0},
	"Unity Station-იდან ორ ნაბიჯში, Idlewood-ის ფარდულის ქვეშ, უბნის სახლია: კონტრაქტების დაფა და ვიღაც, ვინც უბანს კარგად იცნობს.",
	"გაიგე, როგორაა Idlewood: საქმიანობა, უსაფრთხოება, ქუჩის ხმები. უბანი იცვლება იმით, რასაც ყველანი ვაკეთებთ.",
	"დაფაზე მიიღე მისასალმებელი მიწოდება: ტვირთი საწყობიდან Idlewood-ის მაღაზიამდე. არ იჩქარო, გზა მოკლეა.",
	"ნახე, როგორ გიცნობს ქალაქი: შენი ხელობა და სახელი უბნებში კარგად შესრულებული საქმით იზრდება.",
	"Pershing Square-ის Job Center სტაბილურ სამსახურს გთავაზობს; თუ მანქანა გჭირდება, ქირაობა იქვე, გვერდითაა."
};
new g_UiLvStepHint[6][136] = {
	{0},
	"მიჰყევი ნიშნულს: უბნის სახლი სადგურთან ახლოსაა.",
	"დაწერე /quartiere, რომ Idlewood გაიცნო.",
	"დაფასთან, /contratti: შენთვის მისასალმებელი მიწოდებაა.",
	"დაწერე /reputazione, რომ ნახო, როგორ გიცნობს ქალაქი.",
	"ბოლო ნაბიჯი: /lavoro ან /noleggio, შენ აირჩიე."
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
new PlayerText:g_UiLvTd[MAX_PLAYERS][11];
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
	// importo in tetri -> "1.250,50" (economy/currency.inc)
	return Money_Format(value, dest, size);
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
		format(dest, size, "%d წთ-ის წინ", seconds / 60);
	}
	else if (seconds < 86400)
	{
		format(dest, size, "%d სთ-ის წინ", seconds / 3600);
	}
	else
	{
		format(dest, size, "%d დღის წინ", seconds / 86400);
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
	// lari digitati ("25", "12,50") -> tetri (economy/currency.inc)
	return Money_Parse(s, value);
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
		format(dest, size, "{66DD66}შინაური{AAAAAA} (-%d%% ქირაზე და ტაქსზე)", 10);
	}
	else if (rep >= 10)
	{
		format(dest, size, "{FFD24D}ნაცნობი{AAAAAA} (-%d%% ქირაზე და ტაქსზე)", 5);
	}
	else if (rep < 0)
	{
		strcopy(dest, "{FFAA33}დასაბრუნებელი{AAAAAA} (უფრო სწრაფად აღდგება)", size);
	}
	else
	{
		strcopy(dest, "{FFFFFF}უცნობი", size);
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
	Lang_ShowPlayerDialog(playerid, 9725, DIALOG_STYLE_MSGBOX, title, body, "დახურვა", "");
}

PlayerText:UI_LvText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, align);
	PlayerTextDrawColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
	PlayerTextDrawSetProportional(playerid, td, true);
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
	PlayerTextDrawUseBox(playerid, td, true);
	PlayerTextDrawBoxColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
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
	g_UiLvTd[playerid][1] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 2.0), "კონტრაქტი", 0.22, 1.0, -2994689, t_TEXT_DRAW_ALIGN:1);
	g_UiLvTd[playerid][2] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 15.0), "_", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiLvTd[playerid][3] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 27.0), "_", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiLvTd[playerid][4] = UI_LvText(playerid, (14.0 + 5.0), (150.0 + 39.0), "_", 0.2, 0.9, 0xC8C8C8FF, t_TEXT_DRAW_ALIGN:1);
	g_UiLvTd[playerid][5] = UI_LvBox(playerid, 14.0, 212.0, 150.0, 32.0, 153);
	g_UiLvTd[playerid][6] = UI_LvText(playerid, (14.0 + 5.0), (212.0 + 2.0), "ტაქსომეტრი", 0.22, 1.0, -2994689, t_TEXT_DRAW_ALIGN:1);
	g_UiLvTd[playerid][7] = UI_LvText(playerid, (14.0 + 5.0), (212.0 + 15.0), "_", 0.26, 1.1, 0x66DD66FF, t_TEXT_DRAW_ALIGN:1);
	g_UiLvTd[playerid][8] = UI_LvBox(playerid, 14.0, 252.0, 220.0, 40.0, 153);
	g_UiLvTd[playerid][9] = UI_LvText(playerid, (14.0 + 5.0), (252.0 + 2.0), "_", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	PlayerTextDrawTextSize(playerid, PlayerText:g_UiLvTd[playerid][9], ((14.0 + 220.0) - 4.0), 0.0);
	g_UiLvTd[playerid][10] = UI_LvText(playerid, 320.0, 318.0, "_", 0.24, 1.1, -2994689, t_TEXT_DRAW_ALIGN:2);
	g_UiLvTdMade[playerid] = true;
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
			PlayerTextDrawShow(playerid, PlayerText:g_UiLvTd[playerid][i]);
		}
		else
		{
			PlayerTextDrawHide(playerid, PlayerText:g_UiLvTd[playerid][i]);
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
			g_UiLvTimer[playerid] = SetTimerEx("UI_LivingOnTimer", 1000, true, "d", playerid);
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
	PlayerTextDrawColour(playerid, PlayerText:g_UiLvTd[playerid][10], UI_LvToastColor(kind));
	PlayerTextDrawShow(playerid, PlayerText:g_UiLvTd[playerid][10]);
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
		return false;
	}
	if (g_UiLvFltKind[playerid] != 0 && g_UiLvBdKind[playerid][i] != g_UiLvFltKind[playerid])
	{
		return false;
	}
	if (g_UiLvFltEligible[playerid] && g_UiLvBdBlock[playerid][i] != 0)
	{
		return false;
	}
	return true;
}

UI_LvRequirement(playerid, i, dest[], size)
{
	if (g_UiLvBdMinRep[playerid][i] > 0)
	{
		format(dest, size, "დონე %d, რეპ. %d", g_UiLvBdMinLevel[playerid][i], g_UiLvBdMinRep[playerid][i]);
	}
	else if (g_UiLvBdMinLRep[playerid][i] > 0)
	{
		format(dest, size, "დონე %d, ნდობა %d", g_UiLvBdMinLevel[playerid][i], g_UiLvBdMinLRep[playerid][i]);
	}
	else
	{
		format(dest, size, "დონე %d", g_UiLvBdMinLevel[playerid][i]);
	}
}

UI_LvBoardRender(playerid)
{
	new body[3072], line[384], title[576], now = gettime();
	new sReward[48], sTime[36], req[120];
	format(body, 3072, "კონტრაქტი\tტვირთი\tანაზღაურება / ვადა\tმოთხოვნა\n{FFD24D}ფილტრი: %s\t{FFD24D}%s\t{FFD24D}%s\t{AAAAAA}შეცვლა\n", g_UiLvDistrictName[g_UiLvFltDistrict[playerid]], g_UiLvKindName[g_UiLvFltKind[playerid]], g_UiLvFltEligible[playerid] ? "მხოლოდ ჩემთვის" : "ყველა");
	g_UiLvBdRows[playerid] = 0;
	for (new i = 0; i < g_UiLvBdCount[playerid]; i++)
	{
		if (!UI_LvBoardMatch(playerid, i))
		{
			continue;
		}
		UI_LvFmt(g_UiLvBdReward[playerid][i], sReward, 48);
		UI_LvFmtTime(g_UiLvBdExpAt[playerid][i] - now, sTime, 36);
		new bool:ok = g_UiLvBdBlock[playerid][i] == 0;
		if (ok)
		{
			UI_LvRequirement(playerid, i, req, 120);
		}
		else
		{
			strcopy(req, g_UiLvBlockText[g_UiLvBdBlock[playerid][i]], 120);
		}
		format(line, 384, "%s#%d %s > %s%s\t%s%s\t%s₾%s / %s\t%s%s\n", ok ? "{FFFFFF}" : "{888888}", g_UiLvBdId[playerid][i], g_UiLvDistrictName[g_UiLvBdFrom[playerid][i]], g_UiLvDistrictName[g_UiLvBdTo[playerid][i]], g_UiLvBdPremium[playerid][i] ? " *" : "", ok ? "{FFFFFF}" : "{888888}", g_UiLvCargoName[g_UiLvBdCargo[playerid][i]], ok ? "{66DD66}" : "{888888}", sReward, sTime, ok ? "{AAAAAA}" : "{FF8888}", req);
		strcat(body, line, 3072);
		g_UiLvBdRow[playerid][g_UiLvBdRows[playerid]] = i;
		g_UiLvBdRows[playerid]++;
	}
	if (g_UiLvBdRows[playerid] == 0)
	{
		strcat(body, "{AAAAAA}ამ ფილტრით კონტრაქტი არ არის\t\t\t\n", 3072);
	}
	strcat(body, "{AAAAAA}დაფის განახლება\t\t\t", 3072);
	format(title, 576, "კონტრაქტების დაფა%s", g_UiLvBdAt[playerid] ? "" : " (დათვალიერება)");
	Lang_ShowPlayerDialog(playerid, 9700, DIALOG_STYLE_TABLIST_HEADERS, title, body, "გახსნა", "დახურვა");
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
	new body[768];
	format(body, 768, "უბანი: {FFD24D}%s\nპროფესია: {FFD24D}%s\nჩვენება: {FFD24D}%s\n{66DD66}დაფაზე დაბრუნება", g_UiLvDistrictName[g_UiLvFltDistrict[playerid]], g_UiLvKindName[g_UiLvFltKind[playerid]], g_UiLvFltEligible[playerid] ? "მხოლოდ ის, რისი მიღებაც შემიძლია" : "ყველა კონტრაქტი");
	Lang_ShowPlayerDialog(playerid, 9701, DIALOG_STYLE_LIST, "დაფის ფილტრი", body, "შეცვლა", "უკან");
}

UI_LvDetailRender(playerid, i)
{
	new body[1024], title[576], sReward[48], sTime[36], sDist[48], req[120], status[480];
	new block = g_UiLvBdBlock[playerid][i];
	UI_LvFmt(g_UiLvBdReward[playerid][i], sReward, 48);
	UI_LvFmtTime(g_UiLvBdExpAt[playerid][i] - gettime(), sTime, 36);
	UI_LvFmtDist(g_UiLvBdDist[playerid][i], sDist, 48);
	UI_LvRequirement(playerid, i, req, 120);
	g_UiLvDetIdx[playerid] = i;
	g_UiLvDetCan[playerid] = block == 0 && g_UiLvBdAt[playerid];
	if (block != 0)
	{
		format(status, 480, "{FF8888}%s.", g_UiLvBlockText[block]);
	}
	else if (!g_UiLvBdAt[playerid])
	{
		strcopy(status, "{FFD24D}მისი მიღება დაფასთან შეგიძლია: Idlewood-ის უბნის სახლი, Ganton-ის ან Jefferson-ის საწყობი.", 160);
	}
	else
	{
		strcopy(status, "{66DD66}ახლავე შეგიძლია მიღება.", 160);
	}
	format(body, 1024, "{FFFFFF}%s%s\n\n{FFFFFF}აღება: {FFD24D}საწყობი, %s\n{FFFFFF}მიწოდება: {FFD24D}მაღაზია, %s\n{FFFFFF}ტვირთი: {FFD24D}%s\n{FFFFFF}მანძილი: {FFD24D}%s\n{FFFFFF}სავარაუდო ანაზღაურება: {66DD66}₾%s\n{FFFFFF}ხელმისაწვდომია კიდევ: {FFD24D}%s\n{FFFFFF}მოთხოვნები: {AAAAAA}%s\n\n{AAAAAA}მანქანას საწყობი მოგცემს. საბოლოო ანაზღაურება ტვირთის მდგომარეობაზეა დამოკიდებული.\n%s", g_UiLvKindName[g_UiLvBdKind[playerid][i]], g_UiLvBdPremium[playerid][i] ? " {FFD24D}(პრემიუმ: უბანი გენდობა)" : "", g_UiLvDistrictName[g_UiLvBdFrom[playerid][i]], g_UiLvDistrictName[g_UiLvBdTo[playerid][i]], g_UiLvCargoName[g_UiLvBdCargo[playerid][i]], sDist, sReward, sTime, req, status);
	format(title, 576, "კონტრაქტი #%d", g_UiLvBdId[playerid][i]);
	if (g_UiLvDetCan[playerid])
	{
		Lang_ShowPlayerDialog(playerid, 9702, DIALOG_STYLE_MSGBOX, title, body, "მიღება", "უკან");
	}
	else
	{
		Lang_ShowPlayerDialog(playerid, 9702, DIALOG_STYLE_MSGBOX, title, body, "უკან", "დახურვა");
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
	new body[1024], sTime[36];
	UI_LvFmtTime(g_UiLvBdExpAt[playerid][i] - gettime(), sTime, 36);
	format(body, 1024, "{FFFFFF}იღებ კონტრაქტს {FFD24D}#%d{FFFFFF}?\n\nფურგონი გელოდება საწყობში: {FFD24D}%s{FFFFFF}.\nტვირთი მიიტანე მაღაზიაში: {FFD24D}%s{FFFFFF}, მშვიდად და ფრთხილად.\n\n{AAAAAA}ერთდროულად მხოლოდ ერთი კონტრაქტის შესრულება შეგიძლია. კონტრაქტი მოქმედებს %s.", g_UiLvBdId[playerid][i], g_UiLvDistrictName[g_UiLvBdFrom[playerid][i]], g_UiLvDistrictName[g_UiLvBdTo[playerid][i]], sTime);
	Lang_ShowPlayerDialog(playerid, 9703, DIALOG_STYLE_MSGBOX, "კონტრაქტის დადასტურება", body, "დადასტურება", "გაუქმება");
}

UI_CityContractStatus(playerid, contractId, ctState, fromDistrict, toDistrict, cargo, reward, secondsLeft, integrityPct, distanceM, repPenalty)
{
	if (!UI_LvReady(playerid) || contractId <= 0)
	{
		return 0;
	}
	new body[1024], title[576], sReward[48], sTime[36], sDist[48], sInteg[144], step[384];
	new from = UI_LvClampDistrict(fromDistrict), to = UI_LvClampDistrict(toDistrict);
	g_UiLvStId[playerid] = contractId;
	if (CefUI_Active(playerid))
	{
		if (ctState == 2)
		{
			format(step, 384, "მიწოდება მაღაზიაში: %s", g_UiLvDistrictName[to]);
		}
		else
		{
			format(step, 384, "ჩატვირთე ტვირთი საწყობში: %s", g_UiLvDistrictName[from]);
		}
		format(sDist, 48, "%s", g_UiLvDistrictName[ctState == 2 ? to : from]);
		cef_emit_event(playerid, "lssc:contract", 1, contractId, 1, ctState, 0, step, 0, sDist, 1, distanceM, 1, secondsLeft, 1, integrityPct, 1, from, 1, to, 1, cargo, 1, reward, 1, repPenalty);
		CefUI_Open(playerid, "phone", "contracts");
		return 0;
	}
	UI_LvFmt(reward < 0 ? 0 : reward, sReward, 48);
	UI_LvFmtTime(secondsLeft, sTime, 36);
	if (distanceM >= 0)
	{
		UI_LvFmtDist(distanceM, sDist, 48);
	}
	else
	{
		strcopy(sDist, "-", 48);
	}
	if (integrityPct >= 0)
	{
		format(sInteg, 144, "%d%%", UI_LvClamp(integrityPct, 0, 100));
	}
	else
	{
		strcopy(sInteg, "-", 144);
	}
	if (ctState == 2)
	{
		format(step, 384, "ტვირთი ჩატვირთულია: მიიტანე მაღაზიაში: %s", g_UiLvDistrictName[to]);
	}
	else
	{
		format(step, 384, "მიდი საწყობში (%s) და ჩატვირთე ტვირთი", g_UiLvDistrictName[from]);
	}
	format(body, 1024, "{FFFFFF}სტატუსი: {FFD24D}%s\n{FFFFFF}შემდეგი ნაბიჯი: {FFD24D}%s\n{FFFFFF}მანძილი: {FFD24D}%s\n{FFFFFF}დარჩენილი დრო: {FFD24D}%s\n{FFFFFF}ტვირთი: {FFD24D}%s{FFFFFF}, მთლიანობა {FFD24D}%s\n{FFFFFF}სავარაუდო ანაზღაურება: {66DD66}₾%s\n\n{AAAAAA}კონტრაქტის დატოვებისას ფულს არ კარგავ, მაგრამ ტრანსპორტში %d რეპუტაციის ქულას კარგავ.", ctState == 2 ? "მიწოდება მიმდინარეობს" : "მიღებულია", step, sDist, sTime, g_UiLvCargoName[cargo >= 1 && cargo <= 3 ? cargo : 0], sInteg, sReward, repPenalty < 0 ? -repPenalty : repPenalty);
	format(title, 576, "შენი კონტრაქტი #%d", contractId);
	Lang_ShowPlayerDialog(playerid, 9704, DIALOG_STYLE_MSGBOX, title, body, "დატოვება", "დახურვა");
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
	Lang_ShowPlayerDialog(playerid, 9704, DIALOG_STYLE_MSGBOX, "კონტრაქტი არ არის", "{FFFFFF}ამჟამად არცერთ კონტრაქტს არ ასრულებ.\n\n{AAAAAA}Idlewood-ის უბნის სახლის დაფაზე ყოველთვის არის საქმე.", "დაფა", "დახურვა");
	return 0;
}

UI_LvHudRender(playerid)
{
	new line[384], sDist[48], sTime[36];
	if (g_UiLvHudDist[playerid] >= 0)
	{
		UI_LvFmtDist(g_UiLvHudDist[playerid], sDist, 48);
		format(line, 384, "მიმართულება: %s (%s)", g_UiLvHudDest[playerid], sDist);
	}
	else
	{
		format(line, 384, "მიმართულება: %s", g_UiLvHudDest[playerid]);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][3], line);
	if (g_UiLvHudExpAt[playerid] != 0)
	{
		new left = g_UiLvHudExpAt[playerid] - gettime();
		UI_LvFmtTime(left < 0 ? 0 : left, sTime, 36);
	}
	else
	{
		strcopy(sTime, "--:--", 36);
	}
	if (g_UiLvHudInteg[playerid] >= 0)
	{
		format(line, 384, "ვადა: %s  -  ტვირთი %d%%", sTime, g_UiLvHudInteg[playerid]);
	}
	else
	{
		format(line, 384, "ვადა: %s", sTime);
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
	new title[384], goal[384];
	g_UiLvHudId[playerid] = contractId;
	g_UiLvHudState[playerid] = ctState;
	g_UiLvHudDist[playerid] = distanceM;
	g_UiLvHudExpAt[playerid] = secondsLeft >= 0 ? gettime() + secondsLeft : 0;
	g_UiLvHudInteg[playerid] = integrityPct >= 0 ? UI_LvClamp(integrityPct, 0, 100) : -1;
	strcopy(g_UiLvHudDest[playerid], destination[0] != 0 ? destination : "-", 48);
	format(title, 384, "კონტრაქტი #%d  -  %s", contractId, ctState == 2 ? "მიწოდებაშია" : "მიღებულია");
	format(goal, 384, "%s", objective[0] != 0 ? objective : "_");
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][1], title);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][2], goal);
	UI_LvHudRender(playerid);
	if (!g_UiLvHudOn[playerid])
	{
		UI_LvTdShow(playerid, 0, 4, true);
	}
	g_UiLvHudOn[playerid] = true;
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
		UI_LvTdShow(playerid, 0, 4, false);
	}
	g_UiLvHudOn[playerid] = false;
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
	new body[384];
	format(body, 384, "%s\n%s\n%s", g_UiLvDistrictName[1], g_UiLvDistrictName[2], g_UiLvDistrictName[3]);
	Lang_ShowPlayerDialog(playerid, 9706, DIALOG_STYLE_LIST, "რომელი უბანი?", body, "გახსნა", "დახურვა");
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
	new body[2560], line[384], title[192], bAct[192], bSec[192], bCrime[192], prosp[288], lrep[240], ago[48];
	new act = g_UiLvDsAct[playerid];
	UI_LvBar(act, 1000, "66DD66", bAct, 192);
	UI_LvBar(g_UiLvDsSec[playerid], 100, "66AAFF", bSec, 192);
	UI_LvBar(g_UiLvDsCrime[playerid], 100, "FF8866", bCrime, 192);
	if (act < 350)
	{
		strcopy(prosp, "{FFAA33}გაჭირვებაშია{AAAAAA}: ყოველი მიწოდება აღდგენას უწყობს ხელს", 96);
	}
	else if (act >= 650)
	{
		strcopy(prosp, "{66DD66}აყვავებული{AAAAAA}: ყველასთვის მეტი სამუშაოა", 96);
	}
	else
	{
		strcopy(prosp, "{FFFFFF}სტაბილური", 96);
	}
	UI_LvLocalLabel(g_UiLvDsLRep[playerid], lrep, 240);
	format(body, 2560, "{FFFFFF}კეთილდღეობა: %s\n\n{FFFFFF}ეკონომიკური აქტივობა  %s {FFFFFF}%d/%d\n{FFFFFF}უსაფრთხოება                %s {FFFFFF}%d/%d\n{FFFFFF}კრიმინალური წნეხი  %s {FFFFFF}%d/%d%s\n\n{FFFFFF}შენი რეპუტაცია აქ: {FFD24D}%d{FFFFFF} - %s\n", prosp, bAct, act, 1000, bSec, g_UiLvDsSec[playerid], 100, bCrime, g_UiLvDsCrime[playerid], 100, g_UiLvDsCrime[playerid] >= 70 ? "  {FF8866}(რისკის ზონა: კონტრაქტები მეტს იხდის)" : "", g_UiLvDsLRep[playerid], lrep);
	if (g_UiLvDsFest[playerid] > 0)
	{
		format(line, 384, "{66DD66}მიმდინარეობს უბნის დღესასწაული: დარჩა %d საათი.\n", g_UiLvDsFest[playerid]);
		strcat(body, line, 2560);
	}
	if (g_UiLvDsRumor[playerid][0] != 0)
	{
		format(line, 384, "\n{AAAAAA}ხმები დადის: {FFFFFF}\"%s\"\n", g_UiLvDsRumor[playerid]);
		strcat(body, line, 2560);
	}
	strcat(body, "\n{FFD24D}ბოლო მოვლენები\n", 2560);
	if (g_UiLvDsEvCount[playerid] == 0)
	{
		strcat(body, "{AAAAAA}ჯერჯერობით მშვიდი დღეა.\n", 2560);
	}
	for (new i = 0; i < g_UiLvDsEvCount[playerid]; i++)
	{
		new v = g_UiLvDsEvValue[playerid][i];
		UI_LvFmtAgo(g_UiLvDsEvAgo[playerid][i], ago, 48);
		if (v > 0)
		{
			format(line, 384, "{AAAAAA}%s  {FFFFFF}%s {66DD66}(+%d აქტივობა)\n", ago, g_UiLvEventName[g_UiLvDsEvType[playerid][i]], v);
		}
		else if (v < 0)
		{
			format(line, 384, "{AAAAAA}%s  {FFFFFF}%s {FF8866}(%d აქტივობა)\n", ago, g_UiLvEventName[g_UiLvDsEvType[playerid][i]], v);
		}
		else
		{
			format(line, 384, "{AAAAAA}%s  {FFFFFF}%s\n", ago, g_UiLvEventName[g_UiLvDsEvType[playerid][i]]);
		}
		strcat(body, line, 2560);
	}
	format(title, 192, "უბანი: %s", g_UiLvDistrictName[d]);
	Lang_ShowPlayerDialog(playerid, 9707, DIALOG_STYLE_MSGBOX, title, body, "პროექტი", "დახურვა");
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
		g_UiLvRpHas[playerid][t] = false;
	}
	for (new d = 0; d <= 3; d++)
	{
		g_UiLvRpLHas[playerid][d] = false;
	}
	return 0;
}

UI_CityRepTrack(playerid, track, points, completed, failed, ratingSum, ratingCnt, dayGain)
{
	if (!UI_LvValid(playerid) || track < 1 || track > 2)
	{
		return 0;
	}
	g_UiLvRpHas[playerid][track] = true;
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
	g_UiLvRpLHas[playerid][d] = true;
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
	new body[2560], line[576], bar[192], lrep[240];
	strcopy(body, "{FFD24D}შენი ხელობა\n", 1280);
	for (new t = 1; t <= 2; t++)
	{
		new pts = g_UiLvRpHas[playerid][t] ? g_UiLvRpPoints[playerid][t] : 0;
		new tier = UI_LvTier(pts), next = UI_LvTierNext(tier);
		if (next > 0)
		{
			UI_LvBar(pts, next, "FFD24D", bar, 192);
			format(line, 576, "{FFFFFF}%s: {FFD24D}%s{FFFFFF} (+%d%%) - %d ქულა\n  %s {AAAAAA}%s - %d ქულის შემდეგ\n", g_UiLvTrackName[t], g_UiLvTierName[tier], UI_LvTierBonus(tier), pts, bar, g_UiLvTierName[tier + 1], next - pts);
		}
		else
		{
			format(line, 576, "{FFFFFF}%s: {66DD66}%s{FFFFFF} (+%d%%) - %d ქულა, უმაღლესი წოდება\n", g_UiLvTrackName[t], g_UiLvTierName[tier], UI_LvTierBonus(tier), pts);
		}
		strcat(body, line, 2560);
		if (g_UiLvRpHas[playerid][t])
		{
			if (g_UiLvRpRCnt[playerid][t] > 0)
			{
				new avg10 = g_UiLvRpRSum[playerid][t] * 10 / g_UiLvRpRCnt[playerid][t];
				format(line, 576, "  {AAAAAA}შესრულდა %d, ჩაიშალა %d, საშუალო შეფასება %d,%d/5, დღეს +%d\n", g_UiLvRpDone[playerid][t], g_UiLvRpFail[playerid][t], avg10 / 10, avg10 % 10, g_UiLvRpDay[playerid][t]);
			}
			else
			{
				format(line, 576, "  {AAAAAA}შესრულდა %d, ჩაიშალა %d, დღეს +%d\n", g_UiLvRpDone[playerid][t], g_UiLvRpFail[playerid][t], g_UiLvRpDay[playerid][t]);
			}
		}
		else
		{
			strcopy(line, "  {AAAAAA}ჯერ არ დაგიწყია: ყველაფერს პირველი ჯერი აქვს.\n", 192);
		}
		strcat(body, line, 2560);
	}
	strcat(body, "\n{FFD24D}შენი სახელი უბნებში\n", 2560);
	for (new d = 1; d <= 3; d++)
	{
		new rep = g_UiLvRpLHas[playerid][d] ? g_UiLvRpLocal[playerid][d] : 0;
		UI_LvLocalLabel(rep, lrep, 240);
		format(line, 576, "{FFFFFF}%s: {FFD24D}%d{FFFFFF} - %s%s\n", g_UiLvDistrictName[d], rep, lrep, rep >= 20 ? " {66DD66}[პრემიუმ კონტრაქტები]" : "");
		strcat(body, line, 2560);
	}
	strcat(body, "\n{AAAAAA}რეპუტაცია არ იყიდება: ის შრომით იზრდება, ყოველდღე ნაბიჯ-ნაბიჯ.", 2560);
	Lang_ShowPlayerDialog(playerid, 9708, DIALOG_STYLE_MSGBOX, "რეპუტაცია", body, "დახურვა", "");
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
	new body[2048], line[384], title[576], sFee[48], sDep[48];
	format(body, 2048, "მანქანა\tქირა / %d წთ\tგირაო\tდონე\n", g_UiLvRnBlockMin[playerid]);
	for (new i = 0; i < g_UiLvRnCount[playerid]; i++)
	{
		new bool:ok = g_UiLvRnAvail[playerid][i];
		UI_LvFmt(g_UiLvRnFee[playerid][i], sFee, 48);
		UI_LvFmt(g_UiLvRnDep[playerid][i], sDep, 48);
		format(line, 384, "%s%s\t%s₾%s\t%s₾%s\t%s%d\n", ok ? "{FFFFFF}" : "{888888}", g_UiLvRnName[playerid][i], ok ? "{66DD66}" : "{888888}", sFee, ok ? "{FFD24D}" : "{888888}", sDep, ok ? "{AAAAAA}" : "{888888}", g_UiLvRnLevel[playerid][i]);
		strcat(body, line, 2048);
	}
	if (g_UiLvRnDisc[playerid] > 0)
	{
		format(title, 576, "ქირაობა - მცხოვრების ფასდაკლება %d%%", g_UiLvRnDisc[playerid]);
	}
	else
	{
		strcopy(title, "მანქანების ქირაობა", 64);
	}
	Lang_ShowPlayerDialog(playerid, 9709, DIALOG_STYLE_TABLIST_HEADERS, title, body, "არჩევა", "დახურვა");
}

UI_LivingRentalShow(playerid)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	if (g_UiLvRnCount[playerid] == 0)
	{
		UI_LvInfo(playerid, "მანქანების ქირაობა", "{FFFFFF}დღეს თავისუფალი მანქანა არ არის. მოგვიანებით შემოიარე.");
		return 0;
	}
	UI_LvRentModelShow(playerid);
	return 0;
}

UI_LvRentBlocksShow(playerid)
{
	new s = g_UiLvRnSel[playerid], body[512], line[384], title[192], sTot[48];
	body[0] = 0;
	for (new b = 1; b <= g_UiLvRnMaxBlocks[playerid]; b++)
	{
		UI_LvFmt(g_UiLvRnFee[playerid][s] * b, sTot, 48);
		format(line, 384, "{FFFFFF}%d წთ\t{66DD66}₾%s\n", g_UiLvRnBlockMin[playerid] * b, sTot);
		strcat(body, line, 512);
	}
	format(title, 192, "%s - რა ვადით?", g_UiLvRnName[playerid][s]);
	Lang_ShowPlayerDialog(playerid, 9710, DIALOG_STYLE_TABLIST, title, body, "შემდეგი", "უკან");
}

UI_LvRentSummaryShow(playerid)
{
	new s = g_UiLvRnSel[playerid], b = g_UiLvRnBlocks[playerid], body[1024], disc[576];
	new sFee[48], sDep[48], sTot[48];
	UI_LvFmt(g_UiLvRnFee[playerid][s] * b, sFee, 48);
	UI_LvFmt(g_UiLvRnDep[playerid][s], sDep, 48);
	UI_LvFmt(g_UiLvRnFee[playerid][s] * b + g_UiLvRnDep[playerid][s], sTot, 48);
	if (g_UiLvRnDisc[playerid] > 0)
	{
		format(disc, 576, " {AAAAAA}(მცხოვრების %d%% ფასდაკლების ჩათვლით)", g_UiLvRnDisc[playerid]);
	}
	else
	{
		disc[0] = 0;
	}
	format(body, 1024, "{FFFFFF}მანქანა: {FFD24D}%s\n{FFFFFF}ხანგრძლივობა: {FFD24D}%d წუთი\n{FFFFFF}ქირა: {66DD66}₾%s%s\n{FFFFFF}გირაო: {FFD24D}₾%s {AAAAAA}(დაბრუნებისას მოგიბრუნდება, მანქანის მდგომარეობის მიხედვით)\n{FFFFFF}ახლა ჩამოგეჭრება: {66DD66}₾%s\n\n{AAAAAA}თანხა ჯერ საბანკო ანგარიშიდან ჩამოიჭრება, შემდეგ ნაღდი ფულიდან. მანქანა აქ დააბრუნე /riconsegna-თი:\nვადის გასვლის შემდეგ რამდენიმე წუთი გაქვს, მერე გირაო სააგენტოს რჩება.", g_UiLvRnName[playerid][s], g_UiLvRnBlockMin[playerid] * b, sFee, disc, sDep, sTot);
	Lang_ShowPlayerDialog(playerid, 9711, DIALOG_STYLE_MSGBOX, "ქირაობის შეჯამება", body, "ქირაობა", "უკან");
}

UI_LivingRentalReturnConfirm(playerid, rentalId, const modelName[], healthPct, deposit, refund)
{
	if (!UI_LvReady(playerid) || rentalId <= 0)
	{
		return 0;
	}
	new body[1024], sDep[48], sRef[48];
	g_UiLvRnReturnId[playerid] = rentalId;
	UI_LvFmt(deposit < 0 ? 0 : deposit, sDep, 48);
	UI_LvFmt(refund < 0 ? 0 : refund, sRef, 48);
	format(body, 1024, "{FFFFFF}აბრუნებ {FFD24D}%s{FFFFFF}-ს?\n\nმანქანის მდგომარეობა: {FFD24D}%d%%\n{FFFFFF}გადახდილი გირაო: {FFD24D}₾%s\n{FFFFFF}დაგიბრუნდება: {66DD66}₾%s\n\n{AAAAAA}ქირა არ ბრუნდება. მადლობა, რომ ჩვენ აგვირჩიე.", modelName, UI_LvClamp(healthPct, 0, 100), sDep, sRef);
	Lang_ShowPlayerDialog(playerid, 9712, DIALOG_STYLE_MSGBOX, "დაბრუნება", body, "დაბრუნება", "გაუქმება");
	return 0;
}

UI_LivingRentalReceipt(playerid, const modelName[], fee, deposit, refund, minutesUsed)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new body[1024], sFee[48], sDep[48], sRef[48], sKept[48];
	UI_LvFmt(fee < 0 ? 0 : fee, sFee, 48);
	UI_LvFmt(deposit < 0 ? 0 : deposit, sDep, 48);
	UI_LvFmt(refund < 0 ? 0 : refund, sRef, 48);
	UI_LvFmt(deposit - refund < 0 ? 0 : deposit - refund, sKept, 48);
	format(body, 1024, "{FFFFFF}მანქანა: {FFD24D}%s\n{FFFFFF}გამოყენება: {FFD24D}%d წუთი\n{FFFFFF}გადახდილი ქირა: {FFD24D}₾%s\n{FFFFFF}გირაო: {FFD24D}₾%s\n{FFFFFF}დაბრუნდა: {66DD66}₾%s\n{FFFFFF}დაკავდა დაზიანების ან დაგვიანებისთვის: {FF8866}₾%s\n\n{AAAAAA}კარგ გზას გისურვებთ, მალე შევხვდებით.", modelName, minutesUsed < 0 ? 0 : minutesUsed, sFee, sDep, sRef, sKept);
	Lang_ShowPlayerDialog(playerid, 9713, DIALOG_STYLE_MSGBOX, "ქირაობის ქვითარი", body, "დახურვა", "");
	return 0;
}

UI_LivingTaxiOffer(playerid, callId, bool:npc, const passenger[], fromDistrict, toDistrict, distanceM, fareEstimate, ttlSec)
{
	if (!UI_LvReady(playerid) || callId <= 0)
	{
		return 0;
	}
	new body[1024], sDist[48], sFare[48];
	if (ttlSec <= 0)
	{
		ttlSec = 30;
	}
	g_UiLvTxOfferId[playerid] = callId;
	g_UiLvTxOfferLeft[playerid] = ttlSec;
	UI_LvFmtDist(distanceM, sDist, 48);
	UI_LvFmt(fareEstimate < 0 ? 0 : fareEstimate, sFare, 48);
	format(body, 1024, "{FFFFFF}%s {FFD24D}%s{FFFFFF} ტაქსს ეძებს.\n\nსაიდან: {FFD24D}%s\n{FFFFFF}სადამდე: {FFD24D}%s\n{FFFFFF}სავარაუდო მარშრუტი: {FFD24D}%s\n{FFFFFF}სავარაუდო ტარიფი: {66DD66}₾%s\n\n{AAAAAA}პასუხისთვის %d წამი გაქვს (ასევე /accettacorsa-თი).", npc ? "კლიენტი," : "მოთამაშე", passenger[0] != 0 ? passenger : "-", g_UiLvDistrictName[UI_LvClampDistrict(fromDistrict)], g_UiLvDistrictName[UI_LvClampDistrict(toDistrict)], sDist, sFare, ttlSec);
	Lang_ShowPlayerDialog(playerid, 9714, DIALOG_STYLE_MSGBOX, "ტაქსის გამოძახება", body, "მიღება", "იგნორი");
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
	new line[384], sFare[48], sDist[48];
	UI_LvFmt(fare < 0 ? 0 : fare, sFare, 48);
	UI_LvFmtDist(distanceM, sDist, 48);
	format(line, 384, "₾%s  -  %s", sFare, sDist);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][7], line);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][6], running ? "ტაქსომეტრი - მოძრაობაში" : "ტაქსომეტრი - გაჩერებული");
	PlayerTextDrawColour(playerid, PlayerText:g_UiLvTd[playerid][7], running ? 0x66DD66FF : 0xC8C8C8FF);
	UI_LvTdShow(playerid, 5, 7, true);
	g_UiLvMeterOn[playerid] = true;
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
		UI_LvTdShow(playerid, 5, 7, false);
	}
	g_UiLvMeterOn[playerid] = false;
	return 0;
}

UI_LvTaxiPayShow(playerid)
{
	new body[640], sDist[48], sFare[48], disc[576];
	UI_LvFmtDist(g_UiLvTxPayDist[playerid], sDist, 48);
	UI_LvFmt(g_UiLvTxPayFare[playerid], sFare, 48);
	if (g_UiLvTxPayDisc[playerid] > 0)
	{
		format(disc, 576, " {AAAAAA}(მცხოვრების %d%% ფასდაკლების ჩათვლით)", g_UiLvTxPayDisc[playerid]);
	}
	else
	{
		disc[0] = 0;
	}
	format(body, 640, "{FFFFFF}მგზავრობა {FFD24D}%s{FFFFFF}-თან დასრულდა.\n\nმარშრუტი: {FFD24D}%s\n{FFFFFF}თანხა: {66DD66}₾%s%s\n\n{AAAAAA}გადახდა შენი საბანკო ანგარიშიდან მოხდება. თუ რამე არასწორია, შეგიძლია გაასაჩივრო:\nმგზავრობა შეჩერდება და ადმინისტრაცია გადაამოწმებს. უპასუხე %d წამში.", g_UiLvTxPayDriver[playerid], sDist, sFare, disc, g_UiLvTxPayLeft[playerid]);
	Lang_ShowPlayerDialog(playerid, 9715, DIALOG_STYLE_MSGBOX, "მგზავრობის გადახდა", body, "გადახდა", "გასაჩივრება");
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
	new title[576];
	g_UiLvTxRateId[playerid] = rideId;
	format(title, 576, "როგორ იყო %s-თან?", driver[0] != 0 ? driver : "ტაქსის მძღოლი");
	Lang_ShowPlayerDialog(playerid, 9717, DIALOG_STYLE_LIST, title, "{66DD66}***** შესანიშნავი\n{66DD66}**** კარგი\n{FFD24D}*** ნორმალური\n{FFAA33}** გასაუმჯობესებელი\n{FF8866}* იმედგაცრუებული", "შეფასება", "გამოტოვება");
	return 0;
}

UI_LivingTaxiReceipt(playerid, bool:asDriver, rideId, const other[], distanceM, fare, tax, net)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new body[1024], title[576], sDist[48], sFare[48], sTax[48], sNet[48];
	UI_LvFmtDist(distanceM, sDist, 48);
	UI_LvFmt(fare < 0 ? 0 : fare, sFare, 48);
	UI_LvFmt(tax < 0 ? 0 : tax, sTax, 48);
	UI_LvFmt(net < 0 ? 0 : net, sNet, 48);
	if (asDriver)
	{
		format(body, 1024, "{FFFFFF}კლიენტი: {FFD24D}%s\n{FFFFFF}მარშრუტი: {FFD24D}%s\n{FFFFFF}ტარიფი: {FFD24D}₾%s\n{FFFFFF}მუნიციპალური გადასახადი: {FF8866}₾%s\n{FFFFFF}ბანკში ჩაირიცხა: {66DD66}₾%s\n\n{AAAAAA}კარგი მუშაობა, ქალაქი შენი დამსახურებითაც მოძრაობს.", other[0] != 0 ? other : "-", sDist, sFare, sTax, sNet);
	}
	else
	{
		format(body, 1024, "{FFFFFF}ტაქსისტი: {FFD24D}%s\n{FFFFFF}მარშრუტი: {FFD24D}%s\n{FFFFFF}გადახდილია: {66DD66}₾%s\n\n{AAAAAA}მადლობა, რომ ლოს-სანტოსის ტაქსით იმგზავრე.", other[0] != 0 ? other : "-", sDist, sFare);
	}
	format(title, 576, "მგზავრობის ქვითარი #%d", rideId);
	Lang_ShowPlayerDialog(playerid, 9718, DIALOG_STYLE_MSGBOX, title, body, "დახურვა", "");
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
		UI_LvInfo(playerid, "საზოგადოების პროექტები", "{FFFFFF}ამ უბანში ჯერჯერობით ღია პროექტი არ არის.\n\n{AAAAAA}შემდეგი მალე დაიწყება: თვალი ადევნე დაფას.");
		return 0;
	}
	new body[1024], title[576], bar[192], sFund[48], sGoal[48], sLo[48], sHi[48], foot[480], lo = 0, hi = 0;
	new bool:can = UI_LvPrjRange(playerid, lo, hi);
	UI_LvBar(g_UiLvPjFund[playerid], g_UiLvPjGoal[playerid], "66DD66", bar, 192);
	UI_LvFmt(g_UiLvPjFund[playerid], sFund, 48);
	UI_LvFmt(g_UiLvPjGoal[playerid], sGoal, 48);
	if (can)
	{
		UI_LvFmt(lo, sLo, 48);
		UI_LvFmt(hi, sHi, 48);
		format(foot, 480, "{66DD66}დღეს შეგიძლია შეიტანო ₾%s-დან ₾%s-მდე.", sLo, sHi);
	}
	else if (g_UiLvPjBlock[playerid] != 0)
	{
		format(foot, 480, "{FFD24D}%s", g_UiLvPrjBlockText[g_UiLvPjBlock[playerid]]);
	}
	else
	{
		strcopy(foot, "{FFD24D}დღეს მაქსიმუმი უკვე გაიღე: დიდი მადლობა.", 160);
	}
	format(body, 1024, "{FFFFFF}%s\n{AAAAAA}უბანი: %s\n\n%s\n{FFFFFF}შეგროვდა {66DD66}₾%s{FFFFFF} / {FFD24D}₾%s{FFFFFF} - დარჩა %d დღე და %d საათი\n\n{AAAAAA}ფონდი იზრდება ყოველი კონტრაქტით და მგზავრობით უბანში და უბნიდან, შენი ანაზღაურების\nშემცირების გარეშე, და იქ მცხოვრებთა შემოწირულობებით. ყოველი დახმარება მნიშვნელოვანია.\n\n%s", g_UiLvPjName[playerid], g_UiLvDistrictName[g_UiLvPjDistrict[playerid]], bar, sFund, sGoal, g_UiLvPjHours[playerid] / 24, g_UiLvPjHours[playerid] % 24, foot);
	strcopy(title, "საზოგადოების პროექტი", 64);
	if (can)
	{
		Lang_ShowPlayerDialog(playerid, 9719, DIALOG_STYLE_MSGBOX, title, body, "შეწირვა", "დახურვა");
	}
	else
	{
		Lang_ShowPlayerDialog(playerid, 9719, DIALOG_STYLE_MSGBOX, title, body, "დახურვა", "");
	}
	return 0;
}

UI_LvPrjAmountShow(playerid, const err[])
{
	new lo = 0, hi = 0, body[1024], sLo[48], sHi[48];
	if (!UI_LvPrjRange(playerid, lo, hi))
	{
		UI_LivingProjectShow(playerid);
		return 0;
	}
	UI_LvFmt(lo, sLo, 48);
	UI_LvFmt(hi, sHi, 48);
	format(body, 1024, "%s{FFFFFF}რამდენის შეწირვა გინდა {FFD24D}%s{FFFFFF}-ისთვის?\nდასაშვები თანხა: {FFD24D}₾%s{FFFFFF}-დან {FFD24D}₾%s{FFFFFF}-მდე. შეიყვანე თანხა ლარში (მაგ. 25 ან 12,50):", err, g_UiLvPjName[playerid], sLo, sHi);
	Lang_ShowPlayerDialog(playerid, 9720, DIALOG_STYLE_INPUT, "შეწირვა", body, "შემდეგი", "უკან");
	return 0;
}

UI_LvPrjConfirmShow(playerid)
{
	new body[768], sAmt[48];
	UI_LvFmt(g_UiLvPjAmount[playerid], sAmt, 48);
	format(body, 768, "{FFFFFF}სწირავ {66DD66}₾%s{FFFFFF}-ს {FFD24D}%s{FFFFFF}-ისთვის?\n\n{AAAAAA}თანხა ჯერ საბანკო ანგარიშიდან ჩამოიჭრება, შემდეგ ნაღდი ფულიდან, და არ ბრუნდება.\nუბანი ამას არ დაივიწყებს.", sAmt, g_UiLvPjName[playerid]);
	Lang_ShowPlayerDialog(playerid, 9721, DIALOG_STYLE_MSGBOX, "შეწირვის დადასტურება", body, "შეწირვა", "უკან");
}

bool:UI_LvPrjCheck(playerid, amount, err[], size)
{
	new lo = 0, hi = 0, sLo[48], sHi[48];
	err[0] = 0;
	if (!UI_LvPrjRange(playerid, lo, hi))
	{
		return false;
	}
	if (amount < lo || amount > hi)
	{
		UI_LvFmt(lo, sLo, 48);
		UI_LvFmt(hi, sHi, 48);
		format(err, size, "{FF6666}თანხა ლიმიტს სცდება: ₾%s-დან ₾%s-მდე.\n\n", sLo, sHi);
		return false;
	}
	return true;
}

UI_LivingProjectContribute(playerid, amount)
{
	if (!UI_LvReady(playerid))
	{
		return 0;
	}
	new lo = 0, hi = 0, err[480];
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
	if (!UI_LvPrjCheck(playerid, amount, err, 480))
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
	new body[2048], line[384], title[576], cur = g_UiLvHpStep[playerid];
	if (cur < 1)
	{
		cur = 1;
	}
	body[0] = 0;
	for (new s = 1; s <= 5; s++)
	{
		if (s < cur)
		{
			format(line, 384, "{66DD66}[შესრულდა] {AAAAAA}%d. %s\n", s, g_UiLvStepTitle[s]);
		}
		else if (s == cur)
		{
			format(line, 384, "{FFD24D}[ახლა] {FFFFFF}%d. %s\n", s, g_UiLvStepTitle[s]);
		}
		else
		{
			format(line, 384, "{888888}%d. %s\n", s, g_UiLvStepTitle[s]);
		}
		strcat(body, line, 2048);
	}
	g_UiLvHpOffRow[playerid] = -1;
	if (cur <= 5 && !g_UiLvHpOff[playerid])
	{
		strcat(body, "{AAAAAA}მინიშნებების გამორთვა", 2048);
		g_UiLvHpOffRow[playerid] = 5;
	}
	if (cur > 5)
	{
		strcopy(title, "პირველი დღე - დასრულდა", 64);
	}
	else
	{
		format(title, 576, "პირველი დღე - ნაბიჯი %d / %d", cur, 5);
	}
	Lang_ShowPlayerDialog(playerid, 9722, DIALOG_STYLE_LIST, title, body, "გახსნა", "დახურვა");
	return 0;
}

UI_LvHelpStepShow(playerid, step)
{
	new body[1024], title[192], cur = g_UiLvHpStep[playerid];
	g_UiLvHpView[playerid] = step;
	format(body, 1024, "{FFFFFF}%s\n\n%s", g_UiLvStepText[step], step < cur ? "{66DD66}შესრულდა." : (step == cur ? "{FFD24D}ეს შენი შემდეგი ნაბიჯია." : "{AAAAAA}მალე მიხვალ ამ ეტაპამდე."));
	format(title, 192, "%d. %s", step, g_UiLvStepTitle[step]);
	if (step == cur && !g_UiLvHpOff[playerid])
	{
		Lang_ShowPlayerDialog(playerid, 9723, DIALOG_STYLE_MSGBOX, title, body, "რუკაზე მონიშვნა", "უკან");
	}
	else
	{
		Lang_ShowPlayerDialog(playerid, 9723, DIALOG_STYLE_MSGBOX, title, body, "უკან", "დახურვა");
	}
}

UI_LivingHint(playerid, step)
{
	if (!UI_LvReady(playerid) || step < 1 || step > 5 || g_UiLvHpOff[playerid])
	{
		return 0;
	}
	UI_LvTdEnsure(playerid);
	new text[480];
	format(text, 480, "~y~პირველი დღე %d/%d~n~~w~%s", step, 5, g_UiLvStepHint[step]);
	Lang_PlayerTextDrawSetString(playerid, g_UiLvTd[playerid][9], text);
	UI_LvTdShow(playerid, 8, 9, true);
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
		UI_LvTdShow(playerid, 8, 9, false);
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
		UI_LvInfo(playerid, "კეთილი იყოს შენი მობრძანება ლოს-სანტოსში", "{FFFFFF}ქალაქში პირველი დღე გაატარე.\n\nუბნის სახლმა {66DD66}მისასალმებელი ამანათი{FFFFFF} დაგიტოვა: წყალი, მზა კერძი და რამდენიმე ბინტი.\n\n{AAAAAA}აქედან გზას შენ თვითონ ირჩევ. Idlewood გელოდება, როცა მოისურვებ.");
	}
	else
	{
		UI_LvInfo(playerid, "მინიშნებები გამორთულია", "{FFFFFF}კარგი, მინიშნებები აღარ იქნება.\n\n{AAAAAA}თუ დახმარება დაგჭირდება, Idlewood-ის უბნის სახლი ყოველთვის ღიაა.");
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
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiLvTd[playerid][i]);
			g_UiLvTd[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
		}
	}
	g_UiLvTdMade[playerid] = false;
	g_UiLvToastTtl[playerid] = 0;
	g_UiLvHintTtl[playerid] = 0;
	g_UiLvHudOn[playerid] = false;
	g_UiLvHudId[playerid] = 0;
	g_UiLvHudExpAt[playerid] = 0;
	g_UiLvMeterOn[playerid] = false;
	UI_LvConfirmsReset(playerid);
	g_UiLvBdCount[playerid] = 0;
	g_UiLvBdRows[playerid] = 0;
	g_UiLvBdAt[playerid] = false;
	g_UiLvFltDistrict[playerid] = 0;
	g_UiLvFltKind[playerid] = 0;
	g_UiLvFltEligible[playerid] = false;
	g_UiLvDetIdx[playerid] = -1;
	g_UiLvDetCan[playerid] = false;
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
	g_UiLvHpOff[playerid] = false;
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
			PlayerTextDrawHide(playerid, PlayerText:g_UiLvTd[playerid][10]);
		}
	}
	if (g_UiLvHintTtl[playerid] > 0)
	{
		g_UiLvHintTtl[playerid]--;
		if (g_UiLvHintTtl[playerid] <= 0)
		{
			UI_LvTdShow(playerid, 8, 9, false);
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
			UI_LivingToast(playerid, "გამოძახება სხვა ტაქსს გადაეცა", 2);
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
			UI_LivingToast(playerid, "მგზავრობის გადახდის დრო ამოიწურა", 2);
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
		return false;
	}
	switch (dialogid)
	{
		case 9700:
		{
			if (!response)
			{
				return true;
			}
			if (listitem == 0)
			{
				UI_LvFilterShow(playerid);
				return true;
			}
			new row = listitem - 1, rows = g_UiLvBdRows[playerid];
			if (row >= 0 && row < rows)
			{
				UI_LvDetailShow(playerid, g_UiLvBdRow[playerid][row]);
				return true;
			}
			if (row == (rows == 0 ? 1 : rows))
			{
				UI_LivingToast(playerid, "დაფა ახლდება...", 0);
				UI_OnContractBoardRefresh(playerid);
				return true;
			}
			UI_LvBoardRender(playerid);
			return true;
		}
		case 9701:
		{
			if (!response)
			{
				UI_LvBoardRender(playerid);
				return true;
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
					return true;
				}
			}
			UI_LvFilterShow(playerid);
			return true;
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
			return true;
		}
		case 9703:
		{
			new i = g_UiLvDetIdx[playerid];
			if (i < 0 || i >= g_UiLvBdCount[playerid])
			{
				UI_LvBoardRender(playerid);
				return true;
			}
			if (!response)
			{
				UI_LvDetailShow(playerid, i);
				return true;
			}
			g_UiLvDetCan[playerid] = false;
			UI_LivingToast(playerid, "კონტრაქტი მოთხოვნილია, ერთი წამით...", 0);
			UI_OnContractAccept(playerid, g_UiLvBdId[playerid][i]);
			return true;
		}
		case 9704:
		{
			if (!response)
			{
				return true;
			}
			if (g_UiLvStId[playerid] <= 0)
			{
				UI_OnContractBoardRefresh(playerid);
				return true;
			}
			new body[768];
			format(body, 768, "{FFFFFF}ნამდვილად გინდა კონტრაქტის {FFD24D}#%d{FFFFFF} დატოვება?\n\n{AAAAAA}მანქანა საწყობში დაბრუნდება, კონტრაქტი კი დაფაზე სხვისთვის გამოჩნდება.", g_UiLvStId[playerid]);
			Lang_ShowPlayerDialog(playerid, 9705, DIALOG_STYLE_MSGBOX, "დატოვებ კონტრაქტს?", body, "დატოვება", "უკან");
			return true;
		}
		case 9705:
		{
			new id = g_UiLvStId[playerid];
			if (id <= 0)
			{
				return true;
			}
			if (!response)
			{
				UI_LivingToast(playerid, "კონტრაქტი შენთან რჩება", 1);
				return true;
			}
			g_UiLvStId[playerid] = 0;
			UI_OnContractAbandon(playerid, id);
			return true;
		}
		case 9706:
		{
			if (!response)
			{
				return true;
			}
			if (listitem >= 0 && listitem < 3)
			{
				UI_OnDistrictPick(playerid, listitem + 1);
			}
			return true;
		}
		case 9707:
		{
			if (response && g_UiLvDsId[playerid] != 0)
			{
				UI_OnProjectOpen(playerid, g_UiLvDsId[playerid]);
			}
			return true;
		}
		case 9708, 9713, 9718, 9725:
		{
			return true;
		}
		case 9709:
		{
			if (!response)
			{
				return true;
			}
			if (listitem < 0 || listitem >= g_UiLvRnCount[playerid])
			{
				UI_LvRentModelShow(playerid);
				return true;
			}
			if (!g_UiLvRnAvail[playerid][listitem])
			{
				UI_LivingToast(playerid, "ეს მანქანა ახლა შენთვის მიუწვდომელია", 2);
				UI_LvRentModelShow(playerid);
				return true;
			}
			g_UiLvRnSel[playerid] = listitem;
			UI_LvRentBlocksShow(playerid);
			return true;
		}
		case 9710:
		{
			new s = g_UiLvRnSel[playerid];
			if (!response || s < 0 || s >= g_UiLvRnCount[playerid])
			{
				UI_LvRentModelShow(playerid);
				return true;
			}
			if (listitem < 0 || listitem >= g_UiLvRnMaxBlocks[playerid])
			{
				UI_LvRentBlocksShow(playerid);
				return true;
			}
			g_UiLvRnBlocks[playerid] = listitem + 1;
			UI_LvRentSummaryShow(playerid);
			return true;
		}
		case 9711:
		{
			new s = g_UiLvRnSel[playerid], b = g_UiLvRnBlocks[playerid];
			if (s < 0 || s >= g_UiLvRnCount[playerid] || b < 1)
			{
				UI_LvRentModelShow(playerid);
				return true;
			}
			if (!response)
			{
				UI_LvRentBlocksShow(playerid);
				return true;
			}
			g_UiLvRnSel[playerid] = -1;
			g_UiLvRnBlocks[playerid] = 0;
			UI_LivingToast(playerid, "მანქანას ვამზადებთ...", 0);
			UI_OnRentalConfirm(playerid, g_UiLvRnIdx[playerid][s], b);
			return true;
		}
		case 9712:
		{
			new id = g_UiLvRnReturnId[playerid];
			g_UiLvRnReturnId[playerid] = 0;
			if (id <= 0 || !response)
			{
				return true;
			}
			UI_OnRentalReturnConfirm(playerid, id);
			return true;
		}
		case 9714:
		{
			new id = g_UiLvTxOfferId[playerid];
			g_UiLvTxOfferId[playerid] = 0;
			g_UiLvTxOfferLeft[playerid] = 0;
			UI_LvTimerSync(playerid);
			if (id <= 0 || !response)
			{
				return true;
			}
			UI_OnTaxiCallAccept(playerid, id);
			return true;
		}
		case 9715:
		{
			new id = g_UiLvTxPayId[playerid];
			if (id <= 0)
			{
				return true;
			}
			if (response)
			{
				UI_LvTaxiPayReset(playerid);
				UI_LvTimerSync(playerid);
				UI_OnTaxiPayConfirm(playerid, id, 1);
				return true;
			}
			Lang_ShowPlayerDialog(playerid, 9716, DIALOG_STYLE_MSGBOX, "გაასაჩივრებ მგზავრობას?", "{FFFFFF}თუ გაასაჩივრებ, მგზავრობა არ გადაიხდება და ადმინისტრაციის განხილვამდე შეჩერდება.\n\n{AAAAAA}გამოიყენე მხოლოდ მაშინ, თუ რამე მართლა არასწორად წავიდა.", "გასაჩივრება", "უკან");
			return true;
		}
		case 9716:
		{
			new id = g_UiLvTxPayId[playerid];
			if (id <= 0)
			{
				return true;
			}
			if (!response)
			{
				UI_LvTaxiPayShow(playerid);
				return true;
			}
			UI_LvTaxiPayReset(playerid);
			UI_LvTimerSync(playerid);
			UI_OnTaxiPayConfirm(playerid, id, 0);
			return true;
		}
		case 9717:
		{
			new id = g_UiLvTxRateId[playerid];
			g_UiLvTxRateId[playerid] = 0;
			if (id <= 0 || !response || listitem < 0 || listitem >= 5)
			{
				return true;
			}
			UI_LivingToast(playerid, "მადლობა შეფასებისთვის", 1);
			UI_OnTaxiRate(playerid, id, 5 - listitem);
			return true;
		}
		case 9719:
		{
			new lo = 0, hi = 0;
			if (response && UI_LvPrjRange(playerid, lo, hi))
			{
				UI_LvPrjAmountShow(playerid, "");
			}
			return true;
		}
		case 9720:
		{
			if (!response)
			{
				UI_LivingProjectShow(playerid);
				return true;
			}
			new amount = 0, err[480];
			if (!UI_LvParseAmount(inputtext, amount))
			{
				UI_LvPrjAmountShow(playerid, "{FF6666}არასწორი თანხა: ჩაწერე ლარებში, მაგ. 25 ან 12,50.\n\n");
				return true;
			}
			if (!UI_LvPrjCheck(playerid, amount, err, 480))
			{
				if (err[0] != 0)
				{
					UI_LvPrjAmountShow(playerid, err);
				}
				else
				{
					UI_LivingProjectShow(playerid);
				}
				return true;
			}
			g_UiLvPjAmount[playerid] = amount;
			UI_LvPrjConfirmShow(playerid);
			return true;
		}
		case 9721:
		{
			new amount = g_UiLvPjAmount[playerid], err[480];
			if (!response)
			{
				UI_LvPrjAmountShow(playerid, "");
				return true;
			}
			if (amount <= 0 || !UI_LvPrjCheck(playerid, amount, err, 480))
			{
				UI_LivingProjectShow(playerid);
				return true;
			}
			g_UiLvPjAmount[playerid] = 0;
			UI_LivingToast(playerid, "მადლობა: შენი შემოწირულობა ფიქსირდება...", 1);
			UI_OnProjectContribute(playerid, g_UiLvPjId[playerid], amount);
			return true;
		}
		case 9722:
		{
			if (!response)
			{
				return true;
			}
			if (listitem >= 0 && listitem < 5)
			{
				UI_LvHelpStepShow(playerid, listitem + 1);
				return true;
			}
			if (listitem >= 0 && listitem == g_UiLvHpOffRow[playerid])
			{
				Lang_ShowPlayerDialog(playerid, 9724, DIALOG_STYLE_MSGBOX, "გამოვრთოთ მინიშნებები?", "{FFFFFF}პირველი დღის გზა აქ სრულდება, მისასალმებელი ამანათის გარეშე.\n\n{AAAAAA}ყოველთვის შეგიძლია ჰკითხო, ვისაც შეხვდები: ლოს-სანტოსი იმაზე კეთილია, ვიდრე ჩანს.", "გამორთვა", "უკან");
				return true;
			}
			UI_LivingHelpShow(playerid);
			return true;
		}
		case 9723:
		{
			new step = g_UiLvHpView[playerid];
			new bool:mapBtn = step == g_UiLvHpStep[playerid] && !g_UiLvHpOff[playerid];
			if (mapBtn)
			{
				if (response && step >= 1 && step <= 5)
				{
					UI_LivingToast(playerid, "გზას რუკაზე გინიშნავ", 1);
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
			return true;
		}
		case 9724:
		{
			if (!response)
			{
				UI_LivingHelpShow(playerid);
				return true;
			}
			g_UiLvHpOff[playerid] = true;
			UI_LivingHintHide(playerid);
			UI_OnHelpHintsOff(playerid);
			return true;
		}
	}
	return true;
}
