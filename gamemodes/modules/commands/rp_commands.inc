new g_CmdNames[97][16] = {
	"me",
	"do",
	"try",
	"b",
	"ame",
	"pay",
	"stats",
	"who",
	"help",
	"report",
	"v",
	"refuel",
	"concessionario",
	"casa",
	"entra",
	"esci",
	"luce",
	"gas",
	"frigo",
	"bollette",
	"bussa",
	"cucina",
	"mangia",
	"compra",
	"lavoro",
	"inizia",
	"fine",
	"fazione",
	"invita",
	"accetta",
	"promuovi",
	"degrada",
	"licenzia",
	"duty",
	"r",
	"dep",
	"ammanetta",
	"smanetta",
	"multa",
	"multe",
	"cura",
	"rianima",
	"inventario",
	"inv",
	"usa",
	"dai",
	"getta",
	"prendi",
	"documento",
	"atm",
	"banca",
	"saldo",
	"deposita",
	"preleva",
	"bonifico",
	"estratto",
	"salta",
	"storia",
	"mostrastoria",
	"noleggio",
	"noleggia",
	"riconsegna",
	"taxi",
	"chiamataxi",
	"accettacorsa",
	"tassametro",
	"contratti",
	"contratto",
	"reputazione",
	"quartiere",
	"contribuisci",
	"aiuto",
	"telefono",
	"email",
	"goto",
	"setlevel",
	"setmoney",
	"ah",
	"a",
	"reports",
	"rr",
	"gethere",
	"freeze",
	"unfreeze",
	"sethp",
	"kick",
	"mute",
	"unmute",
	"warn",
	"setadmin",
	"givemoney",
	"ban",
	"unban",
	"setfazione",
	"ldset",
	"chatsamp",
	"chatcef"
};

Cmd_Dispatch(playerid, const cmdtext[])
{
	if (cmdtext[0] != 47)
	{
		return 0;
	}
	new name[16], pos = 1, n = 0;
	while (cmdtext[pos] != 0 && cmdtext[pos] != 32)
	{
		if (n >= 15)
		{
			return 0;
		}
		name[n++] = tolower(cmdtext[pos]);
		pos++;
	}
	name[n] = 0;
	while (cmdtext[pos] == 32)
	{
		pos++;
	}
	new index = -1;
	for (new i = 0; i < sizeof g_CmdNames; i++)
	{
		if (!strcmp(name, g_CmdNames[i], 0, 0x7FFFFFFF))
		{
			index = i;
			break;
		}
	}
	if (index < 0)
	{
		return 0;
	}
	if (!Util_IsPlaying(playerid))
	{
		if (!strcmp(name, "salta", 0, 0x7FFFFFFF) && gPlayer[playerid][pLoggedIn] && gPlayer[playerid][pAuthState] == 14)
		{
			Intro_CmdSalta(playerid);
			return 1;
		}
		if (index >= 74)
		{
			return 0;
		}
		Util_Send(playerid, -10270721, "Devi aver effettuato l'accesso ed essere in gioco per usare i comandi.");
		return 1;
	}
	if (index < 5 && Admin_CheckMuted(playerid))
	{
		return 1;
	}
	new fn[24];
	format(fn, 24, "Cmd_%s", name);
	return CallLocalFunction(fn, "ds", playerid, cmdtext[pos]);
}

public Cmd_me(playerid, const params[])
{
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /me azione");
		return 1;
	}
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "* %s %s", name, text);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
	return 1;
}

public Cmd_do(playerid, const params[])
{
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /do descrizione");
		return 1;
	}
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "* %s (( %s ))", text, name);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
	return 1;
}

public Cmd_try(playerid, const params[])
{
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /try azione");
		return 1;
	}
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "* %s prova a %s e %s", name, text, random(100) < 50 ? "ha successo" : "fallisce");
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
	return 1;
}

public Cmd_b(playerid, const params[])
{
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /b testo");
		return 1;
	}
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "(( %s [%d]: %s ))", name, playerid, text);
	Util_SendNear(playerid, 0xAAAAAAFF, 15.0, msg);
	return 1;
}

public Cmd_ame(playerid, const params[])
{
	new text[129];
	if (!Util_SanitizeText(params, text, 128))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /ame azione");
		return 1;
	}
	new name[25], msg[256];
	GetPlayerName(playerid, name, 25);
	SetPlayerChatBubble(playerid, "* %s", -81.42773, 20.0, 6000, text);
	format(msg, 256, "> %s %s", name, text);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
	return 1;
}

