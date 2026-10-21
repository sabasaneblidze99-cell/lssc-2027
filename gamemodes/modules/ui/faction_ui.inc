new g_UiFcShort[3][8] = {
	"-",
	"LSPD",
	"EMS"
};
new g_UiFcFull[3][48] = {
	"ფრაქციის გარეშე",
	"Los Santos Police Department",
	"Los Santos Emergency Medical Services"
};
new g_UiFcRankName[3][7][64] = {
	{"-", "-", "-", "-", "-", "-", "-"},
	{"-", "კადეტი", "ოფიცერი", "უფროსი ოფიცერი", "სერჟანტი", "ლეიტენანტი", "დეპარტამენტის უფროსი"},
	{"-", "მოხალისე", "მაშველი", "პარამედიკი", "ჯგუფის უფროსი", "ექიმი", "სამედიცინო დირექტორი"}
};
new g_UiFcRankPay[3][7] = {
	{0},
	{0, ECON_FAC_PAY_R1, ECON_FAC_PAY_R2, ECON_FAC_PAY_R3, ECON_FAC_PAY_R4, ECON_FAC_PAY_R5, ECON_FAC_PAY_R6},
	{0, ECON_FAC_PAY_R1, ECON_FAC_PAY_R2, ECON_FAC_PAY_R3, ECON_FAC_PAY_R4, ECON_FAC_PAY_R5, ECON_FAC_PAY_R6}
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
	return 0;
}

UI_FcMoney(value, out[], size)
{
	// importo in tetri -> "1.250,50" (economy/currency.inc)
	return Money_Format(value, out, size);
}

UI_FcStatusText(status, out[], size)
{
	switch (status)
	{
		case 2:
		{
			format(out, size, "{66DD66}სამსახურში");
		}
		case 1:
		{
			format(out, size, "{FFFFFF}ონლაინ");
		}
		default:
		{
			format(out, size, "{888888}ოფლაინ");
		}
	}
}

