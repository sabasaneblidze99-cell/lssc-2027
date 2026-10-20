new g_FrtTimer;
new bool:g_FrtActive[MAX_PLAYERS];
new bool:g_FrtWelcome[MAX_PLAYERS];
new g_FrtCt[MAX_PLAYERS];
new g_FrtFrom[MAX_PLAYERS];
new g_FrtTo[MAX_PLAYERS];
new g_FrtCargo[MAX_PLAYERS];
new g_FrtDist[MAX_PLAYERS];
new g_FrtPhase[MAX_PLAYERS];
new g_FrtVeh[MAX_PLAYERS];
new g_FrtSerial[MAX_PLAYERS];
new bool:g_FrtBoarded[MAX_PLAYERS];
new bool:g_FrtDwell[MAX_PLAYERS];
new bool:g_FrtOff[MAX_PLAYERS];
new g_FrtStartTick[MAX_PLAYERS];
new g_FrtWaitTick[MAX_PLAYERS];
new g_FrtDefer[MAX_PLAYERS];
new g_FrtInteg[MAX_PLAYERS];
new Float:g_FrtPath[MAX_PLAYERS];
new Float:g_FrtLastX[MAX_PLAYERS];
new Float:g_FrtLastY[MAX_PLAYERS];
new Float:g_FrtLastZ[MAX_PLAYERS];
new g_FrtLastTick[MAX_PLAYERS];
new g_FrtRetryTick[MAX_PLAYERS];
new g_FrtStateRetry[MAX_PLAYERS];
new g_FrtNoteTick[MAX_PLAYERS];

bool:Freight_ValidId(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS;
}

Freight_ResetPlayer(playerid)
{
	g_FrtActive[playerid] = 0;
	g_FrtWelcome[playerid] = 0;
	g_FrtCt[playerid] = 0;
	g_FrtFrom[playerid] = 0;
	g_FrtTo[playerid] = 0;
	g_FrtCargo[playerid] = 0;
	g_FrtDist[playerid] = 0;
	g_FrtPhase[playerid] = 0;
	g_FrtVeh[playerid] = 65535;
	g_FrtSerial[playerid] = 0;
	g_FrtBoarded[playerid] = 0;
	g_FrtDwell[playerid] = 0;
	g_FrtOff[playerid] = 0;
	g_FrtStartTick[playerid] = 0;
	g_FrtWaitTick[playerid] = 0;
	g_FrtDefer[playerid] = 0;
	g_FrtInteg[playerid] = 100;
	g_FrtPath[playerid] = 0.0;
	g_FrtLastX[playerid] = 0.0;
	g_FrtLastY[playerid] = 0.0;
	g_FrtLastZ[playerid] = 0.0;
	g_FrtLastTick[playerid] = 0;
	g_FrtRetryTick[playerid] = 0;
	g_FrtStateRetry[playerid] = 0;
	g_FrtNoteTick[playerid] = 0;
}

bool:Freight_Point(d, bool:shop, &Float:x, &Float:y, &Float:z)
{
	switch (d)
	{
		case 1:
		{
			if (shop)
			{
				x = 1927.0;
				y = Float:0xC4DFC000;
				z = 13.5;
			}
			else
			{
				x = Float:0x44EB0000;
				y = Float:0xC4DE8000;
				z = 13.5;
			}
		}
		case 2:
		{
			if (shop)
			{
				x = Float:0x451BA000;
				y = Float:0xC4D34000;
				z = 13.5;
			}
			else
			{
				x = Float:0x45160000;
				y = Float:0xC4D98000;
				z = 13.5;
			}
		}
		case 3:
		{
			if (shop)
			{
				x = Float:0x4508E000;
				y = -1155.0;
				z = 24.0;
			}
			else
			{
				x = 2145.0;
				y = -1215.0;
				z = 24.0;
			}
		}
		default:
		{
			x = 0.0;
			y = 0.0;
			z = 0.0;
			return 0;
		}
	}
	return 1;
}

Freight_PairDistance(fromD, toD)
{
	new Float:ax, Float:ay, Float:az, Float:bx, Float:by, Float:bz;
	if (!Freight_Point(fromD, 0, ax, ay, az) || !Freight_Point(toD, 1, bx, by, bz))
	{
		return 0;
	}
	new Float:dx = (bx - ax), Float:dy = (by - ay);
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), 1);
}

