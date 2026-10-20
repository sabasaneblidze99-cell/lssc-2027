// =============================================================================
// ui/hud_cef.inc - HUD "Street Crime's Glass UI" (CEF), lato server.
//
// Il server resta l'autorita': legge salute, armatura, fame, veicolo, quartiere e li invia alla pagina
// SOLO quando cambiano (protocollo: cef/assets/PROTOCOLLO.md, sezione HUD Glass UI).
//  - un unico timer a 250 ms; tachimetro a 4 Hz solo per chi guida; stato/zona/interazioni a 1 Hz;
//  - giocatori senza CEF: niente eventi, restano TextDraw e dialog di prima.
// Tasti (OnPlayerKeyStateChange): Y = interazione col punto vicino, N = azioni rapide.
// =============================================================================

#define HUDC_TICK_MS      (250)
#define HUDC_DANGER_CRIME (70)    // stessa soglia della scheda quartiere (living_ui: hazard)
#define HUDC_SAFE_SEC     (60)
#define HUDC_CHAT_CD_MS   (700)

// punti interattivi (comando esistente eseguito con il tasto Y)
#define HUDC_IA_NONE      (0)
#define HUDC_IA_BANK      (1)
#define HUDC_IA_ATM       (2)
#define HUDC_IA_JOB       (3)
#define HUDC_IA_RENTAL    (4)
#define HUDC_IA_TAXI      (5)
#define HUDC_IA_BOARD     (6)
#define HUDC_IA_HOUSE     (7)
#define HUDC_IA_EXIT      (8)
#define HUDC_IA_SHOP      (9)
#define HUDC_IA_DEALER    (10)
#define HUDC_IA_STATION   (11)

// azioni rapide (bit della maschera inviata con lssc:quick_open)
#define HUDC_Q_INVENTORY  (1)
#define HUDC_Q_PHONE      (2)
#define HUDC_Q_MAP        (4)
#define HUDC_Q_ANIMS      (8)
#define HUDC_Q_VEHICLES   (16)

static const s_HcIaCmd[12][16] = {
	"", "/banca", "/atm", "/lavoro", "/noleggio", "/taxi", "/contratti", "/casa", "/esci", "/compra", "/concessionario", "/refuel"
};
static const s_HcIaLabel[12][80] = {
	"", "ბანკი", "ბანკომატი", "დასაქმების ცენტრი", "ავტომობილის გაქირავება", "ტაქსის პარკი", "კონტრაქტების დაფა",
	"სახლი", "გასვლა", "მაღაზია", "ავტოსალონი", "ბენზინგასამართი"
};

new bool:g_HcOn[MAX_PLAYERS];          // pagina pronta e HUD visibile
new g_HcHp[MAX_PLAYERS], g_HcArmor[MAX_PLAYERS], g_HcHunger[MAX_PLAYERS];
new g_HcVeh[MAX_PLAYERS];              // veicolo mostrato nel tachimetro (0 = nessuno)
new g_HcVSpeed[MAX_PLAYERS], g_HcVFuel[MAX_PLAYERS], g_HcVFlags[MAX_PLAYERS], g_HcVHealth[MAX_PLAYERS];
new g_HcZone[MAX_PLAYERS];             // ultimo stato zona inviato (codificato)
new g_HcIa[MAX_PLAYERS];               // punto interattivo mostrato
new g_HcVoice[MAX_PLAYERS];
new g_HcClockMin[MAX_PLAYERS];
new g_HcLastMoney[MAX_PLAYERS], g_HcLastBank[MAX_PLAYERS], g_HcLastLevel[MAX_PLAYERS];
new g_HcChatTick[MAX_PLAYERS];
new g_HcTicks;
new g_HcTimer;

HudCef_OnInit()
{
	if (!g_HcTimer)
	{
		g_HcTimer = SetTimer("HudCef_OnTick", HUDC_TICK_MS, true);
	}
}

HudCef_OnExit()
{
	if (g_HcTimer)
	{
		KillTimer(g_HcTimer);
		g_HcTimer = 0;
	}
}

