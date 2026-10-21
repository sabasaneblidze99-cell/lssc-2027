new g_UiStoryOriginText[6][320] = {
	{0},
	"გაიზარდა ფერმაში, ბლუბერისა და მონტგომერის შორის, რედ-კაუნტიში.",
	"ეინჯელ-პაინიდანაა - ტყეებს შორის მდებარე ხის სახლებისა და სახერხი საამქროების დაბიდან.",
	"ფლინტ-კაუნტიდანაა - მინდვრების, საწყობებისა და ცარიელი გზების მხარიდან.",
	"უეტსტოუნიდანაა - ტყეებსა და მთებს შორის, ყველაფრისგან შორს.",
	"ბოუნ-კაუნტიდანაა - უდაბნო, მტვერი და ცოტა მეზობელი."
};
new g_UiStoryOriginShort[6][64] = {
	{0},
	"რედ-კაუნტი",
	"ეინჯელ-პაინი",
	"ფლინტ-კაუნტი",
	"უეტსტოუნი",
	"ბოუნ-კაუნტი"
};
new g_UiStoryMotiveText[5][256] = {
	{0},
	"ლოს-სანტოსში სტაბილური სამსახურის საძებნელად ჩამოვიდა.",
	"ნათესავებთან ჩამოვიდა, რომ მათ გვერდით ცხოვრება თავიდან დაეწყო.",
	"ნაჩქარევად ჩამოვიდა - უკან რაღაც დატოვა, რაზეც არ ლაპარაკობს.",
	"ოცნებას გამოეკიდა, რომელსაც პროვინციაში ადგილი არ ჰქონდა."
};
new g_UiStoryMotiveShort[5][48] = {
	{0},
	"სამსახური",
	"ოჯახი",
	"გაქცევა",
	"ოცნება"
};
new g_UiIntroNarration[5][256] = {
	{0},
	"სადაც დავიბადე, ჰორიზონტი სწორი იყო და დრო - ნელი.",
	"მერე გზა გაფართოვდა და ბოლოში სინათლე გამოჩნდა.",
	"ლოს-სანტოსი. ჯერ არ ვიცოდი, რას ვიპოვიდი.",
	"ერთი ჩემოდანი, ცოტა ფული და არანაირი გეგმა."
};
new PlayerText:g_UiIntroTd[MAX_PLAYERS][7];
new bool:g_UiIntroActive[MAX_PLAYERS];
new g_UiIntroTimer[MAX_PLAYERS];
new g_UiIntroAlpha[MAX_PLAYERS];
new g_UiIntroFadeTarget[MAX_PLAYERS];
new g_UiIntroFadeStep[MAX_PLAYERS];
new g_UiIntroFadeLeft[MAX_PLAYERS];
new g_UiIntroFadeTick[MAX_PLAYERS];
new g_UiIntroFadeNext[MAX_PLAYERS];
new g_UiIntroLockEnd[MAX_PLAYERS];
new g_UiIntroLockShown[MAX_PLAYERS];
new bool:g_UiIntroHintOn[MAX_PLAYERS];
new g_UiStoryOrigin[MAX_PLAYERS];
new g_UiStoryMotive[MAX_PLAYERS];
new bool:g_UiStoryActive[MAX_PLAYERS];

bool:UI_IntroValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

UI_StoryOriginName(origin, dest[], size)
{
	dest[0] = 0;
	if (origin >= 1 && origin <= 5)
	{
		Lang_Translit(g_UiStoryOriginShort[origin], dest, size);
	}
}

UI_StoryCompose(const name[], origin, motive, dest[], size)
{
	if (origin < 0 || origin > 5)
	{
		origin = 0;
	}
	if (motive < 0 || motive > 4)
	{
		motive = 0;
	}
	if (origin == 0 && motive == 0)
	{
		strcopy(dest, "წარსული უცნობია.", size);
		return 0;
	}
	new nm[96];
	strcopy(nm, name, 96);
	for (new i = 0; nm[i] != 0; i++)
	{
		if (nm[i] == 95)
		{
			nm[i] = 32;
		}
	}
	if (origin == 0)
	{
		format(dest, size, "%s. წარმოშობა უცნობია. %s", nm, g_UiStoryMotiveText[motive]);
	}
	else if (motive == 0)
	{
		format(dest, size, "%s. %s", nm, g_UiStoryOriginText[origin]);
	}
	else
	{
		format(dest, size, "%s. %s %s", nm, g_UiStoryOriginText[origin], g_UiStoryMotiveText[motive]);
	}
	return 0;
}

