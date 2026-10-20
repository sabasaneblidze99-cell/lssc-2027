new g_UiFcShort[3][8] = {
	"-",
	"LSPD",
	"EMS"
};
new g_UiFcFull[3][40] = {
	"Nessuna fazione",
	"Los Santos Police Department",
	"Los Santos Emergency Medical Services"
};
new g_UiFcRankName[3][7][24] = {
	{"-", "-", "-", "-", "-", "-", "-"},
	{"-", "Cadetto", "Agente", "Agente Scelto", "Sergente", "Tenente", "Capo del Dipartimento"},
	{"-", "Volontario", "Soccorritore", "Paramedico", "Caposquadra", "Medico", "Direttore Sanitario"}
};
new g_UiFcRankPay[3][7] = {
	{0},
	{0, 150, 200, 250, 320, 400, 500},
	{0, 150, 200, 250, 320, 400, 500}
};
new g_UiFcColor[3] = {-1, 0x6FA8FFFF, -8355585};
new g_UiFcFaction[MAX_PLAYERS];
new g_UiFcRank[MAX_PLAYERS];
new bool:g_UiFcDuty[MAX_PLAYERS];
new g_UiFcOnline[MAX_PLAYERS];
new g_UiFcTotal[MAX_PLAYERS];
new g_UiFcMenuAct[MAX_PLAYERS][8];
new g_UiFcMode[MAX_PLAYERS];
new g_UiFcPage[MAX_PLAYERS];
new g_UiFcSel[MAX_PLAYERS];
new g_UiFcCount[MAX_PLAYERS];
new g_UiFcMemId[MAX_PLAYERS][32];
new g_UiFcMemInfo[MAX_PLAYERS][32];
new g_UiFcMemName[MAX_PLAYERS][32][7];

UI_FcValid(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

UI_FcRankText(faction, rank, out[], size)
{
	if (faction < 1 || faction > 2 || rank < 1 || rank > 6)
	{
		format(out, size, "-");
		return 0;
	}
	format(out, size, "%s", g_UiFcRankName[faction][rank]);
}

UI_FcMoney(value, out[], size)
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

UI_FcStatusText(status, out[], size)
{
	switch (status)
	{
		case 2:
		{
			format(out, size, "{66DD66}In servizio");
		}
		case 1:
		{
			format(out, size, "{FFFFFF}Online");
		}
		default:
		{
			format(out, size, "{888888}Offline");
		}
	}
}

UI_FcActionVerb(action, out[], size)
{
	switch (action)
	{
		case 4:
		{
			format(out, size, "Promuovi");
		}
		case 5:
		{
			format(out, size, "Degrada");
		}
		case 6:
		{
			format(out, size, "Licenzia");
		}
		default:
		{
			format(out, size, "Membri");
		}
	}
}

UI_FactionMenuShow(playerid, faction, rank, bool:onDuty, membersOnline, membersTotal)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (faction < 1 || faction > 2 || rank < 1 || rank > 6)
	{
		SendClientMessage(playerid, -10066177, "Non appartieni a nessuna fazione.");
		return 0;
	}
	g_UiFcFaction[playerid] = faction;
	g_UiFcRank[playerid] = rank;
	g_UiFcDuty[playerid] = onDuty;
	g_UiFcOnline[playerid] = membersOnline;
	g_UiFcTotal[playerid] = membersTotal;
	new body[1536], line[192], title[64], rankName[24];
	new n = 0;
	for (new i = 0; i < 8; i++)
	{
		g_UiFcMenuAct[playerid][i] = 0;
	}
	strcat(body, "Informazioni fazione\n", 1536);
	g_UiFcMenuAct[playerid][n++] = 1;
	format(line, 192, "Elenco membri (%d online / %d totali)\n", membersOnline, membersTotal);
	strcat(body, line, 1536);
	g_UiFcMenuAct[playerid][n++] = 2;
	if (rank >= 5)
	{
		strcat(body, "{FFD24D}Invita giocatore\n{FFD24D}Promuovi membro\n{FFD24D}Degrada membro\n{FFD24D}Licenzia membro\n", 1536);
		g_UiFcMenuAct[playerid][n++] = 3;
		g_UiFcMenuAct[playerid][n++] = 4;
		g_UiFcMenuAct[playerid][n++] = 5;
		g_UiFcMenuAct[playerid][n++] = 6;
	}
	UI_FcRankText(faction, rank, rankName, 24);
	format(title, 64, "%s - %s%s", g_UiFcShort[faction], rankName, onDuty ? " (in servizio)" : "");
	Lang_ShowPlayerDialog(playerid, 9320, 2, title, body, "Seleziona", "Chiudi");
}

