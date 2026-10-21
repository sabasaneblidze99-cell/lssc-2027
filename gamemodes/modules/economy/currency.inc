// =============================================================================
// economy/currency.inc - lari georgiano (GEL, ₾, ლარი; 1 ლარი = 100 თეთრი).
//
// TUTTI gli importi del gamemode e del database sono interi in TETRI (mai float):
//   ₾5,50 = 550.  Limite di un saldo: ECON_MONEY_MAX = 1.000.000.000 tetri = ₾10.000.000 (sotto il massimo int32 anche
//   sommando contanti + banca). Il database usa INT: stessi limiti.
// Formato: "₾1.250,50" (punto delle migliaia, virgola dei decimali); senza decimali quando sono zero: "₾85.000".
// Nell'interfaccia SA-MP (font senza ₾) il simbolo diventa "GEL" (core/lang.inc).
// =============================================================================

#define ECON_TETRI          (100)            // tetri in un lari
#define ECON_MONEY_MAX      (1000000000)     // ₾10.000.000: contanti e banca, ciascuno

// numero senza simbolo: 125050 -> "1.250,50", 8500000 -> "85.000"
Money_Format(tetri, dest[], size)
{
	new s[16], len, bool:neg = tetri < 0, lari, cents;
	if (neg)
	{
		tetri = tetri == cellmin ? cellmax : -tetri;
	}
	lari = tetri / ECON_TETRI;
	cents = tetri % ECON_TETRI;
	valstr(s, lari, false);
	len = strlen(s);
	dest[0] = 0;
	if (neg)
	{
		strcat(dest, "-", size);
	}
	new ch[2];
	for (new i = 0; i < len; i++)
	{
		if (i > 0 && (len - i) % 3 == 0)
		{
			strcat(dest, ".", size);
		}
		ch[0] = s[i];
		ch[1] = 0;
		strcat(dest, ch, size);
	}
	if (cents != 0)
	{
		new c[4];
		format(c, sizeof c, ",%02d", cents);
		strcat(dest, c, size);
	}
	return 1;
}

// con simbolo: "₾1.250,50", "-₾45,50"
Money_FormatSym(tetri, dest[], size)
{
	new n[24];
	Money_Format(tetri < 0 ? (tetri == cellmin ? cellmax : -tetri) : tetri, n, sizeof n);
	dest[0] = 0;
	if (tetri < 0)
	{
		strcat(dest, "-", size);
	}
	strcat(dest, "₾", size);
	strcat(dest, n, size);
	return 1;
}

// da usare negli argomenti di format: Money_Str(prezzo) -> "₾85.000"
Money_Str(tetri)
{
	new s[32];
	Money_FormatSym(tetri, s, sizeof s);
	return s;
}

// lari digitati dal giocatore -> tetri. Accetta "12", "12,5", "12.50" (al massimo 2 decimali, niente separatore delle migliaia).
bool:Money_Parse(const str[], &tetri)
{
	new i = 0, lari = 0, cents = 0, dec = -1, digits = 0;
	tetri = 0;
	while (str[i] == 32)
	{
		i++;
	}
	if (str[i] == 0)
	{
		return false;
	}
	for (; str[i] != 0 && str[i] != 32; i++)
	{
		new c = str[i];
		if (c == 44 || c == 46)
		{
			if (dec >= 0 || digits == 0)
			{
				return false;
			}
			dec = 0;
			continue;
		}
		if (c < 48 || c > 57)
		{
			return false;
		}
		if (dec >= 0)
		{
			if (dec >= 2)
			{
				return false;
			}
			cents = cents * 10 + (c - 48);
			dec++;
			continue;
		}
		if (lari > (ECON_MONEY_MAX / ECON_TETRI))
		{
			return false;
		}
		lari = lari * 10 + (c - 48);
		digits++;
	}
	if (digits == 0 || dec == 0)
	{
		return false;
	}
	if (dec == 1)
	{
		cents *= 10;
	}
	if (lari > ECON_MONEY_MAX / ECON_TETRI)
	{
		return false;
	}
	tetri = lari * ECON_TETRI + cents;
	return tetri <= ECON_MONEY_MAX;
}

// importo per il contatore di GTA (solo visualizzazione, lari interi): il saldo vero resta quello del server
Money_GtaDisplay(tetri)
{
	return tetri / ECON_TETRI;
}

// percentuale intera di un importo senza overflow (importi fino a ECON_MONEY_MAX, pct 0..1000)
Money_Pct(tetri, pct)
{
	return (tetri / 100) * pct + ((tetri % 100) * pct) / 100;
}