UI_ShowOriginSelect(playerid)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	g_UiStoryActive[playerid] = true;
	new body[3200], line[384];
	body[0] = 0;
	for (new i = 1; i <= 5; i++)
	{
		format(line, 384, "{FFD24D}%s\n", g_UiStoryOriginShort[i]);
		strcat(body, line, 3200);
	}
	strcat(body, "{AAAAAA}არ მინდა თქმა", 3200);
	Lang_ShowPlayerDialog(playerid, 9600, DIALOG_STYLE_LIST, "შენი ისტორია: წარმოშობა", body, "არჩევა", "");
	return 0;
}

UI_ShowMotiveSelect(playerid)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	g_UiStoryActive[playerid] = true;
	new body[3200], line[384];
	body[0] = 0;
	for (new i = 1; i <= 4; i++)
	{
		format(line, 384, "{FFD24D}%s\n", g_UiStoryMotiveShort[i]);
		strcat(body, line, 3200);
	}
	strcat(body, "{AAAAAA}არ მინდა თქმა", 3200);
	Lang_ShowPlayerDialog(playerid, 9601, DIALOG_STYLE_LIST, "შენი ისტორია: რატომ ლოს-სანტოსი", body, "არჩევა", "უკან");
	return 0;
}

UI_StoryConfirmShow(playerid)
{
	new text[768], body[3200], name[96];
	GetPlayerName(playerid, name, 96);
	UI_StoryCompose(name, g_UiStoryOrigin[playerid], g_UiStoryMotive[playerid], text, 768);
	format(body, 3200, "{FFFFFF}შენი პერსონაჟის წარსული:\n\n{FFD24D}%s\n\n{FFFFFF}ეს მხოლოდ ისტორიაა: თამაშში შესაძლებლობებს არ ცვლის.\nირჩევა მხოლოდ ერთხელ.", text);
	Lang_ShowPlayerDialog(playerid, 9602, DIALOG_STYLE_MSGBOX, "დაადასტურე შენი ისტორია", body, "დადასტურება", "უკან");
}

UI_StoryBegin(playerid)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	g_UiStoryOrigin[playerid] = 0;
	g_UiStoryMotive[playerid] = 0;
	if (CefUI_Active(playerid) && gPlayer[playerid][pAuthState] == 15)
	{
		for (new i = 1; i <= 5; i++)
		{
			CefUI_AuthStoryOpt(playerid, 1, i, g_UiStoryOriginShort[i], g_UiStoryOriginText[i]);
		}
		for (new i = 1; i <= 4; i++)
		{
			CefUI_AuthStoryOpt(playerid, 2, i, g_UiStoryMotiveShort[i], g_UiStoryMotiveText[i]);
		}
		CefUI_AuthStep(playerid, "story", "", 0);
		return 0;
	}
	UI_ShowOriginSelect(playerid);
	return 0;
}

UI_StoryViewShow(playerid, const name[], origin, motive, const title[])
{
	new text[768], body[1600];
	UI_StoryCompose(name, origin, motive, text, 768);
	format(body, 1600, "{FFD24D}%s", text);
	Lang_ShowPlayerDialog(playerid, 9603, DIALOG_STYLE_MSGBOX, title, body, "დახურვა", "");
}

UI_StoryShowOwn(playerid, const name[], origin, motive)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	UI_StoryViewShow(playerid, name, origin, motive, "შენი ისტორია");
	return 0;
}

UI_StoryShowOther(playerid, const name[], origin, motive)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	UI_StoryViewShow(playerid, name, origin, motive, "პერსონაჟის ისტორია");
	return 0;
}

