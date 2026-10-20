new g_JobName[4][16] = {
	"Disoccupato",
	"Autista Bus",
	"Corriere",
	"Netturbino"
};

Job_NameOf(jobId, out[], size)
{
	out[0] = 0;
	if (jobId < 0 || jobId > 3)
	{
		jobId = 0;
	}
	strcat(out, g_JobName[jobId], size);
}
new g_JobStepWord[4][16] = {
	"Tappa",
	"Fermata",
	"Consegna",
	"Cassonetto"
};
new g_JobMinLevel[4] = {0, 1, 2, 1};
new g_JobModel[4] = {0, 431, 498, 408};
new g_JobSteps[4] = {0, 6, 5, 8};
new g_JobPayStep[4] = {0, 28, 45, 24};
new g_JobPayBonus[4] = {0, 40, 30, 40};
new g_JobDwellMs[4] = {0, 4000, 5000, 4000};
new Float:g_JobBusStops[6][3] = {
	{1135.0, -1455.0, 15.5},
	{1534.0, -1664.0, 13.4},
	{1967.0, -1200.0, 25.0},
	{2150.0, -1250.0, 23.9},
	{2465.0, -1698.0, 13.5},
	{2065.0, -1703.5, 13.6}
};
new Float:g_JobCourierPts[8][3] = {
	{2465.0, -1698.0, 13.5},
	{2065.0, -1703.5, 14.1},
	{2032.0, -1665.0, 14.0},
	{2120.0, -1920.0, 13.6},
	{2150.0, -1250.0, 23.9},
	{2306.0, -1190.0, 27.0},
	{2654.0, -1470.0, 30.4},
	{2720.0, -1290.0, 59.6}
};
new Float:g_JobBinPts[8][3] = {
	{2120.0, -1920.0, 13.6},
	{2065.0, -1790.0, 13.6},
	{1950.0, -1815.0, 13.6},
	{2032.0, -1700.0, 14.0},
	{2160.0, -1760.0, 13.6},
	{2300.0, -1800.0, 13.6},
	{2465.0, -1698.0, 13.5},
	{2400.0, -1900.0, 13.6}
};
new bool:g_JsActive[MAX_PLAYERS];
new g_JsJob[MAX_PLAYERS];
new g_JsVeh[MAX_PLAYERS];
new g_JsSerial[MAX_PLAYERS];
new g_JsStep[MAX_PLAYERS];
new g_JsRoute[MAX_PLAYERS][8];
new g_JsEarned[MAX_PLAYERS];
new g_JsCapUsed[MAX_PLAYERS];
new g_JsStartTick[MAX_PLAYERS];
new g_JsLastCpTick[MAX_PLAYERS];
new Float:g_JsLastPos[MAX_PLAYERS][3];
new bool:g_JsDwelling[MAX_PLAYERS];
new g_JsDwellTick[MAX_PLAYERS];
new bool:g_JsOff[MAX_PLAYERS];
new g_JsOffTick[MAX_PLAYERS];
new bool:g_JsCapWarned[MAX_PLAYERS];
new bool:g_JpPending[MAX_PLAYERS];
new g_JpJob[MAX_PLAYERS];
new g_JpAmount[MAX_PLAYERS];
new g_JpCap[MAX_PLAYERS];
new bool:g_JpCompleted[MAX_PLAYERS];
new bool:g_JcRunSet[MAX_PLAYERS];
new g_JcRunLast[MAX_PLAYERS];
new bool:g_JcChgSet[MAX_PLAYERS];
new g_JcChgLast[MAX_PLAYERS];
new bool:g_JcSetPending[MAX_PLAYERS];
new g_JcSetJob[MAX_PLAYERS];
new g_JcHourBucket[MAX_PLAYERS];
new g_JcHourEarned[MAX_PLAYERS][4];
new g_JcHourExp[MAX_PLAYERS];
new g_JcSessRuns[MAX_PLAYERS];
new g_JcSessEarned[MAX_PLAYERS];
new g_JVehOwner[2001];
new g_JVehSerial[2001];
new g_JPool;
new g_JSerialSeq;
new g_JTimer;