UI_FcInfoShow(playerid)
{
	new faction = g_UiFcFaction[playerid], rank = g_UiFcRank[playerid];
	if (faction < 1 || faction > 2)
	{
		return 0;
	}
	new body[1536], line[192], rankName[24], pay[16];
	UI_FcRankText(faction, rank, rankName, 24);
	UI_FcMoney(g_UiFcRankPay[faction][rank], pay, 16);
	format(line, 192, "{FFD24D}%s{FFFFFF}\n\n", g_UiFcFull[faction]);
	strcat(body, line, 1536);
	format(line, 192, "Grado: {FFFFFF}%s (%d/%d)\nServizio: %s{FFFFFF}\n", rankName, rank, 6, g_UiFcDuty[playerid] ? "{66DD66}In servizio" : "{FF6666}Fuori servizio");
	strcat(body, line, 1536);
	format(line, 192, "Stipendio: {66DD66}$%s{FFFFFF} ogni %d min di servizio effettivo (versato in banca)\nMembri: %d online / %d totali\n\n", pay, 15, g_UiFcOnline[playerid], g_UiFcTotal[playerid]);
	strcat(body, line, 1536);
	strcat(body, "Comandi: /duty (vicino alla sede), /r (radio), /dep (interdipartimentale), /fazione lascia.", 1536);
	if (rank >= 5)
	{
		format(line, 192, "\nComando: /invita, /promuovi, /degrada, /licenzia (solo su gradi inferiori al tuo; il grado %d si assegna solo da staff).", 6);
		strcat(body, line, 1536);
	}
	Lang_ShowPlayerDialog(playerid, 9321, 0, "Informazioni fazione", body, "Menu", "Chiudi");
}

UI_FcInviteTargetShow(playerid, error[])
{
	new body[384];
	format(body, 384, "{FFFFFF}Inserisci l'ID del giocatore da invitare in {FFD24D}%s{FFFFFF}.\nDeve essere senza fazione, di livello %d o superiore e a meno di %.0f metri.\nL'invito resta valido %d secondi.\n\n%s", g_UiFcShort[g_UiFcFaction[playerid]], 3, 5.0, 60, error);
	Lang_ShowPlayerDialog(playerid, 9325, 1, "Invita giocatore", body, "Invita", "Annulla");
}

UI_FactionMemberListClear(playerid)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	g_UiFcCount[playerid] = 0;
	g_UiFcSel[playerid] = -1;
}

bool:UI_FactionMemberListAdd(playerid, memberId, name[], rank, status)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	new n = g_UiFcCount[playerid];
	if (n >= 32)
	{
		return 0;
	}
	if (rank < 1)
	{
		rank = 1;
	}
	if (rank > 6)
	{
		rank = 6;
	}
	if (status < 0 || status > 2)
	{
		status = 0;
	}
	g_UiFcMemId[playerid][n] = memberId;
	g_UiFcMemInfo[playerid][n] = rank | status << 8;
	strpack(g_UiFcMemName[playerid][n], name, 7);
	g_UiFcCount[playerid] = n + 1;
	return 1;
}

UI_FcMemberName(playerid, idx, out[], size)
{
	strunpack(out, g_UiFcMemName[playerid][idx], size);
}

bool:UI_FcEligible(ownRank, action, rank, reason[], size)
{
	if (action == 2)
	{
		return 1;
	}
	if (rank >= ownRank)
	{
		format(reason, size, "Il bersaglio non ha un grado inferiore al tuo.");
		return 0;
	}
	if (action == 4 && rank + 1 >= ownRank)
	{
		format(reason, size, "Il nuovo grado deve restare inferiore al tuo.");
		return 0;
	}
	if (action == 5 && rank <= 1)
	{
		format(reason, size, "Il membro e' gia' al grado minimo.");
		return 0;
	}
	return 1;
}

UI_FcPagesCount(playerid)
{
	new pages = (g_UiFcCount[playerid] + 12 - 1) / 12;
	return pages < 1 ? 1 : pages;
}

