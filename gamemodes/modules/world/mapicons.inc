// =============================================================================
// world/mapicons.inc - icone sul radar/mappa per ogni punto con pickup o servizio.
// Icone per giocatore (SetPlayerMapIcon, max 100): 0-30 servizi fissi, 31-95 case (g_HouseRows: max 64), 96-99 riservati.
// Stile locale: l'icona compare quando il punto e' nel raggio del radar e sulla mappa di pausa.
// ID icone di GTA SA: 52 $, 51 camion, 55 deposito, 27 officina, 53 bandiera, 36 scuola, 17 tavola calda,
// 63 Pay'n'Spray, 31 casa libera (verde), 32 casa occupata (rossa).
// =============================================================================

#define MICON_FIRST_HOUSE   (31)
#define MICON_MAX           (96)    // 96-99 riservati (hub: 97 = destinazione)

stock MapIcons_Set(playerid, &slot, Float:x, Float:y, Float:z, icon)
{
	if (slot >= MICON_FIRST_HOUSE)
	{
		return 0;
	}
	SetPlayerMapIcon(playerid, slot, x, y, z, icon, 0, MAPICON_LOCAL);
	slot++;
	return 1;
}

MapIcons_Apply(playerid)
{
	if (!IsPlayerConnected(playerid) || IsPlayerNPC(playerid))
	{
		return 0;
	}
	new s = 0;
	for (new i = 0; i < sizeof s_BankPoints; i++)
	{
		MapIcons_Set(playerid, s, s_BankPoints[i][0], s_BankPoints[i][1], s_BankPoints[i][2], 52);   // banca e ATM
	}
	MapIcons_Set(playerid, s, 1480.5, -1736.0, 13.5, 51);          // Job Center (Job_NearCenter)
	MapIcons_Set(playerid, s, 1505.0, -1700.0, 13.5, 55);          // noleggio (Rental_AtOffice)
	MapIcons_Set(playerid, s, 1771.0, -1895.0, 13.6, 53);          // deposito taxi (Taxi_AtDepot)
	MapIcons_Set(playerid, s, 1655.0, -1665.0, 22.5, 27);          // concessionario (Veh_NearDealer)
	for (new i = 0; i < sizeof g_VehStations; i++)
	{
		MapIcons_Set(playerid, s, g_VehStations[i][0], g_VehStations[i][1], g_VehStations[i][2], 63);   // distributori
	}
	for (new i = 0; i < sizeof s_HubBoard; i++)
	{
		MapIcons_Set(playerid, s, s_HubBoard[i][0], s_HubBoard[i][1], s_HubBoard[i][2], 36);   // bacheche contratti
	}
	for (new i = 0; i < sizeof s_HsShops; i++)
	{
		MapIcons_Set(playerid, s, s_HsShops[i][0], s_HsShops[i][1], s_HsShops[i][2], 17);     // negozi
	}
	for (new i = 0; i < g_HouseCount && MICON_FIRST_HOUSE + i < MICON_MAX; i++)
	{
		MapIcons_House(playerid, i);
	}
	return 1;
}

MapIcons_House(playerid, idx)
{
	if (MICON_FIRST_HOUSE + idx >= MICON_MAX)
	{
		return 0;
	}
	SetPlayerMapIcon(playerid, MICON_FIRST_HOUSE + idx, g_HouseRows[idx][hrExtX], g_HouseRows[idx][hrExtY], g_HouseRows[idx][hrExtZ],
		g_HouseRows[idx][hrOwnerId] <= 0 ? 31 : 32, 0, MAPICON_LOCAL);
	return 1;
}

// casa comprata/venduta/creata: aggiorna l'icona per tutti
MapIcons_OnHouseChanged(idx)
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (IsPlayerConnected(p) && gPlayer[p][pSpawned])
		{
			MapIcons_House(p, idx);
		}
	}
}