UI_FcActionVerb(action, out[], size)
{
	switch (action)
	{
		case 4:
		{
			format(out, size, "დაწინაურება");
		}
		case 5:
		{
			format(out, size, "დაქვეითება");
		}
		case 6:
		{
			format(out, size, "გათავისუფლება");
		}
		default:
		{
			format(out, size, "წევრები");
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
		SendClientMessage(playerid, -10066177, "არცერთ ფრაქციაში არ ხარ.");
		return 0;
	}
	g_UiFcFaction[playerid] = faction;
	g_UiFcRank[playerid] = rank;
	g_UiFcDuty[playerid] = onDuty;
	g_UiFcOnline[playerid] = membersOnline;
	g_UiFcTotal[playerid] = membersTotal;
	new body[3072], line[576], title[192], rankName[72];
	new n = 0;
	for (new i = 0; i < 8; i++)
	{
		g_UiFcMenuAct[playerid][i] = 0;
	}
	strcat(body, "ფრაქციის ინფორმაცია\n", 3072);
	g_UiFcMenuAct[playerid][n++] = 1;
	format(line, 576, "წევრების სია (%d ონლაინ / %d სულ)\n", membersOnline, membersTotal);
	strcat(body, line, 3072);
	g_UiFcMenuAct[playerid][n++] = 2;
	if (rank >= 5)
	{
		strcat(body, "{FFD24D}მოთამაშის მოწვევა\n{FFD24D}წევრის დაწინაურება\n{FFD24D}წევრის დაქვეითება\n{FFD24D}წევრის გათავისუფლება\n", 3072);
		g_UiFcMenuAct[playerid][n++] = 3;
		g_UiFcMenuAct[playerid][n++] = 4;
		g_UiFcMenuAct[playerid][n++] = 5;
		g_UiFcMenuAct[playerid][n] = 6;
	}
	UI_FcRankText(faction, rank, rankName, 72);
	format(title, 192, "%s - %s%s", g_UiFcShort[faction], rankName, onDuty ? " (სამსახურში)" : "");
	Lang_ShowPlayerDialog(playerid, 9320, DIALOG_STYLE_LIST, title, body, "არჩევა", "დახურვა");
	return 0;
}

UI_FcInfoShow(playerid)
{
	new faction = g_UiFcFaction[playerid], rank = g_UiFcRank[playerid];
	if (faction < 1 || faction > 2)
	{
		return 0;
	}
	new body[3072], line[576], rankName[72], pay[48];
	UI_FcRankText(faction, rank, rankName, 72);
	UI_FcMoney(g_UiFcRankPay[faction][rank], pay, 48);
	format(line, 576, "{FFD24D}%s{FFFFFF}\n\n", g_UiFcFull[faction]);
	strcat(body, line, 3072);
	format(line, 576, "წოდება: {FFFFFF}%s (%d/%d)\nსამსახური: %s{FFFFFF}\n", rankName, rank, 6, g_UiFcDuty[playerid] ? "{66DD66}სამსახურში" : "{FF6666}სამსახურს გარეთ");
	strcat(body, line, 3072);
	format(line, 576, "ხელფასი: {66DD66}₾%s{FFFFFF} ყოველ %d წთ რეალურ სამსახურზე (ირიცხება ბანკში)\nწევრები: %d ონლაინ / %d სულ\n\n", pay, 15, g_UiFcOnline[playerid], g_UiFcTotal[playerid]);
	strcat(body, line, 3072);
	strcat(body, "ბრძანებები: /duty (შტაბთან ახლოს), /r (რადიო), /dep (დეპარტამენტთაშორისი), /fazione lascia.", 3072);
	if (rank >= 5)
	{
		format(line, 576, "\nხელმძღვანელობა: /invita, /promuovi, /degrada, /licenzia (მხოლოდ შენზე დაბალ წოდებებზე; %d წოდებას მხოლოდ ადმინისტრაცია ანიჭებს).", 6);
		strcat(body, line, 3072);
	}
	Lang_ShowPlayerDialog(playerid, 9321, DIALOG_STYLE_MSGBOX, "ფრაქციის ინფორმაცია", body, "მენიუ", "დახურვა");
	return 0;
}

UI_FcInviteTargetShow(playerid, const error[])
{
	new body[768];
	format(body, 768, "{FFFFFF}შეიყვანე მოთამაშის ID, რომ მოიწვიო {FFD24D}%s{FFFFFF}-ში.\nუნდა იყოს ფრაქციის გარეშე, მინიმუმ %d დონის და %.0f მეტრზე ახლოს.\nმოწვევა %d წამი მოქმედებს.\n\n%s", g_UiFcShort[g_UiFcFaction[playerid]], 3, 5.0, 60, error);
	Lang_ShowPlayerDialog(playerid, 9325, DIALOG_STYLE_INPUT, "მოთამაშის მოწვევა", body, "მოწვევა", "გაუქმება");
}

UI_FactionMemberListClear(playerid)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	g_UiFcCount[playerid] = 0;
	g_UiFcSel[playerid] = -1;
	return 0;
}

