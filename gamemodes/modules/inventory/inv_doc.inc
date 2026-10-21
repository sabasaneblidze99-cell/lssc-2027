
public Inv_ShowDocument(playerid, viewerid)
{
	if (!Inv_ValidPlayer(playerid) || !Inv_ValidPlayer(viewerid))
	{
		return 1;
	}
	new name[75], job[72], fac[72], rank[72];
	GetPlayerName(playerid, name, 75);
	Job_NameOf(gPlayer[playerid][pJob], job, 72);
	Fac_ShortName(gPlayer[playerid][pFaction], fac, 72);
	Fac_RankName(gPlayer[playerid][pFaction], gPlayer[playerid][pFactionRank], rank, 72);
	new total = gPlayer[playerid][pPlayedMs];
	SendClientMessage(viewerid, -2994689, "--- სან-ანდრეასის პირადობის მოწმობა ---");
	SendClientMessage(viewerid, -1, "სახელი: %s  დონე: %d  ნათამაშები: %dსთ %02dწთ", name, gPlayer[playerid][pLevel], total / 3600000, total % 3600000 / 60000);
	new origin[72];
	UI_StoryOriginName(gPlayer[playerid][pStoryOrigin], origin, 72);
	if (origin[0] != 0)
	{
		SendClientMessage(viewerid, -1, "წარმომავლობა: %s", origin);
	}
	if (gPlayer[playerid][pFaction] > 0)
	{
		SendClientMessage(viewerid, -1, "სამსახური: %s  ფრაქცია: %s (%s)", job, fac, rank);
	}
	else
	{
		SendClientMessage(viewerid, -1, "სამსახური: %s  ფრაქცია: არა", job);
	}
	return 1;
}

Inv_CmdDocumento(playerid, const params[])
{
	new pos = 0, tok[72], target = 0;
	Util_NextToken(params, pos, tok, 72);
	if (!strcmp(tok, "richiedi", true, 0x7FFFFFFF))
	{
		Inv_DocRequest(playerid);
		return 0;
	}
	if (!Inv_Ready(playerid))
	{
		Util_Send(playerid, -10270721, "ინვენტარი მიუწვდომელია: ცოტა ხანში სცადე (/inventario).");
		return 0;
	}
	if (Inv_Count(playerid, 10) < 1)
	{
		Util_Send(playerid, -10270721, "პირადობის მოწმობა არ გაქვს: გამოიყენე /documento richiedi.");
		return 0;
	}
	if (tok[0] == 0)
	{
		Inv_ShowDocument(playerid, playerid);
		return 0;
	}
	if (!Util_ParsePlayerId(tok, target) || !Util_IsPlaying(target))
	{
		Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /documento · /documento [id] · /documento richiedi");
		return 0;
	}
	if (target != playerid && !Util_InRange(playerid, target, 3.0))
	{
		Util_Send(playerid, -10270721, "მოთამაშე ძალიან შორსაა.");
		return 0;
	}
	new tname[75];
	GetPlayerName(target, tname, 75);
	Inv_ShowDocument(playerid, target);
	if (target != playerid)
	{
		SendClientMessage(playerid, 0x66DD66FF, "დოკუმენტი აჩვენე %s-ს.", tname);
	}
	return 0;
}
