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
	g_FrtActive[playerid] = false;
	g_FrtWelcome[playerid] = false;
	g_FrtCt[playerid] = 0;
	g_FrtFrom[playerid] = 0;
	g_FrtTo[playerid] = 0;
	g_FrtCargo[playerid] = 0;
	g_FrtDist[playerid] = 0;
	g_FrtPhase[playerid] = 0;
	g_FrtVeh[playerid] = 65535;
	g_FrtSerial[playerid] = 0;
	g_FrtBoarded[playerid] = false;
	g_FrtDwell[playerid] = false;
	g_FrtOff[playerid] = false;
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
				y = -1790.0;
				z = 13.5;
			}
			else
			{
				x = 1880.0;
				y = -1780.0;
				z = 13.5;
			}
		}
		case 2:
		{
			if (shop)
			{
				x = 2490.0;
				y = -1690.0;
				z = 13.5;
			}
			else
			{
				x = 2400.0;
				y = -1740.0;
				z = 13.5;
			}
		}
		case 3:
		{
			if (shop)
			{
				x = 2190.0;
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
			return false;
		}
	}
	return true;
}

Freight_PairDistance(fromD, toD)
{
	new Float:ax, Float:ay, Float:az, Float:bx, Float:by, Float:bz;
	if (!Freight_Point(fromD, false, ax, ay, az) || !Freight_Point(toD, true, bx, by, bz))
	{
		return 0;
	}
	new Float:dx = (bx - ax), Float:dy = (by - ay);
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), floatround_method:1);
}

Freight_CargoName(cargo, dest[], size)
{
	switch (cargo)
	{
		case 2:
		{
			strcopy(dest, "სურსათი", size);
		}
		case 3:
		{
			strcopy(dest, "materiali", size);
		}
		default:
		{
			strcopy(dest, "სხვადასხვა ტვირთი", size);
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

Freight_Note(playerid, const text[])
{
	new now = GetTickCount();
	if (g_FrtNoteTick[playerid] != 0 && now - g_FrtNoteTick[playerid] < 4000)
	{
		return 0;
	}
	g_FrtNoteTick[playerid] = now;
	Util_Send(playerid, -10270721, text);
	return 0;
}

Freight_IntegrityOf(vehicleid)
{
	new Float:hp;
	GetVehicleHealth(vehicleid, hp);
	return LD_Clamp(floatround((hp / 10.0), floatround_method:1), 0, 100);
}

bool:Freight_VehOk(playerid)
{
	new veh = g_FrtVeh[playerid];
	if (veh == 65535 || veh <= 0 || veh >= 2000 || !IsValidVehicle(veh))
	{
		return false;
	}
	return LD_VehModule(veh) == 3 && LD_VehOwner(veh) == playerid && LD_VehSerial(veh) == g_FrtSerial[playerid];
}

bool:Freight_IsDriving(playerid)
{
	return Freight_VehOk(playerid) && GetPlayerVehicleID(playerid) == g_FrtVeh[playerid] && GetPlayerState(playerid) == PLAYER_STATE_DRIVER;
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
					RemovePlayerFromVehicle(playerid, false);
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
	return 0;
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
	new name[48], goal[288], dest[288];
	new bool:shop = g_FrtPhase[playerid] >= 3;
	LD_DistrictName(shop ? g_FrtTo[playerid] : g_FrtFrom[playerid], name, 48);
	format(dest, 288, "%s %s", shop ? "მაღაზია" : "საწყობი", name);
	switch (g_FrtPhase[playerid])
	{
		case 2:
		{
			strcopy(goal, "ტვირთის რეგისტრაცია...", 96);
		}
		case 4:
		{
			strcopy(goal, "მიწოდების რეგისტრაცია...", 96);
		}
		case 3:
		{
			if (dwellSec >= 0)
			{
				format(goal, 288, "გადმოტვირთვა: იდექი %d წმ", dwellSec);
			}
			else if (g_FrtOff[playerid])
			{
				strcopy(goal, "დაბრუნდი Mule-ის საჭესთან!", 96);
			}
			else
			{
				strcopy(goal, "მიიტანე ტვირთი მაღაზიაში", 96);
			}
		}
		default:
		{
			if (dwellSec >= 0)
			{
				format(goal, 288, "დატვირთვა: იდექი %d წმ", dwellSec);
			}
			else if (!g_FrtBoarded[playerid])
			{
				strcopy(goal, "ჩაჯექი Mule-ში საწყობთან", 96);
			}
			else if (g_FrtOff[playerid])
			{
				strcopy(goal, "დაბრუნდი Mule-ის საჭესთან!", 96);
			}
			else
			{
				strcopy(goal, "შედი საწყობის ჩექპოინტში", 96);
			}
		}
	}
	UI_CityContractHud(playerid, g_FrtCt[playerid], shop ? 2 : 1, goal, dest, distM, Freight_TimeLeft(playerid), shop ? g_FrtInteg[playerid] : -1);
	return 0;
}

Freight_End(playerid, reason)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid])
	{
		return 0;
	}
	new bool:welcome = g_FrtWelcome[playerid];
	new bool:online = IsPlayerConnected(playerid);
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
				Util_Send(playerid, -1, "გადაზიდვის კონტრაქტი დაიხურა: Mule ჩამოგერთვა.");
			}
			case 5:
			{
				Util_Send(playerid, -10270721, "გადაზიდვის კონტრაქტს ვადა გაუვიდა ან აღარ მოქმედებს: Mule ჩამოგერთვა.");
			}
			case 7:
			{
				Util_Send(playerid, -10270721, "ადმინისტრაციამ გადაზიდვის კონტრაქტი გააუქმა.");
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
		Hub_OnWelcomeDelivered(playerid, false);
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
	return 0;
}

