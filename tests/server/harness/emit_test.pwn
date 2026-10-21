#define FILTERSCRIPT
#include <open.mp>
stock T_SafeEmit(const player_id[], const event[], {Float, _}:...)
{
	#pragma unused event
	new n = numargs();
	new c, need, k, b, j;
	for (new a = 2; a + 1 < n; a += 2)
	{
		if (getarg(a) != 0)
		{
			continue;
		}
		// argomento stringa a + 1: si ripulisce sul posto
		j = 0;
		while ((c = getarg(a + 1, j) & 0xFF) != 0)
		{
			if (c < 0x80)
			{
				j++;
				continue;
			}
			if ((c & 0xE0) == 0xC0) need = 1;
			else if ((c & 0xF0) == 0xE0) need = 2;
			else if ((c & 0xF8) == 0xF0) need = 3;
			else
			{
				setarg(a + 1, j, '?');
				j++;
				continue;
			}
			for (k = 1; k <= need; k++)
			{
				b = getarg(a + 1, j + k) & 0xFF;
				if (b == 0 || (b & 0xC0) != 0x80)
				{
					break;
				}
			}
			if (k <= need)
			{
				if (b == 0)
				{
					setarg(a + 1, j, 0);    // lettera tagliata alla fine: si toglie
					break;
				}
				setarg(a + 1, j, '?');
				j++;
				continue;
			}
			j += need + 1;
		}
	}
	// inoltro alla nativa con gli stessi argomenti (stesso schema di Chat_SCM)
	new argStart, argEnd, i, cnt;
	#emit CONST.alt player_id
	#emit LCTRL 5
	#emit ADD
	#emit STOR.S.pri argStart
	argEnd = argStart + (n - 1) * 4;
	for (i = argEnd; i >= argStart; i -= 4)
	{
		#emit LREF.S.pri i
		#emit PUSH.pri
	}
	cnt = n * 4;
	#emit PUSH.S cnt
	#emit SYSREQ.C printf
	cnt += 4;
	#emit LCTRL 4
	#emit LOAD.S.alt cnt
	#emit ADD
	#emit SCTRL 4
	return 1;
}


public OnFilterScriptInit()
{
	new bad[16] = "ab";
	bad[2] = 0xE1; bad[3] = 0x83; bad[4] = 0;
	new mid[16] = "x";
	mid[1] = 0xE1; mid[2] = 0x41; mid[3] = 0x42; mid[4] = 0;
	new ka[32] = "\xE1;\x83;\xA1;ok";
	T_SafeEmit("[EMITTEST] %s|%d|%s|%d|%d|%d|%s|%d|%.2f|%d|%s|%d|%s", "evento", 0, "ciao", 1, 42, 0, bad, 2, 3.5, 0, mid, 0, ka);
	printf("[EMITTEST] bad len=%d mid=%s ka len=%d", strlen(bad), mid, strlen(ka));
	return 1;
}