Job_CdLeftMs(bool:isSet, last, cooldownMs)
{
	if (!isSet)
	{
		return 0;
	}
	new elapsed = GetTickCount() - last;
	if (elapsed < 0 || elapsed >= cooldownMs)
	{
		return 0;
	}
	return cooldownMs - elapsed;
}

Job_SecCeil(ms)
{
	if (ms <= 0)
	{
		return 0;
	}
	return (ms + 1000 - 1) / 1000;
}

bool:Job_IsOnShift(playerid)
{
	return playerid >= 0 && playerid < MAX_PLAYERS && g_JsActive[playerid];
}

Job_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	g_JsActive[playerid] = 0;
	g_JsJob[playerid] = 0;
	g_JsVeh[playerid] = 65535;
	g_JsSerial[playerid] = 0;
	g_JsStep[playerid] = 0;
	for (new i = 0; i < 8; i++)
	{
		g_JsRoute[playerid][i] = 0;
	}
	g_JsEarned[playerid] = 0;
	g_JsCapUsed[playerid] = 0;
	g_JsStartTick[playerid] = 0;
	g_JsLastCpTick[playerid] = 0;
	g_JsLastPos[playerid][0] = 0.0;
	g_JsLastPos[playerid][1] = 0.0;
	g_JsLastPos[playerid][2] = 0.0;
	g_JsDwelling[playerid] = 0;
	g_JsDwellTick[playerid] = 0;
	g_JsOff[playerid] = 0;
	g_JsOffTick[playerid] = 0;
	g_JsCapWarned[playerid] = 0;
	g_JpPending[playerid] = 0;
	g_JpJob[playerid] = 0;
	g_JpAmount[playerid] = 0;
	g_JpCap[playerid] = 0;
	g_JpCompleted[playerid] = 0;
	g_JcRunSet[playerid] = 0;
	g_JcRunLast[playerid] = 0;
	g_JcChgSet[playerid] = 0;
	g_JcChgLast[playerid] = 0;
	g_JcSetPending[playerid] = 0;
	g_JcSetJob[playerid] = 0;
	g_JcHourBucket[playerid] = 0;
	for (new j = 0; j <= 3; j++)
	{
		g_JcHourEarned[playerid][j] = 0;
	}
	g_JcHourExp[playerid] = 0;
	g_JcSessRuns[playerid] = 0;
	g_JcSessEarned[playerid] = 0;
}

Job_RollHour(playerid)
{
	new bucket = gPlayer[playerid][pSessionMs] / 3600000;
	if (bucket == g_JcHourBucket[playerid])
	{
		return 0;
	}
	g_JcHourBucket[playerid] = bucket;
	for (new j = 0; j <= 3; j++)
	{
		g_JcHourEarned[playerid][j] = 0;
	}
	g_JcHourExp[playerid] = 0;
}

bool:Job_NearCenter(playerid)
{
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		return 0;
	}
	return (GetPlayerDistanceFromPoint(playerid, 1480.5, -1736.0, 13.5) <= 4.0);
}

Job_VehicleOwner(vehicleid)
{
	if (vehicleid <= 0 || vehicleid > 2000)
	{
		return 65535;
	}
	new owner = g_JVehOwner[vehicleid];
	if (owner < 0 || owner >= 1000 || !IsPlayerConnected(owner) || !g_JsActive[owner])
	{
		return 65535;
	}
	if (g_JVehSerial[vehicleid] != g_JsSerial[owner] || g_JsVeh[owner] != vehicleid)
	{
		return 65535;
	}
	return owner;
}

bool:Job_IsReturnStep(playerid)
{
	return g_JsStep[playerid] >= g_JobSteps[g_JsJob[playerid]];
}

Job_SpawnPos(job, &Float:x, &Float:y, &Float:z, &Float:a)
{
	switch (job)
	{
		case 1:
		{
			x = 1796.0;
			y = -1872.0;
			z = 13.6;
			a = 90.0;
		}
		case 3:
		{
			x = (2175.0 + 8.0);
			y = -2185.0;
			z = 13.5;
			a = 0.0;
		}
		default:
		{
			x = 2175.0;
			y = -2185.0;
			z = 13.5;
			a = 0.0;
		}
	}
}