bool:UI_IntroOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	#pragma unused inputtext
	if (!UI_IntroValid(playerid) || dialogid < 9600 || dialogid > 9603)
	{
		return false;
	}
	switch (dialogid)
	{
		case 9600:
		{
			if (!g_UiStoryActive[playerid])
			{
				return true;
			}
			if (!response || listitem < 0 || listitem > 5)
			{
				UI_ShowOriginSelect(playerid);
				return true;
			}
			if (listitem == 5)
			{
				g_UiStoryOrigin[playerid] = 0;
				g_UiStoryMotive[playerid] = 0;
				UI_StoryConfirmShow(playerid);
				return true;
			}
			g_UiStoryOrigin[playerid] = listitem + 1;
			g_UiStoryMotive[playerid] = 0;
			UI_ShowMotiveSelect(playerid);
			return true;
		}
		case 9601:
		{
			if (!g_UiStoryActive[playerid])
			{
				return true;
			}
			if (!response || listitem < 0 || listitem > 4)
			{
				if (response)
				{
					UI_ShowMotiveSelect(playerid);
				}
				else
				{
					UI_ShowOriginSelect(playerid);
				}
				return true;
			}
			g_UiStoryMotive[playerid] = listitem == 4 ? 0 : listitem + 1;
			UI_StoryConfirmShow(playerid);
			return true;
		}
		case 9602:
		{
			if (!g_UiStoryActive[playerid])
			{
				return true;
			}
			if (!response)
			{
				if (g_UiStoryOrigin[playerid] == 0)
				{
					UI_ShowOriginSelect(playerid);
				}
				else
				{
					UI_ShowMotiveSelect(playerid);
				}
				return true;
			}
			new origin = g_UiStoryOrigin[playerid], reason = g_UiStoryMotive[playerid];
			if (origin == 0)
			{
				reason = 0;
			}
			g_UiStoryActive[playerid] = false;
			g_UiStoryOrigin[playerid] = 0;
			g_UiStoryMotive[playerid] = 0;
			UI_OnStoryChosen(playerid, origin, reason);
			return true;
		}
		case 9603:
		{
			return true;
		}
	}
	return false;
}

PlayerText:UI_IntroBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
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

PlayerText:UI_IntroText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_FONT:font)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, TEXT_DRAW_ALIGN_CENTRE);
	PlayerTextDrawColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, font);
	PlayerTextDrawSetProportional(playerid, td, true);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	PlayerTextDrawTextSize(playerid, td, 0.0, 600.0);
	return td;
}

UI_IntroTimerStop(playerid)
{
	if (g_UiIntroTimer[playerid] != 0)
	{
		KillTimer(g_UiIntroTimer[playerid]);
		g_UiIntroTimer[playerid] = 0;
	}
}

UI_IntroTimerEnsure(playerid)
{
	if (g_UiIntroTimer[playerid] == 0)
	{
		g_UiIntroTimer[playerid] = SetTimerEx("UI_IntroOnTimer", 60, true, "d", playerid);
	}
}

UI_IntroNeedTimer(playerid)
{
	return g_UiIntroFadeLeft[playerid] > 0 || g_UiIntroLockEnd[playerid] != 0;
}

UI_IntroApplyAlpha(playerid)
{
	if (!g_UiIntroActive[playerid])
	{
		return 0;
	}
	PlayerTextDrawBoxColour(playerid, PlayerText:g_UiIntroTd[playerid][0], g_UiIntroAlpha[playerid] & 255);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][0]);
	return 0;
}

UI_IntroResetState(playerid)
{
	g_UiIntroActive[playerid] = false;
	g_UiIntroAlpha[playerid] = 0;
	g_UiIntroFadeTarget[playerid] = 0;
	g_UiIntroFadeStep[playerid] = 0;
	g_UiIntroFadeLeft[playerid] = 0;
	g_UiIntroFadeTick[playerid] = 60;
	g_UiIntroFadeNext[playerid] = 0;
	g_UiIntroLockEnd[playerid] = 0;
	g_UiIntroLockShown[playerid] = -1;
	g_UiIntroHintOn[playerid] = false;
	for (new i = 0; i < 7; i++)
	{
		g_UiIntroTd[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
	}
}

UI_IntroUiDestroy(playerid)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	UI_IntroTimerStop(playerid);
	if (g_UiIntroActive[playerid])
	{
		for (new i = 0; i < 7; i++)
		{
			if (g_UiIntroTd[playerid][i] != INVALID_PLAYER_TEXT_DRAW)
			{
				PlayerTextDrawDestroy(playerid, PlayerText:g_UiIntroTd[playerid][i]);
			}
		}
	}
	UI_IntroResetState(playerid);
	g_UiStoryActive[playerid] = false;
	g_UiStoryOrigin[playerid] = 0;
	g_UiStoryMotive[playerid] = 0;
	return 0;
}

