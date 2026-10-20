new g_UiJbName[4][16] = {
	"Disoccupato",
	"Autista Bus",
	"Corriere",
	"Netturbino"
};
new g_UiJbVehicle[4][16] = {
	"-",
	"Bus",
	"Boxville",
	"Trashmaster"
};
new g_UiJbStepWord[4][12] = {
	"Tappa",
	"Fermata",
	"Consegna",
	"Cassonetto"
};
new g_UiJbMinLevel[4] = {0, 1, 2, 1};
new g_UiJbSteps[4] = {0, 6, 5, 8};
new g_UiJbPayStep[4] = {0, 28, 45, 24};
new g_UiJbPayBonus[4] = {0, 40, 30, 40};
new g_UiJbDwellMs[4] = {0, 4000, 5000, 4000};
new g_UiJbEstMinutes[4] = {1, 7, 9, 8};
new g_UiJbRoute[4][64] = {
	"-",
	"6 fermate fisse in ordine, poi rientro al deposito",
	"5 consegne a caso tra 8 punti, poi rientro al deposito",
	"8 cassonetti in ordine fisso, poi scarico al deposito"
};
new g_UiJbLevel[MAX_PLAYERS];
new g_UiJbCurrent[MAX_PLAYERS];
new g_UiJbCooldown[MAX_PLAYERS];
new g_UiJbSel[MAX_PLAYERS];

