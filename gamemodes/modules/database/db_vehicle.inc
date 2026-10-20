new g_VehRows[MAX_PLAYERS][4][E_VEH_ROW];
new g_VehNew[MAX_PLAYERS][E_VEH_NEW];

DB_VehStaleRelease(playerid, op)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	if (!IsPlayerConnected(playerid))
	{
		DB_PendingClear(playerid, op);
	}
}

DB_VehCompensate(kind, ownerId, vehDbId)
{
	if (!DB_IsReady() || ownerId <= 0 || vehDbId <= 0)
	{
		Log_Write("DBVEH", "compensazione veicolo NON eseguita (db non pronto o id non valido): verificare a mano");
		return 0;
	}
	new query[512];
	if (kind == 1)
	{
		mysql_format(g_DB, query, 512, "UPDATE vehicles SET deleted = 1 WHERE id = %d AND owner_id = %d AND deleted = 0", vehDbId, ownerId);
	}
	else
	{
		mysql_format(g_DB, query, 512, "UPDATE vehicles SET deleted = 0 WHERE id = %d AND owner_id = %d AND deleted = 1", vehDbId, ownerId);
	}
	mysql_tquery(g_DB, query, "DB_OnVehicleCompensated", "ddd", kind, ownerId, vehDbId);
}

public DB_OnVehicleCompensated(kind, ownerId, vehDbId)
{
	new msg[128];
	format(msg, 128, "%s veicolo %d (owner %d) a sessione scaduta: righe=%d%s", kind == 1 ? "rollback creazione" : "ripristino vendita", vehDbId, ownerId, cache_affected_rows(), cache_affected_rows() == 1 ? "" : " ANOMALIA: verificare a mano");
	Log_Write("DBVEH", msg);
	return 1;
}

bool:DB_VehSessionOk(playerid, name[], ownerId)
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 0;
	}
	return ownerId > 0 && gPlayer[playerid][pCharId] == ownerId;
}

DB_LoadVehicles(playerid, ownerId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || ownerId <= 0)
	{
		DB_DeliverFailure(playerid, 32);
		return 0;
	}
	new name[25], query[768];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 768, "SELECT id, model, plate, color1, color2, price_paid, pos_x, pos_y, pos_z, pos_a, park_x, park_y, park_z, park_a, interior, world, health, fuel_cl, locked, destroyed FROM vehicles WHERE owner_id = %d AND deleted = 0 ORDER BY id LIMIT %d", ownerId, 4);
	DB_PendingSet(playerid, 32);
	mysql_tquery(g_DB, query, "DB_OnVehiclesLoaded", "dsd", playerid, name, ownerId);
}

public DB_OnVehiclesLoaded(playerid, name[], ownerId)
{
	if (!DB_VehSessionOk(playerid, name, ownerId))
	{
		return 1;
	}
	DB_PendingClear(playerid, 32);
	new rows = 0, n = 0, tmp = 0;
	cache_get_row_count(rows);
	for (new i = 0; i < rows && n < 4; i++)
	{
		cache_get_value_name_int(i, "id", g_VehRows[playerid][n][vrId]);
		cache_get_value_name_int(i, "model", g_VehRows[playerid][n][vrModel]);
		cache_get_value_name(i, "plate", g_VehRows[playerid][n][vrPlate], 8);
		cache_get_value_name_int(i, "color1", g_VehRows[playerid][n][vrColor1]);
		cache_get_value_name_int(i, "color2", g_VehRows[playerid][n][vrColor2]);
		cache_get_value_name_int(i, "price_paid", g_VehRows[playerid][n][vrPricePaid]);
		cache_get_value_name_float(i, "pos_x", g_VehRows[playerid][n][vrPosX]);
		cache_get_value_name_float(i, "pos_y", g_VehRows[playerid][n][vrPosY]);
		cache_get_value_name_float(i, "pos_z", g_VehRows[playerid][n][vrPosZ]);
		cache_get_value_name_float(i, "pos_a", g_VehRows[playerid][n][vrPosA]);
		cache_get_value_name_float(i, "park_x", g_VehRows[playerid][n][vrParkX]);
		cache_get_value_name_float(i, "park_y", g_VehRows[playerid][n][vrParkY]);
		cache_get_value_name_float(i, "park_z", g_VehRows[playerid][n][vrParkZ]);
		cache_get_value_name_float(i, "park_a", g_VehRows[playerid][n][vrParkA]);
		cache_get_value_name_int(i, "interior", g_VehRows[playerid][n][vrInterior]);
		cache_get_value_name_int(i, "world", g_VehRows[playerid][n][vrWorld]);
		cache_get_value_name_float(i, "health", g_VehRows[playerid][n][vrHealth]);
		cache_get_value_name_int(i, "fuel_cl", g_VehRows[playerid][n][vrFuelCl]);
		cache_get_value_name_int(i, "locked", tmp);
		g_VehRows[playerid][n][vrLocked] = tmp != 0;
		cache_get_value_name_int(i, "destroyed", tmp);
		g_VehRows[playerid][n][vrDestroyed] = tmp != 0;
		n++;
	}
	if (funcidx("Veh_OnDBLoaded") != -1)
	{
		CallLocalFunction("Veh_OnDBLoaded", "dd", playerid, n);
	}
	return 1;
}