Freight_Stop(playerid, reason, const text[])
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
	return 0;
}

Freight_Picked(playerid)
{
	new now = GetTickCount();
	g_FrtPhase[playerid] = 3;
	g_FrtDwell[playerid] = false;
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
	new name[48];
	LD_DistrictName(g_FrtTo[playerid], name, 48);
	if (g_FrtWelcome[playerid])
	{
		UI_LivingToast(playerid, "ტვირთი დატვირთულია: მიიტანე Idlewood-ის მაღაზიაში.", 1);
	}
	SendClientMessage(playerid, 0x66DD66FF, "ტვირთი დატვირთულია. მიიტანე %s-ის მაღაზიაში: ფრთხილად იარე, Mule-ის დაზიანება ტვირთს აფუჭებს.", name);
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
			Util_Send(playerid, -10270721, "ტვირთი ჯერ ვერ დარეგისტრირდა: დარჩი ჩექპოინტში და რამდენიმე წამში სცადე.");
		}
	}
	return 0;
}

Freight_DoDeliver(playerid)
{
	new now = GetTickCount();
	new seconds = (now - gPlayer[playerid][pFreightLegTick]) / 1000;
	new minSec = LD_FreightMinSeconds(g_FrtDist[playerid]);
	if (seconds < minSec)
	{
		new msg[768];
		format(msg, 768, "consegna troppo rapida: pers. %d contratto %d dopo %d s (minimo %d s)", gPlayer[playerid][pCharId], g_FrtCt[playerid], seconds, minSec);
		Log_Write("FREIGHT", msg);
		format(msg, 768, "მიწოდება არასწორია: ძალიან სწრაფია (კიდევ %d წმ). დარჩი ჩექპოინტში.", minSec - seconds);
		Freight_Note(playerid, msg);
		g_FrtRetryTick[playerid] = now + 5000;
		return 0;
	}
	if (!g_FrtWelcome[playerid] && g_FrtDist[playerid] < 300)
	{
		Freight_Stop(playerid, 5, "კონტრაქტი არასწორია (მარშრუტი ძალიან მოკლეა).");
		return 0;
	}
	new Float:minPath = float(g_FrtDist[playerid] * 80 / 100);
	if ((g_FrtPath[playerid] < minPath))
	{
		new msg[768];
		format(msg, 768, "percorso insufficiente: pers. %d contratto %d percorso %.0f m su %d m", gPlayer[playerid][pCharId], g_FrtCt[playerid], g_FrtPath[playerid], g_FrtDist[playerid]);
		Log_Write("FREIGHT", msg);
		Freight_Note(playerid, "მიწოდება არასწორია: ტვირთი საწყობიდან Mule-ით უნდა ჩამოიტანო.");
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
		Util_Send(playerid, 0x66DD66FF, "მისასალმებელი მიწოდება დასრულდა: Idlewood-ის მაღაზია გიხდის მადლობას!");
		Hub_OnWelcomeDelivered(playerid, true);
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
			Util_Send(playerid, -10270721, "მიწოდება ჯერ ვერ დარეგისტრირდა: დარჩი ჩექპოინტში და რამდენიმე წამში სცადე.");
		}
	}
	return 0;
}

