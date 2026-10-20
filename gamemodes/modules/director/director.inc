new g_DirBoard[24][E_DIR_BOARD] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
new g_DirBoardCount;
new bool:g_DirBoardLoaded;
new g_DirExpAt[1000];
new g_DirRetry[1000];
new g_DirSeq[1000];
new g_DirRelAfter[1000];
new g_DirRelReason[1000];
new bool:g_DirRelSilent[1000];
new g_DirCompInteg[1000];
new g_DirCompSec[1000];
new g_DirCompOp[1000][41] = {
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0}
};
new g_DirCompCt[1000];
new g_DirLockCt[1000];
new g_DirLockAt[1000];

Director_WarehousePos(districtId, &Float:x, &Float:y)
{
	switch (districtId)
	{
		case 1:
		{
			x = Float:0x44EB0000;
			y = Float:0xC4DE8000;
		}
		case 2:
		{
			x = Float:0x45160000;
			y = Float:0xC4D98000;
		}
		case 3:
		{
			x = 2145.0;
			y = -1215.0;
		}
		default:
		{
			x = 0.0;
			y = 0.0;
		}
	}
}

Director_ShopPos(districtId, &Float:x, &Float:y)
{
	switch (districtId)
	{
		case 1:
		{
			x = 1927.0;
			y = Float:0xC4DFC000;
		}
		case 2:
		{
			x = Float:0x451BA000;
			y = Float:0xC4D34000;
		}
		case 3:
		{
			x = Float:0x4508E000;
			y = -1155.0;
		}
		default:
		{
			x = 0.0;
			y = 0.0;
		}
	}
}

Director_RouteMeters(fromD, toD)
{
	new Float:ax, Float:ay, Float:bx, Float:by;
	Director_WarehousePos(fromD, ax, ay);
	Director_ShopPos(toD, bx, by);
	new Float:dx = (Float:(bx) - Float:(ax)), Float:dy = (Float:(by) - Float:(ay));
	return floatround(floatsqroot((Float:((Float:(dx) * Float:(dx))) + Float:((Float:(dy) * Float:(dy))))), 1);
}

Director_BoardFind(contractId)
{
	for (new i = 0; i < g_DirBoardCount; i++)
	{
		if (g_DirBoard[i][DIR_BOARD_0] == contractId)
		{
			return i;
		}
	}
	return -1;
}

Director_BestLocal(playerid, fromD, toD)
{
	new a = LD_GetLocalRep(playerid, fromD), b = LD_GetLocalRep(playerid, toD);
	return a > b ? a : b;
}

bool:Director_PremiumOk(playerid, fromD, toD)
{
	return Director_BestLocal(playerid, fromD, toD) >= 20 || LD_GetProPoints(playerid, 1) >= 100;
}

bool:Director_TickBefore(untilTick)
{
	return untilTick != 0 && GetTickCount() - untilTick < 0;
}

Director_BlockCode(playerid, i)
{
	if (i < 0 || i >= g_DirBoardCount)
	{
		return 10;
	}
	if (gPlayer[playerid][PLAYER_DATA_90] < g_DirBoard[i][DIR_BOARD_7])
	{
		return 1;
	}
	if (LD_GetProPoints(playerid, 1) < g_DirBoard[i][DIR_BOARD_8])
	{
		return 2;
	}
	if (g_DirBoard[i][DIR_BOARD_10] == 0 && Director_BestLocal(playerid, g_DirBoard[i][DIR_BOARD_1], g_DirBoard[i][DIR_BOARD_2]) < g_DirBoard[i][DIR_BOARD_9])
	{
		return 3;
	}
	if (gPlayer[playerid][PLAYER_DATA_347] != 0 || gPlayer[playerid][PLAYER_DATA_360] != 0)
	{
		return 4;
	}
	if (LD_ActiveFlow(playerid) != 0)
	{
		return 5;
	}
	if (Director_TickBefore(gPlayer[playerid][PLAYER_DATA_359]))
	{
		return 6;
	}
	if (DB_LivingPlayerGet(playerid, 2) >= 8 || DB_LivingPlayerGet(playerid, 1) >= 2800)
	{
		return 7;
	}
	if (g_DirLockCt[playerid] == g_DirBoard[i][DIR_BOARD_0] && gettime() - g_DirLockAt[playerid] < 600)
	{
		return 8;
	}
	if (g_DirBoard[i][DIR_BOARD_6] <= gettime())
	{
		return 9;
	}
	return 0;
}