bool:UI_FactionMemberListAdd(playerid, memberId, const name[], rank, status)
{
	if (!UI_FcValid(playerid))
	{
		return false;
	}
	new n = g_UiFcCount[playerid];
	if (n >= 32)
	{
		return false;
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
	return true;
}

UI_FcMemberName(playerid, idx, out[], size)
{
	strunpack(out, g_UiFcMemName[playerid][idx], size);
}

bool:UI_FcEligible(ownRank, action, rank, reason[], size)
{
	if (action == 2)
	{
		return true;
	}
	if (rank >= ownRank)
	{
		format(reason, size, "ამ მოთამაშის წოდება შენზე დაბალი არ არის.");
		return false;
	}
	if (action == 4 && rank + 1 >= ownRank)
	{
		format(reason, size, "ახალი წოდება შენზე დაბალი უნდა დარჩეს.");
		return false;
	}
	if (action == 5 && rank <= 1)
	{
		format(reason, size, "წევრი უკვე მინიმალურ წოდებაზეა.");
		return false;
	}
	return true;
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
	new title[576], verb[48];
	UI_FcActionVerb(mode, verb, 48);
	if (g_UiFcCount[playerid] == 0)
	{
		format(title, 576, "%s - %s", verb, g_UiFcShort[faction]);
		Lang_ShowPlayerDialog(playerid, 9323, DIALOG_STYLE_MSGBOX, title, "{FFFFFF}საჩვენებელი წევრები არ არის.", "დახურვა", "");
		return 0;
	}
	new body[3072], line[576], name[75], rankName[72], status[72], reason[24];
	strcat(body, "სახელი\tწოდება\tსტატუსი\n", 3072);
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
		UI_FcMemberName(playerid, idx, name, 75);
		UI_FcRankText(faction, rank, rankName, 72);
		if (UI_FcEligible(ownRank, mode, rank, reason, 24))
		{
			UI_FcStatusText(st, status, 72);
			format(line, 576, "{FFFFFF}%s\t{FFD24D}%s (%d)\t%s\n", name, rankName, rank, status);
		}
		else
		{
			format(line, 576, "{888888}%s\t%s (%d)\t{888888}ვერ აირჩევ\n", name, rankName, rank);
		}
		strcat(body, line, 3072);
	}
	if (page > 0)
	{
		strcat(body, "{FFD24D}<< წინა გვერდი\t\t\n", 3072);
	}
	if (page < pages - 1)
	{
		strcat(body, "{FFD24D}შემდეგი გვერდი >>\t\t\n", 3072);
	}
	if (mode == 2)
	{
		format(title, 576, "წევრები %s (გვ. %d/%d)", g_UiFcShort[faction], page + 1, pages);
	}
	else
	{
		format(title, 576, "%s წევრი - %s (გვ. %d/%d)", verb, g_UiFcShort[faction], page + 1, pages);
	}
	Lang_ShowPlayerDialog(playerid, 9322, DIALOG_STYLE_TABLIST_HEADERS, title, body, mode == 2 ? "დეტალები" : "არჩევა", "დახურვა");
	return 0;
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
		SendClientMessage(playerid, -10066177, "ამ მოქმედებისთვის საჭირო წოდება არ გაქვს.");
		return 0;
	}
	g_UiFcFaction[playerid] = faction;
	g_UiFcRank[playerid] = ownRank;
	g_UiFcMode[playerid] = mode;
	UI_FcMembersPage(playerid, 0);
	return 0;
}

UI_FcMemberInfoShow(playerid, idx)
{
	new faction = g_UiFcFaction[playerid];
	new rank = g_UiFcMemInfo[playerid][idx] & 255;
	new st = g_UiFcMemInfo[playerid][idx] >> 8 & 255;
	new body[768], name[75], rankName[72], status[72], pay[48];
	UI_FcMemberName(playerid, idx, name, 75);
	UI_FcRankText(faction, rank, rankName, 72);
	UI_FcStatusText(st, status, 72);
	UI_FcMoney(g_UiFcRankPay[faction][rank], pay, 48);
	format(body, 768, "{FFD24D}%s{FFFFFF}\n\nფრაქცია: %s\nწოდება: %s (%d/%d)\nსტატუსი: %s{FFFFFF}\nწოდების ხელფასი: {66DD66}₾%s{FFFFFF} ყოველ %d წთ სამსახურზე", name, g_UiFcFull[faction], rankName, rank, 6, status, pay, 15);
	Lang_ShowPlayerDialog(playerid, 9323, DIALOG_STYLE_MSGBOX, "წევრის დეტალები", body, "უკან", "");
}

