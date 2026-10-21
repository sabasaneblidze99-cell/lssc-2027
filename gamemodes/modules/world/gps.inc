// =============================================================================
// world/gps.inc - GPS: /gps apre il navigatore, la destinazione diventa un checkpoint "da gara" sul radar.
//
// Destinazioni: luoghi importanti di Los Santos (world/az_data.inc, dal GPS di Arizona + porte pubbliche),
// servizi di LS Crime's (Job Center, taxi, noleggio, autosalone, supermercati, case di quartiere),
// "il piu' vicino" (banca/ATM, benzinaio, business Arizona per tipo) e la propria casa.
// Usa il checkpoint da gara (RaceCheckpoint): i checkpoint normali restano a lavori, consegne e aiuto.
// =============================================================================

#define GPS_DLG_MAIN     (9900)
#define GPS_DLG_LIST     (9901)
#define GPS_CAT_PLACES   (0)
#define GPS_CAT_SERVICES (1)
#define GPS_CAT_NEAR     (2)
#define GPS_RADIUS       (6.0)
#define GPS_MAX_ITEMS    (24)

#define GPS_SERVICES     (9)
new const s_GpsSvcName[GPS_SERVICES][128] = {
	"დასაქმების ცენტრი (Job Center)",
	"ტაქსის დეპო",
	"მანქანების ქირაობა",
	"ავტოსალონი",
	"სუპერმარკეტი (Idlewood)",
	"სუპერმარკეტი (Mulholland)",
	"უბნის სახლი (Idlewood)",
	"უბნის სახლი (Ganton)",
	"უბნის სახლი (Jefferson)"
};
new const Float:s_GpsSvcPos[GPS_SERVICES][3] = {
	{1480.5, -1736.0, 13.5},
	{1771.0, -1895.0, 13.6},
	{1505.0, -1700.0, 13.5},
	{1655.0, -1665.0, 22.5},
	{1833.0, -1842.0, 13.6},
	{1004.0, -926.0, 42.3},
	{1850.0, -1800.0, 13.6},
	{2490.0, -1678.0, 13.5},
	{2133.0, -1215.0, 24.0}
};

// "il piu' vicino": 0 banca/ATM, 1 benzinaio, poi i tipi dei business Arizona (s_AzBizType)
#define GPS_NEAR_KINDS   (8)
new const s_GpsNearName[GPS_NEAR_KINDS][96] = {
	"ბანკი / ბანკომატი",
	"ბენზინგასამართი სადგური",
	"მაღაზია 24/7",
	"სასადილო",
	"ბარი",
	"ტანსაცმლის მაღაზია",
	"აქსესუარების მაღაზია",
	"ტრანსპორტის ქირაობა"
};
new const s_GpsNearBizType[GPS_NEAR_KINDS] = {0, 7, 1, 2, 3, 6, 8, 4};

new bool:g_GpsOn[MAX_PLAYERS];
new g_GpsCat[MAX_PLAYERS];
new Float:g_GpsTarget[MAX_PLAYERS][3];
new g_GpsLabel[MAX_PLAYERS][96];

Gps_OnPlayerDisconnect(playerid)
{
	g_GpsOn[playerid] = false;
	g_GpsCat[playerid] = 0;
	g_GpsLabel[playerid][0] = 0;
}

Gps_Stop(playerid, bool:arrived)
{
	if (!g_GpsOn[playerid])
	{
		return 0;
	}
	g_GpsOn[playerid] = false;
	DisablePlayerRaceCheckpoint(playerid);
	if (arrived)
	{
		SendClientMessage(playerid, 0x66DD66FF, "[GPS] მიხვედი: %s.", g_GpsLabel[playerid]);
	}
	else
	{
		SendClientMessage(playerid, 0xAAAAAAFF, "[GPS] ნავიგაცია გამორთულია.");
	}
	return 1;
}