UI_IntroCreate(playerid)
{
	if (!UI_IntroValid(playerid))
	{
		return 0;
	}
	UI_IntroUiDestroy(playerid);
	CefUI_AuthClose(playerid);
	g_UiIntroActive[playerid] = true;
	g_UiIntroTd[playerid][0] = UI_IntroBox(playerid, 0.0, 0.0, 640.0, 482.0, 0);
	g_UiIntroTd[playerid][1] = UI_IntroBox(playerid, 0.0, 0.0, 640.0, 56.0, 255);
	g_UiIntroTd[playerid][2] = UI_IntroBox(playerid, 0.0, (480.0 - 56.0), 640.0, (56.0 + 2.0), 255);
	g_UiIntroTd[playerid][3] = UI_IntroText(playerid, 320.0, (8.0 + (480.0 - 56.0)), "_", 0.3, 1.4, 0xF2F2F2FF, t_TEXT_DRAW_FONT:1);
	g_UiIntroTd[playerid][4] = UI_IntroText(playerid, 320.0, 170.0, "ლოს-სანტოსი", 0.9, 3.6, -1, t_TEXT_DRAW_FONT:3);
	g_UiIntroTd[playerid][5] = UI_IntroText(playerid, 320.0, 232.0, "ყველას ისტორია აქ იწყება.", 0.34, 1.5, 0xF2F2F2FF, t_TEXT_DRAW_FONT:1);
	g_UiIntroTd[playerid][6] = UI_IntroText(playerid, 320.0, (480.0 - 20.0), "_", 0.22, 1.0, 0xB8B8B8FF, t_TEXT_DRAW_FONT:1);
	UI_IntroApplyAlpha(playerid);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][1]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][2]);
	return 0;
}

UI_IntroLetterbox(playerid, bool:on)
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	if (on)
	{
		PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][1]);
		PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][2]);
	}
	else
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiIntroTd[playerid][1]);
		PlayerTextDrawHide(playerid, PlayerText:g_UiIntroTd[playerid][2]);
	}
	return 0;
}

UI_IntroFade(playerid, alpha)
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	if (alpha < 0)
	{
		alpha = 0;
	}
	if (alpha > 255)
	{
		alpha = 255;
	}
	g_UiIntroFadeLeft[playerid] = 0;
	g_UiIntroAlpha[playerid] = alpha;
	UI_IntroApplyAlpha(playerid);
	if (!UI_IntroNeedTimer(playerid))
	{
		UI_IntroTimerStop(playerid);
	}
	return 0;
}

UI_IntroFadeTo(playerid, alpha, steps, stepMs)
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	if (alpha < 0)
	{
		alpha = 0;
	}
	if (alpha > 255)
	{
		alpha = 255;
	}
	if (steps < 1)
	{
		steps = 1;
	}
	if (stepMs < 60)
	{
		stepMs = 60;
	}
	new diff = alpha - g_UiIntroAlpha[playerid];
	if (diff == 0)
	{
		g_UiIntroFadeLeft[playerid] = 0;
		if (!UI_IntroNeedTimer(playerid))
		{
			UI_IntroTimerStop(playerid);
		}
		UI_OnIntroFadeDone(playerid, alpha);
		return 0;
	}
	g_UiIntroFadeTarget[playerid] = alpha;
	g_UiIntroFadeLeft[playerid] = steps;
	g_UiIntroFadeStep[playerid] = diff / steps;
	if (g_UiIntroFadeStep[playerid] == 0)
	{
		g_UiIntroFadeStep[playerid] = diff > 0 ? 1 : -1;
	}
	g_UiIntroFadeTick[playerid] = stepMs;
	g_UiIntroFadeNext[playerid] = GetTickCount() + stepMs;
	UI_IntroTimerEnsure(playerid);
	return 0;
}

UI_IntroSubtitle(playerid, const text[])
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	if (text[0] == 0)
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiIntroTd[playerid][3]);
		return 0;
	}
	new tl[256], buf[213];
	Lang_Translit(text, tl, 256);
	strcopy(buf, tl, 213);
	Lang_PlayerTextDrawSetString(playerid, g_UiIntroTd[playerid][3], buf);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][3]);
	return 0;
}

UI_IntroNarrate(playerid, phase)
{
	if (phase < 1 || phase > 4)
	{
		UI_IntroSubtitle(playerid, "");
	}
	else
	{
		UI_IntroSubtitle(playerid, g_UiIntroNarration[phase]);
	}
}