public Cmd_pay(playerid, const params[])
{
	new pos = 0, tid[12], tam[12], targetid = 0, amount = 0;
	Util_NextToken(params, pos, tid, 12);
	Util_NextToken(params, pos, tam, 12);
	if (tid[0] == 0 || tam[0] == 0 || !Util_ParseInt(tam, amount) || !Util_ParsePlayerId(tid, targetid))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /pay id importo");
		return 1;
	}
	if (gPlayer[playerid][pLevel] < 2)
	{
		SendClientMessage(playerid, -10270721, "Devi essere almeno al livello %d per usare /pay.", 2);
		return 1;
	}
	if (amount < 1 || amount > 50000)
	{
		SendClientMessage(playerid, -10270721, "Importo non valido (%d-%d).", 1, 50000);
		return 1;
	}
	if (targetid == playerid || !Util_IsPlaying(targetid))
	{
		Util_Send(playerid, -10270721, "Giocatore non valido.");
		return 1;
	}
	if (!Util_InRange(playerid, targetid, 3.0))
	{
		Util_Send(playerid, -10270721, "Sei troppo lontano.");
		return 1;
	}
	new payResult = Valid_PayAmount(amount, gPlayer[playerid][pMoney], gPlayer[targetid][pMoney]);
	if (payResult == 2)
	{
		Util_Send(playerid, -10270721, "Non hai abbastanza contanti.");
		return 1;
	}
	if (payResult != 0)
	{
		Util_Send(playerid, -10270721, "Il destinatario non puo' ricevere questa somma.");
		return 1;
	}
	Money_Take(playerid, amount);
	Money_Give(targetid, amount);
	new sn[25], tn[25], msg[256];
	GetPlayerName(playerid, sn, 25);
	GetPlayerName(targetid, tn, 25);
	SendClientMessage(playerid, 0x66DD66FF, "Hai consegnato $%d a %s.", amount, tn);
	SendClientMessage(targetid, 0x66DD66FF, "Hai ricevuto $%d da %s.", amount, sn);
	format(msg, 256, "* %s consegna del denaro a %s", sn, tn);
	Util_SendNear(playerid, 0xC2A2DAFF, 20.0, msg);
	Char_Save(playerid);
	Char_Save(targetid);
	format(msg, 256, "%s -> %s: $%d", sn, tn, amount);
	Log_Write("PAY", msg);
	return 1;
}

public Cmd_lavoro(playerid, const params[])
{
	Job_CmdLavoro(playerid, params);
	LD_OnboardAction(playerid, 5);
	return 1;
}

public Cmd_inizia(playerid, const params[])
{
	Job_CmdInizia(playerid);
	return 1;
}

public Cmd_fine(playerid, const params[])
{
	Job_CmdFine(playerid);
	return 1;
}

public Cmd_fazione(playerid, const params[])
{
	Faction_CmdFazione(playerid, params);
	return 1;
}

public Cmd_invita(playerid, const params[])
{
	Faction_CmdInvita(playerid, params);
	return 1;
}

public Cmd_accetta(playerid, const params[])
{
	Faction_CmdAccetta(playerid);
	return 1;
}

public Cmd_promuovi(playerid, const params[])
{
	Faction_CmdRankOp(playerid, params, 4);
	return 1;
}

public Cmd_degrada(playerid, const params[])
{
	Faction_CmdRankOp(playerid, params, 5);
	return 1;
}

public Cmd_licenzia(playerid, const params[])
{
	Faction_CmdRankOp(playerid, params, 6);
	return 1;
}

public Cmd_duty(playerid, const params[])
{
	Faction_CmdDuty(playerid);
	return 1;
}

public Cmd_r(playerid, const params[])
{
	Faction_CmdRadio(playerid, params, 0);
	return 1;
}

public Cmd_dep(playerid, const params[])
{
	Faction_CmdRadio(playerid, params, 1);
	return 1;
}

public Cmd_ammanetta(playerid, const params[])
{
	Faction_CmdAmmanetta(playerid, params, 1);
	return 1;
}

public Cmd_smanetta(playerid, const params[])
{
	Faction_CmdAmmanetta(playerid, params, 0);
	return 1;
}

public Cmd_multa(playerid, const params[])
{
	Faction_CmdMulta(playerid, params);
	return 1;
}

public Cmd_multe(playerid, const params[])
{
	Faction_CmdMulte(playerid);
	return 1;
}

public Cmd_cura(playerid, const params[])
{
	Faction_CmdCura(playerid, params);
	return 1;
}

