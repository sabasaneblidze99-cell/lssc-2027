new g_UiVehModel[13] = {462, 436, 410, 491, 466, 547, 529, 543, 461, 422, 549, 500, 405};
new g_UiVehPrice[13] = {12000, 12500, 13500, 16000, 17500, 19000, 21000, 22000, 24000, 26000, 28000, 34000, 38000};
new g_UiVehMinLevel[13] = {1, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5};
new g_UiVehName[13][12] = {
	"Faggio",
	"Previon",
	"Manana",
	"Virgo",
	"Glendale",
	"Primo",
	"Willard",
	"Sadler",
	"PCJ-600",
	"Bobcat",
	"Tampa",
	"Mesa",
	"Sentinel"
};
new g_UiVehLevel[MAX_PLAYERS];
new g_UiVehMoney[MAX_PLAYERS];
new g_UiVehBank[MAX_PLAYERS];
new g_UiVehSel[MAX_PLAYERS];
new g_UiVehCount[MAX_PLAYERS];
new g_UiVehSlot[MAX_PLAYERS][4];
new g_UiVehLModel[MAX_PLAYERS][4];
new g_UiVehLPlate[MAX_PLAYERS][4][8];
new g_UiVehFuel[MAX_PLAYERS][4];
new g_UiVehState[MAX_PLAYERS][4];

UI_VehMoney(value, out[], size)
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

UI_VehCatalogIndexOf(model)
{
	for (new i = 0; i < 13; i++)
	{
		if (g_UiVehModel[i] == model)
		{
			return i;
		}
	}
	return -1;
}

UI_VehModelName(model, out[], size)
{
	new idx = UI_VehCatalogIndexOf(model);
	if (idx >= 0)
	{
		format(out, size, "%s", g_UiVehName[idx]);
	}
	else
	{
		format(out, size, "Modello %d", model);
	}
}

UI_VehicleShopShow(playerid, level, money, bank)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_UiVehLevel[playerid] = level;
	g_UiVehMoney[playerid] = money;
	g_UiVehBank[playerid] = bank;
	new body[1536], line[96], price[16];
	for (new i = 0; i < 13; i++)
	{
		UI_VehMoney(g_UiVehPrice[i], price, 16);
		if (level >= g_UiVehMinLevel[i])
		{
			format(line, 96, "{FFFFFF}%s\t{66DD66}$%s\t{AAAAAA}liv. %d\n", g_UiVehName[i], price, g_UiVehMinLevel[i]);
		}
		else
		{
			format(line, 96, "{777777}%s\t$%s\t{FF6666}Bloccato: liv. %d\n", g_UiVehName[i], price, g_UiVehMinLevel[i]);
		}
		strcat(body, line, 1536);
	}
	Lang_ShowPlayerDialog(playerid, 9100, 4, "Concessionario di Los Santos", body, "Scegli", "Chiudi");
	return 0;
}

UI_VehPayShow(playerid)
{
	new idx = g_UiVehSel[playerid];
	new body[320], price[16], cash[16], bank[16], t[48];
	UI_VehMoney(g_UiVehPrice[idx], price, 16);
	UI_VehMoney(g_UiVehMoney[playerid], cash, 16);
	UI_VehMoney(g_UiVehBank[playerid], bank, 16);
	format(body, 320, "{FFFFFF}Contanti\t%s$%s\n", g_UiVehMoney[playerid] >= g_UiVehPrice[idx] ? "{66DD66}" : "{FF6666}", cash);
	format(t, 48, "{FFFFFF}Banca\t%s$%s\n", g_UiVehBank[playerid] >= g_UiVehPrice[idx] ? "{66DD66}" : "{FF6666}", bank);
	strcat(body, t, 320);
	new title[64];
	format(title, 64, "%s - $%s: pagamento", g_UiVehName[idx], price);
	Lang_ShowPlayerDialog(playerid, 9101, 4, title, body, "Acquista", "Indietro");
}

UI_VehicleListClear(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_UiVehCount[playerid] = 0;
	return 0;
}