HudCef_Reset(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_HcOn[playerid] = false;
	g_HcHp[playerid] = g_HcArmor[playerid] = g_HcHunger[playerid] = -2;
	g_HcVeh[playerid] = 0;
	g_HcVSpeed[playerid] = g_HcVFuel[playerid] = g_HcVFlags[playerid] = g_HcVHealth[playerid] = -2;
	g_HcZone[playerid] = -2;
	g_HcIa[playerid] = HUDC_IA_NONE;
	g_HcVoice[playerid] = -2;
	g_HcClockMin[playerid] = -1;
	g_HcLastMoney[playerid] = g_HcLastBank[playerid] = cellmin;
	g_HcLastLevel[playerid] = 0;
	g_HcChatTick[playerid] = 0;
	Chat_SetCef(playerid, false);
	return 1;
}

HudCef_OnPlayerConnect(playerid)
{
	HudCef_Reset(playerid);
}

HudCef_OnPlayerDisconnect(playerid)
{
	HudCef_Reset(playerid);
}

// chiamata da CefUI_HudShow: la pagina e' pronta e l'HUD e' visibile (o viene nascosto)
HudCef_OnVisible(playerid, bool:show)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!show || !CefUI_Active(playerid))
	{
		if (g_HcOn[playerid])
		{
			HudCef_HideVehicle(playerid);
			HudCef_SetInteract(playerid, HUDC_IA_NONE);
		}
		g_HcOn[playerid] = false;
		return 0;
	}
	if (g_HcOn[playerid])
	{
		return 1;
	}
	g_HcOn[playerid] = true;
	// tutto ripartira' con un invio completo
	g_HcHp[playerid] = g_HcArmor[playerid] = g_HcHunger[playerid] = -2;
	g_HcZone[playerid] = -2;
	g_HcVoice[playerid] = -2;
	g_HcClockMin[playerid] = -1;
	new name[MAX_PLAYER_NAME + 1];
	GetPlayerName(playerid, name, sizeof name);
	cef_emit_event(playerid, "lssc:identity", 0, name, 1, playerid);
	// chat CEF: attiva di default per chi ha la pagina (si torna alla chat SA-MP con /chatsamp)
	if (!gPlayer[playerid][pChatSamp])
	{
		Chat_SetCef(playerid, true);
	}
	cef_emit_event(playerid, "lssc:chat_mode", 1, Chat_IsCef(playerid) ? 1 : 0);
	// i tasti arrivano alla pagina anche senza focus (F6 = scrivi in chat)
	#if defined LSRP_CEF
		cef_always_listen_keys(playerid, 1, true);
	#endif
	HudCef_Update(playerid, true);
	return 1;
}

public HudCef_OnTick()
{
	g_HcTicks++;
	new slow = g_HcTicks % (1000 / HUDC_TICK_MS) == 0;
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!g_HcOn[p])
		{
			continue;
		}
		if (!IsPlayerConnected(p))
		{
			g_HcOn[p] = false;
			continue;
		}
		HudCef_UpdateVehicle(p);
		if (slow)
		{
			HudCef_Update(p, false);
		}
	}
}

HudCef_Update(playerid, bool:full)
{
	HudCef_UpdateStatus(playerid);
	HudCef_UpdateZone(playerid);
	HudCef_UpdateInteract(playerid);
	HudCef_UpdateVoice(playerid);
	new h, m, s, y, mo, d;
	gettime(h, m, s);
	if (full || m != g_HcClockMin[playerid])
	{
		g_HcClockMin[playerid] = m;
		getdate(y, mo, d);
		cef_emit_event(playerid, "lssc:clock", 1, h, 1, m, 1, d, 1, mo, 1, y);
	}
	if (full)
	{
		HudCef_UpdateVehicle(playerid);
	}
}