public DB_OnVehiclesCounted(playerid, name[], ownerId)
{
	if (!DB_VehSessionOk(playerid, name, ownerId))
	{
		return 1;
	}
	DB_PendingClear(playerid, 256);
	new cnt = 0;
	cache_get_value_name_int(0, "cnt", cnt);
	if (funcidx("Veh_OnDBCounted") != -1)
	{
		CallLocalFunction("Veh_OnDBCounted", "dd", playerid, cnt);
	}
	return 1;
}

DB_VehGeneratePlate(plate[], size)
{
	format(plate, size, "%d%c%c%c%03d", random(10), random(26) + 65, random(26) + 65, random(26) + 65, random(1000));
}

DB_VehInsertAttempt(playerid, name[], attempt)
{
	new plate[8], query[768];
	DB_VehGeneratePlate(plate, 8);
	mysql_format(g_DB, query, 768, "INSERT IGNORE INTO vehicles (owner_id, model, plate, color1, color2, price_paid, pos_x, pos_y, pos_z, pos_a, park_x, park_y, park_z, park_a, fuel_cl) VALUES (%d, %d, '%e', %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %d)", g_VehNew[playerid][vnOwnerId], g_VehNew[playerid][vnModel], plate, g_VehNew[playerid][vnColor1], g_VehNew[playerid][vnColor2], g_VehNew[playerid][vnPricePaid], g_VehNew[playerid][vnPosX], g_VehNew[playerid][vnPosY], g_VehNew[playerid][vnPosZ], g_VehNew[playerid][vnPosA], g_VehNew[playerid][vnPosX], g_VehNew[playerid][vnPosY], g_VehNew[playerid][vnPosZ], g_VehNew[playerid][vnPosA], g_VehNew[playerid][vnFuelCl]);
	mysql_tquery(g_DB, query, "DB_OnVehicleCreated", "dsdds", playerid, name, g_VehNew[playerid][vnOwnerId], attempt, plate);
}

DB_CreateVehicle(playerid, ownerId, model, color1, color2, pricePaid, Float:x, Float:y, Float:z, Float:a, fuelCl)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || ownerId <= 0 || model < 400 || model > 611 || g_DBPending[playerid] & 64)
	{
		if (g_DBPending[playerid] & 64)
		{
			if (funcidx("Veh_OnDBCreated") != -1)
			{
				CallLocalFunction("Veh_OnDBCreated", "dds", playerid, 0, "");
			}
			return 0;
		}
		DB_DeliverFailure(playerid, 64);
		return 0;
	}
	g_VehNew[playerid][vnOwnerId] = ownerId;
	g_VehNew[playerid][vnModel] = model;
	g_VehNew[playerid][vnColor1] = clamp(color1, 0, 126);
	g_VehNew[playerid][vnColor2] = clamp(color2, 0, 126);
	g_VehNew[playerid][vnPricePaid] = max(pricePaid, 0);
	g_VehNew[playerid][vnFuelCl] = max(fuelCl, 0);
	g_VehNew[playerid][vnPosX] = x;
	g_VehNew[playerid][vnPosY] = y;
	g_VehNew[playerid][vnPosZ] = z;
	g_VehNew[playerid][vnPosA] = a;
	new name[25];
	GetPlayerName(playerid, name, 25);
	DB_PendingSet(playerid, 64);
	DB_VehInsertAttempt(playerid, name, 1);
}