bool:Director_BoardVisible(playerid, i)
{
	if (i < 0 || i >= g_DirBoardCount || g_DirBoard[i][DIR_BOARD_6] <= gettime())
	{
		return 0;
	}
	if (g_DirBoard[i][DIR_BOARD_10] != 0 && !Director_PremiumOk(playerid, g_DirBoard[i][DIR_BOARD_1], g_DirBoard[i][DIR_BOARD_2]))
	{
		return 0;
	}
	return 1;
}

Director_ClearContract(playerid)
{
	gPlayer[playerid][PLAYER_DATA_347] = 0;
	gPlayer[playerid][PLAYER_DATA_348] = 0;
	gPlayer[playerid][PLAYER_DATA_350] = 0;
	gPlayer[playerid][PLAYER_DATA_351] = 0;
	gPlayer[playerid][PLAYER_DATA_352] = 0;
	gPlayer[playerid][PLAYER_DATA_353] = 0;
	gPlayer[playerid][PLAYER_DATA_354] = 0;
	gPlayer[playerid][PLAYER_DATA_355] = 0;
	gPlayer[playerid][PLAYER_DATA_356] = 0;
	gPlayer[playerid][PLAYER_DATA_361] = 0;
	gPlayer[playerid][PLAYER_DATA_362][0] = 0;
	g_DirExpAt[playerid] = 0;
	g_DirRetry[playerid] = 0;
	g_DirRelAfter[playerid] = 0;
	g_DirRelReason[playerid] = 0;
	g_DirRelSilent[playerid] = 0;
	g_DirCompOp[playerid][0] = 0;
	g_DirCompCt[playerid] = 0;
	g_DirSeq[playerid]++;
}

Director_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= 1000)
	{
		return 0;
	}
	Director_ClearContract(playerid);
	g_DirLockCt[playerid] = 0;
	g_DirLockAt[playerid] = 0;
	g_DirCompInteg[playerid] = 0;
	g_DirCompSec[playerid] = 0;
}

Director_SetOp(playerid, pend, tag[], ref)
{
	gPlayer[playerid][PLAYER_DATA_361] = pend;
	gPlayer[playerid][PLAYER_DATA_362][0] = 0;
	DB_LivingMakeOpId(gPlayer[playerid][PLAYER_DATA_362], 41, tag, ref, gPlayer[playerid][PLAYER_DATA_3]);
	g_DirRetry[playerid] = 0;
}

Director_Send(playerid)
{
	new ct = gPlayer[playerid][PLAYER_DATA_347];
	switch (gPlayer[playerid][PLAYER_DATA_361])
	{
		case 1:
		{
			DB_LivingCtAccept(playerid, ct, gPlayer[playerid][PLAYER_DATA_362]);
		}
		case 2:
		{
			DB_LivingCtPickup(playerid, ct, gPlayer[playerid][PLAYER_DATA_362]);
		}
		case 3:
		{
			DB_LivingCtComplete(playerid, ct, gPlayer[playerid][PLAYER_DATA_362], g_DirCompInteg[playerid], g_DirCompSec[playerid]);
		}
		case 4:
		{
			new r = g_DirRelReason[playerid] == 1 || g_DirRelReason[playerid] == 4 ? 1 : 0;
			DB_LivingCtRelease(playerid, ct, gPlayer[playerid][PLAYER_DATA_362], r);
		}
	}
}

bool:Director_ScheduleRetry(playerid)
{
	if (g_DirRetry[playerid] >= 3)
	{
		return 0;
	}
	g_DirRetry[playerid]++;
	SetTimerEx("Director_OnRetry", 2500, 0, "ddd", playerid, gPlayer[playerid][PLAYER_DATA_3], g_DirSeq[playerid]);
	return 1;
}

public Director_OnRetry(playerid, charId, seq)
{
	if (!IsPlayerConnected(playerid) || charId <= 0 || gPlayer[playerid][PLAYER_DATA_3] != charId || g_DirSeq[playerid] != seq)
	{
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_361] == 0 || gPlayer[playerid][PLAYER_DATA_347] == 0)
	{
		return 1;
	}
	Director_Send(playerid);
	return 1;
}