Freight_CargoName(cargo, dest[], size)
{
	switch (cargo)
	{
		case 2:
		{
			strcopy(dest, "merci alimentari", size);
		}
		case 3:
		{
			strcopy(dest, "materiali", size);
		}
		default:
		{
			strcopy(dest, "merci generiche", size);
		}
	}
}

Freight_SecCeil(ms)
{
	if (ms <= 0)
	{
		return 0;
	}
	return (ms + 1000 - 1) / 1000;
}

Freight_Note(playerid, text[])
{
	new now = GetTickCount();
	if (g_FrtNoteTick[playerid] != 0 && now - g_FrtNoteTick[playerid] < 4000)
	{
		return 0;
	}
	g_FrtNoteTick[playerid] = now;
	Util_Send(playerid, -10270721, text);
}

Freight_IntegrityOf(vehicleid)
{
	new Float:hp;
	GetVehicleHealth(vehicleid, hp);
	return LD_Clamp(floatround((hp / Float:0x41200000), 1), 0, 100);
}

bool:Freight_VehOk(playerid)
{
	new veh = g_FrtVeh[playerid];
	if (veh == 65535 || veh <= 0 || veh >= 2000 || !IsValidVehicle(veh))
	{
		return 0;
	}
	return LD_VehModule(veh) == 3 && LD_VehOwner(veh) == playerid && LD_VehSerial(veh) == g_FrtSerial[playerid];
}

bool:Freight_IsDriving(playerid)
{
	return Freight_VehOk(playerid) && GetPlayerVehicleID(playerid) == g_FrtVeh[playerid] && GetPlayerState(playerid) == 2;
}

Freight_RemoveVehicle(playerid)
{
	new veh = g_FrtVeh[playerid];
	if (veh != 65535 && veh > 0 && veh < 2000)
	{
		if (LD_VehModule(veh) == 3 && LD_VehOwner(veh) == playerid && LD_VehSerial(veh) == g_FrtSerial[playerid])
		{
			LD_VehRelease(veh);
			if (IsValidVehicle(veh))
			{
				if (IsPlayerConnected(playerid) && GetPlayerVehicleID(playerid) == veh)
				{
					RemovePlayerFromVehicle(playerid, 0);
				}
				DestroyVehicle(veh);
			}
		}
	}
	g_FrtVeh[playerid] = 65535;
	g_FrtSerial[playerid] = 0;
	gPlayer[playerid][pFreightVehicle] = 0;
}

Freight_ShowCheckpoint(playerid)
{
	if (!IsPlayerConnected(playerid))
	{
		return 0;
	}
	new Float:x, Float:y, Float:z;
	new bool:shop = g_FrtPhase[playerid] >= 3;
	if (!Freight_Point(shop ? g_FrtTo[playerid] : g_FrtFrom[playerid], shop, x, y, z))
	{
		return 0;
	}
	DisablePlayerCheckpoint(playerid);
	SetPlayerCheckpoint(playerid, x, y, z, 6.0);
}

Freight_TimeLeft(playerid)
{
	if (g_FrtPhase[playerid] < 3)
	{
		return -1;
	}
	new left = LD_FreightTimeLimit(g_FrtDist[playerid]) - (GetTickCount() - gPlayer[playerid][pFreightLegTick]) / 1000;
	return left > 0 ? left : 0;
}

Freight_Hud(playerid, distM, dwellSec)
{
	if (g_FrtWelcome[playerid] || g_FrtCt[playerid] <= 0)
	{
		return 0;
	}
	new name[16], goal[96], dest[96];
	new bool:shop = g_FrtPhase[playerid] >= 3;
	LD_DistrictName(shop ? g_FrtTo[playerid] : g_FrtFrom[playerid], name, 16);
	format(dest, 96, "%s %s", shop ? "Negozio" : "Magazzino", name);
	switch (g_FrtPhase[playerid])
	{
		case 2:
		{
			strcopy(goal, "Registrazione del carico...", 96);
		}
		case 4:
		{
			strcopy(goal, "Registrazione della consegna...", 96);
		}
		case 3:
		{
			if (dwellSec >= 0)
			{
				format(goal, 96, "Scarico: resta fermo %d s", dwellSec);
			}
			else if (g_FrtOff[playerid])
			{
				strcopy(goal, "Torna alla guida del Mule!", 96);
			}
			else
			{
				strcopy(goal, "Consegna la merce al negozio", 96);
			}
		}
		default:
		{
			if (dwellSec >= 0)
			{
				format(goal, 96, "Carico: resta fermo %d s", dwellSec);
			}
			else if (!g_FrtBoarded[playerid])
			{
				strcopy(goal, "Sali sul Mule al magazzino", 96);
			}
			else if (g_FrtOff[playerid])
			{
				strcopy(goal, "Torna alla guida del Mule!", 96);
			}
			else
			{
				strcopy(goal, "Entra nel checkpoint del magazzino", 96);
			}
		}
	}
	UI_CityContractHud(playerid, g_FrtCt[playerid], shop ? 2 : 1, goal, dest, distM, Freight_TimeLeft(playerid), shop ? g_FrtInteg[playerid] : -1);
}