UI_VehicleListAdd(playerid, slot, model, const plate[], fuelPct, vstate)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	new n = g_UiVehCount[playerid];
	if (n >= 4)
	{
		return 0;
	}
	if (fuelPct < 0)
	{
		fuelPct = 0;
	}
	if (fuelPct > 100)
	{
		fuelPct = 100;
	}
	g_UiVehSlot[playerid][n] = slot;
	g_UiVehLModel[playerid][n] = model;
	strcopy(g_UiVehLPlate[playerid][n], plate, 8);
	g_UiVehFuel[playerid][n] = fuelPct;
	g_UiVehState[playerid][n] = vstate;
	g_UiVehCount[playerid] = n + 1;
	return 0;
}

UI_VehicleListShow(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (g_UiVehCount[playerid] <= 0)
	{
		Lang_ShowPlayerDialog(playerid, 9105, 0, "I miei veicoli", "{FFFFFF}Non possiedi veicoli.\nAcquistane uno al concessionario di Los Santos.", "Ok", "");
		return 0;
	}
	new body[1536], line[128], name[16], stxt[24];
	strcat(body, "Slot\tModello\tTarga\tCarburante\tStato\n", 1536);
	for (new i = 0; i < g_UiVehCount[playerid]; i++)
	{
		UI_VehModelName(g_UiVehLModel[playerid][i], name, 16);
		switch (g_UiVehState[playerid][i])
		{
			case 1:
			{
				format(stxt, 24, "{66AAFF}Parcheggiato");
			}
			case 2:
			{
				format(stxt, 24, "{FF6666}Distrutto");
			}
			default:
			{
				format(stxt, 24, "{66DD66}In giro");
			}
		}
		format(line, 128, "%d\t%s\t%s\t%d%%\t%s\n", g_UiVehSlot[playerid][i], name, g_UiVehLPlate[playerid][i], g_UiVehFuel[playerid][i], stxt);
		strcat(body, line, 1536);
	}
	Lang_ShowPlayerDialog(playerid, 9102, 5, "I miei veicoli", body, "Seleziona", "Chiudi");
	return 0;
}

UI_VehActionShow(playerid)
{
	new i = g_UiVehSel[playerid], name[16], title[64];
	UI_VehModelName(g_UiVehLModel[playerid][i], name, 16);
	format(title, 64, "%s [%s]", name, g_UiVehLPlate[playerid][i]);
	Lang_ShowPlayerDialog(playerid, 9103, 2, title, "Trova veicolo\nVendi veicolo", "Scegli", "Indietro");
}

UI_VehSellConfirmShow(playerid)
{
	new i = g_UiVehSel[playerid], name[16], body[256], amount[16];
	new idx = UI_VehCatalogIndexOf(g_UiVehLModel[playerid][i]);
	UI_VehModelName(g_UiVehLModel[playerid][i], name, 16);
	if (idx >= 0)
	{
		UI_VehMoney(g_UiVehPrice[idx] * 60 / 100, amount, 16);
		format(body, 256, "{FFFFFF}Vendere {FFD24D}%s{FFFFFF} (targa %s)?\n\nRiceverai circa {66DD66}$%s{FFFFFF} (%d%% del listino) in banca.\nL'operazione non e' reversibile.", name, g_UiVehLPlate[playerid][i], amount, 60);
	}
	else
	{
		format(body, 256, "{FFFFFF}Vendere {FFD24D}%s{FFFFFF} (targa %s)?\n\nRiceverai il %d%% del prezzo di listino in banca.\nL'operazione non e' reversibile.", name, g_UiVehLPlate[playerid][i], 60);
	}
	Lang_ShowPlayerDialog(playerid, 9104, 0, "Conferma vendita", body, "Vendi", "Annulla");
}