// ---------------------------------------------------------------- salute, armatura, fame
HudCef_UpdateStatus(playerid)
{
	new Float:hp, Float:ar;
	GetPlayerHealth(playerid, hp);
	GetPlayerArmour(playerid, ar);
	new ihp = clamp(floatround(hp, floatround_ceil), 0, 100), iar = clamp(floatround(ar, floatround_ceil), 0, 100);
	new ihu = gPlayer[playerid][pSpawned] ? clamp(g_HPl[playerid][hpHunger], 0, 100) : -1;
	if (ihp == g_HcHp[playerid] && iar == g_HcArmor[playerid] && ihu == g_HcHunger[playerid])
	{
		return 0;
	}
	g_HcHp[playerid] = ihp;
	g_HcArmor[playerid] = iar;
	g_HcHunger[playerid] = ihu;
	// sete: il gamemode non ha un sistema di sete -> -1 (la pagina non mostra l'indicatore)
	cef_emit_event(playerid, "lssc:status", 1, ihp, 1, iar, 1, ihu, 1, -1);
	return 1;
}

// ---------------------------------------------------------------- tachimetro
HudCef_HideVehicle(playerid)
{
	if (g_HcVeh[playerid] == 0)
	{
		return 0;
	}
	g_HcVeh[playerid] = 0;
	g_HcVSpeed[playerid] = g_HcVFuel[playerid] = g_HcVFlags[playerid] = g_HcVHealth[playerid] = -2;
	cef_emit_event(playerid, "lssc:veh_show", 1, 0, 0, "");
	return 1;
}

HudCef_UpdateVehicle(playerid)
{
	new vid = GetPlayerState(playerid) == PLAYER_STATE_DRIVER ? GetPlayerVehicleID(playerid) : 0;
	if (vid == 0)
	{
		HudCef_HideVehicle(playerid);
		return 0;
	}
	if (vid != g_HcVeh[playerid])
	{
		g_HcVeh[playerid] = vid;
		g_HcVSpeed[playerid] = g_HcVFuel[playerid] = g_HcVFlags[playerid] = g_HcVHealth[playerid] = -2;
		new name[24];
		Veh_StdModelName(GetVehicleModel(vid), name);
		cef_emit_event(playerid, "lssc:veh_show", 1, 1, 0, name);
	}
	new speed = Veh_SpeedKmh(vid);
	// carburante: solo per i veicoli personali (sistema carburante di vehicle.inc); altrimenti -1
	new fuel = -1, owner = INVALID_PLAYER_ID, slot = -1;
	if (Veh_Lookup(vid, owner, slot) && owner >= 0 && owner < MAX_PLAYERS && slot >= 0)
	{
		fuel = Veh_FuelPct(owner, slot);
	}
	new engine, lights, alarm, doors, bonnet, boot, objective;
	GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
	new flags = (engine != 0 ? 1 : 0) | (lights == 1 ? 2 : 0) | (doors == 1 ? 4 : 0);   // -1 = non impostato: motore acceso, luci spente
	new Float:vh;
	GetVehicleHealth(vid, vh);
	new health = clamp(floatround((vh - 250.0) * 100.0 / 750.0), 0, 100);
	if (speed == g_HcVSpeed[playerid] && fuel == g_HcVFuel[playerid] && flags == g_HcVFlags[playerid] && health == g_HcVHealth[playerid])
	{
		return 1;
	}
	g_HcVSpeed[playerid] = speed;
	g_HcVFuel[playerid] = fuel;
	g_HcVFlags[playerid] = flags;
	g_HcVHealth[playerid] = health;
	cef_emit_event(playerid, "lssc:veh", 1, speed, 1, fuel, 1, flags & 1, 1, (flags >> 1) & 1, 1, health, 1, (flags >> 2) & 1);
	return 1;
}

// ---------------------------------------------------------------- quartiere / zona
HudCef_UpdateZone(playerid)
{
	new indoor = GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0;
	new d = 0, danger = -1;
	if (!indoor)
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		d = District_At(x, y);
		if (d > 0)
		{
			// sicurezza dai dati reali del quartiere (Living Los Santos)
			new crime = District_GetCrime(d), sec = District_GetSecurity(d);
			danger = crime >= HUDC_DANGER_CRIME ? 2 : (sec >= HUDC_SAFE_SEC ? 0 : 1);
		}
	}
	new code = (d * 10 + (danger + 1)) * 2 + (indoor ? 1 : 0);
	if (code == g_HcZone[playerid])
	{
		return 0;
	}
	g_HcZone[playerid] = code;
	cef_emit_event(playerid, "lssc:zone", 1, d, 1, danger, 1, indoor ? 1 : 0);
	return 1;
}