bool:Freight_Begin(playerid, contractId, fromD, toD, cargo, distanceM, bool:welcome)
{
	if (!Freight_ValidId(playerid) || !Util_IsPlaying(playerid) || g_FrtActive[playerid])
	{
		return false;
	}
	if (!LD_IsReady() || gPlayer[playerid][pLivingReady] == 0 || gPlayer[playerid][pCharId] <= 0)
	{
		return false;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		return false;
	}
	if (Job_IsOnShift(playerid) || gPlayer[playerid][pTaxiOnDuty] != 0 || gPlayer[playerid][pRentalId] != 0)
	{
		return false;
	}
	if (welcome)
	{
		if (contractId != 0 || gPlayer[playerid][pContractId] != 0)
		{
			return false;
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
			return false;
		}
		if (gPlayer[playerid][pContractId] != 0 && gPlayer[playerid][pContractId] != contractId)
		{
			return false;
		}
		if (fromD == toD || cargo < 1 || cargo > 3)
		{
			return false;
		}
		if (gPlayer[playerid][pLevel] < 2)
		{
			return false;
		}
	}
	new pair = Freight_PairDistance(fromD, toD);
	if (pair <= 0)
	{
		return false;
	}
	new dist = welcome ? pair : (distanceM > pair ? distanceM : pair);
	if (!welcome && dist < 300)
	{
		return false;
	}
	if (!LD_PoolHasRoom())
	{
		Util_Send(playerid, -10270721, "სატვირთო მანქანა ახლა არ არის თავისუფალი. სცადე ცოტა ხანში.");
		return false;
	}
	new Float:x, Float:y, Float:z, slot = 0;
	Freight_Point(fromD, false, x, y, z);
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_FrtActive[i] && g_FrtFrom[i] == fromD && g_FrtPhase[i] == 1)
		{
			slot++;
		}
	}
	x += (5.0 * float(slot % 4));
	new veh = CreateVehicle(414, x, y, z, 0.0, 1, 1, -1, false);
	if (veh == 65535 || veh <= 0 || veh >= 2000)
	{
		Util_Send(playerid, -10270721, "სატვირთო მანქანის მომზადება ვერ მოხერხდა. სცადე თავიდან.");
		return false;
	}
	new serial = LD_VehRegister(veh, 3, playerid);
	if (serial == 0)
	{
		DestroyVehicle(veh);
		Util_Send(playerid, -10270721, "სატვირთო მანქანა ახლა არ არის თავისუფალი. სცადე ცოტა ხანში.");
		return false;
	}
	SetVehicleVirtualWorld(veh, 0);
	SetVehicleHealth(veh, 1000.0);
	SetVehicleParamsEx(veh, 1, 0, 0, 0, 0, 0, 0);
	new now = GetTickCount();
	Freight_ResetPlayer(playerid);
	g_FrtActive[playerid] = true;
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
	new fromName[48], toName[48], cargoName[288];
	LD_DistrictName(fromD, fromName, 48);
	LD_DistrictName(toD, toName, 48);
	Freight_CargoName(cargo, cargoName, 288);
	if (welcome)
	{
		SendClientMessage(playerid, 0x66DD66FF, "მისასალმებელი მიწოდება: ჩაჯექი Mule-ში Idlewood-ის საწყობთან, ჩექპოინტში %d წმ იდექი, რომ დატვირთო %s, შემდეგ მიიტანე Idlewood-ის მაღაზიაში.", 6, cargoName);
		UI_LivingToast(playerid, "ჩაჯექი Mule-ში Idlewood-ის საწყობთან.", 0);
	}
	else
	{
		SendClientMessage(playerid, 0x66DD66FF, "კონტრაქტი #%d: აიყვანე Mule %s-ის საწყობიდან, ჩექპოინტში (მაქს. %.0f კმ/სთ) %d წმ იდექი, რომ დატვირთო %s, შემდეგ მიიტანე %s-ის მაღაზიაში (%d მ).", contractId, fromName, 0x41200000, 6, cargoName, toName, dist);
		Freight_Hud(playerid, -1, -1);
	}
	return true;
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
			g_FrtDwell[playerid] = false;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "ტვირთის რეგისტრაცია ვერ მოხერხდა: ხელახლა საცდელად დარჩი ჩექპოინტში.");
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
				SendClientMessage(playerid, -1, "კონტრაქტის #%d მიწოდება უკვე დარეგისტრირებულია: ახალი გადახდა არ იქნება.", ct);
			}
			else if (result == 6)
			{
				SendClientMessage(playerid, 0x66DD66FF, "მიწოდება #%d დასრულდა: %s ჩაირიცხა ბანკში (საათობრივი ლიმიტი მიღწეულია). ბალანსი: %s.", ct, Money_Str(paid), Money_Str(balance));
			}
			else
			{
				SendClientMessage(playerid, 0x66DD66FF, "მიწოდება #%d დასრულდა: %s ჩაირიცხა ბანკში. ბალანსი: %s.", ct, Money_Str(paid), Money_Str(balance));
			}
		}
		case 8:
		{
			Freight_End(playerid, 0);
			Util_Send(playerid, -10270721, "კონტრაქტი ჩაიშალა: ტვირთი ძალიან დაზიანებული ჩავიდა. ანაზღაურება არ არის.");
		}
		case -1, 10:
		{
			if (g_FrtDefer[playerid] != 0)
			{
				Freight_Release(playerid, g_FrtDefer[playerid]);
				return 1;
			}
			g_FrtPhase[playerid] = 3;
			g_FrtDwell[playerid] = false;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "მიწოდების რეგისტრაცია ვერ მოხერხდა: ხელახლა საცდელად დარჩი ჩექპოინტში.");
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
			g_FrtDwell[playerid] = false;
			g_FrtRetryTick[playerid] = GetTickCount() + 5000;
			Util_Send(playerid, -10270721, "რეესტრმა მიწოდება უარყო (ძალიან სწრაფია ან სტატუსი არასწორია): ჩექპოინტში რამდენიმე წამი დაელოდე.");
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
	g_FrtDwell[playerid] = false;
	g_FrtRetryTick[playerid] = GetTickCount() + 5000;
	return 0;
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
	if ((seg > (15.0 + ((70.0 * float(dt)) / float(1000)))))
	{
		new msg[256];
		format(msg, 256, "salto del Mule: pers. %d contratto %d %.0f m in %d ms", gPlayer[playerid][pCharId], g_FrtCt[playerid], seg, dt);
		Log_Write("FREIGHT", msg);
		return false;
	}
	g_FrtPath[playerid] += seg;
	return true;
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
		Freight_Stop(playerid, 1, "სამსახურის ცვლა დაიწყე: გადაზიდვის კონტრაქტი დაიხურა.");
		return 0;
	}
	if (gPlayer[playerid][pWounded] || gPlayer[playerid][pCuffed])
	{
		Freight_Stop(playerid, 3, "მართვის მდგომარეობაში არ ხარ: გადაზიდვის კონტრაქტი შეწყდა.");
		return 0;
	}
	if (!Freight_VehOk(playerid))
	{
		Freight_Stop(playerid, 4, "კონტრაქტის Mule აღარ არსებობს: კონტრაქტი შეწყდა.");
		return 0;
	}
	new veh = g_FrtVeh[playerid];
	new Float:hp;
	GetVehicleHealth(veh, hp);
	if ((hp < 300.0))
	{
		Freight_Stop(playerid, 4, "Mule განადგურდა: ტვირთი დაიკარგა, კონტრაქტი შეწყდა.");
		return 0;
	}
	new bool:driving = Freight_IsDriving(playerid);
	if (driving)
	{
		g_FrtBoarded[playerid] = true;
		g_FrtOff[playerid] = false;
		gPlayer[playerid][pFreightLeftVehTick] = 0;
	}
	else if (g_FrtBoarded[playerid])
	{
		if (!g_FrtOff[playerid])
		{
			g_FrtOff[playerid] = true;
			gPlayer[playerid][pFreightLeftVehTick] = now;
		}
		new Float:px, Float:py, Float:pz;
		GetPlayerPos(playerid, px, py, pz);
		if (now - gPlayer[playerid][pFreightLeftVehTick] >= 45000 || (GetVehicleDistanceFromPoint(veh, px, py, pz) > 150.0))
		{
			Freight_Stop(playerid, 4, "Mule მიტოვებულია: გადაზიდვის კონტრაქტი გაუქმდა.");
			return 0;
		}
	}
	else if (now - g_FrtStartTick[playerid] >= 600000)
	{
		Freight_Stop(playerid, 4, "Mule დროულად არ აიყვანე: გადაზიდვის კონტრაქტი გაუქმდა.");
		return 0;
	}
	if (ph == 3)
	{
		if (!Freight_SamplePath(playerid, now))
		{
			Freight_Stop(playerid, 4, "Mule-ის საეჭვო გადაადგილება: გადაზიდვის კონტრაქტი გაუქმდა.");
			return 0;
		}
		new integ = LD_Clamp(floatround((hp / 10.0), floatround_method:1), 0, 100);
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
	if (driving && retryOk && (dist <= 6.0) && (float(Veh_SpeedKmh(veh)) <= 10.0) && GetVehicleVirtualWorld(veh) == 0)
	{
		if (!g_FrtDwell[playerid])
		{
			g_FrtDwell[playerid] = true;
			gPlayer[playerid][pFreightDwellTick] = now;
			if (g_FrtWelcome[playerid])
			{
				UI_LivingToast(playerid, shop ? "მიმდინარეობს გადმოტვირთვა: იდექი." : "მიმდინარეობს დატვირთვა: იდექი.", 0);
			}
		}
		new need = shop ? 5000 : 6000;
		new waited = now - gPlayer[playerid][pFreightDwellTick];
		if (waited >= need)
		{
			g_FrtDwell[playerid] = false;
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
		g_FrtDwell[playerid] = false;
		gPlayer[playerid][pFreightDwellTick] = 0;
	}
	Freight_Hud(playerid, floatround(dist, floatround_method:1), dwellSec);
	return 0;
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
	g_FrtTimer = SetTimer("Freight_OnTick", 1000, true);
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
	return 0;
}

Freight_OnCharReady(playerid)
{
	#pragma unused playerid
}

Freight_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	if (!Freight_ValidId(playerid))
	{
		return 0;
	}
	if (g_FrtActive[playerid])
	{
		Freight_End(playerid, 2);
	}
	Freight_ResetPlayer(playerid);
	return 0;
}