Freight_End(playerid, reason)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid])
	{
		return 0;
	}
	new bool:welcome = g_FrtWelcome[playerid];
	new bool:online = IsPlayerConnected(playerid) != 0;
	Freight_RemoveVehicle(playerid);
	if (online)
	{
		DisablePlayerCheckpoint(playerid);
		if (!welcome)
		{
			UI_CityContractHudHide(playerid);
		}
		switch (reason)
		{
			case 1:
			{
				Util_Send(playerid, -1, "Contratto di trasporto chiuso: il Mule e' stato ritirato.");
			}
			case 5:
			{
				Util_Send(playerid, -10270721, "Contratto di trasporto scaduto o non piu' valido: il Mule e' stato ritirato.");
			}
			case 7:
			{
				Util_Send(playerid, -10270721, "Contratto di trasporto annullato dallo staff.");
			}
		}
	}
	gPlayer[playerid][pFreightLegTick] = 0;
	gPlayer[playerid][pFreightDwellTick] = 0;
	gPlayer[playerid][pFreightLeftVehTick] = 0;
	if (welcome)
	{
		gPlayer[playerid][pFreightWelcome] = 0;
	}
	Freight_ResetPlayer(playerid);
	if (welcome && online && reason != 0 && reason != 2 && reason != 6)
	{
		Hub_OnWelcomeDelivered(playerid, 0);
	}
	return 1;
}

Freight_Release(playerid, reason)
{
	if (!g_FrtActive[playerid])
	{
		return 0;
	}
	if (!g_FrtWelcome[playerid] && g_FrtCt[playerid] > 0)
	{
		g_FrtDefer[playerid] = 0;
		g_FrtPhase[playerid] = 0;
		Director_Release(playerid, reason);
	}
	if (g_FrtActive[playerid])
	{
		Freight_End(playerid, reason);
	}
}

Freight_Stop(playerid, reason, text[])
{
	if (!g_FrtActive[playerid])
	{
		return 0;
	}
	if (text[0] != 0 && IsPlayerConnected(playerid))
	{
		Util_Send(playerid, -10270721, text);
	}
	new ph = g_FrtPhase[playerid];
	if (!g_FrtWelcome[playerid] && (ph == 2 || ph == 4))
	{
		if (g_FrtDefer[playerid] == 0)
		{
			g_FrtDefer[playerid] = reason;
		}
		Freight_RemoveVehicle(playerid);
		if (IsPlayerConnected(playerid))
		{
			DisablePlayerCheckpoint(playerid);
		}
		return 0;
	}
	Freight_Release(playerid, reason);
}

Freight_Picked(playerid)
{
	new now = GetTickCount();
	g_FrtPhase[playerid] = 3;
	g_FrtDwell[playerid] = 0;
	g_FrtStateRetry[playerid] = 0;
	gPlayer[playerid][pFreightLegTick] = now;
	gPlayer[playerid][pFreightDwellTick] = 0;
	if (!g_FrtWelcome[playerid])
	{
		gPlayer[playerid][pContractPhase] = 2;
	}
	g_FrtInteg[playerid] = Freight_IntegrityOf(g_FrtVeh[playerid]);
	g_FrtPath[playerid] = 0.0;
	GetVehiclePos(g_FrtVeh[playerid], g_FrtLastX[playerid], g_FrtLastY[playerid], g_FrtLastZ[playerid]);
	g_FrtLastTick[playerid] = now;
	Freight_ShowCheckpoint(playerid);
	new name[16];
	LD_DistrictName(g_FrtTo[playerid], name, 16);
	if (g_FrtWelcome[playerid])
	{
		UI_LivingToast(playerid, "Merce caricata: portala al negozio di Idlewood.", 1);
	}
	SendClientMessage(playerid, 0x66DD66FF, "Merce caricata. Consegnala al negozio di %s: guida con prudenza, i danni al Mule rovinano il carico.", name);
}