Job_TargetPos(playerid, &Float:x, &Float:y, &Float:z)
{
	new job = g_JsJob[playerid], step = g_JsStep[playerid];
	if (step >= g_JobSteps[job])
	{
		new Float:a;
		Job_SpawnPos(job, x, y, z, a);
		return 0;
	}
	new idx = g_JsRoute[playerid][step];
	switch (job)
	{
		case 1:
		{
			x = g_JobBusStops[idx][0];
			y = g_JobBusStops[idx][1];
			z = g_JobBusStops[idx][2];
		}
		case 2:
		{
			x = g_JobCourierPts[idx][0];
			y = g_JobCourierPts[idx][1];
			z = g_JobCourierPts[idx][2];
		}
		default:
		{
			x = g_JobBinPts[idx][0];
			y = g_JobBinPts[idx][1];
			z = g_JobBinPts[idx][2];
		}
	}
}

Job_ShowCheckpoint(playerid)
{
	new Float:x, Float:y, Float:z;
	Job_TargetPos(playerid, x, y, z);
	DisablePlayerCheckpoint(playerid);
	SetPlayerCheckpoint(playerid, x, y, z, Float:((Job_IsReturnStep(playerid) ? 0x41200000 : 6.0)));
}

Job_BuildRoute(playerid, job)
{
	new total = g_JobSteps[job];
	if (job == 2)
	{
		new pool[8];
		for (new i = 0; i < 8; i++)
		{
			pool[i] = i;
		}
		for (new i = 0; i < total; i++)
		{
			new pick = i + random(8 - i), tmp = pool[i];
			pool[i] = pool[pick];
			pool[pick] = tmp;
			g_JsRoute[playerid][i] = pool[i];
		}
		return 0;
	}
	for (new i = 0; i < total; i++)
	{
		g_JsRoute[playerid][i] = i;
	}
}

Job_VehRemove(playerid)
{
	new veh = g_JsVeh[playerid];
	if (veh != 65535 && veh > 0 && veh <= 2000)
	{
		if (g_JVehOwner[veh] == playerid)
		{
			g_JVehOwner[veh] = 65535;
			g_JVehSerial[veh] = 0;
			if (g_JPool > 0)
			{
				g_JPool--;
			}
		}
		if (IsValidVehicle(veh))
		{
			if (GetPlayerVehicleID(playerid) == veh)
			{
				RemovePlayerFromVehicle(playerid, 0);
			}
			DestroyVehicle(veh);
		}
	}
	g_JsVeh[playerid] = 65535;
}

Job_Teardown(playerid, bool:startCooldown)
{
	if (IsPlayerConnected(playerid))
	{
		DisablePlayerCheckpoint(playerid);
		UI_JobHudHide(playerid);
	}
	Job_VehRemove(playerid);
	g_JsActive[playerid] = 0;
	g_JsEarned[playerid] = 0;
	g_JsCapUsed[playerid] = 0;
	g_JsDwelling[playerid] = 0;
	g_JsOff[playerid] = 0;
	g_JsSerial[playerid] = 0;
	if (startCooldown)
	{
		g_JcRunSet[playerid] = 1;
		g_JcRunLast[playerid] = GetTickCount();
	}
}

Job_Cancel(playerid, const reason[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !g_JsActive[playerid])
	{
		return 0;
	}
	Job_RollHour(playerid);
	new job = g_JsJob[playerid];
	g_JcHourEarned[playerid][job] -= g_JsCapUsed[playerid];
	if (g_JcHourEarned[playerid][job] < 0)
	{
		g_JcHourEarned[playerid][job] = 0;
	}
	new lost = g_JsEarned[playerid];
	Job_Teardown(playerid, 1);
	if (reason[0] != 0 && IsPlayerConnected(playerid))
	{
		SendClientMessage(playerid, -10270721, "Turno annullato: %s", reason);
		if (lost > 0)
		{
			SendClientMessage(playerid, -10270721, "Il compenso maturato ($%d) e' andato perso.", lost);
		}
	}
}

Job_Accrue(playerid, amount)
{
	Job_RollHour(playerid);
	new job = g_JsJob[playerid], room = 2200 - g_JcHourEarned[playerid][job];
	if (room < 0)
	{
		room = 0;
	}
	new granted = amount > room ? room : amount;
	if (granted < amount && !g_JsCapWarned[playerid])
	{
		g_JsCapWarned[playerid] = 1;
		SendClientMessage(playerid, -10270721, "Tetto orario di $%d raggiunto: il turno prosegue ma non matura altro compenso.", 2200);
	}
	g_JcHourEarned[playerid][job] += granted;
	g_JsCapUsed[playerid] += granted;
	g_JsEarned[playerid] += granted;
	return granted;
}