UI_JbMoney(value, out[], size)
{
	new raw[16], len = 0, pos = 0;
	if (value < 0)
	{
		value = 0;
	}
	format(raw, 16, "%d", value);
	len = strlen(raw);
	out[0] = 0;
	for (new i = 0; i < len && pos < size - 2; i++)
	{
		if (i > 0 && (len - i) % 3 == 0)
		{
			out[pos++] = 46;
		}
		out[pos++] = raw[i];
	}
	out[pos] = 0;
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
	if (hourly > 2200)
	{
		hourly = 2200;
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
	new body[1024], line[192], pay[16];
	strcat(body, "Lavoro\tLiv. min\tPaga/ora stimata\tVeicolo\n", 1024);
	for (new job = 1; job <= 3; job++)
	{
		UI_JbMoney(UI_JbHourly(job), pay, 16);
		if (level < g_UiJbMinLevel[job])
		{
			format(line, 192, "{888888}%s\t{FF6666}Liv. %d (bloccato)\t{888888}~$%s\t{888888}%s\n", g_UiJbName[job], g_UiJbMinLevel[job], pay, g_UiJbVehicle[job]);
		}
		else if (job == currentJob)
		{
			format(line, 192, "{FFD24D}%s (attuale)\t{FFFFFF}Liv. %d\t{66DD66}~$%s\t{FFFFFF}%s\n", g_UiJbName[job], g_UiJbMinLevel[job], pay, g_UiJbVehicle[job]);
		}
		else
		{
			format(line, 192, "{FFFFFF}%s\t{FFFFFF}Liv. %d\t{66DD66}~$%s\t{FFFFFF}%s\n", g_UiJbName[job], g_UiJbMinLevel[job], pay, g_UiJbVehicle[job]);
		}
		strcat(body, line, 1024);
	}
	if (currentJob != 0)
	{
		strcat(body, "{FF6666}Lascia il lavoro attuale\t\t\t\n", 1024);
	}
	Lang_ShowPlayerDialog(playerid, 9300, DIALOG_STYLE_TABLIST_HEADERS, "Job Center", body, "Dettagli", "Chiudi");
	return 0;
}

UI_JbDetailShow(playerid, job)
{
	if (job < 1 || job > 3)
	{
		return 0;
	}
	g_UiJbSel[playerid] = job;
	new body[1024], line[192], title[64];
	new runPay[16], hourly[16], cap[16], step[16], bonus[16];
	UI_JbMoney(UI_JbRunPay(job), runPay, 16);
	UI_JbMoney(UI_JbHourly(job), hourly, 16);
	UI_JbMoney(2200, cap, 16);
	UI_JbMoney(g_UiJbPayStep[job], step, 16);
	UI_JbMoney(g_UiJbPayBonus[job], bonus, 16);
	format(line, 192, "{FFD24D}%s{FFFFFF}\n\n", g_UiJbName[job]);
	strcat(body, line, 1024);
	format(line, 192, "Veicolo di servizio: {FFFFFF}%s{FFFFFF}\nPercorso: %s\n", g_UiJbVehicle[job], g_UiJbRoute[job]);
	strcat(body, line, 1024);
	format(line, 192, "Livello minimo: %d\nSosta richiesta: %d s a ogni %s\n\n", g_UiJbMinLevel[job], g_UiJbDwellMs[job] / 1000, g_UiJbStepWord[job]);
	strcat(body, line, 1024);
	format(line, 192, "Paga: {66DD66}$%s{FFFFFF} a %s + {66DD66}$%s{FFFFFF} di bonus a fine giro = {66DD66}$%s{FFFFFF} a giro\n", step, g_UiJbStepWord[job], bonus, runPay);
	strcat(body, line, 1024);
	format(line, 192, "Stima: circa {66DD66}$%s{FFFFFF}/ora (tetto $%s/ora). La paga e' accreditata in banca a fine turno.\n\n", hourly, cap);
	strcat(body, line, 1024);
	strcat(body, "Il turno inizia con /inizia e termina con /fine. Veicolo distrutto o abbandonato, morte o\ndisconnessione annullano il turno e il guadagno maturato va perso.", 1024);
	new bool:ok = true;
	if (g_UiJbLevel[playerid] < g_UiJbMinLevel[job])
	{
		format(line, 192, "\n\n{FF6666}Richiede il livello %d (tu sei livello %d).", g_UiJbMinLevel[job], g_UiJbLevel[playerid]);
		strcat(body, line, 1024);
		ok = false;
	}
	else if (job == g_UiJbCurrent[playerid])
	{
		strcat(body, "\n\n{FFD24D}Questo e' gia' il tuo lavoro attuale.", 1024);
		ok = false;
	}
	else if (g_UiJbCooldown[playerid] > 0)
	{
		format(line, 192, "\n\n{FF6666}Potrai cambiare lavoro tra %d min %02d s.", g_UiJbCooldown[playerid] / 60, g_UiJbCooldown[playerid] % 60);
		strcat(body, line, 1024);
		ok = false;
	}
	else
	{
		format(line, 192, "\n\n{FFD24D}Dopo la scelta dovrai attendere %d minuti prima di cambiare lavoro.", 10);
		strcat(body, line, 1024);
	}
	format(title, 64, "Job Center - %s", g_UiJbName[job]);
	Lang_ShowPlayerDialog(playerid, 9301, DIALOG_STYLE_MSGBOX, title, body, ok ? "Prendi lavoro" : "Indietro", ok ? "Indietro" : "");
	return 0;
}

UI_JbQuitShow(playerid)
{
	new body[256], cur = g_UiJbCurrent[playerid];
	if (cur < 1 || cur > 3)
	{
		return 0;
	}
	format(body, 256, "{FFFFFF}Vuoi davvero lasciare il lavoro di {FFD24D}%s{FFFFFF}?\n\nSe poi scegli un altro lavoro dovrai rispettare il cooldown di %d minuti.", g_UiJbName[cur], 10);
	Lang_ShowPlayerDialog(playerid, 9302, DIALOG_STYLE_MSGBOX, "Lascia il lavoro", body, "Lascia", "Annulla");
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
	new body[1024], line[192], money[16], hourly[16];
	if (jobId == 0)
	{
		strcat(body, "{FFFFFF}Al momento sei {FFD24D}disoccupato{FFFFFF}.\nRecati al Job Center (piazza City Hall) e usa /lavoro prendi.\n\nLavori disponibili:\n", 1024);
		for (new job = 1; job <= 3; job++)
		{
			UI_JbMoney(UI_JbHourly(job), hourly, 16);
			format(line, 192, "{FFFFFF}- %s: livello %d, ~{66DD66}$%s{FFFFFF}/ora%s\n", g_UiJbName[job], g_UiJbMinLevel[job], hourly, level < g_UiJbMinLevel[job] ? " {FF6666}(bloccato){FFFFFF}" : "");
			strcat(body, line, 1024);
		}
	}
	else
	{
		UI_JbMoney(UI_JbRunPay(jobId), money, 16);
		UI_JbMoney(UI_JbHourly(jobId), hourly, 16);
		format(line, 192, "{FFFFFF}Lavoro attuale: {FFD24D}%s{FFFFFF}\nVeicolo: %s\nPercorso: %s\n", g_UiJbName[jobId], g_UiJbVehicle[jobId], g_UiJbRoute[jobId]);
		strcat(body, line, 1024);
		format(line, 192, "Paga a giro: {66DD66}$%s{FFFFFF}  |  stima ~{66DD66}$%s{FFFFFF}/ora (tetto $%d/ora)\n\n", money, hourly, 2200);
		strcat(body, line, 1024);
		UI_JbMoney(earned, money, 16);
		format(line, 192, "Turni completati: %d\nTotale guadagnato: {66DD66}$%s{FFFFFF}\n\n", runs, money);
		strcat(body, line, 1024);
		strcat(body, "Comandi: /inizia (avvia il turno), /fine (chiude e paga il maturato), /lavoro lascia (al Job Center).", 1024);
	}
	Lang_ShowPlayerDialog(playerid, 9303, DIALOG_STYLE_MSGBOX, "Il tuo lavoro", body, "Chiudi", "");
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
	g_UiJHud[playerid][1] = UI_JhText(playerid, (5.0 + left), (384.0 + 2.0), "LAVORO", 0.22, 1.0, -2994689, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][2] = UI_JhText(playerid, (5.0 + left), (384.0 + 14.0), "Tappa 0/0", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][3] = UI_JhText(playerid, (5.0 + left), (384.0 + 26.0), "Maturato: $0", 0.22, 1.0, 0x66DD66FF, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][4] = UI_JhText(playerid, (5.0 + left), (384.0 + 38.0), "Tempo 00:00", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiJHud[playerid][5] = UI_JhText(playerid, (5.0 + left), (384.0 + 51.0), "TORNA AL VEICOLO: 45s", 0.2, 0.9, -12303105, t_TEXT_DRAW_ALIGN:1);
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
	new str[48];
	format(str, 48, "LAVORO: %s", g_UiJbName[jobId]);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][1], str);
	format(str, 48, "%s 0/%d", g_UiJbStepWord[jobId], g_UiJbSteps[jobId]);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][2], str);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][3], "Maturato: $0");
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][4], "Tempo 00:00");
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
	new str[64], money[16];
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
		format(str, 64, "Rientro al deposito");
	}
	else
	{
		format(str, 64, "%s %d/%d", g_UiJbStepWord[job], step, stepTotal);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][2], str);
	UI_JbMoney(earned, money, 16);
	format(str, 64, "Maturato: $%s", money);
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][3], str);
	if (dwellSec >= 0)
	{
		format(str, 64, "Tempo %02d:%02d  Sosta %ds", elapsedSec / 60, elapsedSec % 60, dwellSec);
	}
	else
	{
		format(str, 64, "Tempo %02d:%02d", elapsedSec / 60, elapsedSec % 60);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiJHud[playerid][4], str);
	if (offVehicleSec >= 0)
	{
		format(str, 64, "TORNA AL VEICOLO: %ds", offVehicleSec);
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