Freight_DoPickup(playerid)
{
	if (g_FrtWelcome[playerid])
	{
		Freight_Picked(playerid);
		return 0;
	}
	g_FrtPhase[playerid] = 2;
	g_FrtWaitTick[playerid] = GetTickCount();
	if (!Director_Pickup(playerid))
	{
		if (!g_FrtActive[playerid])
		{
			return 0;
		}
		if (g_FrtPhase[playerid] == 2)
		{
			g_FrtPhase[playerid] = 1;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "Carico non registrato al momento: resta nel checkpoint e riprova tra qualche secondo.");
		}
	}
}

Freight_DoDeliver(playerid)
{
	new now = GetTickCount();
	new seconds = (now - gPlayer[playerid][pFreightLegTick]) / 1000;
	new minSec = LD_FreightMinSeconds(g_FrtDist[playerid]);
	if (seconds < minSec)
	{
		new msg[256];
		format(msg, 256, "consegna troppo rapida: pers. %d contratto %d dopo %d s (minimo %d s)", gPlayer[playerid][pCharId], g_FrtCt[playerid], seconds, minSec);
		Log_Write("FREIGHT", msg);
		format(msg, 256, "Consegna non valida: troppo rapida (ancora %d s). Resta nel checkpoint.", minSec - seconds);
		Freight_Note(playerid, msg);
		g_FrtRetryTick[playerid] = now + 5000;
		return 0;
	}
	if (!g_FrtWelcome[playerid] && g_FrtDist[playerid] < 300)
	{
		Freight_Stop(playerid, 5, "Contratto non valido (tratta troppo corta).");
		return 0;
	}
	new Float:minPath = float(g_FrtDist[playerid] * 80 / 100);
	if ((g_FrtPath[playerid] < minPath))
	{
		new msg[256];
		format(msg, 256, "percorso insufficiente: pers. %d contratto %d percorso %.0f m su %d m", gPlayer[playerid][pCharId], g_FrtCt[playerid], g_FrtPath[playerid], g_FrtDist[playerid]);
		Log_Write("FREIGHT", msg);
		Freight_Note(playerid, "Consegna non valida: la merce deve essere portata dal magazzino con il Mule.");
		g_FrtRetryTick[playerid] = now + 5000;
		return 0;
	}
	new integ = Freight_IntegrityOf(g_FrtVeh[playerid]);
	if (integ < g_FrtInteg[playerid])
	{
		g_FrtInteg[playerid] = integ;
	}
	integ = g_FrtInteg[playerid];
	if (g_FrtWelcome[playerid])
	{
		Freight_End(playerid, 0);
		Util_Send(playerid, 0x66DD66FF, "Consegna di benvenuto completata: il negozio di Idlewood ti ringrazia!");
		Hub_OnWelcomeDelivered(playerid, 1);
		return 0;
	}
	g_FrtPhase[playerid] = 4;
	g_FrtWaitTick[playerid] = now;
	if (!Director_Complete(playerid, integ, seconds))
	{
		if (!g_FrtActive[playerid])
		{
			return 0;
		}
		if (g_FrtPhase[playerid] == 4)
		{
			g_FrtPhase[playerid] = 3;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "Consegna non registrata al momento: resta nel checkpoint e riprova tra qualche secondo.");
		}
	}
}

