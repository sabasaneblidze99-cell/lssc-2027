// =============================================================================
// rental/street_rental.inc - auto a noleggio parcheggiate per la citta' ("noleggio di strada").
//
// Berline di classe base (Landstalker, Premier, Admiral, ...) parcheggiate in punti casuali di Los Santos.
// Il giocatore sale al volante -> offerta $179 per 60 minuti (nessuna cauzione). Il pagamento passa dallo
// stesso sistema dei noleggi (sp_rental_open, vkind 5, migrazione sql/migrations/013_street_rental.sql):
// stesse regole (un solo noleggio alla volta, op_id idempotente, banca + contanti), stessa scadenza e riconsegna.
// La riconsegna si fa ovunque con /riconsegna (o scade da sola); poi l'auto sparisce e un'altra compare altrove.
// =============================================================================

#define SRENT_VKIND        (5)
#define SRENT_PRICE        (179)          // deve coincidere con ld_config RENT_FEE_STREET (il DB decide l'addebito)
#define SRENT_MINUTES      (60)           // = RENT_STREET_MIN
#define SRENT_ACTIVE       (10)           // auto disponibili in contemporanea
#define SRENT_RESPAWN_S    (180)          // dopo un noleggio, un'auto nuova compare entro ~3 minuti
#define SRENT_DIALOG       (9800)

static const s_SrModels[] = { 400, 426, 445, 421, 405, 507, 546, 547, 404, 439, 492, 551, 529, 540, 585, 550 };

// punti di parcheggio all'aperto (interior 0, mondo 0): x, y, z, angolo
static const Float:s_SrSpots[][4] = {
	{1524.0, -1716.0, 13.4, 90.0},      // Pershing Square, vicino all'agenzia
	{1546.0, -1676.0, 13.4, 90.0},      // parcheggio LSPD, Pershing Square
	{1773.0, -1916.0, 13.4, 270.0},     // Unity Station
	{1745.0, -1862.0, 13.4, 0.0},       // Unity Station, uscita
	{1936.0, -1789.0, 13.4, 270.0},     // distributore di Idlewood
	{2079.0, -1808.0, 13.4, 90.0},      // Idlewood, strada principale
	{2228.0, -1735.0, 13.4, 270.0},     // Ganton, palestra
	{2486.0, -1667.0, 13.2, 90.0},      // Grove Street
	{2408.0, -1720.0, 13.5, 0.0},       // Ganton
	{2215.0, -1150.0, 25.7, 270.0},     // Jefferson, motel
	{2128.0, -1196.0, 23.8, 180.0},     // Jefferson
	{1971.0, -1197.0, 25.6, 0.0},       // Glen Park
	{1180.0, -1340.0, 13.6, 270.0},     // All Saints General Hospital
	{1185.0, -1338.0, 13.6, 270.0},     // All Saints, secondo posto
	{1004.0, -946.0, 42.0, 0.0},        // distributore di Mulholland
	{2116.0, -1903.0, 13.4, 0.0},       // distributore di Willowfield
	{1685.0, -2245.0, 13.4, 180.0},     // aeroporto, parcheggio
	{1835.0, -1855.0, 13.4, 90.0}       // El Corona, negozio
};

new g_SrVeh[sizeof s_SrSpots];            // veicolo libero su quel punto (0 = vuoto)
new g_SrPend[MAX_PLAYERS];                // auto per cui il giocatore ha accettato l'offerta (attesa DB)
new g_SrOffer[MAX_PLAYERS];               // auto offerta (dialog aperto)
new g_SrNextSpawn;                        // gettime() del prossimo riempimento
new g_SrTimer;

StreetRental_OnInit()
{
	for (new i = 0; i < sizeof s_SrSpots; i++)
	{
		g_SrVeh[i] = 0;
	}
	for (new n = 0; n < SRENT_ACTIVE; n++)
	{
		StreetRental_SpawnOne();
	}
	g_SrTimer = SetTimer("StreetRental_OnTick", 10000, true);
}