UI_FcMembersPage(playerid, page)
{
	new faction = g_UiFcFaction[playerid], ownRank = g_UiFcRank[playerid], mode = g_UiFcMode[playerid];
	new pages = UI_FcPagesCount(playerid);
	if (page < 0)
	{
		page = 0;
	}
	if (page >= pages)
	{
		page = pages - 1;
	}
	g_UiFcPage[playerid] = page;
	g_UiFcSel[playerid] = -1;
	new title[64], verb[16];
	UI_FcActionVerb(mode, verb, 16);
	if (g_UiFcCount[playerid] == 0)
	{
		format(title, 64, "%s - %s", verb, g_UiFcShort[faction]);
		Lang_ShowPlayerDialog(playerid, 9323, 0, title, "{FFFFFF}Nessun membro da mostrare.", "Chiudi", "");
		return 0;
	}
	new body[1536], line[192], name[25], rankName[24], status[24], reason[8];
	strcat(body, "Nome\tGrado\tStato\n", 1536);
	new start = page * 12;
	new n = g_UiFcCount[playerid] - start;
	if (n > 12)
	{
		n = 12;
	}
	for (new i = 0; i < n; i++)
	{
		new idx = start + i;
		new rank = g_UiFcMemInfo[playerid][idx] & 255;
		new st = g_UiFcMemInfo[playerid][idx] >> 8 & 255;
		UI_FcMemberName(playerid, idx, name, 25);
		UI_FcRankText(faction, rank, rankName, 24);
		if (UI_FcEligible(ownRank, mode, rank, reason, 8))
		{
			UI_FcStatusText(st, status, 24);
			format(line, 192, "{FFFFFF}%s\t{FFD24D}%s (%d)\t%s\n", name, rankName, rank, status);
		}
		else
		{
			format(line, 192, "{888888}%s\t%s (%d)\t{888888}Non selezionabile\n", name, rankName, rank);
		}
		strcat(body, line, 1536);
	}
	if (page > 0)
	{
		strcat(body, "{FFD24D}<< Pagina precedente\t\t\n", 1536);
	}
	if (page < pages - 1)
	{
		strcat(body, "{FFD24D}Pagina successiva >>\t\t\n", 1536);
	}
	if (mode == 2)
	{
		format(title, 64, "Membri %s (pag. %d/%d)", g_UiFcShort[faction], page + 1, pages);
	}
	else
	{
		format(title, 64, "%s membro - %s (pag. %d/%d)", verb, g_UiFcShort[faction], page + 1, pages);
	}
	Lang_ShowPlayerDialog(playerid, 9322, 5, title, body, mode == 2 ? "Dettagli" : "Seleziona", "Chiudi");
}

UI_FactionMembersShow(playerid, faction, ownRank, mode)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (faction < 1 || faction > 2 || ownRank < 1 || ownRank > 6)
	{
		return 0;
	}
	if (mode < 2 || mode > 6 || mode == 3)
	{
		mode = 2;
	}
	if (mode != 2 && ownRank < 5)
	{
		SendClientMessage(playerid, -10066177, "Non hai il grado necessario per questa azione.");
		return 0;
	}
	g_UiFcFaction[playerid] = faction;
	g_UiFcRank[playerid] = ownRank;
	g_UiFcMode[playerid] = mode;
	UI_FcMembersPage(playerid, 0);
}

UI_FcMemberInfoShow(playerid, idx)
{
	new faction = g_UiFcFaction[playerid];
	new rank = g_UiFcMemInfo[playerid][idx] & 255;
	new st = g_UiFcMemInfo[playerid][idx] >> 8 & 255;
	new body[384], name[25], rankName[24], status[24], pay[16];
	UI_FcMemberName(playerid, idx, name, 25);
	UI_FcRankText(faction, rank, rankName, 24);
	UI_FcStatusText(st, status, 24);
	UI_FcMoney(g_UiFcRankPay[faction][rank], pay, 16);
	format(body, 384, "{FFD24D}%s{FFFFFF}\n\nFazione: %s\nGrado: %s (%d/%d)\nStato: %s{FFFFFF}\nStipendio di grado: {66DD66}$%s{FFFFFF} ogni %d min di servizio", name, g_UiFcFull[faction], rankName, rank, 6, status, pay, 15);
	Lang_ShowPlayerDialog(playerid, 9323, 0, "Dettaglio membro", body, "Indietro", "");
}