bool:Freight_Begin(playerid, contractId, fromD, toD, cargo, distanceM, bool:welcome)
{
	if (!Freight_ValidId(playerid) || !Util_IsPlaying(playerid) || g_FrtActive[playerid])
	{
		return 0;
	}
	if (!LD_IsReady() || gPlayer[playerid][pLivingReady] == 0 || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		return 0;
	}
	if (Job_IsOnShift(playerid) || gPlayer[playerid][pTaxiOnDuty] != 0 || gPlayer[playerid][pRentalId] != 0)
	{
		return 0;
	}
	if (welcome)
	{
		if (contractId != 0 || gPlayer[playerid][pContractId] != 0)
		{
			return 0;
		}
		fromD = 1;
		toD = 1;
		if (cargo < 1 || cargo > 3)
		{
			cargo = 1;
		}
	}
	else
	{
		if (contractId <= 0 || gPlayer[playerid][pFreightWelcome] != 0)
		{
			return 0;
		}
		if (gPlayer[playerid][pContractId] != 0 && gPlayer[playerid][pContractId] != contractId)
		{
			return 0;
		}
		if (fromD == toD || cargo < 1 || cargo > 3)
		{
			return 0;
		}
		if (gPlayer[playerid][pLevel] < 2)
		{
			return 0;
		}
	}
	new pair = Freight_PairDistance(fromD, toD);
	if (pair <= 0)
	{
		return 0;
	}
	new dist = welcome ? pair : (distanceM > pair ? distanceM : pair);
	if (!welcome && dist < 300)
	{
		return 0;
	}
	if (!LD_PoolHasRoom())
	{
		Util_Send(playerid, -10270721, "Nessun veicolo da carico disponibile al momento. Riprova tra poco.");
		return 0;
	}
	new Float:x, Float:y, Float:z, slot = 0;
	Freight_Point(fromD, 0, x, y, z);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FrtActive[i] && g_FrtFrom[i] == fromD && g_FrtPhase[i] == 1)
		{
			slot++;
		}
	}
	x += (5.0 * float(slot % 4));
	new veh = CreateVehicle(414, x, y, z, 0.0, 1, 1, -1, 0);
	if (veh == 65535 || veh <= 0 || veh >= 2000)
	{
		Util_Send(playerid, -10270721, "Impossibile preparare il veicolo da carico. Riprova.");
		return 0;
	}
	new serial = LD_VehRegister(veh, 3, playerid);
	if (serial == 0)
	{
		DestroyVehicle(veh);
		Util_Send(playerid, -10270721, "Nessun veicolo da carico disponibile al momento. Riprova tra poco.");
		return 0;
	}
	SetVehicleVirtualWorld(veh, 0);
	SetVehicleHealth(veh, Float:0x447A0000);
	SetVehicleParamsEx(veh, 1, 0, 0, 0, 0, 0, 0);
	new now = GetTickCount();
	Freight_ResetPlayer(playerid);
	g_FrtActive[playerid] = 1;
	g_FrtWelcome[playerid] = welcome;
	g_FrtCt[playerid] = contractId;
	g_FrtFrom[playerid] = fromD;
	g_FrtTo[playerid] = toD;
	g_FrtCargo[playerid] = cargo;
	g_FrtDist[playerid] = dist;
	g_FrtPhase[playerid] = 1;
	g_FrtVeh[playerid] = veh;
	g_FrtSerial[playerid] = serial;
	g_FrtStartTick[playerid] = now;
	gPlayer[playerid][pFreightVehicle] = veh;
	gPlayer[playerid][pFreightLegTick] = 0;
	gPlayer[playerid][pFreightDwellTick] = 0;
	gPlayer[playerid][pFreightLeftVehTick] = 0;
	if (welcome)
	{
		gPlayer[playerid][pFreightWelcome] = 1;
	}
	else
	{
		gPlayer[playerid][pContractId] = contractId;
		gPlayer[playerid][pContractPhase] = 1;
		gPlayer[playerid][pContractFromDistrict] = fromD;
		gPlayer[playerid][pContractToDistrict] = toD;
		gPlayer[playerid][pContractCargo] = cargo;
		gPlayer[playerid][pContractDistance] = distanceM;
		if (gPlayer[playerid][pContractStartTick] == 0)
		{
			gPlayer[playerid][pContractStartTick] = now;
		}
	}
	Freight_ShowCheckpoint(playerid);
	new fromName[16], toName[16], cargoName[96];
	LD_DistrictName(fromD, fromName, 16);
	LD_DistrictName(toD, toName, 16);
	Freight_CargoName(cargo, cargoName, 96);
	if (welcome)
	{
		SendClientMessage(playerid, 0x66DD66FF, "Consegna di benvenuto: sali sul Mule al magazzino di Idlewood, fermati nel checkpoint %d s per caricare %s, poi portali al negozio di Idlewood.", 6, cargoName);
		UI_LivingToast(playerid, "Sali sul Mule al magazzino di Idlewood.", 0);
	}
	else
	{
		SendClientMessage(playerid, 0x66DD66FF, "Contratto #%d: ritira il Mule al magazzino di %s, fermati nel checkpoint (max %.0f km/h) %d s per caricare %s, poi consegna al negozio di %s (%d m).", contractId, fromName, 0x41200000, 6, cargoName, toName, dist);
		Freight_Hud(playerid, -1, -1);
	}
	return 1;
}