Director_RunDeferred(playerid)
{
	new r = g_DirRelAfter[playerid];
	g_DirRelAfter[playerid] = 0;
	if (r != 0 && gPlayer[playerid][PLAYER_DATA_347] != 0 && gPlayer[playerid][PLAYER_DATA_361] == 0)
	{
		Director_Release(playerid, r);
	}
}

Director_EndLocal(playerid, freightReason, bool:released, relReason)
{
	new ct = gPlayer[playerid][PLAYER_DATA_347], charId = gPlayer[playerid][PLAYER_DATA_3], fromD = gPlayer[playerid][PLAYER_DATA_350];
	Freight_End(playerid, freightReason);
	Director_ClearContract(playerid);
	if (released && ct > 0)
	{
		g_DirLockCt[playerid] = ct;
		g_DirLockAt[playerid] = gettime();
		LD_Emit(103, fromD, charId, relReason, "-");
	}
	DB_LivingBoard(65535);
}

Director_OnInit()
{
	g_DirBoardCount = 0;
	g_DirBoardLoaded = 0;
	for (new p = 0; p < 1000; p++)
	{
		Director_ResetPlayer(p);
	}
	LD_Listen("Director_OnDistrictEvent", 10);
}

Director_OnExit()
{
	for (new p = 0; p < 1000; p++)
	{
		Director_ClearContract(p);
	}
	g_DirBoardCount = 0;
}

Director_OnPlayerConnect(playerid)
{
	Director_ResetPlayer(playerid);
}

Director_OnCharReady(playerid)
{
	if (!g_DirBoardLoaded || g_DirBoardCount < 4)
	{
		Director_OnCycle();
	}
}

Director_OnPlayerDisconnect(playerid, reason)
{
	new ct = gPlayer[playerid][PLAYER_DATA_347];
	if (ct != 0 && gPlayer[playerid][PLAYER_DATA_361] == 0 && gPlayer[playerid][PLAYER_DATA_348] != 0)
	{
		new op[41];
		DB_LivingMakeOpId(op, 41, "cr", ct, gPlayer[playerid][PLAYER_DATA_3]);
		DB_LivingCtRelease(playerid, ct, op, 0);
	}
	Director_ResetPlayer(playerid);
}

public Director_OnDistrictEvent(type, districtId, charId, value, opId[])
{
	if (type == 9 || type == 8)
	{
		DB_LivingBoard(65535);
	}
	return 1;
}

Director_PlayersInGame()
{
	new n = 0;
	for (new i = 0; i < 1000; i++)
	{
		if (Util_IsPlaying(i) && gPlayer[i][PLAYER_DATA_338] != 0)
		{
			n++;
		}
	}
	return n;
}

Director_Generate(bucket)
{
	new slots = DB_LivingBucketSlots();
	for (new s = 0; s < slots; s++)
	{
		new fromD = 0, toD = 0, cargo = 0;
		if (!LD_ContractSlot(bucket, s, fromD, toD, cargo))
		{
			continue;
		}
		new meters = Director_RouteMeters(fromD, toD);
		if (meters < 300)
		{
			continue;
		}
		new bool:premium = 0;
		if (s >= 3)
		{
			new h = LD_Hash32(bucket, s, 2);
			premium = LD_HashMod(h >>> 24, 2) == 0;
		}
		DB_LivingGenerate(bucket, s, fromD, toD, fromD, toD, cargo, meters, 2, 0, 0, premium ? 1 : 0);
	}
}

public Director_OnCycle()
{
	if (!LD_IsReady())
	{
		return 1;
	}
	new now = gettime();
	for (new p = 0; p < 1000; p++)
	{
		if (!IsPlayerConnected(p) || gPlayer[p][PLAYER_DATA_347] == 0 || gPlayer[p][PLAYER_DATA_361] != 0)
		{
			continue;
		}
		if (gPlayer[p][PLAYER_DATA_348] != 1 && gPlayer[p][PLAYER_DATA_348] != 2)
		{
			continue;
		}
		if (g_DirExpAt[p] > 0 && now >= g_DirExpAt[p])
		{
			Director_Release(p, 5);
		}
	}
	if (Director_PlayersInGame() > 0)
	{
		Director_Generate(LD_Bucket(now));
	}
	DB_LivingBoard(65535);
	return 1;
}