UI_FcConfirmShow(playerid, idx)
{
	new faction = g_UiFcFaction[playerid], mode = g_UiFcMode[playerid];
	new rank = g_UiFcMemInfo[playerid][idx] & 255;
	new body[1024], name[75], oldName[72], newName[72], verb[48];
	UI_FcMemberName(playerid, idx, name, 75);
	UI_FcRankText(faction, rank, oldName, 72);
	UI_FcActionVerb(mode, verb, 48);
	g_UiFcSel[playerid] = idx;
	switch (mode)
	{
		case 4:
		{
			UI_FcRankText(faction, rank + 1, newName, 72);
			format(body, 1024, "{FFFFFF}დააწინაურო {FFD24D}%s{FFFFFF} {FFD24D}%s{FFFFFF}-დან {66DD66}%s{FFFFFF}-მდე?", name, oldName, newName);
		}
		case 5:
		{
			UI_FcRankText(faction, rank - 1, newName, 72);
			format(body, 1024, "{FFFFFF}დააქვეითო {FFD24D}%s{FFFFFF} {FFD24D}%s{FFFFFF}-დან {FF6666}%s{FFFFFF}-მდე?", name, oldName, newName);
		}
		default:
		{
			format(body, 1024, "{FFFFFF}გაათავისუფლო {FFD24D}%s{FFFFFF} ({FFD24D}%s{FFFFFF}) {FFD24D}%s{FFFFFF}-დან?\n\n{FF6666}დაკარგავს ფრაქციას, წოდებას და სამსახურს. ოპერაცია შეუქცევადია.", name, oldName, g_UiFcShort[faction]);
		}
	}
	Lang_ShowPlayerDialog(playerid, 9324, DIALOG_STYLE_MSGBOX, verb, body, "დადასტურება", "გაუქმება");
}

UI_FactionInviteShow(playerid, faction, const inviterName[], ttlSec)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	if (faction < 1 || faction > 2)
	{
		return 0;
	}
	new body[1024], rank1[72];
	UI_FcRankText(faction, 1, rank1, 72);
	format(body, 1024, "{FFD24D}%s{FFFFFF} გიწვევს {FFD24D}%s{FFFFFF}-ში.\n\nშეხვალ წოდებით {FFD24D}%s{FFFFFF}.\nმოწვევის ვადა {FF6666}%d{FFFFFF} წამში იწურება.", inviterName, g_UiFcFull[faction], rank1, ttlSec > 0 ? ttlSec : 60);
	Lang_ShowPlayerDialog(playerid, 9326, DIALOG_STYLE_MSGBOX, "ფრაქციაში მოწვევა", body, "მიღება", "უარყოფა");
	return 0;
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
	return 0;
}