Freight_OnPickupResult(playerid, result)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid] || g_FrtWelcome[playerid] || g_FrtPhase[playerid] != 2)
	{
		return 0;
	}
	if (g_FrtDefer[playerid] != 0)
	{
		Freight_Release(playerid, g_FrtDefer[playerid]);
		return 1;
	}
	switch (result)
	{
		case 1, 2:
		{
			if (!Freight_VehOk(playerid))
			{
				Freight_Release(playerid, 4);
				return 1;
			}
			Freight_Picked(playerid);
		}
		case -1, 10:
		{
			g_FrtPhase[playerid] = 1;
			g_FrtDwell[playerid] = 0;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "Registrazione del carico non riuscita: resta nel checkpoint per ritentare.");
		}
		default:
		{
			Freight_Release(playerid, 5);
		}
	}
	return 1;
}

Freight_OnCompleteResult(playerid, result, paid, balance)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid] || g_FrtWelcome[playerid] || g_FrtPhase[playerid] != 4)
	{
		return 0;
	}
	switch (result)
	{
		case 1, 2, 6:
		{
			new ct = g_FrtCt[playerid];
			Freight_End(playerid, 0);
			if (result == 2)
			{
				SendClientMessage(playerid, -1, "Consegna del contratto #%d gia' registrata: nessun nuovo pagamento.", ct);
			}
			else if (result == 6)
			{
				SendClientMessage(playerid, 0x66DD66FF, "Consegna #%d completata: $%d accreditati in banca (tetto orario raggiunto). Saldo: $%d.", ct, paid, balance);
			}
			else
			{
				SendClientMessage(playerid, 0x66DD66FF, "Consegna #%d completata: $%d accreditati in banca. Saldo: $%d.", ct, paid, balance);
			}
		}
		case 8:
		{
			Freight_End(playerid, 0);
			Util_Send(playerid, -10270721, "Contratto FALLITO: la merce e' arrivata troppo danneggiata. Nessun compenso.");
		}
		case -1, 10:
		{
			if (g_FrtDefer[playerid] != 0)
			{
				Freight_Release(playerid, g_FrtDefer[playerid]);
				return 1;
			}
			g_FrtPhase[playerid] = 3;
			g_FrtDwell[playerid] = 0;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "Registrazione della consegna non riuscita: resta nel checkpoint per ritentare.");
		}
		case 4:
		{
			g_FrtStateRetry[playerid]++;
			if (g_FrtDefer[playerid] != 0 || g_FrtStateRetry[playerid] >= 3)
			{
				Freight_Release(playerid, g_FrtDefer[playerid] != 0 ? g_FrtDefer[playerid] : 5);
				return 1;
			}
			g_FrtPhase[playerid] = 3;
			g_FrtDwell[playerid] = 0;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "Consegna rifiutata dal registro (troppo rapida o stato non valido): attendi qualche secondo nel checkpoint.");
		}
		default:
		{
			Freight_Release(playerid, g_FrtDefer[playerid] != 0 ? g_FrtDefer[playerid] : 5);
		}
	}
	return 1;
}

Freight_WaitTimeout(playerid)
{
	new msg[256];
	format(msg, 256, "esito DB non ricevuto: pers. %d contratto %d fase %d", gPlayer[playerid][pCharId], g_FrtCt[playerid], g_FrtPhase[playerid]);
	Log_Write("FREIGHT", msg);
	if (g_FrtDefer[playerid] != 0)
	{
		Freight_Release(playerid, g_FrtDefer[playerid]);
		return 0;
	}
	g_FrtPhase[playerid] = g_FrtPhase[playerid] == 2 ? 1 : 3;
	g_FrtDwell[playerid] = 0;
	g_FrtRetryTick[playerid] = GetTickCount() + 5000;
}

bool:Freight_SamplePath(playerid, now)
{
	new Float:x, Float:y, Float:z;
	GetVehiclePos(g_FrtVeh[playerid], x, y, z);
	new Float:dx = (x - g_FrtLastX[playerid]), Float:dy = (y - g_FrtLastY[playerid]), Float:dz = (z - g_FrtLastZ[playerid]);
	new Float:seg = floatsqroot((((dx * dx) + (dy * dy)) + (dz * dz)));
	new dt = now - g_FrtLastTick[playerid];
	if (dt < 0)
	{
		dt = 0;
	}
	g_FrtLastX[playerid] = x;
	g_FrtLastY[playerid] = y;
	g_FrtLastZ[playerid] = z;
	g_FrtLastTick[playerid] = now;
	if ((seg > (15.0 + ((Float:0x428C0000 * float(dt)) / float(1000)))))
	{
		new msg[256];
		format(msg, 256, "salto del Mule: pers. %d contratto %d %.0f m in %d ms", gPlayer[playerid][pCharId], g_FrtCt[playerid], seg, dt);
		Log_Write("FREIGHT", msg);
		return 0;
	}
	g_FrtPath[playerid] += seg;
	return 1;
}

