// =============================================================================
// voice/voice.inc - chat vocale con SampVoice per open.mp (AmyrAhmady/sampvoice, versione x.x.x-omp), opzionale.
//
// Compilato solo con LSRP_VOICE (il server deve caricare il plugin SampVoice, altrimenti open.mp
// rifiuta il gamemode per natives mancanti). Senza LSRP_VOICE tutte le funzioni sono vuote.
//  - voce locale di prossimita' (tasto B): stream a distanza agganciato al giocatore;
//  - radio di fazione (tasto Z): un canale per fazione, solo per chi e' in servizio.
// Client: SampVoice per SA-MP 0.3.7 R1/R3 (vedi docs/HUD_GLASS_UI.md).
// =============================================================================

#define VOICE_LOCAL_DIST   (25.0)
#define VOICE_KEY_LOCAL    (0x42)   // B
#define VOICE_KEY_RADIO    (0x5A)   // Z
#define VOICE_CH_LOCAL     (0)
#define VOICE_CH_RADIO     (1)
#define VOICE_MAX_FACTIONS (16)

#if defined LSRP_VOICE

#include <sampvoice>

new SV_DLSTREAM:g_VoiceLocal[MAX_PLAYERS] = { SV_NULL, ... };
new SV_GSTREAM:g_VoiceRadio[VOICE_MAX_FACTIONS] = { SV_NULL, ... };
new g_VoiceRadioOf[MAX_PLAYERS];        // fazione della radio che ascolta (0 = nessuna)
new bool:g_VoiceRadioTalk[MAX_PLAYERS]; // sta parlando in radio (Z premuto)
new g_VoiceTimer;

Voice_OnInit()
{
	for (new f = 1; f < VOICE_MAX_FACTIONS; f++)
	{
		g_VoiceRadio[f] = SvCreateGStream(0x5BA8FFFF, f == 2 ? "Radio EMS" : "Radio LSPD");
	}
	g_VoiceTimer = SetTimer("Voice_OnTick", 1000, true);
	Log_Write("VOICE", "SampVoice attivo: voce locale (B) e radio di fazione (Z)");
}

Voice_OnExit()
{
	if (g_VoiceTimer)
	{
		KillTimer(g_VoiceTimer);
		g_VoiceTimer = 0;
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		Voice_OnPlayerDisconnect(p);
	}
	for (new f = 1; f < VOICE_MAX_FACTIONS; f++)
	{
		if (g_VoiceRadio[f])
		{
			SvDeleteStream(g_VoiceRadio[f]);
			g_VoiceRadio[f] = SV_NULL;
		}
	}
}

// 0 plugin assente sul client, 1 nessun microfono, 2 pronto
Voice_State(playerid)
{
	if (SvGetVersion(playerid) == SV_NULL)
	{
		return 0;
	}
	return SvHasMicro(playerid) ? 2 : 1;
}

// la voce si abilita quando il personaggio e' in gioco (dopo il login): niente voce durante registrazione/intro
Voice_OnPlayerSpawn(playerid)
{
	if (Voice_State(playerid) != 2 || g_VoiceLocal[playerid])
	{
		return 0;
	}
	g_VoiceLocal[playerid] = SvCreateDLStreamAtPlayer(VOICE_LOCAL_DIST, SV_INFINITY, playerid, 0xF6F3FAFF, "Locale");
	if (!g_VoiceLocal[playerid])
	{
		return 0;
	}
	SvAddKey(playerid, VOICE_KEY_LOCAL);
	SvAddKey(playerid, VOICE_KEY_RADIO);
	return 1;
}

Voice_OnPlayerDisconnect(playerid)
{
	Voice_RadioLeave(playerid);
	if (g_VoiceLocal[playerid])
	{
		SvDetachSpeakerFromStream(g_VoiceLocal[playerid], playerid);
		SvDeleteStream(g_VoiceLocal[playerid]);
		g_VoiceLocal[playerid] = SV_NULL;
	}
}

Voice_RadioLeave(playerid)
{
	new f = g_VoiceRadioOf[playerid];
	g_VoiceRadioTalk[playerid] = false;
	if (f <= 0 || f >= VOICE_MAX_FACTIONS)
	{
		return 0;
	}
	if (g_VoiceRadio[f])
	{
		SvDetachSpeakerFromStream(g_VoiceRadio[f], playerid);
		SvDetachListenerFromStream(g_VoiceRadio[f], playerid);
	}
	g_VoiceRadioOf[playerid] = 0;
	return 1;
}

// B = voce locale, Z = radio (solo chi ascolta la radio della propria fazione, cioe' in servizio)
public SV_VOID:OnPlayerActivationKeyPress(SV_UINT:playerid, SV_UINT:keyid)
{
	if (playerid >= MAX_PLAYERS || !gPlayer[playerid][pSpawned])
	{
		return;
	}
	if (keyid == VOICE_KEY_LOCAL && g_VoiceLocal[playerid])
	{
		SvAttachSpeakerToStream(g_VoiceLocal[playerid], playerid);
	}
	else if (keyid == VOICE_KEY_RADIO && g_VoiceRadioOf[playerid] > 0 && g_VoiceRadio[g_VoiceRadioOf[playerid]])
	{
		SvAttachSpeakerToStream(g_VoiceRadio[g_VoiceRadioOf[playerid]], playerid);
		g_VoiceRadioTalk[playerid] = true;
	}
}

public SV_VOID:OnPlayerActivationKeyRelease(SV_UINT:playerid, SV_UINT:keyid)
{
	if (playerid >= MAX_PLAYERS)
	{
		return;
	}
	if (keyid == VOICE_KEY_LOCAL && g_VoiceLocal[playerid])
	{
		SvDetachSpeakerFromStream(g_VoiceLocal[playerid], playerid);
	}
	else if (keyid == VOICE_KEY_RADIO && g_VoiceRadioOf[playerid] > 0 && g_VoiceRadio[g_VoiceRadioOf[playerid]])
	{
		SvDetachSpeakerFromStream(g_VoiceRadio[g_VoiceRadioOf[playerid]], playerid);
		g_VoiceRadioTalk[playerid] = false;
	}
}

// radio: in ascolto se membro di fazione IN SERVIZIO (stessa regola di /r), scollegata altrimenti
public Voice_OnTick()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (!IsPlayerConnected(p) || !g_VoiceLocal[p])
		{
			continue;
		}
		new f = gPlayer[p][pFaction];
		new want = (gPlayer[p][pSpawned] && gPlayer[p][pOnDuty] && f > 0 && f < VOICE_MAX_FACTIONS) ? f : 0;
		if (want == g_VoiceRadioOf[p])
		{
			continue;
		}
		Voice_RadioLeave(p);
		if (want > 0 && g_VoiceRadio[want])
		{
			SvAttachListenerToStream(g_VoiceRadio[want], p);
			g_VoiceRadioOf[p] = want;
		}
	}
}

#else

Voice_OnInit() {}
Voice_OnExit() {}
Voice_State(playerid)
{
	#pragma unused playerid
	return 0;
}
Voice_OnPlayerSpawn(playerid)
{
	#pragma unused playerid
}
Voice_OnPlayerDisconnect(playerid)
{
	#pragma unused playerid
}

#endif
