
Char_GetSpawnPoint(index, &Float:x, &Float:y, &Float:z, &Float:a)
{
	switch (index)
	{
		case 1:
		{
			x = 1481.0;
			y = -1749.5;
			z = 15.45;
			a = 0.0;
		}
		case 2:
		{
			x = 1967.0;
			y = Float:0xC4960000;
			z = 25.6;
			a = Float:0x43340000;
		}
		case 3:
		{
			x = 369.0;
			y = Float:0xC4FDC000;
			z = 7.67;
			a = Float:0x42B40000;
		}
		default:
		{
			x = 1742.6;
			y = Float:0xC4E88000;
			z = 13.5;
			a = 0.0;
		}
	}
}

Char_PickSpawn(playerid)
{
	new Float:x, Float:y, Float:z, Float:a;
	Char_GetSpawnPoint(random(4), x, y, z, a);
	gPlayer[playerid][pPosX] = x;
	gPlayer[playerid][pPosY] = y;
	gPlayer[playerid][pPosZ] = z;
	gPlayer[playerid][pAngle] = a;
	gPlayer[playerid][pInterior] = 0;
	gPlayer[playerid][pWorld] = 0;
}

Char_ApplySpawnInfo(playerid)
{
	SetSpawnInfo(playerid, 255, gPlayer[playerid][pSkin], gPlayer[playerid][pPosX], gPlayer[playerid][pPosY], gPlayer[playerid][pPosZ], gPlayer[playerid][pAngle], 0, 0, 0, 0, 0, 0);
	SetPlayerInterior(playerid, gPlayer[playerid][pInterior]);
	SetPlayerVirtualWorld(playerid, gPlayer[playerid][pWorld]);
}

Char_Spawn(playerid)
{
	gPlayer[playerid][pAuthState] = 8;
	Char_ApplySpawnInfo(playerid);
	SpawnPlayer(playerid);
}

Char_SetLoginCamera(playerid)
{
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerCameraPos(playerid, (1481.0 + Float:0x42700000), (-1749.5 + Float:0x42480000), (15.45 + 25.0));
	SetPlayerCameraLookAt(playerid, 1481.0, -1749.5, 15.45, 2);
}