public Living_OnDBBoard(playerid, result, count)
{
	if (playerid != 65535 || result != 1)
	{
		return 1;
	}
	new n = 0;
	for (new i = 0; i < count && n < 24; i++)
	{
		if (g_LdBoard[i][LD_BOARD_13] != 0 && g_LdBoard[i][LD_BOARD_13] != 7)
		{
			continue;
		}
		g_DirBoard[n][DIR_BOARD_0] = g_LdBoard[i][LD_BOARD_0];
		g_DirBoard[n][DIR_BOARD_1] = g_LdBoard[i][LD_BOARD_1];
		g_DirBoard[n][DIR_BOARD_2] = g_LdBoard[i][LD_BOARD_2];
		g_DirBoard[n][DIR_BOARD_3] = g_LdBoard[i][LD_BOARD_5];
		g_DirBoard[n][DIR_BOARD_4] = g_LdBoard[i][LD_BOARD_6];
		g_DirBoard[n][DIR_BOARD_5] = g_LdBoard[i][LD_BOARD_7];
		g_DirBoard[n][DIR_BOARD_6] = g_LdBoard[i][LD_BOARD_8];
		g_DirBoard[n][DIR_BOARD_7] = g_LdBoard[i][LD_BOARD_9];
		g_DirBoard[n][DIR_BOARD_8] = g_LdBoard[i][LD_BOARD_10];
		g_DirBoard[n][DIR_BOARD_9] = g_LdBoard[i][LD_BOARD_11];
		g_DirBoard[n][DIR_BOARD_10] = g_LdBoard[i][LD_BOARD_12];
		n++;
	}
	g_DirBoardCount = n;
	g_DirBoardLoaded = 1;
	LD_Emit(104, 0, 0, n, "-");
	return 1;
}

Director_BoardCount()
{
	return g_DirBoardCount;
}

bool:Director_BoardGet(index, &contractId, &fromD, &toD, &cargo, &reward, &distanceM, &expiresAt, &minLevel, &minRep, &minLocalRep, &premium)
{
	if (index < 0 || index >= g_DirBoardCount)
	{
		contractId = 0;
		fromD = 0;
		toD = 0;
		cargo = 0;
		reward = 0;
		distanceM = 0;
		expiresAt = 0;
		minLevel = 0;
		minRep = 0;
		minLocalRep = 0;
		premium = 0;
		return 0;
	}
	contractId = g_DirBoard[index][DIR_BOARD_0];
	fromD = g_DirBoard[index][DIR_BOARD_1];
	toD = g_DirBoard[index][DIR_BOARD_2];
	cargo = g_DirBoard[index][DIR_BOARD_3];
	reward = g_DirBoard[index][DIR_BOARD_5];
	distanceM = g_DirBoard[index][DIR_BOARD_4];
	expiresAt = g_DirBoard[index][DIR_BOARD_6];
	minLevel = g_DirBoard[index][DIR_BOARD_7];
	minRep = g_DirBoard[index][DIR_BOARD_8];
	minLocalRep = g_DirBoard[index][DIR_BOARD_9];
	premium = g_DirBoard[index][DIR_BOARD_10];
	return 1;
}

Director_ExpiresAt(playerid)
{
	if (playerid < 0 || playerid >= 1000)
	{
		return 0;
	}
	return g_DirExpAt[playerid];
}

Director_BlockMsg(playerid, code)
{
	switch (code)
	{
		case 1:
		{
			Util_Send(playerid, -10270721, "Serve un livello piu' alto per questo contratto.");
		}
		case 2:
		{
			Util_Send(playerid, -10270721, "La tua reputazione nei trasporti non basta ancora per questo contratto.");
		}
		case 3:
		{
			Util_Send(playerid, -10270721, "Il quartiere non ti conosce abbastanza per questo contratto.");
		}
		case 4:
		{
			Util_Send(playerid, -10270721, "Hai gia' un contratto attivo (/contratto).");
		}
		case 5:
		{
			LD_SendBlock(playerid, 4);
		}
		case 6:
		{
			Util_Send(playerid, -10270721, "Prenditi un attimo: potrai accettare un nuovo contratto tra poco.");
		}
		case 7:
		{
			Util_Send(playerid, -10270721, "Per quest'ora hai lavorato abbastanza: riprova piu' tardi.");
		}
		case 8:
		{
			Util_Send(playerid, -10270721, "Hai lasciato questo contratto da poco: scegline un altro.");
		}
		default:
		{
			Util_Send(playerid, -10270721, "Questo contratto non e' piu' disponibile.");
		}
	}
}

