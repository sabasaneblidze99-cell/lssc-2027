// =============================================================================
// core/chat.inc - adapter della chat (HUD Glass UI)
//
// Tutti i messaggi al giocatore passano da Chat_Line():
//   - giocatore con pagina CEF e chat CEF attiva -> evento "lssc:chat" (testo UTF-8 originale, anche georgiano,
//     con il colore SA-MP come prefisso {RRGGBB}); la chat di SA-MP non riceve nulla (nessun doppione);
//   - tutti gli altri -> SendClientMessage nativo, con il georgiano traslitterato in latino (core/lang.inc).
// SendClientMessage e' ridefinito qui sotto: il codice esistente non cambia.
// Categorie (cef/assets/hud/js/events.js): 0 sistema, 1 IC, 2 OOC, 3 fazione, 4 lavoro, 5 annunci, 6 emergenze, 7 admin.
// =============================================================================

#define CHAT_CAT_AUTO      (-1)
#define CHAT_CAT_SYSTEM    (0)
#define CHAT_CAT_IC        (1)
#define CHAT_CAT_OOC       (2)
#define CHAT_CAT_FACTION   (3)
#define CHAT_CAT_JOB       (4)
#define CHAT_CAT_ADS       (5)
#define CHAT_CAT_EMERGENCY (6)
#define CHAT_CAT_ADMIN     (7)

#define CHAT_LINE_MAX      (512)

native bool:Chat_RawSCM(playerid, colour, const format[], OPEN_MP_TAGS:...) = SendClientMessage;

new bool:g_ChatCef[MAX_PLAYERS];      // chat CEF attiva per il giocatore (decisa da ui/hud_cef.inc)
new g_ChatNextCat = CHAT_CAT_AUTO;    // categoria esplicita per i prossimi messaggi (Chat_SetCategory)

// Categoria esplicita per i messaggi successivi; CHAT_CAT_AUTO = riconoscimento dal testo
stock Chat_SetCategory(cat)
{
	g_ChatNextCat = cat;
}

stock Chat_SetCef(playerid, bool:on)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_ChatCef[playerid] = on;
	return 1;
}

stock bool:Chat_IsCef(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && g_ChatCef[playerid];
}

// salta i codici colore iniziali {RRGGBB}
stock Chat_TextStart(const text[])
{
	new i = 0;
	while (text[i] == '{' && text[i + 7] == '}')
	{
		i += 8;
	}
	return i;
}

stock Chat_Prefix(const text[], start, const prefix[])
{
	return strcmp(text[start], prefix, false, strlen(prefix)) == 0;
}

stock Chat_CategoryOf(const text[], colour)
{
	if (g_ChatNextCat != CHAT_CAT_AUTO)
	{
		return g_ChatNextCat;
	}
	new s = Chat_TextStart(text);
	if (Chat_Prefix(text, s, "[STAFF]") || Chat_Prefix(text, s, "[ADMIN") || Chat_Prefix(text, s, "AdmCmd") || Chat_Prefix(text, s, "[REPORT"))
	{
		return CHAT_CAT_ADMIN;
	}
	if (Chat_Prefix(text, s, "[RADIO]") || Chat_Prefix(text, s, "[DIP]") || Chat_Prefix(text, s, "[FAZIONE"))
	{
		return CHAT_CAT_FACTION;
	}
	if (Chat_Prefix(text, s, "((") || colour == 0xAAAAAAFF)
	{
		return CHAT_CAT_OOC;
	}
	if (Chat_Prefix(text, s, "[911]"))
	{
		return CHAT_CAT_EMERGENCY;
	}
	if (Chat_Prefix(text, s, "[LAVORO]") || Chat_Prefix(text, s, "[TAXI]") || Chat_Prefix(text, s, "[CONTRATTO"))
	{
		return CHAT_CAT_JOB;
	}
	return CHAT_CAT_SYSTEM;
}

stock Chat_Line(playerid, colour, const text[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (g_ChatCef[playerid] && CefUI_Active(playerid))
	{
		static line[CHAT_LINE_MAX + 16];
		format(line, sizeof line, "{%06x}%s", (colour >>> 8) & 0xFFFFFF, text);
		cef_emit_event(playerid, "lssc:chat", 1, Chat_CategoryOf(text, colour), 0, line);
		return 1;
	}
	static out[CHAT_LINE_MAX];
	Lang_Translit(text, out, sizeof out);
	return Chat_RawSCM(playerid, colour, "%s", out);
}

// SendClientMessage(playerid, colour, format[], ...) con formattazione come la nativa open.mp
stock Chat_SCM(playerid, colour, const fmt[], {Float, _}:...)
{
	new n = (numargs() - 3) * 4;
	if (n <= 0)
	{
		return Chat_Line(playerid, colour, fmt);
	}
	new message[CHAT_LINE_MAX], argStart, argEnd, i, cnt;
	#emit CONST.alt fmt
	#emit LCTRL 5
	#emit ADD
	#emit STOR.S.pri argStart
	argEnd = argStart + n;
	// argomenti variabili (indirizzi) dall'ultimo al primo, poi fmt, dimensione, destinazione
	for (i = argEnd; i > argStart; i -= 4)
	{
		#emit LREF.S.pri i
		#emit PUSH.pri
	}
	#emit PUSH.S fmt
	#emit PUSH.C 512    // = CHAT_LINE_MAX (#emit non espande le macro)
	#emit PUSH.ADR message
	cnt = n + 12;
	#emit PUSH.S cnt
	#emit SYSREQ.C format
	n += 16;
	#emit LCTRL 4
	#emit LOAD.S.alt n
	#emit ADD
	#emit SCTRL 4
	return Chat_Line(playerid, colour, message);
}

#define SendClientMessage Chat_SCM

// chat "normale" (testo senza comando): come la chat di SA-MP, a tutti, con il nome nel colore del giocatore
stock Chat_Broadcast(playerid, const text[])
{
	new name[MAX_PLAYER_NAME + 1], line[CHAT_LINE_MAX];
	GetPlayerName(playerid, name, sizeof name);
	format(line, sizeof line, "{%06x}%s{FFFFFF}: %s", (GetPlayerColor(playerid) >>> 8) & 0xFFFFFF, name, text);
	g_ChatNextCat = CHAT_CAT_IC;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i))
		{
			Chat_Line(i, -1, line);
		}
	}
	g_ChatNextCat = CHAT_CAT_AUTO;
	return 1;
}
