new g_UiJbName[4][48] = {
	"უმუშევარი",
	"ავტობუსის მძღოლი",
	"კურიერი",
	"დამლაგებელი"
};
new g_UiJbVehicle[4][16] = {
	"-",
	"Bus",
	"Boxville",
	"Trashmaster"
};
new g_UiJbStepWord[4][32] = {
	"გაჩერება",
	"გაჩერება",
	"მიწოდება",
	"ნაგვის ურნა"
};
new g_UiJbMinLevel[4] = {0, 1, 2, 1};
new g_UiJbSteps[4] = {0, 6, 5, 8};
new g_UiJbPayStep[4] = {0, ECON_JOB_BUS_STEP, ECON_JOB_COURIER_STEP, ECON_JOB_CLEANER_STEP};   // tetri, economy/econ_config.inc
new g_UiJbPayBonus[4] = {0, ECON_JOB_BUS_BONUS, ECON_JOB_COURIER_BONUS, ECON_JOB_CLEANER_BONUS};
new g_UiJbDwellMs[4] = {0, 4000, 5000, 4000};
new g_UiJbEstMinutes[4] = {1, 7, 9, 8};
new g_UiJbRoute[4][176] = {
	"-",
	"6 ფიქსირებული გაჩერება თანმიმდევრობით, შემდეგ დეპოში დაბრუნება",
	"5 მიწოდება 8 შემთხვევითი წერტილიდან, შემდეგ დეპოში დაბრუნება",
	"8 ურნა ფიქსირებული თანმიმდევრობით, შემდეგ დეპოში დაცლა"
};
new g_UiJbLevel[MAX_PLAYERS];
new g_UiJbCurrent[MAX_PLAYERS];
new g_UiJbCooldown[MAX_PLAYERS];
new g_UiJbSel[MAX_PLAYERS];

UI_JbMoney(value, out[], size)
{
	// importo in tetri -> "1.250,50" (economy/currency.inc)
	return Money_Format(value, out, size);
}

UI_JbValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

UI_JbRunPay(job)
{
	if (job < 1 || job > 3)
	{
		return 0;
	}
	return g_UiJbSteps[job] * g_UiJbPayStep[job] + g_UiJbPayBonus[job];
}

UI_JbHourly(job)
{
	if (job < 1 || job > 3)
	{
		return 0;
	}
	new hourly = UI_JbRunPay(job) * 60 / g_UiJbEstMinutes[job];
	if (hourly > ECON_JOB_HOURLY_CAP)
	{
		hourly = ECON_JOB_HOURLY_CAP;
	}
	return hourly;
}

UI_JobCenterShow(playerid, level, currentJob, cooldownSec)
{
	if (!UI_JbValid(playerid))
	{
		return 0;
	}
	if (currentJob < 0 || currentJob > 3)
	{
		currentJob = 0;
	}
	g_UiJbLevel[playerid] = level;
	g_UiJbCurrent[playerid] = currentJob;
	g_UiJbCooldown[playerid] = cooldownSec > 0 ? cooldownSec : 0;
	new body[2048], line[576], pay[48];
	strcat(body, "სამსახური\tმინ. დონე\tსავარ. ხელფასი/სთ\tმანქანა\n", 2048);
	for (new job = 1; job <= 3; job++)
	{
		UI_JbMoney(UI_JbHourly(job), pay, 48);
		if (level < g_UiJbMinLevel[job])
		{
			format(line, 576, "{888888}%s\t{FF6666}დონე %d (დაბლოკილი)\t{888888}~₾%s\t{888888}%s\n", g_UiJbName[job], g_UiJbMinLevel[job], pay, g_UiJbVehicle[job]);
		}
		else if (job == currentJob)
		{
			format(line, 576, "{FFD24D}%s (მიმდინარე)\t{FFFFFF}დონე %d\t{66DD66}~₾%s\t{FFFFFF}%s\n", g_UiJbName[job], g_UiJbMinLevel[job], pay, g_UiJbVehicle[job]);
		}
		else
		{
			format(line, 576, "{FFFFFF}%s\t{FFFFFF}დონე %d\t{66DD66}~₾%s\t{FFFFFF}%s\n", g_UiJbName[job], g_UiJbMinLevel[job], pay, g_UiJbVehicle[job]);
		}
		strcat(body, line, 2048);
	}
	if (currentJob != 0)
	{
		strcat(body, "{FF6666}მიმდინარე სამსახურის დატოვება\t\t\t\n", 2048);
	}
	Lang_ShowPlayerDialog(playerid, 9300, DIALOG_STYLE_TABLIST_HEADERS, "დასაქმების ცენტრი", body, "დეტალები", "დახურვა");
	return 0;
}