bool:Director_Accept(playerid, contractId)
{
	if (!Util_IsPlaying(playerid))
	{
		return 0;
	}
	if (!LD_IsReady() || gPlayer[playerid][PLAYER_DATA_338] == 0)
	{
		LD_SendBlock(playerid, 1);
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_361] != 0 || DB_LivingBusy(playerid, 0))
	{
		Util_Send(playerid, -10270721, "Un'operazione sul contratto e' gia' in corso: attendi.");
		return 0;
	}
	if (Hub_NearBoard(playerid) < 0)
	{
		Util_Send(playerid, -10270721, "I contratti si accettano a una bacheca: Casa del Quartiere di Idlewood, magazzino di Ganton o di Jefferson.");
		return 0;
	}
	new block = LD_CheckStart(playerid);
	if (block != 0)
	{
		LD_SendBlock(playerid, block);
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_90] < 2)
	{
		Director_BlockMsg(playerid, 1);
		return 0;
	}
	new i = Director_BoardFind(contractId);
	if (i < 0 || !Director_BoardVisible(playerid, i))
	{
		Director_BlockMsg(playerid, 9);
		return 0;
	}
	new code = Director_BlockCode(playerid, i);
	if (code != 0)
	{
		Director_BlockMsg(playerid, code);
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_347] = contractId;
	gPlayer[playerid][PLAYER_DATA_348] = 0;
	gPlayer[playerid][PLAYER_DATA_350] = g_DirBoard[i][DIR_BOARD_1];
	gPlayer[playerid][PLAYER_DATA_351] = g_DirBoard[i][DIR_BOARD_2];
	gPlayer[playerid][PLAYER_DATA_352] = g_DirBoard[i][DIR_BOARD_3];
	gPlayer[playerid][PLAYER_DATA_353] = g_DirBoard[i][DIR_BOARD_4];
	gPlayer[playerid][PLAYER_DATA_354] = g_DirBoard[i][DIR_BOARD_5];
	g_DirExpAt[playerid] = g_DirBoard[i][DIR_BOARD_6];
	Director_SetOp(playerid, 1, "ca", contractId);
	Director_Send(playerid);
	return 1;
}

bool:Director_Pickup(playerid)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_347] == 0 || gPlayer[playerid][PLAYER_DATA_348] != 1)
	{
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_361] != 0)
	{
		return 0;
	}
	Director_SetOp(playerid, 2, "cp", gPlayer[playerid][PLAYER_DATA_347]);
	Director_Send(playerid);
	return 1;
}

bool:Director_Complete(playerid, integrityPct, seconds)
{
	new ct = gPlayer[playerid][PLAYER_DATA_347];
	if (!IsPlayerConnected(playerid) || ct == 0 || gPlayer[playerid][PLAYER_DATA_348] != 2)
	{
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_361] != 0)
	{
		return 0;
	}
	g_DirCompInteg[playerid] = LD_Clamp(integrityPct, 0, 100);
	g_DirCompSec[playerid] = seconds > 0 ? seconds : 0;
	gPlayer[playerid][PLAYER_DATA_361] = 3;
	g_DirRetry[playerid] = 0;
	if (g_DirCompCt[playerid] == ct && g_DirCompOp[playerid][0] != 0)
	{
		gPlayer[playerid][PLAYER_DATA_362][0] = 0;
		strcat(gPlayer[playerid][PLAYER_DATA_362], g_DirCompOp[playerid], 41);
	}
	else
	{
		DB_LivingMakeOpId(gPlayer[playerid][PLAYER_DATA_362], 41, "cc", ct, gPlayer[playerid][PLAYER_DATA_3]);
		g_DirCompCt[playerid] = ct;
		g_DirCompOp[playerid][0] = 0;
		strcat(g_DirCompOp[playerid], gPlayer[playerid][PLAYER_DATA_362], 41);
	}
	Director_Send(playerid);
	return 1;
}

bool:Director_Release(playerid, reason)
{
	if (!IsPlayerConnected(playerid) || gPlayer[playerid][PLAYER_DATA_347] == 0)
	{
		return 0;
	}
	if (gPlayer[playerid][PLAYER_DATA_361] == 4)
	{
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_361] != 0)
	{
		g_DirRelAfter[playerid] = reason;
		return 1;
	}
	Freight_End(playerid, reason);
	g_DirRelReason[playerid] = reason;
	Director_SetOp(playerid, 4, "cr", gPlayer[playerid][PLAYER_DATA_347]);
	Director_Send(playerid);
	return 1;
}