Freight_TickPlayer(playerid, now)
{
	new ph = g_FrtPhase[playerid];
	if (ph == 2 || ph == 4)
	{
		if (now - g_FrtWaitTick[playerid] >= 20000)
		{
			Freight_WaitTimeout(playerid);
		}
		else if (g_FrtDefer[playerid] == 0 && Freight_VehOk(playerid))
		{
			Freight_Hud(playerid, -1, -1);
		}
		return 0;
	}
	if (Job_IsOnShift(playerid))
	{
		Freight_Stop(playerid, 1, "Hai iniziato un turno di lavoro: il contratto di trasporto e' stato chiuso.");
		return 0;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Freight_Stop(playerid, 3, "Non sei in condizione di guidare: contratto di trasporto interrotto.");
		return 0;
	}
	if (!Freight_VehOk(playerid))
	{
		Freight_Stop(playerid, 4, "Il Mule del contratto non esiste piu': contratto interrotto.");
		return 0;
	}
	new veh = g_FrtVeh[playerid];
	new Float:hp;
	GetVehicleHealth(veh, hp);
	if ((hp < Float:0x43960000))
	{
		Freight_Stop(playerid, 4, "Mule distrutto: il carico e' perso e il contratto e' interrotto.");
		return 0;
	}
	new bool:driving = Freight_IsDriving(playerid);
	if (driving)
	{
		g_FrtBoarded[playerid] = 1;
		g_FrtOff[playerid] = 0;
		gPlayer[playerid][pFreightLeftVehTick] = 0;
	}
	else if (g_FrtBoarded[playerid])
	{
		if (!g_FrtOff[playerid])
		{
			g_FrtOff[playerid] = 1;
			gPlayer[playerid][pFreightLeftVehTick] = now;
		}
		new Float:px, Float:py, Float:pz;
		GetPlayerPos(playerid, px, py, pz);
		if (now - gPlayer[playerid][pFreightLeftVehTick] >= 45000 || (GetVehicleDistanceFromPoint(veh, px, py, pz) > Float:0x43160000))
		{
			Freight_Stop(playerid, 4, "Mule abbandonato: contratto di trasporto rilasciato.");
			return 0;
		}
	}
	else if (now - g_FrtStartTick[playerid] >= 600000)
	{
		Freight_Stop(playerid, 4, "Non hai preso in consegna il Mule in tempo: contratto di trasporto rilasciato.");
		return 0;
	}
	if (ph == 3)
	{
		if (!Freight_SamplePath(playerid, now))
		{
			Freight_Stop(playerid, 4, "Spostamento anomalo del Mule: contratto di trasporto annullato.");
			return 0;
		}
		new integ = LD_Clamp(floatround((hp / Float:0x41200000), 1), 0, 100);
		if (integ < g_FrtInteg[playerid])
		{
			g_FrtInteg[playerid] = integ;
		}
	}
	new bool:shop = ph == 3;
	new Float:tx, Float:ty, Float:tz;
	Freight_Point(shop ? g_FrtTo[playerid] : g_FrtFrom[playerid], shop, tx, ty, tz);
	new Float:dist = (driving ? GetVehicleDistanceFromPoint(veh, tx, ty, tz) : GetPlayerDistanceFromPoint(playerid, tx, ty, tz));
	new dwellSec = -1;
	new bool:retryOk = g_FrtRetryTick[playerid] == 0 || now - g_FrtRetryTick[playerid] >= 0;
	if (driving && retryOk && (dist <= 6.0) && (float(Veh_SpeedKmh(veh)) <= Float:0x41200000) && GetVehicleVirtualWorld(veh) == 0)
	{
		if (!g_FrtDwell[playerid])
		{
			g_FrtDwell[playerid] = 1;
			gPlayer[playerid][pFreightDwellTick] = now;
			if (g_FrtWelcome[playerid])
			{
				UI_LivingToast(playerid, shop ? "Scarico in corso: resta fermo." : "Carico in corso: resta fermo.", 0);
			}
		}
		new need = shop ? 5000 : 6000;
		new waited = now - gPlayer[playerid][pFreightDwellTick];
		if (waited >= need)
		{
			g_FrtDwell[playerid] = 0;
			gPlayer[playerid][pFreightDwellTick] = 0;
			if (shop)
			{
				Freight_DoDeliver(playerid);
			}
			else
			{
				Freight_DoPickup(playerid);
			}
			if (!g_FrtActive[playerid])
			{
				return 0;
			}
		}
		else
		{
			dwellSec = Freight_SecCeil(need - waited);
		}
	}
	else
	{
		g_FrtDwell[playerid] = 0;
		gPlayer[playerid][pFreightDwellTick] = 0;
	}
	Freight_Hud(playerid, floatround(dist, 1), dwellSec);
}