bool:UI_FactionOnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
	if (!UI_FcValid(playerid))
	{
		return false;
	}
	switch (dialogid)
	{
		case 9320:
		{
			if (!response)
			{
				return true;
			}
			if (listitem < 0 || listitem >= 8 || g_UiFcMenuAct[playerid][listitem] == 0)
			{
				UI_FactionMenuShow(playerid, g_UiFcFaction[playerid], g_UiFcRank[playerid], g_UiFcDuty[playerid], g_UiFcOnline[playerid], g_UiFcTotal[playerid]);
				return true;
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
					return true;
				}
				UI_FcInviteTargetShow(playerid, "");
			}
			else
			{
				if (act != 2 && g_UiFcRank[playerid] < 5)
				{
					return true;
				}
				UI_OnFactionMenuAction(playerid, act);
			}
			return true;
		}
		case 9321:
		{
			if (response)
			{
				UI_FactionMenuShow(playerid, g_UiFcFaction[playerid], g_UiFcRank[playerid], g_UiFcDuty[playerid], g_UiFcOnline[playerid], g_UiFcTotal[playerid]);
			}
			return true;
		}
		case 9322:
		{
			if (!response)
			{
				return true;
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
				return true;
			}
			if (listitem < n)
			{
				new idx = start + listitem;
				new mode = g_UiFcMode[playerid];
				if (mode == 2)
				{
					g_UiFcSel[playerid] = idx;
					UI_FcMemberInfoShow(playerid, idx);
					return true;
				}
				new reason[192];
				reason[0] = 0;
				if (g_UiFcRank[playerid] < 5 || !UI_FcEligible(g_UiFcRank[playerid], mode, g_UiFcMemInfo[playerid][idx] & 255, reason, 192))
				{
					if (reason[0] != 0)
					{
						SendClientMessage(playerid, -10066177, reason);
					}
					UI_FcMembersPage(playerid, page);
					return true;
				}
				UI_FcConfirmShow(playerid, idx);
				return true;
			}
			new pos = listitem - n;
			if (page > 0)
			{
				if (pos == 0)
				{
					UI_FcMembersPage(playerid, page - 1);
					return true;
				}
				pos--;
			}
			if (page < pages - 1 && pos == 0)
			{
				UI_FcMembersPage(playerid, page + 1);
				return true;
			}
			UI_FcMembersPage(playerid, page);
			return true;
		}
		case 9323:
		{
			if (response && g_UiFcCount[playerid] > 0)
			{
				UI_FcMembersPage(playerid, g_UiFcPage[playerid]);
			}
			return true;
		}
		case 9324:
		{
			new idx = g_UiFcSel[playerid];
			new mode = g_UiFcMode[playerid];
			if (idx < 0 || idx >= g_UiFcCount[playerid])
			{
				return true;
			}
			if (!response)
			{
				UI_FcMembersPage(playerid, g_UiFcPage[playerid]);
				return true;
			}
			new reason[192];
			if (g_UiFcRank[playerid] < 5 || !UI_FcEligible(g_UiFcRank[playerid], mode, g_UiFcMemInfo[playerid][idx] & 255, reason, 192))
			{
				return true;
			}
			new memberId = g_UiFcMemId[playerid][idx];
			g_UiFcSel[playerid] = -1;
			UI_OnFactionMemberAction(playerid, mode, memberId);
			return true;
		}
		case 9325:
		{
			if (!response)
			{
				return true;
			}
			if (g_UiFcRank[playerid] < 5)
			{
				return true;
			}
			new len = strlen(inputtext), bool:digits = len >= 1 && len <= 4;
			for (new i = 0; digits && i < len; i++)
			{
				if (inputtext[i] < 48 || inputtext[i] > 57)
				{
					digits = false;
				}
			}
			new target = digits ? strval(inputtext) : -1;
			if (target < 0 || target >= MAX_PLAYERS || !IsPlayerConnected(target))
			{
				UI_FcInviteTargetShow(playerid, "{FF4444}არასწორი ID ან მოთამაშე არ არის ონლაინ.");
				return true;
			}
			if (target == playerid)
			{
				UI_FcInviteTargetShow(playerid, "{FF4444}საკუთარ თავს ვერ მოიწვევ.");
				return true;
			}
			UI_OnFactionInviteTarget(playerid, target);
			return true;
		}
		case 9326:
		{
			UI_OnFactionInviteReply(playerid, response ? 1 : 0);
			return true;
		}
		case 9327:
		{
			return true;
		}
	}
	return false;
}

PlayerText:UI_FhText(playerid, Float:x, Float:y, const text[], Float:lw, Float:lh, color, t_TEXT_DRAW_ALIGN:align)
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