// ---------------------------------------------------------------- voce (SampVoice, opzionale)
HudCef_UpdateVoice(playerid)
{
	new st = Voice_State(playerid);
	if (st == g_HcVoice[playerid])
	{
		return 0;
	}
	g_HcVoice[playerid] = st;
	cef_emit_event(playerid, "lssc:voice", 1, st, 0, "B");
	return 1;
}

// ---------------------------------------------------------------- interazioni
HudCef_FindInteract(playerid)
{
	if (!gPlayer[playerid][pSpawned])
	{
		return HUDC_IA_NONE;
	}
	new st = GetPlayerState(playerid);
	if (st == PLAYER_STATE_DRIVER)
	{
		return Veh_NearStation(playerid) ? HUDC_IA_STATION : HUDC_IA_NONE;
	}
	if (st != PLAYER_STATE_ONFOOT)
	{
		return HUDC_IA_NONE;
	}
	new inside = House_FindInside(playerid);
	if (inside != -1)
	{
		return House_NearExit(playerid, inside) ? HUDC_IA_EXIT : HUDC_IA_NONE;
	}
	if (Bank_NearPoint(playerid, true) >= 0) return HUDC_IA_BANK;
	if (Bank_NearPoint(playerid, false) >= 0) return HUDC_IA_ATM;
	if (Job_NearCenter(playerid)) return HUDC_IA_JOB;
	if (Rental_AtOffice(playerid, 3.0)) return HUDC_IA_RENTAL;
	if (Taxi_AtDepot(playerid)) return HUDC_IA_TAXI;
	if (Hub_NearBoard(playerid) >= 0) return HUDC_IA_BOARD;
	if (House_FindNear(playerid) != -1) return HUDC_IA_HOUSE;
	if (House_NearShop(playerid)) return HUDC_IA_SHOP;
	if (Veh_NearDealer(playerid)) return HUDC_IA_DEALER;
	return HUDC_IA_NONE;
}

HudCef_SetInteract(playerid, ia)
{
	if (ia == g_HcIa[playerid])
	{
		return 0;
	}
	g_HcIa[playerid] = ia;
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	if (ia == HUDC_IA_NONE)
	{
		cef_emit_event(playerid, "lssc:interact", 1, 0, 0, "", 0, "", 1, -1);
	}
	else
	{
		cef_emit_event(playerid, "lssc:interact", 1, 1, 0, "Y", 0, s_HcIaLabel[ia], 1, -1);
	}
	return 1;
}

HudCef_UpdateInteract(playerid)
{
	HudCef_SetInteract(playerid, HudCef_FindInteract(playerid));
}

// ---------------------------------------------------------------- tasti
HudCef_OnKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
	if (!g_HcOn[playerid] || !gPlayer[playerid][pSpawned])
	{
		return 0;
	}
	// Y: esegue il comando del punto interattivo (lo stesso che il giocatore potrebbe scrivere)
	if ((newkeys & KEY_YES) && !(oldkeys & KEY_YES))
	{
		new ia = HudCef_FindInteract(playerid);
		if (ia != HUDC_IA_NONE)
		{
			new cmd[16];
			strcat(cmd, s_HcIaCmd[ia]);      // copia: Cmd_Dispatch vuole un array modificabile (errore con pawncc 3.10.11)
			Cmd_Dispatch(playerid, cmd);
			return 1;
		}
	}
	// N: menu azioni rapide (solo a piedi, nessuna vista CEF aperta)
	if ((newkeys & KEY_NO) && !(oldkeys & KEY_NO) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && g_CefView[playerid] == 0)
	{
		cef_emit_event(playerid, "lssc:quick_open", 1, HUDC_Q_INVENTORY | HUDC_Q_PHONE | HUDC_Q_VEHICLES);
		return 1;
	}
	return 0;
}