UI_JbDetailShow(playerid, job)
{
	if (job < 1 || job > 3)
	{
		return 0;
	}
	g_UiJbSel[playerid] = job;
	new body[2048], line[576], title[192];
	new runPay[48], hourly[48], cap[48], step[48], bonus[48];
	UI_JbMoney(UI_JbRunPay(job), runPay, 48);
	UI_JbMoney(UI_JbHourly(job), hourly, 48);
	UI_JbMoney(ECON_JOB_HOURLY_CAP, cap, 48);
	UI_JbMoney(g_UiJbPayStep[job], step, 48);
	UI_JbMoney(g_UiJbPayBonus[job], bonus, 48);
	format(line, 576, "{FFD24D}%s{FFFFFF}\n\n", g_UiJbName[job]);
	strcat(body, line, 2048);
	format(line, 576, "სამსახურებრივი მანქანა: {FFFFFF}%s{FFFFFF}\nმარშრუტი: %s\n", g_UiJbVehicle[job], g_UiJbRoute[job]);
	strcat(body, line, 2048);
	format(line, 576, "მინიმალური დონე: %d\nსაჭირო გაჩერება: %d წმ ყოველ %s-ზე\n\n", g_UiJbMinLevel[job], g_UiJbDwellMs[job] / 1000, g_UiJbStepWord[job]);
	strcat(body, line, 2048);
	format(line, 576, "ხელფასი: {66DD66}₾%s{FFFFFF} ყოველ %s-ზე + {66DD66}₾%s{FFFFFF} ბონუსი წრის ბოლოს = {66DD66}₾%s{FFFFFF} წრეზე\n", step, g_UiJbStepWord[job], bonus, runPay);
	strcat(body, line, 2048);
	format(line, 576, "სავარაუდოდ: დაახლ. {66DD66}₾%s{FFFFFF}/სთ (ლიმიტი ₾%s/სთ). ხელფასი ბანკში ჩაირიცხება ცვლის ბოლოს.\n\n", hourly, cap);
	strcat(body, line, 2048);
	strcat(body, "ცვლა იწყება /inizia-თი და მთავრდება /fine-ით. მანქანის განადგურება ან მიტოვება, სიკვდილი ან\nგათიშვა ცვლას აუქმებს და დაგროვილი შემოსავალი იკარგება.", 2048);
	new bool:ok = true;
	if (g_UiJbLevel[playerid] < g_UiJbMinLevel[job])
	{
		format(line, 576, "\n\n{FF6666}საჭიროა დონე %d (შენ ხარ დონე %d).", g_UiJbMinLevel[job], g_UiJbLevel[playerid]);
		strcat(body, line, 2048);
		ok = false;
	}
	else if (job == g_UiJbCurrent[playerid])
	{
		strcat(body, "\n\n{FFD24D}ეს უკვე შენი მიმდინარე სამსახურია.", 2048);
		ok = false;
	}
	else if (g_UiJbCooldown[playerid] > 0)
	{
		format(line, 576, "\n\n{FF6666}სამსახურს შეცვლი %d წთ %02d წმ-ში.", g_UiJbCooldown[playerid] / 60, g_UiJbCooldown[playerid] % 60);
		strcat(body, line, 2048);
		ok = false;
	}
	else
	{
		format(line, 576, "\n\n{FFD24D}არჩევის შემდეგ სამსახურის შესაცვლელად %d წუთი მოგიწევს ლოდინი.", 10);
		strcat(body, line, 2048);
	}
	format(title, 192, "დასაქმების ცენტრი - %s", g_UiJbName[job]);
	Lang_ShowPlayerDialog(playerid, 9301, DIALOG_STYLE_MSGBOX, title, body, ok ? "აყვანა" : "უკან", ok ? "უკან" : "");
	return 0;
}

