// =============================================================================
// character/char_select.inc - M15: tre personaggi per account.
//
// Dopo il login: selezione (CEF vista "chars" oppure dialog 9005) con i 3 slot.
//   personaggio esistente -> DB_LoadCharacter(slot) -> nome RP (SetPlayerName) -> scelta dello spawn
//   slot vuoto            -> nome e cognome (9006) -> data di nascita (9007) -> genere/aspetto/storia gia' esistenti
// Il nome e' "Nome_Cognome" latino, unico fra i personaggi e diverso dagli username degli altri account.
// Data di nascita RP: eta' 18-80 anni. Tutto e' rivalidato dal server (anche cio' che arriva dalla pagina CEF).
// Colonne usate: characters.char_name/first_name/last_name/birth_date/gender (migrazione 012).
// Se la lettura della lista non arriva entro 5 s si torna al comportamento precedente (slot 0).
// =============================================================================

#define CS_SLOTS            (3)
#define CS_DIALOG_LIST      (9005)
#define CS_DIALOG_NAME      (9006)
#define CS_DIALOG_BIRTH     (9007)
#define CS_STATE_LIST       (17)      // lista personaggi mostrata
#define CS_STATE_NEW        (18)      // creazione: nome / nascita
#define CS_LIST_TIMEOUT_MS  (5000)
#define CS_AGE_MIN          (18)
#define CS_AGE_MAX          (80)

new g_CsId[MAX_PLAYERS][CS_SLOTS];
new g_CsName[MAX_PLAYERS][CS_SLOTS][25];
new g_CsLevel[MAX_PLAYERS][CS_SLOTS];
new g_CsCash[MAX_PLAYERS][CS_SLOTS];
new g_CsAway[MAX_PLAYERS][CS_SLOTS];
new g_CsSkin[MAX_PLAYERS][CS_SLOTS];
new g_CsSlot[MAX_PLAYERS];                 // slot scelto (0..2)
new g_CsNewName[MAX_PLAYERS][25];          // nome del nuovo personaggio (vuoto = nome del launcher)
new g_CsFirst[MAX_PLAYERS][21];
new g_CsLast[MAX_PLAYERS][21];
new g_CsBirth[MAX_PLAYERS][11];            // "YYYY-MM-DD"
new g_CsLoadedName[MAX_PLAYERS][25];       // char_name letto con il personaggio
new g_CsToken[MAX_PLAYERS];
new g_CsTimer[MAX_PLAYERS];
new bool:g_CsBusy[MAX_PLAYERS];            // controllo del nome in corso
new bool:g_CsListed[MAX_PLAYERS];
new bool:g_CsCef[MAX_PLAYERS];             // creazione avviata dalla pagina CEF

CharSel_OnPlayerConnect(playerid)
{
	g_CsSlot[playerid] = 0;
	g_CsNewName[playerid][0] = 0;
	g_CsFirst[playerid][0] = 0;
	g_CsLast[playerid][0] = 0;
	g_CsBirth[playerid][0] = 0;
	g_CsLoadedName[playerid][0] = 0;
	g_CsBusy[playerid] = false;
	g_CsListed[playerid] = false;
	g_CsCef[playerid] = false;
	g_CsToken[playerid]++;
	CharSel_StopTimer(playerid);
}

CharSel_OnPlayerDisconnect(playerid)
{
	g_CsToken[playerid]++;
	CharSel_StopTimer(playerid);
}

CharSel_StopTimer(playerid)
{
	if (g_CsTimer[playerid] != 0)
	{
		KillTimer(g_CsTimer[playerid]);
		g_CsTimer[playerid] = 0;
	}
}

stock bool:CharSel_Active(playerid)
{
	new st = gPlayer[playerid][pAuthState];
	return st == CS_STATE_LIST || st == CS_STATE_NEW;
}

// slot del personaggio in caricamento/creazione (db_character.inc)
stock CharSel_Slot(playerid)
{
	return g_CsSlot[playerid] >= 0 && g_CsSlot[playerid] < CS_SLOTS ? g_CsSlot[playerid] : 0;
}