UI_FcConfirmShow(playerid, idx)
{
	new faction = g_UiFcFaction[playerid], mode = g_UiFcMode[playerid];
	new rank = g_UiFcMemInfo[playerid][idx] & 255;
	new body[512], name[25], oldName[24], newName[24], verb[16];
	UI_FcMemberName(playerid, idx, name, 25);
	UI_FcRankText(faction, rank, oldName, 24);
	UI_FcActionVerb(mode, verb, 16);
	g_UiFcSel[playerid] = idx;
	switch (mode)
	{
		case 4:
		{
			UI_FcRankText(faction, rank + 1, newName, 24);
			format(body, 512, "{FFFFFF}Promuovere {FFD24D}%s{FFFFFF} da {FFD24D}%s{FFFFFF} a {66DD66}%s{FFFFFF}?", name, oldName, newName);
		}
		case 5:
		{
			UI_FcRankText(faction, rank - 1, newName, 24);
			format(body, 512, "{FFFFFF}Degradare {FFD24D}%s{FFFFFF} da {FFD24D}%s{FFFFFF} a {FF6666}%s{FFFFFF}?", name, oldName, newName);
		}
		default:
		{
			format(body, 512, "{FFFFFF}Licenziare {FFD24D}%s{FFFFFF} ({FFD24D}%s{FFFFFF}) da {FFD24D}%s{FFFFFF}?\n\n{FF6666}Perdera' fazione, grado e servizio. L'operazione non e' reversibile.", name, oldName, g_UiFcShort[faction]);
		}
	}
	Lang_ShowPlayerDialog(playerid, 9324, 0, verb, body, "Conferma", "Annulla");
}

UI_FactionInviteShow(playerid, faction, inviterName[], ttlSec)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (faction < 1 || faction > 2)
	{
		return 0;
	}
	new body[512], rank1[24];
	UI_FcRankText(faction, 1, rank1, 24);
	format(body, 512, "{FFD24D}%s{FFFFFF} ti invita a entrare in {FFD24D}%s{FFFFFF}.\n\nEntrerai con il grado {FFD24D}%s{FFFFFF}.\nL'invito scade tra {FF6666}%d{FFFFFF} secondi.", inviterName, g_UiFcFull[faction], rank1, ttlSec > 0 ? ttlSec : 60);
	Lang_ShowPlayerDialog(playerid, 9326, 0, "Invito in fazione", body, "Accetta", "Rifiuta");
}

UI_FactionInviteClose(playerid)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (GetPlayerDialogID(playerid) == 9326)
	{
		HidePlayerDialog(playerid);
	}
}