PlayerText:UI_FhBox(playerid, Float:x, Float:y, Float:w, Float:h, color)
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
new PlayerText:g_UiFHud[MAX_PLAYERS][3];
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
	g_UiFHud[playerid][1] = UI_FhText(playerid, (5.0 + left), (356.0 + 2.0), "სამსახურში", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiFHud[playerid][2] = UI_FhText(playerid, (5.0 + left), (356.0 + 12.0), "სამედ. ნაკრები: 0", 0.2, 0.9, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiFHudCreated[playerid] = true;
	return 0;
}

UI_FhApply(playerid, faction, rank, kitCharges)
{
	new str[576], rankName[72];
	UI_FcRankText(faction, rank, rankName, 72);
	format(str, 576, "სამსახურში - %s", rankName);
	Lang_PlayerTextDrawSetString(playerid, g_UiFHud[playerid][1], str);
	PlayerTextDrawColour(playerid, PlayerText:g_UiFHud[playerid][1], g_UiFcColor[faction]);
	PlayerTextDrawShow(playerid, PlayerText:g_UiFHud[playerid][1]);
	if (kitCharges >= 0)
	{
		format(str, 576, "სამედ. ნაკრები: %d/%d", kitCharges, 8);
		Lang_PlayerTextDrawSetString(playerid, g_UiFHud[playerid][2], str);
		PlayerTextDrawColour(playerid, PlayerText:g_UiFHud[playerid][2], kitCharges == 0 ? -10066177 : -1);
		PlayerTextDrawShow(playerid, PlayerText:g_UiFHud[playerid][2]);
		g_UiFHudKit[playerid] = true;
	}
	else if (g_UiFHudKit[playerid])
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiFHud[playerid][2]);
		g_UiFHudKit[playerid] = false;
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
	g_UiFHudShown[playerid] = true;
	g_UiFHudKit[playerid] = false;
	PlayerTextDrawShow(playerid, PlayerText:g_UiFHud[playerid][0]);
	PlayerTextDrawHide(playerid, PlayerText:g_UiFHud[playerid][2]);
	UI_FhApply(playerid, faction, rank, -1);
	return 0;
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
	return 0;
}

UI_FactionHudHide(playerid)
{
	if (!UI_FcValid(playerid) || !g_UiFHudCreated[playerid])
	{
		return 0;
	}
	g_UiFHudShown[playerid] = false;
	g_UiFHudKit[playerid] = false;
	for (new i = 0; i < 3; i++)
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiFHud[playerid][i]);
	}
	return 0;
}
new PlayerText:g_UiIHud[MAX_PLAYERS][6];
new bool:g_UiIHudCreated[MAX_PLAYERS];
new bool:g_UiIHudShown[MAX_PLAYERS];

UI_IhCreate(playerid)
{
	if (g_UiIHudCreated[playerid])
	{
		return 0;
	}
	g_UiIHud[playerid][0] = UI_FhBox(playerid, 210.0, 60.0, 220.0, 58.0, 170);
	g_UiIHud[playerid][1] = UI_FhText(playerid, (210.0 + 6.0), (60.0 + 2.0), "დაჭრილი ხარ", 0.3, 1.3, -12303105, t_TEXT_DRAW_ALIGN:1);
	g_UiIHud[playerid][2] = UI_FhText(playerid, (210.0 + 6.0), (60.0 + 21.0), "ავტო-ჰოსპიტალიზაცია 3:00-ში", 0.22, 1.0, -1, t_TEXT_DRAW_ALIGN:1);
	g_UiIHud[playerid][3] = UI_FhBox(playerid, (210.0 + 6.0), (60.0 + 35.0), 208.0, 5.0, 0x444444FF);
	g_UiIHud[playerid][4] = UI_FhBox(playerid, (210.0 + 6.0), (60.0 + 35.0), 208.0, 5.0, -12303105);
	g_UiIHud[playerid][5] = UI_FhText(playerid, (210.0 + 6.0), (60.0 + 43.0), "დაელოდე EMS-ის ექიმს. მოძრაობა არ შეგიძლია.", 0.18, 0.85, _:-0.005729198, t_TEXT_DRAW_ALIGN:1);
	g_UiIHudCreated[playerid] = true;
	return 0;
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
	new str[432];
	format(str, 432, "ავტო-ჰოსპიტალიზაცია %d:%02d-ში", secondsLeft / 60, secondsLeft % 60);
	Lang_PlayerTextDrawSetString(playerid, g_UiIHud[playerid][2], str);
	new bool:warn = secondsLeft <= 30;
	new bool:blinkOff = warn && GetTickCount() / 500 % 2 == 1;
	PlayerTextDrawColour(playerid, PlayerText:g_UiIHud[playerid][2], warn ? (blinkOff ? -12303292 : -12303105) : -1);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIHud[playerid][2]);
	new Float:fraction = (float(secondsLeft) / float(180));
	PlayerTextDrawTextSize(playerid, PlayerText:g_UiIHud[playerid][4], (0.1 + ((210.0 + 6.0) + (208.0 * fraction))), 0.0);
	PlayerTextDrawShow(playerid, PlayerText:g_UiIHud[playerid][4]);
	return 0;
}

