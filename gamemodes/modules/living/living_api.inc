new bool:g_LdReady;
new g_LdLsnName[8][32];
new g_LdLsnPrio[8];
new g_LdLsnCount;
new g_LdEmitDepth;
new g_LdVehMod[2000];
new g_LdVehOwner[2000];
new g_LdVehSerial[2000];
new g_LdVehSerialSeq;
new g_LdPool;

bool:LD_IsReady()
{
	return g_LdReady;
}

LD_SetReady(bool:ready)
{
	g_LdReady = ready;
	if (!ready)
	{
		return 0;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		Living_TryCharReady(i);
	}
}

bool:LD_LsnBefore(prio, const fn[], idx)
{
	if (prio != g_LdLsnPrio[idx])
	{
		return prio < g_LdLsnPrio[idx];
	}
	return strcmp(fn, g_LdLsnName[idx], 0, 0x7FFFFFFF) < 0;
}

bool:LD_Listen(const fn[], prio)
{
	if (strlen(fn) >= 32 || funcidx(fn) == -1)
	{
		return 0;
	}
	for (new i = 0; i < g_LdLsnCount; i++)
	{
		if (!strcmp(fn, g_LdLsnName[i], 0, 0x7FFFFFFF))
		{
			return 0;
		}
	}
	if (g_LdLsnCount >= 8)
	{
		return 0;
	}
	new pos = g_LdLsnCount;
	while (pos > 0 && LD_LsnBefore(prio, fn, pos - 1))
	{
		g_LdLsnPrio[pos] = g_LdLsnPrio[pos - 1];
		strmid(g_LdLsnName[pos], g_LdLsnName[pos - 1], 0, 32, 32);
		pos--;
	}
	g_LdLsnPrio[pos] = prio;
	strmid(g_LdLsnName[pos], fn, 0, 32, 32);
	g_LdLsnCount++;
	return 1;
}

LD_Emit(type, districtId, charId, value, const opId[])
{
	if (g_LdEmitDepth >= 2)
	{
		new msg[256];
		format(msg, 256, "evento %d scartato (annidamento): quartiere %d pers. %d valore %d", type, districtId, charId, value);
		Log_Write("LIVING", msg);
		return 0;
	}
	g_LdEmitDepth++;
	new n = g_LdLsnCount;
	for (new i = 0; i < n; i++)
	{
		if (opId[0] == 0)
		{
			CallLocalFunction(g_LdLsnName[i], "dddds", type, districtId, charId, value, "-");
		}
		else
		{
			CallLocalFunction(g_LdLsnName[i], "dddds", type, districtId, charId, value, opId);
		}
	}
	g_LdEmitDepth--;
	return n;
}

LD_OnboardAction(playerid, step)
{
	if (!Util_IsPlaying(playerid) || gPlayer[playerid][pCharId] <= 0)
	{
		return 0;
	}
	LD_Emit(106, 0, gPlayer[playerid][pCharId], step, "-");
}

LD_GetLocalRep(playerid, districtId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || districtId < 1 || districtId > 3)
	{
		return 0;
	}
	return gPlayer[playerid][pDistrictRep][districtId];
}

LD_GetProPoints(playerid, track)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || track < 1 || track > 2)
	{
		return 0;
	}
	return gPlayer[playerid][pProPoints][track];
}

LD_SetProPoints(playerid, track, points)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || track < 1 || track > 2)
	{
		return 0;
	}
	gPlayer[playerid][pProPoints][track] = points > 0 ? points : 0;
}

LD_ActiveFlow(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (Job_IsOnShift(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][pContractId] != 0 || gPlayer[playerid][pFreightWelcome] != 0)
	{
		return 2;
	}
	if (gPlayer[playerid][pTaxiOnDuty] != 0 || gPlayer[playerid][pRentalClass] == 2)
	{
		return 3;
	}
	if (gPlayer[playerid][pRentalId] != 0)
	{
		return 4;
	}
	return 0;
}

LD_CheckStart(playerid)
{
	if (!g_LdReady || gPlayer[playerid][pLivingReady] == 0)
	{
		return 1;
	}
	if (gPlayer[playerid][pWounded])
	{
		return 2;
	}
	if (gPlayer[playerid][pCuffed])
	{
		return 3;
	}
	if (LD_ActiveFlow(playerid) != 0)
	{
		return 4;
	}
	return 0;
}

bool:LD_SendBlock(playerid, block)
{
	switch (block)
	{
		case 1:
		{
			Util_Send(playerid, -10270721, "I servizi della citta' si stanno avviando: riprova tra poco.");
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "Sei ferito: non puoi farlo ora.");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, "Sei ammanettato: non puoi farlo ora.");
		}
		case 4:
		{
			switch (LD_ActiveFlow(playerid))
			{
				case 1:
				{
					Util_Send(playerid, -10270721, "Sei in turno di lavoro: termina prima il turno (/fine).");
				}
				case 2:
				{
					Util_Send(playerid, -10270721, "Hai gia' un contratto attivo (/contratto).");
				}
				case 3:
				{
					Util_Send(playerid, -10270721, "Sei in servizio taxi: prima /taxi off.");
				}
				default:
				{
					Util_Send(playerid, -10270721, "Hai un noleggio in corso: prima /riconsegna.");
				}
			}
		}
		default:
		{
			return 0;
		}
	}
	return 1;
}

bool:LD_PoolHasRoom()
{
	return g_LdPool < 40;
}

