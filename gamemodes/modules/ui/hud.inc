new PlayerText:g_UiHud[MAX_PLAYERS][6];
new bool:g_UiHudActive[MAX_PLAYERS];

UI_ExpForLevel(level)
{
	return (level - 1) * 2 * level / 2 + (level - 1) * 2;
}

PlayerText:UI_HudText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
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

PlayerText:UI_HudBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
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

UI_HudSetBar(playerid, Float:fraction)
{
	if ((fraction < 0.0))
	{
		fraction = 0.0;
	}
	if ((fraction > 1.0))
	{
		fraction = 1.0;
	}
	new Float:x = (5.0 + (632.0 - 150.0));
	PlayerTextDrawTextSize(playerid, PlayerText:g_UiHud[playerid][5], (0.1 + (x + (140.0 * fraction))), 0.0);
	PlayerTextDrawShow(playerid, PlayerText:g_UiHud[playerid][5]);
}

UI_HudDestroy(playerid)
{
	CefUI_HudShow(playerid, false);
	if (playerid < 0 || playerid >= MAX_PLAYERS || !g_UiHudActive[playerid])
	{
		return 0;
	}
	for (new i = 0; i < 6; i++)
	{
		if (g_UiHud[playerid][i] != INVALID_PLAYER_TEXT_DRAW)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiHud[playerid][i]);
		}
		g_UiHud[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
	}
	g_UiHudActive[playerid] = false;
	return 0;
}

UI_HudCreate(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (g_UiHudActive[playerid])
	{
		UI_HudDestroy(playerid);
	}
	if (CefUI_Active(playerid))
	{
		CefUI_HudShow(playerid, true);
		return 0;
	}
	for (new i = 0; i < 6; i++)
	{
		g_UiHud[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
	}
	new Float:left = (632.0 - 150.0);
	g_UiHud[playerid][0] = UI_HudBox(playerid, left, 330.0, 150.0, 62.0, 153);
	g_UiHud[playerid][1] = UI_HudText(playerid, (632.0 - 5.0), (330.0 + 3.0), "$0", 0.3, 1.2, 0x66DD66FF, t_TEXT_DRAW_ALIGN:3);
	g_UiHud[playerid][2] = UI_HudText(playerid, (632.0 - 5.0), (330.0 + 20.0), "Banca: $0", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:3);
	g_UiHud[playerid][3] = UI_HudText(playerid, (5.0 + left), (330.0 + 36.0), "Liv. 1  Exp 0", 0.22, 1.0, -2994689, t_TEXT_DRAW_ALIGN:1);
	g_UiHud[playerid][4] = UI_HudBox(playerid, (5.0 + left), (330.0 + 51.0), 140.0, 6.0, 0x444444FF);
	g_UiHud[playerid][5] = UI_HudBox(playerid, (5.0 + left), (330.0 + 51.0), 140.0, 6.0, -2994689);
	for (new i = 0; i < 6; i++)
	{
		if (i != 5)
		{
			PlayerTextDrawShow(playerid, PlayerText:g_UiHud[playerid][i]);
		}
	}
	g_UiHudActive[playerid] = true;
	UI_HudSetBar(playerid, 0.0);
	return 0;
}

UI_HudCefData(playerid, money, bank, level, exp)
{
	if (level >= 50)
	{
		CefUI_HudData(playerid, money, bank, 50, 0, 0);
		return 0;
	}
	if (level < 1)
	{
		level = 1;
	}
	new step = level * 2 + 2;
	new inLevel = exp - UI_ExpForLevel(level);
	if (inLevel < 0)
	{
		inLevel = 0;
	}
	if (inLevel > step)
	{
		inLevel = step;
	}
	CefUI_HudData(playerid, money, bank, level, inLevel, step);
	return 0;
}

bool:UI_HudIsActive(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return false;
	}
	return g_UiHudActive[playerid];
}

UI_HudUpdate(playerid, money, bank, level, exp)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	UI_HudCefData(playerid, money, bank, level, exp);
	if (!g_UiHudActive[playerid])
	{
		return 0;
	}
	new str[48];
	format(str, 48, "$%d", money);
	Lang_PlayerTextDrawSetString(playerid, g_UiHud[playerid][1], str);
	format(str, 48, "Banca: $%d", bank);
	Lang_PlayerTextDrawSetString(playerid, g_UiHud[playerid][2], str);
	if (level >= 50)
	{
		format(str, 48, "Liv. %d  (MAX)", 50);
		Lang_PlayerTextDrawSetString(playerid, g_UiHud[playerid][3], str);
		UI_HudSetBar(playerid, 1.0);
		return 0;
	}
	if (level < 1)
	{
		level = 1;
	}
	new base = UI_ExpForLevel(level);
	new step = level * 2 + 2;
	new inLevel = exp - base;
	if (inLevel < 0)
	{
		inLevel = 0;
	}
	if (inLevel > step)
	{
		inLevel = step;
	}
	format(str, 48, "Liv. %d  Exp %d/%d", level, inLevel, step);
	Lang_PlayerTextDrawSetString(playerid, g_UiHud[playerid][3], str);
	UI_HudSetBar(playerid, (float(inLevel) / float(step)));
	return 0;
}