StreetRental_OnExit()
{
	if (g_SrTimer)
	{
		KillTimer(g_SrTimer);
		g_SrTimer = 0;
	}
	for (new i = 0; i < sizeof s_SrSpots; i++)
	{
		if (g_SrVeh[i] != 0 && IsValidVehicle(g_SrVeh[i]))
		{
			DestroyVehicle(g_SrVeh[i]);
		}
		g_SrVeh[i] = 0;
	}
}

StreetRental_Count()
{
	new n = 0;
	for (new i = 0; i < sizeof s_SrSpots; i++)
	{
		if (g_SrVeh[i] != 0)
		{
			n++;
		}
	}
	return n;
}

StreetRental_SpotOf(vehicleid)
{
	if (vehicleid <= 0)
	{
		return -1;
	}
	for (new i = 0; i < sizeof s_SrSpots; i++)
	{
		if (g_SrVeh[i] == vehicleid)
		{
			return i;
		}
	}
	return -1;
}

bool:StreetRental_Watched(spot)
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (IsPlayerConnected(p) && GetPlayerInterior(p) == 0 &&
			GetPlayerDistanceFromPoint(p, s_SrSpots[spot][0], s_SrSpots[spot][1], s_SrSpots[spot][2]) < 25.0)
		{
			return true;
		}
	}
	return false;
}

// un'auto nuova su un punto libero scelto a caso (niente se un giocatore e' a meno di 25 m: non compare sotto gli occhi)
StreetRental_SpawnOne()
{
	new free[sizeof s_SrSpots], n = 0;
	for (new i = 0; i < sizeof s_SrSpots; i++)
	{
		if (g_SrVeh[i] == 0 && !StreetRental_Watched(i))
		{
			free[n++] = i;
		}
	}
	if (n == 0 || !LD_PoolHasRoom())
	{
		return 0;
	}
	new s = free[random(n)], model = s_SrModels[random(sizeof s_SrModels)];
	new col = random(126);
	new veh = CreateVehicle(model, s_SrSpots[s][0], s_SrSpots[s][1], s_SrSpots[s][2], s_SrSpots[s][3], col, col, -1, false);
	if (veh == INVALID_VEHICLE_ID || veh <= 0)
	{
		return 0;
	}
	SetVehicleParamsEx(veh, 0, 0, 0, 0, 0, 0, 0);
	SetVehicleNumberPlate(veh, "RENT");
	g_SrVeh[s] = veh;
	return 1;
}

public StreetRental_OnTick()
{
	if (StreetRental_Count() < SRENT_ACTIVE && gettime() >= g_SrNextSpawn)
	{
		if (StreetRental_SpawnOne())
		{
			g_SrNextSpawn = gettime() + 20;
		}
	}
}

StreetRental_OnPlayerConnect(playerid)
{
	g_SrPend[playerid] = 0;
	g_SrOffer[playerid] = 0;
}

StreetRental_OnPlayerDisconnect(playerid)
{
	g_SrPend[playerid] = 0;
	g_SrOffer[playerid] = 0;
}