bool:UI_FactionOnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	switch (dialogid)
	{
		case 9320:
		{
			if (!response)
			{
				return 1;
			}
			if (listitem < 0 || listitem >= 8 || g_UiFcMenuAct[playerid][listitem] == 0)
			{
				UI_FactionMenuShow(playerid, g_UiFcFaction[playerid], g_UiFcRank[playerid], g_UiFcDuty[playerid], g_UiFcOnline[playerid], g_UiFcTotal[playerid]);
				return 1;
			}
			new act = g_UiFcMenuAct[playerid][listitem];
			if (act == 1)
			{
				UI_FcInfoShow(playerid);
			}
			else if (act == 3)
			{
				if (g_UiFcRank[playerid] < 5)
				{
					return 1;
				}
				UI_FcInviteTargetShow(playerid, "");
			}
			else
			{
				if (act != 2 && g_UiFcRank[playerid] < 5)
				{
					return 1;
				}
				UI_OnFactionMenuAction(playerid, act);
			}
			return 1;
		}
		case 9321:
		{
			if (response)
			{
				UI_FactionMenuShow(playerid, g_UiFcFaction[playerid], g_UiFcRank[playerid], g_UiFcDuty[playerid], g_UiFcOnline[playerid], g_UiFcTotal[playerid]);
			}
			return 1;
		}
		case 9322:
		{
			if (!response)
			{
				return 1;
			}
			new page = g_UiFcPage[playerid];
			new pages = UI_FcPagesCount(playerid);
			new start = page * 12;
			new n = g_UiFcCount[playerid] - start;
			if (n > 12)
			{
				n = 12;
			}
			if (listitem < 0)
			{
				UI_FcMembersPage(playerid, page);
				return 1;
			}
			if (listitem < n)
			{
				new idx = start + listitem;
				new mode = g_UiFcMode[playerid];
				if (mode == 2)
				{
					g_UiFcSel[playerid] = idx;
					UI_FcMemberInfoShow(playerid, idx);
					return 1;
				}
				new reason[64];
				reason[0] = 0;
				if (g_UiFcRank[playerid] < 5 || !UI_FcEligible(g_UiFcRank[playerid], mode, g_UiFcMemInfo[playerid][idx] & 255, reason, 64))
				{
					if (reason[0] != 0)
					{
						SendClientMessage(playerid, -10066177, reason);
					}
					UI_FcMembersPage(playerid, page);
					return 1;
				}
				UI_FcConfirmShow(playerid, idx);
				return 1;
			}
			new pos = listitem - n;
			if (page > 0)
			{
				if (pos == 0)
				{
					UI_FcMembersPage(playerid, page - 1);
					return 1;
				}
				pos--;
			}
			if (page < pages - 1 && pos == 0)
			{
				UI_FcMembersPage(playerid, page + 1);
				return 1;
			}
			UI_FcMembersPage(playerid, page);
			return 1;
		}
		case 9323:
		{
			if (response && g_UiFcCount[playerid] > 0)
			{
				UI_FcMembersPage(playerid, g_UiFcPage[playerid]);
			}
			return 1;
		}
		case 9324:
		{
			new idx = g_UiFcSel[playerid];
			new mode = g_UiFcMode[playerid];
			if (idx < 0 || idx >= g_UiFcCount[playerid])
			{
				return 1;
			}
			if (!response)
			{
				UI_FcMembersPage(playerid, g_UiFcPage[playerid]);
				return 1;
			}
			new reason[64];
			if (g_UiFcRank[playerid] < 5 || !UI_FcEligible(g_UiFcRank[playerid], mode, g_UiFcMemInfo[playerid][idx] & 255, reason, 64))
			{
				return 1;
			}
			new memberId = g_UiFcMemId[playerid][idx];
			g_UiFcSel[playerid] = -1;
			UI_OnFactionMemberAction(playerid, mode, memberId);
			return 1;
		}
		case 9325:
		{
			if (!response)
			{
				return 1;
			}
			if (g_UiFcRank[playerid] < 5)
			{
				return 1;
			}
			new len = strlen(inputtext), bool:digits = len >= 1 && len <= 4;
			for (new i = 0; digits && i < len; i++)
			{
				if (inputtext[i] < 48 || inputtext[i] > 57)
				{
					digits = 0;
				}
			}
			new target = digits ? strval(inputtext) : -1;
			if (target < 0 || target >= MAX_PLAYERS || !IsPlayerConnected(target))
			{
				UI_FcInviteTargetShow(playerid, "{FF4444}ID non valido o giocatore non connesso.");
				return 1;
			}
			if (target == playerid)
			{
				UI_FcInviteTargetShow(playerid, "{FF4444}Non puoi invitare te stesso.");
				return 1;
			}
			UI_OnFactionInviteTarget(playerid, target);
			return 1;
		}
		case 9326:
		{
			UI_OnFactionInviteReply(playerid, response ? 1 : 0);
			return 1;
		}
		case 9327:
		{
			return 1;
		}
	}
	return 0;
}