Job_Finish(playerid, bool:completed)
{
	new amount = g_JsEarned[playerid], job = g_JsJob[playerid], cap = g_JsCapUsed[playerid];
	new bool:busy = g_JpPending[playerid];
	Job_Teardown(playerid, 1);
	if (amount <= 0)
	{
		SendClientMessage(playerid, -1, "Turno terminato: nessun compenso maturato.");
		return 0;
	}
	if (busy || gPlayer[playerid][pCharId] <= 0)
	{
		g_JcHourEarned[playerid][job] -= cap;
		if (g_JcHourEarned[playerid][job] < 0)
		{
			g_JcHourEarned[playerid][job] = 0;
		}
		SendClientMessage(playerid, -10270721, "Un accredito precedente e' ancora in corso: compenso non registrato.");
		return 0;
	}
	g_JpPending[playerid] = 1;
	g_JpJob[playerid] = job;
	g_JpAmount[playerid] = amount;
	g_JpCap[playerid] = cap;
	g_JpCompleted[playerid] = completed;
	SendClientMessage(playerid, -1, "Turno terminato: registrazione del compenso ($%d) in corso...", amount);
	DB_JobPayout(playerid, gPlayer[playerid][pCharId], job, amount);
}

public Job_OnDBPayout(playerid, jobId, amount, result)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (!g_JpPending[playerid] || g_JpJob[playerid] != jobId || g_JpAmount[playerid] != amount)
	{
		return 1;
	}
	g_JpPending[playerid] = 0;
	new cap = g_JpCap[playerid];
	new bool:completed = g_JpCompleted[playerid];
	g_JpCap[playerid] = 0;
	g_JpAmount[playerid] = 0;
	if (result != 1)
	{
		Job_RollHour(playerid);
		g_JcHourEarned[playerid][jobId] -= cap;
		if (g_JcHourEarned[playerid][jobId] < 0)
		{
			g_JcHourEarned[playerid][jobId] = 0;
		}
		SendClientMessage(playerid, -10270721, result < 0 ? "Errore del servizio: compenso non accreditato." : "Compenso rifiutato: nulla accreditato.");
		new msg[128];
		format(msg, 128, "payout non accreditato job %d importo %d esito %d char %d", jobId, amount, result, gPlayer[playerid][pCharId]);
		Log_Write("JOB", msg);
		return 1;
	}
	new paid = Economy_PaycheckPaid(amount, Money_BankRoom(playerid));
	if (paid > 0 && Money_GiveBank(playerid, paid, "job", ""))
	{
		SendClientMessage(playerid, 0x66DD66FF, "Compenso: $%d versati in banca. Saldo: $%d.", paid, gPlayer[playerid][pBank]);
		if (paid < amount)
		{
			SendClientMessage(playerid, -10270721, "Il conto ha raggiunto il limite: parte del compenso e' andata persa.");
		}
	}
	else
	{
		SendClientMessage(playerid, -10270721, "Il conto bancario e' al limite: compenso non versato.");
	}
	g_JcSessEarned[playerid] += amount;
	if (completed)
	{
		g_JcSessRuns[playerid]++;
		Job_RollHour(playerid);
		if (g_JcHourExp[playerid] < 4)
		{
			new gain = 1;
			if (g_JcHourExp[playerid] + gain > 4)
			{
				gain = 4 - g_JcHourExp[playerid];
			}
			g_JcHourExp[playerid] += gain;
			Level_AddExp(playerid, gain);
		}
	}
	Char_Save(playerid);
	return 1;
}

