
public Inv_ShowDocument(playerid, viewerid)
{
	if (!Inv_ValidPlayer(playerid) || !Inv_ValidPlayer(viewerid))
	{
		return 1;
	}
	new name[25], job[24], fac[24], rank[24];
	GetPlayerName(playerid, name, 25);
	Job_NameOf(gPlayer[playerid][pJob], job, 24);
	Fac_ShortName(gPlayer[playerid][pFaction], fac, 24);
	Fac_RankName(gPlayer[playerid][pFaction], gPlayer[playerid][pFactionRank], rank, 24);
	new total = gPlayer[playerid][pPlayedMs];
	SendClientMessage(viewerid, -2994689, "--- Documento d'identita' di San Andreas ---");
	SendClientMessage(viewerid, -1, "Nome: %s  Livello: %d  Ore di gioco: %dh %02dm", name, gPlayer[playerid][pLevel], total / 3600000, total % 3600000 / 60000);
	new origin[24];
	UI_StoryOriginName(gPlayer[playerid][pStoryOrigin], origin, 24);
	if (origin[0] != 0)
	{
		SendClientMessage(viewerid, -1, "Origine: %s", origin);
	}
	if (gPlayer[playerid][pFaction] > 0)
	{
		SendClientMessage(viewerid, -1, "Lavoro: %s  Fazione: %s (%s)", job, fac, rank);
	}
	else
	{
		SendClientMessage(viewerid, -1, "Lavoro: %s  Fazione: nessuna", job);
	}
	return 1;
}

Inv_CmdDocumento(playerid, const params[])
{
	new pos = 0, tok[24], target = 0;
	Util_NextToken(params, pos, tok, 24);
	if (!strcmp(tok, "richiedi", 1, 0x7FFFFFFF))
	{
		Inv_DocRequest(playerid);
		return 0;
	}
	if (!Inv_Ready(playerid))
	{
		Util_Send(playerid, -10270721, "Inventario non disponibile: riprova tra poco (/inventario).");
		return 0;
	}
	if (Inv_Count(playerid, 10) < 1)
	{
		Util_Send(playerid, -10270721, "Non hai il documento d'identita': usa /documento richiedi.");
		return 0;
	}
	if (tok[0] == 0)
	{
		Inv_ShowDocument(playerid, playerid);
		return 0;
	}
	if (!Util_ParsePlayerId(tok, target) || !Util_IsPlaying(target))
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /documento · /documento [id] · /documento richiedi");
		return 0;
	}
	if (target != playerid && !Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "Il giocatore e' troppo lontano.");
		return 0;
	}
	new tname[25];
	GetPlayerName(target, tname, 25);
	Inv_ShowDocument(playerid, target);
	if (target != playerid)
	{
		SendClientMessage(playerid, 0x66DD66FF, "Hai mostrato il documento a %s.", tname);
	}
	return 0;
}
