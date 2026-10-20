
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
			y = -1200.0;
			z = 25.6;
			a = 180.0;
		}
		case 3:
		{
			x = 369.0;
			y = -2030.0;
			z = 7.67;
			a = 90.0;
		}
		default:
		{
			x = 1742.6;
			y = -1860.0;
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
	SetSpawnInfo(playerid, 255, gPlayer[playerid][pSkin], gPlayer[playerid][pPosX], gPlayer[playerid][pPosY], gPlayer[playerid][pPosZ], gPlayer[playerid][pAngle], WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
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
	SetPlayerCameraPos(playerid, (1481.0 + 60.0), (-1749.5 + 50.0), (15.45 + 25.0));
	SetPlayerCameraLookAt(playerid, 1481.0, -1749.5, 15.45, CAMERA_CUT);
}