Freight_OnPlayerDeath(playerid)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid])
	{
		return 0;
	}
	Freight_Stop(playerid, 3, "დაჭრილი ხარ: გადაზიდვის კონტრაქტი შეწყდა, Mule ჩამოგერთვა.");
	return 0;
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
	ClearAnimations(playerid, SYNC_NONE);
	Util_Send(playerid, -10270721, "ეს სატვირთო მანქანა სხვა გადამზიდს ეკუთვნის.");
	return 0;
}

Freight_OnPlayerExitVehicle(playerid, vehicleid)
{
	if (!Freight_ValidId(playerid) || !g_FrtActive[playerid] || g_FrtVeh[playerid] != vehicleid || !g_FrtBoarded[playerid])
	{
		return 0;
	}
	if (!g_FrtOff[playerid])
	{
		g_FrtOff[playerid] = true;
		gPlayer[playerid][pFreightLeftVehTick] = GetTickCount();
		SendClientMessage(playerid, -10270721, "დაბრუნდი Mule-ის საჭესთან %d წამში (და %.0f მ-ზე შორს არ წახვიდე), თორემ კონტრაქტი გაუქმდება.", 45, 0x43160000);
	}
	return 0;
}

Freight_OnPlayerStateChange(playerid, t_PLAYER_STATE:newstate, t_PLAYER_STATE:oldstate)
{
	#pragma unused oldstate
	if (newstate != PLAYER_STATE_DRIVER)
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
		RemovePlayerFromVehicle(playerid, false);
		Util_Send(playerid, -10270721, "ეს სატვირთო მანქანა სხვა გადამზიდს ეკუთვნის.");
		return 0;
	}
	if (g_FrtActive[playerid] && g_FrtVeh[playerid] == veh)
	{
		g_FrtBoarded[playerid] = true;
		g_FrtOff[playerid] = false;
		gPlayer[playerid][pFreightLeftVehTick] = 0;
	}
	return 0;
}

Freight_OnVehicleDeath(vehicleid, killerid)
{
	#pragma unused killerid
	if (LD_VehModule(vehicleid) != 3)
	{
		return 0;
	}
	new owner = LD_VehOwner(vehicleid);
	if (!Freight_ValidId(owner) || !g_FrtActive[owner] || g_FrtVeh[owner] != vehicleid)
	{
		return 0;
	}
	Freight_Stop(owner, 4, "Mule განადგურდა: ტვირთი დაიკარგა, კონტრაქტი შეწყდა.");
	return 0;
}