UI_InjuryHudShow(playerid, secondsLeft)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	UI_IhCreate(playerid);
	g_UiIHudShown[playerid] = true;
	for (new i = 0; i < 6; i++)
	{
		PlayerTextDrawShow(playerid, PlayerText:g_UiIHud[playerid][i]);
	}
	UI_InjuryHudUpdate(playerid, secondsLeft);
	return 0;
}

UI_InjuryHudHide(playerid)
{
	if (!UI_FcValid(playerid) || !g_UiIHudCreated[playerid])
	{
		return 0;
	}
	g_UiIHudShown[playerid] = false;
	for (new i = 0; i < 6; i++)
	{
		PlayerTextDrawHide(playerid, PlayerText:g_UiIHud[playerid][i]);
	}
	return 0;
}

UI_InjuryEndShow(playerid, reason, fee)
{
	if (!UI_FcValid(playerid))
	{
		return 0;
	}
	UI_InjuryHudHide(playerid);
	new body[768], money[48];
	if (reason == 1)
	{
		if (fee > 0)
		{
			UI_FcMoney(fee, money, 48);
			format(body, 768, "{FFFFFF}დახმარება დროულად არ მოვიდა: All Saints-ის საავადმყოფოში მოგათავსეს და გამოჯანმრთელდი.\n\nმკურნალობის ხარჯი: {FF6666}₾%s{FFFFFF}.", money);
		}
		else
		{
			format(body, 768, "{FFFFFF}დახმარება დროულად არ მოვიდა: All Saints-ის საავადმყოფოში მოგათავსეს და გამოჯანმრთელდი.\n\nხარჯი არ ჩამოგეჭრა (სახსრები არ გყოფნის).");
		}
		Lang_ShowPlayerDialog(playerid, 9327, DIALOG_STYLE_MSGBOX, "საავადმყოფოში მოთავსება", body, "OK", "");
	}
	else
	{
		format(body, 768, "{FFFFFF}EMS-ის ექიმი დაგეხმარა: ისევ ფეხზე ხარ.\nთამაში ფრთხილად განაგრძე.");
		Lang_ShowPlayerDialog(playerid, 9327, DIALOG_STYLE_MSGBOX, "დახმარება", body, "OK", "");
	}
	return 0;
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
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiFHud[playerid][i]);
			g_UiFHud[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
		}
	}
	if (g_UiIHudCreated[playerid])
	{
		for (new i = 0; i < 6; i++)
		{
			PlayerTextDrawDestroy(playerid, PlayerText:g_UiIHud[playerid][i]);
			g_UiIHud[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
		}
	}
	g_UiFHudCreated[playerid] = false;
	g_UiFHudShown[playerid] = false;
	g_UiFHudKit[playerid] = false;
	g_UiIHudCreated[playerid] = false;
	g_UiIHudShown[playerid] = false;
	g_UiFcFaction[playerid] = 0;
	g_UiFcRank[playerid] = 0;
	g_UiFcDuty[playerid] = false;
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
	return 0;
}