LD_VehRegister(vehicleid, module, playerid)
{
	if (vehicleid <= 0 || vehicleid >= 2000 || module == 0)
	{
		return 0;
	}
	if (g_LdVehMod[vehicleid] != 0 || g_LdPool >= 40)
	{
		return 0;
	}
	g_LdVehSerialSeq++;
	if (g_LdVehSerialSeq <= 0)
	{
		g_LdVehSerialSeq = 1;
	}
	g_LdVehMod[vehicleid] = module;
	g_LdVehOwner[vehicleid] = playerid + 1;
	g_LdVehSerial[vehicleid] = g_LdVehSerialSeq;
	g_LdPool++;
	return g_LdVehSerialSeq;
}

LD_VehRelease(vehicleid)
{
	if (vehicleid <= 0 || vehicleid >= 2000 || g_LdVehMod[vehicleid] == 0)
	{
		return 0;
	}
	g_LdVehMod[vehicleid] = 0;
	g_LdVehOwner[vehicleid] = 0;
	g_LdVehSerial[vehicleid] = 0;
	if (g_LdPool > 0)
	{
		g_LdPool--;
	}
}

LD_VehModule(vehicleid)
{
	if (vehicleid <= 0 || vehicleid >= 2000)
	{
		return 0;
	}
	return g_LdVehMod[vehicleid];
}

LD_VehOwner(vehicleid)
{
	if (vehicleid <= 0 || vehicleid >= 2000 || g_LdVehOwner[vehicleid] == 0)
	{
		return 65535;
	}
	return g_LdVehOwner[vehicleid] - 1;
}

LD_VehSerial(vehicleid)
{
	if (vehicleid <= 0 || vehicleid >= 2000)
	{
		return 0;
	}
	return g_LdVehSerial[vehicleid];
}

LD_VehCleanupOwner(playerid)
{
	for (new v = 1; v < 2000; v++)
	{
		if (g_LdVehOwner[v] != playerid + 1)
		{
			continue;
		}
		new msg[256];
		format(msg, 256, "veicolo %d (modulo %d) non rilasciato dal modulo alla disconnessione di %d: distrutto", v, g_LdVehMod[v], playerid);
		Log_Write("LIVING", msg);
		LD_VehRelease(v);
		DestroyVehicle(v);
	}
}

Living_OnInit()
{
	g_LdReady = 0;
	g_LdLsnCount = 0;
	g_LdEmitDepth = 0;
	g_LdPool = 0;
	for (new v = 0; v < 2000; v++)
	{
		g_LdVehMod[v] = 0;
		g_LdVehOwner[v] = 0;
		g_LdVehSerial[v] = 0;
	}
	District_OnInit();
	Director_OnInit();
	CityOS_OnInit();
	Rental_OnInit();
	Taxi_OnInit();
	Freight_OnInit();
	Hub_OnInit();
}

Living_OnExit()
{
	Freight_OnExit();
	Taxi_OnExit();
	Rental_OnExit();
	Director_OnExit();
	Hub_OnExit();
	CityOS_OnExit();
	District_OnExit();
	g_LdReady = 0;
	g_LdLsnCount = 0;
}

Living_OnPlayerConnect(playerid)
{
	District_OnPlayerConnect(playerid);
	Director_OnPlayerConnect(playerid);
	CityOS_OnPlayerConnect(playerid);
	Rental_OnPlayerConnect(playerid);
	Taxi_OnPlayerConnect(playerid);
	Freight_OnPlayerConnect(playerid);
	Hub_OnPlayerConnect(playerid);
}

Living_OnPlayerDisconnect(playerid, reason)
{
	Freight_OnPlayerDisconnect(playerid, reason);
	Taxi_OnPlayerDisconnect(playerid, reason);
	Rental_OnPlayerDisconnect(playerid, reason);
	Director_OnPlayerDisconnect(playerid, reason);
	Hub_OnPlayerDisconnect(playerid, reason);
	CityOS_OnPlayerDisconnect(playerid, reason);
	District_OnPlayerDisconnect(playerid, reason);
	LD_VehCleanupOwner(playerid);
}

Living_TryCharReady(playerid)
{
	if (!g_LdReady || !Util_IsPlaying(playerid))
	{
		return 0;
	}
	if (gPlayer[playerid][pCharId] <= 0 || gPlayer[playerid][pLivingReady] != 0)
	{
		return 0;
	}
	gPlayer[playerid][pLivingReady] = 1;
	District_OnCharReady(playerid);
	Director_OnCharReady(playerid);
	CityOS_OnCharReady(playerid);
	Rental_OnCharReady(playerid);
	Taxi_OnCharReady(playerid);
	Freight_OnCharReady(playerid);
	Hub_OnCharReady(playerid);
}

Living_OnPlayerSpawn(playerid)
{
	Living_TryCharReady(playerid);
}

Living_OnPlayerDeath(playerid)
{
	Freight_OnPlayerDeath(playerid);
	Taxi_OnPlayerDeath(playerid);
	Rental_OnPlayerDeath(playerid);
}

Living_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	Freight_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	Taxi_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	Rental_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
}

Living_OnPlayerExitVehicle(playerid, vehicleid)
{
	Freight_OnPlayerExitVehicle(playerid, vehicleid);
	Taxi_OnPlayerExitVehicle(playerid, vehicleid);
	Rental_OnPlayerExitVehicle(playerid, vehicleid);
}

Living_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate, t_PLAYER_STATE:oldstate)
{
	Freight_OnPlayerStateChange(playerid, newstate, oldstate);
	Taxi_OnPlayerStateChange(playerid, newstate, oldstate);
	Rental_OnPlayerStateChange(playerid, newstate, oldstate);
}

Living_OnVehicleDeath(vehicleid, killerid)
{
	Freight_OnVehicleDeath(vehicleid, killerid);
	Taxi_OnVehicleDeath(vehicleid, killerid);
	Rental_OnVehicleDeath(vehicleid, killerid);
}
