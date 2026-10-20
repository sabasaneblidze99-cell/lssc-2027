// =============================================================================
// voice/voice.inc - ponte verso la chat vocale (filterscripts/lsrp_voce.pwn).
//
// La voce NON e' nel gamemode: se il componente SampVoice manca, il gamemode parte lo stesso.
// Qui solo i dati per il filterscript e lo stato per l'HUD.
// =============================================================================

Voice_OnInit() {}
Voice_OnExit() {}
Voice_OnPlayerSpawn(playerid)
{
	#pragma unused playerid
}
Voice_OnPlayerDisconnect(playerid)
{
	#pragma unused playerid
}

// 0 voce assente (filterscript o plugin client mancante), 1 niente microfono, 2 pronto
Voice_State(playerid)
{
	return CallRemoteFunction("VoiceFS_State", "d", playerid);
}

// dati per il filterscript della voce
public LSRP_VoiceInfo(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	new f = gPlayer[playerid][pFaction];
	return (gPlayer[playerid][pSpawned] ? 1 : 0) | (gPlayer[playerid][pOnDuty] ? 2 : 0) | ((f & 0xFF) << 8);
}