UI_JbQuitShow(playerid)
{
	new body[768], cur = g_UiJbCurrent[playerid];
	if (cur < 1 || cur > 3)
	{
		return 0;
	}
	format(body, 768, "{FFFFFF}ნამდვილად გინდა {FFD24D}%s{FFFFFF}-ის სამსახურის დატოვება?\n\nსხვა სამსახურის არჩევისას %d წუთი მოგიწევს ლოდინი.", g_UiJbName[cur], 10);
	Lang_ShowPlayerDialog(playerid, 9302, DIALOG_STYLE_MSGBOX, "სამსახურის დატოვება", body, "დატოვება", "გაუქმება");
	return 0;
}

UI_JobInfoShow(playerid, jobId, level, runs, earned)
{
	if (!UI_JbValid(playerid))
	{
		return 0;
	}
	if (jobId < 0 || jobId > 3)
	{
		jobId = 0;
	}
	new body[2048], line[576], money[48], hourly[48];
	if (jobId == 0)
	{
		strcat(body, "{FFFFFF}ამჟამად {FFD24D}უმუშევარი{FFFFFF} ხარ.\nმიდი Job Center-ში (City Hall-ის მოედანი) და გამოიყენე /lavoro prendi.\n\nხელმისაწვდომი სამსახურები:\n", 2048);
		for (new job = 1; job <= 3; job++)
		{
			UI_JbMoney(UI_JbHourly(job), hourly, 48);
			format(line, 576, "{FFFFFF}- %s: დონე %d, ~{66DD66}₾%s{FFFFFF}/სთ%s\n", g_UiJbName[job], g_UiJbMinLevel[job], hourly, level < g_UiJbMinLevel[job] ? " {FF6666}(დაბლოკილი){FFFFFF}" : "");
			strcat(body, line, 2048);
		}
	}
	else
	{
		UI_JbMoney(UI_JbRunPay(jobId), money, 48);
		UI_JbMoney(UI_JbHourly(jobId), hourly, 48);
		format(line, 576, "{FFFFFF}მიმდინარე სამსახური: {FFD24D}%s{FFFFFF}\nმანქანა: %s\nმარშრუტი: %s\n", g_UiJbName[jobId], g_UiJbVehicle[jobId], g_UiJbRoute[jobId]);
		strcat(body, line, 2048);
		format(line, 576, "ხელფასი წრეზე: {66DD66}₾%s{FFFFFF}  |  სავარაუდოდ ~{66DD66}₾%s{FFFFFF}/სთ (ლიმიტი %s/სთ)\n\n", money, hourly, Money_Str(ECON_JOB_HOURLY_CAP));
		strcat(body, line, 2048);
		UI_JbMoney(earned, money, 48);
		format(line, 576, "დასრულებული ცვლები: %d\nსულ გამომუშავებული: {66DD66}₾%s{FFFFFF}\n\n", runs, money);
		strcat(body, line, 2048);
		strcat(body, "ბრძანებები: /inizia (ცვლის დაწყება), /fine (დასრულება და დაგროვილის გადახდა), /lavoro lascia (Job Center-ში).", 2048);
	}
	Lang_ShowPlayerDialog(playerid, 9303, DIALOG_STYLE_MSGBOX, "შენი სამსახური", body, "დახურვა", "");
	return 0;
}