// ---------------------------------------------------------------- lista
// al posto di DB_LoadCharacter in Account_CompleteLogin
CharSel_Begin(playerid)
{
	gPlayer[playerid][pAuthState] = CS_STATE_LIST;
	g_CsListed[playerid] = false;
	g_CsToken[playerid]++;
	for (new s = 0; s < CS_SLOTS; s++)
	{
		g_CsId[playerid][s] = 0;
		g_CsName[playerid][s][0] = 0;
	}
	new query[400];
	mysql_format(g_DB, query, sizeof query,
		"SELECT slot, id, COALESCE(char_name, '') AS cn, level, money + bank AS cash, skin, COALESCE(TIMESTAMPDIFF(SECOND, last_save, NOW()), -1) AS away FROM characters WHERE account_id = %d AND slot < %d ORDER BY slot",
		gPlayer[playerid][pAccountId], CS_SLOTS);
	mysql_tquery(g_DB, query, "CharSel_OnList", "ddd", playerid, gPlayer[playerid][pAccountId], g_CsToken[playerid]);
	CharSel_StopTimer(playerid);
	g_CsTimer[playerid] = SetTimerEx("CharSel_OnListTimeout", CS_LIST_TIMEOUT_MS, false, "dd", playerid, g_CsToken[playerid]);
	return 1;
}

public CharSel_OnList(playerid, accountId, token)
{
	if (!IsPlayerConnected(playerid) || g_CsToken[playerid] != token || gPlayer[playerid][pAccountId] != accountId ||
		gPlayer[playerid][pAuthState] != CS_STATE_LIST)
	{
		return 1;
	}
	CharSel_StopTimer(playerid);
	new rows = 0, slot, count = 0;
	cache_get_row_count(rows);
	for (new r = 0; r < rows; r++)
	{
		cache_get_value_name_int(r, "slot", slot);
		if (slot < 0 || slot >= CS_SLOTS)
		{
			continue;
		}
		cache_get_value_name_int(r, "id", g_CsId[playerid][slot]);
		cache_get_value_name(r, "cn", g_CsName[playerid][slot], 25);
		cache_get_value_name_int(r, "level", g_CsLevel[playerid][slot]);
		cache_get_value_name_int(r, "cash", g_CsCash[playerid][slot]);
		cache_get_value_name_int(r, "skin", g_CsSkin[playerid][slot]);
		cache_get_value_name_int(r, "away", g_CsAway[playerid][slot]);
		if (g_CsName[playerid][slot][0] == 0)
		{
			GetPlayerName(playerid, g_CsName[playerid][slot], 25);   // personaggio legacy: nome del launcher
		}
		count++;
	}
	g_CsListed[playerid] = true;
	if (count == 0)
	{
		CharSel_StartNew(playerid, 0);        // account nuovo: si crea subito il primo personaggio
		return 1;
	}
	CharSel_ShowList(playerid);
	return 1;
}

// lettura non arrivata: comportamento precedente (un solo personaggio, slot 0)
public CharSel_OnListTimeout(playerid, token)
{
	if (!IsPlayerConnected(playerid) || g_CsToken[playerid] != token || gPlayer[playerid][pAuthState] != CS_STATE_LIST)
	{
		return 1;
	}
	g_CsTimer[playerid] = 0;
	g_CsToken[playerid]++;
	print("[CHAR] lista personaggi non disponibile: caricamento dello slot 0");
	g_CsSlot[playerid] = 0;
	gPlayer[playerid][pAuthState] = 4;
	DB_LoadCharacter(playerid, gPlayer[playerid][pAccountId]);
	return 1;
}

CharSel_AwayText(seconds, out[], size)
{
	out[0] = 0;
	if (seconds < 0)
	{
		return 0;
	}
	if (seconds < 3600)
	{
		format(out, size, "%d წთ წინ", seconds / 60 > 0 ? seconds / 60 : 1);
	}
	else if (seconds < 86400)
	{
		format(out, size, "%d სთ წინ", seconds / 3600);
	}
	else
	{
		format(out, size, "%d დღის წინ", seconds / 86400);
	}
	return 1;
}