Gps_Start(playerid, Float:x, Float:y, Float:z, const label[])
{
	if (g_GpsOn[playerid])
	{
		DisablePlayerRaceCheckpoint(playerid);
	}
	g_GpsOn[playerid] = true;
	g_GpsTarget[playerid][0] = x;
	g_GpsTarget[playerid][1] = y;
	g_GpsTarget[playerid][2] = z;
	g_GpsLabel[playerid][0] = 0;
	strcat(g_GpsLabel[playerid], label, 96);
	SetPlayerRaceCheckpoint(playerid, CP_TYPE_GROUND_FINISH, x, y, z, x, y, z, GPS_RADIUS);
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	new dist = floatround(VectorSize(x - px, y - py, 0.0));
	SendClientMessage(playerid, 0x99CCFFFF, "[GPS] %s - %d მ. წითელი წერტილი რადარზე. გამორთვა: /gps off", label, dist);
	return 1;
}

// la propria casa (prima casa posseduta dal personaggio)
Gps_OwnHouse(playerid)
{
	new charId = gPlayer[playerid][pCharId];
	if (charId <= 0)
	{
		return -1;
	}
	for (new i = 0; i < g_HouseCount; i++)
	{
		if (g_HouseRows[i][hrOwnerId] == charId)
		{
			return i;
		}
	}
	return -1;
}

Gps_ShowMain(playerid)
{
	new body[640];
	format(body, sizeof body, "{FFFFFF}მნიშვნელოვანი ადგილები\n{FFFFFF}სერვისები და სამსახური\n{FFFFFF}უახლოესი...\n{FFD24D}ჩემი სახლი\n%s",
		g_GpsOn[playerid] ? "{FF6666}GPS-ის გამორთვა" : "{888888}GPS-ის გამორთვა");
	Lang_ShowPlayerDialog(playerid, GPS_DLG_MAIN, DIALOG_STYLE_LIST, "GPS ნავიგატორი", body, "არჩევა", "დახურვა");
	return 1;
}

Gps_ShowList(playerid, cat)
{
	new body[2048], line[160], title[64];
	g_GpsCat[playerid] = cat;
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	switch (cat)
	{
		case GPS_CAT_PLACES:
		{
			strcat(title, "მნიშვნელოვანი ადგილები", sizeof title);
			for (new i = 0; i < AZ_GPS && i < GPS_MAX_ITEMS; i++)
			{
				format(line, sizeof line, "{FFFFFF}%s\t{AAAAAA}%d მ\n", s_AzGpsName[i], floatround(VectorSize(s_AzGpsPos[i][0] - px, s_AzGpsPos[i][1] - py, 0.0)));
				strcat(body, line, sizeof body);
			}
		}
		case GPS_CAT_SERVICES:
		{
			strcat(title, "სერვისები და სამსახური", sizeof title);
			for (new i = 0; i < GPS_SERVICES; i++)
			{
				format(line, sizeof line, "{FFFFFF}%s\t{AAAAAA}%d მ\n", s_GpsSvcName[i], floatround(VectorSize(s_GpsSvcPos[i][0] - px, s_GpsSvcPos[i][1] - py, 0.0)));
				strcat(body, line, sizeof body);
			}
		}
		default:
		{
			strcat(title, "უახლოესი", sizeof title);
			for (new i = 0; i < GPS_NEAR_KINDS; i++)
			{
				format(line, sizeof line, "{FFFFFF}%s\n", s_GpsNearName[i]);
				strcat(body, line, sizeof body);
			}
		}
	}
	Lang_ShowPlayerDialog(playerid, GPS_DLG_LIST, cat == GPS_CAT_NEAR ? DIALOG_STYLE_LIST : DIALOG_STYLE_TABLIST, title, body, "წასვლა", "უკან");
	return 1;
}

