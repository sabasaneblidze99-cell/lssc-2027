new bool:g_CosWaitBoard[MAX_PLAYERS];
new g_CosRumor[3][8][] = {
	{"La panetteria di Idlewood ha riaperto dopo l'estate: il pane esce caldo alle sei.", "Alla Casa del Quartiere cercano volontari per il doposcuola del giovedi'.", "Il campetto dietro la stazione ha di nuovo le reti: qualcuno le ha ricucite a mano.", "Un vecchio meccanico ripara bici gratis ai ragazzi, basta portargli un caffe'.", "Le signore del palazzo giallo organizzano una cena di quartiere sabato sera.", "Il negozio all'angolo ha appeso una lavagna: chi ha bisogno lascia un biglietto.", "Dicono che il murale vicino alla fermata sara' restaurato dagli stessi artisti.", "Chi esce dal turno di notte trova sempre una luce accesa alla Casa del Quartiere."},
	{"A Ganton il barbiere sotto i portici taglia i capelli ai ragazzi prima degli esami.", "Il negozio di Ganton aspetta un carico di frutta: chi consegna e' il benvenuto.", "Sulla Grove hanno ridipinto le panchine, ognuna con un colore diverso.", "Un gruppo di madri accompagna i bambini a scuola a turno, a piedi.", "Il torneo di basket del venerdi' quest'anno raccoglie fondi per il campetto.", "Un ex corriere ha aperto una piccola officina: assume chi vuole imparare.", "Le luci della via principale sono tornate: i commercianti hanno fatto colletta.", "Dicono che il vecchio cinema all'aperto potrebbe tornare per una sera d'estate."},
	{"Al magazzino di Jefferson servono autisti puntuali: chi lavora bene viene ricordato.", "Il motel sulla collina offre la colazione a chi aiuta a scaricare le merci.", "Un coro di quartiere prova ogni martedi' nella sala della chiesa.", "La lavanderia a gettoni e' diventata il posto dove ci si scambia le notizie.", "I ragazzi del quartiere puliscono il parco ogni prima domenica del mese.", "Un fornaio di Jefferson regala le pagnotte invendute alla mensa la sera.", "La fermata dell'autobus ha una nuova pensilina: costruita da chi la usa.", "Dicono che chi consegna con cura a Jefferson trova sempre una porta aperta."}
};

bool:CityOS_Playing(playerid)
{
	return Util_IsPlaying(playerid) && gPlayer[playerid][pLivingReady] != 0 && LD_IsReady();
}

CityOS_Integrity(playerid)
{
	new veh = gPlayer[playerid][pFreightVehicle];
	if (veh <= 0 || veh >= 2000 || LD_VehModule(veh) != 3 || LD_VehOwner(veh) != playerid)
	{
		return -1;
	}
	new Float:h;
	GetVehicleHealth(veh, h);
	return LD_Clamp(floatround(h, 1) / 10, 0, 100);
}

CityOS_NextDistance(playerid)
{
	new d = gPlayer[playerid][pContractPhase] == 2 ? gPlayer[playerid][pContractToDistrict] : gPlayer[playerid][pContractFromDistrict];
	new Float:tx, Float:ty, Float:px, Float:py, Float:pz;
	new bool:shop = gPlayer[playerid][pContractPhase] == 2;
	switch (d)
	{
		case 1:
		{
			tx = Float:((shop ? 1927.0 : 0x44EB0000));
			ty = Float:((shop ? 0xC4DFC000 : 0xC4DE8000));
		}
		case 2:
		{
			tx = Float:((shop ? 0x451BA000 : 0x45160000));
			ty = Float:((shop ? 0xC4D34000 : 0xC4D98000));
		}
		case 3:
		{
			tx = Float:((shop ? 0x4508E000 : 2145.0));
			ty = Float:((shop ? -1155.0 : -1215.0));
		}
		default:
		{
			return -1;
		}
	}
	GetPlayerPos(playerid, px, py, pz);
	new Float:dx = (tx - px), Float:dy = (ty - py);
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), 1);
}

