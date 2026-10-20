// =============================================================================
// filterscripts/lsrp_voce.pwn - chat vocale LSSC (SampVoice per open.mp), SEPARATA dal gamemode.
//
// Perche' separata: se il componente SampVoice (-omp) manca o e' sbagliato, open.mp rifiuta solo
// questo filterscript; il gamemode lsrp funziona comunque (senza voce).
// Il gamemode fornisce i dati con la public LSRP_VoiceInfo(playerid):
//   bit 0 = in gioco (spawn fatto), bit 1 = in servizio, bit 8-15 = fazione.
// L'HUD chiede lo stato con VoiceFS_State(playerid): 0 niente plugin client, 1 niente microfono, 2 pronto.
// Tasti: B voce locale (25 m), Z radio della fazione (solo in servizio).
// config.json: "side_scripts": ["filterscripts/lsrp_voce"]; componente sampvoice in components/.
// =============================================================================
#include <open.mp>
#include <sampvoice>

#define VOICE_LOCAL_DIST   (25.0)
#define VOICE_KEY_LOCAL    (0x42)   // B
#define VOICE_KEY_RADIO    (0x5A)   // Z
#define VOICE_MAX_FACTIONS (16)

forward Voice_OnTick();
forward VoiceFS_State(playerid);

VoiceFS_Info(playerid)
{
	return CallRemoteFunction("LSRP_VoiceInfo", "d", playerid);
}

public VoiceFS_State(playerid)
{
	return Voice_State(playerid);
}

public OnFilterScriptInit()
{
	Voice_OnInit();
	return 1;
}

public OnFilterScriptExit()
{
	Voice_OnExit();
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	Voice_OnPlayerDisconnect(playerid);
	return 1;
}


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
	print("[VOICE] SampVoice attivo: voce locale (B) e radio di fazione (Z)");
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
	if (Voice_State(playerid) != 2 || g_VoiceLocal[playerid] || !(VoiceFS_Info(playerid) & 1))
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
	if (playerid >= MAX_PLAYERS || !(VoiceFS_Info(playerid) & 1))
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
		new info = VoiceFS_Info(p), f = (info >> 8) & 0xFF;
		if ((info & 1) && !g_VoiceLocal[p])
		{
			Voice_OnPlayerSpawn(p);       // in gioco (dopo login/intro): si crea la voce locale
		}
		new want = ((info & 1) && (info & 2) && f > 0 && f < VOICE_MAX_FACTIONS) ? f : 0;
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