bool:UI_VehicleOnDialogResponse(playerid, dialogid, response, listitem)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return false;
	}
	switch (dialogid)
	{
		case 9100:
		{
			if (!response)
			{
				return true;
			}
			if (listitem < 0 || listitem >= 13)
			{
				UI_VehicleShopShow(playerid, g_UiVehLevel[playerid], g_UiVehMoney[playerid], g_UiVehBank[playerid]);
				return true;
			}
			if (g_UiVehLevel[playerid] < g_UiVehMinLevel[listitem])
			{
				SendClientMessage(playerid, -10066177, "Questo modello richiede un livello superiore.");
				UI_VehicleShopShow(playerid, g_UiVehLevel[playerid], g_UiVehMoney[playerid], g_UiVehBank[playerid]);
				return true;
			}
			g_UiVehSel[playerid] = listitem;
			UI_VehPayShow(playerid);
			return true;
		}
		case 9101:
		{
			if (!response || listitem < 0 || listitem > 1)
			{
				UI_VehicleShopShow(playerid, g_UiVehLevel[playerid], g_UiVehMoney[playerid], g_UiVehBank[playerid]);
				return true;
			}
			new idx = g_UiVehSel[playerid];
			new funds = listitem == 1 ? g_UiVehBank[playerid] : g_UiVehMoney[playerid];
			if (idx < 0 || idx >= 13)
			{
				return true;
			}
			if (funds < g_UiVehPrice[idx])
			{
				SendClientMessage(playerid, -10066177, "Fondi insufficienti con questo metodo di pagamento.");
				UI_VehPayShow(playerid);
				return true;
			}
			UI_OnVehicleBuyConfirm(playerid, idx, listitem);
			return true;
		}
		case 9102:
		{
			if (!response)
			{
				return true;
			}
			if (listitem < 0 || listitem >= g_UiVehCount[playerid])
			{
				UI_VehicleListShow(playerid);
				return true;
			}
			g_UiVehSel[playerid] = listitem;
			UI_VehActionShow(playerid);
			return true;
		}
		case 9103:
		{
			if (!response)
			{
				UI_VehicleListShow(playerid);
				return true;
			}
			new i = g_UiVehSel[playerid];
			if (i < 0 || i >= g_UiVehCount[playerid])
			{
				return true;
			}
			if (listitem == 0)
			{
				UI_OnVehicleListAction(playerid, g_UiVehSlot[playerid][i], 0);
			}
			else if (listitem == 1)
			{
				UI_VehSellConfirmShow(playerid);
			}
			else
			{
				UI_VehActionShow(playerid);
			}
			return true;
		}
		case 9104:
		{
			new i = g_UiVehSel[playerid];
			if (!response)
			{
				UI_VehActionShow(playerid);
				return true;
			}
			if (i < 0 || i >= g_UiVehCount[playerid])
			{
				return true;
			}
			UI_OnVehicleListAction(playerid, g_UiVehSlot[playerid][i], 1);
			return true;
		}
		case 9105:
		{
			return true;
		}
	}
	return false;
}
new g_UiVHud[MAX_PLAYERS][6];
new bool:g_UiVHudCreated[MAX_PLAYERS];
new bool:g_UiVHudShown[MAX_PLAYERS];

PlayerText:UI_VHudText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, text);
	PlayerTextDrawLetterSize(playerid, td, lw, lh);
	PlayerTextDrawAlignment(playerid, td, align);
	PlayerTextDrawColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetProportional(playerid, td, true);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawBackgroundColour(playerid, td, 255);
	return td;
}

PlayerText:UI_VHudBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
{
	new PlayerText:td = Lang_CreatePlayerTextDraw(playerid, x, y, "_");
	PlayerTextDrawLetterSize(playerid, td, 0.0, (h / 9.0));
	PlayerTextDrawTextSize(playerid, td, (x + w), 0.0);
	PlayerTextDrawUseBox(playerid, td, true);
	PlayerTextDrawBoxColour(playerid, td, color);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawSetOutline(playerid, td, 0);
	return td;
}