Job_StepDone(playerid, now)
{
	new job = g_JsJob[playerid], total = g_JobSteps[job];
	new Float:tx, Float:ty, Float:tz;
	Job_TargetPos(playerid, tx, ty, tz);
	new Float:dx = (tx - g_JsLastPos[playerid][0]), Float:dy = (ty - g_JsLastPos[playerid][1]), Float:dz = (tz - g_JsLastPos[playerid][2]);
	new Float:dist = floatsqroot((((dx * dx) + (dy * dy)) + (dz * dz))), elapsedMs = now - g_JsLastCpTick[playerid];
	if (elapsedMs < 400 || ((dist * float(1000)) > (40.0 * float(elapsedMs))))
	{
		new name[25], msg[128];
		GetPlayerName(playerid, name, 25);
		format(msg, 128, "tratta non valida: %s job %d tappa %d dist %.0f m in %d ms", name, job, g_JsStep[playerid], dist, elapsedMs);
		Log_Write("JOB", msg);
		Job_Cancel(playerid, "tratta percorsa troppo in fretta (controllo anti-abuso).");
		return 0;
	}
	g_JsLastPos[playerid][0] = tx;
	g_JsLastPos[playerid][1] = ty;
	g_JsLastPos[playerid][2] = tz;
	g_JsLastCpTick[playerid] = now;
	g_JsDwelling[playerid] = 0;
	if (g_JsStep[playerid] >= total)
	{
		Job_Accrue(playerid, g_JobPayBonus[job]);
		SendClientMessage(playerid, 0x66DD66FF, "Rientro completato: bonus di fine giro.");
		Job_Finish(playerid, 1);
		return 0;
	}
	new granted = Job_Accrue(playerid, g_JobPayStep[job]);
	g_JsStep[playerid]++;
	SendClientMessage(playerid, 0x66DD66FF, "%s %d/%d completata: +$%d (maturato $%d).", g_JobStepWord[job], g_JsStep[playerid], total, granted, g_JsEarned[playerid]);
	if (g_JsStep[playerid] >= total)
	{
		SendClientMessage(playerid, 0x99CCFFFF, "Ultima tappa fatta: rientra al deposito per il bonus (checkpoint sulla mappa).");
	}
	Job_ShowCheckpoint(playerid);
}

public Job_OnTick()
{
	new now = GetTickCount();
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!g_JsActive[i])
		{
			continue;
		}
		if (!IsPlayerConnected(i) || !gPlayer[i][pSpawned])
		{
			Job_Cancel(i, "");
			continue;
		}
		if (gPlayer[i][pCuffed] || gPlayer[i][pWounded])
		{
			Job_Cancel(i, "non sei in condizione di lavorare.");
			continue;
		}
		new veh = g_JsVeh[i];
		if (Job_VehicleOwner(veh) != i)
		{
			Job_Cancel(i, "il veicolo di lavoro non esiste piu'.");
			continue;
		}
		new Float:hp;
		GetVehicleHealth(veh, hp);
		if ((hp < 300.0))
		{
			Job_Cancel(i, "veicolo di lavoro distrutto.");
			continue;
		}
		new bool:inVeh = GetPlayerVehicleID(i) == veh && GetPlayerVehicleSeat(i) == 0, offSec = -1;
		if (inVeh)
		{
			g_JsOff[i] = 0;
		}
		else
		{
			if (!g_JsOff[i])
			{
				g_JsOff[i] = 1;
				g_JsOffTick[i] = now;
			}
			new offMs = now - g_JsOffTick[i], Float:px, Float:py, Float:pz;
			GetPlayerPos(i, px, py, pz);
			if (offMs >= 45000 || (GetVehicleDistanceFromPoint(veh, px, py, pz) > 150.0))
			{
				Job_Cancel(i, "veicolo di lavoro abbandonato.");
				continue;
			}
			offSec = Job_SecCeil(45000 - offMs);
		}
		new job = g_JsJob[i], dwellSec = -1;
		new bool:ret = Job_IsReturnStep(i);
		if (inVeh)
		{
			new Float:tx, Float:ty, Float:tz;
			Job_TargetPos(i, tx, ty, tz);
			new Float:radius = Float:((ret ? 0x41200000 : 6.0));
			if ((GetVehicleDistanceFromPoint(veh, tx, ty, tz) <= radius) && (float(Veh_SpeedKmh(veh)) <= 12.0))
			{
				if (!g_JsDwelling[i])
				{
					g_JsDwelling[i] = 1;
					g_JsDwellTick[i] = now;
				}
				new dwellMs = ret ? 0 : g_JobDwellMs[job], waited = now - g_JsDwellTick[i];
				if (waited >= dwellMs)
				{
					Job_StepDone(i, now);
					if (!g_JsActive[i])
					{
						continue;
					}
				}
				else
				{
					dwellSec = Job_SecCeil(dwellMs - waited);
				}
			}
			else
			{
				g_JsDwelling[i] = 0;
			}
		}
		else
		{
			g_JsDwelling[i] = 0;
		}
		UI_JobHudUpdate(i, g_JsStep[i] + 1, g_JobSteps[job], g_JsEarned[i], (now - g_JsStartTick[i]) / 1000, dwellSec, offSec);
	}
	return 1;
}

