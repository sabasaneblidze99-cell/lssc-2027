// =============================================================================
// core/boot.inc - avvio a tappe.
//
// OnGameModeInit fa solo il minimo; il resto (connessione al database, moduli, 14.956 oggetti del mondo, veicoli)
// viene eseguito un pezzo alla volta da un timer: nessuna callback lunga (crashdetect "Long callback execution"
// segnala tutto cio' che supera 5 ms). Un giocatore che entra durante l'avvio (circa 2 secondi) aspetta il login
// finche' l'avvio non e' finito.
// =============================================================================

#define BOOT_TICK_MS        (20)

new g_BootStep;
new g_BootTimer;
new bool:g_BootDone;
new g_BootStartTick;
new bool:g_BootWaitLogin[MAX_PLAYERS];
new g_BootMaxMs, g_BootMaxStep;

forward Boot_OnTick();
forward Boot_OnLoginQueue();

Boot_Start()
{
	g_BootStep = 0;
	g_BootDone = false;
	g_BootStartTick = GetTickCount();
	g_BootTimer = SetTimer("Boot_OnTick", BOOT_TICK_MS, true);
}

stock bool:Boot_IsDone()
{
	return g_BootDone;
}

// login rimandato a fine avvio (chiamata da OnPlayerConnect)
bool:Boot_DeferLogin(playerid)
{
	if (g_BootDone)
	{
		return false;
	}
	g_BootWaitLogin[playerid] = true;
	return true;
}

Boot_OnPlayerDisconnect(playerid)
{
	g_BootWaitLogin[playerid] = false;
}

Boot_Finish()
{
	KillTimer(g_BootTimer);
	g_BootTimer = 0;
	g_BootDone = true;
	printf("[AVVIO] completato in %d ms (%d tappe, la piu' lunga: tappa %d, %d ms)", GetTickCount() - g_BootStartTick, g_BootStep, g_BootMaxStep, g_BootMaxMs);
	// chi e' entrato durante l'avvio: un login per tick, non tutti nella stessa callback
	SetTimer("Boot_OnLoginQueue", BOOT_TICK_MS, false);
}

public Boot_OnLoginQueue()
{
	for (new p = 0; p < MAX_PLAYERS; p++)
	{
		if (g_BootWaitLogin[p])
		{
			g_BootWaitLogin[p] = false;
			if (IsPlayerConnected(p))
			{
				Account_OnConnect(p);
			}
			SetTimer("Boot_OnLoginQueue", BOOT_TICK_MS, false);
			return 1;
		}
	}
	return 1;
}

// una tappa per tick, nello stesso ordine del vecchio OnGameModeInit
public Boot_OnTick()
{
	new step = g_BootStep++, t0 = GetTickCount();
	switch (step)
	{
		case 0:
		{
			DB_Connect();
		}
		case 1:
		{
			CefBridge_OnInit();
			Voice_OnInit();
			Char_OnInit();
			Economy_OnInit();
		}
		case 2:
		{
			Veh_OnInit();
		}
		case 3:
		{
			House_OnInit();
		}
		case 4:
		{
			Job_OnInit();
			Faction_OnInit();
		}
		case 5:
		{
			Inv_OnInit();
			Bank_OnInit();
		}
		case 6:
		{
			Living_OnInit();
		}
		case 7:
		{
			HudCef_OnInit();
			StreetRental_OnInit();
		}
		default:
		{
			// mondo Arizona: porte e business, poi il mapping a pezzi, poi i veicoli a pezzi
			if (!AzWorld_BootStep(step - 8))
			{
				Boot_Finish();
				return 1;
			}
		}
	}
	if (GetTickCount() - t0 > g_BootMaxMs)
	{
		g_BootMaxMs = GetTickCount() - t0;
		g_BootMaxStep = step;
	}
	return 1;
}