// il punto piu' vicino del tipo richiesto; false se non ce n'e' nessuno
bool:Gps_Nearest(playerid, kind, &Float:x, &Float:y, &Float:z)
{
	new Float:px, Float:py, Float:pz, Float:best = 999999.0, Float:d;
	GetPlayerPos(playerid, px, py, pz);
	new bool:found = false;
	if (kind == 0)
	{
		for (new i = 0; i < sizeof s_BankPoints; i++)
		{
			d = VectorSize(s_BankPoints[i][0] - px, s_BankPoints[i][1] - py, 0.0);
			if (d < best)
			{
				best = d; x = s_BankPoints[i][0]; y = s_BankPoints[i][1]; z = s_BankPoints[i][2]; found = true;
			}
		}
		return found;
	}
	if (kind == 1)
	{
		for (new i = 0; i < sizeof g_VehStations; i++)
		{
			d = VectorSize(g_VehStations[i][0] - px, g_VehStations[i][1] - py, 0.0);
			if (d < best)
			{
				best = d; x = g_VehStations[i][0]; y = g_VehStations[i][1]; z = g_VehStations[i][2]; found = true;
			}
		}
	}
	new type = s_GpsNearBizType[kind];
	for (new i = 0; i < AZ_BIZ; i++)
	{
		if (s_AzBizType[i] != type)
		{
			continue;
		}
		d = VectorSize(s_AzBizPos[i][0] - px, s_AzBizPos[i][1] - py, 0.0);
		if (d < best)
		{
			best = d; x = s_AzBizPos[i][0]; y = s_AzBizPos[i][1]; z = s_AzBizPos[i][2]; found = true;
		}
	}
	return found;
}

Gps_OnDialogResponse(playerid, dialogid, response, listitem)
{
	if (dialogid == GPS_DLG_MAIN)
	{
		if (!response)
		{
			return 1;
		}
		switch (listitem)
		{
			case 0, 1, 2:
			{
				Gps_ShowList(playerid, listitem);
			}
			case 3:
			{
				new h = Gps_OwnHouse(playerid);
				if (h < 0)
				{
					SendClientMessage(playerid, -10270721, "[GPS] სახლი არ გაქვს. თავისუფალ სახლს შესასვლელთან მწვანე ნიშანი აქვს: /casa.");
					return 1;
				}
				Gps_Start(playerid, g_HouseRows[h][hrExtX], g_HouseRows[h][hrExtY], g_HouseRows[h][hrExtZ], "ჩემი სახლი");
			}
			default:
			{
				if (!Gps_Stop(playerid, false))
				{
					SendClientMessage(playerid, 0xAAAAAAFF, "[GPS] ნავიგაცია უკვე გამორთულია.");
				}
			}
		}
		return 1;
	}
	if (dialogid == GPS_DLG_LIST)
	{
		if (!response)
		{
			Gps_ShowMain(playerid);
			return 1;
		}
		switch (g_GpsCat[playerid])
		{
			case GPS_CAT_PLACES:
			{
				if (listitem >= 0 && listitem < AZ_GPS)
				{
					Gps_Start(playerid, s_AzGpsPos[listitem][0], s_AzGpsPos[listitem][1], s_AzGpsPos[listitem][2], s_AzGpsName[listitem]);
				}
			}
			case GPS_CAT_SERVICES:
			{
				if (listitem >= 0 && listitem < GPS_SERVICES)
				{
					Gps_Start(playerid, s_GpsSvcPos[listitem][0], s_GpsSvcPos[listitem][1], s_GpsSvcPos[listitem][2], s_GpsSvcName[listitem]);
				}
			}
			default:
			{
				new Float:x, Float:y, Float:z;
				if (listitem >= 0 && listitem < GPS_NEAR_KINDS && Gps_Nearest(playerid, listitem, x, y, z))
				{
					Gps_Start(playerid, x, y, z, s_GpsNearName[listitem]);
				}
				else
				{
					SendClientMessage(playerid, -10270721, "[GPS] ასეთი ადგილი ახლოს ვერ მოიძებნა.");
				}
			}
		}
		return 1;
	}
	return 0;
}

Gps_OnEnterRaceCheckpoint(playerid)
{
	if (!g_GpsOn[playerid])
	{
		return 0;
	}
	Gps_Stop(playerid, true);
	return 1;
}

public Cmd_gps(playerid, const params[])
{
	if (!Util_IsPlaying(playerid))
	{
		return 1;
	}
	if (params[0] != 0 && (!strcmp(params, "off", true, 0x7FFFFFFF) || !strcmp(params, "0", true, 0x7FFFFFFF)))
	{
		if (!Gps_Stop(playerid, false))
		{
			SendClientMessage(playerid, 0xAAAAAAFF, "[GPS] ნავიგაცია უკვე გამორთულია.");
		}
		return 1;
	}
	Gps_ShowMain(playerid);
	return 1;
}