public Cmd_rianima(playerid, const params[])
{
	Faction_CmdRianima(playerid, params);
	return 1;
}

public Cmd_setfazione(playerid, const params[])
{
	return Faction_CmdSetfazione(playerid, params) ? 1 : 0;
}

public Cmd_stats(playerid, const params[])
{
	new name[25], level = gPlayer[playerid][pLevel], exp = gPlayer[playerid][pExp];
	GetPlayerName(playerid, name, 25);
	SendClientMessage(playerid, -2994689, "--- Statistiche di %s ---", name);
	if (level >= 50)
	{
		SendClientMessage(playerid, -1, "Livello: %d (massimo)  Exp totale: %d", level, exp);
	}
	else
	{
		SendClientMessage(playerid, -1, "Livello: %d  Exp: %d/%d", level, exp - Level_ExpForLevel(level), Level_StepExp(level));
	}
	SendClientMessage(playerid, -1, "Contanti: $%d  Banca: $%d", gPlayer[playerid][pMoney], gPlayer[playerid][pBank]);
	new played = gPlayer[playerid][pSessionMs];
	new total = gPlayer[playerid][pPlayedMs];
	SendClientMessage(playerid, -1, "Ore di gioco: sessione %dh %02dm, totali %dh %02dm", played / 3600000, played % 3600000 / 60000, total / 3600000, total % 3600000 / 60000);
	new remainSec = (900000 - gPlayer[playerid][pPaycheckMs]) / 1000;
	if (remainSec < 0)
	{
		remainSec = 0;
	}
	SendClientMessage(playerid, -1, "Prossimo stipendio tra: %d min %02d s (%d $ in banca)", remainSec / 60, remainSec % 60, Economy_PaycheckAmount(level));
	return 1;
}

public Cmd_who(playerid, const params[])
{
	new total = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (Util_IsPlaying(i))
		{
			total++;
		}
	}
	SendClientMessage(playerid, -2994689, "--- Giocatori online (%d) ---", total);
	new shown = 0, name[25];
	for (new i = 0; i < MAX_PLAYERS && shown < 50; i++)
	{
		if (!Util_IsPlaying(i))
		{
			continue;
		}
		GetPlayerName(i, name, 25);
		SendClientMessage(playerid, -1, "[%d] %s", i, name);
		shown++;
	}
	if (total > shown)
	{
		SendClientMessage(playerid, -1, "... e altri %d giocatori.", total - shown);
	}
	return 1;
}

public Cmd_help(playerid, const params[])
{
	SendClientMessage(playerid, -2994689, "--- Comandi ---");
	Util_Send(playerid, 0x99CCFFFF, "/me azione · /do descrizione · /try azione · /ame azione");
	Util_Send(playerid, 0x99CCFFFF, "/b testo (chat OOC locale)");
	Util_Send(playerid, 0x99CCFFFF, "/pay id importo · /stats · /who · /report testo");
	Util_Send(playerid, 0x99CCFFFF, "/v lock|engine|lights|park|find|list|sell|recover · /refuel [litri] · /concessionario");
	Util_Send(playerid, 0x99CCFFFF, "/casa [info|compra|vendi|serratura|chiave|chiavi|revoca] · /entra · /esci · /bussa · /luce · /gas");
	Util_Send(playerid, 0x99CCFFFF, "/frigo [metti|prendi n] · /cucina · /mangia [freddo] · /bollette");
	Util_Send(playerid, 0x99CCFFFF, "Inventario: /inventario (/inv) · /usa oggetto · /dai id oggetto [q] · /getta oggetto [q] · /prendi · /documento [id|richiedi]");
	Util_Send(playerid, 0x99CCFFFF, "Banca (vicino a un ATM o sportello): /atm (menu) · /banca (solo sportello) · /saldo (ovunque) · /deposita importo · /preleva importo · /bonifico id|nome importo · /estratto");
	Util_Send(playerid, 0x99CCFFFF, "Storia: /storia (la tua) · /storia imposta (una volta, se non l'hai scelta) · /mostrastoria id (a chi ti sta vicino)");
	if (Util_GetAdmin(playerid) >= 2)
	{
		Util_Send(playerid, -5963521, "Staff: /storia id (legge la storia di un giocatore, registrato in admin_log)");
	}
	Util_Send(playerid, 0x99CCFFFF, "Supermercato: /compra [cibo|pasto|acqua|snack|kit|benda|sigarette|accendino|telefono|zaino] [n]");
	Util_Send(playerid, 0x99CCFFFF, "Lavori: /lavoro [info|prendi|lascia] (Job Center, piazza City Hall) · /inizia · /fine · /multe");
	Util_Send(playerid, 0x99CCFFFF, "Citta': /contratti (bacheca) · /contratto [abbandona] · /reputazione · /quartiere [nome] · /contribuisci importo · /aiuto [off] · /telefono");
	Util_Send(playerid, 0x99CCFFFF, "Noleggio (Pershing Sq.): /noleggio · /riconsegna  ·  Taxi (deposito Unity): /taxi on|off · /accettacorsa [id] · /tassametro start|stop · /chiamataxi");
	if (Util_GetAdmin(playerid) >= 3)
	{
		Util_Send(playerid, -5963521, "Staff: /ldset (valori dei quartieri per test, registrato in admin_log)");
	}
	Faction_SendHelp(playerid);
	if (Util_GetAdmin(playerid) >= 3)
	{
		Util_Send(playerid, -5963521, "Staff: /setfazione id|Nome_Cognome fazione(0-2) grado(1-6)");
	}
	Admin_SendHelp(playerid);
	return 1;
}