UI_VHudCreate(playerid)
{
	if (g_UiVHudCreated[playerid])
	{
		return 0;
	}
	new Float:left = (632.0 - 150.0);
	g_UiVHud[playerid][0] = _:UI_VHudBox(playerid, left, 398.0, 150.0, 56.0, 153);
	g_UiVHud[playerid][1] = UI_VHudText(playerid, (632.0 - 5.0), (398.0 + 2.0), "0 km/h", 0.3, 1.2, -1, t_TEXT_DRAW_ALIGN:3);
	g_UiVHud[playerid][2] = UI_VHudText(playerid, (5.0 + left), (398.0 + 22.0), "Benzina 0.0/0 L", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiVHud[playerid][3] = UI_VHudBox(playerid, (5.0 + left), (398.0 + 35.0), 140.0, 6.0, 0x444444FF);
	g_UiVHud[playerid][4] = UI_VHudBox(playerid, (5.0 + left), (398.0 + 35.0), 140.0, 6.0, 0x66DD66FF);
	g_UiVHud[playerid][5] = UI_VHudText(playerid, (5.0 + left), (398.0 + 43.0), "MOTORE SPENTO", 0.2, 0.9, -10066177, t_TEXT_DRAW_ALIGN:1);
	g_UiVHudCreated[playerid] = 1;
	return 0;
}

UI_VHudSetVisible(playerid, bool:visible)
{
	for (new i = 0; i < 6; i++)
	{
		if (visible)
		{
			PlayerTextDrawShow(playerid, PlayerText:g_UiVHud[playerid][i]);
		}
		else
		{
			PlayerTextDrawHide(playerid, PlayerText:g_UiVHud[playerid][i]);
		}
	}
}

UI_VehicleHudShow(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (CefUI_Active(playerid))
	{
		return 0;    // tachimetro CEF (ui/hud_cef.inc)
	}
	UI_VHudCreate(playerid);
	g_UiVHudShown[playerid] = 1;
	UI_VHudSetVisible(playerid, true);
	return 0;
}

UI_VehicleHudHide(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !g_UiVHudCreated[playerid])
	{
		return 0;
	}
	g_UiVHudShown[playerid] = 0;
	UI_VHudSetVisible(playerid, false);
	return 0;
}

UI_VehicleHudUpdate(playerid, Float:fuel, Float:fuelMax, speedKmh, bool:engineOn)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !g_UiVHudCreated[playerid] || !g_UiVHudShown[playerid])
	{
		return 0;
	}
	if ((fuelMax < 1.0))
	{
		fuelMax = 1.0;
	}
	if ((fuel < 0.0))
	{
		fuel = 0.0;
	}
	if ((fuel > fuelMax))
	{
		fuel = fuelMax;
	}
	if (speedKmh < 0)
	{
		speedKmh = 0;
	}
	new Float:fraction = (fuel / fuelMax);
	new bool:low = ((100.0 * fraction) < float(15));
	new str[48];
	format(str, 48, "%d km/h", speedKmh);
	Lang_PlayerTextDrawSetString(playerid, g_UiVHud[playerid][1], str);
	format(str, 48, "Benzina %.1f/%.0f L", fuel, fuelMax);
	Lang_PlayerTextDrawSetString(playerid, g_UiVHud[playerid][2], str);
	new bool:blinkOff = low && GetTickCount() / 500 % 2 == 1;
	PlayerTextDrawColour(playerid, PlayerText:g_UiVHud[playerid][2], low ? (blinkOff ? -10066364 : -10066177) : -1);
	PlayerTextDrawShow(playerid, PlayerText:g_UiVHud[playerid][2]);
	PlayerTextDrawBoxColour(playerid, PlayerText:g_UiVHud[playerid][4], low ? -12303105 : 0x66DD66FF);
	PlayerTextDrawTextSize(playerid, PlayerText:g_UiVHud[playerid][4], (0.1 + ((5.0 + (632.0 - 150.0)) + (140.0 * fraction))), 0.0);
	PlayerTextDrawShow(playerid, PlayerText:g_UiVHud[playerid][4]);
	Lang_PlayerTextDrawSetString(playerid, g_UiVHud[playerid][5], engineOn ? "MOTORE ACCESO" : "MOTORE SPENTO");
	PlayerTextDrawColour(playerid, PlayerText:g_UiVHud[playerid][5], engineOn ? 0x66DD66FF : -10066177);
	PlayerTextDrawShow(playerid, PlayerText:g_UiVHud[playerid][5]);
	return 0;
}

UI_VehicleUiDestroy(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (g_UiVHudCreated[playerid])
	{
		for (new i = 0; i < 6; i++)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiVHud[playerid][i]);
			g_UiVHud[playerid][i] = 65535;
		}
	}
	g_UiVHudCreated[playerid] = 0;
	g_UiVHudShown[playerid] = 0;
	g_UiVehCount[playerid] = 0;
	g_UiVehSel[playerid] = 0;
	return 0;
}