Job_Start(playerid)
{
	new job = gPlayer[playerid][pJob];
	if (job < 1 || job > 3)
	{
		Util_Send(playerid, -10270721, "Non hai un lavoro. Recati al Job Center e usa /lavoro prendi.");
		return 0;
	}
	if (g_JsActive[playerid])
	{
		Util_Send(playerid, -10270721, "Hai gia' un turno in corso. Usa /fine per terminarlo.");
		return 0;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed] || gPlayer[playerid][pOnDuty] || g_JpPending[playerid] || g_JcSetPending[playerid])
	{
		Util_Send(playerid, -10270721, "Non puoi iniziare un turno adesso (ferito, ammanettato, in servizio o operazione in corso).");
		return 0;
	}
	if (gPlayer[playerid][pLevel] < g_JobMinLevel[job])
	{
		SendClientMessage(playerid, -10270721, "Per questo lavoro serve il livello %d.", g_JobMinLevel[job]);
		return 0;
	}
	new left = Job_CdLeftMs(g_JcRunSet[playerid], g_JcRunLast[playerid], 30000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Attendi ancora %d secondi prima di un nuovo turno.", Job_SecCeil(left));
		return 0;
	}
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
	{
		Util_Send(playerid, -10270721, "Devi trovarti all'aperto per iniziare il turno.");
		return 0;
	}
	new Float:x, Float:y, Float:z, Float:a;
	Job_SpawnPos(job, x, y, z, a);
	if (!IsPlayerInRangeOfPoint(playerid, 40.0, x, y, z))
	{
		Util_Send(playerid, -10270721, "Devi trovarti al deposito del tuo lavoro per iniziare il turno.");
		return 0;
	}
	if (g_JPool >= 40)
	{
		Util_Send(playerid, -10270721, "Nessun veicolo di lavoro disponibile al momento. Riprova tra poco.");
		return 0;
	}
	new veh = CreateVehicle(g_JobModel[job], x, y, z, a, 1, 1, -1, 0);
	if (veh == 65535 || veh <= 0 || veh > 2000)
	{
		Util_Send(playerid, -10270721, "Impossibile creare il veicolo di lavoro. Riprova.");
		return 0;
	}
	SetVehicleHealth(veh, 1000.0);
	SetVehicleParamsEx(veh, 1, 0, 0, 0, 0, 0, 0);
	g_JSerialSeq++;
	g_JVehOwner[veh] = playerid;
	g_JVehSerial[veh] = g_JSerialSeq;
	g_JPool++;
	g_JsActive[playerid] = 1;
	g_JsJob[playerid] = job;
	g_JsVeh[playerid] = veh;
	g_JsSerial[playerid] = g_JSerialSeq;
	g_JsStep[playerid] = 0;
	g_JsEarned[playerid] = 0;
	g_JsCapUsed[playerid] = 0;
	g_JsCapWarned[playerid] = 0;
	g_JsDwelling[playerid] = 0;
	g_JsOff[playerid] = 0;
	g_JsStartTick[playerid] = GetTickCount();
	g_JsLastCpTick[playerid] = g_JsStartTick[playerid];
	g_JsLastPos[playerid][0] = x;
	g_JsLastPos[playerid][1] = y;
	g_JsLastPos[playerid][2] = z;
	Job_BuildRoute(playerid, job);
	if (!PutPlayerInVehicle(playerid, veh, 0))
	{
		Job_Teardown(playerid, 0);
		Util_Send(playerid, -10270721, "Impossibile salire sul veicolo di lavoro. Riprova.");
		return 0;
	}
	Job_ShowCheckpoint(playerid);
	UI_JobHudShow(playerid, job);
	SendClientMessage(playerid, 0x66DD66FF, "Turno iniziato (%s). Segui i checkpoint sulla mappa: fermati nel cerchio (max %.0f km/h) per %d secondi. /fine per terminare.", g_JobName[job], 12.0, g_JobDwellMs[job] / 1000);
}

