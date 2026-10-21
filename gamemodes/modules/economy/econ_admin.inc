// =============================================================================
// economy/econ_admin.inc - /economia (admin livello 2+): massa monetaria e flussi delle ultime 24 ore.
// Sola lettura: legge characters (contanti + banca) e bank_ledger (entrate/uscite per tipo). I movimenti solo in
// contanti (negozi, /pay, carburante) non passano dal registro bancario: il pannello lo dice.
// I valori economici si cambiano in economy/econ_config.inc (e /ldset per la vita in citta').
// =============================================================================

#define ECON_DLG_STATS (9910)

forward Econ_OnSupply(playerid, const name[]);
forward Econ_OnFlows(playerid, const name[], const supply[]);

public Cmd_economia(playerid, const params[])
{
	#pragma unused params
	if (Util_GetAdmin(playerid) < 2)
	{
		return 0;
	}
	if (!DB_IsReady())
	{
		Util_Send(playerid, -10270721, "ბაზა მიუწვდომელია.");
		return 1;
	}
	new name[75];
	GetPlayerName(playerid, name, 75);
	// importi in tetri -> lari formattati dal database (BIGINT: nessun overflow di Pawn)
	mysql_tquery(g_DB, "SELECT COUNT(*) AS n, FORMAT(COALESCE(SUM(money), 0) / 100, 2, 'de_DE') AS cash, FORMAT(COALESCE(SUM(bank), 0) / 100, 2, 'de_DE') AS bank, FORMAT(COALESCE(SUM(money + bank), 0) / 100, 2, 'de_DE') AS total, FORMAT(COALESCE(AVG(money + bank), 0) / 100, 2, 'de_DE') AS avg FROM characters", "Econ_OnSupply", "s", name);
	return 1;
}

public Econ_OnSupply(playerid, const name[])
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	new n = 0, cash[32], bank[32], total[32], avg[32], supply[512];
	if (cache_num_rows() > 0)
	{
		cache_get_value_name_int(0, "n", n);
		cache_get_value_name(0, "cash", cash, 32);
		cache_get_value_name(0, "bank", bank, 32);
		cache_get_value_name(0, "total", total, 32);
		cache_get_value_name(0, "avg", avg, 32);
	}
	format(supply, sizeof supply, "{FFD24D}ფულის მასა{FFFFFF}\nპერსონაჟები: %d\nნაღდი: ₾%s\nბანკი: ₾%s\nსულ: ₾%s\nსაშუალოდ: ₾%s\n\n", n, cash, bank, total, avg);
	new q[512];
	mysql_format(g_DB, q, sizeof q, "SELECT kind, FORMAT(SUM(GREATEST(delta, 0)) / 100, 2, 'de_DE') AS inn, FORMAT(SUM(GREATEST(-delta, 0)) / 100, 2, 'de_DE') AS outt, COUNT(*) AS c FROM bank_ledger WHERE ts > NOW() - INTERVAL 24 HOUR GROUP BY kind ORDER BY SUM(ABS(delta)) DESC LIMIT 12");
	mysql_tquery(g_DB, q, "Econ_OnFlows", "ss", name, supply);
	return 1;
}

public Econ_OnFlows(playerid, const name[], const supply[])
{
	if (!DB_PlayerMatches(playerid, name))
	{
		return 1;
	}
	new body[2048], line[160], kind[16], inn[32], outt[32], c = 0;
	strcat(body, supply, sizeof body);
	strcat(body, "{FFD24D}ბანკი, ბოლო 24 საათი (ტიპი: შემოსავალი / გასავალი){FFFFFF}\n", sizeof body);
	new rows = cache_num_rows();
	for (new i = 0; i < rows; i++)
	{
		cache_get_value_name(i, "kind", kind, sizeof kind);
		cache_get_value_name(i, "inn", inn, sizeof inn);
		cache_get_value_name(i, "outt", outt, sizeof outt);
		cache_get_value_name_int(i, "c", c);
		format(line, sizeof line, "%s: {66DD66}+₾%s{FFFFFF} / {FF6666}-₾%s{FFFFFF} (%d)\n", kind, inn, outt, c);
		strcat(body, line, sizeof body);
	}
	if (rows == 0)
	{
		strcat(body, "{AAAAAA}ოპერაციები არ არის.\n", sizeof body);
	}
	strcat(body, "\n{AAAAAA}მხოლოდ ნაღდი ფულით (მაღაზია, /pay, საწვავი) აქ არ ჩანს. ფასები: economy/econ_config.inc", sizeof body);
	Lang_ShowPlayerDialog(playerid, ECON_DLG_STATS, DIALOG_STYLE_MSGBOX, "ეკონომიკა (GEL)", body, "დახურვა", "");
	return 1;
}