bool:UI_JobOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	#pragma unused inputtext
	if (!UI_JbValid(playerid))
	{
		return false;
	}
	switch (dialogid)
	{
		case 9300:
		{
			if (!response)
			{
				return true;
			}
			if (listitem >= 0 && listitem < 3)
			{
				UI_JbDetailShow(playerid, listitem + 1);
			}
			else if (listitem == 3 && g_UiJbCurrent[playerid] != 0)
			{
				UI_JbQuitShow(playerid);
			}
			else
			{
				UI_JobCenterShow(playerid, g_UiJbLevel[playerid], g_UiJbCurrent[playerid], g_UiJbCooldown[playerid]);
			}
			return true;
		}
		case 9301:
		{
			new job = g_UiJbSel[playerid];
			new bool:ok = job >= 1 && job <= 3 && g_UiJbLevel[playerid] >= g_UiJbMinLevel[job] && job != g_UiJbCurrent[playerid] && g_UiJbCooldown[playerid] <= 0;
			if (response && ok)
			{
				UI_OnJobSelect(playerid, job);
				return true;
			}
			UI_JobCenterShow(playerid, g_UiJbLevel[playerid], g_UiJbCurrent[playerid], g_UiJbCooldown[playerid]);
			return true;
		}
		case 9302:
		{
			if (response && g_UiJbCurrent[playerid] != 0)
			{
				UI_OnJobQuit(playerid);
				return true;
			}
			UI_JobCenterShow(playerid, g_UiJbLevel[playerid], g_UiJbCurrent[playerid], g_UiJbCooldown[playerid]);
			return true;
		}
		case 9303:
		{
			return true;
		}
	}
	return false;
}
new PlayerText:g_UiJHud[MAX_PLAYERS][6];
new bool:g_UiJHudCreated[MAX_PLAYERS];
new bool:g_UiJHudShown[MAX_PLAYERS];
new bool:g_UiJHudWarn[MAX_PLAYERS];
new g_UiJHudJob[MAX_PLAYERS];

PlayerText:UI_JhText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
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

PlayerText:UI_JhBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
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

UI_JhCreate(playerid)
{
	if (g_UiJHudCreated[playerid])
	{
		return 0;
	}
	new Float:left = (476.0 - 150.0);
	g_UiJHud[playerid][0] = UI_JhBox(playerid, left, 384.0, 150.0, 66.0, 153);
	g_UiJHud[playerid][1] = UI_JhText(playerid, (5.0 + left), (384.0 + 2.0), "სამსახური", 0.22, 1.0, -2994689, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][2] = UI_JhText(playerid, (5.0 + left), (384.0 + 14.0), "გაჩერება 0/0", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][3] = UI_JhText(playerid, (5.0 + left), (384.0 + 26.0), "დაგროვდა: ₾0", 0.22, 1.0, 0x66DD66FF, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][4] = UI_JhText(playerid, (5.0 + left), (384.0 + 38.0), "დრო 00:00", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][5] = UI_JhText(playerid, (5.0 + left), (384.0 + 51.0), "დაბრუნდი მანქანაში: 45s", 0.2, 0.9, -12303105, t_TEXT_DRAW_ALIGN:1);
	g_UiJHudCreated[playerid] = true;
	return 0;
}

UI_JhSetVisible(playerid, bool:visible)
{
	for (new i = 0; i < 6; i++)
	{
		if (i == 5)
		{
			continue;
		}
		if (visible)
		{
			PlayerTextDrawShow(playerid, PlayerText:g_UiJHud[playerid][i]);
		}
		else
		{
			PlayerTextDrawHide(playerid, PlayerText:g_UiJHud[playerid][i]);
		}
	}
	if (!visible || !g_UiJHudWarn[playerid])
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiJHud[playerid][5]);
		g_UiJHudWarn[playerid] = false;
	}
}