CharSel_ShowList(playerid)
{
	gPlayer[playerid][pAuthState] = CS_STATE_LIST;
	Char_SetLoginCamera(playerid);
	new away[48];
	if (CefUI_Active(playerid))
	{
		cef_emit_event(playerid, "lssc:chars_begin", 1, CS_SLOTS);
		for (new s = 0; s < CS_SLOTS; s++)
		{
			CharSel_AwayText(g_CsAway[playerid][s], away, sizeof away);
			cef_emit_event(playerid, "lssc:chars_add", 1, s, 1, g_CsId[playerid][s] > 0 ? 1 : 0, 0, g_CsName[playerid][s],
				1, g_CsLevel[playerid][s], 1, g_CsCash[playerid][s], 1, g_CsSkin[playerid][s], 0, away);
		}
		CefUI_Open(playerid, "chars", "");
		return 1;
	}
	new body[1024], line[256];
	strcat(body, "სლოტი\tპერსონაჟი\tდონე\n");
	for (new s = 0; s < CS_SLOTS; s++)
	{
		if (g_CsId[playerid][s] > 0)
		{
			CharSel_AwayText(g_CsAway[playerid][s], away, sizeof away);
			format(line, sizeof line, "%d\t{FFFFFF}%s\t{AAAAAA}%d - %s\n", s + 1, g_CsName[playerid][s], g_CsLevel[playerid][s], away);
		}
		else
		{
			format(line, sizeof line, "%d\t{66DD66}+ ახალი პერსონაჟი\t \n", s + 1);
		}
		strcat(body, line);
	}
	Lang_ShowPlayerDialog(playerid, CS_DIALOG_LIST, DIALOG_STYLE_TABLIST_HEADERS, "აირჩიე პერსონაჟი", body, "არჩევა", "გასვლა");
	return 1;
}

CharSel_Pick(playerid, slot)
{
	if (gPlayer[playerid][pAuthState] != CS_STATE_LIST || !g_CsListed[playerid] || slot < 0 || slot >= CS_SLOTS)
	{
		return 0;
	}
	if (g_CsId[playerid][slot] <= 0)
	{
		CharSel_StartNew(playerid, slot);
		return 1;
	}
	g_CsSlot[playerid] = slot;
	g_CsNewName[playerid][0] = 0;
	gPlayer[playerid][pAuthState] = 4;
	if (CefUI_Active(playerid))
	{
		CefUI_Close(playerid, "chars");
	}
	DB_LoadCharacter(playerid, gPlayer[playerid][pAccountId]);
	return 1;
}

// ---------------------------------------------------------------- creazione
CharSel_StartNew(playerid, slot)
{
	g_CsSlot[playerid] = slot;
	g_CsNewName[playerid][0] = 0;
	g_CsBirth[playerid][0] = 0;
	g_CsBusy[playerid] = false;
	gPlayer[playerid][pAuthState] = CS_STATE_NEW;
	if (CefUI_Active(playerid))
	{
		g_CsCef[playerid] = true;
		if (CharSel_CountChars(playerid) == 0)
		{
			// account nuovo: la pagina non ha ancora la lista, si manda con 3 slot vuoti
			cef_emit_event(playerid, "lssc:chars_begin", 1, CS_SLOTS);
			for (new s = 0; s < CS_SLOTS; s++)
			{
				cef_emit_event(playerid, "lssc:chars_add", 1, s, 1, 0, 0, "", 1, 0, 1, 0, 1, 0, 0, "");
			}
			CefUI_Open(playerid, "chars", "");
		}
		cef_emit_event(playerid, "lssc:chars_new", 1, slot);   // la pagina mostra il modulo nome + nascita
		return 1;
	}
	g_CsCef[playerid] = false;
	CharSel_ShowNameDialog(playerid, "");
	return 1;
}

CharSel_CountChars(playerid)
{
	new n = 0;
	for (new s = 0; s < CS_SLOTS; s++)
	{
		if (g_CsId[playerid][s] > 0)
		{
			n++;
		}
	}
	return n;
}

CharSel_ShowNameDialog(playerid, const err[])
{
	new body[1024], launcher[25];
	GetPlayerName(playerid, launcher, sizeof launcher);
	format(body, sizeof body,
		"%s%s{FFFFFF}შეიყვანე პერსონაჟის სახელი და გვარი ფორმატში {66DD66}Saxeli_Gvari{FFFFFF} (მაგ. Giorgi_Beridze).\n\n{AAAAAA}მხოლოდ ლათინური ასოები, ორივე დიდი ასოთი იწყება.",
		err, err[0] ? "\n\n" : "");
	if (CharSel_CountChars(playerid) == 0)
	{
		new extra[160];
		format(extra, sizeof extra, "\n{AAAAAA}ცარიელი ველი = %s", launcher);
		strcat(body, extra);
	}
	Lang_ShowPlayerDialog(playerid, CS_DIALOG_NAME, DIALOG_STYLE_INPUT, "ახალი პერსონაჟი - სახელი", body, "შემდეგი", "უკან");
}