public Cmd_report(playerid, const params[])
{
	new text[97];
	if (!Util_SanitizeText(params, text, 96))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /report testo");
		return 1;
	}
	new now = GetTickCount(), elapsed = now - gPlayer[playerid][pLastReportTick];
	if (elapsed < 30000)
	{
		SendClientMessage(playerid, -10270721, "Attendi ancora %d secondi prima di un nuovo report.", (30000 - elapsed + 1000 - 1) / 1000);
		return 1;
	}
	new name[25], msg[256], staff = 0;
	GetPlayerName(playerid, name, 25);
	format(msg, 256, "[REPORT] %s [%d]: %s", name, playerid, text);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!IsPlayerConnected(i) || Util_GetAdmin(i) < 1)
		{
			continue;
		}
		Util_Send(i, -5963521, msg);
		staff++;
	}
	if (staff == 0)
	{
		Util_Send(playerid, -10270721, "Nessun membro dello staff e' online: il report non e' stato inviato.");
		return 1;
	}
	gPlayer[playerid][pLastReportTick] = now;
	Admin_StoreReport(playerid, text);
	Util_Send(playerid, 0x66DD66FF, "Report inviato allo staff.");
	return 1;
}

public Cmd_v(playerid, const params[])
{
	Veh_CmdV(playerid, params);
	return 1;
}

public Cmd_refuel(playerid, const params[])
{
	Veh_CmdRefuel(playerid, params);
	return 1;
}

public Cmd_concessionario(playerid, const params[])
{
	Veh_CmdDealer(playerid);
	return 1;
}

public Cmd_casa(playerid, const params[])
{
	House_CmdCasa(playerid, params);
	return 1;
}

public Cmd_entra(playerid, const params[])
{
	House_CmdEntra(playerid);
	return 1;
}

public Cmd_esci(playerid, const params[])
{
	House_CmdEsci(playerid);
	return 1;
}

public Cmd_luce(playerid, const params[])
{
	House_CmdToggleUtility(playerid, 0);
	return 1;
}

public Cmd_gas(playerid, const params[])
{
	House_CmdToggleUtility(playerid, 1);
	return 1;
}

public Cmd_frigo(playerid, const params[])
{
	House_CmdFrigo(playerid, params);
	return 1;
}

public Cmd_bollette(playerid, const params[])
{
	House_CmdBollette(playerid);
	return 1;
}

public Cmd_bussa(playerid, const params[])
{
	House_CmdBussa(playerid);
	return 1;
}

public Cmd_cucina(playerid, const params[])
{
	House_CmdCucina(playerid);
	return 1;
}

public Cmd_mangia(playerid, const params[])
{
	House_CmdMangia(playerid, params);
	return 1;
}

public Cmd_compra(playerid, const params[])
{
	House_CmdCompra(playerid, params);
	return 1;
}

public Cmd_inventario(playerid, const params[])
{
	Inv_CmdInventario(playerid);
	return 1;
}

public Cmd_inv(playerid, const params[])
{
	Inv_CmdInventario(playerid);
	return 1;
}

public Cmd_usa(playerid, const params[])
{
	new pos = 0, tok[24];
	Util_NextToken(params, pos, tok, 24);
	new itemId = Inv_ParseItem(tok);
	if (itemId < 1 || itemId > 10)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /usa [oggetto] (pasto, acqua, snack, kit, benda, sigarette, telefono, documento)");
		return 1;
	}
	Inv_UseStart(playerid, itemId);
	return 1;
}