UI_JobHudShow(playerid, jobId)
{
	if (!UI_JbValid(playerid))
	{
		return 0;
	}
	if (jobId < 1 || jobId > 3)
	{
		return 0;
	}
	UI_JhCreate(playerid);
	g_UiJHudJob[playerid] = jobId;
	g_UiJHudShown[playerid] = true;
	g_UiJHudWarn[playerid] = false;
	new str[432];
	format(str, 432, "სამსახური: %s", g_UiJbName[jobId]);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][1], str);
	format(str, 432, "%s 0/%d", g_UiJbStepWord[jobId], g_UiJbSteps[jobId]);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][2], str);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][3], "დაგროვდა: ₾0");
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][4], "დრო 00:00");
	UI_JhSetVisible(playerid, true);
	return 0;
}

UI_JobHudHide(playerid)
{
	if (!UI_JbValid(playerid) || !g_UiJHudCreated[playerid])
	{
		return 0;
	}
	g_UiJHudShown[playerid] = false;
	UI_JhSetVisible(playerid, false);
	return 0;
}

UI_JobHudUpdate(playerid, step, stepTotal, earned, elapsedSec, dwellSec, offVehicleSec)
{
	if (!UI_JbValid(playerid) || !g_UiJHudCreated[playerid] || !g_UiJHudShown[playerid])
	{
		return 0;
	}
	new job = g_UiJHudJob[playerid];
	new str[576], money[48];
	if (stepTotal < 0)
	{
		stepTotal = 0;
	}
	if (step < 0)
	{
		step = 0;
	}
	if (elapsedSec < 0)
	{
		elapsedSec = 0;
	}
	if (step > stepTotal)
	{
		format(str, 576, "დეპოში დაბრუნება");
	}
	else
	{
		format(str, 576, "%s %d/%d", g_UiJbStepWord[job], step, stepTotal);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][2], str);
	UI_JbMoney(earned, money, 48);
	format(str, 576, "დაგროვდა: ₾%s", money);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][3], str);
	if (dwellSec >= 0)
	{
		format(str, 576, "დრო %02d:%02d  გაჩერება %ds", elapsedSec / 60, elapsedSec % 60, dwellSec);
	}
	else
	{
		format(str, 576, "დრო %02d:%02d", elapsedSec / 60, elapsedSec % 60);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][4], str);
	if (offVehicleSec >= 0)
	{
		format(str, 576, "დაბრუნდი მანქანაში: %ds", offVehicleSec);
		Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][5], str);
		new bool:blinkOff = GetTickCount() / 500 % 2 == 1;
		PlayerTextDrawColour(playerid, PlayerText:g_UiJHud[playerid][5], blinkOff ? -12303292 : -12303105);
		PlayerTextDrawShow(playerid, PlayerText:g_UiJHud[playerid][5]);
		g_UiJHudWarn[playerid] = true;
	}
	else if (g_UiJHudWarn[playerid])
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiJHud[playerid][5]);
		g_UiJHudWarn[playerid] = false;
	}
	PlayerTextDrawShow(playerid, PlayerText:g_UiJHud[playerid][2]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiJHud[playerid][3]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiJHud[playerid][4]);
	return 0;
}

UI_JobUiDestroy(playerid)
{
	if (!UI_JbValid(playerid))
	{
		return 0;
	}
	if (g_UiJHudCreated[playerid])
	{
		for (new i = 0; i < 6; i++)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiJHud[playerid][i]);
			g_UiJHud[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
		}
	}
	g_UiJHudCreated[playerid] = false;
	g_UiJHudShown[playerid] = false;
	g_UiJHudWarn[playerid] = false;
	g_UiJHudJob[playerid] = 0;
	g_UiJbLevel[playerid] = 0;
	g_UiJbCurrent[playerid] = 0;
	g_UiJbCooldown[playerid] = 0;
	g_UiJbSel[playerid] = 0;
	return 0;
}