CharSel_ShowBirthDialog(playerid, const err[])
{
	new body[768];
	format(body, sizeof body, "%s%s{FFFFFF}დაბადების თარიღი ფორმატში {66DD66}DD/MM/YYYY{FFFFFF} (მაგ. 14/05/1998).\n\n{AAAAAA}ასაკი: %d-დან %d წლამდე.",
		err, err[0] ? "\n\n" : "", CS_AGE_MIN, CS_AGE_MAX);
	Lang_ShowPlayerDialog(playerid, CS_DIALOG_BIRTH, DIALOG_STYLE_INPUT, "ახალი პერსონაჟი - დაბადება", body, "შემდეგი", "უკან");
}

// errore verso la pagina CEF o il dialog: 1 formato nome, 2 nome occupato, 3 data non valida, 4 riprova
CharSel_Error(playerid, code)
{
	if (g_CsCef[playerid] && CefUI_Active(playerid))
	{
		cef_emit_event(playerid, "lssc:chars_err", 1, code);
		return 1;
	}
	switch (code)
	{
		case 1: CharSel_ShowNameDialog(playerid, "{FF6347}სახელი არასწორია.");
		case 2: CharSel_ShowNameDialog(playerid, "{FF6347}ეს სახელი უკვე დაკავებულია.");
		case 3: CharSel_ShowBirthDialog(playerid, "{FF6347}თარიღი არასწორია.");
		default: CharSel_ShowNameDialog(playerid, "{FF6347}სცადე თავიდან.");
	}
	return 1;
}