public Freight_OnTick()
{
	new now = GetTickCount();
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!g_FrtActive[i])
		{
			continue;
		}
		if (!IsPlayerConnected(i))
		{
			Freight_End(i, 2);
			continue;
		}
		Freight_TickPlayer(i, now);
	}
	return 1;
}

Freight_OnInit()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Freight_ResetPlayer(i);
	}
	if (g_FrtTimer != 0)
	{
		KillTimer(g_FrtTimer);
	}
	g_FrtTimer = SetTimer("Freight_OnTick", 1000, 1);
}

Freight_OnExit()
{
	if (g_FrtTimer != 0)
	{
		KillTimer(g_FrtTimer);
	}
	g_FrtTimer = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FrtActive[i])
		{
			Freight_End(i, 6);
		}
	}
}

Freight_OnPlayerConnect(playerid)
{
	if (!Freight_ValidId(playerid))
	{
		return 0;
	}
	Freight_ResetPlayer(playerid);
}

Freight_OnCharReady(playerid)
{
}

Freight_OnPlayerDisconnect(playerid, reason)
{
	if (!Freight_ValidId(playerid))
	{
		return 0;
	}
	if (g_FrtActive[playerid])
	{
		Freight_End(playerid, 2);
	}
	Freight_ResetPlayer(playerid);
}

Freight_OnPlayerDeath(playerid)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid])
	{
		return 0;
	}
	Freight_Stop(playerid, 3, "Sei stato ferito: il contratto di trasporto e' interrotto e il Mule ritirato.");
}

Freight_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if (LD_VehModule(vehicleid) != 3)
	{
		return 0;
	}
	new owner = LD_VehOwner(vehicleid);
	if (owner == playerid || ispassenger != 0)
	{
		return 0;
	}
	ClearAnimations(playerid, 0);
	Util_Send(playerid, -10270721, "Questo veicolo da carico e' assegnato a un altro trasportatore.");
}

Freight_OnPlayerExitVehicle(playerid, vehicleid)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid] || g_FrtVeh[playerid] != vehicleid || !g_FrtBoarded[playerid])
	{
		return 0;
	}
	if (!g_FrtOff[playerid])
	{
		g_FrtOff[playerid] = 1;
		gPlayer[playerid][pFreightLeftVehTick] = GetTickCount();
		SendClientMessage(playerid, -10270721, "Torna alla guida del Mule entro %d secondi (e resta entro %.0f m) o il contratto sara' rilasciato.", 45, 0x43160000);
	}
}

Freight_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate, t_PLAYER_STATE:oldstate)
{
	if (newstate != 2)
	{
		return 0;
	}
	new veh = GetPlayerVehicleID(playerid);
	if (LD_VehModule(veh) != 3)
	{
		return 0;
	}
	if (LD_VehOwner(veh) != playerid)
	{
		RemovePlayerFromVehicle(playerid, 0);
		Util_Send(playerid, -10270721, "Questo veicolo da carico e' assegnato a un altro trasportatore.");
		return 0;
	}
	if (g_FrtActive[playerid] && g_FrtVeh[playerid] == veh)
	{
		g_FrtBoarded[playerid] = 1;
		g_FrtOff[playerid] = 0;
		gPlayer[playerid][pFreightLeftVehTick] = 0;
	}
}

Freight_OnVehicleDeath(vehicleid, killerid)
{
	if (LD_VehModule(vehicleid) != 3)
	{
		return 0;
	}
	new owner = LD_VehOwner(vehicleid);
	if (!Freight_ValidId(owner) || !g_FrtActive[owner] || g_FrtVeh[owner] != vehicleid)
	{
		return 0;
	}
	Freight_Stop(owner, 4, "Mule distrutto: il carico e' perso e il contratto e' interrotto.");
}