public Cmd_dai(playerid, const params[])
{
	Inv_CmdDai(playerid, params);
	return 1;
}

public Cmd_getta(playerid, const params[])
{
	Inv_CmdGetta(playerid, params);
	return 1;
}

public Cmd_prendi(playerid, const params[])
{
	Inv_CmdPrendi(playerid);
	return 1;
}

public Cmd_documento(playerid, const params[])
{
	Inv_CmdDocumento(playerid, params);
	return 1;
}

public Cmd_salta(playerid, const params[])
{
	Intro_CmdSalta(playerid);
	return 1;
}

public Cmd_storia(playerid, const params[])
{
	Intro_CmdStoria(playerid, params);
	return 1;
}

public Cmd_mostrastoria(playerid, const params[])
{
	Intro_CmdMostra(playerid, params);
	return 1;
}

public Cmd_atm(playerid, const params[])
{
	Bank_CmdAtm(playerid, 0);
	return 1;
}

public Cmd_banca(playerid, const params[])
{
	Bank_CmdAtm(playerid, 1);
	return 1;
}

public Cmd_saldo(playerid, const params[])
{
	Bank_CmdSaldo(playerid);
	return 1;
}

public Cmd_deposita(playerid, const params[])
{
	Bank_CmdDeposita(playerid, params);
	return 1;
}

public Cmd_preleva(playerid, const params[])
{
	Bank_CmdPreleva(playerid, params);
	return 1;
}

public Cmd_bonifico(playerid, const params[])
{
	Bank_CmdBonifico(playerid, params);
	return 1;
}

public Cmd_estratto(playerid, const params[])
{
	Bank_CmdEstratto(playerid);
	return 1;
}

bool:Cmd_LivingGate(playerid)
{
	if (LD_IsReady())
	{
		return 1;
	}
	LD_SendBlock(playerid, 1);
	return 0;
}

public Cmd_noleggio(playerid, const params[])
{
	if (!Cmd_LivingGate(playerid))
	{
		return 1;
	}
	Rental_CmdNoleggio(playerid, params);
	LD_OnboardAction(playerid, 5);
	return 1;
}

public Cmd_noleggia(playerid, const params[])
{
	return Cmd_noleggio(playerid, params);
}

public Cmd_riconsegna(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Rental_CmdRiconsegna(playerid, params);
	}
	return 1;
}

public Cmd_taxi(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Taxi_CmdTaxi(playerid, params);
	}
	return 1;
}

public Cmd_chiamataxi(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Taxi_CmdChiamataxi(playerid, params);
	}
	return 1;
}

public Cmd_accettacorsa(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Taxi_CmdAccettacorsa(playerid, params);
	}
	return 1;
}

public Cmd_tassametro(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Taxi_CmdTassametro(playerid, params);
	}
	return 1;
}

public Cmd_contratti(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		CityOS_CmdContratti(playerid, params);
	}
	return 1;
}

public Cmd_contratto(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		CityOS_CmdContratto(playerid, params);
	}
	return 1;
}

public Cmd_reputazione(playerid, const params[])
{
	if (!Cmd_LivingGate(playerid))
	{
		return 1;
	}
	CityOS_CmdReputazione(playerid, params);
	LD_OnboardAction(playerid, 4);
	return 1;
}

public Cmd_quartiere(playerid, const params[])
{
	if (!Cmd_LivingGate(playerid))
	{
		return 1;
	}
	CityOS_CmdQuartiere(playerid, params);
	LD_OnboardAction(playerid, 2);
	return 1;
}

public Cmd_contribuisci(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Hub_CmdContribuisci(playerid, params);
	}
	return 1;
}

public Cmd_email(playerid, const params[])
{
	Account_CmdEmail(playerid, params);
	return 1;
}

public Cmd_telefono(playerid, const params[])
{
	CefBridge_CmdTelefono(playerid);
	return 1;
}

public Cmd_aiuto(playerid, const params[])
{
	if (Cmd_LivingGate(playerid))
	{
		Hub_CmdAiuto(playerid, params);
	}
	return 1;
}

public Cmd_ldset(playerid, const params[])
{
	return District_CmdLdset(playerid, params) ? 1 : 0;
}

public Cmd_chatsamp(playerid, const params[])
{
	HudCef_CmdChat(playerid, false);
	return 1;
}

public Cmd_chatcef(playerid, const params[])
{
	HudCef_CmdChat(playerid, true);
	return 1;
}