CityOS_ShowBoard(playerid)
{
	if (!CityOS_Playing(playerid))
	{
		return 0;
	}
	new now = gettime(), n = Director_BoardCount();
	UI_CityBoardBegin(playerid);
	new id = 0, fromD = 0, toD = 0, cargo = 0, reward = 0, meters = 0, expires = 0, minLevel = 0, minRep = 0, minLocal = 0, premium = 0, rows = 0;
	for (new i = 0; i < n && rows < 12; i++)
	{
		if (!Director_BoardVisible(playerid, i))
		{
			continue;
		}
		if (!Director_BoardGet(i, id, fromD, toD, cargo, reward, meters, expires, minLevel, minRep, minLocal, premium))
		{
			continue;
		}
		UI_CityBoardAdd(playerid, id, 1, fromD, toD, cargo, reward, meters, expires - now, premium != 0, minLevel, minRep, minLocal, Director_BlockCode(playerid, i));
		rows++;
	}
	UI_CityBoardShow(playerid, Hub_NearBoard(playerid) >= 0);
}

CityOS_ShowDistrict(playerid, districtId)
{
	if (!CityOS_Playing(playerid))
	{
		return 0;
	}
	if (districtId < 1 || districtId > 3)
	{
		UI_CityDistrictPick(playerid);
		return 0;
	}
	DB_LivingHistory(playerid, districtId);
}

CityOS_RenderDistrict(playerid, districtId, count)
{
	new now = gettime(), fest = DB_LivingDistrictGet(districtId, 4), festH = 0;
	if (fest > now)
	{
		festH = (fest - now + 3600 - 1) / 3600;
	}
	UI_CityDistrictBegin(playerid, districtId, District_GetActivity(districtId), District_GetSecurity(districtId), District_GetCrime(districtId), LD_GetLocalRep(playerid, districtId), festH);
	new r = LD_HashMod(LD_Hash32(LD_Bucket(now), districtId, 3), 8);
	UI_CityDistrictRumor(playerid, g_CosRumor[districtId - 1][r]);
	for (new i = 0; i < count && i < 8; i++)
	{
		new ago = now - g_LdHist[i][lhTimestamp];
		UI_CityDistrictEvent(playerid, g_LdHist[i][lhType], ago > 0 ? ago : 0, g_LdHist[i][lhActDelta]);
	}
	UI_CityDistrictShow(playerid);
}

CityOS_OnInit()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		g_CosWaitBoard[i] = 0;
	}
	LD_Listen("CityOS_OnDistrictEvent", 30);
}

CityOS_OnExit()
{
}

CityOS_OnPlayerConnect(playerid)
{
	g_CosWaitBoard[playerid] = 0;
}

CityOS_OnCharReady(playerid)
{
	g_CosWaitBoard[playerid] = 0;
}

CityOS_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	g_CosWaitBoard[playerid] = 0;
}

public CityOS_OnDistrictEvent(type, districtId, charId, value, const opId[])
{
	if (type != 104)
	{
		return 1;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!g_CosWaitBoard[i])
		{
			continue;
		}
		g_CosWaitBoard[i] = 0;
		if (IsPlayerConnected(i))
		{
			CityOS_ShowBoard(i);
		}
	}
	return 1;
}

CityOS_CmdContratti(playerid, const params[])
{
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tok[24], num[24];
	Util_NextToken(params, pos, tok, 24);
	if (!strcmp(tok, "accetta", 1, 0x7FFFFFFF) && tok[0] != 0)
	{
		new id = 0;
		Util_NextToken(params, pos, num, 24);
		if (!Util_ParseInt(num, id) || id <= 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "Uso: /contratti accetta <numero contratto>");
			return 1;
		}
		Director_Accept(playerid, id);
		return 1;
	}
	CityOS_ShowBoard(playerid);
	return 1;
}