PlayerText:UI_FhText(playerid, Float:x, Float:y, text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
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

PlayerText:UI_FhBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
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
new g_UiFHud[MAX_PLAYERS][3];
new bool:g_UiFHudCreated[MAX_PLAYERS];
new bool:g_UiFHudShown[MAX_PLAYERS];
new bool:g_UiFHudKit[MAX_PLAYERS];

UI_FhCreate(playerid)
{
	if (g_UiFHudCreated[playerid])
	{
		return 0;
	}
	new Float:left = (476.0 - 150.0);
	g_UiFHud[playerid][0] = UI_FhBox(playerid, left, 356.0, 150.0, 24.0, 153);
	g_UiFHud[playerid][1] = UI_FhText(playerid, (5.0 + left), (356.0 + 2.0), "IN SERVIZIO", 0.2, 0.9, -1, 1);
	g_UiFHud[playerid][2] = UI_FhText(playerid, (5.0 + left), (356.0 + 12.0), "Kit medico: 0", 0.2, 0.9, -1, 1);
	g_UiFHudCreated[playerid] = 1;
}

UI_FhApply(playerid, faction, rank, kitCharges)
{
	new str[64], rankName[24];
	UI_FcRankText(faction, rank, rankName, 24);
	format(str, 64, "IN SERVIZIO - %s", rankName);
	Lang_PlayerTextDrawSetString(playerid, g_UiFHud[playerid][1], str);
	PlayerTextDrawColour(playerid, g_UiFHud[playerid][1], g_UiFcColor[faction]);
	PlayerTextDrawShow(playerid, g_UiFHud[playerid][1]);
	if (kitCharges >= 0)
	{
		format(str, 64, "Kit medico: %d/%d", kitCharges, 8);
		Lang_PlayerTextDrawSetString(playerid, g_UiFHud[playerid][2], str);
		PlayerTextDrawColour(playerid, g_UiFHud[playerid][2], kitCharges == 0 ? -10066177 : -1);
		PlayerTextDrawShow(playerid, g_UiFHud[playerid][2]);
		g_UiFHudKit[playerid] = 1;
	}
	else if (g_UiFHudKit[playerid])
	{
		PlayerTextDrawHide(playerid, g_UiFHud[playerid][2]);
		g_UiFHudKit[playerid] = 0;
	}
}

UI_FactionHudShow(playerid, faction, rank)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (faction < 1 || faction > 2 || rank < 1 || rank > 6)
	{
		return 0;
	}
	UI_FhCreate(playerid);
	g_UiFHudShown[playerid] = 1;
	g_UiFHudKit[playerid] = 0;
	PlayerTextDrawShow(playerid, g_UiFHud[playerid][0]);
	PlayerTextDrawHide(playerid, g_UiFHud[playerid][2]);
	UI_FhApply(playerid, faction, rank, -1);
}

UI_FactionHudUpdate(playerid, faction, rank, kitCharges)
{
	if (!UI_FcValid(playerid) || !g_UiFHudCreated[playerid] || !g_UiFHudShown[playerid])
	{
		return 0;
	}
	if (faction < 1 || faction > 2 || rank < 1 || rank > 6)
	{
		return 0;
	}
	UI_FhApply(playerid, faction, rank, kitCharges);
}

UI_FactionHudHide(playerid)
{
	if (!UI_FcValid(playerid) || !g_UiFHudCreated[playerid])
	{
		return 0;
	}
	g_UiFHudShown[playerid] = 0;
	g_UiFHudKit[playerid] = 0;
	for (new i = 0; i < 3; i++)
	{
		PlayerTextDrawHide(playerid, g_UiFHud[playerid][i]);
	}
}
new g_UiIHud[MAX_PLAYERS][6];
new bool:g_UiIHudCreated[MAX_PLAYERS];
new bool:g_UiIHudShown[MAX_PLAYERS];

UI_IhCreate(playerid)
{
	if (g_UiIHudCreated[playerid])
	{
		return 0;
	}
	g_UiIHud[playerid][0] = UI_FhBox(playerid, 210.0, 60.0, 220.0, 58.0, 170);
	g_UiIHud[playerid][1] = UI_FhText(playerid, (210.0 + 6.0), (60.0 + 2.0), "SEI FERITO", 0.3, 1.3, -12303105, 1);
	g_UiIHud[playerid][2] = UI_FhText(playerid, (210.0 + 6.0), (60.0 + 21.0), "Ricovero automatico tra 3:00", 0.22, 1.0, -1, 1);
	g_UiIHud[playerid][3] = UI_FhBox(playerid, (210.0 + 6.0), (60.0 + 35.0), 208.0, 5.0, 0x444444FF);
	g_UiIHud[playerid][4] = UI_FhBox(playerid, (210.0 + 6.0), (60.0 + 35.0), 208.0, 5.0, -12303105);
	g_UiIHud[playerid][5] = UI_FhText(playerid, (210.0 + 6.0), (60.0 + 43.0), "Attendi un medico EMS. Non puoi muoverti.", 0.18, 0.85, -0.005729198, 1);
	g_UiIHudCreated[playerid] = 1;
}