Director_OnAccept(playerid, result, contractId, v2)
{
	new charId = gPlayer[playerid][PLAYER_DATA_3], msg[160];
	if (result == 1)
	{
		gPlayer[playerid][PLAYER_DATA_361] = 0;
		gPlayer[playerid][PLAYER_DATA_348] = 1;
		gPlayer[playerid][PLAYER_DATA_355] = GetTickCount();
		gPlayer[playerid][PLAYER_DATA_359] = GetTickCount() + 20000;
		if (v2 > 0)
		{
			g_DirExpAt[playerid] = v2;
		}
		if (!Freight_Begin(playerid, contractId, gPlayer[playerid][PLAYER_DATA_350], gPlayer[playerid][PLAYER_DATA_351], gPlayer[playerid][PLAYER_DATA_352], gPlayer[playerid][PLAYER_DATA_353], 0))
		{
			Util_Send(playerid, -10270721, "Il magazzino non ha un mezzo disponibile adesso: contratto rimesso in bacheca senza penalita'.");
			Director_Release(playerid, 6);
			return 0;
		}
		LD_Emit(101, gPlayer[playerid][PLAYER_DATA_350], charId, contractId, gPlayer[playerid][PLAYER_DATA_362]);
		DB_LivingBoard(65535);
		Director_RunDeferred(playerid);
		return 0;
	}
	if (result == -1)
	{
		if (Director_ScheduleRetry(playerid))
		{
			return 0;
		}
		g_DirRelSilent[playerid] = 1;
		g_DirRelReason[playerid] = 6;
		Director_SetOp(playerid, 4, "cr", contractId);
		Director_Send(playerid);
		Util_Send(playerid, -10270721, "Il servizio contratti non risponde: riprova tra poco.");
		return 0;
	}
	Director_ClearContract(playerid);
	switch (result)
	{
		case 2:
		{
			Util_Send(playerid, -10270721, "Hai gia' chiuso questo contratto.");
		}
		case 4:
		{
			Util_Send(playerid, -10270721, "Qualcun altro ha appena preso questo contratto.");
		}
		case 5:
		{
			Util_Send(playerid, -10270721, "Questo contratto e' scaduto.");
		}
		case 6:
		{
			Director_BlockMsg(playerid, 7);
		}
		case 7:
		{
			Util_Send(playerid, -10270721, "Non soddisfi ancora i requisiti di questo contratto (livello, reputazione o attesa tra contratti).");
		}
		case 10:
		{
			Util_Send(playerid, -10270721, "Un'operazione e' gia' in corso: attendi.");
		}
		default:
		{
			format(msg, 160, "Contratto non accettato (esito %d).", result);
			Util_Send(playerid, -10270721, msg);
		}
	}
	DB_LivingBoard(65535);
}

Director_AfterFreight(playerid, contractId, bool:terminal, relReason)
{
	if (gPlayer[playerid][PLAYER_DATA_347] != contractId || gPlayer[playerid][PLAYER_DATA_361] != 0)
	{
		return 0;
	}
	if (terminal)
	{
		Director_Release(playerid, relReason);
	}
	else
	{
		Director_RunDeferred(playerid);
	}
}

Director_OnPickup(playerid, result)
{
	new ct = gPlayer[playerid][PLAYER_DATA_347];
	if (result == -1 && Director_ScheduleRetry(playerid))
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_361] = 0;
	if (result == 1 || result == 2)
	{
		gPlayer[playerid][PLAYER_DATA_348] = 2;
		if (gPlayer[playerid][PLAYER_DATA_356] == 0)
		{
			gPlayer[playerid][PLAYER_DATA_356] = GetTickCount();
		}
		LD_Emit(102, gPlayer[playerid][PLAYER_DATA_350], gPlayer[playerid][PLAYER_DATA_3], ct, gPlayer[playerid][PLAYER_DATA_362]);
		Freight_OnPickupResult(playerid, 1);
		Director_AfterFreight(playerid, ct, 0, 0);
		return 0;
	}
	new bool:terminal = result != -1 && result != 10;
	Freight_OnPickupResult(playerid, result);
	Director_AfterFreight(playerid, ct, terminal, 5);
}