Job_CmdInizia(playerid)
{
	Job_Start(playerid);
}

Job_CmdFine(playerid)
{
	if (!g_JsActive[playerid])
	{
		Util_Send(playerid, -10270721, "Non hai nessun turno in corso.");
		return 0;
	}
	Job_Finish(playerid, 0);
}

Job_CmdLavoro(playerid, params[])
{
	new pos = 0, sub[12];
	Util_NextToken(params, pos, sub, 12);
	if (sub[0] == 0 || !strcmp(sub, "info", 1, 0x7FFFFFFF))
	{
		UI_JobInfoShow(playerid, gPlayer[playerid][pJob], gPlayer[playerid][pLevel], g_JcSessRuns[playerid], g_JcSessEarned[playerid]);
		return 0;
	}
	new bool:take = !strcmp(sub, "prendi", 1, 0x7FFFFFFF), bool:leave = !strcmp(sub, "lascia", 1, 0x7FFFFFFF);
	if (!take && !leave)
	{
		Util_Send(playerid, 0x99CCFFFF, "Uso: /lavoro [info|prendi|lascia]  (prendi e lascia solo al Job Center)");
		return 0;
	}
	if (!Job_NearCenter(playerid))
	{
		Util_Send(playerid, -10270721, "Devi essere al Job Center (piazza City Hall).");
		return 0;
	}
	if (g_JsActive[playerid])
	{
		Util_Send(playerid, -10270721, "Termina prima il turno in corso (/fine).");
		return 0;
	}
	if (leave && gPlayer[playerid][pJob] == 0)
	{
		Util_Send(playerid, -10270721, "Non hai un lavoro da lasciare.");
		return 0;
	}
	new left = Job_CdLeftMs(g_JcChgSet[playerid], g_JcChgLast[playerid], 600000);
	UI_JobCenterShow(playerid, gPlayer[playerid][pLevel], gPlayer[playerid][pJob], Job_SecCeil(left));
}

public UI_OnJobSelect(playerid, jobIndex)
{
	if (!Util_IsPlaying(playerid))
	{
		return 1;
	}
	if (jobIndex < 1 || jobIndex > 3)
	{
		return 1;
	}
	if (!Job_NearCenter(playerid))
	{
		Util_Send(playerid, -10270721, "Devi essere al Job Center.");
		return 1;
	}
	if (g_JsActive[playerid] || g_JpPending[playerid] || g_JcSetPending[playerid] || gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Util_Send(playerid, -10270721, "Non puoi cambiare lavoro adesso.");
		return 1;
	}
	if (gPlayer[playerid][pLevel] < g_JobMinLevel[jobIndex])
	{
		SendClientMessage(playerid, -10270721, "Per questo lavoro serve il livello %d.", g_JobMinLevel[jobIndex]);
		return 1;
	}
	if (gPlayer[playerid][pJob] == jobIndex)
	{
		Util_Send(playerid, -10270721, "Fai gia' questo lavoro.");
		return 1;
	}
	new left = Job_CdLeftMs(g_JcChgSet[playerid], g_JcChgLast[playerid], 600000);
	if (left > 0)
	{
		SendClientMessage(playerid, -10270721, "Potrai cambiare lavoro tra %d secondi.", Job_SecCeil(left));
		return 1;
	}
	g_JcSetPending[playerid] = 1;
	g_JcSetJob[playerid] = jobIndex;
	DB_SetJob(playerid, gPlayer[playerid][pCharId], jobIndex);
	return 1;
}

public UI_OnJobQuit(playerid)
{
	if (!Util_IsPlaying(playerid))
	{
		return 1;
	}
	if (!Job_NearCenter(playerid))
	{
		Util_Send(playerid, -10270721, "Devi essere al Job Center.");
		return 1;
	}
	if (gPlayer[playerid][pJob] == 0 || g_JsActive[playerid] || g_JpPending[playerid] || g_JcSetPending[playerid])
	{
		Util_Send(playerid, -10270721, "Non puoi lasciare il lavoro adesso.");
		return 1;
	}
	g_JcSetPending[playerid] = 1;
	g_JcSetJob[playerid] = 0;
	DB_SetJob(playerid, gPlayer[playerid][pCharId], 0);
	return 1;
}