public DB_OnVehicleCreated(playerid, name[], ownerId, attempt, plate[])
{
	if (!DB_VehSessionOk(playerid, name, ownerId))
	{
		if (cache_affected_rows() == 1)
		{
			DB_VehCompensate(1, ownerId, cache_insert_id());
		}
		DB_VehStaleRelease(playerid, 64);
		return 1;
	}
	if (cache_affected_rows() == 1)
	{
		new vehDbId = cache_insert_id();
		DB_PendingClear(playerid, 64);
		if (funcidx("Veh_OnDBCreated") != -1)
		{
			CallLocalFunction("Veh_OnDBCreated", "dds", playerid, vehDbId, plate);
		}
		return 1;
	}
	if (attempt < 10)
	{
		DB_VehInsertAttempt(playerid, name, attempt + 1);
		return 1;
	}
	DB_PendingClear(playerid, 64);
	if (funcidx("Veh_OnDBCreated") != -1)
	{
		CallLocalFunction("Veh_OnDBCreated", "dds", playerid, 0, "");
	}
	return 1;
}

DB_DeleteVehicle(playerid, ownerId, vehDbId)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid))
	{
		return 0;
	}
	if (!DB_IsReady() || ownerId <= 0 || vehDbId <= 0)
	{
		DB_DeliverFailure(playerid, 128);
		return 0;
	}
	new name[25], query[512];
	GetPlayerName(playerid, name, 25);
	mysql_format(g_DB, query, 512, "UPDATE vehicles SET deleted = 1 WHERE id = %d AND owner_id = %d AND deleted = 0", vehDbId, ownerId);
	DB_PendingSet(playerid, 128);
	mysql_tquery(g_DB, query, "DB_OnVehicleDeleted", "dsdd", playerid, name, ownerId, vehDbId);
}

public DB_OnVehicleDeleted(playerid, name[], ownerId, vehDbId)
{
	if (!DB_VehSessionOk(playerid, name, ownerId))
	{
		if (cache_affected_rows() == 1)
		{
			DB_VehCompensate(2, ownerId, vehDbId);
		}
		DB_VehStaleRelease(playerid, 128);
		return 1;
	}
	DB_PendingClear(playerid, 128);
	if (funcidx("Veh_OnDBDeleted") != -1)
	{
		CallLocalFunction("Veh_OnDBDeleted", "ddd", playerid, vehDbId, cache_affected_rows());
	}
	return 1;
}

DB_SaveVehicle(vehDbId, Float:x, Float:y, Float:z, Float:a, interior, world, Float:health, fuelCl, bool:locked, bool:destroyed)
{
	if (!DB_IsReady() || vehDbId <= 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE vehicles SET pos_x = %f, pos_y = %f, pos_z = %f, pos_a = %f, interior = %d, world = %d, health = %f, fuel_cl = %d, locked = %d, destroyed = %d WHERE id = %d AND deleted = 0", x, y, z, a, clamp(interior, -32768, 32767), clamp(world, -32768, 32767), health, max(fuelCl, 0), locked ? 1 : 0, destroyed ? 1 : 0, vehDbId);
	mysql_tquery(g_DB, query, "", "");
}

DB_SetVehiclePark(vehDbId, ownerId, Float:x, Float:y, Float:z, Float:a)
{
	if (!DB_IsReady() || vehDbId <= 0 || ownerId <= 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE vehicles SET park_x = %f, park_y = %f, park_z = %f, park_a = %f WHERE id = %d AND owner_id = %d AND deleted = 0", x, y, z, a, vehDbId, ownerId);
	mysql_tquery(g_DB, query, "", "");
}

DB_RecoverVehicle(vehDbId, ownerId, Float:health, minFuelCl)
{
	if (!DB_IsReady() || vehDbId <= 0 || ownerId <= 0)
	{
		return 0;
	}
	new query[768];
	mysql_format(g_DB, query, 768, "UPDATE vehicles SET destroyed = 0, health = %f, fuel_cl = GREATEST(fuel_cl, %d), pos_x = park_x, pos_y = park_y, pos_z = park_z, pos_a = park_a WHERE id = %d AND owner_id = %d AND deleted = 0 AND destroyed = 1", health, max(minFuelCl, 0), vehDbId, ownerId);
	mysql_tquery(g_DB, query, "", "");
}