bool:CharSel_LeapYear(y)
{
	return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

// giorno/mese/anno -> "YYYY-MM-DD" se la data esiste e l'eta' e' 18-80
bool:CharSel_SetBirth(playerid, d, m, y)
{
	new dim[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (m < 1 || m > 12 || d < 1)
	{
		return false;
	}
	new maxd = dim[m - 1] + ((m == 2 && CharSel_LeapYear(y)) ? 1 : 0);
	if (d > maxd)
	{
		return false;
	}
	new cy, cm, cd;
	getdate(cy, cm, cd);
	new age = cy - y;
	if (cm < m || (cm == m && cd < d))
	{
		age--;
	}
	if (age < CS_AGE_MIN || age > CS_AGE_MAX)
	{
		return false;
	}
	format(g_CsBirth[playerid], 11, "%04d-%02d-%02d", y, m, d);
	return true;
}

bool:CharSel_ParseBirth(playerid, const text[])
{
	new d = 0, m = 0, y = 0, part = 0, digits = 0;
	for (new i = 0; text[i] != 0; i++)
	{
		new c = text[i];
		if (c >= '0' && c <= '9')
		{
			digits++;
			if (part == 0) d = d * 10 + c - '0';
			else if (part == 1) m = m * 10 + c - '0';
			else if (part == 2) y = y * 10 + c - '0';
			if (digits > 8)
			{
				return false;
			}
		}
		else if (c == '/' || c == '.' || c == '-')
		{
			part++;
			if (part > 2)
			{
				return false;
			}
		}
		else if (c != ' ')
		{
			return false;
		}
	}
	return part == 2 && y >= 1000 && CharSel_SetBirth(playerid, d, m, y);
}

// nome proposto: formato, poi unicita' sul DB (personaggi e username degli altri account)
CharSel_SubmitName(playerid, const name[])
{
	if (gPlayer[playerid][pAuthState] != CS_STATE_NEW || g_CsBusy[playerid])
	{
		return 0;
	}
	new tmp[25];
	strcat(tmp, name, sizeof tmp);
	if (strlen(name) > 20 || !Util_IsValidRPName(tmp))
	{
		CharSel_Error(playerid, 1);
		return 0;
	}
	g_CsBusy[playerid] = true;
	g_CsNewName[playerid][0] = 0;
	strcat(g_CsNewName[playerid], tmp, 25);
	new query[320];
	mysql_format(g_DB, query, sizeof query,
		"SELECT (SELECT COUNT(*) FROM characters WHERE char_name = '%e') + (SELECT COUNT(*) FROM accounts WHERE name = '%e' AND id <> %d) AS taken",
		tmp, tmp, gPlayer[playerid][pAccountId]);
	mysql_tquery(g_DB, query, "CharSel_OnNameChecked", "dd", playerid, g_CsToken[playerid]);
	return 1;
}

public CharSel_OnNameChecked(playerid, token)
{
	if (!IsPlayerConnected(playerid) || g_CsToken[playerid] != token || gPlayer[playerid][pAuthState] != CS_STATE_NEW)
	{
		return 1;
	}
	g_CsBusy[playerid] = false;
	new rows = 0, taken = 1;
	cache_get_row_count(rows);
	if (rows > 0)
	{
		cache_get_value_name_int(0, "taken", taken);
	}
	if (taken != 0)
	{
		g_CsNewName[playerid][0] = 0;
		CharSel_Error(playerid, 2);
		return 1;
	}
	// un altro giocatore online con lo stesso nome (launcher): SetPlayerName fallirebbe
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (i != playerid && IsPlayerConnected(i))
		{
			new other[25];
			GetPlayerName(i, other, sizeof other);
			if (strcmp(other, g_CsNewName[playerid], true) == 0)
			{
				g_CsNewName[playerid][0] = 0;
				CharSel_Error(playerid, 2);
				return 1;
			}
		}
	}
	if (g_CsCef[playerid] && g_CsBirth[playerid][0] != 0)
	{
		CharSel_Finish(playerid);        // dalla pagina arrivano nome e data insieme
		return 1;
	}
	CharSel_ShowBirthDialog(playerid, "");
	return 1;
}

// nome e data validi: nome RP applicato, poi il percorso esistente (genere -> aspetto -> storia -> intro)
CharSel_Finish(playerid)
{
	new cur[25], us;
	GetPlayerName(playerid, cur, sizeof cur);
	if (strcmp(cur, g_CsNewName[playerid], false) != 0 && SetPlayerName(playerid, g_CsNewName[playerid]) != 1)
	{
		g_CsNewName[playerid][0] = 0;
		CharSel_Error(playerid, 2);
		return 0;
	}
	us = strfind(g_CsNewName[playerid], "_");
	g_CsFirst[playerid][0] = 0;
	g_CsLast[playerid][0] = 0;
	strmid(g_CsFirst[playerid], g_CsNewName[playerid], 0, us, 21);
	strmid(g_CsLast[playerid], g_CsNewName[playerid], us + 1, strlen(g_CsNewName[playerid]), 21);
	if (g_CsCef[playerid] && CefUI_Active(playerid))
	{
		CefUI_Close(playerid, "chars");
	}
	gPlayer[playerid][pAuthState] = 5;
	Intro_OnNoCharacter(playerid);
	Account_ShowPrompt(playerid);   // genere (CEF o dialog 9003)
	return 1;
}

// nome RP del personaggio caricato (Char_OnDBLoaded): prima di qualunque altra query del personaggio
bool:CharSel_ApplyLoadedName(playerid)
{
	if (g_CsLoadedName[playerid][0] == 0)
	{
		return true;               // legacy: resta il nome del launcher
	}
	new cur[25];
	GetPlayerName(playerid, cur, sizeof cur);
	if (strcmp(cur, g_CsLoadedName[playerid], false) == 0)
	{
		return true;
	}
	return SetPlayerName(playerid, g_CsLoadedName[playerid]) == 1;
}

// usate da database/db_character.inc (incluso prima di questo file)
CharSel_SetLoadedName(playerid, const name[])
{
	g_CsLoadedName[playerid][0] = 0;
	if (Util_IsValidRPNameC(name))
	{
		strcat(g_CsLoadedName[playerid], name, 25);
	}
}

CharSel_Identity(playerid, name[], first[], last[], birth[], size = sizeof name)
{
	name[0] = 0;
	first[0] = 0;
	last[0] = 0;
	birth[0] = 0;
	if (g_CsNewName[playerid][0] == 0)
	{
		GetPlayerName(playerid, name, size);
		new us = strfind(name, "_");
		if (us > 0)
		{
			strmid(first, name, 0, us, 21);
			strmid(last, name, us + 1, strlen(name), 21);
		}
		return 0;
	}
	strcat(name, g_CsNewName[playerid], size);
	strcat(first, g_CsFirst[playerid], 21);
	strcat(last, g_CsLast[playerid], 21);
	strcat(birth, g_CsBirth[playerid], 11);
	return 1;
}

// ---------------------------------------------------------------- dialog e pagina
bool:CharSel_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (dialogid != CS_DIALOG_LIST && dialogid != CS_DIALOG_NAME && dialogid != CS_DIALOG_BIRTH)
	{
		return false;
	}
	switch (dialogid)
	{
		case CS_DIALOG_LIST:
		{
			if (gPlayer[playerid][pAuthState] != CS_STATE_LIST)
			{
				return true;
			}
			if (!response)
			{
				Account_Kick(playerid, "ნახვამდის!");
				return true;
			}
			CharSel_Pick(playerid, listitem);
		}
		case CS_DIALOG_NAME:
		{
			if (gPlayer[playerid][pAuthState] != CS_STATE_NEW)
			{
				return true;
			}
			if (!response)
			{
				if (CharSel_CountChars(playerid) > 0)
				{
					CharSel_ShowList(playerid);
				}
				else
				{
					CharSel_ShowNameDialog(playerid, "");
				}
				return true;
			}
			if (inputtext[0] == 0 && CharSel_CountChars(playerid) == 0)
			{
				new launcher[25];
				GetPlayerName(playerid, launcher, sizeof launcher);
				CharSel_SubmitName(playerid, launcher);
				return true;
			}
			CharSel_SubmitName(playerid, inputtext);
		}
		case CS_DIALOG_BIRTH:
		{
			if (gPlayer[playerid][pAuthState] != CS_STATE_NEW || g_CsNewName[playerid][0] == 0)
			{
				return true;
			}
			if (!response)
			{
				g_CsNewName[playerid][0] = 0;
				CharSel_ShowNameDialog(playerid, "");
				return true;
			}
			if (!CharSel_ParseBirth(playerid, inputtext))
			{
				CharSel_ShowBirthDialog(playerid, "{FF6347}თარიღი არასწორია.");
				return true;
			}
			CharSel_Finish(playerid);
		}
	}
	return true;
}