// al volante di un'auto libera: motore spento finche' non si noleggia
StreetRental_OnStateChange(playerid, PLAYER_STATE:newstate)
{
	if (newstate != PLAYER_STATE_DRIVER)
	{
		if (g_SrOffer[playerid] != 0)
		{
			g_SrOffer[playerid] = 0;
			HidePlayerDialog(playerid);
		}
		return 0;
	}
	new veh = GetPlayerVehicleID(playerid);
	if (StreetRental_SpotOf(veh) < 0)
	{
		return 0;
	}
	SetVehicleParamsEx(veh, 0, 0, 0, 0, 0, 0, 0);
	if (!Util_IsPlaying(playerid))
	{
		RemovePlayerFromVehicle(playerid);
		return 1;
	}
	if (gPlayer[playerid][pRentalId] != 0 || gPlayer[playerid][pRentalPendOp] != 0)
	{
		Util_Send(playerid, 0xFF6347FF, "უკვე გაქვს ნაქირავები მანქანა. დააბრუნე /riconsegna-ით.");
		RemovePlayerFromVehicle(playerid);
		return 1;
	}
	new name[24], body[320];
	Veh_StdModelName(GetVehicleModel(veh), name);
	format(body, sizeof body,
		"{FFFFFF}%s\n\n{AAAAAA}ქირა: {66DD66}$%d{AAAAAA} - %d წუთი\nგირაო არ არის. გადახდა: ბანკიდან ან ნაღდით.\n\n{AAAAAA}დაბრუნება ნებისმიერ ადგილას: /riconsegna",
		name, SRENT_PRICE, SRENT_MINUTES);
	g_SrOffer[playerid] = veh;
	Lang_ShowPlayerDialog(playerid, SRENT_DIALOG, DIALOG_STYLE_MSGBOX, "მანქანის ქირაობა", body, "ქირაობა", "გასვლა");
	return 1;
}

bool:StreetRental_OnDialogResponse(playerid, dialogid, response)
{
	if (dialogid != SRENT_DIALOG)
	{
		return false;
	}
	new veh = g_SrOffer[playerid];
	g_SrOffer[playerid] = 0;
	if (veh == 0 || GetPlayerVehicleID(playerid) != veh || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	{
		return true;
	}
	if (!response)
	{
		RemovePlayerFromVehicle(playerid);
		return true;
	}
	if (StreetRental_SpotOf(veh) < 0 || gPlayer[playerid][pRentalId] != 0 || gPlayer[playerid][pRentalPendOp] != 0)
	{
		RemovePlayerFromVehicle(playerid);
		return true;
	}
	new have = (gPlayer[playerid][pBank] > 0 ? gPlayer[playerid][pBank] : 0) + (gPlayer[playerid][pMoney] > 0 ? gPlayer[playerid][pMoney] : 0);
	if (have < SRENT_PRICE)
	{
		Util_Send(playerid, 0xFF6347FF, "არ გაქვს საკმარისი თანხა (ბანკი + ნაღდი).");
		RemovePlayerFromVehicle(playerid);
		return true;
	}
	g_SrPend[playerid] = veh;
	if (!Rental_BeginOpen(playerid, SRENT_VKIND, 1, 1, GetVehicleModel(veh)))
	{
		g_SrPend[playerid] = 0;
		Util_Send(playerid, 0xFF6347FF, "ქირაობა ახლა შეუძლებელია, სცადე მოგვიანებით.");
		RemovePlayerFromVehicle(playerid);
		return true;
	}
	Util_Send(playerid, -1, "ქირაობის დამუშავება...");
	return true;
}

// chiamata da Rental_SpawnVehicle per vkind 5: l'auto c'e' gia', si consegna quella parcheggiata
bool:StreetRental_Take(playerid)
{
	new veh = g_SrPend[playerid];
	g_SrPend[playerid] = 0;
	new s = StreetRental_SpotOf(veh);
	if (s < 0 || !IsValidVehicle(veh))
	{
		return false;
	}
	if (LD_VehRegister(veh, 1, playerid) == 0)
	{
		return false;
	}
	g_SrVeh[s] = 0;                       // non piu' libera: il punto si riempira' con un'altra auto
	g_SrNextSpawn = gettime() + SRENT_RESPAWN_S;
	SetVehicleParamsEx(veh, 1, 0, 0, 0, 0, 0, 0);
	gPlayer[playerid][pRentalVehicle] = veh;
	if (GetPlayerVehicleID(playerid) != veh)
	{
		PutPlayerInVehicle(playerid, veh, 0);
	}
	return true;
}

// l'auto libera e' stata distrutta (incidente, esplosione): si libera il punto
StreetRental_OnVehicleDeath(vehicleid)
{
	new s = StreetRental_SpotOf(vehicleid);
	if (s < 0)
	{
		return 0;
	}
	g_SrVeh[s] = 0;
	DestroyVehicle(vehicleid);
	return 1;
}