public Job_OnDBSet(playerid, jobId, result)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (!g_JcSetPending[playerid] || g_JcSetJob[playerid] != jobId)
	{
		return 1;
	}
	g_JcSetPending[playerid] = 0;
	if (result == 1)
	{
		gPlayer[playerid][pJob] = jobId;
		g_JcChgSet[playerid] = 1;
		g_JcChgLast[playerid] = GetTickCount();
		if (jobId == 0)
		{
			Util_Send(playerid, 0x66DD66FF, "Hai lasciato il lavoro.");
		}
		else
		{
			SendClientMessage(playerid, 0x66DD66FF, "Ora lavori come %s. Usa /inizia per iniziare un turno.", g_JobName[jobId]);
		}
		new name[25], msg[128];
		GetPlayerName(playerid, name, 25);
		format(msg, 128, "%s lavoro -> %d", name, jobId);
		Log_Write("JOB", msg);
	}
	else if (result == 0)
	{
		Util_Send(playerid, -10270721, "Cambio di lavoro non eseguito.");
	}
	else
	{
		Util_Send(playerid, -10270721, "Servizio temporaneamente non disponibile: riprova piu' tardi.");
	}
	return 1;
}

Job_OnInit()
{
	for (new v = 0; v <= 2000; v++)
	{
		g_JVehOwner[v] = 65535;
		g_JVehSerial[v] = 0;
	}
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		Job_ResetPlayer(p);
	}
	g_JPool = 0;
	g_JTimer = SetTimer("Job_OnTick", 500, 1);
	CreatePickup(1239, 1, 1480.5, -1736.0, 13.5, 0);
	Lang_Create3DTextLabel("Job Center\nUsa /lavoro", -2994689, 1480.5, -1736.0, (13.5 + 1.0), 15.0, 0, 0);
}

Job_OnExit()
{
	if (g_JTimer != 0)
	{
		KillTimer(g_JTimer);
	}
	g_JTimer = 0;
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_JsActive[i])
		{
			Job_Teardown(i, 0);
		}
	}
}

Job_OnPlayerConnect(playerid)
{
	Job_ResetPlayer(playerid);
}

Job_OnPlayerDisconnect(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (g_JsActive[playerid])
	{
		Job_Cancel(playerid, "");
	}
	Job_ResetPlayer(playerid);
}

Job_OnPlayerDeath(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	Job_Cancel(playerid, "sei morto o ferito.");
}

Job_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	new owner = Job_VehicleOwner(vehicleid);
	if (owner == 65535 || owner == playerid)
	{
		return 0;
	}
	if (ispassenger == 0)
	{
		ClearAnimations(playerid, 0);
		Util_Send(playerid, -10270721, "Questo veicolo di lavoro e' in uso da un altro lavoratore.");
	}
}

Job_OnPlayerExitVehicle(playerid, vehicleid)
{
	if (!g_JsActive[playerid] || g_JsVeh[playerid] != vehicleid)
	{
		return 0;
	}
	if (!g_JsOff[playerid])
	{
		g_JsOff[playerid] = 1;
		g_JsOffTick[playerid] = GetTickCount();
		SendClientMessage(playerid, -10270721, "Torna sul veicolo di lavoro entro %d secondi o il turno sara' annullato.", 45);
	}
}

Job_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate)
{
	if (newstate != 2)
	{
		return 0;
	}
	new veh = GetPlayerVehicleID(playerid), owner = Job_VehicleOwner(veh);
	if (owner == 65535)
	{
		return 0;
	}
	if (owner != playerid)
	{
		RemovePlayerFromVehicle(playerid, 0);
		Util_Send(playerid, -10270721, "Questo veicolo di lavoro e' in uso da un altro lavoratore.");
		return 0;
	}
	g_JsOff[playerid] = 0;
}

Job_OnVehicleDeath(vehicleid)
{
	new owner = Job_VehicleOwner(vehicleid);
	if (owner != 65535)
	{
		Job_Cancel(owner, "veicolo di lavoro distrutto.");
	}
}