public Cef_OnCharPick(playerid, arguments[])
{
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid) || gPlayer[playerid][pAuthState] != CS_STATE_LIST)
	{
		return 1;
	}
	CharSel_Pick(playerid, CefBridge_Arg(arguments, 0, -1));
	return 1;
}

// lssc:char_create slot nome cognome giorno mese anno (nomi latini: nessuno spazio)
public Cef_OnCharCreate(playerid, arguments[])
{
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid))
	{
		return 1;
	}
	new st = gPlayer[playerid][pAuthState];
	new slot = CefBridge_Arg(arguments, 0, -1);
	if (slot < 0 || slot >= CS_SLOTS || g_CsBusy[playerid])
	{
		return 1;
	}
	if (st == CS_STATE_LIST && g_CsListed[playerid] && g_CsId[playerid][slot] <= 0)
	{
		g_CsSlot[playerid] = slot;
		gPlayer[playerid][pAuthState] = CS_STATE_NEW;
		st = CS_STATE_NEW;
	}
	if (st != CS_STATE_NEW || slot != g_CsSlot[playerid])
	{
		return 1;
	}
	g_CsCef[playerid] = true;
	new pos = 0, first[24], last[24], name[48];
	Util_NextToken(arguments, pos, first, 2);       // salta lo slot
	Util_NextToken(arguments, pos, first, sizeof first);
	Util_NextToken(arguments, pos, last, sizeof last);
	new d = CefBridge_Arg(arguments, 3, 0), m = CefBridge_Arg(arguments, 4, 0), y = CefBridge_Arg(arguments, 5, 0);
	g_CsBirth[playerid][0] = 0;
	if (!CharSel_SetBirth(playerid, d, m, y))
	{
		CharSel_Error(playerid, 3);
		return 1;
	}
	format(name, sizeof name, "%s_%s", first, last);
	CharSel_SubmitName(playerid, name);
	return 1;
}

public Cef_OnCharBack(playerid, arguments[])
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][pAuthState] != CS_STATE_NEW || g_CsBusy[playerid])
	{
		return 1;
	}
	if (CharSel_CountChars(playerid) > 0)
	{
		g_CsBirth[playerid][0] = 0;
		gPlayer[playerid][pAuthState] = CS_STATE_LIST;
	}
	return 1;
}