// pagina -> server: azione rapida scelta (stessi comandi del gioco: il server rivalida tutto)
public Cef_OnQuick(playerid, arguments[])
{
	if (!g_HcOn[playerid])
	{
		return 1;
	}
	if (!strcmp(arguments, "inventory", true))
	{
		Cmd_Dispatch(playerid, "/inventario");
	}
	else if (!strcmp(arguments, "phone", true))
	{
		Cmd_Dispatch(playerid, "/telefono");
	}
	else if (!strcmp(arguments, "vehicles", true))
	{
		Cmd_Dispatch(playerid, "/v list");
	}
	return 1;
}

public Cef_OnQuickClose(playerid, arguments[])
{
	return 1;
}

// pagina -> server: messaggio dalla chat CEF (UTF-8 in esadecimale). Comandi '/' come OnPlayerCommandText,
// testo normale come OnPlayerText (stesse verifiche: login, mute).
public Cef_OnChatSend(playerid, arguments[])
{
	if (!IsPlayerConnected(playerid) || !CefUI_Active(playerid))
	{
		return 1;
	}
	new now = GetTickCount();
	if (g_HcChatTick[playerid] != 0 && now - g_HcChatTick[playerid] < HUDC_CHAT_CD_MS)
	{
		return 1;
	}
	g_HcChatTick[playerid] = now;
	new text[400];
	if (!CefBridge_HexDecode(arguments, text, sizeof text) || text[0] == 0)
	{
		return 1;
	}
	// niente caratteri di controllo
	for (new i = 0; text[i] != 0; i++)
	{
		if (text[i] < 32 && text[i] >= 0)
		{
			text[i] = ' ';
		}
	}
	if (text[0] == '/')
	{
		if (!Cmd_Dispatch(playerid, text))
		{
			Util_Send(playerid, 0xFF6347FF, "უცნობი ბრძანება. დაწერე /help.");
		}
		return 1;
	}
	if (Char_OnText(playerid) && Admin_OnText(playerid))
	{
		Chat_Broadcast(playerid, text);
	}
	return 1;
}

// /chatsamp e /chatcef: scelta della chat (preferenza salvata solo per la sessione)
HudCef_CmdChat(playerid, bool:cef)
{
	if (!CefUI_Active(playerid))
	{
		Util_Send(playerid, 0x99CCFFFF, "CEF ჩატი ხელმისაწვდომია მხოლოდ CEF-ით.");
		return 1;
	}
	gPlayer[playerid][pChatSamp] = !cef;
	Chat_SetCef(playerid, cef);
	cef_emit_event(playerid, "lssc:chat_mode", 1, cef ? 1 : 0);
	Util_Send(playerid, 0x66DD66FF, cef ? "ჩატი: CEF (F6 - დაწერა)." : "ჩატი: SA-MP (T - დაწერა).");
	return 1;
}

// ---------------------------------------------------------------- denaro e livello (feedback)
// Chiamata dopo ogni aggiornamento dell'HUD (CefUI_HudData): +$ / -$ e nuovo livello solo su valori reali del server.
HudCef_OnHudData(playerid, money, bank, level)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (g_HcOn[playerid] && g_HcLastMoney[playerid] != cellmin)
	{
		if (money != g_HcLastMoney[playerid])
		{
			cef_emit_event(playerid, "lssc:money_delta", 1, money - g_HcLastMoney[playerid], 1, 0);
		}
		if (bank != g_HcLastBank[playerid])
		{
			cef_emit_event(playerid, "lssc:money_delta", 1, bank - g_HcLastBank[playerid], 1, 1);
		}
		if (level > g_HcLastLevel[playerid] && g_HcLastLevel[playerid] > 0)
		{
			cef_emit_event(playerid, "lssc:levelup", 1, level);
		}
	}
	g_HcLastMoney[playerid] = money;
	g_HcLastBank[playerid] = bank;
	g_HcLastLevel[playerid] = level;
	return 1;
}

// notifica generica (kind: 0 successo, 1 info, 2 avviso, 3 errore, 4 soldi ricevuti, 5 soldi spesi, 6 missione, 7 lavoro, 8 RP)
HudCef_Notify(playerid, kind, const title[], const text[], ms = 0)
{
	if (!CefUI_Active(playerid))
	{
		return 0;
	}
	cef_emit_event(playerid, "lssc:notify", 1, kind, 0, title, 0, text, 1, ms);
	return 1;
}