UI_IntroTitle(playerid, bool:show)
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	if (show)
	{
		PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][4]);
		PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][5]);
	}
	else
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiIntroTd[playerid][4]);
		PlayerTextDrawHide(playerid, PlayerText:g_UiIntroTd[playerid][5]);
	}
	return 0;
}

UI_IntroHintRender(playerid, secondsLeft)
{
	new text[288];
	if (secondsLeft > 0)
	{
		format(text, 288, "გამოტოვება შეგეძლება %d წამში", secondsLeft);
	}
	else
	{
		strcopy(text, "/salta (ან Enter) გამოსატოვებლად", 96);
	}
	Lang_PlayerTextDrawSetString(playerid, g_UiIntroTd[playerid][6], text);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIntroTd[playerid][6]);
	g_UiIntroLockShown[playerid] = secondsLeft;
}

UI_IntroSkipHint(playerid, lockSeconds)
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	g_UiIntroHintOn[playerid] = true;
	if (lockSeconds > 0)
	{
		g_UiIntroLockEnd[playerid] = GetTickCount() + lockSeconds * 1000;
		if (g_UiIntroLockEnd[playerid] == 0)
		{
			g_UiIntroLockEnd[playerid] = 1;
		}
		UI_IntroHintRender(playerid, lockSeconds);
		UI_IntroTimerEnsure(playerid);
	}
	else
	{
		g_UiIntroLockEnd[playerid] = 0;
		UI_IntroHintRender(playerid, 0);
		if (!UI_IntroNeedTimer(playerid))
		{
			UI_IntroTimerStop(playerid);
		}
	}
	return 0;
}

UI_IntroSkipHintHide(playerid)
{
	if (!UI_IntroValid(playerid) || !g_UiIntroActive[playerid])
	{
		return 0;
	}
	g_UiIntroHintOn[playerid] = false;
	g_UiIntroLockEnd[playerid] = 0;
	g_UiIntroLockShown[playerid] = -1;
	PlayerTextDrawHide(playerid, PlayerText:g_UiIntroTd[playerid][6]);
	if (!UI_IntroNeedTimer(playerid))
	{
		UI_IntroTimerStop(playerid);
	}
	return 0;
}

public UI_IntroOnTimer(playerid)
{
	if (!UI_IntroValid(playerid) || !IsPlayerConnected(playerid) || !g_UiIntroActive[playerid])
	{
		if (UI_IntroValid(playerid))
		{
			UI_IntroTimerStop(playerid);
		}
		return 1;
	}
	new now = GetTickCount();
	if (g_UiIntroLockEnd[playerid] != 0)
	{
		new left = g_UiIntroLockEnd[playerid] - now;
		if (left <= 0)
		{
			g_UiIntroLockEnd[playerid] = 0;
			if (g_UiIntroHintOn[playerid])
			{
				UI_IntroHintRender(playerid, 0);
			}
		}
		else
		{
			new sec = (left + 999) / 1000;
			if (sec != g_UiIntroLockShown[playerid] && g_UiIntroHintOn[playerid])
			{
				UI_IntroHintRender(playerid, sec);
			}
		}
	}
	if (g_UiIntroFadeLeft[playerid] > 0 && now - g_UiIntroFadeNext[playerid] >= 0)
	{
		g_UiIntroFadeLeft[playerid]--;
		g_UiIntroFadeNext[playerid] = now + g_UiIntroFadeTick[playerid];
		if (g_UiIntroFadeLeft[playerid] == 0)
		{
			g_UiIntroAlpha[playerid] = g_UiIntroFadeTarget[playerid];
		}
		else
		{
			g_UiIntroAlpha[playerid] += g_UiIntroFadeStep[playerid];
			if (g_UiIntroAlpha[playerid] < 0)
			{
				g_UiIntroAlpha[playerid] = 0;
			}
			if (g_UiIntroAlpha[playerid] > 255)
			{
				g_UiIntroAlpha[playerid] = 255;
			}
		}
		UI_IntroApplyAlpha(playerid);
		if (g_UiIntroFadeLeft[playerid] == 0)
		{
			if (!UI_IntroNeedTimer(playerid))
			{
				UI_IntroTimerStop(playerid);
			}
			UI_OnIntroFadeDone(playerid, g_UiIntroAlpha[playerid]);
			return 1;
		}
	}
	if (!UI_IntroNeedTimer(playerid))
	{
		UI_IntroTimerStop(playerid);
	}
	return 1;
}