CityOS_CmdContratto(playerid, const params[])
{
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tok[24];
	Util_NextToken(params, pos, tok, 24);
	new ct = gPlayer[playerid][pContractId];
	new bool:active = ct != 0 && (gPlayer[playerid][pContractPhase] == 1 || gPlayer[playerid][pContractPhase] == 2);
	if (tok[0] != 0 && !strcmp(tok, "abbandona", 1, 0x7FFFFFFF))
	{
		if (!active)
		{
			Util_Send(playerid, -10270721, "Non hai un contratto da lasciare.");
			return 1;
		}
		Director_Release(playerid, 1);
		return 1;
	}
	if (!active)
	{
		if (ct != 0)
		{
			Util_Send(playerid, -1, "La tua richiesta di contratto e' in elaborazione: attendi un istante.");
		}
		else
		{
			UI_CityContractNone(playerid);
		}
		return 1;
	}
	new exp = Director_ExpiresAt(playerid), left = -1;
	if (exp > 0)
	{
		left = exp > gettime() ? exp - gettime() : 0;
	}
	UI_CityContractStatus(playerid, ct, gPlayer[playerid][pContractPhase], gPlayer[playerid][pContractFromDistrict], gPlayer[playerid][pContractToDistrict], gPlayer[playerid][pContractCargo], gPlayer[playerid][pContractReward], left, CityOS_Integrity(playerid), CityOS_NextDistance(playerid), -5);
	return 1;
}

CityOS_CmdReputazione(playerid, const params[])
{
	#pragma unused params
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	UI_CityRepBegin(playerid);
	UI_CityRepTrack(playerid, 1, LD_GetProPoints(playerid, 1), 0, 0, 0, 0, 0);
	new rating = DB_LivingPlayerGet(playerid, 3);
	UI_CityRepTrack(playerid, 2, LD_GetProPoints(playerid, 2), 0, 0, rating, rating > 0 ? 100 : 0, 0);
	for (new d = 1; d <= 3; d++)
	{
		UI_CityRepLocal(playerid, d, LD_GetLocalRep(playerid, d));
	}
	UI_CityRepShow(playerid);
	return 1;
}

CityOS_CmdQuartiere(playerid, const params[])
{
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tok[24], d = 0;
	Util_NextToken(params, pos, tok, 24);
	if (tok[0] != 0)
	{
		d = LD_ParseDistrict(tok);
		if (d == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "Uso: /quartiere [idlewood|ganton|jefferson]");
			return 1;
		}
	}
	else
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		d = GetPlayerInterior(playerid) == 0 ? District_At(x, y) : 0;
	}
	CityOS_ShowDistrict(playerid, d);
	return 1;
}

public Living_OnDBHistory(playerid, result, districtId, count)
{
	if (!CityOS_Playing(playerid) || districtId < 1 || districtId > 3)
	{
		return 1;
	}
	if (result == 10)
	{
		Util_Send(playerid, -10270721, "Sto ancora caricando i tuoi dati: riprova tra un istante.");
		return 1;
	}
	CityOS_RenderDistrict(playerid, districtId, result == 1 ? count : 0);
	return 1;
}

public UI_OnContractAccept(playerid, contractId)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	Director_Accept(playerid, contractId);
	return 1;
}

public UI_OnContractAbandon(playerid, contractId)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	if (contractId <= 0 || gPlayer[playerid][pContractId] != contractId)
	{
		Util_Send(playerid, -10270721, "Questo contratto non e' piu' attivo.");
		return 1;
	}
	Director_Release(playerid, 1);
	return 1;
}

public UI_OnContractBoardRefresh(playerid)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	if (!DB_IsReady())
	{
		CityOS_ShowBoard(playerid);
		return 1;
	}
	g_CosWaitBoard[playerid] = 1;
	DB_LivingBoard(65535);
	return 1;
}

public UI_OnDistrictPick(playerid, districtId)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	CityOS_ShowDistrict(playerid, districtId);
	return 1;
}
