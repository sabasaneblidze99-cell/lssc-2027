// =============================================================================
// cef/cef_safe.inc - ogni testo mandato alla pagina CEF e' UTF-8 valido.
//
// Il componente samp-cef converte le stringhe in stringhe Rust: un testo UTF-8 rotto (per esempio georgiano
// tagliato a meta' da un buffer troppo piccolo o da un dato del DB) fa chiudere TUTTO il server.
// Qui cef_emit_event e' ridefinito: prima di inoltrare, ogni argomento stringa viene ripulito sul posto
// (sequenze non valide -> '?', ultima lettera tagliata -> tolta). Le stringhe gia' valide non cambiano.
// =============================================================================

#if defined LSRP_CEF && !defined CEF_FAKE

native Cef_RawEmit(player_id, const event[], {CEF_ValueType, Float, _}:...) = cef_emit_event;

stock Cef_SafeEmit(player_id, const event[], {CEF_ValueType, Float, _}:...)
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
	#emit SYSREQ.C Cef_RawEmit
	cnt += 4;
	#emit LCTRL 4
	#emit LOAD.S.alt cnt
	#emit ADD
	#emit SCTRL 4
	return 1;
}

#define cef_emit_event Cef_SafeEmit

#endif