Director_OnComplete(playerid, result, amount, balance, v1, v2)
{
	new ct = gPlayer[playerid][PLAYER_DATA_347], toD = gPlayer[playerid][PLAYER_DATA_351], charId = gPlayer[playerid][PLAYER_DATA_3];
	if (result == -1 && Director_ScheduleRetry(playerid))
	{
		return 0;
	}
	gPlayer[playerid][PLAYER_DATA_361] = 0;
	new bool:paid = result == 1 || result == 6;
	if (result == -1 || result == 10 || result == 4)
	{
		if (result == 4)
		{
			g_DirCompOp[playerid][0] = 0;
			g_DirCompCt[playerid] = 0;
		}
		Freight_OnCompleteResult(playerid, result, 0, 0);
		Director_AfterFreight(playerid, ct, 0, 0);
		return 0;
	}
	new op[41];
	strcat(op, gPlayer[playerid][PLAYER_DATA_362], 41);
	if (paid || result == 8)
	{
		LD_SetProPoints(playerid, 1, v1);
	}
	if (paid && v2 > 0)
	{
		District_SetCache(toD, v2, -1, -1);
	}
	if (paid || result == 8 || result == 2)
	{
		Freight_OnCompleteResult(playerid, result, paid ? amount : 0, balance);
		Director_EndLocal(playerid, 0, 0, 0);
		gPlayer[playerid][PLAYER_DATA_359] = GetTickCount() + 90000;
		if (paid)
		{
			District_Emit(1, toD, charId, amount, op);
		}
		else
		{
			if (result == 8)
			{
				District_Emit(2, toD, charId, ct, op);
			}
		}
		District_ReloadPlayer(playerid);
		return 0;
	}
	g_DirCompOp[playerid][0] = 0;
	g_DirCompCt[playerid] = 0;
	Freight_OnCompleteResult(playerid, result, 0, balance);
	Director_AfterFreight(playerid, ct, 1, 5);
}

Director_OnReleased(playerid, result, v2)
{
	if (result == -1 && Director_ScheduleRetry(playerid))
	{
		return 0;
	}
	if (result == 1 && v2 >= 0)
	{
		LD_SetProPoints(playerid, 1, v2);
	}
	new reason = g_DirRelReason[playerid], bool:silent = g_DirRelSilent[playerid];
	if (result == -1)
	{
		Log_Write("DIRECTOR", "rilascio contratto senza esito dal DB: lo completa la pulizia al prossimo login/boot");
	}
	if (!silent && result == 1)
	{
		if (reason == 1 || reason == 4)
		{
			Util_Send(playerid, -10270721, "Hai lasciato il contratto: nessuna perdita di denaro, ma la tua reputazione nei trasporti ne risente.");
		}
		else
		{
			if (reason != 5)
			{
				Util_Send(playerid, -1, "Contratto interrotto senza penalita': e' tornato in bacheca.");
			}
		}
	}
	Director_EndLocal(playerid, reason, 1, reason);
}

public Living_OnDBContract(playerid, action, result, contractId, amount, balance, v1, v2, v3)
{
	if (playerid < 0 || playerid >= 1000 || !IsPlayerConnected(playerid))
	{
		return 1;
	}
	if (gPlayer[playerid][PLAYER_DATA_347] == 0 || (contractId != 0 && contractId != gPlayer[playerid][PLAYER_DATA_347]))
	{
		if (result != 10)
		{
			new msg[160];
			format(msg, 160, "esito contratto fuori sequenza: azione %d contratto %d esito %d", action, contractId, result);
			Log_Write("DIRECTOR", msg);
		}
		return 1;
	}
	switch (action)
	{
		case 1:
		{
			if (gPlayer[playerid][PLAYER_DATA_361] == 1)
			{
				Director_OnAccept(playerid, result, gPlayer[playerid][PLAYER_DATA_347], v2);
			}
		}
		case 2:
		{
			if (gPlayer[playerid][PLAYER_DATA_361] == 2)
			{
				Director_OnPickup(playerid, result);
			}
		}
		case 4:
		{
			if (gPlayer[playerid][PLAYER_DATA_361] == 3)
			{
				Director_OnComplete(playerid, result, amount, balance, v1, v2);
			}
		}
		case 3:
		{
			if (gPlayer[playerid][PLAYER_DATA_361] == 4)
			{
				Director_OnReleased(playerid, result, v2);
			}
		}
	}
	return 1;
}