UI_InjuryHudUpdate(playerid, secondsLeft)
{
	if (!UI_FcValid(playerid) || !g_UiIHudCreated[playerid] || !g_UiIHudShown[playerid])
	{
		return 0;
	}
	if (secondsLeft < 0)
	{
		secondsLeft = 0;
	}
	if (secondsLeft > 180)
	{
		secondsLeft = 180;
	}
	new str[48];
	format(str, 48, "Ricovero automatico tra %d:%02d", secondsLeft / 60, secondsLeft % 60);
	Lang_PlayerTextDrawSetString(playerid, g_UiIHud[playerid][2], str);
	new bool:warn = secondsLeft <= 30;
	new bool:blinkOff = warn && GetTickCount() / 500 % 2 == 1;
	PlayerTextDrawColour(playerid, g_UiIHud[playerid][2], warn ? (blinkOff ? -12303292 : -12303105) : -1);
	PlayerTextDrawShow(playerid, g_UiIHud[playerid][2]);
	new Float:fraction = (float(secondsLeft) / float(180));
	PlayerTextDrawTextSize(playerid, g_UiIHud[playerid][4], (0.1 + ((210.0 + 6.0) + (208.0 * fraction))), 0.0);
	PlayerTextDrawShow(playerid, g_UiIHud[playerid][4]);
}

UI_InjuryHudShow(playerid, secondsLeft)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	UI_IhCreate(playerid);
	g_UiIHudShown[playerid] = 1;
	for (new i = 0; i < 6; i++)
	{
		PlayerTextDrawShow(playerid, g_UiIHud[playerid][i]);
	}
	UI_InjuryHudUpdate(playerid, secondsLeft);
}

UI_InjuryHudHide(playerid)
{
	if (!UI_FcValid(playerid) || !g_UiIHudCreated[playerid])
	{
		return 0;
	}
	g_UiIHudShown[playerid] = 0;
	for (new i = 0; i < 6; i++)
	{
		PlayerTextDrawHide(playerid, g_UiIHud[playerid][i]);
	}
}

UI_InjuryEndShow(playerid, reason, fee)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	UI_InjuryHudHide(playerid);
	new body[384], money[16];
	if (reason == 1)
	{
		if (fee > 0)
		{
			UI_FcMoney(fee, money, 16);
			format(body, 384, "{FFFFFF}Nessun soccorso e' arrivato in tempo: sei stato ricoverato all'ospedale di All Saints e ti sei ripreso.\n\nSpese di ricovero addebitate: {FF6666}$%s{FFFFFF}.", money);
		}
		else
		{
			format(body, 384, "{FFFFFF}Nessun soccorso e' arrivato in tempo: sei stato ricoverato all'ospedale di All Saints e ti sei ripreso.\n\nNessuna spesa addebitata (fondi insufficienti).");
		}
		Lang_ShowPlayerDialog(playerid, 9327, 0, "Ricovero in ospedale", body, "OK", "");
	}
	else
	{
		format(body, 384, "{FFFFFF}Un medico EMS ti ha soccorso: sei di nuovo in piedi.\nRiprendi il gioco con prudenza.");
		Lang_ShowPlayerDialog(playerid, 9327, 0, "Soccorso", body, "OK", "");
	}
}

UI_FactionUiDestroy(playerid)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (g_UiFHudCreated[playerid])
	{
		for (new i = 0; i < 3; i++)
		{
			PlayerTextDrawDestroy(playerid, g_UiFHud[playerid][i]);
			g_UiFHud[playerid][i] = 65535;
		}
	}
	if (g_UiIHudCreated[playerid])
	{
		for (new i = 0; i < 6; i++)
		{
			PlayerTextDrawDestroy(playerid, g_UiIHud[playerid][i]);
			g_UiIHud[playerid][i] = 65535;
		}
	}
	g_UiFHudCreated[playerid] = 0;
	g_UiFHudShown[playerid] = 0;
	g_UiFHudKit[playerid] = 0;
	g_UiIHudCreated[playerid] = 0;
	g_UiIHudShown[playerid] = 0;
	g_UiFcFaction[playerid] = 0;
	g_UiFcRank[playerid] = 0;
	g_UiFcDuty[playerid] = 0;
	g_UiFcOnline[playerid] = 0;
	g_UiFcTotal[playerid] = 0;
	g_UiFcMode[playerid] = 2;
	g_UiFcPage[playerid] = 0;
	g_UiFcSel[playerid] = -1;
	g_UiFcCount[playerid] = 0;
	for (new i = 0; i < 8; i++)
	{
		g_UiFcMenuAct[playerid][i] = 0;
	}
}
